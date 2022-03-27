# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import LHCPeriod
from ISF_Algorithms.CollectionMergerConfig import CollectionMergerCfg


def TRTSensitiveDetectorCfg(ConfigFlags, name="TRTSensitiveDetector", **kwargs):
    bare_collection_name = "TRTUncompressedHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TRTUncompressedHits"
    region = "ID"

    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                    bare_collection_name,
                                                    mergeable_collection_suffix,
                                                    merger_input_property,
                                                    region)
    

    logicalVolumeNames = ["TRT::Gas", "TRT::GasMA"]
    if ConfigFlags.GeoModel.Run in [LHCPeriod.Run2, LHCPeriod.Run3]:  # RUN2 configuration
        logicalVolumeNames += ["TRT::Gas_Ar","TRT::GasMA_Ar",
                               "TRT::Gas_Kr","TRT::GasMA_Kr"]
    kwargs.setdefault("LogicalVolumeNames", logicalVolumeNames)
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result = ComponentAccumulator()
    result.merge(acc)
    result.setPrivateTools(CompFactory.TRTSensitiveDetectorTool(name, **kwargs))
    return result


def TRTSensitiveDetector_CTBCfg(ConfigFlags, name="TRTSensitiveDetector_CTB", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["TRT::GasMA"])
    kwargs.setdefault("OutputCollectionNames", ["TRTUncompressedHits"])
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.TRTSensitiveDetectorTool(name, **kwargs))
    return result
