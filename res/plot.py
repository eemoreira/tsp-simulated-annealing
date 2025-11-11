import sys
import matplotlib.pyplot as plt
import os

def ler_dados(arquivo):
    iteracoes = []
    custos = []
    with open(arquivo, "r") as f:
        for linha in f:
            partes = linha.strip().split()
            if len(partes) == 2:
                try:
                    it = int(partes[0])
                    custo = float(partes[1])
                    iteracoes.append(it)
                    custos.append(custo)
                except ValueError:
                    continue
    return iteracoes, custos

if __name__ == "__main__":
    arquivos = [
        ("res/Cooling-0_run_0.txt", "Cooling-0"),
        ("res/Cooling-1_run_0.txt", "Cooling-1"),
        ("res/Cooling-5_run_0.txt", "Cooling-5")
    ]

    plt.figure(figsize=(10, 6))

    for caminho, nome in arquivos:
        if not os.path.exists(caminho):
            print(f"Arquivo não encontrado: {caminho}")
            continue
        iteracoes, custos = ler_dados(caminho)
        if not iteracoes or not custos:
            print(f"Nenhum dado válido encontrado em {caminho}.")
            continue

        plt.plot(iteracoes, custos, label=nome, linewidth=1)

    plt.xlabel("Iteração")
    plt.ylabel("Custo (tourCost)")
    plt.title("Evolução do Custo por Iteração")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    saida = "res/iterations_comparacao.png"
    plt.savefig(saida)
    print(f"Gráfico combinado salvo em: {saida}")

