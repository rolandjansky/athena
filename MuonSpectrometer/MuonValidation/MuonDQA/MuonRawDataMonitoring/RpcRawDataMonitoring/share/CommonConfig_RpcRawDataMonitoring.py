#--------------------------------------------------------------
# Configure algorithm.
#--------------------------------------------------------------

# setup trigger config
from RecExConfig.RecFlags import rec
rec.doTrigger=True

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doTrigger=True

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doTriggerConfigOnly=True

# read ESD and don't write anything
rec.readESD=True
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False
rec.doDPD=False
rec.doESD=False
doTAG=False

# switch off as much as possible
rec.doTruth=False
rec.doRecoTiming=False
# rec.doDetStatus=True
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
rec.doCheckDictionary=False
rec.doFileMetaData=False
rec.doAODCaloCells=False
recAlgs.doAtlfast=False
recAlgs.doMonteCarloReact=False
rec.doEgamma=False
rec.CBNTAthenaAware=False
rec.doAODSelect=False
rec.doWritexAOD=False
rec.doPerfMon=False
rec.doTagRawSummary=False
rec.doBTagging=False
rec.doSemiDetailedPerfMon=False
rec.doAODall=False
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

from AthenaCommon.DetFlags import DetFlags      
DetFlags.detdescr.all_setOn()
#
# TGC CABLING
##from MuonCablingServers.MuonCablingServersConf import TGCcablingServerSvc
##ServiceMgr += TGCcablingServerSvc()
##theApp.CreateSvc += [ "TGCcablingServerSvc" ]
##ServiceMgr.TGCcablingServerSvc.Atlas=True
##ServiceMgr.TGCcablingServerSvc.forcedUse=True
##ServiceMgr.TGCcablingServerSvc.useMuonTGC_CablingSvc=True
##from TGC_CondCabling.TGC_CondCablingConf import TGCCablingDbTool
##ToolSvc += TGCCablingDbTool()
##from IOVDbSvc.CondDB import conddb
##conddb.addFolderSplitMC('TGC','/TGC/CABLING/MAP_SCHEMA','/TGC/CABLING/MAP_SCHEMA')

