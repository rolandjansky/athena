#Author: Will Buttinger

#This is the configuration file for an athena job using SUSYTools to calibrate physics objects
#You would use this joboption by copying it and substituting the TestAlg for your own algorithm
#and subtituting your own input files

import AthenaPoolCnvSvc.ReadAthenaPool #read xAOD files

theApp.EvtMax = 400 #set to -1 to run on all events

inputFile = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SUSYTools/DAOD_PHYSVAL.mc16_13TeV.410470.AFII_mc16e_p4237.PHYSVAL.pool.root'

svcMgr.EventSelector.InputCollections = [ inputFile ] #specify input files here, takes a list
svcMgr.MessageSvc.OutputLevel = INFO 

ToolSvc += CfgMgr.ST__SUSYObjDef_xAOD("SUSYTools")

ToolSvc.SUSYTools.ConfigFile = "SUSYTools/SUSYTools_Default.conf" # Grab the default config file

ToolSvc.SUSYTools.AutoconfigurePRWTool = True
ToolSvc.SUSYTools.PRWLumiCalcFiles = [ "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190318/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root" ]

ToolSvc.SUSYTools.DataSource = 2  #configure to run on AFII sim

algseq = CfgMgr.AthSequencer("AthAlgSeq") #The main alg sequence

#this bit is used in the ATN nightly tests, for appending event rates to previous results
try:
    from TrigValTools.Utils import getPreviousNightlyPath
    myPath = getPreviousNightlyPath()
except ImportError:
    myPath="."

algseq += CfgMgr.SUSYToolsAlg("AtlFastAlg",RootStreamName="MYSTREAM",RateMonitoringPath=myPath,CheckTruthJets=True) #Substitute your alg here

#You algorithm can use the SUSYTools through a ToolHandle:
#
#ToolHandle<ST::ISUSYObjDef_xAODTool> m_SUSYTools;
#
#If you declare the ToolHandle as a property (like in SUSYToolsAlg):
#
#  declareProperty( "SUSYTools",   m_SUSYTools      );
#
#You can then do something like the following:
algseq.AtlFastAlg.SUSYTools = ToolSvc.SUSYTools

#That completes the minimum configuration. The rest is extra....
algseq.AtlFastAlg.DoSyst = True
algseq.AtlFastAlg.OutputLevel = INFO 

svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M" #Creates more space for displaying tool names
svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=100) #message every 100 events processed

#this is for the output of histograms
svcMgr+=CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["MYSTREAM DATAFILE='monitoring.atlfast.root' OPT='RECREATE'"]
