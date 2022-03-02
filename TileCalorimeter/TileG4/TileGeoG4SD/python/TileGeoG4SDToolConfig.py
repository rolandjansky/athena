# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType
from ISF_Algorithms.CollectionMergerConfig import CollectionMergerCfg

TileGeoG4SDTool, TileGeoG4SDCalc=CompFactory.getComps("TileGeoG4SDTool","TileGeoG4SDCalc",)

def TileGeoG4SDCfg(ConfigFlags, name="TileGeoG4SD", **kwargs):
    bare_collection_name = "TileHitVec"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TileHits"
    region = "CALO"

    result, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name, mergeable_collection_suffix, merger_input_property, region)
    kwargs.setdefault("LogicalVolumeNames", ["Tile::Scintillator"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    kwargs.setdefault("TileCalculator", result.getPrimaryAndMerge(TileGeoG4SDCalcCfg(ConfigFlags)).name )

    result.setPrivateTools( TileGeoG4SDTool(name, **kwargs)  )
    return result


def TileCTBGeoG4SDCfg(ConfigFlags, name="TileCTBGeoG4SD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Tile::Scintillator"])
    kwargs.setdefault("OutputCollectionNames", ["TileHitVec"])

    result = ComponentAccumulator()
    kwargs.setdefault("TileCalculator", result.getPrimaryAndMerge(TileCTBGeoG4SDCalcCfg(ConfigFlags)).name )

    result.setPrivateTools( TileGeoG4SDTool(name, **kwargs) )
    return result


def TileGeoG4SDCalcCfg(ConfigFlags, name="TileGeoG4SDCalc", **kwargs):
    result = ComponentAccumulator()

    if ConfigFlags.Beam.Type is BeamType.Cosmics or ConfigFlags.Sim.ReadTR:
        kwargs.setdefault("DeltaTHit", [1])
        kwargs.setdefault("DoTOFCorrection", False)
    kwargs.setdefault("DoCalibHitParticleID", ConfigFlags.Sim.ParticleID )

    result.addService( TileGeoG4SDCalc(name, **kwargs), primary = True )
    return result


def TileCTBGeoG4SDCalcCfg(ConfigFlags, name="TileCTBGeoG4SDCalc", **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("TileTB", True)
    kwargs.setdefault("DoCalibHitParticleID", ConfigFlags.Sim.ParticleID )

    result.addService( TileGeoG4SDCalc(name, **kwargs), primary = True )
    return result
