#pragma once

#include <queue>
#include <thread>
#include <atomic>
#include <algorithm> // for push_heap

#include "common.hpp"
#include "vw_map.hpp"

enum Spec : uint8_t { IDLE, BUSY, DONE };

struct SNode{
    int g, h;
    VWState state;
    std::atomic<uint8_t> spec{IDLE};
    SNode* parent;
    vector<VWState> children; // not storing nodes since atomics can't copy

    SNode(int g, VWState s) : g(g), h(s.get_nearest_l2_norm()), state(s) {}

    bool operator<(const SNode& other) const {
        return f() < other.f();
    }
    bool operator>(const SNode& other) const {
        return f() > other.f();
    }

    bool operator==(const SNode& other) const {
        return children == other.children && g == other.g && h == other.h;
    }

    int f() const {
        return g+h;
    }
};

struct NodeCmp {
    bool operator()(const SNode* a, const SNode* b) const {
        int fa = a->g + a->h, fb = b->g + b->h;
        return fa > fb || (fa == fb && a->g < b->g);   // min-f on top, tie-break high g
    }
};

inline void s_generate_successors(const VWMap& m, SNode* n, size_t expansion_cost_us) {
    auto successors = n->state.generate_successors(m);
    spin_for(std::chrono::microseconds(expansion_cost_us));
    for(auto& s : successors) {
        n->children.push_back(s);
    }
}

inline vector<VWState> s_get_result(VWState initial, SNode* final) {
    vector<VWState> states;
    auto curr = final;
    while (curr->state != initial) {
        states.insert(states.begin(), curr->state);
        curr = curr->parent;
    }
    states.insert(states.begin(), initial);
    return states;
}

inline Result spastar(const VWMap& m, const VWState& initial, size_t expansion_cost_us, size_t threads, size_t board_k, Stats& stats){
    vector<SNode*> open;
    auto best_g = unordered_map<VWState, int>();
    
    std::deque<SNode> arena; // This contains every SNode value, everything else just uses pointers
    std::vector<std::atomic<SNode*>> board(board_k);   // all start as nullptr

    SNode* initial_node = &arena.emplace_back(0, initial);
    best_g[initial] = 0;
    
    // cerr << "Pushing initial node " << initial_node.state << "\n";
    open.push_back(initial_node);
    push_heap(open.begin(), open.end(), NodeCmp{});


    size_t board_pos = 0;
    board[board_pos] = initial_node;
    board_pos = ++board_pos&board_k;
    // spawn threads:

    vector<jthread> workers;
    for(size_t i = 0; i < threads; i++) {
        workers.push_back(jthread ([&](std::stop_token stoken) {
            while (!stoken.stop_requested()) {
                bool worked = 0;
                for (auto& slot : board) {
                    SNode* n = slot.load(std::memory_order_acquire);
                    uint8_t expected = IDLE;
                    if (n && n->spec.compare_exchange_strong(expected, BUSY)) {
                        s_generate_successors(m, n, expansion_cost_us);  // + spin
                        worked = 1;
                        n->spec.store(DONE, std::memory_order_release);
                        stats.speculations.fetch_add(1, std::memory_order_relaxed);
                    }
                }
                stats.empty_scans.fetch_add(worked, std::memory_order_relaxed);
            }
        }));
    }


    while(!open.empty()){
        pop_heap(open.begin(), open.end(), NodeCmp{});
        SNode* current = open.back();
        open.pop_back();

        if (best_g[current->state] < current->g) {
            continue;
        }

        stats.expanded ++;

        if(current->state.trash_positions.empty()) {
            // cerr << "Success! Calculating solution";
            for (auto& w : workers) {
                w.request_stop();
            }
            return s_get_result(initial_node->state, current);
        }


        uint8_t expected = IDLE;
        if (current->spec.compare_exchange_strong(expected, BUSY)) {
            stats.misses++;                 // nobody touched it, so main expands it
            s_generate_successors(m, current, expansion_cost_us);
            current->spec.store(DONE, std::memory_order_release);
        } else if (expected == DONE) {
            stats.hits++;                   // already done, free
        } else {
            stats.waits++;                  // BUSY: a speculator is midway, so wait
            while (current->spec.load(std::memory_order_acquire) != DONE) {}
        }
        
        for(const auto& s : current->children) {

            auto bg = best_g.find(s);
            stats.generated++;
            auto new_g = current->g+1;
            if (bg == best_g.end() || new_g < bg->second) {
                best_g[s] = new_g;
                // cerr << "Pushing child " << s.state << " to open list\n";
                
                SNode* s_node = &arena.emplace_back(new_g, s);
                open.push_back(s_node);
                push_heap(open.begin(), open.end(), NodeCmp{});

                s_node->parent = current;
                // cerr << "Publishing " << s_node << " to board\n";
                
                board[board_pos] = s_node;
                board_pos = ++board_pos&board_k;
            }
        }
        // set board to the approx top k of the heap
        // size_t n = min<size_t>(board_k, open.size());
        // copy(open.begin(), open.begin() + n, board.begin());
        // fill(board.begin() + n, board.end(), nullptr);   // clear leftover slots
    }
    return nullopt;
}
