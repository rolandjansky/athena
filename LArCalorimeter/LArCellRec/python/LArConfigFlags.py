# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

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
    return lcf


_lArRunInfo=None

def _getLArRunInfo(prevFlags):
    global _lArRunInfo #Model-level cache of lar run info
    if _lArRunInfo is None:
        from LArConditionsCommon.LArRunFormat import getLArFormatForRun
        runnbr=prevFlags.Input.RunNumber[0] #If more than one run, assume config for first run is valid for all runs
        dbStr="COOLONL_LAR/"+prevFlags.IOVDb.DatabaseInstance
        _lArRunInfo=getLArFormatForRun(run=runnbr,connstring=dbStr)
        print "Got LArRunInfo for run ",runnbr
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
        
