from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg    
from AthenaCommon.Configurable import Configurable
from AthenaCommon.Logging import log

Configurable.configurableRun3Behavior = True

## Just enable ID for the moment.
ConfigFlags.Input.isMC             = True

ConfigFlags.ITk.Geometry.AllLocal = False
if ConfigFlags.ITk.Geometry.AllLocal:
  detectors = [
    "ITkPixel",
    "ITkStrip",
    "Bpipe"
  ]
  from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
  setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)

ConfigFlags.GeoModel.AtlasVersion = "ATLAS-P2-ITK-24-00-00"
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-SIM-00-00-00"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.TrackingGeometry.MaterialSource = "Input"

ConfigFlags.Detector.GeometryCalo = False
ConfigFlags.Detector.GeometryMuon = False

# This should run serially for the moment.
ConfigFlags.Concurrency.NumThreads = 1
ConfigFlags.Concurrency.NumConcurrentEvents = 1

log.debug('Lock config flags now.')
ConfigFlags.lock()

log.debug('dumping config flags now.')
ConfigFlags.dump()

cfg=MainServicesCfg(ConfigFlags)    

from TrkExUnitTests.TrkExUnitTestsConfig import ExtrapolationEngineTestCfg
topoAcc=ExtrapolationEngineTestCfg(ConfigFlags,
                                   NumberOfTestsPerEvent   = 100,
                                   # parameters mode: 0 - neutral tracks, 1 - charged particles
                                   ParametersMode          = 1,
                                   # do the full test backwards as well
                                   BackExtrapolation       = False,
                                   # Smear the production vertex - standard primary vertex paramters
                                   SmearOrigin             = True,
                                   SimgaOriginD0           = 2./3.,
                                   SimgaOriginZ0           = 50.,
                                   # pT range for testing
                                   PtMin                   = 1000,
                                   PtMax                   = 1000,
                                   # The test range in Eta
                                   EtaMin                  =  -5.,
                                   EtaMax                  =   5.,
                                   # Configure how you wanna run
                                   CollectSensitive        = True,
                                   CollectPassive          = True,
                                   CollectBoundary         = True,
                                   CollectMaterial         = True,
                                   UseHGTD                 = ConfigFlags.Detector.GeometryHGTD,
                                   # the path limit to test
                                   PathLimit               = -1.,
                                   )

cfg.merge(topoAcc)

cfg.printConfig()

cfg.run(10)
f=open("ExtrapolationEngineTestConfig.pkl","wb")
cfg.store(f)
f.close()
