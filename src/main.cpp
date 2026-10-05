#include <atomic>
#include <iostream>

#include "vw_map.hpp"
#include "timer.hpp"
#include "astar.hpp"
#include "pastar.hpp"
#include "spastar.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    string algo = "astar";
    size_t expansion_cost_us = 0;
    size_t threads = 1;
    size_t board_k = 16;
    if (argc > 1) algo = argv[1];
    if (argc > 2) expansion_cost_us = stoul(argv[2]);
    if (argc > 3) threads = stoul(argv[3]);
    if (argc > 4) board_k = stoul(argv[4]);

    auto [m, s] = parse_map();

    Stats st;
    Timer t;
    Result res;
    if (algo == "astar")       res = astar(m, s, expansion_cost_us, threads, board_k, st);
    else if (algo == "pastar") res = pastar(m, s, expansion_cost_us, threads, board_k, st);
    else if (algo == "spastar") res = spastar(m, s, expansion_cost_us, threads - 1, board_k, st);
    else { cerr << "unknown algo " << algo << "\n"; return 1; }
    long us = t.get_us();

    long path = res ? res->size() - 1 : -1;
    cout << algo << "," << expansion_cost_us << "," << threads << "," << board_k << ","
         << us << "," << st.expanded << "," << st.generated << "," << st.hits << "," << st.waits << "," << st.misses << "," << st.speculations.load(memory_order::acquire) << "," << st.empty_scans.load(memory_order::acquire) << "," << path << "\n";
}