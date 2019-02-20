# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

from BCM_G4_SD.BCM_G4_SDConf import BCMSensorSDTool

def BCMSensorSDCfg(ConfigFlags, name="BCMSensorSD", **kwargs):

    result = ComponentAccumulator()
    bare_collection_name = "BCMHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "BCMHits"
    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::bcmDiamondLog"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.merge(acc)
    return result, BCMSensorSDTool(name, **kwargs)


