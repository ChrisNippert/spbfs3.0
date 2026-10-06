import pandas as pd
import matplotlib.pyplot as plt

CSV = "results/all.csv"
df = pd.read_csv(CSV)
df["ms"] = df["us"] / 1000

colors = {1: "tab:blue", 2: "tab:orange", 4: "tab:green", 8: "tab:red"}
styles = {"pastar": "-", "spastar": "--"}
saved = []

# ---------- helpers ----------
def has_algo(name, data=df):
    return (data["algo"] == name).any()

def has_cols(*cols, data=df):
    missing = [c for c in cols if c not in data.columns]
    return not missing, missing

def skip(section, why):
    print(f"[skip] {section}: {why}")

def style_ax(ax, title, ylabel=None, ylim=None):
    ax.set_title(title)
    ax.set_xlabel("added expansion cost (µs)")
    ax.set_xscale("symlog", linthresh=1)
    ax.set_xlim(left=0)
    if ylabel: ax.set_ylabel(ylabel)
    if ylim:   ax.set_ylim(*ylim)
    ax.grid(alpha=0.3)

def legend_if_any(ax):
    if ax.get_legend_handles_labels()[0]:
        ax.legend()

def plot_per_thread(ax, data, col):
    for t, g in data.groupby("threads"):
        ax.plot(g["cost"], g[col], "--o", color=colors.get(t), label=f"{t} thr")

def save(fig, name):
    fig.tight_layout()
    fig.savefig(f"results/{name}", dpi=150)
    plt.close(fig)
    saved.append(name)

# ---------- pick a default K that actually exists ----------
DEFAULT_K = None
if has_algo("spastar") and "board_k" in df.columns:
    ks = sorted(df.loc[df["algo"] == "spastar", "board_k"].unique())
    DEFAULT_K = 32 if 32 in ks else ks[0]
    print(f"default K for main comparison: {DEFAULT_K} (available: {ks})")

main = df if DEFAULT_K is None else df[(df["algo"] != "spastar") | (df["board_k"] == DEFAULT_K)]
sp = main[main["algo"] == "spastar"].copy()

# ---------- sanity checks ----------
print("path lengths seen:", sorted(df["path"].unique()))
print(df.groupby("algo")["expanded"].agg(["min", "max"]))
ok, _ = has_cols("speculations", "hits", "waits")
if ok and len(sp):
    bad = sp[sp["speculations"] < sp["hits"] + sp["waits"]]
    print("invariant violations (speculations < hits + waits):", len(bad))

# ---------- serial baseline (used by several sections) ----------
avg = main.groupby(["algo", "threads", "cost"])["ms"].mean().reset_index()
serial = avg[avg["algo"] == "astar"].set_index("cost")["ms"] if has_algo("astar") else None

def speedup(costs, ms):
    """serial time / ms at matching cost; NaN where serial is missing."""
    return serial.reindex(costs).values / ms.values

base_us = None
if has_algo("astar") and (df.loc[df["algo"] == "astar", "cost"] == 0).any():
    s0 = df[(df["algo"] == "astar") & (df["cost"] == 0)]
    base_us = (s0["us"] / s0["expanded"]).mean()
    print(f"base expansion ≈ {base_us:.2f} µs")

# ---------- 1. time + speedup ----------
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
for (algo, t), g in avg.groupby(["algo", "threads"]):
    if algo == "astar":
        ax1.plot(g["cost"], g["ms"], "k:o", label="serial A*")
        continue
    kw = dict(color=colors.get(t), linestyle=styles.get(algo, "-"), marker="o",
              label=f"{algo}, {t} thr")
    ax1.plot(g["cost"], g["ms"], **kw)
    if serial is not None:
        ax2.plot(g["cost"], speedup(g["cost"], g["ms"]), **kw)
style_ax(ax1, "Search time", "ms")
ax1.set_yscale("log")
style_ax(ax2, "Speedup vs serial" + ("" if serial is not None else " (no astar data)"), "speedup")
ax2.axhline(1, color="k", linestyle="--")
legend_if_any(ax1); legend_if_any(ax2)
save(fig, "plot.png")

# ---------- 2. pop outcomes ----------
ok, missing = has_cols("hits", "waits", "misses")
if not len(sp):
    skip("pop outcomes", "no spastar rows")
elif not ok:
    skip("pop outcomes", f"missing columns {missing}")
else:
    pops = sp["hits"] + sp["waits"] + sp["misses"]
    for col in ["hits", "waits", "misses"]:
        sp[col + "_frac"] = sp[col] / pops
    fr = sp.groupby(["threads", "cost"])[["hits_frac", "waits_frac", "misses_frac"]].mean().reset_index()

    fig2, axes = plt.subplots(1, 3, figsize=(15, 4), sharey=True)
    for ax, col in zip(axes, ["hits_frac", "waits_frac", "misses_frac"]):
        plot_per_thread(ax, fr, col)
        style_ax(ax, col.replace("_frac", ""), ylim=(0, 1))
    axes[0].set_ylabel("fraction of pops")
    legend_if_any(axes[0])
    save(fig2, "pop_outcomes.png")

# ---------- 3. speculator waste + utilization (+ empty scans if present) ----------
ok, missing = has_cols("speculations", "hits", "waits")
spec = sp[sp["threads"] > 1].copy() if len(sp) else sp
if not len(spec):
    skip("speculators", "no spastar rows with threads > 1")
elif not ok:
    skip("speculators", f"missing columns {missing}")
else:
    spec["waste_frac"] = (spec["speculations"] - spec["hits"] - spec["waits"]) / spec["speculations"]
    cols = ["waste_frac"]
    if base_us is not None:
        spec["util"] = spec["speculations"] * (spec["cost"] + base_us) / ((spec["threads"] - 1) * spec["us"])
        cols.append("util")
    if "empty-scans" in spec.columns:
        spec["empty_per_spec"] = spec["empty-scans"] / spec["speculations"].clip(lower=1)
        cols.append("empty_per_spec")
    agg = spec.groupby(["threads", "cost"])[cols].mean().reset_index()

    titles = {"waste_frac":     ("Speculator effort thrown away", "fraction wasted", (0, 1)),
              "util":           ("Speculator utilization", "fraction of time expanding", (0, 1.05)),
              "empty_per_spec": ("Empty board scans per speculation", "scans / speculation", None)}
    fig3, axes = plt.subplots(1, len(cols), figsize=(6 * len(cols), 5), squeeze=False)
    for ax, col in zip(axes[0], cols):
        plot_per_thread(ax, agg, col)
        style_ax(ax, *titles[col])
        if col == "empty_per_spec": ax.set_yscale("log")
        legend_if_any(ax)
    if base_us is None:
        print("[note] utilization skipped: no astar cost-0 runs for base expansion time")
    save(fig3, "speculators.png")

# ---------- 4. board size (K) sweep ----------
all_sp = df[df["algo"] == "spastar"]
if not len(all_sp) or "board_k" not in df.columns:
    skip("board size", "no spastar rows / board_k column")
elif all_sp["board_k"].nunique() < 2:
    skip("board size", "only one K value present")
else:
    T = 8 if (all_sp["threads"] == 8).any() else all_sp["threads"].max()
    kd = all_sp[all_sp["threads"] == T].copy()
    kavg = kd.groupby(["board_k", "cost"])["ms"].mean().reset_index()
    if base_us is not None and T > 1 and "speculations" in kd.columns:
        kd["util"] = kd["speculations"] * (kd["cost"] + base_us) / ((T - 1) * kd["us"])
        kavg = kavg.merge(kd.groupby(["board_k", "cost"])["util"].mean().reset_index())

    nk = kavg["board_k"].nunique()
    k_colors = plt.cm.viridis([i / max(1, nk - 1) for i in range(nk)])

    fig5, (ax6, ax7) = plt.subplots(1, 2, figsize=(12, 5))
    for c, (k, g) in zip(k_colors, kavg.groupby("board_k")):
        if serial is not None:
            ax6.plot(g["cost"], speedup(g["cost"], g["ms"]), "--o", color=c, label=f"K={k}")
        else:
            ax6.plot(g["cost"], g["ms"], "--o", color=c, label=f"K={k}")
        if "util" in g:
            ax7.plot(g["cost"], g["util"], "--o", color=c, label=f"K={k}")

    naive = avg[(avg["algo"] == "pastar") & (avg["threads"] == T)]
    if len(naive) and serial is not None:
        ax6.plot(naive["cost"], speedup(naive["cost"], naive["ms"]), "-", color="gray", label=f"pastar, {T} thr")

    style_ax(ax6, f"spastar by board size ({T} thr)", "speedup vs serial" if serial is not None else "ms")
    if serial is not None: ax6.axhline(1, color="k", linestyle="--")
    style_ax(ax7, f"speculator utilization by board size ({T} thr)", "fraction of time expanding", (0, 1.05))
    legend_if_any(ax6); legend_if_any(ax7)
    save(fig5, "board_size.png")

# ---------- 5. text breakdown: expansions per second ----------
tp = df[(df["algo"] != "astar") | (df["threads"] == 1)].copy()
tp["exp_per_s"] = tp["expanded"] / (tp["us"] / 1e6)
has_k = "board_k" in tp.columns

def config_label(row):
    if row["algo"] == "astar":  return "astar"
    if row["algo"] == "pastar": return f"pastar  t={row['threads']}"
    return f"spastar t={row['threads']}" + (f" k={row['board_k']}" if has_k else "")

tp["config"] = tp.apply(config_label, axis=1)
tp["_o"] = tp["algo"].map({"astar": 0, "pastar": 1, "spastar": 2}).fillna(3)
tp["_k"] = tp["board_k"].where(tp["algo"] == "spastar", 0) if has_k else 0

table = (tp.groupby(["_o", "threads", "_k", "config", "cost"])["exp_per_s"]
           .mean().unstack("cost")
           .droplevel(["_o", "threads", "_k"]))

fmt = lambda v: f"{v/1000:,.0f}k" if pd.notna(v) else "-"
pretty = table.map(fmt) if hasattr(table, "map") else table.applymap(fmt)
pretty.columns = [f"{c}µs" for c in pretty.columns]

text = "Expansions per second (mean of runs), columns = added expansion cost\n\n" + pretty.to_string()
print("\n" + text)
with open("results/throughput.txt", "w") as f:
    f.write(text + "\n")

print("\nsaved:", ", ".join(saved) or "(no figures)")