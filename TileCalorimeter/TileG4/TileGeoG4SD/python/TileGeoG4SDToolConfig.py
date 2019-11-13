# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

from TileGeoG4SD.TileGeoG4SDConf import TileGeoG4SDTool, TileGeoG4SDCalc

def TileGeoG4SDCfg(ConfigFlags, name="TileGeoG4SD", **kwargs):
    bare_collection_name = "TileHitVec"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TileHits"

    result, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name, mergeable_collection_suffix, merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Tile::Scintillator"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.setPrivateTools( TileGeoG4SDTool(name, **kwargs)  )
    return result


def TileCTBGeoG4SDCfg(ConfigFlags, name="TileCTBGeoG4SD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Tile::Scintillator"])
    kwargs.setdefault("OutputCollectionNames", ["TileHitVec"])

    result = TileGeoG4SDCalcCfg(ConfigFlags)
    kwargs.setdefault("TileCalculator", result.getService("TileCTBGeoG4SDCalc") )

    result.setPrivateTools( TileGeoG4SDTool(name, **kwargs) )
    return result


def TileGeoG4SDCalcCfg(ConfigFlags, name="TileGeoG4SDCalc", **kwargs):
    result = ComponentAccumulator()

    from G4AtlasApps.SimFlags import simFlags
    if ConfigFlags.Beam.Type == 'cosmics' or ConfigFlags.Sim.ReadTR:
        kwargs.setdefault("DeltaTHit", [1])
        kwargs.setdefault("DoTOFCorrection", False)
    if ConfigFlags.Sim.ParticleID:
        kwargs.setdefault("DoCalibHitParticleID", ConfigFlags.Sim.ParticleID )

    result.setPrivateTools( TileGeoG4SDCalc(name, **kwargs) )
    return result


def TileCTBGeoG4SDCalcCfg(ConfigFlags, name="TileCTBGeoG4SDCalc", **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("TileTB", True)
    if ConfigFlags.Sim.ParticleID:
        kwargs.setdefault("DoCalibHitParticleID", ConfigFlags.Sim.ParticleID )

    result.setPrivateTools( TileGeoG4SDCalc(name, **kwargs) )
    return result
