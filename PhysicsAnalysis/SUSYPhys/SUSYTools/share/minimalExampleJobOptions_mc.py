#Author: Will Buttinger

#This is the configuration file for an athena job using SUSYTools to calibrate physics objects
#You would use this joboption by copying it and substituting the TestAlg for your own algorithm
#and subtituting your own input files


import AthenaPoolCnvSvc.ReadAthenaPool #read xAOD files


theApp.EvtMax = 400 #set to -1 to run on all events

inputFile = os.environ['ASG_TEST_FILE_MC'] #test input file
svcMgr.EventSelector.InputCollections = [ inputFile ] #specify input files here, takes a list

from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
AST99TruthClassifier = MCTruthClassifier(name = "AST99TruthClassifier")
ToolSvc += AST99TruthClassifier
AST99tauTruthTool = CfgMgr.TauAnalysisTools__TauTruthMatchingTool(
                                        name = "AST99TauTruthMatchingTool",
                              WriteTruthTaus = True,
                                 OutputLevel = INFO,
                       MCTruthClassifierTool = AST99TruthClassifier )
ToolSvc += AST99tauTruthTool

ToolSvc += CfgMgr.ST__SUSYObjDef_xAOD("SUSYTools")

ToolSvc.SUSYTools.ConfigFile = "SUSYTools/SUSYTools_Default.conf" #look in the data directory of SUSYTools for other config files
ToolSvc.SUSYTools.PRWConfigFiles = [
    "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root", 
    "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root"
    ]
ToolSvc.SUSYTools.PRWLumiCalcFiles = [
    "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20160720/physics_25ns_20.7.lumicalc.OflLumi-13TeV-005.root",
    "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20160803/physics_25ns_20.7.lumicalc.OflLumi-13TeV-005.root"
    ]

algseq = CfgMgr.AthSequencer("AthAlgSeq") #The main alg sequence

#this bit is used in the ATN nightly tests, for appending event rates to previous results
try:
    from TrigValTools.Utils import getPreviousNightlyPath
    myPath = getPreviousNightlyPath()
except ImportError:
    myPath="."

algseq += CfgMgr.SUSYToolsAlg("MCAlg",RootStreamName="MYSTREAM",RateMonitoringPath=myPath,TauTruthMatchingTool=AST99tauTruthTool,CheckTruthJets=True) #Substitute your alg here

#You algorithm can use the SUSYTools through a ToolHandle:
#
#ToolHandle<ST::ISUSYObjDef_xAODTool> m_SUSYTools;
#
#If you declare the ToolHandle as a property (like in SUSYToolsAlg):
#
#  declareProperty( "SUSYTools",   m_SUSYTools      );
#
#You can then do something like the following:
algseq.MCAlg.SUSYTools = ToolSvc.SUSYTools

#That completes the minimum configuration. The rest is extra....
algseq.MCAlg.DoSyst = True


svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M" #Creates more space for displaying tool names
svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=100) #message every 100 events processed

#this is for the output of histograms
svcMgr+=CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["MYSTREAM DATAFILE='monitoring.mc.root' OPT='RECREATE'"]

