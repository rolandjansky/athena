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
#"valid2.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5748_tid01564760_00/AOD.01564760._000001.pool.root.1",
#"valid2.117050.PowhegPythia_P2011C_ttbar.recon.ESD.e2658_s1967_s1964_r5748_tid01564760_00/ESD.01564760._000111.pool.root.1",
#"AOD.01564760._000001.pool.root.1",
#"valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5729_tid01529329_00/AOD.01529329._000169.pool.root.1",
#"valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5729_tid01529329_00/AOD.01529329._000077.pool.root.1",
#"valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5729_tid01529329_00/AOD.01529329._000127.pool.root.1",
#"valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5729_tid01529329_00/AOD.01529329._000036.pool.root.1",
#"valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5729_tid01529329_00/AOD.01529329._000159.pool.root.1",
#"data15_13TeV.00270953.physics_Main.merge.AOD.f611_m1463._lb0222._0001.1"
"root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_13TeV/87/79/AOD.06065951._000001.pool.root.1",
"root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_13TeV/db/08/AOD.06065951._000002.pool.root.1",
"root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_13TeV/7f/ec/AOD.06065951._000003.pool.root.1",
"root://eosatlas//eos/atlas/atlasdatadisk/rucio/data15_13TeV/e9/4e/AOD.06065951._000004.pool.root.1",
]

#PdgId=15
dbglevel=INFO
#dbglevel=DEBUG

### all events:
athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)
athenaCommonFlags.SkipEvents.set_Value_and_Lock(SkipEvents)
### for test, some events:
#athenaCommonFlags.EvtMax.set_Value_and_Lock(5)

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

from InDetBeamSpotService.InDetBeamSpotServiceConf import BeamCondSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += BeamCondSvc(name="InDetBeamSpotOnline")

#Tau 3-prong - false by default
#TestMonTool.doTauThreeProng = True
#Tau Et cut
#TestMonTool.tauEtCutOffline= True


TestMonTool.ntupleChainNames += [
"Offline",
#"Muons",
#"Electrons",
#"Electrons_TightLH",
#"Electrons_MediumLH",
#"Electrons_LooseLH",
#"Taus",
#"Taus3",


  #egamma
#"HLT_e24_medium1_idperf:InDetTrigTrackingxAODCnv_Electron_FTF",
#"HLT_e24_medium1_idperf:InDetTrigTrackingxAODCnv_Electron_EFID",
#"HLT_e24_medium1_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig",
"HLT_e24_lhmedium_idperf_L1EM20VH:InDetTrigTrackingxAODCnv_Electron_FTF",
"HLT_e24_lhmedium_idperf_L1EM20VH:InDetTrigTrackingxAODCnv_Electron_EFID",
"HLT_e24_lhmedium_idperf_L1EM20VH:InDetTrigTrackingxAODCnv_Electron_IDTrig",
"HLT_e24_medium1_idperf_L1EM18VH:InDetTrigTrackingxAODCnv_Electron_FTF",
"HLT_e24_medium1_idperf_L1EM18VH:InDetTrigTrackingxAODCnv_Electron_EFID",
"HLT_e24_medium1_idperf_L1EM18VH:InDetTrigTrackingxAODCnv_Electron_IDTrig",
"HLT_e24_medium_idperf_L1EM20VH:InDetTrigTrackingxAODCnv_Electron_FTF",
"HLT_e24_medium_idperf_L1EM20VH:InDetTrigTrackingxAODCnv_Electron_EFID",
"HLT_e24_medium_idperf_L1EM20VH:InDetTrigTrackingxAODCnv_Electron_IDTrig",
"HLT_e5_lhloose_idperf:InDetTrigTrackingxAODCnv_Electron_FTF",
"HLT_e5_lhloose_idperf:InDetTrigTrackingxAODCnv_Electron_EFID",
"HLT_e5_lhloose_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig",
"HLT_e5_loose1_idperf:InDetTrigTrackingxAODCnv_Electron_FTF",
"HLT_e5_loose1_idperf:InDetTrigTrackingxAODCnv_Electron_EFID",
"HLT_e5_loose1_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig",
"HLT_e5_loose_idperf:InDetTrigTrackingxAODCnv_Electron_FTF",
"HLT_e5_loose_idperf:InDetTrigTrackingxAODCnv_Electron_EFID",
"HLT_e5_loose_idperf:InDetTrigTrackingxAODCnv_Electron_IDTrig",


  #mu
"HLT_mu10_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
"HLT_mu10_idperf:InDetTrigTrackingxAODCnv_Muon_EFID",
"HLT_mu10_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig",
"HLT_mu13_mu13_idperf_Zmumu:InDetTrigTrackingxAODCnv_Muon_FTF",
"HLT_mu13_mu13_idperf_Zmumu:InDetTrigTrackingxAODCnv_Muon_EFID",
"HLT_mu13_mu13_idperf_Zmumu:InDetTrigTrackingxAODCnv_Muon_IDTrig",
"HLT_mu20_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
"HLT_mu20_idperf:InDetTrigTrackingxAODCnv_Muon_EFID",
"HLT_mu20_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig",
"HLT_mu24_L2Star_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
"HLT_mu24_L2Star_idperf:InDetTrigTrackingxAODCnv_Muon_EFID",
"HLT_mu24_L2Star_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig",
"HLT_mu24_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
"HLT_mu24_idperf:InDetTrigTrackingxAODCnv_Muon_EFID",
"HLT_mu24_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig",
"HLT_mu4_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
"HLT_mu4_idperf:InDetTrigTrackingxAODCnv_Muon_EFID",
"HLT_mu4_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig",
"HLT_mu4_mu4_idperf_bJpsimumu_noid:InDetTrigTrackingxAODCnv_Muon_FTF",
"HLT_mu4_mu4_idperf_bJpsimumu_noid:InDetTrigTrackingxAODCnv_Muon_EFID",
"HLT_mu4_mu4_idperf_bJpsimumu_noid:InDetTrigTrackingxAODCnv_Muon_IDTrig",
"HLT_mu6_L2Star_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
"HLT_mu6_L2Star_idperf:InDetTrigTrackingxAODCnv_Muon_EFID",
"HLT_mu6_L2Star_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig",
"HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_FTF",
"HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_EFID",
"HLT_mu6_idperf:InDetTrigTrackingxAODCnv_Muon_IDTrig",


  #tau
"HLT_tau160_idperf_track:InDetTrigTrackingxAODCnv_Tau_FTF",
"HLT_tau160_idperf_track:InDetTrigTrackingxAODCnv_Tau_EFID",
"HLT_tau160_idperf_track:InDetTrigTrackingxAODCnv_Tau_IDTrig",
"HLT_tau160_idperf_tracktwo:InDetTrigTrackingxAODCnv_Tau_FTF",
"HLT_tau160_idperf_tracktwo:InDetTrigTrackingxAODCnv_Tau_EFID",
"HLT_tau160_idperf_tracktwo:InDetTrigTrackingxAODCnv_Tau_IDTrig",
"HLT_tau20_r1_idperf:InDetTrigTrackingxAODCnv_Tau_FTF",
"HLT_tau20_r1_idperf:InDetTrigTrackingxAODCnv_Tau_EFID",
"HLT_tau20_r1_idperf:InDetTrigTrackingxAODCnv_Tau_IDTrig",
"HLT_tau25_idperf_track:InDetTrigTrackingxAODCnv_Tau_FTF",
"HLT_tau25_idperf_track:InDetTrigTrackingxAODCnv_Tau_EFID",
"HLT_tau25_idperf_track:InDetTrigTrackingxAODCnv_Tau_IDTrig",
"HLT_tau25_idperf_tracktwo:InDetTrigTrackingxAODCnv_Tau_FTF",
"HLT_tau25_idperf_tracktwo:InDetTrigTrackingxAODCnv_Tau_EFID",
"HLT_tau25_idperf_tracktwo:InDetTrigTrackingxAODCnv_Tau_IDTrig",
"HLT_tau25_r1_idperf:InDetTrigTrackingxAODCnv_Tau_FTF",
"HLT_tau25_r1_idperf:InDetTrigTrackingxAODCnv_Tau_EFID",
"HLT_tau25_r1_idperf:InDetTrigTrackingxAODCnv_Tau_IDTrig",


  #bjet
"HLT_j110_bperf:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j110_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j110_bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j110_bperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j110_bperf_split:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j110_bperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j150_bperf:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j150_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j150_bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j150_bperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j150_bperf_split:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j150_bperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j15_bperf:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j15_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j15_bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j15_bperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j15_bperf_split:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j15_bperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j25_bperf:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j25_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j25_bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j25_bperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j25_bperf_split:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j25_bperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j260_bperf:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j260_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j260_bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j260_bperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j260_bperf_split:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j260_bperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j320_bperf:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j320_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j320_bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j320_bperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j320_bperf_split:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j320_bperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j35_bperf:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j35_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j35_bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j35_bperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j35_bperf_split:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j35_bperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j400_bperf:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j400_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j400_bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j400_bperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j400_bperf_split:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j400_bperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j45_bperf:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j45_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j45_bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j45_bperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j45_bperf_split:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j45_bperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j55_bperf:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j55_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j55_bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j55_bperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j55_bperf_split:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j55_bperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j85_bperf:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j85_bperf:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j85_bperf:InDetTrigTrackingxAODCnv_Bjet_IDTrig",
"HLT_j85_bperf_split:InDetTrigTrackingxAODCnv_Bjet_FTF",
"HLT_j85_bperf_split:InDetTrigTrackingxAODCnv_Bjet_EFID",
"HLT_j85_bperf_split:InDetTrigTrackingxAODCnv_Bjet_IDTrig",


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
#ServiceMgr.MessageSvc.OutputLevel = DEBUG


#
## Number of Events to process
##theApp.EvtMax = 5
MessageSvc.infoLimit  = 1000000
MessageSvc.debugLimit = 1000000


#svcMgr.AuditorSvc.Auditors += [ "ChronoAuditor"]
