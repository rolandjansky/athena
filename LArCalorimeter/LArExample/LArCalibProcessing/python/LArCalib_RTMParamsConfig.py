# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory 
from AthenaConfiguration.MainServicesConfig import MainServicesCfg

def LArRTMParamsCfg(flags):

    #Get basic services and cond-algos
    from LArCalibProcessing.LArCalibBaseConfig import LArCalibBaseCfg,chanSelStr
    result=LArCalibBaseCfg(flags)

    from LArCalibProcessing.utils import FolderTagResolver
    FolderTagResolver._globalTag=flags.IOVDb.GlobalTag
    rs=FolderTagResolver()
    DetCellParamsTag=rs.getFolderTag(flags.LArCalib.DetCellParams.Folder)
    CaliPulseParamsTag=rs.getFolderTag(flags.LArCalib.CaliPulseParams.Folder)
    CaliWaveTag=rs.getFolderTag(flags.LArCalib.CaliWave.Folder)
    del rs #Close database

    #Retrieve CaliWave: 
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    result.merge(addFolders(flags,flags.LArCalib.CaliWave.Folder,detDb=flags.LArCalib.Input.Database, tag=CaliWaveTag, modifiers=chanSelStr(flags)))

    if not flags.LArCalib.RTM.ExtractAll:
        #Get information from database:
        result.merge(addFolders(flags,flags.LArCalib.DetCellParams.Folder,detDb=flags.LArCalib.Input.Database, tag=DetCellParamsTag))
        result.merge(addFolders(flags,flags.LArCalib.CaliPulseParams.Folder,detDb=flags.LArCalib.Input.Database, tag=CaliPulseParamsTag))

    
    LArRTMParamExtractor = CompFactory.LArRTMParamExtractor()

    LArRTMParamExtractor.useTBB=True

    LArRTMParamExtractor.KeyList        = ["LArCaliWave"]
    LArRTMParamExtractor.TestMode       = False
    LArRTMParamExtractor.GroupingType   = flags.LArCalib.GroupingType
    LArRTMParamExtractor.IgnoreDACSelection = True
    LArRTMParamExtractor.isSC           = flags.LArCalib.isSC
    
    LArRTMParamExtractor.ExtractTaur    = True
    LArRTMParamExtractor.ExtractTcal    = flags.LArCalib.RTM.ExtractAll
    LArRTMParamExtractor.ExtractFstep   = flags.LArCalib.RTM.ExtractAll
    LArRTMParamExtractor.ExtractOmega0  = flags.LArCalib.RTM.ExtractAll

    LArRTMParamExtractor.FT            = flags.LArCalib.Preselection.FT
    LArRTMParamExtractor.PosNeg        = 0 if len(flags.LArCalib.Preselection.Side)==0 else flags.LArCalib.Preselection.Side[0]
    LArRTMParamExtractor.Slot          = flags.LArCalib.Preselection.Slot

    LArRTMParamExtractor.DumpOmegaScan = flags.LArCalib.RTM.DumpOmegaScan
    LArRTMParamExtractor.DumpResOscill      = flags.LArCalib.RTM.DumpResOscill


    theLArWFParamTool = CompFactory.LArWFParamTool()
    theLArWFParamTool.isSC = flags.LArCalib.isSC

    theLArWFParamTool.UseOmegaScanHelper = True
    theLArWFParamTool.CosRespScan = [ False ,  True , False , False ] # Strips only
    theLArWFParamTool.Omega0Min   = [ 0.100 , 0.220 ,  0.060 ,  0.100 ]
    theLArWFParamTool.Omega0Max   = [ 0.600 , 0.310 ,  0.215 ,  0.700 ]
    theLArWFParamTool.NpointScan  = [   800 ,   900 ,    310 ,    120 ]
    theLArWFParamTool.StoreResOscill = [True * 4] if flags.LArCalib.RTM.DumpResOscill else [False *4]
    
    LArRTMParamExtractor.LArWFParamTool=theLArWFParamTool

    result.addEventAlgo(LArRTMParamExtractor)

    rootfile=flags.LArCalib.Output.ROOTFile
    if rootfile != "":
        LArWFParams2Ntuple = CompFactory.LArWFParams2Ntuple("LArWFParams2Ntuple")
        LArWFParams2Ntuple.DumpCaliPulseParams = True
        LArWFParams2Ntuple.DumpDetCellParams   = True
        LArWFParams2Ntuple.CaliPulseParamsKey="LArCaliPulseParams_RTM"
        LArWFParams2Ntuple.DetCellParamsKey="LArDetCellParams_RTM"
        result.addEventAlgo(LArWFParams2Ntuple)
   
        if flags.LArCalib.RTM.DumpOmegaScan:
            LArOmegaScans2Ntuple = CompFactory.LArCaliWaves2Ntuple("LArOmegaScans2Ntuple")
            LArOmegaScans2Ntuple.NtupleName = "OMEGASCAN"
            LArOmegaScans2Ntuple.KeyList = ["OmegaScan"]
            result.addEventAlgo(LArOmegaScans2Ntuple)

        if ( flags.LArCalib.RTM.DumpResOscill ):
            LArResOscillsBefore2Ntuple = CompFactory.LArCaliWaves2Ntuple("LArResOscillsBefore2Ntuple")
            LArResOscillsBefore2Ntuple.NtupleName = "RESOSCILLBEFORE"
            LArResOscillsBefore2Ntuple.KeyList = ["ResOscillBefore"]
            result.addEventAlgo(LArResOscillsBefore2Ntuple)
            
            LArResOscillsAfter2Ntuple = CompFactory.LArCaliWaves2Ntuple("LArResOscillsAfter2Ntuple")
            LArResOscillsAfter2Ntuple.NtupleName = "RESOSCILLAFTER"
            LArResOscillsAfter2Ntuple.KeyList = ["ResOscillAfter"]
            result.addEventAlgo(LArResOscillsAfter2Ntuple)

        import os
        if os.path.exists(rootfile):
            os.remove(rootfile)
        result.addService(CompFactory.NTupleSvc(Output = [ "FILE1 DATAFILE='"+rootfile+"' OPT='NEW'" ]))
        result.setAppProperty("HistogramPersistency","ROOT")
        pass # end if ROOT ntuple writing



    from RegistrationServices.OutputConditionsAlgConfig import OutputConditionsAlgCfg
    result.merge(OutputConditionsAlgCfg(flags,
                                        outputFile=flags.LArCalib.Output.POOLFile,
                                        ObjectList=["LArCaliPulseParamsComplete#"+"LArCaliPulseParams_RTM#"+flags.LArCalib.CaliPulseParams.Folder,
                                                    "LArDetCellParamsComplete#"+"LArDetCellParams_RTM#"+flags.LArCalib.DetCellParams.Folder,],
                                        IOVTagList=[CaliPulseParamsTag,DetCellParamsTag]
                                    ))

    #RegistrationSvc    
    result.addService(CompFactory.IOVRegistrationSvc(RecreateFolders = False))
    result.getService("IOVDbSvc").DBInstance=""


    #MC Event selector since we have no input data file 
    mcCnvSvc = CompFactory.McCnvSvc()
    cfg.addService(mcCnvSvc)
    cfg.addService(CompFactory.EvtPersistencySvc("EventPersistencySvc",CnvServices=[mcCnvSvc.getFullJobOptName(),]))
    eventSelector=CompFactory.McEventSelector("EventSelector",
                                              RunNumber = flags.LArCalib.Input.RunNumbers[0],
                                              EventsPerRun      = 1,
                                              FirstEvent	       = 0,
                                              InitialTimeStamp  = 0,
                                              TimeStampInterval = 1
                                          )

    result.addService(eventSelector)
    return result

if __name__ == "__main__":


    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from LArCalibProcessing.LArCalibConfigFlags import addLArCalibFlags
    addLArCalibFlags(ConfigFlags)


    ConfigFlags.Input.TypedCollections=[]
    ConfigFlags.LArCalib.Input.RunNumbers=[401351,]
    ConfigFlags.LArCalib.Input.Database="/home/wlampl/calibTest/00400939_00400943_00400945_Barrel-EMB-EMEC_HIGH_40_21.0.20_1/poolFiles/myDB200_00400939_00400943_00400945_EB-EMBA_one.db_Delay"
    ConfigFlags.LArCalib.Input.SubDet="EM"
    ConfigFlags.LArCalib.BadChannelDB="/home/wlampl/calibTest/00400939_00400943_00400945_Barrel-EMB-EMEC_HIGH_40_21.0.20_1/poolFiles/SnapshotBadChannel_00400939_00400943_00400945_EB-EMBA.db"
    ConfigFlags.LArCalib.BadChannelTag="-RUN2-UPD3-00"
    ConfigFlags.LArCalib.Output.ROOTFile="larparams.root"
    ConfigFlags.IOVDb.DatabaseInstance="CONDBR2"
    ConfigFlags.IOVDb.DBConnection="sqlite://;schema=output.sqlite;dbname=CONDBR2"
    ConfigFlags.IOVDb.GlobalTag="LARCALIB-RUN2-02"
    #ConfigFlags.Exec.OutputLevel=1

    ConfigFlags.lock()
    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(LArRTMParamsCfg(ConfigFlags))

    print("Start running...")
    cfg.run(1)
