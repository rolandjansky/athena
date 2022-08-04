# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

if __name__=="__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root"]
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-RUN2-09"
    calo_seeds=True

    import os
    if os.environ.get('ATHENA_CORE_NUMBER',None) is not None :
        ConfigFlags.Concurrency.NumThreads = int(os.environ['ATHENA_CORE_NUMBER'])

    if not calo_seeds :
        ConfigFlags.InDet.Tracking.doCaloSeededBrem=False
        ConfigFlags.InDet.Tracking.doHadCaloSeededSSS=False
        ConfigFlags.InDet.Tracking.doCaloSeededAmbi=False
        ConfigFlags.Detector.EnableCalo=False

    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(ConfigFlags))

    if calo_seeds :
        from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
        acc.merge(CaloTopoClusterCfg(ConfigFlags))

    if ConfigFlags.InDet.doTruth :
        from xAODTruthCnv.RedoTruthLinksConfig import RedoTruthLinksAlgCfg
        acc.merge( RedoTruthLinksAlgCfg(ConfigFlags) )

    from InDetConfig.TrackRecoConfig import InDetTrackRecoCfg
    acc.merge(InDetTrackRecoCfg(ConfigFlags))

    acc.run(100)

    with open("config.pkl", "wb") as file:
      acc.store(file)

