# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getHGTDSensorSD(name="HGTDSensorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["HGTD::HGTDSiSensor0", "HGTD::HGTDSiSensor1", "HGTD::HGTDSiSensor2", "HGTD::HGTDSiSensor3"])
    return CfgMgr.HGTDSensorSDTool(name, **kwargs)
