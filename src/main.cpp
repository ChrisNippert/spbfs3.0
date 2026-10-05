#include <optional>
#include <queue>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <iomanip> // Required for std::setprecision

#include "vw_map.hpp"
#include "timer.hpp"

using namespace std;

inline void spin_for(std::chrono::nanoseconds d) {
    auto end = std::chrono::steady_clock::now() + d;
    while (std::chrono::steady_clock::now() < end) { /* burn */ }
}

struct Node{
    int g, h;
    VWState state;
    vector<Node> children;

    Node(int g, VWState s) : g(g), h(s.get_nearest_l2_norm()), state(s) {}

    bool operator<(const Node& other) const {
        return f() < other.f();
    }
    bool operator>(const Node& other) const {
        return f() > other.f();
    }

    bool operator==(const Node& other) const {
        return children == other.children && g == other.g && h == other.h;
    }

    int f() const {
        return g+h;
    }
};

void generate_successors(const VWMap& m, Node& n) {
    auto successors = n.state.generate_successors(m);
    for(auto& s : successors) {
        n.children.push_back(Node(n.g+1, s));
    }
}

vector<VWState> get_result(unordered_map<VWState, VWState> map, VWState initial, VWState final) {
    vector<VWState> states;
    auto curr = final;
    while (curr != initial) {
        states.insert(states.begin(), curr);
        curr = map.at(curr); // should be a wel constructed map and will not fail
    }
    states.insert(states.begin(), initial);
    return states;
}

size_t nodes_generated = 0;
size_t nodes_expanded = 0;
size_t expansion_cost_us = 0;

optional<vector<VWState>> astar(VWMap m, VWState initial){
    // open set
    priority_queue<Node, vector<Node>, greater<Node>> open;
    auto from = unordered_map<VWState, VWState>(); // Map that resolves a certain vwstate to its parent
    auto best_g = unordered_map<VWState, int>();

    Node initial_node = Node(0, initial);
    best_g[initial] = 0;
    
    // cerr << "Pushing initial node " << initial_node.state << "\n";
    open.push(initial_node);

    while(!open.empty()){
        // cerr << "Popping from open\n";
        Node current = open.top();
        open.pop();

        if (best_g[current.state] < current.g) {
            // skip because another node had a better g with the same state.
            // cerr << "Skipped since I hit a node with a better g value\n";
            continue;
        }

        nodes_expanded ++;

        if(current.state.trash_positions.empty()) {
            // cerr << "Success! Calculating solution";
            return get_result(from, initial_node.state, current.state);
        }


        // get neighbors
        
        generate_successors(m, current);
        spin_for(std::chrono::microseconds(expansion_cost_us));
        for(const auto& s : current.children) {

            auto bg = best_g.find(s.state);
            nodes_generated++;
            if (bg == best_g.end() || s.g < bg->second) {
                best_g[s.state] = s.g;
                from.insert_or_assign(s.state, current.state);  // update the table's g
                // cerr << "Pushing child " << s.state << " to open list\n";
                open.push(s);
            }
        }
    }
    return nullopt;
}



int main(int argc, char* argv[])
{

    if(argc != 1) {
        expansion_cost_us = stoi(argv[1]);
    }
    cerr << "Additional expansion cost set to " << expansion_cost_us << "\n";

    auto [m, s] = parse_map();
    Timer t;
    auto res = astar(m, s);
    auto us = t.get_us();
    if(res){
        // cout << res.value() << '\n';
        // cout << "Path Length: " << res.value().size()-1 << "\n";
        // cout << nodes_generated << " nodes generated\n";
        // cout << nodes_expanded << " nodes expanded\n";
        // cout << nodes_expanded/us * 1000 << " expansions/ms\n";
        // cout << (nodes_expanded/us * 1000000) << " expansions/s\n";
        cout << expansion_cost_us << "," << (long)us << "," << nodes_expanded << "\n";
    } else {
        cout << "Failed to find a solution\n";
    }
    return 0;
}

