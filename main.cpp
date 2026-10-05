#include <optional>
#include <queue>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "vw_map.hpp"

using namespace std;

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

optional<vector<VWState>> astar(VWMap m, VWState initial){
    // open set
    priority_queue<Node, vector<Node>, greater<Node>> open;
    auto from = unordered_map<VWState, VWState>(); // Map that resolves a certain vwstate to its parent
    auto best_g = unordered_map<VWState, int>();

    Node initial_node = Node(0, initial);
    
    cerr << "Pushing initial node " << initial_node.state << "\n";
    open.push(initial_node);

    while(!open.empty()){
        cerr << "Popping from open\n";
        Node current = open.top();
        open.pop();

        if(current.state.trash_positions.empty()) {
            cerr << "Success! Calculating solution";
            return get_result(from, initial_node.state, current.state);
        }

        if (best_g[current.state] < current.g) {
            // skip because another node had a better g with the same state.
            cerr << "Skipped since I hit a node with a better g value\n";
            continue;
        }

        // get neighbors
        generate_successors(m, current);
        for(auto s : current.children) {

            auto [it, succ] = from.insert({s.state, current.state}); // get old value in lookup map
            auto bg = best_g.find(s.state);
            if (bg == best_g.end() || s.g < bg->second) {
                best_g[s.state] = s.g;
                from.insert_or_assign(s.state, current.state);  // update the table's g
                cerr << "Pushing child " << s.state << " to open list\n";
                open.push(s);
            }
        }
    }
    return nullopt;
}



int main(int argc, char const *argv[])
{
    auto [m, s] = parse_map();
    auto res = astar(m, s);
    if(res){
        cout << res.value() << '\n';
    } else {
        cout << "Failed to find a solution\n";
    }
    return 0;
}