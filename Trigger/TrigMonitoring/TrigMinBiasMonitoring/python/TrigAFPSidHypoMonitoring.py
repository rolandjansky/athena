#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def TrigAFPSidHypoMonitoring(configFlags):
    from AthenaMonitoring import AthMonitorCfgHelper
    monConfig = AthMonitorCfgHelper(configFlags, 'TrigAFPSidHypoMonitoring')

    from AthenaConfiguration.ComponentFactory import CompFactory
    alg = monConfig.addAlgorithm( CompFactory.TrigAFPSidHypoMonitoringAlg, 'TrigAFPSidHypoMonitoring')
    # will read it from menu in the future
    alg.Chains = ["all", "AFP", "HLT_mb_afprec_L1CEP-CjJ50"]

    mbtsGroup = monConfig.addGroup(alg, 'AFPCoarse', topPath='HLT/MBAFPCoarse/')
    mbtsGroup.defineHistogram('TrigCounts', title='Trigger counts;;Event rate',
                              xbins=len(alg.Chains), xmin=0, xmax=len(alg.Chains), xlabels=list(alg.Chains))
    mbtsGroup.defineHistogram('xDiff', title='xLocal diff online-offline', xbins=101, xmin=-10, xmax=10)
    mbtsGroup.defineHistogram('yDiff', title='yLocal diff online-offline', xbins=101, xmin=-10, xmax=10)
    mbtsGroup.defineHistogram('xDiff;xDiffWideRange', title='xLocal diff online-offline large scale', xbins=101, xmin=-100, xmax=100)
    mbtsGroup.defineHistogram('yDiff;yDiffWideRange', title='yLocal diff online-offline large scale', xbins=101, xmin=-100, xmax=100)
    
    return monConfig.result()

if __name__ == '__main__':
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
