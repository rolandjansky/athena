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

    # need to configure calo geometry, otherwise we get a crash
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    result.merge(LArGMCfg(configFlags))
    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(TileGMCfg(configFlags))

  idSub = [sd in subDetectors for sd in ("Pixel", "SCT", "TRT")]
  if any(idSub):
    # ANY of the ID subdetectors are on => we require GM sources
    # In principle we could require only what is enabled, but the group
    # does extra config that I don't want to duplicate here
    from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
    result.merge(InDetGeometryCfg(configFlags))
    
    if not all(idSub):
      from AthenaCommon.Logging import log
      log.warning("ConfigFlags indicate %s should be built. Not all ID subdetectors are set, but I'll set all of them up to capture the extra setup happening here.", ", ".join(subDetectors))
      

    
  actsTrackingGeometrySvc = Acts_ActsTrackingGeometrySvc(name, BuildSubDetectors = subDetectors)

  from AthenaCommon.Constants import VERBOSE
  actsTrackingGeometrySvc.OutputLevel = VERBOSE
  result.addService(actsTrackingGeometrySvc)
  return result

def ActsPropStepRootWriterSvcCfg(configFlags, 
                                 name="ActsPropStepRootWriterSvc",
                                 FilePath="propsteps.root",
                                 TreeName="propsteps"):
    result = ComponentAccumulator()

    ActsPropStepRootWriterSvc = CompFactory.ActsPropStepRootWriterSvc
    svc = ActsPropStepRootWriterSvc(name=name, 
                                    FilePath=FilePath, 
                                    TreeName=TreeName)

    result.addService(svc)

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

def ActsAlignmentCondAlgCfg(configFlags, name = "ActsAlignmentCondAlg", **kwargs) :
  result = ComponentAccumulator()
  
  acc = ActsTrackingGeometrySvcCfg(configFlags)
  result.merge(acc)
  
  Acts_ActsAlignmentCondAlg = CompFactory.ActsAlignmentCondAlg
  actsAlignmentCondAlg = Acts_ActsAlignmentCondAlg(name, **kwargs)
  result.addCondAlgo(actsAlignmentCondAlg)
  
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
