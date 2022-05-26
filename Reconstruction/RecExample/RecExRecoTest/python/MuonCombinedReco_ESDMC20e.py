# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

if __name__=="__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
    #Made in master, 25 May 2022 using
    #https://gitlab.cern.ch/atlas/athena/-/blob/b74428c8ff7dcd8c859fce16001c60a14b78256b/Reconstruction/RecExample/RecJobTransformTests/test/test_mc20e_13TeV.sh
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.recon.ESD.e6337_s3681_r13145/myESD.pool.root"]
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(ConfigFlags))

    from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedReconstructionCfg
    acc.merge(MuonCombinedReconstructionCfg(ConfigFlags))

    acc.run(100)

