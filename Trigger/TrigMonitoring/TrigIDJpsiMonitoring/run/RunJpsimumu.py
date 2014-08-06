# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## set up monitoring
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager

from TrigIDJpsiMonitoring.TrigIDJpsiMonitoringConf import TrigJpsiMonTool
TestMonTool = TrigJpsiMonTool( name="TestMonTool")
ToolSvc += TestMonTool
HLTMonManager.AthenaMonTools += [ "TrigJpsiMonTool/TestMonTool" ]

##### Trigger Decision Part #####################

## set up trigger decision tool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
tdt.OutputLevel = INFO

# these lines are new:
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter()

rec.doWriteAOD=False
rec.doWriteESD=False

#########cut part

TrigJpsiMonTool.matchR = 0.1
TrigJpsiMonTool.pTCutOffline = 4000.0
TrigJpsiMonTool.d0Cut = 1000.0
TrigJpsiMonTool.z0Cut = 2000.0
TrigJpsiMonTool.d0CutOffline = 1000.0
TrigJpsiMonTool.z0CutOffline = 2000.0
#TrigJpsiMonTool.pTCutOffline = 4000.0
#TrigJpsiMonTool.d0Cut = 1000.0
#TrigJpsiMonTool.d0CutOffline = 1000.0
#TrigJpsiMonTool.matchR = 1.0
#TrigJpsiMonTool.z0CutOffline = 2000.0

#TrigJpsiMonTool.Chain0 = "EF_2mu13_Zmumu_IDTrkNoCut"
TrigJpsiMonTool.Chain0 = "EF_mu20"
TrigJpsiMonTool.Chain1 = "EF_mu24i_tight"
#TrigJpsiMonTool.Chain1 = "EF_2mu4T_Jpsimumu_IDTrkNoCut"
#TrigJpsiMonTool.Chain1 = ""
TrigJpsiMonTool.Chain2 = ""
TrigJpsiMonTool.Chain3 = ""

TrigJpsiMonTool.IDSCAN = ""
TrigJpsiMonTool.SiTrack = ""
TrigJpsiMonTool.EF = "InDetTrigParticleCreation_Muon_EFID"
TrigJpsiMonTool.StratA = "TrigL2SiTrackFinder_MuonA"
#TrigJpsiMonTool.StratA = "TrigSiTrackFinder_Muon"
#TrigJpsiMonTool.StratB = "TrigL2SiTrackFinder_MuonB"
#TrigJpsiMonTool.StratC = "TrigL2SiTrackFinder_MuonC"
#TrigJpsiMonTool.StratA = ""
TrigJpsiMonTool.StratB = ""
TrigJpsiMonTool.StratC = ""


TrigJpsiMonTool.TRTSF = "";
TrigJpsiMonTool.EFTRT = "";
##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='IDTrackingCBNT.aan.root' OPT='RECREATE'" ]
HLTMonManager.FileKey = "AANT"
#from AnalysisTools.AnalysisToolsConf import AANTupleStream
#topSequence += AANTupleStream()
#AANTupleStream = AANTupleStream()
#AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
#AANTupleStream.OutputName = 'IDTrackingCBNT.aan.root'
#AANTupleStream.WriteInputDataHeader = True
#AANTupleStream.OutputLevel = WARNING
