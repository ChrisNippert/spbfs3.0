#include <iostream>
#include <chrono>

class Timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

public:
    Timer() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    ~Timer() {
    }

    
    double get_ms() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(start_time).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(end_time).time_since_epoch().count();
        
        auto duration = end - start;
        return duration * 0.001;
    }

    double get_us() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(start_time).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(end_time).time_since_epoch().count();
        
        return end - start;
    }

    void print() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(start_time).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(end_time).time_since_epoch().count();
        
        auto duration = end - start;
        double ms = duration * 0.001; // Convert microseconds to milliseconds

        std::cerr << "Elapsed time: " << duration << " µs (" << ms << " ms)\n";
    }
};