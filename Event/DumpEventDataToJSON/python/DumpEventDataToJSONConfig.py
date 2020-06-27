# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Constants import VERBOSE
DumpEventDataToJsonAlg=CompFactory.DumpEventDataToJsonAlg

def DumpEventDataToJSONAlgCfg(configFlags, doExtrap=False, **kwargs):
    result=ComponentAccumulator()
    extrapolationEngine=""
    if doExtrap:
      from AtlasGeoModel.GeoModelConfig import GeoModelCfg
      gmsAcc=GeoModelCfg( configFlags )
      result.merge(gmsAcc)
      
      from TrkConfig.AtlasExtrapolationEngineConfig import AtlasExtrapolationEngineCfg
      extrapAcc = AtlasExtrapolationEngineCfg(configFlags)
      extrapolationEngine = extrapAcc.getPrimary()
      result.merge(extrapAcc)
      
      kwargs.setdefault('Extrapolator', extrapolationEngine)
      
    dumpAlg = DumpEventDataToJsonAlg(ExtrapolateTrackParticles=doExtrap, **kwargs )
    result.addEventAlgo(dumpAlg)
    return result

if __name__=="__main__":
    from AthenaCommon.Configurable import Configurable
    from AthenaCommon.Logging import log
    # from AthenaCommon.Constants import DEBUG
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    # from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg    
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

    Configurable.configurableRun3Behavior=1

    # Uncomment for debugging
    # log.setLevel(DEBUG)
    
    # To run on MC do e.g.
    ConfigFlags.Input.Files = ["../q221/myESD.pool.root"]
    # To run on data do e.g.
    # ConfigFlags.Input.Files = ["../q431/myESD.pool.root"]
        
    # Just enable ID for the moment.
    ConfigFlags.Detector.GeometryPixel = True     
    ConfigFlags.Detector.GeometrySCT   = True 
    ConfigFlags.Detector.GeometryTRT   = True
    
    # This should run serially for the moment.
    ConfigFlags.Concurrency.NumThreads = 1
    ConfigFlags.Concurrency.NumConcurrentEvents = 1
    
    ConfigFlags.lock()
    log.debug('Lock config flags now.')
    ConfigFlags.lock()

    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    
    # Disable doExtrap if you would prefer not to use the extrapolator.
    topoAcc=DumpEventDataToJSONAlgCfg(ConfigFlags, doExtrap = False, OutputLevel=VERBOSE, OutputLocation="EventData_new.json")
    cfg.merge(topoAcc)

    cfg.run(10)
    f=open("DumpEventDataToJSONConfig.pkl","wb")
    cfg.store(f)
    f.close()



