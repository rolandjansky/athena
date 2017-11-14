# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import os

def AssembleIO(stream="TRIGGERSFTESTER"):    
    #--------------------------------------------------------------
    # Reduce the event loop spam a bit
    #--------------------------------------------------------------
    if os.path.exists("%s/athfile-cache.ascii.gz" % (os.getcwd())):
        print "INFO: Old athfile-cache found. Will delete it otherwise athena just freaks out. This little boy"
        os.system("rm %s/athfile-cache.ascii.gz" % (os.getcwd()))
    from GaudiSvc.GaudiSvcConf import THistSvc
    from AthenaCommon.JobProperties import jobproperties
    import AthenaPoolCnvSvc.ReadAthenaPool
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    from AthenaCommon.AppMgr import ServiceMgr
    from PathResolver import PathResolver
    ServiceMgr += AthenaEventLoopMgr(EventPrintoutInterval=5000)
   
    ServiceMgr += THistSvc()
    OutFileName = "SFTest.root" if not "outFile" in globals() else outFile
    ServiceMgr.THistSvc.Output += ["%s DATAFILE='%s' OPT='RECREATE'"%(stream,OutFileName)]
    ROOTFiles = []
    
    if "inputFile" in globals():
        print "Use the following %s as input" % (inputFile)
        ROOTFiles = []
        ResolvedInFile = PathResolver.FindCalibFile(inputFile)
        ReolvedInDir = PathResolver.FindCalibDirectory(inputFile)
        if os.path.isfile(ResolvedInFile): 
            print "INFO: Found file %s"%(inputFile)
            ROOTFiles.append(ResolvedInFile)
                                             
        elif os.path.isdir(ReolvedInDir):
          for DirEnt in os.listdir(ReolvedInDir):
              if DirEnt.rfind(".root") != -1:
                  if DirEnt.find(ReolvedInDir) != -1 : ROOTFiles.append(DirEnt)
                  else: ROOTFiles.append("%s/%s"%(ReolvedInDir,DirEnt))
        else: raise RuntimeError("Invalid input " + inputFile)
        if len(ROOTFiles) == 0: raise RuntimeError("No ROOT files could be loaded as input")
        ServiceMgr.EventSelector.InputCollections = ROOTFiles
        acf.FilesInput = ROOTFiles
       
    if "nevents" in globals():
        print "Only run on %i events" % (int(nevents))
        theApp.EvtMax = int (nevents)
    if "nskip" in globals():
        print "Skip the first %i events" % (int(nskip))
        ServiceMgr.EventSelector.SkipEvents = int(nksip)

    
    
def GetTriggerSFTool(MuonWP="Medium", Binning="fine"):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr, GlobalFlags
    ToolName = "TriggerSFTool_%s_%s"%(MuonWP,Binning)
    if not hasattr(ToolSvc, ToolName):
        from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonTriggerScaleFactors
        TriggerTool = CfgMgr.CP__MuonTriggerScaleFactors(ToolName)
        TriggerTool.MuonQuality = MuonWP
        TriggerTool.Binning = Binning
        #TriggerTool.AllowZeroSF = True
        ToolSvc += TriggerTool
    return getattr(ToolSvc,ToolName)

def GetMuonEfficiencyTool(MuonWP="Medium", Release=""):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr, GlobalFlags
    ToolName = "MuonEfficiencyTool_%s%s"%(MuonWP, Release if len(Release) == 0 else "_"+Release)
    if not hasattr(ToolSvc,ToolName):
        from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors
        EffiTool = CfgMgr.CP__MuonEfficiencyScaleFactors(ToolName)
        EffiTool.WorkingPoint = MuonWP
        EffiTool.LowPtThreshold = 15.e3
        if len(Release) > 0: EffiTool.CalibrationRelease = Release
        ToolSvc += EffiTool
    return getattr(ToolSvc,ToolName)

def GetPRWTool(
        PRWLumiCalcFiles = [
#            "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_OflLumi-13TeV-009_data15_13TeV.periodAllYear_DetStatus-v89-pro21-02_Unknown_PHYS_StandardGRL_All_Good_25ns.root",
#            "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_OflLumi-13TeV-009_data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.root",
            "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_OflLumi-13TeV-001_data17_13TeV.periodAllYear_DetStatus-v96-pro21-12_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.root"
			    ],
        PRWMCConfigFiles = ['dev/SUSYTools/merged_prw_mc16a_latest.root',
                            'dev/SUSYTools/mc16a_defaults_buggy.NotRecommended.prw.root']):
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr, GlobalFlags
    if not hasattr(ToolSvc, "prwTool"):
        from PileupReweighting.PileupReweightingConf import CP__PileupReweightingTool
        prwTool = CfgMgr.CP__PileupReweightingTool("prwTool")
        prwTool.ConfigFiles = PRWMCConfigFiles
        prwTool.LumiCalcFiles = PRWLumiCalcFiles  
        prwTool.UnrepresentedDataThreshold = 0.5      
        ToolSvc += prwTool
    return getattr(ToolSvc,"prwTool")

