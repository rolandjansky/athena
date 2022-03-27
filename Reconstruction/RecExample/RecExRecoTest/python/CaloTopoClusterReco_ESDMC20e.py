# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

if __name__=="__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root"]
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(ConfigFlags))

    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    acc.merge(CaloTopoClusterCfg(ConfigFlags))

    acc.run(100)

