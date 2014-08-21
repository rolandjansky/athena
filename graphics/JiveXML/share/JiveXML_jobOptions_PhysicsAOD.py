#
# jOs for RTT testing: Cosmics/1beam AODs, run from Jan10
# Castor access for official ESDs and AODs is closed, use
# wget from Bham instead. Admittingly this is messy, should be
# put into private Castor area, or some other place on CERN-AFS.
#   jpt 11Nov09
#
import os
#### remove db link (RecExCommon_links is executed
#### ahead of all RTT jOs):
os.system('rm -rf sqlite200')
#### link is updated (no need to update jOs):
os.system('cp /afs/cern.ch/atlas/project/Atlantis/rttInputs/AOD_data11_RTT.pool.root .')

#from RecExConfig.RecFlags  import rec  #obsolete
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

## 'FilesInput' triggers full autoconfiguration, which
## replaces GEO and CONDDB setting by hand
## Details see in Twiki: RecExCommonAutoConfiguration
athenaCommonFlags.FilesInput.set_Value_and_Lock(['AOD_data11_RTT.pool.root'])

athenaCommonFlags.EvtMax.set_Value_and_Lock(10)

jp.Rec.doJiveXML.set_Value_and_Lock(True)
jp.Rec.doCBNT.set_Value_and_Lock(False)
jp.Rec.doAOD.set_Value_and_Lock(False)
jp.Rec.doWriteTAG.set_Value_and_Lock(False)
jp.Rec.doMonitoring.set_Value_and_Lock(False)

# to turn off perfmon - avoids writing out big ntuples

jp.Rec.doPerfMon=False

# main jobOption - must always be included
include ("RecExCommon/RecExCommon_topOptions.py")

theAODJetRetriever.FavouriteJetCollection="AntiKt4TopoEMJets"
### 'EM' are up-to-date jets (May 2011), 'LC' could be default in 2012:
theAODJetRetriever.OtherJetCollections=["AntiKt6TopoEMJets","AntiKt4LCTopoJets","AntiKt6LCTopoJets","AntiKt6TowerJets","AntiKt6GhostTowerJets"]

theJetRecJetRetriever.FavouriteJetCollection="AntiKt4TopoEMJets"
### 'EM' are up-to-date jets (May 2011), 'LC' could be default in 2012:
theJetRecJetRetriever.OtherJetCollections=["AntiKt6TopoEMJets","AntiKt4LCTopoJets","AntiKt6LCTopoJets","AntiKt6TowerJets","AntiKt6GhostTowerJets"]

theBJetRetriever.FavouriteJetCollection="AntiKt4TopoEMJets"
### 'EM' are up-to-date jets (May 2011), 'LC' could be default in 2012:
theBJetRetriever.OtherJetCollections=["AntiKt6TopoEMJets","AntiKt4LCTopoJets","AntiKt6LCTopoJets"]

# for track-vertex association
theVertexRetriever.TrackCollection = "TrackParticleCandidate"

# geometry version written into xml-file (for upgrade)
from AthenaCommon.GlobalFlags import globalflags
theEventData2XML.GeometryVersion = globalflags.DetDescrVersion()

