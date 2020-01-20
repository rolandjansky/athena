# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from AthenaConfiguration.ComponentFactory import CompFactory

def BeamPipeGeometryCfg(flags):
    result=GeoModelCfg(flags)
    BeamPipeDetectorTool=CompFactory.BeamPipeDetectorTool
    result.getPrimary().DetectorTools += [ BeamPipeDetectorTool() ]
    return result
