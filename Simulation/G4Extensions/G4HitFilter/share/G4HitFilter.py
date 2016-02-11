
from G4HitFilter.G4HitFilterConfig import addG4HitFilterTool
from AthenaCommon.CfgGetter import getPublicTool


#hitf = getPublicTool('G4HitFilter',tryDefaultConfigurable=True)
hitf = getPublicTool('G4UA::G4HitFilterTool',tryDefaultConfigurable=True)

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

addG4HitFilterTool(hitf)

#UAStore.addAction(hitf,['EndOfEvent'])

