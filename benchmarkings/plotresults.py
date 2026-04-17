import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("benchmark.csv")

subset = df[(df["type"] == "int") & (df["structure"] == "std::set")]

plt.plot(subset["size"], subset["insert_us"])
plt.show()
