import numpy as np
import matplotlib.pyplot as plt


def load(file):
    concentration = np.genfromtxt(file)[:, 0]
    clusters_count = np.genfromtxt(file)[:, 1]
    clusters_size = np.genfromtxt(file)[:, 2]
    return concentration, clusters_count, clusters_size


pic_size = np.array((10, 6))

plt.figure(figsize=pic_size)
conc_sc, clusters_count_sc, clusters_size_sc = load(
    "../data/output_txt/clusters_SC.txt"
)
conc_bcc, clusters_count_bcc, clusters_size_bcc = load(
    "../data/output_txt/clusters_BCC.txt"
)
conc_fcc, clusters_count_fcc, clusters_size_fcc = load(
    "../data/output_txt/clusters_FCC.txt"
)

plt.figure(figsize=pic_size)
fig, ax = plt.subplots(figsize=pic_size)
ax.spines["top"].set_visible(False)
ax.spines["right"].set_visible(False)
plt.plot(conc_sc, clusters_count_sc, c="black", linewidth=2, label="SC", markersize=8)
plt.plot(conc_bcc, clusters_count_bcc, c="red", linewidth=2, label="BCC", markersize=8)
plt.plot(conc_fcc, clusters_count_fcc, c="blue", linewidth=2, label="FCC", markersize=8)

plt.xlabel("Концентрация магнетика", fontsize=12)
plt.ylabel("Число кластеров", fontsize=12)
plt.legend(fontsize=12)
plt.tight_layout()

plt.savefig("../data/output_png/clusters_count.png", dpi=300, bbox_inches="tight")
# plt.show()

plt.figure(figsize=pic_size)
fig, ax = plt.subplots(figsize=pic_size)
ax.spines["top"].set_visible(False)
ax.spines["right"].set_visible(False)
plt.plot(conc_sc, clusters_size_sc, c="black", linewidth=2, label="SC", markersize=8)
plt.plot(conc_bcc, clusters_size_bcc, c="red", linewidth=2, label="BCC", markersize=8)
plt.plot(conc_fcc, clusters_size_fcc, c="blue", linewidth=2, label="FCC", markersize=8)

plt.xlabel("Концентрация магнетика", fontsize=12)
plt.ylabel("Параметр порядка кластеров", fontsize=12)
plt.legend(fontsize=12)
plt.tight_layout()

plt.savefig("../data/output_png/cluster_size.png", dpi=300, bbox_inches="tight")
# plt.show()
