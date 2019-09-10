# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Core configuration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# Local
from MuonConfig.MuonSegmentFindingConfig import MuonSegmentFindingCfg
from MuonConfig.MuonTrackBuildingConfig import MuonTrackBuildingCfg


def MuonReconstructionCfg(flags):
    # TODO - add lots! 
    # https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py
    result=ComponentAccumulator()
    result.merge( MuonSegmentFindingCfg(flags) )
    result.merge( MuonTrackBuildingCfg(flags) )
    return result
    
if __name__=="__main__":
    # To run this, do e.g. 
    # python ../athena/MuonSpectrometer/MuonConfig/python/MuonReconstructionConfig.py
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
    ConfigFlags.Detector.GeometryMDT   = True 
    ConfigFlags.Detector.GeometryTGC   = True
    ConfigFlags.Detector.GeometryCSC   = True     
    ConfigFlags.Muon.doCSCs = False # FIXME - this does not yet work. Need to investigate why.
    ConfigFlags.Detector.GeometryRPC   = True 
    
    log.setLevel(DEBUG)
    from AthenaCommon.Logging import log
    log.debug('About to set up Segment Finding.')
    
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q221/21.3/v1/myESD.pool.root"]
    ConfigFlags.lock()

    cfg=ComponentAccumulator()

    # This is a temporary fix! Should be private!
    from MuonRecHelperTools.MuonRecHelperToolsConf import Muon__MuonEDMHelperSvc
    muon_edm_helper_svc = Muon__MuonEDMHelperSvc("MuonEDMHelperSvc")
    cfg.addService( muon_edm_helper_svc )

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    cfg.merge(MuonReconstructionCfg(ConfigFlags))
              
    f=open("MuonSegmentFinding.pkl","w")
    cfg.store(f)
    f.close()
