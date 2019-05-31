# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from AthenaConfiguration.AthConfigFlags import AthConfigFlags


def createLArConfigFlags(): 
    lcf=AthConfigFlags()
 
    lcf.addFlag("LAr.doAlign",lambda prevFlags : prevFlags.GeoModel.Layout=="atlas")
    lcf.addFlag("LAr.doHVCorr",lambda prevFlags : not prevFlags.Input.isMC)
    lcf.addFlag("LAr.doCellEmMisCalib",lambda prevFlags : prevFlags.Input.isMC)

    lcf.addFlag("LAr.RawChannelSource",_determineRawChannelSource)
                #sensible value are "input": read from the input-file, bytestream or RDO)
                #                   "calculated": re-computed by the offline LArRawChannelBuilder
                #                   "both": overwrite the digits computed 

    lcf.addFlag("LAr.doCellNoiseMasking",True)
    lcf.addFlag("LAr.doCellSporadicNoiseMasking",True)
    # Include MC shape folder
    lcf.addFlag("LAr.UseMCShape", True)
    # Include Mphys pulse calibration folder
    lcf.addFlag("LAr.HasMphys", True)
    # Include HVCorr folder in MC
    lcf.addFlag("LAr.HasHVCorr", True)
    # Name of sqlite file containing Electronic Calibration values
    lcf.addFlag("LAr.ElecCalibSqlite", "")
    # Load Electronic Calibration constants
    lcf.addFlag("LAr.LoadElecCalib", True)
    # Folder name for Optimal Filtering coefficients
    lcf.addFlag("LAr.OFCShapeFolder", "")
    # Load conditions with this `run-number' string
    lcf.addFlag("LAr.ForceIOVRunNumber", "")
    # Include Shape folder
    lcf.addFlag("LAr.UseShape", True)
    # Number of samples in LAr digitization + ROD emulation
    lcf.addFlag("LAr.RODnSamples", 5)
    # Index of first sample in LAr digitization + ROD emulation
    lcf.addFlag("LAr.RODFirstSample", 0)
    # DataBase server string
    lcf.addFlag("LAr.DBConnection", "")
    return lcf


_lArRunInfo=None

def _getLArRunInfo(prevFlags):
    global _lArRunInfo #Model-level cache of lar run info
    if _lArRunInfo is None:
        from LArConditionsCommon.LArRunFormat import getLArFormatForRun
        runnbr=prevFlags.Input.RunNumber[0] #If more than one run, assume config for first run is valid for all runs
        dbStr="COOLONL_LAR/"+prevFlags.IOVDb.DatabaseInstance
        _lArRunInfo=getLArFormatForRun(run=runnbr,connstring=dbStr)
        print ("Got LArRunInfo for run ",runnbr)
    return _lArRunInfo
    

def _determineRawChannelSource(prevFlags):
    if (prevFlags.Input.isMC):
        return "input"
    
    lri=_getLArRunInfo(prevFlags)
    #runType: 0=RawData, 1=RawDataResult, 2=Result
    if lri is None or lri.runType is None:
        #Warning ... 
        return "both"
    if (lri.runType==0):
        return "calculated" #Have only digits in bytestream
    elif (lri.runType==1):
        return "both"       #Have both, digits and raw-channels in bytestream
    elif (lri.runType==2):
        return "input"      #Have only raw-channels in bytestream
    else:
        #Warning ... 
        return "both" 
        
