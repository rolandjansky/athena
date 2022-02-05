# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Algorithms.CollectionMergerConfig import CollectionMergerCfg


def SctSensorSDCfg(ConfigFlags, name="SctSensorSD", **kwargs):
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

    result = ComponentAccumulator()
    result.merge(acc)
    result.setPrivateTools(CompFactory.SctSensorSDTool(name, **kwargs))
    return result


def SctSensor_CTBCfg(ConfigFlags, name="SctSensor_CTB", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["SCT::ECSensor0"])
    kwargs.setdefault("OutputCollectionNames", ["SCT_Hits"])
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.SctSensor_CTBTool(name, **kwargs))
    return result


def ITkStripSensorSDCfg(ConfigFlags, name="ITkStripSensorSD", **kwargs):
    bare_collection_name = "ITkStripHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "ITkStripHits"
    region = "ITk"

    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                    bare_collection_name,
                                                    mergeable_collection_suffix,
                                                    merger_input_property,
                                                    region)
    kwargs.setdefault("GmxSensor", True)
    kwargs.setdefault("LogicalVolumeNames", ["ITkStrip::BRLSensorSS","ITkStrip::BRLSensorMS",
                                             "ITkStrip::ECSensor0","ITkStrip::ECSensor1","ITkStrip::ECSensor2",
                                             "ITkStrip::ECSensor3","ITkStrip::ECSensor4","ITkStrip::ECSensor5"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result = ComponentAccumulator()
    result.merge(acc)
    result.setPrivateTools(CompFactory.SctSensorSDTool(name, **kwargs))
    return result
