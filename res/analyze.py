#!/usr/bin/env python3
"""
analyze_results.py

Uso:
    python3 analyze_results.py [path_to_results_file]

Formato esperado do arquivo:
    Cooling-0 1234.56
    Cooling-1 1300.12
    Cooling-5 1278.90
    ...
(uma linha por resultado; label seguido do custo final)
"""

import sys
import os
from collections import defaultdict
import numpy as np
import csv
from matplotlib import pyplot as plt

def read_results(path):
    groups = defaultdict(list)
    with open(path, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            # aceitamos linhas com label + value (primeiros dois tokens)
            if len(parts) < 2:
                continue
            label = parts[0]
            try:
                value = float(parts[1])
            except ValueError:
                # ignora linhas mal formatadas
                continue
            groups[label].append(value)
    return groups

def summarize(groups):
    summary = {}
    for label, values in groups.items():
        arr = np.array(values, dtype=float)
        mean = float(np.mean(arr)) if arr.size > 0 else float("nan")
        std = float(np.std(arr, ddof=1)) if arr.size > 1 else 0.0
        summary[label] = {
            "count": int(arr.size),
            "mean": mean,
            "std": std,
            "mean_plus_minus_std": f"{mean:.6g} ± {std:.6g}"
        }
    return summary

def save_csv(summary, outpath):
    fieldnames = ["label", "count", "mean", "std", "mean±std"]
    with open(outpath, "w", newline="") as csvf:
        writer = csv.DictWriter(csvf, fieldnames=fieldnames)
        writer.writeheader()
        for label, stats in sorted(summary.items()):
            writer.writerow({
                "label": label,
                "count": stats["count"],
                "mean": f"{stats['mean']:.10g}",
                "std": f"{stats['std']:.10g}",
                "mean±std": stats["mean_plus_minus_std"]
            })

def plot_boxplot(groups, outpath):
    # ordena por label para consistência
    labels = sorted(groups.keys())
    data = [groups[l] for l in labels]

    plt.figure(figsize=(9,6))
    # boxplot: cada lista em data vira uma caixa
    plt.boxplot(data, labels=labels, notch=False, showmeans=True)
    plt.xlabel("Cooling schedule / Instância")
    plt.ylabel("Custo final (tourCost)")
    plt.title("Box-plot das 10 runs por cooling schedule")
    plt.grid(axis='y', linestyle='--', linewidth=0.5, alpha=0.7)
    plt.tight_layout()
    plt.savefig(outpath)
    plt.close()

def print_summary(summary):
    print("\nResumo dos resultados (por label):\n")
    print(f"{'label':25s} {'count':>5s} {'mean':>14s} {'std':>14s} {'mean±std':>20s}")
    print("-"*80)
    for label, stats in sorted(summary.items()):
        print(f"{label:25s} {stats['count']:5d} {stats['mean']:14.6f} {stats['std']:14.6f} {stats['mean_plus_minus_std']:>20s}")
    print("")

def main():
    path = sys.argv[1] if len(sys.argv) > 1 else "res/results.txt"
    if not os.path.exists(path):
        print(f"Arquivo não encontrado: {path}")
        sys.exit(1)

    groups = read_results(path)

    if not groups:
        print(f"Nenhum dado válido encontrado em {path}")
        sys.exit(1)

    summary = summarize(groups)

    # garante pasta de saída
    out_dir = os.path.dirname(path) or "."
    csv_out = os.path.join(out_dir, "results_summary.csv")
    png_out = os.path.join(out_dir, "results_boxplot.png")

    save_csv(summary, csv_out)
    plot_boxplot(groups, png_out)
    print_summary(summary)

    print(f"Tabela salva em: {csv_out}")
    print(f"Box-plot salvo em: {png_out}")

if __name__ == "__main__":
    main()

