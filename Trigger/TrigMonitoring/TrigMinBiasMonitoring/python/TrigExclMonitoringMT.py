#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

"""
@brief configuration for the min bias monitoring
"""

import math
from TrigConfigSvc.TriggerConfigAccess import getHLTMenuAccess

def TrigExcl(configFlags):
    from AthenaMonitoring import AthMonitorCfgHelper
    monConfig = AthMonitorCfgHelper(configFlags, "HLTExclMonAlg")

    from AthenaConfiguration.ComponentFactory import CompFactory
    alg = monConfig.addAlgorithm(CompFactory.ExclMinBiasTrkMonAlg, "HLTExclMonAlg")
    trkSel = CompFactory.InDet.InDetTrackSelectionTool("InDetTrackSelectionTool_LoosePrimary", CutLevel="LoosePrimary")

    alg.TrackSelectionTool = trkSel

    alg.triggerList = [c for c in getHLTMenuAccess(configFlags) if '_excl_' in c]
    alg.refTriggerList = ['HLT_mb_sptrk_L1RD0_FILLED', 'HLT_mb_sp_L1RD0_FILLED']

    for ref in alg.refTriggerList:
        for chain in alg.triggerList:
            # Efficiencies
            mbRefTrigGroup = monConfig.addGroup(alg, f'{ref}_{chain}', topPath=f'HLT/MinBiasMon/{chain}/{ref}/')
            mbRefTrigGroup.defineHistogram(f'passedRefTrig,evtPileUp;eff_pileup_{chain}_vs_{ref}', type='TEfficiency', title=f'Efficiency: {chain} vs {ref};pileup;Efficiency', xbins=50, xmin=0, xmax=10)
            mbRefTrigGroup.defineHistogram(f'passedRefTrig,leadingPtOffline;eff_leadingPt_{chain}_vs_{ref}', type='TEfficiency', title=f'Efficiency: {chain} vs {ref};leading offline track pT [GeV];Efficiency', xbins=50, xmin=0, xmax=10)
            mbRefTrigGroup.defineHistogram(f'passedRefTrig,leadingPtOnline;eff_leadingPtOn_{chain}_vs_{ref}', type='TEfficiency', title=f'Efficiency: {chain} vs {ref};leading online track pT [GeV];Efficiency', xbins=50, xmin=0, xmax=10)

            mbRefTrigGroup = monConfig.addGroup(alg, f'{ref}_{chain}_passed', topPath=f'HLT/MinBiasMon/{chain}/{ref}/')

            # Online tracks
            mbRefTrigGroup.defineHistogram('evtPileUp', title=';pileup', xbins=50, xmin=0, xmax=5)
            mbRefTrigGroup.defineHistogram('nTrkOfflineGood', title=';Offline good track count', xbins=20, xmin=0, xmax=20)
            mbRefTrigGroup.defineHistogram('nTrkOfflineGoodPt1', title=';Offline good track count, pT > 1 GeV', xbins=20, xmin=0, xmax=20)
            mbRefTrigGroup.defineHistogram('nTrkOfflineGoodPt2', title=';Offline good track count, pT > 2 GeV', xbins=20, xmin=0, xmax=20)
            mbRefTrigGroup.defineHistogram('nTrkOfflineGoodPt4', title=';Offline good track count, pT > 4 GeV', xbins=20, xmin=0, xmax=20)
            mbRefTrigGroup.defineHistogram('nTrkOfflineGoodPt6', title=';Offline good track count, pT > 6 GeV', xbins=20, xmin=0, xmax=20)
            mbRefTrigGroup.defineHistogram('trkOfflinePt', cutmask='trackCuts', title=';Offline good track pT [GeV]', xbins=100, xmin=0, xmax=10)
            mbRefTrigGroup.defineHistogram('trkOfflineEta', cutmask='trackCuts', title=';Offline good track eta', xbins=100, xmin=-3, xmax=3)
            mbRefTrigGroup.defineHistogram('trkOfflinePhi', cutmask='trackCuts', title=';Offline good track phi', xbins=100, xmin=-math.pi, xmax=math.pi)
            mbRefTrigGroup.defineHistogram('trkOfflineD0', cutmask='trackCuts', title=';Offline good track d0 [mm]', xbins=100, xmin=-10, xmax=10)
            mbRefTrigGroup.defineHistogram('trkOfflineD0;trkOfflineD0_zoom', cutmask='trackCuts', title=';Offline good track d0 [mm]', xbins=100, xmin=-2, xmax=2)
            mbRefTrigGroup.defineHistogram('trkOfflineZ0', cutmask='trackCuts', title=';Offline good track z0 [mm]', xbins=100, xmin=-100, xmax=100)
            mbRefTrigGroup.defineHistogram('leadingPtOffline', title=';Offline leading track pT [GeV]', xbins=100, xmin=0, xmax=10)

            # Offline tracks
            mbRefTrigGroup.defineHistogram('nTrkOnlineGood', title=';Online good track count', xbins=20, xmin=0, xmax=20)
            mbRefTrigGroup.defineHistogram('nTrkOnlineGoodPt1', title=';Online good track count, pT > 1 GeV', xbins=20, xmin=0, xmax=20)
            mbRefTrigGroup.defineHistogram('nTrkOnlineGoodPt2', title=';Online good track count, pT > 2 GeV', xbins=20, xmin=0, xmax=20)
            mbRefTrigGroup.defineHistogram('nTrkOnlineGoodPt4', title=';Online good track count, pT > 4 GeV', xbins=20, xmin=0, xmax=20)
            mbRefTrigGroup.defineHistogram('nTrkOnlineGoodPt6', title=';Online good track count, pT > 6 GeV', xbins=20, xmin=0, xmax=20)
            mbRefTrigGroup.defineHistogram('trkOnlinePt', cutmask='trackOnlineCuts', title=';Online good track pT [GeV]', xbins=100, xmin=0, xmax=10)
            mbRefTrigGroup.defineHistogram('trkOnlineEta', cutmask='trackOnlineCuts', title=';Online good track eta', xbins=100, xmin=-3, xmax=3)
            mbRefTrigGroup.defineHistogram('trkOnlinePhi', cutmask='trackOnlineCuts', title=';Online good track phi', xbins=100, xmin=-math.pi, xmax=math.pi)
            mbRefTrigGroup.defineHistogram('trkOnlineD0', cutmask='trackOnlineCuts', title=';Online good track d0 [mm]', xbins=100, xmin=-10, xmax=10)
            mbRefTrigGroup.defineHistogram('trkOnlineD0;trkOnlineD0_zoom', cutmask='trackOnlineCuts', title=';Online good track d0 [mm]', xbins=100, xmin=-2, xmax=2)
            mbRefTrigGroup.defineHistogram('trkOnlineZ0', cutmask='trackOnlineCuts', title=';Online good track z0 [mm]', xbins=100, xmin=-100, xmax=100)
            mbRefTrigGroup.defineHistogram('leadingPtOnline', title=';Online leading track pT [GeV]', xbins=100, xmin=0, xmax=10)

    return monConfig.result()


if __name__ == "__main__":
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable

    Configurable.configurableRun3Behavior = 1


    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.DQ.Environment = "AOD"
    ConfigFlags.Concurrency.NumConcurrentEvents = 5

    ConfigFlags.Output.HISTFileName = "TestExclMonitorOutput.root"
    ConfigFlags.fillFromArgs()
    from AthenaCommon.Logging import logging
    log = logging.getLogger(__name__)
    log.info("Input %s", str(ConfigFlags.Input.Files))
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

    cfg = MainServicesCfg(ConfigFlags)

    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(TrigExcl(ConfigFlags)) # for local testing save reference histograms

    # If you want to turn on more detailed messages ...
    from AthenaCommon.Constants import DEBUG
    cfg.getEventAlgo("HLTExclMonAlg").OutputLevel = DEBUG
    cfg.printConfig(withDetails=True)  # set True for exhaustive info
    with open("cfg.pkl", "wb") as f:
        cfg.store(f)

    cfg.run()  # use cfg.run(20) to only run on first 20 events
    # to test, run:
    # python -m TrigMinBiasMonitoring.TrigExclMonitoringMT --filesInput=
