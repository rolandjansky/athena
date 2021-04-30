#Author: Will Buttinger

#This is the configuration file for an athena job using SUSYTools to calibrate physics objects
#You would use this joboption by copying it and substituting the TestAlg for your own algorithm
#and subtituting your own input files

import AthenaPoolCnvSvc.ReadAthenaPool #read xAOD files

theApp.EvtMax = 400 #set to -1 to run on all events

### This JO uses MC16a by default, but if you specify MCCampaign (with -c "MCCampaign= ...) you can
### swap the MC campaign this is run for (used in ART tests)
try:
    MCCampaign
except:
    MCCampaign = 'MCe'

if MCCampaign == 'MCa':
    inputFile = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SUSYTools/DAOD_PHYSVAL.mc16_13TeV.410470.FS_mc16a_p4237.PHYSVAL.pool.root'
elif MCCampaign == 'MCd':
    inputFile = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SUSYTools/DAOD_PHYSVAL.mc16_13TeV.410470.FS_mc16d_p4237.PHYSVAL.pool.root'
else: # MC16e by default
    inputFile = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SUSYTools/DAOD_PHYSVAL.mc16_13TeV.410470.FS_mc16e_p4237.PHYSVAL.pool.root'

svcMgr.EventSelector.InputCollections = [ inputFile ] #specify input files here, takes a list
svcMgr.MessageSvc.OutputLevel = INFO 

ToolSvc += CfgMgr.ST__SUSYObjDef_xAOD("SUSYTools")

ToolSvc.SUSYTools.ConfigFile = "SUSYTools/SUSYTools_Default.conf" # Grab the default config file

ToolSvc.SUSYTools.AutoconfigurePRWTool = True
if MCCampaign == 'MCa':
   ToolSvc.SUSYTools.PRWLumiCalcFiles = [
      "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20180129/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root",
      "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root"
   ]
elif MCCampaign == 'MCd':
    ToolSvc.SUSYTools.PRWLumiCalcFiles = [ "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root" ]
else: #default MCe
    ToolSvc.SUSYTools.PRWLumiCalcFiles = [ "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190318/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root" ]

algseq = CfgMgr.AthSequencer("AthAlgSeq") #The main alg sequence

#this bit is used in the ATN nightly tests, for appending event rates to previous results
try:
    from TrigValTools.Utils import getPreviousNightlyPath
    myPath = getPreviousNightlyPath()
except ImportError:
    myPath="."

algseq += CfgMgr.SUSYToolsAlg("MCAlg",RootStreamName="MYSTREAM",RateMonitoringPath=myPath,CheckTruthJets=True) #Substitute your alg here

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
algseq.MCAlg.OutputLevel = INFO 


svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M" #Creates more space for displaying tool names
svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=100) #message every 100 events processed

#this is for the output of histograms
svcMgr+=CfgMgr.THistSvc()
svcMgr.THistSvc.Output += [ "MYSTREAM DATAFILE='monitoring.%s.root' OPT='RECREATE'" % MCCampaign ]
