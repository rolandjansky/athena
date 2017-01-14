
### Temporally disabled to avoid a crash or an ERROR message (start) ###
### Checked with devval rel_6 as of 2014-12-07

rec.doTrigger=False # Otherwise, you will get "Py:TriggerMenu.menu.GenerateMenu   ERROR GenerateMenu: Problems when importing EgammaDef.py or PhotonDef.py, disabling egamma chains."

### Temporally disabled to avoid a crash or an ERROR message (end) ###

# Magnet is on
from AthenaCommon.DetFlags import DetFlags
DetFlags.BField_setOn()

DetFlags.pileup.all_setOff()

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.SLHC_Geometry=True

## MaterialStoreGateKey
TrkDetFlags.MaterialStoreGateKey="/GLOBAL/TrackingGeo/LayerMaterialITK"

## MaterialMagicTag
from AthenaCommon.GlobalFlags import jobproperties
DetDescrVersion = jobproperties.Global.DetDescrVersion()
# splitGeo = DetDescrVersion.split('-')
# TrkDetFlags.MaterialMagicTag = splitGeo[0] + '-' + splitGeo[1] + '-' + splitGeo[2]
TrkDetFlags.MaterialMagicTag = DetDescrVersion

## MaterialVersion and MaterialSubVersion
TrkDetFlags.MaterialVersion=17
TrkDetFlags.MaterialSubVersion=""

## To use the local material map
TrkDetFlags.MaterialDatabaseLocal=False
if TrkDetFlags.MaterialDatabaseLocal():
    TrkDetFlags.MaterialDatabaseLocalPath="./" 
    TrkDetFlags.MaterialDatabaseLocalName="AtlasLayerMaterial-"+DetDescrVersion+".db"

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSLHC=True
if not InDetFlags.useInDetDynamicCuts(): # because InDetDynamicCuts covers whole eta region
    InDetFlags.doForwardTracks=True

InDetFlags.doSlimming=False
InDetFlags.doBeamHalo=False
InDetFlags.doStatistics=True
InDetFlags.useDCS=False

InDetFlags.doSGDeletion=False # This should be False. 
#InDetFlags.doPixelClusterSplitting=False  # Christoph Rauchegger: Set to true (default) for run2 ambi solving  # Previously: This should be False.
InDetFlags.doLowBetaFinder=False # This should be False. 

# InDetFlags.doStandardPlots=True 
# InDetFlags.doPrintConfigurables = True

if rec.OutputFileNameForRecoStep() == 'RAWtoESD':
    InDetFlags.writeRDOs = True
    InDetFlags.doSLHCConversionFinding = False
    if not InDetFlags.useInDetDynamicCuts(): # because InDetDynamicCuts covers whole eta region
        InDetFlags.doSLHCVeryForward = True

# if rec.OutputFileNameForRecoStep() == 'ESDtoAOD': 
#     InDetFlags.doNewTracking=False
#     InDetFlags.doVertexFinding=False
#     InDetFlags.doPRDFormation=False
#     InDetFlags.doParticleCreation=False
#     InDetFlags.disableInDetReco=False
#     InDetFlags.doStatistics=False
#     InDetFlags.disableTracking=False
#     InDetFlags.doForwardTracks=False
#     InDetFlags.doSGDeletion=True

if rec.OutputFileNameForRecoStep() == 'ESDtoDPD': 
    rec.doDPD = True
    from InDetPrepRawDataToxAOD.InDetDxAODJobProperties import InDetDxAODFlags
    InDetDxAODFlags.DumpUnassociatedHits = False
    InDetDxAODFlags.DumpPixelInfo = True
    InDetDxAODFlags.DumpPixelRdoInfo = True
    InDetDxAODFlags.DumpSctInfo = True
    InDetDxAODFlags.DumpSctRdoInfo = True
    InDetDxAODFlags.ThinHitsOnTrack = False
    InDetDxAODFlags.ThinTrackSelection = "InDetTrackParticles.pt > 0.0*GeV"
    InDetDxAODFlags.DumpTriggerInfo = False
    InDetDxAODFlags.DumpLArCollisionTime = False
