# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
###############################################################
#
# Write the tracking geometry as a obj and json files.
#
###############################################################

##########################################################################
# start from scratch with component accumulator

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from ActsGeometry.ActsGeometryConfig import ActsAlignmentCondAlgCfg
from ActsGeometry.ActsGeometryConfig import ActsTrackingGeometryToolCfg
from ActsGeometry.ActsGeometryConfig import ActsMaterialJsonWriterToolCfg
from ActsGeometry.ActsGeometryConfig import ActsObjWriterToolCfg

def ActsWriteTrackingGeometryCfg(configFlags, name="ActsWriteTrackingGeometry", **kwargs):

  result = ComponentAccumulator()

  acc, actsTrackingGeometryTool = ActsTrackingGeometryToolCfg(configFlags) 
  result.merge(acc)
  ActsMaterialJsonWriterTool = ActsMaterialJsonWriterToolCfg(OutputFile = "geometry-maps.json",
                                                             processSensitives = False,
                                                             processNonMaterial = True)

  kwargs["MaterialJsonWriterTool"] = ActsMaterialJsonWriterTool.getPrimary()                                                           
  result.merge(ActsMaterialJsonWriterTool)

  ActsObjWriterTool = ActsObjWriterToolCfg(OutputDirectory = "obj",
                                           SubDetectors = ["Pixel", "SCT", "TRT"])
 
  kwargs["ObjWriterTool"] = ActsObjWriterTool.getPrimary()     
  result.merge(ActsObjWriterTool)

  ActsWriteTrackingGeometry = CompFactory.ActsWriteTrackingGeometry
  alg = ActsWriteTrackingGeometry(name, **kwargs)
  result.addEventAlgo(alg)

  return result

if "__main__" == __name__:
  from AthenaCommon.Configurable import Configurable
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import VERBOSE
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg

  Configurable.configurableRun3Behavior = True

  ## Just enable ID for the moment.
  ConfigFlags.Input.isMC             = True
  ConfigFlags.GeoModel.AtlasVersion  = "ATLAS-R2-2016-01-00-01"
  ConfigFlags.IOVDb.GlobalTag        = "OFLCOND-SIM-00-00-00"
  ConfigFlags.Detector.GeometryBpipe = True
  ConfigFlags.Detector.GeometryID    = True
  ConfigFlags.Detector.GeometryPixel = True
  ConfigFlags.Detector.GeometrySCT   = True
  ConfigFlags.Detector.GeometryCalo  = True
  ConfigFlags.Detector.GeometryMuon  = False
  ConfigFlags.Detector.GeometryTRT   = True
  ConfigFlags.Acts.TrackingGeometry.buildAllAvailableSubDetectors = True

  ConfigFlags.Concurrency.NumThreads = 1
  ConfigFlags.Concurrency.NumConcurrentEvents = 1

  ConfigFlags.lock()
  ConfigFlags.dump()

  cfg = MainServicesCfg(ConfigFlags)

  from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
  cfg.merge(BeamPipeGeometryCfg(ConfigFlags))

  alignCondAlgCfg = ActsAlignmentCondAlgCfg(ConfigFlags)

  cfg.merge(alignCondAlgCfg)

  alg = ActsWriteTrackingGeometryCfg(ConfigFlags,
                                     OutputLevel=VERBOSE)

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

  cfg.run(1)
