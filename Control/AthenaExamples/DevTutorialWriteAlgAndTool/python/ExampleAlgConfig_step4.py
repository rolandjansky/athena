# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Constants import VERBOSE
from DevTutorialWriteAlgAndTool.DevTutorialWriteAlgAndToolConf import ExampleAlg_step4 as ExampleAlg
from TrkExTools.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

def ExampleAlgCfg(configFlags, **kwargs):
    result=ComponentAccumulator()
    alg = ExampleAlg(**kwargs )
    result.addEventAlgo(alg)
    return result

if __name__=="__main__":
    from AthenaCommon.Configurable import Configurable
    from AthenaCommon.Logging import log
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg    
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.ESD

    ConfigFlags.Detector.GeometryPixel = True     
    ConfigFlags.Detector.GeometrySCT   = True 
    ConfigFlags.Detector.GeometryTRT   = True
    ConfigFlags.Concurrency.NumThreads = 8
    ConfigFlags.Concurrency.NumConcurrentEvents = 8


    ConfigFlags.lock()

    cfg=MainServicesThreadedCfg(ConfigFlags)
    
    cfg.merge(PoolReadCfg(ConfigFlags))
    
    from AthenaCommon.Constants import VERBOSE
    acc=ExampleAlgCfg(ConfigFlags, OutputLevel=VERBOSE)
    cfg.merge(acc)

    cfg.run(10)



