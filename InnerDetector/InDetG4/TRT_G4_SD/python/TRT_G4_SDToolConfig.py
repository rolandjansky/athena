# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

from TRT_G4_SD.TRT_G4_SDConf import TRTSensitiveDetectorTool 

def TRTSensitiveDetectorCfg(ConfigFlags, name="TRTSensitiveDetector", **kwargs):

    result = ComponentAccumulator()
    bare_collection_name = "TRTUncompressedHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TRTUncompressedHits"
    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property)
    result.merge(acc)

    logicalVolumeNames = ["TRT::Gas","TRT::GasMA"]
    if ( ConfigFlags.GeoModel.Run in ["RUN2", "RUN3"] ) :
        ## RUN2 configuration
        logicalVolumeNames += ["TRT::Gas_Ar","TRT::GasMA_Ar",
                               "TRT::Gas_Kr","TRT::GasMA_Kr"]
    kwargs.setdefault("LogicalVolumeNames", logicalVolumeNames)

    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    return result, TRTSensitiveDetectorTool(name, **kwargs)


def TRTSensitiveDetector_CTBCfg(name="TRTSensitiveDetector_CTB", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["TRT::GasMA"])
    kwargs.setdefault("OutputCollectionNames", ["TRTUncompressedHits"])
    return TRTSensitiveDetectorTool(name, **kwargs)
