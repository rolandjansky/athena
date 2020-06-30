# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.ComponentFactory import CompFactory

from ActsGeometry.ActsGeometryConfig import ActsExtrapolationToolCfg
from ActsGeometry.ActsGeometryConfig import NominalAlignmentCondAlgCfg
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

def PropResultRootWriterSvcCfg(configFlags, name="PropResultRootWriterSvc", **kwargs) :
  result = ComponentAccumulator()

  Trk__PropResultRootWriterSvc = CompFactory.Trk.PropResultRootWriterSvc
  propResultRootWriterSvc = Trk__PropResultRootWriterSvc(name, **kwargs)
  result.addService(propResultRootWriterSvc)
  
  return result, propResultRootWriterSvc


def ExtrapolatorComparisonTestCfg(configFlags, name = "ExtrapolatorComparisonTest", **kwargs ) :
  result=ComponentAccumulator()  
  
  if "Extrapolator" not in kwargs:
    kwargs["Extrapolator"] = result.popToolsAndMerge(AtlasExtrapolatorCfg(configFlags, 'AtlasExtrapolator'))
  
  if "ExtrapolationTool" not in kwargs:
    actsextrapolation = ActsExtrapolationToolCfg(configFlags)
    kwargs["ExtrapolationTool"] = actsextrapolation.getPrimary()
    result.merge(actsextrapolation)
  
  Trk__ExtrapolatorComparisonTest = CompFactory.Trk.ExtrapolatorComparisonTest
  extrapolationTest = Trk__ExtrapolatorComparisonTest(name, **kwargs)
  result.addEventAlgo(extrapolationTest)
  
  return result

if __name__=="__main__":
    from AthenaCommon.Configurable import Configurable
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import VERBOSE
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg    
    
    #log.setLevel(VERBOSE)
    
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
    ConfigFlags.Detector.GeometryCalo  = False
    ConfigFlags.Detector.GeometryMuon  = False
    #ConfigFlags.Detector.GeometryTRT   = True
    ConfigFlags.TrackingGeometry.MaterialSource = "Input"
    
    # This should run serially for the moment.
    ConfigFlags.Concurrency.NumThreads = 1
    ConfigFlags.Concurrency.NumConcurrentEvents = 1
    
    log.debug('Lock config flags now.')
    ConfigFlags.lock()
    
    ConfigFlags.dump()

    cfg=MainServicesCfg(ConfigFlags)

    from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg     
    cfg.merge(InDetGeometryCfg(ConfigFlags))
    
    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    cfg.merge(BeamPipeGeometryCfg(ConfigFlags))
    
    nominalAlignmentCondAlg = NominalAlignmentCondAlgCfg(ConfigFlags, OutputLevel=VERBOSE)
    cfg.merge(nominalAlignmentCondAlg)
    
    histSvc = CompFactory.THistSvc(Output = ["ExtrapolationStudies DATAFILE='ExtrapolationStudies.root' OPT='RECREATE'"])
    histSvc.OutputLevel=VERBOSE
    cfg.addService( histSvc )
    
    accAtlasPropResult, atlasPropResultRootWriterSvc = PropResultRootWriterSvcCfg(ConfigFlags, 
                                                                                  name="ATLASPropResultRootWriterSvc",
                                                                                  TreeName="ATLAS")
    cfg.merge(accAtlasPropResult)
  
    accActsPropResult, actsPropResultRootWriterSvc = PropResultRootWriterSvcCfg(ConfigFlags, 
                                                                                name="ACTSPropResultRootWriterSvc",
                                                                                TreeName="ACTS")
    cfg.merge(accActsPropResult)
    
    topoAcc=ExtrapolatorComparisonTestCfg(ConfigFlags,
                                          EventsPerExecute = 1000,
                                          StartPerigeeMinPt   = 10000,
                                          StartPerigeeMaxPt   = 10000,
                                          ReferenceSurfaceRadius = [80],
                                          ReferenceSurfaceHalfZ  = [500],
                                          ATLASPropResultRootWriter = atlasPropResultRootWriterSvc, 
                                          ACTSPropResultRootWriter = actsPropResultRootWriterSvc)
    cfg.merge(topoAcc)
    
    cfg.printConfig()
    
    #cfg.getPublicTool("InDetLayerArrayCreator").OutputLevel=VERBOSE
    #cfg.getPublicTool("InDetTrackingVolumeArrayCreator").OutputLevel=VERBOSE
    #cfg.getPublicTool("InDetCylinderVolumeCreator").OutputLevel=VERBOSE
    cfg.getPublicTool("InDetCylinderVolumeCreator").LayerArrayCreator=cfg.getPublicTool("InDetLayerArrayCreator")
    #cfg.getPublicTool("InDetTrackingGeometryBuilder").OutputLevel=VERBOSE
    #cfg.getService("AtlasTrackingGeometrySvc").OutputLevel=VERBOSE
    
    ## Building TRT straws
    #cfg.getPublicTool("InDetTrackingGeometryBuilder").LayerBinningType = [2, 2, 2]
    #cfg.getPublicTool("TRT_LayerBuilder").ModelLayersOnly = False    

    cfg.run(10)
    f=open("ExtrapolatorComparisonTestConfig.pkl","w")
    cfg.store(f)
    f.close()
