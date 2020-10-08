"""
This job options file will run an example extrapolation using the
Acts tracking geometry and the Acts extrapolation toolchain.
"""

# start from scratch with component accumulator

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from ActsGeometry.ActsGeometryConfig import ActsExtrapolationToolCfg
from ActsGeometry.ActsGeometryConfig import ActsAlignmentCondAlgCfg

def ActsExtrapolationAlgCfg(configFlags, name = "ActsExtrapolationAlg", **kwargs):
  result = ComponentAccumulator()

  if "ExtrapolationTool" not in kwargs:
    extrapTool = ActsExtrapolationToolCfg(configFlags)
    kwargs["ExtrapolationTool"] = extrapTool.getPrimary()
    result.merge(extrapTool)

  ActsExtrapolationAlg = CompFactory.ActsExtrapolationAlg
  alg = ActsExtrapolationAlg(name, **kwargs)
  result.addEventAlgo(alg)

  return result

if "__main__" == __name__:
  from AthenaCommon.Configurable import Configurable
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import VERBOSE, INFO
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg
  from ActsGeometry.ActsGeometryConfig import ActsMaterialTrackWriterSvcCfg
  from ActsGeometry.ActsGeometryConfig import ActsExtrapolationToolCfg

  Configurable.configurableRun3Behavior = True

  ## Just enable ID for the moment.
  ConfigFlags.Input.isMC             = True
  ConfigFlags.Beam.Type = ''
  ConfigFlags.GeoModel.AtlasVersion  = "ATLAS-R2-2016-01-00-01"
  ConfigFlags.IOVDb.GlobalTag        = "OFLCOND-SIM-00-00-00"
  ConfigFlags.Detector.SimulateBpipe = True
  ConfigFlags.Detector.SimulateID    = True
  ConfigFlags.Detector.GeometryBpipe = True
  ConfigFlags.Detector.GeometryID    = True
  ConfigFlags.Detector.GeometryPixel = True
  ConfigFlags.Detector.GeometrySCT   = True
  ConfigFlags.Detector.GeometryCalo  = True
  ConfigFlags.Detector.GeometryMuon  = False
  ConfigFlags.Detector.GeometryTRT   = True
  ConfigFlags.TrackingGeometry.MaterialSource = "material-maps.json"

  ConfigFlags.Concurrency.NumThreads = 10
  ConfigFlags.Concurrency.NumConcurrentEvents = 10

  ConfigFlags.lock()
  ConfigFlags.dump()

  cfg = MainServicesCfg(ConfigFlags)

  from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
  cfg.merge(BeamPipeGeometryCfg(ConfigFlags))

  alignCondAlgCfg = ActsAlignmentCondAlgCfg(ConfigFlags)

  cfg.merge(alignCondAlgCfg)

  cfg.merge(ActsMaterialTrackWriterSvcCfg("ActsMaterialTrackWriterSvc",
                                          "MaterialTracks_mapped.root"))
  print('DEF WRITER : ')
  extrapol = ActsExtrapolationToolCfg(ConfigFlags,
                                      InteractionMultiScatering = True,
                                      InteractionEloss = True,
                                      InteractionRecord = True)
  cfg.merge(extrapol)
  
  alg = ActsExtrapolationAlgCfg(ConfigFlags,
                                OutputLevel=INFO,
                                NParticlesPerEvent=int(1e4),
                                EtaRange=[-2.5, 2.5],
                                PtRange=[20, 100],
                                WriteMaterialTracks = True,
                                ExtrapolationTool=extrapol.getPrimary())

  cfg.merge(alg)

  cfg.printConfig()

  log.info("CONFIG DONE")

  cfg.run(100)
