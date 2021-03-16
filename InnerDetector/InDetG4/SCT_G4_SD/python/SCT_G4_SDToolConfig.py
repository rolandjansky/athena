# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

SctSensorSDTool=CompFactory.SctSensorSDTool
SctSensor_CTBTool=CompFactory.SctSensor_CTBTool


def SctSensorSDCfg(ConfigFlags, name="SctSensorSD", **kwargs):

    result = ComponentAccumulator()
    bare_collection_name = "SCT_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "SCTHits"
    region = "ID"
    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                    bare_collection_name,
                                                    mergeable_collection_suffix,
                                                    merger_input_property,
                                                    region)
    kwargs.setdefault("LogicalVolumeNames", ["SCT::BRLSensor","SCT::ECSensor0","SCT::ECSensor1",
                                              "SCT::ECSensor2","SCT::ECSensor3"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.merge(acc)
    return result, SctSensorSDTool(name, **kwargs)


def ITkStripSensorSDCfg(ConfigFlags, name="ITkStripSensorSD", **kwargs):

    kwargs.setdefault("LogicalVolumeNames", ["ITkStrip::BRLSensorSS","ITkStrip::BRLSensorMS",
                                             "ITkStrip::ECSensor0","ITkStrip::ECSensor1","ITkStrip::ECSensor2",
                                             "ITkStrip::ECSensor3","ITkStrip::ECSensor4","ITkStrip::ECSensor5"])
    kwargs.setdefault("GmxSensor", True )
    return SctSensorSDCfg(ConfigFlags, name, **kwargs)


def SctSensor_CTBCfg(name="SctSensor_CTB", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["SCT::ECSensor0"])
    kwargs.setdefault("OutputCollectionNames", ["SCT_Hits"])
    return SctSensor_CTBTool(name, **kwargs)
