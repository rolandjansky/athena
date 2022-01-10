# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory 
from AthenaConfiguration.MainServicesConfig import MainServicesCfg

def LArRampCfg(flags):

    #Get basic services and cond-algos
    from LArCalibProcessing.LArCalibBaseConfig import LArCalibBaseCfg,chanSelStr
    result=LArCalibBaseCfg(flags)

    #Calibration runs are taken in fixed gain. 
    #The SG key of the digit-container is name of the gain
    gainStrMap={0:"HIGH",1:"MEDIUM",2:"LOW"}
    digKey=gainStrMap[flags.LArCalib.Gain]

    from LArCalibProcessing.utils import FolderTagResolver
    FolderTagResolver._globalTag=flags.IOVDb.GlobalTag
    tagResolver=FolderTagResolver()
    pedestalTag=tagResolver.getFolderTag(flags.LArCalib.Pedestal.Folder)
    caliOFCTag=tagResolver.getFolderTag(flags.LArCalib.OFCCali.Folder)

    rampTag=tagResolver.getFolderTag(flags.LArCalib.Ramp.Folder)
    del tagResolver
    
    print("pedestalTag",pedestalTag)
    print("rampTag",rampTag)


    from IOVDbSvc.IOVDbSvcConfig import addFolders
    result.merge(addFolders(flags,flags.LArCalib.Pedestal.Folder,detDb=flags.LArCalib.Input.Database, tag=pedestalTag,  modifiers=chanSelStr(flags), 
                            className="LArPedestalComplete"))
    result.merge(addFolders(flags,flags.LArCalib.OFCCali.Folder,detDb=flags.LArCalib.Input.Database, tag=caliOFCTag, modifiers=chanSelStr(flags)))
    

    result.addEventAlgo(CompFactory.LArRawCalibDataReadingAlg(LArAccCalibDigitKey=digKey,
                                                              LArFebHeaderKey="LArFebHeader",
                                                              SubCaloPreselection=flags.LArCalib.Input.SubDet,
                                                              PosNegPreselection=flags.LArCalib.Preselection.Side,
                                                              BEPreselection=flags.LArCalib.Preselection.BEC,
                                                              FTNumPreselection=flags.LArCalib.Preselection.FT))
    
    from LArROD.LArFebErrorSummaryMakerConfig import LArFebErrorSummaryMakerCfg
    result.merge(LArFebErrorSummaryMakerCfg(flags))
    result.getEventAlgo("LArFebErrorSummaryMaker").CheckAllFEB=False


    if flags.LArCalib.Input.SubDet == "EM":
        from LArCalibProcessing.LArStripsXtalkCorrConfig import LArStripsXtalkCorrCfg
        result.merge(LArStripsXtalkCorrCfg(flags,[digKey,]))

        theLArCalibShortCorrector = CompFactory.LArCalibShortCorrector(KeyList = [digKey,])
        result.addEventAlgo(theLArCalibShortCorrector)
    pass




    theLArRampBuilder = CompFactory.LArRampBuilder()
    theLArRampBuilder.KeyList      = [digKey,]
    theLArRampBuilder.SubtractDac0 = False
    theLArRampBuilder.ProblemsToMask=["deadCalib","deadReadout","deadPhys","almostDead","short"]

    theLArRampBuilder.RecoType = "OF"
    theLArRampBuilder.PeakOFTool=CompFactory.LArOFPeakRecoTool(UseShape = False,OutputLevel=2)

    theLArRampBuilder.DAC0         = 4294967294
    theLArRampBuilder.StoreRawRamp = True
    theLArRampBuilder.StoreRecRamp = True
    theLArRampBuilder.Polynom      = 1    
    theLArRampBuilder.RampRange    = 3600 # Check on the raw data ADC sample before ped subtraction and pulse reconstruction to include point in fit
    theLArRampBuilder.correctBias  = False
    #theLArRampBuilder.ConsecutiveADCs = 0
    theLArRampBuilder.minDAC = 10      # minimum DAC value to use in fit
    theLArRampBuilder.KeyOutput = "LArRamp"
    theLArRampBuilder.DeadChannelCut = -9999
    theLArRampBuilder.GroupingType = flags.LArCalib.GroupingType
    #theLArRampBuilder.LongNtuple = False

    theLArRampBuilder.isSC = flags.LArCalib.isSC

    if flags.LArCalib.Input.SubDet == "HEC":
        theLArRampBuilder.isHEC = True
        theLArRampBuilder.HECKey = "LArHEC_PAmap"
        result.merge(addFolders(flags,'/LAR/ElecCalibOfl/HecPAMap','LAR_OFL'))
    
    result.addEventAlgo(theLArRampBuilder)


    # Bad-channel patching 
    if flags.LArCalib.CorrectBadChannels:
        LArRampPatcher=CompFactory.getComp("LArCalibPatchingAlg<LArRampComplete>")
        theLArRampPatcher=LArRampPatcher("LArRampPatcher")
        theLArRampPatcher.ContainerKey="LArRamp"
        theLArRampPatcher.PatchMethod="PhiAverage"
   
        theLArRampPatcher.ProblemsToPatch=["deadCalib","deadReadout","deadPhys","almostDead","short"]
        theLArRampPatcher.UseCorrChannels=False      
        result.addEventAlgo(theLArRampPatcher)




    #ROOT ntuple writing:
    rootfile=flags.LArCalib.Output.ROOTFile
    if rootfile != "":
        result.addEventAlgo(CompFactory.LArRamps2Ntuple(ContainerKey = ["LArRamp"+digKey], #for RawRamp
                                                        AddFEBTempInfo = False,
                                                        RawRamp = True,
                                                        SaveAllSamples =  True,
                                                    ))

        import os
        if os.path.exists(rootfile):
            os.remove(rootfile)
        result.addService(CompFactory.NTupleSvc(Output = [ "FILE1 DATAFILE='"+rootfile+"' OPT='NEW'" ]))
        result.setAppProperty("HistogramPersistency","ROOT")
        pass # end if ROOT ntuple writing




    #Output (POOL + sqlite) file writing:
    from RegistrationServices.OutputConditionsAlgConfig import OutputConditionsAlgCfg
    result.merge(OutputConditionsAlgCfg(flags,
                                        outputFile=flags.LArCalib.Output.POOLFile,
                                        ObjectList=["LArRampComplete#LArRamp#"+flags.LArCalib.Ramp.Folder,],
                                        IOVTagList=[rampTag,]
                                    ))

    #RegistrationSvc    
    result.addService(CompFactory.IOVRegistrationSvc(RecreateFolders = False))


    result.getService("IOVDbSvc").DBInstance=""

    return result


if __name__ == "__main__":


    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from LArCalibProcessing.LArCalibConfigFlags import addLArCalibFlags
    addLArCalibFlags(ConfigFlags)


    ConfigFlags.LArCalib.Input.Dir = "/scratch/wlampl/calib21/Sept10"
    ConfigFlags.LArCalib.Input.Type="calibration_LArElec-Ramp"
    ConfigFlags.LArCalib.Input.RunNumbers=[401351,]
    ConfigFlags.LArCalib.Input.Database="db.sqlite"
    ConfigFlags.LArCalib.Input.SubDet="EM"
    ConfigFlags.Input.Files=ConfigFlags.LArCalib.Input.Files
    ConfigFlags.LArCalib.Preselection.BEC=[1]
    ConfigFlags.LArCalib.Preselection.Side=[0]
    ConfigFlags.LArCalib.BadChannelDB="/afs/cern.ch/user/l/larcalib/w0/data/WorkingDirectory/00401338_00401349_00401351_EndCap-EMB-EMEC_HIGH_40_21.0.20_1/poolFiles/SnapshotBadChannel_00401338_00401349_00401351_EB-EMECA.db"
    ConfigFlags.LArCalib.BadChannelTag="-RUN2-UPD3-00"
    ConfigFlags.LArCalib.Output.ROOTFile="larramp.root"

    ConfigFlags.IOVDb.DBConnection="sqlite://;schema=output.sqlite;dbname=CONDBR2"
    ConfigFlags.IOVDb.GlobalTag="LARCALIB-RUN2-02"
    #ConfigFlags.Exec.OutputLevel=1
    print ("Input files to be processed:")
    for f in ConfigFlags.Input.Files:
        print (f)

    ConfigFlags.lock()
    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(LArRampCfg(ConfigFlags))

    print("Start running...")
    cfg.run()
