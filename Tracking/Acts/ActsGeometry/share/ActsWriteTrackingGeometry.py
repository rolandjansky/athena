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
                                                             processnonmaterial = True)

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
  from AthenaCommon.Constants import VERBOSE, INFO
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg

  Configurable.configurableRun3Behavior = True

  ## Just enable ID for the moment.
  ConfigFlags.Input.isMC             = True
  ConfigFlags.Beam.Type = ''
  ConfigFlags.GeoModel.AtlasVersion  = "ATLAS-R2-2016-01-00-01"
  ConfigFlags.IOVDb.GlobalTag        = "OFLCOND-SIM-00-00-00"
  ConfigFlags.Detector.SimulateBpipe = False
  ConfigFlags.Detector.SimulateID    = False
  ConfigFlags.Detector.GeometryBpipe = True
  ConfigFlags.Detector.GeometryID    = True
  ConfigFlags.Detector.GeometryPixel = True
  ConfigFlags.Detector.GeometrySCT   = True
  ConfigFlags.Detector.GeometryCalo  = True
  ConfigFlags.Detector.GeometryMuon  = False
  ConfigFlags.Detector.GeometryTRT   = True

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

  cfg.printConfig()

  log.info("CONFIG DONE")

  cfg.run(1)