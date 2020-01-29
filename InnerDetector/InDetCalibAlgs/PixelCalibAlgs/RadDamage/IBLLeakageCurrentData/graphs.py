#Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import matplotlib
#matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os, sys
import numpy as np
import pandas as pd

# Jennet averages over modules and saves TGraphs
def averageModules (suffix):

    blacklist = ["LI_S01_C_M4","LI_S03_A_M4","LI_S05_C_M4","LI_S11_A_M4","LI_S12_A_M4","LI_S13_C_M4"]

    indir = "/eos/atlas/user/j/jdickins/Pixel/LeakageCurrent/IBLData/processedData/means/"
    header = ["unix-timestamp","HV_VMeas","start","intlumi","HV_VSet","volume","HV_VMeas_0","HV_VMeas_1","PP4LV","TModule","HV_IMeas","ENV_TT","I_Eg1.12","I_Eg1.21","I_Eg1.30"]

    df_concat = pd.DataFrame({})
    # loop over modules and add the ones with the right suffix
    for stave in range(1,15):
        staveString = str(stave)
        if stave<10:
            staveString="0"+str(stave)
        m = "LI_S" + str(staveString) + "_" + suffix
        print(m)
        this_infile = indir + m + ".ssv"
        if m in blacklist:
            print(m + " is blacklisted. Skipping...")
            continue

        this_infile = indir + "LI_S" + str(staveString) + "_" + suffix + ".ssv"
        this_df = pd.read_csv(this_infile, names=header, delimiter=',', skiprows=1)
        df_concat = df_concat.append(this_df)

    df_avg = df_concat.groupby(df_concat.index).mean()
    df_avg = df_avg[["intlumi","I_Eg1.12","I_Eg1.21","I_Eg1.30"]]
#    df_avg.plot.scatter("intlumi","I_Eg1.12",marker=".")

#    print((1.0*max(df_avg["start"])-min(df_avg["start"]))/31557600,max(df_avg["intlumi"]))

    saveFileName = indir + suffix + ".ssv"
    if os.path.exists(saveFileName):
        os.remove(saveFileName)
    df_avg.to_csv(saveFileName,index=False)

# Begin script
def main():

    averageModules("A_M1")
    averageModules("C_M1")
    averageModules("A_M2")
    averageModules("C_M2")
    averageModules("A_M3")
    averageModules("C_M3")
    averageModules("A_M4")
    averageModules("C_M4")

if __name__ == "__main__":
    main()
