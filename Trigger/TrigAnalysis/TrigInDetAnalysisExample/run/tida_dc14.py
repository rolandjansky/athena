#Example jO with autoconfiguration. Runs truth on MC and offline on data
#Taken from version in https://twiki.cern.ch/twiki/bin/viewauth/Atlas/JiveXMLWithAODorESD#Autoconfiguration_e_g_with_mc09
## RTT jOs run in RecExCommon environment
## (RecExCommon_links.sh), but this doesn't work
## with Cosmics, need to remove static db link
## with shell command:
import os
#### remove db link:
os.system('rm -rf sqlite200')
#remove Pool catalogue
os.system('rm -f PoolFileCatalog.xml*')
##
from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

athenaCommonFlags.FilesInput=[
#"ESD.01529329._000155.pool.root.1",
#"valid1.117050.PowhegPythia_P2011C_ttbar.recon.ESD.e2658_s1967_s1964_r5729_tid01529329_00/ESD.01529329._000155.pool.root.1",
#"AOD.01529329._000005.pool.root.1",
"valid2.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5748_tid01564760_00/AOD.01564760._000001.pool.root.1",
#"valid2.117050.PowhegPythia_P2011C_ttbar.recon.ESD.e2658_s1967_s1964_r5748_tid01564760_00/ESD.01564760._000111.pool.root.1",
#"AOD.01564760._000001.pool.root.1",
#"valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5729_tid01529329_00/AOD.01529329._000169.pool.root.1",
#"valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5729_tid01529329_00/AOD.01529329._000077.pool.root.1",
#"valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5729_tid01529329_00/AOD.01529329._000127.pool.root.1",
#"valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5729_tid01529329_00/AOD.01529329._000036.pool.root.1",
#"valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5729_tid01529329_00/AOD.01529329._000159.pool.root.1",
]

#PdgId=15
dbglevel=INFO

### all events:
#athenaCommonFlags.EvtMax.set_Value_and_Lock(-1)
### for test, some events:
athenaCommonFlags.EvtMax.set_Value_and_Lock(15)

#rec.doJiveXML.set_Value_and_Lock(True)
rec.doCBNT.set_Value_and_Lock(False)
rec.doMonitoring.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)
#If you don't want truth in MC
#rec.doTruth.set_Value_and_Lock(False)
### comment-in the following line for
### pathena-setup to get single-event-ESD:
#rec.doWriteESD.set_Value_and_Lock(True)

include("RecExCommon/RecExCommon_topOptions.py")
##
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


##### Trigger Decision Part #####################

## set up trigger decision tool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
tdt.OutputLevel = dbglevel


##### Histogram File Part #################

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager

############ TrigInDetAnalysis part ################################

from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestMonToolAC
TestMonTool = TrigTestMonToolAC( name="TestMonToolAC")
TestMonTool.OutputLevel = dbglevel
TestMonTool.buildNtuple = True
TestMonTool.AnalysisConfig = "nTuple" #Change to Tier0 for T0 Analysis
TestMonTool.EnableLumi = False
if (rec.doTruth == True):
	TestMonTool.mcTruth = True
	TestMonTool.ntupleChainNames = ['Truth']

else:
	TestMonTool.mcTruth = False
	TestMonTool.ntupleChainNames = ['']

#Tau 3-prong - false by default
#TestMonTool.doTauThreeProng = True
#Tau Et cut
#TestMonTool.tauEtCutOffline= True


TestMonTool.ntupleChainNames += [
       "Offline",
#  "Muons",
#  "Electrons",
#  "Taus",
#  "Taus3",
  #egamma
#  "L2_e22vh_medium_IDTrkNoCut:TrigIDSCAN_eGamma",
#  "L2_e22vh_medium_IDTrkNoCut:TrigSiTrack_eGamma",
#  "EF_e22vh_medium_IDTrkNoCut:InDetTrigParticleCreation_Electron_EFID",
#"HLT_e24_medium_idperf:TrigFastTrackFinder_eGamma",
#"HLT_e24_medium_idperf:InDetTrigParticleCreation_Electron_EFID",
#"HLT_e24_medium_idperf:InDetTrigTrackingxAODCnv_Electron_EFID",
#"HLT_e5_loose_idperf:TrigFastTrackFinder_eGamma",
#"HLT_e5_loose_idperf:InDetTrigParticleCreation_Electron_EFID",
#"HLT_e5_loose_idperf:InDetTrigTrackingxAODCnv_Electron_EFID",
"HLT_e24_medium1_idperf:InDetTrigTrackingxAODCnv_Electron_FTF",
"HLT_e24_medium1_idperf:InDetTrigTrackingxAODCnv_Electron_EFID",
"HLT_e5_loose1_idperf:InDetTrigTrackingxAODCnv_Electron_FTF",
"HLT_e5_loose1_idperf:InDetTrigTrackingxAODCnv_Electron_EFID",




  #mu
#  "L2_mu20_IDTrkNoCut:TrigIDSCAN_Muon",
#  "L2_mu20_IDTrkNoCut:TrigSiTrack_Muon",
#  "EF_mu20_IDTrkNoCut:InDetTrigParticleCreation_Muon_EFID",
   #    "HLT_mu24_idperf:TrigFastTrackFinder_Muon",
   #    "HLT_mu24_idperf:InDetTrigParticleCreation_Muon_EFID",
   #    "HLT_mu24_idperf:InDetTrigTrackingxAODCnv_Muon_EFID",
   #    "HLT_mu6_idperf:TrigFastTrackFinder_Muon",
   #    "HLT_mu6_idperf:InDetTrigParticleCreation_Muon_EFID",
   #    "HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_EFID",
"HLT_mu24_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
"HLT_mu24_idperf:InDetTrigTrackingxAODCnv_Muon_EFID",
"HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
"HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_EFID",


  #tau
#  "L2_tau29_IDTrkNoCut:TrigIDSCAN_Tau",
#  "L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:1",
#  "L2_tau29_IDTrkNoCut:TrigSiTrack_Tau_robust",
#  "L2_tau125_IDTrkNoCut:TrigIDSCAN_Tau",
#  "L2_tau125_IDTrkNoCut:TrigSiTrack_Tau",
#  "L2_tau125_IDTrkNoCut:TrigSiTrack_Tau_robust",
#  "EF_tau29_IDTrkNoCut:InDetTrigParticleCreation_Tau_EFID",
#  "EF_tau125_IDTrkNoCut:InDetTrigParticleCreation_Tau_EFID",
  #  "HLT_tau29_idperf:TrigFastTrackFinder_Tau",
  #  "HLT_tau29_idperf:InDetTrigParticleCreation_Tau_EFID",
  #  "HLT_tau29_idperf:InDetTrigTrackingxAODCnv_Tau_EFID",
    "HLT_tau29_idperf:InDetTrigTrackingxAODCnv_Tau_FTF",
    "HLT_tau29_idperf:InDetTrigTrackingxAODCnv_Tau_EFID",
  #bjet
#  "L2_b10_IDTrkNoCut:TrigIDSCAN_Jet",
#  "L2_b10_IDTrkNoCut:TrigSiTrack_Jet",
#  "EF_b10_IDTrkNoCut:InDetTrigParticleCreation_Bjet_EFID",
       'HLT_j55_bperf:TrigFastTrackFinder_Jet',
       'HLT_j55_bperf:InDetTrigParticleCreation_Bjet_EFID',
       'HLT_j55_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID',
       'HLT_j55_EFID_bperf:InDetTrigParticleCreation_Bjet_EFID',
       'HLT_j55_EFID_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID',

]

TestMonTool.outputFileName="TrkNtuple.root"
ToolSvc += TestMonTool
HLTMonManager.AthenaMonTools += [ "TrigTestMonToolAC/TestMonToolAC" ]

print TestMonTool

if(TestMonTool.AnalysisConfig == "Tier0"):
	from GaudiSvc.GaudiSvcConf import THistSvc
	ServiceMgr += THistSvc()
	ServiceMgr.THistSvc.Output = ["AANT DATAFILE='TIDA_T0.root' OPT='RECREATE'"]
	HLTMonManager.FileKey = "AANT"



# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = INFO


#
## Number of Events to process
##theApp.EvtMax = 5
MessageSvc.infoLimit  = 1000000
MessageSvc.debugLimit = 1000000


#svcMgr.AuditorSvc.Auditors += [ "ChronoAuditor"]
