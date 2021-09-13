# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.ComponentFactory import CompFactory


def PositionMomentumWriterCfg(configFlags, name="PositionMomentumWriter", **kwargs) :
  result = ComponentAccumulator()

  Trk__PositionMomentumWriter = CompFactory.Trk.PositionMomentumWriter
  posMomWriter = Trk__PositionMomentumWriter(name, **kwargs)
  result.addPublicTool(posMomWriter)
  #result.setPrivateTools(posMomWriter)
  return result, posMomWriter

def ExtrapolationEngineTestCfg(configFlags, name = "ExtrapolationEngineTest", **kwargs ) :
  result=ComponentAccumulator()  
  
  from AtlasGeoModel.GeoModelConfig import GeoModelCfg
  gmsAcc = GeoModelCfg(configFlags)
  result.merge(gmsAcc)

  #from TrkConfig.AtlasExtrapolationEngineConfig import AtlasExtrapolationEngineCfg
  #kwargs["ExtrapolationEngine"] = result.popToolsAndMerge(AtlasExtrapolationEngineCfg(configFlags, nameprefix=''))
  
  from TrkConfig.AtlasExtrapolationEngineConfig import AtlasExtrapolationEngineCfg
  extrapAcc = AtlasExtrapolationEngineCfg(configFlags)
  extrapolationEngine = extrapAcc.getPrimary()
  result.merge(extrapAcc)
  kwargs["ExtrapolationEngine"] = extrapolationEngine

  posMomAcc, posMomWriter = PositionMomentumWriterCfg(configFlags)
  result.merge(posMomAcc)
  kwargs.setdefault('PositionMomentumWriter', posMomWriter)
     
  Trk__ExtrapolationEngineTest = CompFactory.Trk.ExtrapolationEngineTest
  extrapolationTest = Trk__ExtrapolationEngineTest(name, **kwargs)
  result.addEventAlgo(extrapolationTest)
  
  return result

if __name__=="__main__":
    from AthenaCommon.Configurable import Configurable
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import VERBOSE
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
    Configurable.configurableRun3Behavior = True
    
    ## Just enable ID for the moment.
    ConfigFlags.Input.isMC             = True
    
    ConfigFlags.GeoModel.useLocalGeometry = True
    detectors = [
      "ITkPixel",
      "ITkStrip"
    ]
    
    from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)

    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-P2-ITK-24-00-00"
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-SIM-00-00-00"
    ConfigFlags.GeoModel.Align.Dynamic = False
    ConfigFlags.TrackingGeometry.MaterialSource = "Input"
    ConfigFlags.Beam.Type =''
    
    ConfigFlags.Detector.GeometryCalo  = False
    ConfigFlags.Detector.GeometryMuon  = False
    
    # This should run serially for the moment.
    ConfigFlags.Concurrency.NumThreads = 1
    ConfigFlags.Concurrency.NumConcurrentEvents = 1
    
    log.debug('Lock config flags now.')
    ConfigFlags.lock()
    
    log.debug('dumping config flags now.')
    ConfigFlags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg=MainServicesCfg(ConfigFlags)
    
    from AthenaConfiguration.ComponentFactory import CompFactory
    
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelGeometryCfg
    itkPixel = ITkPixelGeometryCfg(ConfigFlags)
    cfg.merge(itkPixel)
    
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripGeometryCfg
    itkStrip = ITkStripGeometryCfg(ConfigFlags)
    cfg.merge(itkStrip)
    
    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    cfg.merge(BeamPipeGeometryCfg(ConfigFlags))
    
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    cfg.merge(TrackingGeometrySvcCfg(ConfigFlags))
    
    histSvc = CompFactory.THistSvc(Output = ["val DATAFILE='ExtrapolationEngineTest.root' TYPE='ROOT' OPT='RECREATE'"])
    histSvc.OutputLevel=VERBOSE
    cfg.addService( histSvc )
    
    topoAcc=ExtrapolationEngineTestCfg(ConfigFlags,
                                       NumberOfTestsPerEvent   = 100,
                                       # parameters mode: 0 - neutral tracks, 1 - charged particles 
                                       ParametersMode          = 1,
                                       # do the full test backwards as well            
                                       BackExtrapolation       = False,
                                       # Smear the production vertex - standard primary vertex paramters
                                       SmearOrigin             = False,
                                       SimgaOriginD0           = 2./3.,
                                       SimgaOriginZ0           = 50.,
                                       SmearFlatOriginZ0       = False,
                                       Z0Min                   = -150.,
                                       Z0Max                   =  150.,
                                       Z0Values                = [-150., 0., 150.],
                                       SmearFlatOriginD0       = False,
                                       D0Min                   = -2.0,
                                       D0Max                   =  2.0,
                                       # pT range for testing
                                       PtMin                   = 1000,
                                       PtMax                   = 1000,
                                       # The test range in Eta                      
                                       EtaMin                  =  -5.,
                                       EtaMax                  =   5.,
                                       #EtaMin                  =  -2.0
                                       #EtaMax                  =   2.0
                                       #PhiMin                  =  -0.5
                                       #PhiMax                  =   0.5
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
    
    #vp1 = CompFactory.VP1Alg() 
    #cfg.addEventAlgo(vp1)

    cfg.run(1000)
    f=open("ExtrapolationEngineTestConfig.pkl","wb")
    cfg.store(f)
    f.close()
