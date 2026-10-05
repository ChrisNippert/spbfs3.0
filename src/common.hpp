#pragma once

#include <cstddef>
#include <chrono>
#include <vector>
#include <atomic>

#include "vw_map.hpp"

struct Node{
    int g, h;
    VWState state;
    vector<Node> children;
    Node* parent;

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

struct NodePtrCmp {
    bool operator()(const Node* a, const Node* b) const {
        int fa = a->f(), fb = b->f();
        return fa > fb || (fa == fb && a->g < b->g);
    }
};

inline void spin_for(std::chrono::nanoseconds d) {
    auto end = std::chrono::steady_clock::now() + d;
    while (std::chrono::steady_clock::now() < end) {}
}

inline void generate_successors(const VWMap& m, Node& n, size_t expansion_cost_us) {
    spin_for(std::chrono::microseconds(expansion_cost_us));
    auto successors = n.state.generate_successors(m);
    for(auto& s : successors) {
        n.children.push_back(Node(n.g+1, s));
    }
}


struct Stats {
    size_t expanded = 0, generated = 0;
    size_t hits = 0, waits = 0, misses = 0;   // spastar only
    alignas(64) atomic<size_t> speculations = 0, empty_scans = 0;
};
using Result = std::optional<std::vector<VWState>>;

inline vector<VWState> get_result(VWState initial, Node* final) {
    vector<VWState> states;
    auto curr = final;
    while (curr->state != initial) {
        states.insert(states.begin(), curr->state);
        curr = curr->parent;
    }
    states.insert(states.begin(), initial);
    return states;
}