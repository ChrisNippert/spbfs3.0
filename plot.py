import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results/serial.csv")
g = df.groupby("cost")["us"].agg(["mean", "std"]) / 1000  # → ms

plt.errorbar(g.index, g["mean"], yerr=g["std"], marker="o", capsize=3, label="serial A*")
plt.xlabel("added expansion cost (µs)")
plt.ylabel("search time (ms)")
plt.grid(alpha=0.3)
plt.legend()
plt.savefig("results/sweep.png", dpi=150)