# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory 
from AthenaConfiguration.MainServicesConfig import MainEvgenServicesCfg

def LArHVScaleCorr2NtupleCfg(flags, rootfile="hvcorr_read.root"):

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    result=LArGMCfg(flags)

    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
    result.merge(LArOnOffIdMappingCfg(flags))

    from LArBadChannelTool.LArBadChannelConfig import LArBadChannelCfg
    result.merge(LArBadChannelCfg(flags))

    from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
    result.merge(LArElecCalibDbCfg(flags,["HVScaleCorr"]))


    theLArHVScaleCorr2Ntuple = CompFactory.LArHVScaleCorr2Ntuple("LArHVScaleCorr2Ntuple")
    theLArHVScaleCorr2Ntuple.AddFEBTempInfo = False
    result.addEventAlgo(theLArHVScaleCorr2Ntuple)

    import os
    if os.path.exists(rootfile):
        os.remove(rootfile)
    result.addService(CompFactory.NTupleSvc(Output = [ "FILE1 DATAFILE='"+rootfile+"' OPT='NEW'" ]))
    result.setAppProperty("HistogramPersistency","ROOT")

    return result

if __name__=="__main__":
    import sys
    from time import strptime
    from calendar import timegm

    if len(sys.argv)<2:
        print("Usage:")
        print("%s <time> <outputfile> <globaltag>" % sys.argv[0])
        sys.exit(-1)
    

    try:
        ts=strptime(sys.argv[1]+'/UTC','%Y-%m-%d:%H:%M:%S/%Z')
        TimeStamp=int(timegm(ts))
        TimeStamp_ns=TimeStamp*1000000000
    except ValueError as e:
        print("ERROR in time specification, use e.g. 2007-05-25:14:01:00")
        print(e)
        sys.exit(-1)

    from LArCalibProcessing.TimeStampToRunLumi import TimeStampToRunLumi
    
    rlb=TimeStampToRunLumi(TimeStamp_ns)
    if rlb is None:
        print("WARNING: Failed to convert time",TimeStamp_ns,"into a run/lumi number")
        sys.exit(-1)


    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from LArCalibProcessing.LArCalibConfigFlags import addLArCalibFlags
    addLArCalibFlags(ConfigFlags)

    ConfigFlags.Input.RunNumber=rlb[0]
    ConfigFlags.Input.TimeStamp=TimeStamp
    ConfigFlags.Input.Files=[]
    ConfigFlags.IOVDb.DatabaseInstance="CONDBR2"

    rootfile="hvcorr_read.root"
    if len(sys.argv)>2:
        rootFile=sys.argv[2]

    if len(sys.argv)>3:
        ConfigFlags.IOVDb.GlobalTag=sys.argv[3]
        
    ConfigFlags.lock()
    cfg=MainEvgenServicesCfg(ConfigFlags)
    cfg.merge(LArHVScaleCorr2NtupleCfg(ConfigFlags))
    
    
    print("Start running...")
    cfg.run(1)
