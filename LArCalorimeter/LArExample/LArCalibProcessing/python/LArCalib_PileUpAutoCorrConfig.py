
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory 
from AthenaConfiguration.MainServicesConfig import MainServicesCfg

def LArPileUpAutoCorrCfg(flags):

    #Get basic services and cond-algos
    from LArCalibProcessing.LArCalibBaseConfig import LArCalibBaseCfg,chanSelStr
    result=LArCalibBaseCfg(flags)

    from IOVDbSvc.IOVDbSvcConfig import addFolders

    #load fsampl, MinBias Average and PulseShape 32 samples from OFLP200
    result.merge(addFolders(flags,"/LAR/ElecCalibMC/Shape",detDb="LAR_OFL", db="OFLP200",tag="LARElecCalibMCShapeLArPileupShape-RUN2-2018",className="LArShape32MC"))
    result.merge(addFolders(flags,"/LAR/ElecCalibMC/fSampl",detDb="LAR_OFL", db="OFLP200",tag="LARElecCalibMCfSampl-G4101-20371-FTFP_BERT_BIRK_v2",className="LArfSamplMC"))
    result.merge(addFolders(flags,"/LAR/ElecCalibMC/MinBias",detDb="LAR_OFL", db="OFLP200",tag="LARElecCalibMCMinBias-mc16-Epos-A3-s3687",className="LArMinBiasMC"))

    result.addCondAlgo(CompFactory.LArMCSymCondAlg())

    LArShapeSymAlg =  CompFactory.getComp("LArSymConditionsAlg<LArShape32MC, LArShape32Sym>")
    result.addCondAlgo(LArShapeSymAlg(ReadKey="LArShape",WriteKey="LArShapeSym"))

    LArfSamplSymAlg =  CompFactory.getComp("LArSymConditionsAlg<LArfSamplMC, LArfSamplSym>")
    result.addCondAlgo(LArfSamplSymAlg(ReadKey="LArfSampl", WriteKey="LArfSamplSym"))

    LArMinBiasSymAlg =  CompFactory.getComp("LArSymConditionsAlg<LArMinBiasMC, LArMinBiasSym>")    
    result.addCondAlgo(LArMinBiasSymAlg(ReadKey="LArMinBias",WriteKey="LArMinBiasSym"))

    from LArCalibProcessing.utils import FolderTagResolver
    FolderTagResolver._globalTag=flags.IOVDb.GlobalTag
    rs=FolderTagResolver()
    AutoCorrTag=rs.getFolderTag(flags.LArCalib.AutoCorr.Folder)
    PhysAutoCorrTag= rs.getFolderTag(flags.LArCalib.PhysAutoCorr.Folder)
    nColl=flags.LArCalib.OFC.Ncoll
    if (nColl>0):
        #Insert mu in tag-name:
        elems=PhysAutoCorrTag.split("-")
        PhysAutoCorrTag="-".join([elems[0]+"_mu_%i"%nColl,]+elems[1:])
        del elems

    del rs

    result.merge(addFolders(flags,flags.LArCalib.AutoCorr.Folder,detDb=flags.LArCalib.Input.Database, tag=AutoCorrTag, modifiers=chanSelStr(flags), 
                            className="LArAutoCorrComplete"))

    #Need ADC2MeV values for AutoCorrCondAlg ... 
    #use current production values as input conditions
    from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
    requiredConditions=["Ramp","DAC2uA","uA2MeV","MphysOverMcal","HVScaleCorr","Pedestal"]
    result.merge(LArElecCalibDbCfg(flags,requiredConditions))
    result.addCondAlgo(CompFactory.LArADC2MeVCondAlg(UseFEBGainTresholds=False))
    theLArAutoCorrTotalCondAlg=CompFactory.LArAutoCorrTotalCondAlg()
    theLArAutoCorrTotalCondAlg.Nminbias=nColl
    theLArAutoCorrTotalCondAlg.Nsamples=flags.LArCalib.OFC.Nsamples
    from AthenaCommon.SystemOfUnits import ns
    theLArAutoCorrTotalCondAlg.deltaBunch=int(flags.Beam.BunchSpacing/( 25.*ns)+0.5)
    theLArAutoCorrTotalCondAlg.isSuperCell=flags.LArCalib.isSC
    theLArAutoCorrTotalCondAlg.isMC=False
    theLArAutoCorrTotalCondAlg.LArAutoCorrObjKey="LArAutoCorr"
    theLArAutoCorrTotalCondAlg.LArAutoCorrTotalObjKey="LArPhysAutoCorr"  
    result.addCondAlgo(theLArAutoCorrTotalCondAlg)
    
    result.addEventAlgo(CompFactory.LArAutoCorrAlgToDB(NMinbias=nColl))

    
    #Ntuple writing
    rootfile=flags.LArCalib.Output.ROOTFile
    if rootfile != "":
        result.addEventAlgo(CompFactory.LArAutoCorr2Ntuple(ContainerKey="LArPhysAutoCorr"))
        import os
        if os.path.exists(rootfile):
            os.remove(rootfile)
        result.addService(CompFactory.NTupleSvc(Output = [ "FILE1 DATAFILE='"+rootfile+"' OPT='NEW'" ]))
        result.setAppProperty("HistogramPersistency","ROOT")
    pass # end if ROOT ntuple writing
    
    

    from RegistrationServices.OutputConditionsAlgConfig import OutputConditionsAlgCfg
    result.merge(OutputConditionsAlgCfg(flags,
                                        outputFile=flags.LArCalib.Output.POOLFile,
                                        ObjectList=["LArAutoCorrComplete#LArPhysAutoCorr#"+flags.LArCalib.PhysAutoCorr.Folder,],
                                        IOVTagList=[PhysAutoCorrTag,]
                                    ))


    #RegistrationSvc    
    result.addService(CompFactory.IOVRegistrationSvc(RecreateFolders = False))
    result.getService("IOVDbSvc").DBInstance=""


    #MC Event selector since we have no input data file 
    mcCnvSvc = CompFactory.McCnvSvc()
    result.addService(mcCnvSvc)
    result.addService(CompFactory.EvtPersistencySvc("EventPersistencySvc",CnvServices=[mcCnvSvc.getFullJobOptName(),]))
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

    ConfigFlags.Input.Files=[]
    ConfigFlags.LArCalib.Input.RunNumbers=[400939,]
    ConfigFlags.LArCalib.Input.Database="/home/wlampl/calibTest/00400939_00400943_00400945_Barrel-EMB-EMEC_HIGH_40_21.0.20_1/poolFiles/myDB200_00400939_00400943_00400945_EB-EMBA_one.db_Delay"
    ConfigFlags.LArCalib.Input.SubDet="EM"
    ConfigFlags.LArCalib.BadChannelDB="/home/wlampl/calibTest/00400939_00400943_00400945_Barrel-EMB-EMEC_HIGH_40_21.0.20_1/poolFiles/SnapshotBadChannel_00400939_00400943_00400945_EB-EMBA.db"
    ConfigFlags.LArCalib.BadChannelTag="-RUN2-UPD3-00"
    ConfigFlags.LArCalib.Output.ROOTFile="physAC.root"
    ConfigFlags.IOVDb.DatabaseInstance="CONDBR2"
    ConfigFlags.IOVDb.DBConnection="sqlite://;schema=output.sqlite;dbname=CONDBR2"
    ConfigFlags.IOVDb.GlobalTag="LARCALIB-RUN2-02"
    #ConfigFlags.Exec.OutputLevel=1
    ConfigFlags.LArCalib.OFC.Ncoll=20
    ConfigFlags.LArCalib.OFC.Nsamples=5
    ConfigFlags.lock()

    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(LArPileUpAutoCorrCfg(ConfigFlags))


    print("Folders:")
    print(cfg.getService("IOVDbSvc").Folders)
    print("ADC2MeV:")
    print(cfg.getCondAlgo("LArADC2MeVCondAlg"))
    print("AutoCorrTotal:")
    print(cfg.getCondAlgo("LArAutoCorrTotalCondAlg"))


    print("Start running...")
    cfg.run(1)
