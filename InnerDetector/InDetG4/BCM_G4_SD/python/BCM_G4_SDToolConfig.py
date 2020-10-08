# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

BCMSensorSDTool=CompFactory.BCMSensorSDTool

def BCMSensorSDCfg(ConfigFlags, name="BCMSensorSD", **kwargs):

    result = ComponentAccumulator()
    bare_collection_name = "BCMHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "BCMHits"
    region = "ID"
    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                    bare_collection_name,
                                                    mergeable_collection_suffix,
                                                    merger_input_property,
                                                    region)
    kwargs.setdefault("LogicalVolumeNames", ["Pixel::bcmDiamondLog"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.merge(acc)
    return result, BCMSensorSDTool(name, **kwargs)


