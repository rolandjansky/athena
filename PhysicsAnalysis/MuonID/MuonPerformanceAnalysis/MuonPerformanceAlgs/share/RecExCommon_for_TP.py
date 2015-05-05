
# Set up RecExCommon to get the trigger configuration from the xAOD

from RecExConfig.RecFlags import rec
rec.doTrigger=True
from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doTrigger=True
recAlgs.doAtlfast=False
recAlgs.doMonteCarloReact=False
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doTriggerConfigOnly=True
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False
rec.doDPD=False
rec.doESD=False
doTAG=False
rec.doTruth=False
rec.doRecoTiming=False
rec.doDetStatus=False
rec.doShowSizeStatistics=False
rec.readTAG=False
rec.readRDO=False
rec.doHist=False
rec.doContainerRemapping=False
rec.doJiveXML=False
rec.doEdmMonitor=False
rec.doDumpPoolInputContent=False
rec.doHeavyIon=False
rec.doHIP=False
rec.doWriteBS=False
rec.doPhysValMonHists=False
rec.doVP1=False
rec.doJiveXML=False
rec.doMuon=False
rec.doCheckDictionary=False
rec.doFileMetaData=False
rec.doCalo=False
rec.doAODCaloCells=False

rec.doEgamma=False
rec.CBNTAthenaAware=False
rec.doAODSelect=False
rec.doWritexAOD=False
rec.doPerfMon=False
rec.doZdc=False
rec.doTagRawSummary=False
rec.doBTagging=False
rec.doSemiDetailedPerfMon=False
rec.doForwardDet=False
rec.doInDet=True
rec.doAODall=False
rec.doLucid=False
rec.doTau=False
rec.doJetMissingETTag=False


recAlgs.doCaloTrkMuId=False
recAlgs.doEFlow=False
recAlgs.doMissingET=False
recAlgs.doMuGirl=False
recAlgs.doMuTag=False
recAlgs.doMuonIDCombined=False
recAlgs.doMuonIDStandAlone=False
recAlgs.doStaco=False
recAlgs.doTileMuID=False
recAlgs.doTrackParticleCellAssociation=False
recAlgs.doTrackRecordFilter=False


include("RecExCommon/RecExCommon_topOptions.py")


