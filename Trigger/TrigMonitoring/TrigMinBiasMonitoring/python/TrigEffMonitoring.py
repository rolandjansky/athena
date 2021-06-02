#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
@brief configuration for the trigger efficiency monitoring
'''


def TrigEff(configFlags):
    from AthenaMonitoring import AthMonitorCfgHelper
    monConfig = AthMonitorCfgHelper(
        configFlags, 'HLTEfficiencyMonitoringAlgflags')

    from AthenaConfiguration.ComponentFactory import CompFactory
    alg = monConfig.addAlgorithm(
        CompFactory.HLTEfficiencyMonitoringAlg, 'HLTEfficiencyMonitoringAlg')

    trkSel = CompFactory.InDet.InDetTrackSelectionTool(
        "InDetTrackSelectionTool_TightPrimary", CutLevel="TightPrimary"
    )

    alg.TrackSelectionTool = trkSel

    alg.triggerList = ["HLT_mb_sptrk_L1RD0_FILLED", "HLT_mb_sp_L1RD0_FILLED", "HLT_mb_mbts_L1MBTS_1",
                       "HLT_mb_mbts_L1MBTS_1_1", "HLT_mb_mbts_L1MBTS_2", "HLT_mb_sp400_trk40_hmt_L1RD0_FILLED",
                       "HLT_mb_sp600_trk45_hmt_L1RD0_FILLED", "HLT_mb_sp1200_trk60_hmt_L1RD0_FILLED", "HLT_mb_sp1400_trk80_hmt_L1RD0_FILLED",
                       "HLT_mb_sp2000_trk100_hmt_L1RD0_FILLED", "HLT_mb_sp3000_trk100_hmt_L1RD0_FILLED", "HLT_mb_sp5000_trk80_hmt_L1RD0_FILLED",
                       "HLT_mb_sp5000_trk100_hmt_L1RD0_FILLED", "HLT_mb_sp5000_trk200_hmt_L1RD0_FILLED", "HLT_mb_sp400_pusup300_trk40_hmt_L1RD0_FILLED"]
    alg.refTriggerList = ["HLT_noalg_L1RD0_FILLED", "HLT_noalg_L1RD0_FILLED", "HLT_noalg_L1MBTS_1_1",
                          "HLT_noalg_L1MBTS_1_1", "HLT_noalg_L1MBTS_2", "HLT_noalg_L1RD0_FILLED",
                          "HLT_noalg_L1RD0_FILLED", "HLT_noalg_L1RD0_FILLED", "HLT_noalg_L1RD0_FILLED",
                          "HLT_noalg_L1RD0_FILLED", "HLT_noalg_L1RD0_FILLED", "HLT_noalg_L1RD0_FILLED",
                          "HLT_noalg_L1RD0_FILLED", "HLT_noalg_L1RD0_FILLED", "HLT_noalg_L1RD0_FILLED"]

    length = len(alg.triggerList)

    mainGroup = monConfig.addGroup(
        alg, 'TrigAll', topPath='HLT/')
    mainGroup.defineHistogram('TrigCounts', title='Trigger counts;;Event rate',
                              xbins=length, xmin=0, xmax=length, xlabels=list(alg.triggerList))

    for refchain in alg.refTriggerList:
        for chain in alg.triggerList:
            effGroup = monConfig.addGroup(
                alg, chain+refchain, topPath='HLT/EffAll/')
            effGroup.defineHistogram('EffPassed,nTrkOffline;' + chain + '_ref_' + refchain, type='TEfficiency',
                                     title=chain+';Offline Good nTrk;Efficiency', xbins=200, xmin=0, xmax=400)

    return monConfig.result()


if __name__ == '__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Constants import DEBUG
    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    #ConfigFlags.Input.Files = ['myAOD.pool.root']
    ConfigFlags.Input.Files = [
        '/afs/cern.ch/user/k/kburka/workspace/mbts/AOD.25577237._000120.pool.root.1']
    ConfigFlags.Output.HISTFileName = 'TestEffMonitorOutput.root'
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(TrigEff(ConfigFlags))

    cfg.getEventAlgo('HLTEfficiencyMonitoringAlg').OutputLevel = DEBUG  # DEBUG
    cfg.printConfig(withDetails=True)  # set True for exhaustive info
    with open("cfg.pkl", "wb") as f:
        cfg.store(f)

    cfg.run(20)  # use cfg.run(20) to only run on first 20 events
    # to run:
    # python -m TrigMinBiasMonitoring.TrigEffMonitoring
