# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getG4HitFilterTool(name="G4UA::G4HitFilterTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    kwargs.setdefault('VolumeNames',
                      ["BCMHits",
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
                       "TileHitVec"]
                   )
    return CfgMgr.G4UA__G4HitFilterTool(name, **kwargs)
