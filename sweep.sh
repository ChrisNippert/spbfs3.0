#!/bin/bash
make || exit 1
mkdir -p results # Ensures the results directory exists

echo "algo,cost,threads,board_k,us,expanded,generated,hits,waits,misses,speculations,empty-scans,path" > results/all.csv

# 1. Determine which algorithms to run
# If no arguments are passed ($# == 0), default to all three.
# Otherwise, use the arguments provided by the user.
if [[ $# -eq 0 ]]; then
  algos=("astar" "pastar" "spastar")
else
  algos=("$@")
fi

costs=(0 1 2 5 10 20)
threads=(1 2 4 8)
ks=(16 32 64 128)

for a in "${algos[@]}"; do
  # Quick validation to catch typos in user arguments
  if [[ "$a" != "astar" && "$a" != "pastar" && "$a" != "spastar" ]]; then
    echo "Warning: Unknown algorithm '$a'. Skipping."
    continue
  fi

  for c in "${costs[@]}"; do
    for t in "${threads[@]}"; do
      # Rule 1: 'astar' is purely serial and only runs on 1 thread
      [[ "$a" == "astar" && $t -ne 1 ]] && continue
      
      for k in "${ks[@]}"; do
        # Rule 2: Only 'spastar' cares about k; skip other values of k for astar and pastar
        [[ ( "$a" == "astar" || "$a" == "pastar" ) && $k -ne 16 ]] && continue
        
        echo "Running $a c=$c t=$t k=$k"
        
        # Run 3 iterations for averaging
        for i in 1 2 3; do
          if [[ "$a" == "astar" ]]; then
            ./bin/spbfs astar "$c" < maps/100x100_t5 >> results/all.csv
          elif [[ "$a" == "pastar" ]]; then
            ./bin/spbfs pastar "$c" "$t" < maps/100x100_t5 >> results/all.csv
          elif [[ "$a" == "spastar" ]]; then
            ./bin/spbfs spastar "$c" "$t" "$k" < maps/100x100_t5 >> results/all.csv
          fi
        done

      done
    done
  done
done

echo "Done! Results saved to results/all.csv"
