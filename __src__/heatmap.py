import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

df = pd.read_csv("output/D50_throughput.csv", index_col=0)
print(df.dropna(how="all", axis=1, inplace=True))

plt.figure()
sns.heatmap(df, cmap="jet")
plt.show()
plt.close("all")