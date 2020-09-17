###### Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration ####### 

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doTrigger=False

# reat ESD don't write anything
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

rec.doShowSizeStatistics=False
rec.readTAG=False
rec.readRDO=False
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

rec.AutoConfiguration = [ "everything" ]

from AthenaCommon.DetFlags import DetFlags      
DetFlags.detdescr.all_setOn()


include("RecExCommon/RecExCommon_topOptions.py")

