# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from G4AtlasApps.SimFlags import simFlags

from ISF_Algorithms.collection_merger_helpers import generate_mergeable_collection_name


def getPixelSensorSD(name="PixelSensorSD", **kwargs):
    bare_collection_name = "PixelHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "PixelHits"
    region = "ID"
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::siBLayLog","Pixel::siLog","Pixel::dbmDiamondLog"])
    #kwargs.setdefault("LogicalVolumeNames", ["Pixel::siBLayLog","Pixel::siLog"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    return CfgMgr.PixelSensorSDTool(name, **kwargs)

def getPixelSensor_CTB(name="PixelSensor_CTB", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::siBLayLog","Pixel::siLog"])
    kwargs.setdefault("OutputCollectionNames", ["PixelHits"])
    return CfgMgr.PixelSensorSDTool(name, **kwargs)

def getDBMSensorSD(name="DBMSensorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::dbmDiamondLog"])
    kwargs.setdefault("OutputCollectionNames", ["DBMHits"])
    return CfgMgr.PixelSensorSDTool(name, **kwargs)
