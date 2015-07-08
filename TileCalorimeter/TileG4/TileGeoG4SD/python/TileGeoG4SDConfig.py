# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getTileGeoG4SD(name="TileGeoG4SD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Tile::Scintillator"])
    kwargs.setdefault("OutputCollectionNames", ["TileHitVec"])

    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() == 'cosmics' or (hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn):
        kwargs.setdefault("DeltaTHit", [1])
        kwargs.setdefault("DoTOFCorrection", False)
    return CfgMgr.TileGeoG4SDTool(name, **kwargs)


def getTileCTBGeoG4SD(name="TileCTBGeoG4SD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Tile::Scintillator"])
    kwargs.setdefault("OutputCollectionNames", ["TileHitVec"])
    return CfgMgr.TileGeoG4SDTool(name, **kwargs)
