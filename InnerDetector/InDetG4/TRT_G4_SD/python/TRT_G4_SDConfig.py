# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from G4AtlasApps.SimFlags import simFlags

from ISF_Algorithms.collection_merger_helpers import generate_mergeable_collection_name


def getTRTSensitiveDetector(name="TRTSensitiveDetector", **kwargs):
    bare_collection_name = "TRTUncompressedHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TRTUncompressedHits"
    region = "ID"
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)
    logicalVolumeNames = ["TRT::Gas","TRT::GasMA"]
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as geoFlags
    if ( geoFlags.Run() in ["RUN2", "RUN3"] ) :
        ## RUN2 configuration
        logicalVolumeNames += ["TRT::Gas_Ar","TRT::GasMA_Ar",
                               "TRT::Gas_Kr","TRT::GasMA_Kr"]
    kwargs.setdefault("LogicalVolumeNames", logicalVolumeNames)

    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    return CfgMgr.TRTSensitiveDetectorTool(name, **kwargs)


def getTRTSensitiveDetector_CTB(name="TRTSensitiveDetector_CTB", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["TRT::GasMA"])
    kwargs.setdefault("OutputCollectionNames", ["TRTUncompressedHits"])
    return CfgMgr.TRTSensitiveDetectorTool(name, **kwargs)
