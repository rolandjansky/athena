# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getBLMSensorSD(name="BLMSensorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::blmDiamondLog"])
    kwargs.setdefault("OutputCollectionNames", ["BLMHits"])
    return CfgMgr.BLMSensorSDTool(name, **kwargs)
