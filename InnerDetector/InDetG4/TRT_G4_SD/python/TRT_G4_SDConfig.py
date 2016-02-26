# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getTRTSensitiveDetector(name="TRTSensitiveDetector", **kwargs):
    logicalVolumeNames = ["TRT::Gas","TRT::GasMA"]
    from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags
    if geoFlags.Run()=="RUN2":
        ## RUN2 configuration
        logicalVolumeNames += ["TRT::Gas_Ar","TRT::GasMA_Ar",
                               "TRT::Gas_Kr","TRT::GasMA_Kr"]
    kwargs.setdefault("LogicalVolumeNames", logicalVolumeNames)

    kwargs.setdefault("OutputCollectionNames", ["TRTUncompressedHits"])
    return CfgMgr.TRTSensitiveDetectorTool(name, **kwargs)

def getTRTSensitiveDetector_CTB(name="TRTSensitiveDetector_CTB", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["TRT::GasMA"])
    kwargs.setdefault("OutputCollectionNames", ["TRTUncompressedHits"])
    return CfgMgr.TRTSensitiveDetectorTool(name, **kwargs)

