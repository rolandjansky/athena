# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
###############################################################
#
# Map material from a Geantino scan onto the surfaces and 
# volumes of the detector to creat a material map.
#
###############################################################


##########################################################################
# start from scratch with component accumulator

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from ActsGeometry.ActsGeometryConfig import ActsMaterialStepConverterToolCfg
from ActsGeometry.ActsGeometryConfig import ActsSurfaceMappingToolCfg, ActsVolumeMappingToolCfg
from ActsGeometry.ActsGeometryConfig import ActsMaterialJsonWriterToolCfg

from ActsGeometry.ActsGeometryConfig import ActsAlignmentCondAlgCfg

def ActsMaterialMappingCfg(configFlags, name = "ActsMaterialMapping", **kwargs):
  result = ComponentAccumulator()

  MaterialStepConverterTool = ActsMaterialStepConverterToolCfg()
  kwargs["MaterialStepConverterTool"] = MaterialStepConverterTool.getPrimary()   
  result.merge(MaterialStepConverterTool)

  ActsSurfaceMappingTool = ActsSurfaceMappingToolCfg(configFlags)
  kwargs["SurfaceMappingTool"] = ActsSurfaceMappingTool.getPrimary()   
  result.merge(ActsSurfaceMappingTool)

  ActsVolumeMappingTool = ActsVolumeMappingToolCfg(configFlags)
  kwargs["VolumeMappingTool"] = ActsVolumeMappingTool.getPrimary()
  result.merge(ActsVolumeMappingTool)

  ActsMaterialJsonWriterTool = ActsMaterialJsonWriterToolCfg(OutputFile = "material-maps.json",
                                                            processSensitives = False,
                                                            processNonMaterial = False)
                                                            
  kwargs["MaterialJsonWriterTool"] = ActsMaterialJsonWriterTool.getPrimary()   
  result.merge(ActsMaterialJsonWriterTool)

  ActsMaterialMapping = CompFactory.ActsMaterialMapping
  alg = ActsMaterialMapping(name, **kwargs)
  result.addEventAlgo(alg)

  return result

if "__main__" == __name__:
  from AthenaCommon.Configurable import Configurable
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import INFO
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg
  from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
  from ActsGeometry.ActsGeometryConfig import ActsMaterialTrackWriterSvcCfg
  Configurable.configurableRun3Behavior = True

  ## Just enable ID for the moment.
  ConfigFlags.Input.isMC             = True
  ConfigFlags.Beam.Type = ''
  ConfigFlags.GeoModel.AtlasVersion  = "ATLAS-R2-2016-01-00-01"
  ConfigFlags.IOVDb.GlobalTag        = "OFLCOND-SIM-00-00-00"
  ConfigFlags.Detector.GeometryBpipe = True
  ConfigFlags.Detector.GeometryID    = True
  ConfigFlags.Detector.GeometryPixel = True
  ConfigFlags.Detector.GeometrySCT   = True
  ConfigFlags.Detector.GeometryCalo  = True
  ConfigFlags.Detector.GeometryMuon  = False
  ConfigFlags.Detector.GeometryTRT   = True
  ConfigFlags.Acts.TrackingGeometry.MaterialSource = "geometry-maps.json"
  # ConfigFlags.Acts.TrackingGeometry.MaterialSource = "/eos/project-a/acts/public/MaterialMaps/ATLAS/geometry-maps.json"
  ConfigFlags.Acts.TrackingGeometry.buildAllAvailableSubDetectors = True
  ConfigFlags.Concurrency.NumThreads = 1
  ConfigFlags.Concurrency.NumConcurrentEvents = 1

  ConfigFlags.lock()
  ConfigFlags.dump()

  cfg = MainServicesCfg(ConfigFlags)

  cfg.merge(ActsMaterialTrackWriterSvcCfg(ConfigFlags,
                                          "ActsMaterialTrackWriterSvc",
                                          "MaterialTracks_mapping.root"))

  cfg.merge(PoolReadCfg(ConfigFlags))
  eventSelector = cfg.getService("EventSelector")
  eventSelector.InputCollections = ["MaterialStepFile.root"]

  from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
  cfg.merge(BeamPipeGeometryCfg(ConfigFlags))

  alignCondAlgCfg = ActsAlignmentCondAlgCfg(ConfigFlags)

  cfg.merge(alignCondAlgCfg)

  alg = ActsMaterialMappingCfg(ConfigFlags,
                               OutputLevel=INFO,
                               mapSurfaces = True,
                               mapVolumes = True)

  cfg.merge(alg)

  tgSvc = cfg.getService("ActsTrackingGeometrySvc")

  # Service will have removed TRT and Calo
  # We want them enabled for testing
  tgSvc.BuildSubDetectors += [
    "TRT",
    "Calo"
  ]
  # needed to construct the calo geometry in ACTS
  tgSvc.CaloVolumeBuilder = CompFactory.ActsCaloTrackingVolumeBuilder()

  cfg.printConfig()

  log.info("CONFIG DONE")

  cfg.run(80000)
