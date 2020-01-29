2018 IBL Leakage current measurement

Code started by Nick Dann and further developed by Jennet Dickinson (jdickinson@lbl.gov).  For questions, please contact Jennet and/or Ben Nachman (bnachman@cern.ch).  



Contents:

grabAllAutoStave.py
* Nick's modified script can download data from DCS if run on lxplus. 
  Jennet's raw 2018 data is being stored at 
  /eos/atlas/atlascerngroupdisk/det-ibl/data/leakage-current/rawData/
  Function grabData gets one day at a time. Fuction grabDataStave divides this into 
  smaller bites and then merges into one file per day. Use grabDataStave if grabData 
  returns errors like this: "Detected very big DB data request"

getIBLDate.py
* Nick's modified script reads the raw data and adds new days to the processed
  .ssv file for each module
  Processed data is being stored at 
  /eos/atlas/atlascerngroupdisk/det-ibl/data/leakage-current/processedData

lumiFormat.py
* Processes the luminosity from COOL to match the format of Nick's lumi text files. 
  Thank you to Aidan Grummer and Eric Torrence for the COOL setup.
  From COOL output, take the instantaneous luminosity for each lumi block from the 
  "Lumi" column and multiply by the length of the lumi block in seconds (column "Len (s)"). 
  This gives the integrated luminosity collected during this lumi block. Warning: 
  the column "IntLumi" looks useful, but counts integrated luminosity from the beginning 
  of the fill, not the beginning of the LB

avgData.py
* Saves a pandas dataframe containing HV_VMeas [V], PP4LV [V], TModule [C], HV_IMeas [mA], and 
  integrated luminosity [fb-1] per LB for each module. Also plots HV_VMeas [V], PP4LV [V], 
  TModule [C], HV_IMeas [mA] against integrated luminosity. See output here:
  /eos/atlas/atlascerngroupdisk/det-ibl/data/leakage-current/processedData/means_data

  Fills all variables for each lumi-block, then keeps only those lumi-blocks where a 
  measurement of HV_IMeas is made.

  Run by doing 
  python avgData.py ${moduleName}

avgSim.py
* Saves a pandas dataframe containing HV_VMeas [V], PP4LV [V], TModule [C], HV_IMeas [mA], and
  integrated luminosity [fb-1] per LB for each module. Also plots HV_VMeas [V], PP4LV [V],
  TModule [C], HV_IMeas [mA] against integrated luminosity. See output here:
  /eos/atlas/atlascerngroupdisk/det-ibl/data/leakage-current/processedData/means_sim

  Fills all variables for each hour, including during shutdowns. Used as input for
  Hamburg model simulations.

  Run by doing
  python avgSim.py ${moduleName}

submit_avg.sh and run_avg.sh
* Scripts for submitting batch jobs on lxplus to do the average with avgOverLB.py
  Submits one job per module
  
graphs.py
* Averages over module groups at similar z and creates TGraphs for plotting

plot.C
* Draws the plots
