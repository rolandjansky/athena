# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import os

def AssembleIO():    
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
    OutFileName = "TriggerSFTest.root" if not "outFile" in globals() else outFile
    ServiceMgr.THistSvc.Output += ["TRIGGERSFTESTER DATAFILE='{}' OPT='RECREATE'".format(OutFileName)]
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

def GetPRWTool(
        PRWLumiCalcFiles = [
            "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_OflLumi-13TeV-009_data15_13TeV.periodAllYear_DetStatus-v89-pro21-02_Unknown_PHYS_StandardGRL_All_Good_25ns.root",
            "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_OflLumi-13TeV-009_data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.root",
#            "/afs/cern.ch/atlas/project/muon/mcp/PRWFiles/ilumicalc_histograms_OflLumi-13TeV-001_data17_13TeV.periodAllYear_DetStatus-v90-pro21-03_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.root"
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
        ToolSvc += prwTool
    return getattr(ToolSvc,"prwTool")


#------------------------------------------------------------------------------
def getMuonTriggerSFTester(input_name, triggers=[], MuonWP="Medium", Binning = "fine"):
    from AthenaCommon.AlgSequence import AlgSequence
    algSeq = AlgSequence()
    algName ="MuTriggerSFTester_%s_%s"%(MuonWP,Binning)
    if not hasattr(algSeq, algName):
        alg = CfgMgr.Trig__TestTrigSF(algName)
        alg.inputContainerName = input_name
        alg.prwTool = GetPRWTool() 
        alg.TriggerEfficiencyTool = GetTriggerSFTool(MuonWP,Binning)
        alg.triggers = triggers
        alg.outputStream   = "%s_%s"%(MuonWP, Binning)
        algSeq +=  alg
    return getattr(algSeq,algName)


#######################################################################
#		Setup the Tools
#######################################################################
#
#athena -c "inputFile='/ptmp/mpp/zenon/DirectStau/mc16_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.deriv.DAOD_SUSY3.e3601_s3126_r9364_r9315_p3179/DAOD_SUSY3.11544886._000178.pool.root.1'" MuonEfficiencyCorrections/TriggerSF_Tester_jobOptions.py
#

#trigs = [
        #"HLT_mu20_iloose_L1MU15_OR_HLT_mu40",
        #"HLT_2mu10",
        #"HLT_mu18_mu8noL1",
        #"HLT_mu24_imedium_OR_HLT_mu40",
        #"HLT_mu24_imedium_OR_HLT_mu50",
        #"HLT_mu24_ivarmedium_OR_HLT_mu40", 
        #"HLT_mu24_ivarmedium_OR_HLT_mu50",
        #"HLT_mu26_ivarmedium_OR_HLT_mu50",
        #"HLT_2mu14",
        #"HLT_mu20_mu8noL1",
        #"HLT_mu22_mu8noL1",
        #]

trigs = [
            #'HLT_mu10',
            #'HLT_mu10_msonly',
            #'HLT_mu14',
            #'HLT_mu18',
            #'HLT_mu20',
            #'HLT_mu20_iloose_L1MU15',
            #'HLT_mu20_L1MU15',
            #'HLT_mu20_msonly',
            #'HLT_mu22',
            #'HLT_mu24',
            #'HLT_mu24_iloose_L1MU15',
            #'HLT_mu24_imedium',
            #'HLT_mu24_L1MU15',
            #'HLT_mu26',
            #'HLT_mu26_imedium',
            #'HLT_mu26_ivarmedium',
            #'HLT_mu24_ivarmedium',
            #'HLT_mu40',
            #'HLT_mu4',
            #'HLT_mu4_msonly',
            'HLT_mu50',
            #'HLT_mu6',
            #'HLT_mu6_msonly',
            ]


AssembleIO()

getMuonTriggerSFTester("Muons" , trigs)


