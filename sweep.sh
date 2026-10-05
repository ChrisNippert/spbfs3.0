make || exit 1
echo "algo,cost,threads,board_k,us,expanded,generated,hits,waits,misses,speculations,empty-scans,path" > results/all.csv
for a in astar pastar spastar; do
  for c in 0 1 2 5 10 20; do
    for t in 1 2 4 8; do
      [ "$a" = astar ] && [ $t -ne 1 ] && continue   # serial only needs threads=1
      for k in 16 32 64 128; do
        [ "$a" = pastar ] || [ "$a" = astar ] && [ "$k" -ne 16 ] && continue   # pastar doesn't care about k
        echo "Running $a $c $t $k"
        for i in 1 2 3; do ./bin/spbfs $a $c $t $k < maps/big_map.vw >> results/all.csv; done
      done
    done
  done
done