echo "cost,us,expanded" > serial.csv
for c in 0 1 2 5 10 20 50; do
  for i in 1 2 3; do
    ./bin/spbfs $c < maps/big_map.vw >> results/serial.csv
  done
done