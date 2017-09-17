# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

from ISF_Algorithms.collection_merger_helpers import generate_mergeable_collection_name


def getTileGeoG4SD(name="TileGeoG4SD", **kwargs):
    bare_collection_name = "TileHitVec"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TileHits"
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Tile::Scintillator"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() == 'cosmics' or (hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn):
        kwargs.setdefault("DeltaTHit", [1])
        kwargs.setdefault("DoTOFCorrection", False)
    return CfgMgr.TileGeoG4SDTool(name, **kwargs)


def getTileCTBGeoG4SD(name="TileCTBGeoG4SD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Tile::Scintillator"])
    kwargs.setdefault("OutputCollectionNames", ["TileHitVec"])
    kwargs.setdefault("TileCalculator", "TileCTBGeoG4SDCalc")
    return CfgMgr.TileGeoG4SDTool(name, **kwargs)


def getTileGeoG4SDCalc(name="TileGeoG4SDCalc", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() == 'cosmics' or (hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn):
        kwargs.setdefault("DeltaTHit", [1])
        kwargs.setdefault("DoTOFCorrection", False)
    return CfgMgr.TileGeoG4SDCalc(name, **kwargs)


def getTileCTBGeoG4SDCalc(name="TileCTBGeoG4SDCalc", **kwargs):
    return CfgMgr.TileGeoG4SDCalc(name, **kwargs)


