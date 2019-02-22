# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

from BLM_G4_SD.BLM_G4_SDConf import BLMSensorSDTool


def BLMSensorSDCfg(ConfigFlags, name="BLMSensorSD", **kwargs):
    
    result = ComponentAccumulator()
    bare_collection_name = "BLMHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "BLMHits"
    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::blmDiamondLog"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.merge(acc)
    return result, BLMSensorSDTool(name, **kwargs)
