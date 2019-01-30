# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Core configuration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# Local
from MuonConfig.MuonSegmentFindingConfig import MuonSegmentFindingCfg


def MuonReconstructionCfg(flags):
    # TODO - add lots! 
    # https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py
    result=ComponentAccumulator()
    result.merge( MuonSegmentFindingCfg(flags) )
    return result
    
if __name__=="__main__":
    # To run this, do e.g. 
    # python ../athena/MuonSpectrometer/MuonConfig/python/MuonReconstructionConfig.py
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    log.setLevel(DEBUG)
    from AthenaCommon.Logging import log
    log.debug('About to set up Segment Finding.')
    
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q221/21.3/v1/myESD.pool.root"]
    ConfigFlags.Muon.doCSCs = False 
    ConfigFlags.lock()

    cfg=ComponentAccumulator()

    # This is a temporary fix! Should be private!
    from MuonRecHelperTools.MuonRecHelperToolsConf import Muon__MuonEDMHelperTool
    muon_edm_helper_tool = Muon__MuonEDMHelperTool("MuonEDMHelperTool", MuonExtrapolator=None) # Don't bring in this dependency yet!
    cfg.addPublicTool( muon_edm_helper_tool )

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    cfg.merge(MuonReconstructionCfg(ConfigFlags))
              
    f=open("MuonSegmentFinding.pkl","w")
    cfg.store(f)
    f.close()
