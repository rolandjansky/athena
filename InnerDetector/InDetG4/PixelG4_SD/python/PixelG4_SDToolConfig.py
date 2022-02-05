# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Algorithms.CollectionMergerConfig import CollectionMergerCfg


def PixelSensorSDCfg(ConfigFlags, name="PixelSensorSD", **kwargs):
    bare_collection_name = "PixelHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "PixelHits"
    region = "ID"

    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                    bare_collection_name,
                                                    mergeable_collection_suffix,
                                                    merger_input_property,
                                                    region)
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::siBLayLog","Pixel::siLog","Pixel::dbmDiamondLog"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result = ComponentAccumulator()
    result.merge(acc)
    result.setPrivateTools(CompFactory.PixelSensorSDTool(name, **kwargs))
    return result


def PixelSensor_CTBCfg(ConfigFlags, name="PixelSensor_CTB", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::siBLayLog","Pixel::siLog"])
    kwargs.setdefault("OutputCollectionNames", ["PixelHits"])
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.PixelSensorSDTool(name, **kwargs))
    return result


def DBMSensorSDCfg(ConfigFlags, name="DBMSensorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::dbmDiamondLog"])
    kwargs.setdefault("OutputCollectionNames", ["DBMHits"])
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.PixelSensorSDTool(name, **kwargs))
    return result


def ITkPixelSensorSDCfg(ConfigFlags, name="ITkPixelSensorSD", **kwargs):
    bare_collection_name = "ITkPixelHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "ITkPixelHits"
    region = "ITk"

    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                    bare_collection_name,
                                                    mergeable_collection_suffix,
                                                    merger_input_property,
                                                    region)
    # Ensure we create a Gmx sensor 
    kwargs.setdefault("GmxSensor",True) 
    kwargs.setdefault("LogicalVolumeNames", ["ITkPixel::InnerBarrelSingleMod_Sensor",
                                             "ITkPixel::InnerRingSingleMod_Sensor",
                                             "ITkPixel::InnerQuadMod_Sensor",
                                             "ITkPixel::OuterQuadMod_Sensor",
                                             "ITkPixel::InclinedQuadMod_Sensor"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result = ComponentAccumulator()
    result.merge(acc)
    result.setPrivateTools(CompFactory.PixelSensorSDTool(name, **kwargs))
    return result

def PLRSensorSDCfg(ConfigFlags, name="PLRSensorSD", **kwargs):
    bare_collection_name = "PLRHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "PLRHits"
    region = "ITk"

    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                    bare_collection_name,
                                                    mergeable_collection_suffix,
                                                    merger_input_property,
                                                    region)
    # Ensure we create a Gmx sensor 
    kwargs.setdefault("GmxSensor",True)
    kwargs.setdefault("LogicalVolumeNames", ["PLR::PLR_Sensor"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result = ComponentAccumulator()
    result.merge(acc)
    result.setPrivateTools(CompFactory.PixelSensorSDTool(name, **kwargs))
    return result
