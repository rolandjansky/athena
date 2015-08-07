# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getPixelSensorSD(name="PixelSensorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::siBLayLog","Pixel::siLog","Pixel::dbmDiamondLog"])
    #kwargs.setdefault("LogicalVolumeNames", ["Pixel::siBLayLog","Pixel::siLog"])
    kwargs.setdefault("OutputCollectionNames", ["PixelHits"])
    return CfgMgr.PixelSensorSDTool(name, **kwargs)

def getPixelSensor_CTB(name="PixelSensor_CTB", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::siBLayLog","Pixel::siLog"])
    kwargs.setdefault("OutputCollectionNames", ["PixelHits"])
    return CfgMgr.PixelSensorSDTool(name, **kwargs)

def getDBMSensorSD(name="DBMSensorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::dbmDiamondLog"])
    kwargs.setdefault("OutputCollectionNames", ["DBMHits"])
    return CfgMgr.PixelSensorSDTool(name, **kwargs)
