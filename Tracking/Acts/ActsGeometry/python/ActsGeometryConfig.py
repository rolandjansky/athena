# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.ComponentFactory import CompFactory


def ActsTrackingGeometrySvcCfg(configFlags, name = "ActsTrackingGeometrySvc", **kwargs) :
  result = ComponentAccumulator()

  Acts_ActsTrackingGeometrySvc = CompFactory.ActsTrackingGeometrySvc
  subDetectors = []
  if configFlags.Detector.GeometryBpipe:
    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    result.merge(BeamPipeGeometryCfg(configFlags))
  if configFlags.Detector.GeometryPixel:
    subDetectors += ["Pixel"]
  if configFlags.Detector.GeometrySCT and configFlags.Acts.TrackingGeometry.buildAllAvailableSubDetectors:
    subDetectors += ["SCT"]
  if configFlags.Detector.GeometryTRT and configFlags.Acts.TrackingGeometry.buildAllAvailableSubDetectors:
    # Commented out because TRT is not production ready yet and we don't 
    # want to turn it on even if the global flag is set
    #  subDetectors += ["TRT"]
    pass

  if configFlags.Detector.GeometryCalo and configFlags.Acts.TrackingGeometry.buildAllAvailableSubDetectors:
    # Commented out because Calo is not production ready yet and we don't 
    # want to turn it on even if the global flag is set
    #  subDetectors += ["Calo"]

    # need to configure calo geometry, otherwise we get a crash
    # Do this even though it's not production ready yet, so the service can
    # be forced to build the calorimeter later on anyway
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    result.merge(LArGMCfg(configFlags))
    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(TileGMCfg(configFlags))

  if configFlags.Detector.GeometryITkPixel:
    subDetectors += ["ITkPixel"]
  if configFlags.Detector.GeometryITkStrip:
    subDetectors += ["ITkStrip"]


  if configFlags.Detector.GeometryBpipe:
    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    result.merge(BeamPipeGeometryCfg(configFlags))
    kwargs.setdefault("BuildBeamPipe", True)


  idSub = [sd in subDetectors for sd in ("Pixel", "SCT", "TRT", "ITkPixel", "ITkStrip")]
  if any(idSub):
    # ANY of the ID subdetectors are on => we require GM sources
    from InDetConfig.InDetGeometryConfig import InDetGeometryCfg
    result.merge(InDetGeometryCfg(configFlags))

  if "Calo" in subDetectors:
    # in case Calo is enabled, we need this tool
    kwargs.setdefault("CaloVolumeBuilder", CompFactory.ActsCaloTrackingVolumeBuilder())

  actsTrackingGeometrySvc = Acts_ActsTrackingGeometrySvc(name,
                                                         BuildSubDetectors=subDetectors,
                                                         **kwargs)


  if configFlags.Acts.TrackingGeometry.MaterialSource == "Input":
    actsTrackingGeometrySvc.UseMaterialMap = True
    actsTrackingGeometrySvc.MaterialMapInputFile = "material-maps.json"
  if configFlags.Acts.TrackingGeometry.MaterialSource.find(".json") != -1:  
    actsTrackingGeometrySvc.UseMaterialMap = True
    actsTrackingGeometrySvc.MaterialMapInputFile = configFlags.Acts.TrackingGeometry.MaterialSource
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

  result.merge(ActsAlignmentCondAlgCfg(configFlags))
  
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
  
  if configFlags.Detector.GeometryITk:
    from PixelConditionsAlgorithms.ITkPixelConditionsConfig import ITkPixelAlignCondAlgCfg
    result.merge(ITkPixelAlignCondAlgCfg(configFlags))

    from SCT_ConditionsAlgorithms.ITkStripConditionsAlgorithmsConfig import ITkStripAlignCondAlgCfg
    result.merge(ITkStripAlignCondAlgCfg(configFlags))

    kwargs.setdefault("PixelAlignStoreReadKey", "ITkPixelAlignmentStore")
    kwargs.setdefault("SCTAlignStoreReadKey", "ITkStripAlignmentStore")
  else:
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelAlignCondAlgCfg
    result.merge(PixelAlignCondAlgCfg(configFlags))

    from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConfig import SCT_AlignCondAlgCfg
    result.merge(SCT_AlignCondAlgCfg(configFlags))

    kwargs.setdefault("PixelAlignStoreReadKey", "PixelAlignmentStore")
    kwargs.setdefault("SCTAlignStoreReadKey", "SCTAlignmentStore")

  
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


def ActsMaterialTrackWriterSvcCfg(configFlags,
                                  name="ActsMaterialTrackWriterSvc",
                                  FilePath="MaterialTracks_mapping.root",
                                  TreeName="material-tracks") :
  result = ComponentAccumulator()

  acc = ActsTrackingGeometrySvcCfg(configFlags)
  result.merge(acc)

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

def ActsVolumeMappingToolCfg(configFlags, name = "ActsVolumeMappingTool" ) :
  result=ComponentAccumulator()
    
  acc, actsTrackingGeometryTool = ActsTrackingGeometryToolCfg(configFlags) 
  result.merge(acc)

  Acts_ActsVolumeMappingTool = CompFactory.ActsVolumeMappingTool
  ActsVolumeMappingTool = Acts_ActsVolumeMappingTool(name)

  from AthenaCommon.Constants import INFO
  ActsVolumeMappingTool.OutputLevel = INFO

  result.addPublicTool(ActsVolumeMappingTool, primary=True)
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


def ActsExtrapolationAlgCfg(configFlags, name = "ActsExtrapolationAlg", **kwargs):
  result = ComponentAccumulator()

  if "ExtrapolationTool" not in kwargs:
    extrapTool = ActsExtrapolationToolCfg(configFlags)
    kwargs["ExtrapolationTool"] = extrapTool.getPrimary()
    result.merge(extrapTool)

  propStepWriterSvc = ActsPropStepRootWriterSvcCfg(configFlags)
  result.merge(propStepWriterSvc)

  ActsExtrapolationAlg = CompFactory.ActsExtrapolationAlg
  alg = ActsExtrapolationAlg(name, **kwargs)
  result.addEventAlgo(alg)

  return result
