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

if athenaCommonFlags.FilesInput()==[]:
  athenaCommonFlags.FilesInput=[
    'data/data12_8TeV.00215456.physics_JetTauEtmiss.merge.AOD.f495_m1266/data12_8TeV.00215456.physics_JetTauEtmiss.merge.AOD.f495_m1266._lb0407._0001.1',
   ]

PdgId=15

### all events:
athenaCommonFlags.EvtMax.set_Value_and_Lock(-1)
### for test, some events:
#athenaCommonFlags.EvtMax.set_Value_and_Lock(5)

#rec.doJiveXML.set_Value_and_Lock(True)
rec.doCBNT.set_Value_and_Lock(False)
rec.doMonitoring.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(True)
rec.doWriteAOD.set_Value_and_Lock(True)
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
tdt.OutputLevel = INFO

##### Histogram File Part #################

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager


############ TrigIDtrkMonioting part ################################

from TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig import TrigIDtrkMonitingTool

montools = TrigIDtrkMonitoringTool()

print montools

HLTMonManager.AthenaMonTools += montools


############ TrigInDetAnalysis part ################################



from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestMonToolAC
TestMonTool = TrigTestMonToolAC( name="TestMonToolAC")
TestMonTool.OutputLevel = INFO
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
  "Taus",
  "Taus3",
  #egamma
#  "L2_e22vh_medium_IDTrkNoCut:TrigIDSCAN_eGamma",
#  "L2_e22vh_medium_IDTrkNoCut:TrigSiTrack_eGamma",
#  "EF_e22vh_medium_IDTrkNoCut:InDetTrigParticleCreation_Electron_EFID",
  #mu
#  "L2_mu20_IDTrkNoCut:TrigIDSCAN_Muon",
#  "L2_mu20_IDTrkNoCut:TrigSiTrack_Muon",
#  "EF_mu20_IDTrkNoCut:InDetTrigParticleCreation_Muon_EFID",

  #tau
#  "L2_tau29_IDTrkNoCut:TrigIDSCAN_Tau",
  "L2_tau29_IDTrkNoCut:TrigL2SiTrackFinder_Tau:1",
#  "L2_tau29_IDTrkNoCut:TrigSiTrack_Tau_robust",
#  "L2_tau125_IDTrkNoCut:TrigIDSCAN_Tau",
#  "L2_tau125_IDTrkNoCut:TrigSiTrack_Tau",
#  "L2_tau125_IDTrkNoCut:TrigSiTrack_Tau_robust",
  "EF_tau29_IDTrkNoCut:InDetTrigParticleCreation_Tau_EFID",
#  "EF_tau125_IDTrkNoCut:InDetTrigParticleCreation_Tau_EFID",
  #bjet
#  "L2_b10_IDTrkNoCut:TrigIDSCAN_Jet",
#  "L2_b10_IDTrkNoCut:TrigSiTrack_Jet",
#  "EF_b10_IDTrkNoCut:InDetTrigParticleCreation_Bjet_EFID",
   "HLT_.*id.*cosmic.*:InDetTrigTrackingxAODCnv_CosmicsN_EFID"
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
