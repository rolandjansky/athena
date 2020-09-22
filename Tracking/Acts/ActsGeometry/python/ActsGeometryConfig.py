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
      
  actsTrackingGeometrySvc = Acts_ActsTrackingGeometrySvc(name, BuildSubDetectors=subDetectors)

  if configFlags.TrackingGeometry.MaterialSource == "Input":
    actsTrackingGeometrySvc.UseMaterialMap = True
    actsTrackingGeometrySvc.MaterialMapInputFile = "material-maps.json"
  if configFlags.TrackingGeometry.MaterialSource.find(".json") != -1:  
    actsTrackingGeometrySvc.UseMaterialMap = True
    actsTrackingGeometrySvc.MaterialMapInputFile = configFlags.TrackingGeometry.MaterialSource
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
def ActsExtrapolationToolCfg(configFlags, name="ActsExtrapolationTool", **kwargs) :
  result=ComponentAccumulator()
  
  acc  = MagneticFieldSvcCfg(configFlags)
  result.merge(acc)
  
  acc, actsTrackingGeometryTool = ActsTrackingGeometryToolCfg(configFlags) 
  result.merge(acc)
  
  Acts_ActsExtrapolationTool = CompFactory.ActsExtrapolationTool
  actsExtrapolationTool = Acts_ActsExtrapolationTool(name, **kwargs)
  result.addPublicTool(actsExtrapolationTool, primary=True)
  return result


def ActsMaterialTrackWriterSvcCfg(name="ActsMaterialTrackWriterSvc",
                                  FilePath="MaterialTracks_mapping.root",
                                  TreeName="material-tracks") :
  result = ComponentAccumulator()

  Acts_ActsMaterialTrackWriterSvc = CompFactory.ActsMaterialTrackWriterSvc
  ActsMaterialTrackWriterSvc = Acts_ActsMaterialTrackWriterSvc(name, 
                                                               FilePath=FilePath,
                                                               TreeName=TreeName)

  from AthenaCommon.Constants import INFO
  ActsMaterialTrackWriterSvc.OutputLevel = INFO
  result.addService(ActsMaterialTrackWriterSvc, primary=True)
  return result

def ActsMaterialStepConverterToolCfg(name = "ActsMaterialStepConverterTool" ) :
  result=ComponentAccumulator()
  
  Acts_ActsMaterialStepConverterTool = CompFactory.ActsMaterialStepConverterTool
  ActsMaterialStepConverterTool = Acts_ActsMaterialStepConverterTool(name)

  from AthenaCommon.Constants import INFO
  ActsMaterialStepConverterTool.OutputLevel = INFO

  result.addPublicTool(ActsMaterialStepConverterTool, primary=True)
  return result

def ActsSurfaceMappingToolCfg(configFlags, name = "ActsSurfaceMappingTool" ) :
  result=ComponentAccumulator()
    
  acc, actsTrackingGeometryTool = ActsTrackingGeometryToolCfg(configFlags) 
  result.merge(acc)

  Acts_ActsSurfaceMappingTool = CompFactory.ActsSurfaceMappingTool
  ActsSurfaceMappingTool = Acts_ActsSurfaceMappingTool(name)

  from AthenaCommon.Constants import INFO
  ActsSurfaceMappingTool.OutputLevel = INFO

  result.addPublicTool(ActsSurfaceMappingTool, primary=True)
  return result

def ActsMaterialJsonWriterToolCfg(name= "ActsMaterialJsonWriterTool", **kwargs) :
  result=ComponentAccumulator()
    
  Acts_ActsMaterialJsonWriterTool = CompFactory.ActsMaterialJsonWriterTool
  ActsMaterialJsonWriterTool = Acts_ActsMaterialJsonWriterTool(name, **kwargs)

  from AthenaCommon.Constants import INFO
  ActsMaterialJsonWriterTool.OutputLevel = INFO

  result.addPublicTool(ActsMaterialJsonWriterTool, primary=True)
  return result

def ActsObjWriterToolCfg(name= "ActsObjWriterTool", **kwargs) :
  result=ComponentAccumulator()
    
  Acts_ActsObjWriterTool = CompFactory.ActsObjWriterTool
  ActsObjWriterTool = Acts_ActsObjWriterTool(name, **kwargs)

  from AthenaCommon.Constants import INFO
  ActsObjWriterTool.OutputLevel = INFO

  result.addPublicTool(ActsObjWriterTool, primary=True)
  return result
