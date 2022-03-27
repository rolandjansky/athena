#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration




def TrigAFPSidHypoMonitoring(configFlags):
    from AthenaMonitoring import AthMonitorCfgHelper
    monConfig = AthMonitorCfgHelper(configFlags, 'TrigAFPSidHypoMonitoring')

    from AthenaConfiguration.ComponentFactory import CompFactory
    alg = monConfig.addAlgorithm( CompFactory.TrigAFPSidHypoMonitoringAlg, 'TrigAFPSidHypoMonitoring')
    # will read it from menu in the future
    alg.Chains = ["all", "AFP", "HLT_mb_afprec_L1AFP_A_AND_C_TOF_jJ50", "HLT_mb_afprec_L1RD0_FILLED"]

    mbtsGroup = monConfig.addGroup(alg, 'AFPCoarse', topPath='HLT/MBAFPCoarse/')
    mbtsGroup.defineHistogram('TrigCounts', title='Trigger counts;;Event rate',
                              xbins=len(alg.Chains), xmin=0, xmax=len(alg.Chains), xlabels=list(alg.Chains))


    return monConfig.result()

if __name__ == '__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Constants import DEBUG
    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Output.HISTFileName = 'TestMonitorOutput.root'
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(TrigAFPSidHypoMonitoring(ConfigFlags))

    cfg.getEventAlgo('TrigAFPSidHypoMonitoring').OutputLevel = DEBUG
    cfg.printConfig(withDetails=True)  # set True for exhaustive info
    with open("cfg.pkl", "wb") as f:
        cfg.store(f)

    cfg.run() 
    # to run:
    # python -m TrigMinBiasMonitoring.TrigAFPSidHypoMonitoring --filesInput=
