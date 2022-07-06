#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


from TrigMuonMonitoring.TrigMuonMonitoringConfig import TrigMuonMonConfig



if __name__=='__main__':     
    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    aodfile = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = [nightly+aodfile]
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'HIST.root'

    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    muonMonitorAcc = TrigMuonMonConfig(ConfigFlags)
    cfg.merge(muonMonitorAcc)

    cfg.printConfig(withDetails=False)
    cfg.run()
