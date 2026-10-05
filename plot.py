import pandas as pd
import matplotlib.pyplot as plt

# algo,cost,threads,us,expanded,generated,hits,waits,misses,speculations,path
df = pd.read_csv("results/all.csv")
df["ms"] = df["us"] / 1000

DEFAULT_K = 16
main = df[(df["algo"] != "spastar") | (df["board_k"] == DEFAULT_K)]

colors = {1: "tab:blue", 2: "tab:orange", 4: "tab:green", 8: "tab:red"}
styles = {"pastar": "-", "spastar": "--"}

def style_ax(ax, title, ylabel=None, ylim=None):
    ax.set_title(title)
    ax.set_xlabel("added expansion cost (µs)")
    ax.set_xscale("symlog", linthresh=1)
    ax.set_xlim(left=0)
    if ylabel: ax.set_ylabel(ylabel)
    if ylim:   ax.set_ylim(*ylim)
    ax.grid(alpha=0.3)

def plot_per_thread(ax, data, col):
    for t, g in data.groupby("threads"):
        ax.plot(g["cost"], g[col], "--o", color=colors.get(t), label=f"{t} thr")

# ---------- sanity checks ----------
print("path lengths seen:", sorted(df["path"].unique()))   # want a single value

sp = df[df["algo"] == "spastar"].copy()
bad = sp[sp["speculations"] < sp["hits"] + sp["waits"]]
print("invariant violations (speculations < hits + waits):", len(bad))

# ---------- 1. time + speedup ----------
avg = df.groupby(["algo", "threads", "cost"])["ms"].mean().reset_index()
serial = avg[avg["algo"] == "astar"].set_index("cost")["ms"]

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
for (algo, t), g in avg.groupby(["algo", "threads"]):
    if algo == "astar":
        ax1.plot(g["cost"], g["ms"], "k:o", label="serial A*")
        continue
    kw = dict(color=colors.get(t), linestyle=styles.get(algo, "-"), marker="o",
              label=f"{algo}, {t} thr")
    ax1.plot(g["cost"], g["ms"], **kw)
    ax2.plot(g["cost"], serial.loc[g["cost"]].values / g["ms"].values, **kw)

style_ax(ax1, "Search time", "ms")
ax1.set_yscale("log")
style_ax(ax2, "Speedup vs serial", "speedup")
ax2.axhline(1, color="k", linestyle="--")
ax1.legend(); ax2.legend()
fig.tight_layout()
fig.savefig("results/plot.png", dpi=150)

# ---------- 2. pop outcomes ----------
pops = sp["hits"] + sp["waits"] + sp["misses"]
for col in ["hits", "waits", "misses"]:
    sp[col + "_frac"] = sp[col] / pops
fr = sp.groupby(["threads", "cost"])[["hits_frac", "waits_frac", "misses_frac"]].mean().reset_index()

fig2, axes = plt.subplots(1, 3, figsize=(15, 4), sharey=True)
for ax, col in zip(axes, ["hits_frac", "waits_frac", "misses_frac"]):
    plot_per_thread(ax, fr, col)
    style_ax(ax, col.replace("_frac", ""), ylim=(0, 1))
axes[0].set_ylabel("fraction of pops")
axes[0].legend()
fig2.tight_layout()
fig2.savefig("results/pop_outcomes.png", dpi=150)

# ---------- 3. speculator waste + utilization ----------
spec = sp[sp["threads"] > 1].copy()

spec["waste_frac"] = (spec["speculations"] - spec["hits"] - spec["waits"]) / spec["speculations"]

s0 = df[(df["algo"] == "astar") & (df["cost"] == 0)]
base_us = (s0["us"] / s0["expanded"]).mean()      # base cost of one expansion
print(f"base expansion ≈ {base_us:.2f} µs")
spec["util"] = spec["speculations"] * (spec["cost"] + base_us) / ((spec["threads"] - 1) * spec["us"])

agg = spec.groupby(["threads", "cost"])[["waste_frac", "util"]].mean().reset_index()

fig3, (ax3, ax4) = plt.subplots(1, 2, figsize=(12, 5))
plot_per_thread(ax3, agg, "waste_frac")
style_ax(ax3, "Speculator effort thrown away", "fraction wasted", (0, 1))
plot_per_thread(ax4, agg, "util")
style_ax(ax4, "Speculator utilization", "fraction of time expanding", (0, 1.05))
ax3.legend(); ax4.legend()
fig3.tight_layout()
fig3.savefig("results/speculators.png", dpi=150)

print("saved plot.png, pop_outcomes.png, speculators.png")

# ---------- 4. board size (K) sweep ----------
T = 8   # thread count to study
kd = df[(df["algo"] == "spastar") & (df["threads"] == T)].copy()

kd["util"] = kd["speculations"] * (kd["cost"] + base_us) / ((kd["threads"] - 1) * kd["us"])
kavg = kd.groupby(["board_k", "cost"])[["ms", "util"]].mean().reset_index()
kavg["speedup"] = serial.loc[kavg["cost"]].values / kavg["ms"].values

# naive parallel at the same thread count, for reference
naive = avg[(avg["algo"] == "pastar") & (avg["threads"] == T)]

k_colors = plt.cm.viridis([i / max(1, kavg["board_k"].nunique() - 1) for i in range(kavg["board_k"].nunique())])

fig5, (ax6, ax7) = plt.subplots(1, 2, figsize=(12, 5))
for c, (k, g) in zip(k_colors, kavg.groupby("board_k")):
    ax6.plot(g["cost"], g["speedup"], "--o", color=c, label=f"K={k}")
    ax7.plot(g["cost"], g["util"],    "--o", color=c, label=f"K={k}")
ax6.plot(naive["cost"], serial.loc[naive["cost"]].values / naive["ms"].values,
         "-", color="gray", label=f"pastar, {T} thr")

style_ax(ax6, f"spastar speedup by board size ({T} thr)", "speedup vs serial")
ax6.axhline(1, color="k", linestyle="--")
style_ax(ax7, f"speculator utilization by board size ({T} thr)", "fraction of time expanding", (0, 1.05))
ax6.legend(); ax7.legend()
fig5.tight_layout()
fig5.savefig("results/board_size.png", dpi=150)
print("saved board_size.png")

# ---------- 5. text breakdown: expansions per second ----------
K_COL = "board_k"   # rename if your CSV calls it something else (e.g. "k_bench")

tp = df.copy()
tp["exp_per_s"] = tp["expanded"] / (tp["us"] / 1e6)

def config_label(row):
    if row["algo"] == "astar":
        return "astar"
    if row["algo"] == "pastar":
        return f"pastar  t={row['threads']}"
    return f"spastar t={row['threads']} k={row[K_COL]}"

tp = tp[(tp["algo"] != "astar") | (tp["threads"] == 1)]   # serial only once
tp["config"] = tp.apply(config_label, axis=1)

# sort: astar first, then pastar, then spastar, by threads then k
order = {"astar": 0, "pastar": 1, "spastar": 2}
tp["_o"] = tp["algo"].map(order)
tp[K_COL] = tp[K_COL].where(tp["algo"] == "spastar", 0)

table = (tp.groupby(["_o", "threads", K_COL, "config", "cost"])["exp_per_s"]
           .mean()
           .unstack("cost")
           .droplevel(["_o", "threads", K_COL]))

pretty = table.map(lambda v: f"{v/1000:,.0f}k" if pd.notna(v) else "-")
pretty.columns = [f"{c}µs" for c in pretty.columns]

text = "Expansions per second (mean of runs), columns = added expansion cost\n\n" + pretty.to_string()
print("\n" + text)
with open("results/throughput.txt", "w") as f:
    f.write(text + "\n")