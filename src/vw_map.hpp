/**
 * vw_map.hpp
 * includes map for vacuum world
 */

#pragma once

#include <iostream>
#include <memory>
#include <cstddef>
#include <vector>
#include <cmath>
#include <unordered_set>
#include <limits>
#include <string>
#include <functional>
#include <tuple>

using namespace std;

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if (i != vec.size() - 1) {
            os << ", "; // Add a comma between elements
        }
    }
    os << "]";
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T>& set) {
    os << "[";
    for (const auto& s : set) {
        os << s << ", ";
    }
    os << "]";
    return os;
}

struct Position{
    int r, c;
    double euclidean(const Position& other) const {
        double dr = other.r - r;
        double dc = other.c - c;
        return std::hypot(dr, dc); 
    }

    friend ostream& operator<<(ostream& os, const Position& p);
    
    bool operator==(const Position&) const = default;  // inside Position
};

template<> struct std::hash<Position> {
    size_t operator()(const Position& p) const {
        return (size_t)p.r << 32 | (unsigned)p.c;
    }
};

inline ostream& operator<<(ostream& os, const Position& p) {
    return os << "(" << p.r << "," << p.c << ")";
}

enum Object : char{
    WALL = '#',
    ROBOT = '@',
    TRASH = '*'
};

struct VWMap {
    int rows, cols;
    vector<bool>     walls;       // r*cols + c -> is wall
    vector<Position> trash_pos;       // id -> position
    vector<int>      trash_id;        // r*cols + c -> id, or -1

    int idx(const Position& p) const { return p.r * cols + p.c; }

    bool is_valid_position(const Position& p) const {
        return p.r >= 0 && p.r < rows && p.c >= 0 && p.c < cols && !walls[idx(p)];
    }
};

class VWState{
public:
    Position robot_position;
    uint64_t trash; // bit i set = trash i still there

    bool is_goal() const { return trash == 0; }

    /**
     * Returns euclidean distance to nearest trash position
     */
    int get_nearest_l2_norm(const VWMap& map) const {
        if (trash == 0) return 0;
        int best = numeric_limits<int>::max();
        for (uint64_t m = trash; m; m &= m - 1) {
            const Position& t = map.trash_pos[std::countr_zero(m)];
            int dr = t.r - robot_position.r, dc = t.c - robot_position.c;
            best = std::min(best, dr*dr + dc*dc);
        }
        return (int)std::sqrt((double)best);
    }


    vector<VWState> generate_successors(const VWMap& map) const{
        // move robot up, down, left, right vacuum. Valid states are returned
        vector<VWState> out;
        out.reserve(5);
        static constexpr Position DIRS[] = {{-1,0},{1,0},{0,-1},{0,1}};
        for (auto d : DIRS) {
            Position p{robot_position.r + d.r, robot_position.c + d.c};
            if (map.is_valid_position(p))
                out.push_back(VWState{p, trash});
        }
        int id = map.trash_id[map.idx(robot_position)];
        if (id >= 0 && (trash >> id & 1))
            out.push_back(VWState{robot_position, trash & ~(1ULL << id)});
        return out;
    }

    friend std::ostream& operator<<(std::ostream& os, const VWState& p);

    bool operator==(const VWState& other) const = default;
};

template<> struct std::hash<VWState> {
    size_t operator()(const VWState& s) const {
        size_t h = std::hash<Position>{}(s.robot_position);
        return h ^ (s.trash * 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
    }
};

inline ostream& operator<<(ostream& os, const VWState& s) {
    os << "VWState(RobotPos: " << s.robot_position << ", Trash ids: [";
    for (uint64_t m = s.trash; m; m &= m - 1) os << std::countr_zero(m) << " ";
    return os << "])";
}

struct Problem {
    VWMap map;
    VWState start;
};

inline Problem parse_map(){
    using enum Object;
    string line;
    int rows, cols;
    if (getline(cin, line)) cols = stoi(line); else exit(1);
    if (getline(cin, line)) rows = stoi(line); else exit(2);

    VWMap map{rows, cols, vector<bool>(rows * cols, false), {}, vector<int>(rows * cols, -1)};
    Position robot{0, 0};
    uint64_t mask = 0;

    for (int r = 0; r < rows; ++r) {
        getline(cin, line);
        for (int c = 0; c < (int)line.size() && c < cols; ++c) {
            Position p{r, c};
            if (line[c] == WALL)  map.walls[map.idx(p)] = true;
            if (line[c] == ROBOT) robot = p;
            if (line[c] == TRASH) {
                int id = (int)map.trash_pos.size();
                if (id >= 64) { cerr << "more than 64 trash not supported\n"; exit(3); }
                map.trash_pos.push_back(p);
                map.trash_id[map.idx(p)] = id;
                mask |= 1ULL << id;
            }
        }
    }

    return Problem{std::move(map), VWState{robot, mask}};
}