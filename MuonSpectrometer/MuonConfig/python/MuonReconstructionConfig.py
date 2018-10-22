# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getPublicTool,getPublicToolClone,getPrivateTool

def MuonReconstructionCfg(flags):
    # TODO - add lots! 
    # https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py
    result=ComponentAccumulator()
    from MuonConfig.MuonSegmentFindingConfig import MuonSegmentFindingCfg
    result.merge( MuonSegmentFindingCfg(flags) )
    return result
    
if __name__=="__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    log.setLevel(DEBUG)

    ConfigFlags.Input.isMC=False
    ConfigFlags.Input.Files = ["myESD.pool.root"]
    ConfigFlags.lock()

    cfg=ComponentAccumulator()

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))
    
    acc=MuonReconstructionCfg(ConfigFlags)
    
    cfg.merge(acc)
              
    f=open("MuonReconstruction.pkl","w")
    cfg.store(f)
    f.close()
