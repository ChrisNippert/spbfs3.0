#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <atomic>

#include "common.hpp"

inline Result pastar(const VWMap& m, const VWState& initial, size_t expansion_cost_us, size_t threads, size_t board_k, Stats& stats){
    mutex mut;

    priority_queue<Node*, vector<Node*>, NodePtrCmp> open;
    auto best_g = unordered_map<VWState, int>();
    best_g.reserve(1 << 20);

    atomic<int> best_solution = numeric_limits<int>::max();
    optional<Node*> final_node;
    atomic<size_t> active = 0;

    Node* initial_node = new Node(0, initial, m);
    best_g[initial] = 0;
    
    open.push(initial_node);

    { // own scope for auto jthread joining
        vector<jthread> workers;
        for(size_t i = 0; i < threads; i++) {
            workers.push_back(jthread ([&] {
                unique_lock lk(mut, defer_lock);
                while(true){
                    lk.lock();
                    bool nothing_left = open.empty() || open.top()->f() >= best_solution;

                    if (nothing_left && active == 0) { 
                        lk.unlock(); return; 
                    
                    }
                    if (nothing_left) {
                        lk.unlock();
                        this_thread::yield();
                        continue;
                    }  // others may still push work      
                                 
                    Node* current = open.top();
                    open.pop();

                    if (best_g.at(current->state) < current->g) {
                        lk.unlock();
                        this_thread::yield();
                        continue;
                    }

                    stats.expanded++;

                    if(current->state.is_goal()) { // success?
                        if(current->g < best_solution) {
                            best_solution = current->g;
                            final_node = current;
                        }
                        lk.unlock();
                        continue;
                    }
                    active++;
                    lk.unlock();

                    // get neighbors
                    generate_successors(m, *current, expansion_cost_us);
                    lk.lock();
                    for(auto& s : current->children) {

                        auto bg = best_g.find(s.state);
                        stats.generated++;
                        if (bg == best_g.end() || s.g < bg->second) {
                            best_g[s.state] = s.g;
                            s.parent = current; 
                            open.push(&s);
                        }
                    }
                    active --;
                    lk.unlock();
                }
            }));
        }
    }
    if(final_node.has_value()) {
        return get_result(initial_node->state, final_node.value());
    }
    return nullopt;
}