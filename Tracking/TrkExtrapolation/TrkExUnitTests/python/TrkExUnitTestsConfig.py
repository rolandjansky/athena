# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""Define methods to configure TrkExUnitTest"""

def PositionMomentumWriterCfg(configFlags, name="PositionMomentumWriter", **kwargs) :
  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
  from AthenaConfiguration.ComponentFactory import CompFactory

  result = ComponentAccumulator()

  Trk__PositionMomentumWriter = CompFactory.Trk.PositionMomentumWriter
  posMomWriter = Trk__PositionMomentumWriter(name, **kwargs)
  result.addPublicTool(posMomWriter)
  #result.setPrivateTools(posMomWriter)
  return result, posMomWriter

def ExtrapolationEngineTestCfg(configFlags, name = "ExtrapolationEngineTest", **kwargs ) :
  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
  from AthenaConfiguration.ComponentFactory import CompFactory
  
  result=ComponentAccumulator()  
  
  from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg     
  result.merge(InDetGeometryCfg(configFlags))
  
  from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
  result.merge(BeamPipeGeometryCfg(configFlags))
  
  from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
  result.merge(TrackingGeometrySvcCfg(configFlags))
  
  histSvc = CompFactory.THistSvc(Output = ["val DATAFILE='ExtrapolationEngineTest.root' TYPE='ROOT' OPT='RECREATE'"])
  result.addService( histSvc )    
    
  from AtlasGeoModel.GeoModelConfig import GeoModelCfg
  gmsAcc = GeoModelCfg(configFlags)
  result.merge(gmsAcc)

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

def ExtrapolationEngineTestITkCfg(configFlags, name = "ExtrapolationEngineTest", **kwargs ) :
  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
  from AthenaConfiguration.ComponentFactory import CompFactory
  
  result=ComponentAccumulator()  
  
  from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelGeometryCfg
  itkPixel = ITkPixelGeometryCfg(configFlags)
  result.merge(itkPixel)
  
  from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripGeometryCfg
  itkStrip = ITkStripGeometryCfg(configFlags)
  result.merge(itkStrip)
  
  from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
  result.merge(BeamPipeGeometryCfg(configFlags))
  
  from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
  result.merge(TrackingGeometrySvcCfg(configFlags))
  
  histSvc = CompFactory.THistSvc(Output = ["val DATAFILE='ExtrapolationEngineTestITk.root' TYPE='ROOT' OPT='RECREATE'"])
  result.addService( histSvc )
  
  from AtlasGeoModel.GeoModelConfig import GeoModelCfg
  gmsAcc = GeoModelCfg(configFlags)
  result.merge(gmsAcc)

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
