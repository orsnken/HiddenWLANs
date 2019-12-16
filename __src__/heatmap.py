import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

df = pd.read_csv("output/D30_throughput.csv", index_col=0)
print(df.dropna(how="all", axis=1, inplace=True))

plt.figure()
sns.heatmap(df, cmap="Spectral_r")
plt.show()
plt.close("all")