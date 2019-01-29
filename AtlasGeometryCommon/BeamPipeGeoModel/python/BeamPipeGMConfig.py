# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg

def BeamPipeGeometryCfg(flags):
    result,gms=GeoModelCfg(flags)
    from BeamPipeGeoModel.BeamPipeGeoModelConf import BeamPipeDetectorTool
    gms.DetectorTools += [ BeamPipeDetectorTool() ]
    result.addService(gms)
    return result
