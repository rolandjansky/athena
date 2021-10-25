# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory 
from AthenaConfiguration.MainServicesConfig import MainServicesCfg

def LArPhysWavePredictionCfg(flags):

    #Get basic services and cond-algos
    from LArCalibProcessing.LArCalibBaseConfig import LArCalibBaseCfg
    result=LArCalibBaseCfg(flags)

    from LArCalibProcessing.utils import FolderTagResolver
    FolderTagResolver._globalTag=flags.IOVDb.GlobalTag
    rs=FolderTagResolver()
    CaliWaveTag=rs.getFolderTag(flags.LArCalib.CaliWave.Folder)
    DetCellParamsTag=rs.getFolderTag(flags.LArCalib.DetCellParams.Folder)
    CaliPulseParamsTag=rs.getFolderTag(flags.LArCalib.CaliPulseParams.Folder)
    MphysOverMcalTag=rs.getFolderTag(flags.LArCalib.MPhysOverMCal.Folder)
    PhysWaveTag=rs.getFolderTag(flags.LArCalib.PhysWave.Folder)
    del rs #Close database

    #Retrieve inputs 
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    result.merge(addFolders(flags,flags.LArCalib.CaliWave.Folder,detDb=flags.LArCalib.Input.Database, tag=CaliWaveTag))
    result.merge(addFolders(flags,flags.LArCalib.CaliPulseParams.Folder,detDb=flags.LArCalib.Input.Database, tag=CaliPulseParamsTag))
    result.merge(addFolders(flags,flags.LArCalib.DetCellParams.Folder,detDb=flags.LArCalib.Input.Database, tag=DetCellParamsTag))
    result.merge(addFolders(flags,"/LAR/ElecCalibOfl/Tdrift/Computed",detDb="LAR_OFL",tag="LARElecCalibOflTdriftComputed-calib-03"))
    

    if flags.LArCalib.Input.SubDet == "HEC":
        result.merge(addFolders(flags,"/LAR/ElecCalibOfl/PhysWaves/HECIdeal",detDb="LAR_OFL",db="COMP200",tag="LARElecCalibOflPhysWavesHECIdeal-calib-02"))


    LArPhysWavePredictor = CompFactory.LArPhysWavePredictor( "LArPhysWavePredictor" )
    LArPhysWavePredictor.ProblemsToMask= ["deadCalib","deadReadout","deadPhys","almostDead","short"]
    LArPhysWavePredictor.TestMode      = False
    LArPhysWavePredictor.isSC          = flags.LArCalib.isSC
    LArPhysWavePredictor.KeyCaliList   =  [ "LArCaliWave" ]
    LArPhysWavePredictor.UseCaliPulseParamsFromJO = False
    LArPhysWavePredictor.UseDetCellParamsFromJO   = False
    LArPhysWavePredictor.UseTdriftFromJO          = False
    #LArPhysWavePredictor.Tdrift	              = TdriftVector
    LArPhysWavePredictor.UseDoubleTriangle        = False
    #LArPhysWavePredictor.Tdrift2	              = TdriftVector2
    #LArPhysWavePredictor.WeightTriangle2          = TdriftWeight2
    LArPhysWavePredictor.UseTimeShiftFromJO       = True
    LArPhysWavePredictor.GroupingType             = flags.LArCalib.GroupingType
    LArPhysWavePredictor.NormalizeCali	          = flags.LArCalib.Input.SubDet != "HEC"
    LArPhysWavePredictor.KeyMphysMcali	          = "LArMphysOverMcal"
    LArPhysWavePredictor.DumpMphysMcali           = False # set to True to dump on a ASCII file
    LArPhysWavePredictor.KeyPhys                  = "LArPhysWave"
    LArPhysWavePredictor.isHEC                    = flags.LArCalib.Input.SubDet == "HEC"

    result.addEventAlgo(LArPhysWavePredictor)
    
    if (flags.LArCalib.Input.SubDet == "HEC"):
        LArPhysWaveHECTool=CompFactory.LArPhysWaveHECTool()
        LArPhysWaveHECTool.NormalizeCali     = False  
        LArPhysWaveHECTool.TimeOriginShift   = False
        LArPhysWaveHECTool.SubtractBaseline  = False
        result.addPublicTool(LArPhysWaveHECTool)


    else: #not HEC but EM:
        LArPhysWaveTool=CompFactory.LArPhysWaveTool()
        LArPhysWaveTool.NormalizeCali     = False # this is taken care by LArPhysWavePredictor
        LArPhysWaveTool.TimeOriginShift   = False
        LArPhysWaveTool.SubtractBaseline  = False
        LArPhysWaveTool.InjPointCorrLayer = [ 1, 1, 1, 1 ]
        LArPhysWaveTool.InjPointUseTauR   = [ 1, 1, 1, 1 ]
        result.addPublicTool(LArPhysWaveTool)
   
 
    rootfile=flags.LArCalib.Output.ROOTFile
    if rootfile != "":
      LArPhysWaves2Ntuple              = CompFactory.LArPhysWaves2Ntuple("LArPhysWaves2Ntuple")
      LArPhysWaves2Ntuple.NtupleName   = "PHYSWAVE" 
      LArPhysWaves2Ntuple.AddFEBTempInfo   = False  
      LArPhysWaves2Ntuple.KeyList      = [ "LArPhysWave"  ]
      LArPhysWaves2Ntuple.isSC = flags.LArCalib.isSC
      result.addEventAlgo(LArPhysWaves2Ntuple)

      LArMphysOverMcal2Ntuple                = CompFactory.LArMphysOverMcal2Ntuple( "LArMphysOverMcal2Ntuple" )
      LArMphysOverMcal2Ntuple.ContainerKey   = "LArMphysOverMcal"
      LArMphysOverMcal2Ntuple.AddFEBTempInfo   = False
      LArMphysOverMcal2Ntuple.isSC = flags.LArCalib.isSC
      result.addEventAlgo(LArMphysOverMcal2Ntuple)

      import os
      if os.path.exists(rootfile):
          os.remove(rootfile)
      result.addService(CompFactory.NTupleSvc(Output = [ "FILE1 DATAFILE='"+rootfile+"' OPT='NEW'" ]))
      result.setAppProperty("HistogramPersistency","ROOT")
    pass # end if ROOT ntuple writing
    

    from RegistrationServices.OutputConditionsAlgConfig import OutputConditionsAlgCfg
    result.merge(OutputConditionsAlgCfg(flags,
                                        outputFile=flags.LArCalib.Output.POOLFile,
                                        ObjectList=["LArPhysWaveContainer#LArPhysWave#"+flags.LArCalib.PhysWave.Folder,
                                                    "LArMphysOverMcalComplete#LArMphysOverMcal#"+flags.LArCalib.MPhysOverMCal.Folder,],
                                        IOVTagList=[PhysWaveTag,MphysOverMcalTag]
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



    ConfigFlags.LArCalib.Input.RunNumbers=[401351,]
    ConfigFlags.LArCalib.Input.Database="/home/wlampl/calibTest/00400939_00400943_00400945_Barrel-EMB-EMEC_HIGH_40_21.0.20_1/poolFiles/myDB200_00400939_00400943_00400945_EB-EMBA_one.db_Delay"
    ConfigFlags.LArCalib.Input.SubDet="EM"
    ConfigFlags.LArCalib.BadChannelDB="/home/wlampl/calibTest/00400939_00400943_00400945_Barrel-EMB-EMEC_HIGH_40_21.0.20_1/poolFiles/SnapshotBadChannel_00400939_00400943_00400945_EB-EMBA.db"
    ConfigFlags.LArCalib.BadChannelTag="-RUN2-UPD3-00"
    ConfigFlags.LArCalib.Output.ROOTFile="physwave.root"
    ConfigFlags.IOVDb.DatabaseInstance="CONDBR2"
    ConfigFlags.IOVDb.DBConnection="sqlite://;schema=output.sqlite;dbname=CONDDBR2"
    ConfigFlags.IOVDb.GlobalTag="LARCALIB-RUN2-02"
    #ConfigFlags.Exec.OutputLevel=1

    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(LArPhysWavePredictionCfg(ConfigFlags))

    print("Start running...")
    cfg.run(1)
