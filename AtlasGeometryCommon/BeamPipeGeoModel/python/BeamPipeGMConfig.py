# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AtlasGeoModel.GeoModelConfig import GeoModelCfg

def BeamPipeGeometryCfg(flags):
    result=GeoModelCfg(flags)
    from BeamPipeGeoModel.BeamPipeGeoModelConf import BeamPipeDetectorTool
    result.getPrimary().DetectorTools += [ BeamPipeDetectorTool() ]
    return result
