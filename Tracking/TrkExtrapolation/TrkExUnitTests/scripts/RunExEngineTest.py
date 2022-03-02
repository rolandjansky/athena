from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg    
from AthenaCommon.Configurable import Configurable
from AthenaCommon.Logging import log

Configurable.configurableRun3Behavior = True

## Just enable ID for the moment.
ConfigFlags.Input.isMC             = True

ConfigFlags.IOVDb.GlobalTag        = "OFLCOND-SIM-00-00-00"    
ConfigFlags.GeoModel.AtlasVersion  = "ATLAS-R2-2016-01-00-01"
ConfigFlags.Detector.GeometryBpipe = True
ConfigFlags.Detector.GeometryID    = True
ConfigFlags.Detector.GeometryPixel = True
ConfigFlags.Detector.GeometrySCT   = True

ConfigFlags.Detector.GeometryTRT   = False
ConfigFlags.Detector.GeometryCalo  = False
ConfigFlags.Detector.GeometryMuon  = False

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
                                   EtaMin                  =  -3.,
                                   EtaMax                  =   3.,
                                   # Configure how you wanna run                  
                                   CollectSensitive        = True,
                                   CollectPassive          = True,
                                   CollectBoundary         = True,
                                   CollectMaterial         = True,
                                   UseHGTD                 = False,
                                   # the path limit to test                        
                                   PathLimit               = -1.,
                                   )
cfg.merge(topoAcc)

cfg.printConfig()

cfg.run(10)
f=open("ExtrapolationEngineTestConfig.pkl","wb")
cfg.store(f)
f.close()
