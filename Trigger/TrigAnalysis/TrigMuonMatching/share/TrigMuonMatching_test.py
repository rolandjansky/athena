
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
if not acf.EvtMax.is_locked():
    acf.EvtMax=2000

FNAME= ["/afs/cern.ch/user/k/kokasaha/public/sample/AOD.04274415._000011.pool.root.1"]
acf.FilesInput=FNAME

rec.doCBNT=False
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

include("RecExCommon/RecExCommon_topOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from TrigMuonMatching.TrigMuonMatchingConf import Trig__TrigMuonMatching
matchtool = Trig__TrigMuonMatching("MatchingTool");
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += matchtool

from TrigMuonMatching.TrigMuonMatchingConf import Trig__TrigMuonMatching_example
alg = Trig__TrigMuonMatching_example()
alg.MuonMatchTool = matchtool
alg.OutputLevel = DEBUG
theJob += alg

include("TriggerTest/TriggerTestCommon.py")

