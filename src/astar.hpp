#pragma once

#include <queue>

#include "common.hpp"

inline Result astar(const VWMap& m, const VWState& initial, size_t expansion_cost_us, size_t threads, size_t board_k, Stats& stats){
    // open set
    priority_queue<Node*, vector<Node*>, NodePtrCmp> open;
    auto best_g = unordered_map<VWState, int>();

    Node* initial_node = new Node(0, initial);
    best_g[initial] = 0;
    
    open.push(initial_node);
    while(!open.empty()){
        Node* current = open.top();
        open.pop();

        if (best_g[current->state] < current->g) continue;

        stats.expanded ++;

        if(current->state.trash_positions.empty()) 
            return get_result(initial_node->state, current);

        generate_successors(m, *current, expansion_cost_us);
        for(auto& s : current->children) {

            auto bg = best_g.find(s.state);
            stats.generated++;
            if (bg == best_g.end() || s.g < bg->second) {
                best_g[s.state] = s.g;
                s.parent = current;
                open.push(&s);
            }
        }
    }
    return nullopt;
}