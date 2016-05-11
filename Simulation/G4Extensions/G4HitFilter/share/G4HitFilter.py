from AthenaCommon.CfgGetter import getPublicTool
from G4AtlasApps.SimFlags import simFlags
if hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions():
    # configuration for MT actions
    from G4HitFilter.G4HitFilterConfig import addG4HitFilterTool
    hitf = getPublicTool('G4UA::G4HitFilterTool',tryDefaultConfigurable=True)
    addG4HitFilterTool(hitf)
else:
    # V1 userActions
    try:
        from G4AtlasServices.G4AtlasUserActionConfig import UAStore
    except ImportError:
        from G4AtlasServices.UserActionStore import UAStore
    hitf = getPublicTool('G4HitFilter',tryDefaultConfigurable=True)
    UAStore.addAction(hitf,['EndOfEvent'])

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
