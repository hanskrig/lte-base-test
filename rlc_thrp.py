import pandas as pd
import numpy as np
df_dl = pd.read_csv("DlRlcStats.txt",  sep="\t", index_col = False)
df_ul = pd.read_csv("UlRlcStats.txt",  sep="\t", index_col = False)

#calculate simulation time
sim_time = df_dl.iloc[-1]["end"] - df_dl.iloc[0]["% start"]

# Calculate byte throughput in DL
df_thrp_dl = df_dl.groupby("IMSI").agg(thrp_dl=("RxBytes", "sum"))
df_thrp_dl["thrp_dl"] /= sim_time  # Divide by simulation time

# Calculate byte throughput in UL
df_thrp_ul = df_ul.groupby("IMSI").agg(thrp_ul=("RxBytes", "sum"))
df_thrp_ul["thrp_ul"] /= sim_time  # Divide by simulation time

# Merging DL and UL throughput on IMSI
df_thrp = pd.merge(df_thrp_dl, df_thrp_ul, on="IMSI")
print("Throughput in bytes")
print(df_thrp)

unit_val = ['b', 'b']
col_names = ['thrp_dl', 'thrp_ul']

for i, thrp_ind in enumerate(col_names):
    th = 8*df_thrp[thrp_ind] 
    unit_count = 0
    
    while th.mean() > 1024: 
        th /= 1024
        unit_count += 1
    
    df_thrp[thrp_ind] = th  
    
    # Update units
    if unit_count == 1:
        unit_val[i] = "K" + unit_val[i]
    elif unit_count == 2:
        unit_val[i] = "M" + unit_val[i]
    elif unit_count == 3:
        unit_val[i] = "G" + unit_val[i]



df_thrp = df_thrp.sort_values(by="IMSI").reset_index()

print("Throughput by UE:")

#printing thrp
for id in df_thrp.index:
    print(
    "UE with IMSI {ind} received {dl:.2f} {str_dl} in DL and transmitted {ul:.2f} {str_ul} in UL".format(
        ind=int(df_thrp.iloc[id]["IMSI"]),
        dl=float(df_thrp.iloc[id]["thrp_dl"]),
        ul=float(df_thrp.iloc[id]["thrp_ul"]),
        str_dl=unit_val[0],
        str_ul=unit_val[1]
    )
)
