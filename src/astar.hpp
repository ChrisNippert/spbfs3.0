#pragma once

#include <queue>

#include "common.hpp"

inline Result astar(const VWMap& m, const VWState& initial, size_t expansion_cost_us, size_t threads, size_t board_k, Stats& stats){
    // open set
    priority_queue<Node*, vector<Node*>, greater<Node*>> open;
    auto best_g = unordered_map<VWState, int>();

    Node* initial_node = new Node(0, initial);
    best_g[initial] = 0;
    
    // cerr << "Pushing initial node " << initial_node.state << "\n";
    open.push(initial_node);

    while(!open.empty()){
        // cerr << "Popping from open\n";
        Node* current = open.top();
        open.pop();

        if (best_g[current->state] < current->g) {
            // skip because another node had a better g with the same state.
            // cerr << "Skipped since I hit a node with a better g value\n";
            continue;
        }

        stats.expanded ++;

        if(current->state.trash_positions.empty()) {
            // cerr << "Success! Calculating solution";
            return get_result(initial_node->state, current);
        }


        // get neighbors
        
        generate_successors(m, *current);
        spin_for(std::chrono::microseconds(expansion_cost_us));
        for(auto& s : current->children) {

            auto bg = best_g.find(s.state);
            stats.generated++;
            if (bg == best_g.end() || s.g < bg->second) {
                best_g[s.state] = s.g;
                s.parent = current;
                // cerr << "Pushing child " << s.state << " to open list\n";
                open.push(&s);
            }
        }
    }
    return nullopt;
}