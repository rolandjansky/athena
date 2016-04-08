# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getSctSensorSD(name="SctSensorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["SCT::BRLSensor","SCT::ECSensor0","SCT::ECSensor1",
                                             "SCT::ECSensor2","SCT::ECSensor3"])
    kwargs.setdefault("OutputCollectionNames", ["SCT_Hits"])
    return CfgMgr.SctSensorSDTool(name, **kwargs)


def getSLHC_SctSensorSD(name="SLHC_SctSensorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["SCT::BRLSensor","SCT::BRLSensorSS","SCT::BRLSensorMS",
                                             "SCT::ECSensor0","SCT::ECSensor1","SCT::ECSensor2",
                                             "SCT::ECSensor3","SCT::ECSensor4","SCT::ECSensor5"])
    return getSctSensorSD(name, **kwargs)


def getSLHC_SctSensorSD_Gmx(name="SLHC_SctSensorSD_Gmx", **kwargs):
    kwargs.setdefault("GmxSensor", True )
    return getSLHC_SctSensorSD(name, **kwargs)


def getSctSensor_CTB(name="SctSensor_CTB", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["SCT::ECSensor0"])
    kwargs.setdefault("OutputCollectionNames", ["SCT_Hits"])
    #kwargs.setdefault("OutputLevel",  1 )
    return CfgMgr.SctSensor_CTBTool(name, **kwargs)
