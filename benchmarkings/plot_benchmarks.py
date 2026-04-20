import argparse
import csv
import re
from io import StringIO

import matplotlib.pyplot as plt
import pandas as pd

TIME_COL = "real_time"  # поменяйте на "cpu_time", если хотите CPU time
OUTPUT_FILE = "benchmark_9graphs.png"


NAME_RE = re.compile(
    r'^"?BM_(?P<op>[A-Za-z]+)<(?P<structure>.+?),\s*(?P<type>[^>]+)>/(?P<size>\d+)"?$'
)


def extract_bm_rows(path: str) -> pd.DataFrame:
    """
    Reads the file, keeps only header + lines starting with BM_,
    and returns a DataFrame with parsed columns:
    operation, structure, type, size, time.
    """
    with open(path, "r", encoding="utf-8") as f:
        lines = [line.rstrip("\n") for line in f if line.strip()]

    header_idx = None
    for i, line in enumerate(lines):
        if line.startswith("name,"):
            header_idx = i
            break

    if header_idx is None:
        raise ValueError("Header line starting with 'name,' was not found.")

    # Keep only benchmark rows that start with BM_
    kept_lines = [lines[header_idx]]
    for line in lines[header_idx + 1 :]:
        s = line.lstrip()
        if s.startswith('"BM_') or s.startswith("BM_"):
            kept_lines.append(line)

    text = "\n".join(kept_lines)
    reader = csv.DictReader(StringIO(text))

    rows = []
    for row in reader:
        name = row["name"].strip()

        m = NAME_RE.match(name)
        if not m:
            continue

        op = m.group("op")
        structure_raw = m.group("structure").strip()
        type_name = m.group("type").strip()
        size = int(m.group("size"))

        # Normalize structure name:
        #   SwissTable<int> -> SwissTable
        #   std::unordered_set<int> -> std::unordered_set
        #   std::set<int> -> std::set
        structure = re.sub(r"<.*>$", "", structure_raw).strip()

        # Convert time to numeric
        time_value = pd.to_numeric(row[TIME_COL], errors="coerce")
        if pd.isna(time_value):
            continue

        rows.append(
            {
                "operation": op,
                "structure": structure,
                "type": type_name,
                "size": size,
                "time": float(time_value),
            }
        )

    df = pd.DataFrame(rows)
    if df.empty:
        raise ValueError("No benchmark rows were parsed. Check the file format.")
    return df


def plot_benchmarks(df: pd.DataFrame, output_file: str = OUTPUT_FILE) -> None:
    df["type"] = df["type"].replace(
        {
            "std::string": "string",
            "string": "string",
            "int": "int",
            "double": "double",
        }
    )

    type_order = ["int", "double", "string"]
    op_order = ["Insert", "Find", "Erase"]

    # Keep only the types/ops we know, but do not fail if some are missing
    types = [t for t in type_order if t in df["type"].unique()]
    ops = [o for o in op_order if o in df["operation"].unique()]

    fig, axes = plt.subplots(
        len(types),
        len(ops),
        figsize=(22, 22),
        sharex=True,
        sharey=False,
    )

    if len(types) == 1 and len(ops) == 1:
        axes = [[axes]]
    elif len(types) == 1:
        axes = [axes]
    elif len(ops) == 1:
        axes = [[ax] for ax in axes]

    for i, t in enumerate(types):
        for j, op in enumerate(ops):
            ax = axes[i][j]
            subset = df[(df["type"] == t) & (df["operation"] == op)].copy()
            subset = subset.sort_values("size")

            for structure in sorted(subset["structure"].unique()):
                s = subset[subset["structure"] == structure]
                ax.plot(
                    s["size"],
                    s["time"],
                    marker="o",
                    linewidth=2,
                    label=structure,
                )

            ax.set_xscale("log")
            ax.set_yscale("log")
            ax.set_title(f"{t} — {op}")
            ax.set_xlabel("Size")
            ax.set_ylabel(f"{TIME_COL} (ns)")
            ax.grid(True, which="both", linestyle="--", alpha=0.4)
            ax.legend(fontsize=8)

    fig.suptitle(
        f"Benchmark visualization ({TIME_COL})",
        fontsize=16,
        fontweight="bold",
    )
    fig.tight_layout(rect=[0, 0.02, 1, 0.96])

    plt.savefig(output_file, dpi=200, bbox_inches="tight")
    plt.show()
    print(f"Saved to: {output_file}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "file",
        help="Path to benchmark CSV file",
        default="./results/googlebenchmark.csv",
    )
    args = parser.parse_args()

    df = extract_bm_rows(args.file)
    plot_benchmarks(df)


if __name__ == "__main__":
    main()
