# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

from PixelG4_SD.PixelG4_SDConf import PixelSensorSDTool


def PixelSensorSDCfg(ConfigFlags, name="PixelSensorSD", **kwargs):

    result = ComponentAccumulator()
    bare_collection_name = "PixelHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "PixelHits"
    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::siBLayLog","Pixel::siLog","Pixel::dbmDiamondLog"])
    #kwargs.setdefault("LogicalVolumeNames", ["Pixel::siBLayLog","Pixel::siLog"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.merge(acc)
    return result, PixelSensorSDTool(name, **kwargs)

def PixelSensor_CTBCfg(name="PixelSensor_CTB", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::siBLayLog","Pixel::siLog"])
    kwargs.setdefault("OutputCollectionNames", ["PixelHits"])
    return PixelSensorSDTool(name, **kwargs)

def DBMSensorSDCfg(name="DBMSensorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::dbmDiamondLog"])
    kwargs.setdefault("OutputCollectionNames", ["DBMHits"])
    return PixelSensorSDTool(name, **kwargs)


