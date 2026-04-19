import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# -----------------------------
# Load data from CSV file
# -----------------------------
# Put your benchmark data into benchmark.csv
df = pd.read_csv("benchmark.csv")

# -----------------------------
# Settings
# -----------------------------
sns.set(style="whitegrid")
plt.rcParams["figure.figsize"] = (18, 12)

metrics = ["insert_us", "find_us", "erase_us"]
titles = {
    "insert_us": "Insert",
    "find_us": "Find",
    "erase_us": "Erase"
}

# -----------------------------
# One page per type
# -----------------------------
for data_type in df["type"].unique():

    fig, axes = plt.subplots(1, 3, figsize=(20, 6))
    fig.suptitle(f"Benchmark Results ({data_type})", fontsize=18, fontweight="bold")

    subset = df[df["type"] == data_type]

    for i, metric in enumerate(metrics):
        ax = axes[i]

        sns.lineplot(
            data=subset,
            x="size",
            y=metric,
            hue="structure",
            marker="o",
            linewidth=2,
            ax=ax
        )

        ax.set_title(titles[metric])
        ax.set_xlabel("Size")
        ax.set_ylabel("Time (μs)")
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.grid(True, which="both", linestyle="--", alpha=0.5)

        if i != 2:
            ax.get_legend().remove()
        else:
            ax.legend(title="Structure")

    plt.tight_layout()
    plt.show()