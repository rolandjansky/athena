# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import os

def AssembleIO(stream="TRIGGERSFTESTER"):    
    #--------------------------------------------------------------
    # Reduce the event loop spam a bit
    #--------------------------------------------------------------
    if os.path.exists("%s/athfile-cache.ascii.gz" % (os.getcwd())):
        print ("INFO: Old athfile-cache found. Will delete it otherwise athena just freaks out. This little boy")
        os.system("rm %s/athfile-cache.ascii.gz" % (os.getcwd()))
    from GaudiSvc.GaudiSvcConf import THistSvc
    import AthenaPoolCnvSvc.ReadAthenaPool  # noqa: F401 (setup POOL reading)
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    from AthenaCommon.AppMgr import theApp, ServiceMgr
    from PathResolver import PathResolver
    ServiceMgr += AthenaEventLoopMgr(EventPrintoutInterval=5000)
   
    ServiceMgr += THistSvc()
    OutFileName = globals().get("outFile", "SFTest.root")
    ServiceMgr.THistSvc.Output += ["%s DATAFILE='%s' OPT='RECREATE'"%(stream,OutFileName)]
    ROOTFiles = []
    
    if "inputFile" in globals():
        print ("Use the following %s as input" % globals()["inputFile"])
        ROOTFiles = []
        ResolvedInFile = PathResolver.FindCalibFile(globals()["inputFile"])
        ReolvedInDir = PathResolver.FindCalibDirectory(globals()["inputFile"])
        if os.path.isfile(ResolvedInFile): 
            print ("INFO: Found file %s" % globals()["inputFile"])
            ROOTFiles.append(ResolvedInFile)
                                             
        elif os.path.isdir(ReolvedInDir):
          for DirEnt in os.listdir(ReolvedInDir):
              if DirEnt.rfind(".root") != -1:
                  if DirEnt.find(ReolvedInDir) != -1 : ROOTFiles.append(DirEnt)
                  else: ROOTFiles.append("%s/%s"%(ReolvedInDir,DirEnt))
        else: raise RuntimeError("Invalid input " + globals()["inputFile"])
        if len(ROOTFiles) == 0: raise RuntimeError("No ROOT files could be loaded as input")
        ServiceMgr.EventSelector.InputCollections = ROOTFiles
        acf.FilesInput = ROOTFiles
       
    if "nevents" in globals():
        print ("Only run on %i events" % globals()["nevents"])
        theApp.EvtMax = int (globals()["nevents"])
    if "nskip" in globals():
        print ("Skip the first %i events" % globals("nskip"))
        ServiceMgr.EventSelector.SkipEvents = int(globals("nskip"))

    
    
def GetTriggerSFTool(MuonWP="Medium", Binning="fine"):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr
    ToolName = "TriggerSFTool_%s_%s"%(MuonWP,Binning)
    if not hasattr(ToolSvc, ToolName):
        from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonTriggerScaleFactors  # noqa: F401
        TriggerTool = CfgMgr.CP__MuonTriggerScaleFactors(ToolName)
        TriggerTool.MuonQuality = MuonWP
        TriggerTool.Binning = Binning
        #TriggerTool.AllowZeroSF = True
        ToolSvc += TriggerTool
    return getattr(ToolSvc,ToolName)

def GetMuonEfficiencyTool(MuonWP="Medium", Release="", CustomInput = "", BreakDownSystematics=False, UncorrelateSystematics=False):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr
    ToolName = "MuonEfficiencyTool_%s%s"%(MuonWP, Release if len(Release) == 0 else "_"+Release)
    if not hasattr(ToolSvc,ToolName):
        from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors  # noqa: F401
        EffiTool = CfgMgr.CP__MuonEfficiencyScaleFactors(ToolName)
        EffiTool.WorkingPoint = MuonWP
        EffiTool.LowPtThreshold = 15.e3
        EffiTool.ApplyKinematicSystematic = True
        EffiTool.BreakDownSystematics = BreakDownSystematics
        EffiTool.UncorrelateSystematics = UncorrelateSystematics
        if len(CustomInput) > 0 : EffiTool.CustomInputFolder = CustomInput
        elif len(Release) > 0: EffiTool.CalibrationRelease = Release
        ToolSvc += EffiTool
    return getattr(ToolSvc,ToolName)

def GetSelectionTool():
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr
    if not hasattr(ToolSvc, "selectionTool"):
        from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool  # noqa: F401
        selTool = CfgMgr.CP__MuonSelectionTool("selectionTool")        
        ToolSvc += selTool
    return getattr(ToolSvc,"selectionTool")

def GetPRWTool(
        # this default is for MC16a -> data2016
        PRWLumiCalcFiles = [
            "GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root",
            "GoodRunsLists/data16_13TeV/20180129/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root",
            "GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root",
            "GoodRunsLists/data18_13TeV/20190318/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root"
        ],
        PRWMCConfigFiles = ["dev/PileupReweighting/mc16_13TeV/pileup_mc16%s_dsid%d_FS.root"%(c,mc) for c in ["a","d","e"] for mc in [410470,301534,361107,345060]  ]
                            ):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr
    if not hasattr(ToolSvc, "prwTool"):
        from PileupReweighting.PileupReweightingConf import CP__PileupReweightingTool  # noqa: F401
        prwTool = CfgMgr.CP__PileupReweightingTool("prwTool")
        prwTool.ConfigFiles = PRWMCConfigFiles
        prwTool.LumiCalcFiles = PRWLumiCalcFiles  
        ToolSvc += prwTool
    return getattr(ToolSvc,"prwTool")

