# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.ComponentFactory import CompFactory


def ActsTrackingGeometrySvcCfg(configFlags, name = "ActsTrackingGeometrySvc" ) :
  result = ComponentAccumulator()
  
  Acts_ActsTrackingGeometrySvc = CompFactory.ActsTrackingGeometrySvc
  subDetectors = []
  if configFlags.Detector.GeometryPixel:
    subDetectors += ["Pixel"]
  if configFlags.Detector.GeometrySCT:
    subDetectors += ["SCT"]
  if configFlags.Detector.GeometryTRT:
    subDetectors += ["TRT"]
  if configFlags.Detector.GeometryCalo:
    subDetectors += ["Calo"]
    
  actsTrackingGeometrySvc = Acts_ActsTrackingGeometrySvc(name, BuildSubDetectors = subDetectors)
  result.addService(actsTrackingGeometrySvc)
  return result

def ActsTrackingGeometryToolCfg(configFlags, name = "ActsTrackingGeometryTool" ) :
  result = ComponentAccumulator()
  
  acc = ActsTrackingGeometrySvcCfg(configFlags)
  result.merge(acc)
  
  Acts_ActsTrackingGeometryTool = CompFactory.ActsTrackingGeometryTool
  actsTrackingGeometryTool = Acts_ActsTrackingGeometryTool(name)
  result.addPublicTool(actsTrackingGeometryTool)
  
  return result, actsTrackingGeometryTool

def NominalAlignmentCondAlgCfg(configFlags, name = "NominalAlignmentCondAlg", **kwargs) :
  result = ComponentAccumulator()
  
  acc = ActsTrackingGeometrySvcCfg(configFlags)
  result.merge(acc)
  
  Acts_NominalAlignmentCondAlg = CompFactory.NominalAlignmentCondAlg
  nominalAlignmentCondAlg = Acts_NominalAlignmentCondAlg(name, **kwargs)
  result.addCondAlgo(nominalAlignmentCondAlg)
  
  return result

from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
def ActsExtrapolationToolCfg(configFlags, name = "ActsExtrapolationTool" ) :
  result=ComponentAccumulator()
  
  acc  = MagneticFieldSvcCfg(configFlags)
  result.merge(acc)
  
  acc, actsTrackingGeometryTool = ActsTrackingGeometryToolCfg(configFlags) 
  result.merge(acc)
  
  Acts_ActsExtrapolationTool = CompFactory.ActsExtrapolationTool
  actsExtrapolationTool = Acts_ActsExtrapolationTool(name)
  result.addPublicTool(actsExtrapolationTool, primary=True)
  return result
