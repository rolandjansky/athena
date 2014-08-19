#
# steering file for testing muon trigger slice with offline supertools
#
from RecExConfig.RecFlags import rec as recFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

athenaCommonFlags.PoolRDOInput = ["RDO.pool.root"]

globalflags.DetDescrVersion = "ATLAS-GEO-16-01-00"

if not ('EvtMax' in dir()):
    theApp.EvtMax = 10

# do trigger
TriggerModernConfig = True # not sure this actually does anything 
recFlags.doTrigger  = True
recFlags.doESD      = True

# detector flags
recFlags.doMuon          = True
recFlags.doInDet         = True
recFlags.doCalo          = True
recFlags.doLArg          = True
recFlags.doTile          = True
recFlags.doLucid         = True

# muon flags
#from MuonRecExample.MuonRecFlags import muonRecFlags
#muonRecFlags.doStandalone = False

#from MuonRecExample.MuonStandaloneFlags import muonStandaloneFlags
#muonStandaloneFlags.trackBuilder = 'Moore'

#from TrigMuonSuperEF.TrigMuonFlags import trigMuonFlags
#trigMuonFlags.doStandalone = True
#trigMuonFlags.trackBuilder = 'Moore'

# don't do unnecessary stuff
recFlags.doMuonCombined  = False
recFlags.readESD         = False
recFlags.doWriteESD      = False
recFlags.doCBNT          = False
recFlags.doWriteAOD      = False
recFlags.doWriteTAG      = False
recFlags.doAOD           = False
recFlags.doAODall        = False
recFlags.doJiveXML       = False

# physics flags
recFlags.doHeavyIon        = False
recFlags.doEgamma          = False
recFlags.doTau             = False
recFlags.doJetMissingETTag = False

# monitoring/auditing
recFlags.doPerfMon             = False
recFlags.doDetailedPerfMon     = False
recFlags.doSemiDetailedPerfMon = False
recFlags.doMonitoring          = False
recFlags.doFastPhysMonitoring  = False
recFlags.doDetailedAuditor     = False
recFlags.doSGAuditor           = False

# algorithms
from RecExConfig.RecAlgsFlags import recAlgs as recAlgsFlags
recAlgsFlags.doTrigger           = True
recAlgsFlags.doTrackRecordFilter = True # filter truth muons getting into the spectrometer
recAlgsFlags.doMissingET         = False
recAlgsFlags.doMissingETSig      = False
recAlgsFlags.doObjMissingET      = False
recAlgsFlags.doEFlow             = False
recAlgsFlags.doEFlowJet          = False
recAlgsFlags.doAtlfast           = False
recAlgsFlags.doMuonSpShower      = False # needs JetRec
recAlgsFlags.doTileMuID          = False
recAlgsFlags.doMuTag             = False
recAlgsFlags.doCaloTrkMuId       = False
recAlgsFlags.doEgammaBremReco    = False
recAlgsFlags.doAtlfast           = False

# disable JetRec (no RecFlags flag for that)
try:
    from JetRec.JetRecFlags import jetFlags
    jetFlags.Enabled = False
except ImportError:
    pass

# bug in RecExCond does not pass recFlags.doTau()
try:
    from tauRec.tauRecFlags import jobproperties
    jobproperties.tauRecFlags.Enabled = recFlags.doTau()
except ImportError:
    pass

#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------

####  First switch all slices OFF
TriggerFlags.Slices_all_setOff()

# Enable Muon slice
TriggerFlags.MuonSlice.setAll()

#TriggerFlags.MuonSlice.signatures = ['mu6', 'mu6_ef', 'mu20i']
TriggerFlags.MuonSlice.signatures = ['mu15_mu10_EFFS']

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

# from testMuonSliceAthenaModernRDOtoESDAOD.py
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

MessageSvc.debugLimit = 10000
MessageSvc.Format = "% F%65W%S%7W%R%T %0W%M"
MessageSvc.OutputLevel = INFO


#
# here we add the algorithm used for debugging TMEFSegmentFinder and TMEFTrackBuilder
#
L2OutputLvl = 3
EFOutputLvl = 2

topSequence.TrigSteer_EF.TrigMuonEFSegmentFinder.OutputLevel = EFOutputLvl
topSequence.TrigSteer_EF.TrigMuonEFTrackBuilder.OutputLevel = EFOutputLvl
topSequence.TrigSteer_EF.TrigMuonEFExtrapolator.OutputLevel = EFOutputLvl
topSequence.TrigSteer_EF.TrigMuonEFCombiner.OutputLevel = EFOutputLvl

# activate summaries for offline tools
topSequence.TrigSteer_EF.TrigMuonEFSegmentFinder.SegmentsFinderTool.DoSummary = False
topSequence.TrigSteer_EF.TrigMuonEFTrackBuilder.TrackBuilderTool.DoSummary = False

# for debug purposes write TrackCollection in StoreGate and use MuonTrackPerformanceAlg
topSequence.TrigSteer_EF.TrigMuonEFSegmentFinder.recordSegmentCombinations = False
topSequence.TrigSteer_EF.TrigMuonEFSegmentFinder.recordPatternCombinations = False
topSequence.TrigSteer_EF.TrigMuonEFTrackBuilder.recordTrackCollection = True

if topSequence.TrigSteer_EF.TrigMuonEFTrackBuilder.recordTrackCollection:
    from MuonTrackPerformance.MuonTrackPerformanceConf import MuonTrackPerformanceAlg
    topSequence += MuonTrackPerformanceAlg("MuonTrackPerformanceAlg",
                                           TrackInputLocation = "TrigMuonEFTBTracks",
                                           SegmentCombitLocation = "TrigMuonEFSegments" )
    topSequence.MuonTrackPerformanceAlg.DoSummary = 1
    topSequence.MuonTrackPerformanceAlg.DoTrackDebug = 5
    topSequence.MuonTrackPerformanceAlg.OutputLevel = EFOutputLvl

#from MuonRdoToPrepData.MuonRdoToPrepDataGenConf import MdtRdoToMdtPrepData
#MdtRdoToMdtPrepData = MdtRdoToMdtPrepData()
#MdtRdoToMdtPrepData.OutputLevel = INFO
#RpcRdoToRpcPrepData.OutputLevel = INFO
#TgcRdoToTgcPrepData.OutputLevel = INFO
#CscRdoToCscPrepData.OutputLevel = INFO
    
if TriggerFlags.doMuon() and TriggerFlags.doLVL2() and TriggerFlags.MuonSlice.signatures():
    muFast = topSequence.TrigSteer_L2.allConfigurables.get('muFast_Muon')
    muFast.MUlvl1INFO  = False
    muFast.MUtrackINFO = False
    muFast.MUroadsINFO = False
    muFast.MUdecoINFO  = False
    muFast.MUcontINFO  = False
    muFast.MUfitINFO   = False
    muFast.MUsagINFO   = False
    muFast.MUptINFO    = True
    muFast.TestString = "muFast_Muon  REGTEST "
    muFast.OutputLevel = L2OutputLvl
    muFastHypo6 = topSequence.TrigSteer_L2.allConfigurables.get('MufastHypo_Muon_6GeV_v11a')
    muFastHypo6.OutputLevel = L2OutputLvl
    muFastHypo20 = topSequence.TrigSteer_L2.allConfigurables.get('MufastHypo_Muon_20GeV_v11a')
    muFastHypo20.OutputLevel = L2OutputLvl
    muCombHypo6 = topSequence.TrigSteer_L2.allConfigurables.get('MucombHypo_Muon_6GeV')
    muCombHypo6.OutputLevel = L2OutputLvl
    muCombHypo20 = topSequence.TrigSteer_L2.allConfigurables.get('MucombHypo_Muon_20GeV')
    muCombHypo20.OutputLevel = L2OutputLvl


print topSequence
print ServiceMgr

from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")
