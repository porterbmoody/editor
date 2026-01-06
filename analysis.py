#%%
import pandas as pd
import os
import csv

file_path = 'logs/log2.txt'

# with open(file_path, newline='') as csvfile:
    # reader = csv.DictReader(csvfile)
    # for row in reader:
        # print(row)
rows = []
with open(file_path, 'r') as f:
    for line in f:
        print(line, end='')
        rows.append(line)

# %%
import pandas as pd

file_path = 'logs/log2.txt'
rows = []

with open(file_path, 'r') as f:
    line_number = 0
    for line in f:
        if line_number == 0:
            line_number += 1
            continue
        line_number += 1
        print(line_number)
        values = line.strip().split(',')
        rows.append(values)

df = pd.DataFrame(rows, columns = ['root','c0','c1','c2','c3','c4','c5','c6','c7','c8','c9','c10','c11','c12','c13'])
df


# %%
# df.head(10000).groupby(['root', 'c0']).sum().reset_index()
# df.head(10000).groupby(['root', 'c0']).agg('sum').reset_index()

# Option 2: Using agg()
grouped = df.groupby(['root', 'c0', 'c1']).agg(
    count=('c0', 'count'),
    # value_sum=('value_column', 'sum')
).reset_index().sort_values(by='count',ascending=False).query("count>6")

grouped

# grouped

# %%
df.groupby(['root']).sum()

# %%

import pandas as pd
import matplotlib.pyplot as plt

# top_df = df.sort_values('count', ascending=False).head(10)

plt.figure(figsize=(12,6))
plt.bar(grouped['c0'], grouped['count'], color='skyblue')
plt.xticks(rotation=45, ha='right')
plt.xlabel('c0')
plt.ylabel('Count')
plt.title('Top 10 c0 counts')
plt.tight_layout()
plt.show()

# %%
import pandas as pd
import matplotlib.pyplot as plt

plt.figure(figsize=(12,6))
plt.bar(grouped.head(100)['c1'], grouped.head(100)['count'], color='skyblue')
plt.xticks(rotation=45, ha='right')
plt.xlabel('c1')
plt.ylabel('Count')
plt.title('Top 10 c0 counts')
plt.tight_layout()
plt.show()
