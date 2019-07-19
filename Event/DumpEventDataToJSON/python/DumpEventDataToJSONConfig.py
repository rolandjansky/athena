# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Constants import VERBOSE
from DumpEventDataToJSON.DumpEventDataToJSONConf import DumpEventDataToJsonAlg


def DumpEventDataToJSONAlgCfg(configFlags, doExtrap=False):
    result=ComponentAccumulator()
    if doExtrap:
      from TrkExEngine.AtlasExtrapolationEngineConfig import AtlasExtrapolationEngineCfg
      extrapAcc = AtlasExtrapolationEngineCfg(configFlags)

      extrapolationEngine = extrapAcc.popPrivateTools()
      result.merge(extrapAcc)

    dumpAlg = DumpEventDataToJsonAlg(OutputLevel=VERBOSE, ExtrapolateTracks=doExtrap, Extrapolator = extrapolationEngine)
    result.addEventAlgo(dumpAlg)
    return result

if __name__=="__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Input.Files = ["../q221/myESD.pool.root"]

    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

    cfg=MainServicesThreadedCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
  
    topoAcc=DumpEventDataToJSONAlgCfg(ConfigFlags, doExtrap = True)
    cfg.merge(topoAcc)

    cfg.run(2)
    #f=open("DumpEventDataToJSONConfig.pkl","w")
    #cfg.store(f)
    #f.close()



