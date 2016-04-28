# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getHGTDSensorSD(name="HGTDSensorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::HGTDSiSensor0", "LArMgr::HGTDSiSensor1", "LArMgr::HGTDSiSensor2", "LArMgr::HGTDSiSensor3"])
    kwargs.setdefault("OutputCollectionNames", ["LArHitHGTD"])
    kwargs.setdefault("TimeBinType", "Uniform")
    kwargs.setdefault("TimeBinWidth", 0.005)
    kwargs.setdefault("TimeWindowMin",11.8)
    kwargs.setdefault("TimeWindowMax",14.5)
    return CfgMgr.HGTDSensorSDTool(name, **kwargs)
