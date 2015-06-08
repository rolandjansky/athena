# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getBCMSensorSD(name="BCMSensorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::bcmDiamondLog"])
    kwargs.setdefault("OutputCollectionNames", ["BCMHits"])
    return CfgMgr.BCMSensorSDTool(name, **kwargs)
