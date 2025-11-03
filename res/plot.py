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

def plotar(iteracoes, custos, saida):
    plt.figure(figsize=(10, 6))
    plt.plot(iteracoes, custos, marker='o', linestyle='-', markersize=1, linewidth=1)
    plt.xlabel("Iteração")
    plt.ylabel("Custo (tourCost)")
    plt.title("Evolução do Custo por Iteração")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(saida)
    print(f"Gráfico salvo em: {saida}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"Uso: python {sys.argv[0]} <arquivo_log>")
        sys.exit(1)

    caminho = sys.argv[1]
    iteracoes, custos = ler_dados(caminho)

    if not iteracoes:
        print("Nenhum dado encontrado no arquivo.")
    else:
        nome_saida = os.path.splitext(caminho)[0] + ".png"
        plotar(iteracoes, custos, nome_saida)

