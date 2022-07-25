# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.MainServicesConfig import MainEvgenServicesCfg

def HVCorrConfig(flags,outputName="hvcorr"):
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    result=LArGMCfg(flags)
    
    #Use the standard LArHVScaleCfg and adjust properties afterwards as needed 
    from LArCalibUtils.LArHVScaleConfig import LArHVScaleCfg

    result.merge(LArHVScaleCfg(flags))
    result.getCondAlgo("LArHVCondAlg").UndoOnlineHVCorr=False
    result.getCondAlgo("LArHVCondAlg").keyOutputCorr="NewLArHVScaleCorr"


    #The LArHVCorrMaker creates a flat blob in a CondAttrListCollection
    #Input: The HV Scale Correction computed by the LArHVCondAlg based on the DCS HV values
    result.addEventAlgo(CompFactory.LArHVCorrMaker(LArHVScaleCorr="NewLArHVScaleCorr"))


    #Ntuple writing ... 
    from LArCalibProcessing.LArCalib_HVScale2NtupleConfig import LArHVScaleCorr2NtupleCfg
    result.merge(LArHVScaleCorr2NtupleCfg(flags,rootfile=outputName+'.root'))

    #sqlite writing ... 
    from RegistrationServices.OutputConditionsAlgConfig import OutputConditionsAlgCfg
    result.merge(OutputConditionsAlgCfg(flags,
                                        outputFile="dummy.root",
                                        ObjectList=["CondAttrListCollection#/LAR/ElecCalibFlat/HVScaleCorr",],
                                        Run1=flags.Input.RunNumber,
                                        RUN2=0xFFFFFFFF-1
                                    ))
    


    #RegistrationSvc    
    result.addService(CompFactory.IOVRegistrationSvc(RecreateFolders = True,
                                                     SVFolder=True,
                                                     OverrideNames = ["HVScaleCorr"],
                                                     OverrideTypes = ["Blob16M"],
                                                 ))
    result.getService("IOVDbSvc").DBInstance=""

    return result




if __name__=="__main__":
    import sys
    from time import time,strptime
    from calendar import timegm

    if len(sys.argv)<2:
        print("Usage:")
        print("%s <time>" % sys.argv[0])
        sys.exit(-1)
    
    outputName="hvcorr"
    if len(sys.argv)>2:
        outputName=sys.argv[2]


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



    print("---> Working on run",rlb[0],"LB",rlb[1],"Timestamp:",TimeStamp)
    timediff=int(time()-TimeStamp)
    if timediff<0:
        print("ERROR: Timestamp in the future???")
    else:
        (days,remainder)=divmod(timediff,24*60*60)
        (hours,seconds)=divmod(remainder,60*60)
        print ("---> Timestamp is %i days %i hours and %i minutes ago" % (days,hours,int(seconds/60)))
    pass
     

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from LArCalibProcessing.LArCalibConfigFlags import addLArCalibFlags
    addLArCalibFlags(ConfigFlags)

    ConfigFlags.Input.RunNumber=rlb[0]
    ConfigFlags.Input.LumiBlockNumber=rlb[1]
    ConfigFlags.Input.TimeStamp=TimeStamp
    ConfigFlags.Input.Files=[]
    ConfigFlags.IOVDb.DatabaseInstance="CONDBR2"
    ConfigFlags.IOVDb.DBConnection="sqlite://;schema="+outputName+".sqlite;dbname=CONDBR2"
    #ConfigFlags.Exec.OutputLevel=1
    ConfigFlags.lock()
    cfg=MainEvgenServicesCfg(ConfigFlags)
    #First LB not set by McEventSelectorCfg, set it here:
    cfg.getService("EventSelector").FirstLB=ConfigFlags.Input.LumiBlockNumber 
    cfg.merge(HVCorrConfig(ConfigFlags,outputName))
    
    
    print("Start running...")
    cfg.run(1)
