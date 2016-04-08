
from G4AtlasServices.G4AtlasUserActionConfig import UAStore
from AthenaCommon.CfgGetter import getPublicTool


hitf = getPublicTool('G4HitFilter',tryDefaultConfigurable=True)

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

UAStore.addAction(hitf,['EndOfEvent'])

