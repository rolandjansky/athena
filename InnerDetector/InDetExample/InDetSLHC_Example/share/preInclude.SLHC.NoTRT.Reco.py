include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.NoTRT.py")

### Temporally disabled to avoid a crash or an ERROR message (start) ###
### Checked with devval rel_6 as of 2014-12-07

rec.doTrigger=False # Otherwise, you will get "Py:TriggerMenu.menu.GenerateMenu   ERROR GenerateMenu: Problems when importing EgammaDef.py or PhotonDef.py, disabling egamma chains."

### Temporally disabled to avoid a crash or an ERROR message (end) ###

# Magnet is on
from AthenaCommon.DetFlags import DetFlags
DetFlags.BField_setOn()

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

InDetFlags.doSLHC=True
InDetFlags.doForwardTracks=True
InDetFlags.doSlimming=False
InDetFlags.doBeamHalo=False
InDetFlags.doStatistics=True
InDetFlags.useDCS=False
InDetFlags.doBremRecovery=True
InDetFlags.doCaloSeededBrem=False

InDetFlags.doSGDeletion=False # This should be False. 
InDetFlags.doPixelClusterSplitting=False # This should be False.
InDetFlags.doLowBetaFinder=False # This should be False. 

if rec.OutputFileNameForRecoStep() == 'RAWtoESD':
    InDetFlags.writeRDOs=True
    InDetFlags.doSLHCVeryForward=True

# InDetFlags.doPhysValMon=True 

# if rec.OutputFileNameForRecoStep() == 'ESDtoAOD': 
#     InDetFlags.doNewTracking=False
#     InDetFlags.doVertexFinding=False
#     InDetFlags.doPRDFormation=False
#     InDetFlags.doParticleCreation=False
#     InDetFlags.disableInDetReco.set_Value_and_Lock(False)
#     InDetFlags.doStatistics=False
#     InDetFlags.disableTracking=False
#     InDetFlags.doForwardTracks=False
#     InDetFlags.doSGDeletion=True

# InDetFlags.doPrintConfigurables.set_Value_and_Lock(True)
