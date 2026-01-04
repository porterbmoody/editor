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
df.head().groupby(['root']).sum()


# %%
df.groupby(['root']).sum()



# %%
