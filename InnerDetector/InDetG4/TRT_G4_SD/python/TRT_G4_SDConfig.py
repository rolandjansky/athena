# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getTRTSensitiveDetector(name="TRTSensitiveDetector", **kwargs):
    from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags
    if geoFlags.Run()=="RUN2":
        ## RUN2 configuration
        kwargs.setdefault("LogicalVolumeNames", ["TRT::Gas","TRT::GasMA","TRT::Gas_Ar","TRT::GasMA_Ar"])
        ## Will have to extend this in the future to cope with possible Krypton filled volumes.
    else:
        ## RUN1 configuration
        kwargs.setdefault("LogicalVolumeNames", ["TRT::Gas","TRT::GasMA"])
    kwargs.setdefault("OutputCollectionNames", ["TRTUncompressedHits"])
    return CfgMgr.TRTSensitiveDetectorTool(name, **kwargs)
