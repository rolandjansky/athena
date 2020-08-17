# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

TileGeoG4SDTool, TileGeoG4SDCalc=CompFactory.getComps("TileGeoG4SDTool","TileGeoG4SDCalc",)

def TileGeoG4SDCfg(ConfigFlags, name="TileGeoG4SD", **kwargs):
    bare_collection_name = "TileHitVec"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TileHits"
    region = "CALO"

    result, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name, mergeable_collection_suffix, merger_input_property, region)
    kwargs.setdefault("LogicalVolumeNames", ["Tile::Scintillator"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.merge(TileGeoG4SDCalcCfg(ConfigFlags))
    kwargs.setdefault("TileCalculator", result.getService("TileGeoG4SDCalc") )

    result.setPrivateTools( TileGeoG4SDTool(name, **kwargs)  )
    return result


def TileCTBGeoG4SDCfg(ConfigFlags, name="TileCTBGeoG4SD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Tile::Scintillator"])
    kwargs.setdefault("OutputCollectionNames", ["TileHitVec"])

    result = TileCTBGeoG4SDCalcCfg(ConfigFlags)
    kwargs.setdefault("TileCalculator", result.getService("TileCTBGeoG4SDCalc") )

    result.setPrivateTools( TileGeoG4SDTool(name, **kwargs) )
    return result


def TileGeoG4SDCalcCfg(ConfigFlags, name="TileGeoG4SDCalc", **kwargs):
    result = ComponentAccumulator()

    if ConfigFlags.Beam.Type == 'cosmics' or ConfigFlags.Sim.ReadTR:
        kwargs.setdefault("DeltaTHit", [1])
        kwargs.setdefault("DoTOFCorrection", False)
    if ConfigFlags.Sim.ParticleID:
        kwargs.setdefault("DoCalibHitParticleID", ConfigFlags.Sim.ParticleID )

    result.addService( TileGeoG4SDCalc(name, **kwargs) )
    return result


def TileCTBGeoG4SDCalcCfg(ConfigFlags, name="TileCTBGeoG4SDCalc", **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("TileTB", True)
    if ConfigFlags.Sim.ParticleID:
        kwargs.setdefault("DoCalibHitParticleID", ConfigFlags.Sim.ParticleID )

    result.addService( TileGeoG4SDCalc(name, **kwargs) )
    return result
