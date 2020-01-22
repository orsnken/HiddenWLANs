import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv("output/D100_normalized.csv", index_col=0)
print(df.dropna(how="all", axis=1, inplace=True))

plt.figure()
sns.heatmap(df, cmap="jet", robust=True)
# sns.heatmap(df, cmap="jet", vmin=-4.0, vmax=2.0)
plt.show()
plt.close("all")