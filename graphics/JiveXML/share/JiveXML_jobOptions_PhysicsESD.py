## RTT jOs run in RecExCommon environment
## (RecExCommon_links.sh), but this doesn't work
## with Cosmics, need to remove static db link
## with shell command:
import os
#### remove db link:
os.system('rm -rf sqlite200')
# link is updated (no need to update jOs):
os.system('cp /afs/cern.ch/atlas/project/Atlantis/rttInputs/ESD_data11_RTT.pool.root .')
#
#### example (not needed):
##os.system('RecExCommission_links.sh')
##
## this is a trick to over-ride .set_Value_and_Lock
## settings in readesd.py: The first setting counts !
##
#from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

## 'FilesInput' triggers full autoconfiguration, which
## replaces GEO and CONDDB setting by hand
athenaCommonFlags.FilesInput.set_Value_and_Lock(['ESD_data11_RTT.pool.root'])

athenaCommonFlags.EvtMax.set_Value_and_Lock(10)

jp.Rec.doJiveXML=True
jp.Rec.doCBNT.set_Value_and_Lock(False)
jp.Rec.doMonitoring.set_Value_and_Lock(False)
jp.Rec.doAOD.set_Value_and_Lock(False)
jp.Rec.doWriteAOD.set_Value_and_Lock(False)

#### not necessary anymore  
#rec.abortOnUncheckedStatusCode.set_Value_and_Lock(False)

#### job options to retrieve spacepoints from ESDs
#### not done by default ! From Thijs Cornellisen
from InDetRecExample.InDetJobProperties import InDetFlags
jobproperties.InDetJobProperties.Enabled.set_Value_and_Lock(True)
InDetFlags.doSpacePointFormation.set_Value_and_Lock(True)
#### investigation by Eric showed: Following line
#### causes wrong jet-track-association, commented-out,jpt 26Aug10
##InDetFlags.disableInDetReco.set_Value_and_Lock(False)

#### new 23Feb10: load full electronics calibration database
#### not done by default anymore. Info by G.Unal.
from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.LoadElecCalib.set_Value_and_Lock(True)

include ("RecExCommission/readesd.py")
##
## Calo details:
theCaloTileRetriever.DoTileCellDetails = True
theCaloTileRetriever.DoTileDigit = True
theCaloMBTSRetriever.DoMBTSDigits = True
theCaloFCalRetriever.DoFCalCellDetails = True
theCaloLArRetriever.DoLArCellDetails = True
theCaloHECRetriever.DoHECCellDetails = True
theLArDigitRetriever.DoLArDigit = True
theLArDigitRetriever.DoHECDigit = True
theLArDigitRetriever.DoFCalDigit = True

# default is ExtendedTracks,
# not present in normal ESDs, so:
theTrackRetriever.PriorityTrackCollection = "Tracks"
# default association is with CombinedInDetTracks,
# which are not present in normal ESDs, so: 
theJetRecJetRetriever.TrackCollection = "Tracks"

## special flags for spacepoints from data11 ESDs, provided
## by Jean-Baptiste de Vivie
topSequence.InDetSiTrackerSpacePointFinder.ProcessPixels   = True
topSequence.InDetSiTrackerSpacePointFinder.ProcessSCTs     = True
topSequence.InDetSiTrackerSpacePointFinder.ProcessOverlaps = True

theAODJetRetriever.FavouriteJetCollection="AntiKt4TopoEMJets"
### 'EM' are up-to-date jets (May 2011), 'LC' could be default in 2012:
theAODJetRetriever.OtherJetCollections=["AntiKt6TopoEMJets","AntiKt4LCTopoJets","AntiKt6LCTopoJets","AntiKt6TowerJets","AntiKt6GhostTowerJets"]

theJetRecJetRetriever.FavouriteJetCollection="AntiKt4TopoEMJets"
### 'EM' are up-to-date jets (May 2011), 'LC' could be default in 2012:
theJetRecJetRetriever.OtherJetCollections=["AntiKt6TopoEMJets","AntiKt4LCTopoJets","AntiKt6LCTopoJets","AntiKt6TowerJets","AntiKt6GhostTowerJets"]

theBJetRetriever.FavouriteJetCollection="AntiKt4TopoEMJets"
### 'EM' are up-to-date jets (May 2011), 'LC' could be default in 2012: 
theBJetRetriever.OtherJetCollections=["AntiKt6TopoEMJets","AntiKt4LCTopoJets","AntiKt6LCTopoJets"]

# too many muon collections, write only most important:
theTrackParticleRetriever.OtherTrackCollections = ["CombinedFitMuonParticles","StacoTrackParticles","MuonboyTrackParticles"]
theTrackRetriever.OtherTrackCollections = ["CombinedFitMuonTracks","MuonSpectrometerTracks","ConvertedStacoTracks ","ConvertedMuIdCBTracks","CombinedInDetTracks","ExtendedTracks"]

# geometry version written intoxml-file (for upgrade)
from AthenaCommon.GlobalFlags import globalflags
theEventData2XML.GeometryVersion = globalflags.DetDescrVersion()


