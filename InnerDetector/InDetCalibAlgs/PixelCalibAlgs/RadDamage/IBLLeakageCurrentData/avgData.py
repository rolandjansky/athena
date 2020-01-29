#Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os, sys
import numpy as np
import pandas as pd
import datetime, time

# Factor that corrects the leakage current to 20 degrees C
def tempCorr(Temp,Eg):

    kB = 8.617*pow(10,-5) # eV/K
    Tref = 273.0 # Reference temperature in K
    Temp = Temp + 273 # Convert to K

    return pow(1.0*Tref/Temp,2)*np.exp((-0.5*Eg/kB)*(1.0/Tref - 1.0/Temp))

# Jennet shamelessly steals Nick's code for bookkeeping
def averageData (m,lumi_df):

    #home directory definition                                                                                                        
    fillerDate = datetime.datetime(2000,1,1,1,1,1,1)
    tempDate = datetime.datetime(2000,1,1,1,1,1,1)
    returnList =[fillerDate,fillerDate,fillerDate,False,"LI_S00_0_M0"]
    homeDirectory =  os.path.expanduser('/eos/atlas/user/j/jdickins/Pixel/LeakageCurrent/')

    # Define path to folder                                                                                                  
    dataFolder = homeDirectory + "/IBLData/processedData/means_dat/"
    inputFolder = homeDirectory + "/IBLData/processedData/"
    runDataFolder = homeDirectory + "/IBLData/processedData/Lumi/runData.txt"

    if not os.path.exists(dataFolder):
        os.mkdir(dataFolder)

    # Time bins = every day
    b = lumi_df["start"].drop_duplicates()

    output_dict = pd.DataFrame({})

    # Loop over lumi blocks                                                                                                   
    lumis = []
    total_lumi = 0 
    for l in lumi_df["intlumi"]:                                                                        
        total_lumi += l/(10**9)                                                                                        
        lumis += [ total_lumi ]   

    print(total_lumi)
    lumi_df["totlumi"] = lumis

#    tmp_dict = lumi_df.groupby(pd.cut(lumi_df["start"],bins=b),as_index=False).mean()
#    tmp_dict.fillna(method='ffill',inplace=True)
    output_dict["start"] = b
    output_dict["intlumi"] = lumi_df["totlumi"]
    output_dict.fillna(method='ffill',inplace=True) 
    times = [datetime.datetime.utcfromtimestamp(s) for s in b]

    plt.scatter(times,output_dict["intlumi"],marker=".")
    plt.title(m)
    plt.savefig(dataFolder+"intlumi/"+m+"_time.png")
    plt.close()
   
    # Jennet gets these from https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PixelConditionsRUN2
    voltage_settings = []

    volume = []
    sensorSize_planar = 50*250*200*1E-12 #cm3
    sensorSize_3D = 50*250*230*1E-12 #cm3

    sensorsPerModule = 336*80

    if m.endswith("M4"): # 3D module
        for s in output_dict["start"]:
            volume += [sensorSize_3D*sensorsPerModule*4]

            if m == "LI_S11_A_M4":
                if s < time.mktime(datetime.date(2017,1,1).timetuple()):
                    voltage_settings += [20.0]
                elif s < time.mktime(datetime.date(2018,1,1).timetuple()):
                    voltage_settings += [30.0]
                else:
                    voltage_settings += [20.0]
                continue
            if m == "LI_S12_A_M4":
                if s < time.mktime(datetime.date(2017,1,1).timetuple()):
                    voltage_settings += [20.0]
                elif s < time.mktime(datetime.date(2018,1,1).timetuple()):
                    voltage_settings += [21.0]
                else:
                    voltage_settings += [30.0]
                continue
            if m == "LI_S13_A_M4":
                if s < time.mktime(datetime.date(2017,1,1).timetuple()):
                    voltage_settings += [15.0]
                else:
                    voltage_settings += [40.0]
                continue
            if s < time.mktime(datetime.date(2017,1,1).timetuple()):
                voltage_settings += [20.0]
            else:
                voltage_settings += [40.0]
    else: # Planar module  
        for s in output_dict["start"]:
            volume += [sensorSize_planar*sensorsPerModule*4]

            if s < time.mktime(datetime.date(2016,9,16).timetuple()):
                voltage_settings += [80.0]
            elif s < time.mktime(datetime.date(2017,1,1).timetuple()):
                voltage_settings += [150.0]
            elif s < time.mktime(datetime.date(2017,11,7).timetuple()):
                voltage_settings += [350.0]
            else:
                voltage_settings += [400.0]

    output_dict["HV_VSet"] = voltage_settings
    output_dict["volume"] = volume  

    dataTypes = ["PP4LV","TModule","ENV_TT","HV_VMeas","HV_IMeas"]
    dataType_index = 0

    for dataType in dataTypes:

        print ("Investigating " + dataType )

        if not os.path.exists(dataFolder+dataType):
                os.mkdir(dataFolder+dataType)

        # DO THE AVERAGES
        infile = inputFolder + dataType + "/" + m + ".ssv"
        meas_header=["module_name","measurement_date","measurement_time","unix-timestamp",dataType]
        meas_dict = pd.read_csv(infile, names=meas_header, delimiter=' ', skiprows=1)
        output_dict[dataType] = meas_dict.groupby(pd.cut(meas_dict["unix-timestamp"],bins=b),as_index=False).mean()[dataType]

        if dataType == "TModule" or dataType == "PP4LV" or dataType == "ENV_TT":
            output_dict.fillna(method='ffill',inplace=True)

        if dataType == "HV_VMeas":
            output_dict["HV_VMeas_0"] = meas_dict.groupby(pd.cut(meas_dict["unix-timestamp"],bins=b),as_index=False).mean()[dataType]
            output_dict.fillna(method='ffill',inplace=True)
            output_dict["HV_VMeas_1"] = meas_dict.groupby(pd.cut(meas_dict["unix-timestamp"],bins=b),as_index=False).mean()[dataType]
            output_dict.fillna(method='bfill',inplace=True)
            output_dict["HV_VMeas"] = output_dict[["HV_VMeas_0","HV_VMeas_1"]].mean(axis=1)

        output_dict.plot.scatter(x="intlumi",y=dataType,marker=".")
        plt.title(m)
        plt.savefig(dataFolder+dataType+"/"+m+".png")
        plt.close()
        
        plt.scatter(times,output_dict[dataType],marker=".")
        plt.title(m)
        plt.savefig(dataFolder+dataType+"/"+m+"_time.png")
        plt.close()

    # Take cooling pipe temp                                                                                            
#    output_dict['TModule'] = np.where(output_dict['TModule'] < -20, output_dict['ENV_TT'], output_dict['TModule'])
        
    plt.scatter(times,output_dict["TModule"],marker=".",s=1,label="TModule")
    plt.scatter(times,output_dict["ENV_TT"],marker=".",s=1,label="ENV_TT")
    plt.legend()
    plt.title(m)
    plt.savefig(m+".png")
    plt.close()

    saveFileName = dataFolder + m + "_nocuts.ssv"
    if os.path.exists(saveFileName):
        os.remove(saveFileName)
    output_dict.to_csv(saveFileName,index=False)

    output_dict.dropna(inplace=True)

    # Veto
    output_dict = output_dict[abs(output_dict["HV_VMeas"]-output_dict["HV_VSet"])<1.0]

    # Correct
    output_dict["I_Eg1.12"] = [ row["HV_IMeas"] * tempCorr(row["TModule"],1.12) / row["volume"] for i, row in output_dict.iterrows() ]
    output_dict["I_Eg1.21"] = [ row["HV_IMeas"] * tempCorr(row["TModule"],1.21) / row["volume"] for i, row in output_dict.iterrows() ]
    output_dict["I_Eg1.30"] = [ row["HV_IMeas"] * tempCorr(row["TModule"],1.30) / row["volume"] for i, row in output_dict.iterrows() ]

    if not os.path.exists(dataFolder+"I_Eg1.12"):
        os.mkdir(dataFolder+"I_Eg1.12")
    output_dict.plot.scatter("intlumi","I_Eg1.12",marker=".")
    plt.title(m)
    plt.savefig(dataFolder+"I_Eg1.12/"+m+".png")
    plt.close()

    if not os.path.exists(dataFolder+"I_Eg1.21"):
        os.mkdir(dataFolder+"I_Eg1.21")
    output_dict.plot.scatter("intlumi","I_Eg1.21",marker=".")
    plt.title(m)
    plt.savefig(dataFolder+"I_Eg1.21/"+m+".png")
    plt.close()

    if not os.path.exists(dataFolder+"I_Eg1.30"):
        os.mkdir(dataFolder+"I_Eg1.30")
    output_dict.plot.scatter("intlumi","I_Eg1.30",marker=".")
    plt.title(m)
    plt.savefig(dataFolder+"I_Eg1.30/"+m+".png")
    plt.close()

    saveFileName = dataFolder + m + ".ssv"
    if os.path.exists(saveFileName):
        os.remove(saveFileName)
    output_dict.to_csv(saveFileName,index=False)

# Begin script
def main():

    infile_lumi = "/eos/atlas/user/j/jdickins/Pixel/LeakageCurrent/IBLData/processedData/Lumi/runData.txt"
    lumi_header=["run","fill","lb","start","len","0","1","lumiall","intlumi"]
    lumi_df=pd.read_csv(infile_lumi, names=lumi_header, delimiter=' ', skiprows=0)

#    lumi_df.drop_duplicates(subset='intlumi',keep='first',inplace=True)

    input_module = sys.argv[1]
    averageData(input_module,lumi_df)

if __name__ == "__main__":
    main()
