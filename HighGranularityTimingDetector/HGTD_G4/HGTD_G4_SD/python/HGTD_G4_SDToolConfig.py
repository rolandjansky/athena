# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Algorithms.CollectionMergerConfig import CollectionMergerCfg


def HgtdSensorSDCfg(ConfigFlags, name="HgtdSensorSD", **kwargs):
    bare_collection_name = "HGTD_Hits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "HGTDHits"
    region = "ID"

    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                    bare_collection_name,
                                                    mergeable_collection_suffix,
                                                    merger_input_property,
                                                    region)
    # TODO: for running GMX implementation
    # kwargs.setdefault("GmxSensor",True)
    # kwargs.setdefault("LogicalVolumeNames", ["HGTD::HGTDSiSensor"])
    kwargs.setdefault("LogicalVolumeNames", ["HGTD::HGTDSiSensor0", "HGTD::HGTDSiSensor1", "HGTD::HGTDSiSensor2", "HGTD::HGTDSiSensor3"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result = ComponentAccumulator()
    result.merge(acc)
    result.setPrivateTools(CompFactory.HGTDSensorSDTool(name, **kwargs))
    return result
