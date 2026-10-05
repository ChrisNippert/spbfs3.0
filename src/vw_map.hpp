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

ostream& operator<<(ostream& os, const Position& p) {
    return os << "(" << p.r << "," << p.c << ")";
}

enum Object : char{
    WALL = '#',
    ROBOT = '@',
    TRASH = '*'
};

struct VWMap {
    int rows, cols;
    unordered_set<Position> wall_positions;

public:
    bool is_valid_position(const Position& p) const {
        return (
                p.c < cols && p.c >= 0
            &&  p.r < rows && p.r >= 0
            &&  !wall_positions.contains(p)
        );
    }
};

class VWState{
public:
    Position robot_position;
    unordered_set<Position> trash_positions;

    /**
     * Returns euclidean distance to nearest trash position
     */
    int get_nearest_l2_norm() const {
        if(trash_positions.empty()) return 0;
        double nearest = numeric_limits<double>::max();
        for(const auto& t  : trash_positions) {
            double d = robot_position.euclidean(t);
            if ( d < nearest )  nearest = d;
        }
        return (int)nearest;
    }

    vector<VWState> generate_successors(const VWMap& map) const{
        // move robot up, down, left, right vacuum. Valid states are returned
        vector<VWState> valid_states;
        static constexpr Position DIRS[] = {{-1,0},{1,0},{0,-1},{0,1}};
        for (auto d : DIRS) {
            Position p{robot_position.r + d.r, robot_position.c + d.c};
            if (map.is_valid_position(p))
                valid_states.push_back(VWState{p, trash_positions});
        }
        if (trash_positions.contains(robot_position)) {
            auto new_set = trash_positions;
            new_set.erase(robot_position);
            valid_states.push_back(VWState{robot_position, std::move(new_set)});
        }
        return valid_states;
    }

    friend std::ostream& operator<<(std::ostream& os, const VWState& p);

    bool operator==(const VWState& other) const = default;
};

template<> struct std::hash<VWState> {
    size_t operator()(const VWState& s) const {
        size_t h = std::hash<Position>{}(s.robot_position);
        size_t t = 0;
        for (const auto& p : s.trash_positions)
            t += std::hash<Position>{}(p);   // order doesn't matter
        return h ^ (t * 0x9e3779b97f4a7c15ull);
    }
};

struct Problem {
    VWMap map;
    VWState start;
};

inline Problem parse_map(){
    using enum Object;
    // reads stdin for the file contents being piped in
    string input_str;
    int rows, cols;

    if (getline(cin, input_str)) 
        cols = stoi(input_str);
    else exit(1);
    if (getline(cin, input_str)) 
        rows = stoi(input_str);
    else exit(2);

    unordered_set<Position> walls;
    unordered_set<Position> trash;
    Position robot;

    for (int r = 0; r < rows; ++r) {
        getline(cin, input_str);
        int c = 0;
        for(auto ch : input_str) {
            if(ch == WALL) 
                walls.insert(Position{r,c});
            if(ch == ROBOT) 
                robot=Position{r,c};
            if(ch == TRASH) 
                trash.insert(Position{r,c});
            c++;
        }
    }

    return Problem{VWMap{rows, cols, walls}, VWState{robot, trash}};
}

inline std::ostream& operator<<(std::ostream& os, const VWState& p) {
    return os << "VWState(RobotPos: " << p.robot_position << ", Trash: " << p.trash_positions << ")";
}