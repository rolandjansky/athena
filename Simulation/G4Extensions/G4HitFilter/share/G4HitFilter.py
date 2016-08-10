from AthenaCommon.CfgGetter import getPublicTool
from G4AtlasApps.SimFlags import simFlags
# configuration for MT actions
from G4HitFilter.G4HitFilterConfig import addG4HitFilterTool
hitf = getPublicTool('G4UA::G4HitFilterTool',tryDefaultConfigurable=True)
addG4HitFilterTool(hitf)

hitf.VolumeNames=[
    "BCMHits",
    "BLMHits",
    "CSC_Hits",
    "LArCalibrationHitActive",
    "LArCalibrationHitDeadMaterial",
    "LArCalibrationHitInactive",
    "LArHitEMB",
    "LArHitEMEC",
    "LArHitFCAL",
    "LArHitHEC",
    "MBTSHits",
    "MDT_Hits",
    "PixelHits",
    "RPC_Hits",
    "SCT_Hits",
    "TGC_Hits",
    "TRTUncompressedHits",
    "TileHitVec"
    ]
