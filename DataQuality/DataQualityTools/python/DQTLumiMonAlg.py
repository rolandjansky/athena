#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def DQTLumiMonAlgConfig(flags, isOld=False):
    from AthenaMonitoring import AthMonitorCfgHelper
    if isOld: # replace with the old version of the tool
        from AthenaMonitoring import AthMonitorCfgHelperOld as AthMonitorCfgHelper

    helper = AthMonitorCfgHelper(flags, 'DQTLumiMonAlgCfg')
    # Three instances of the algorithm. One using any trigger, another using only muon
    # triggers, and the final using only electron triggers.
    DQTLumiMonAlgConfigByTriggerChain(helper)
    DQTLumiMonAlgConfigByTriggerChain(helper,'CATEGORY_monitoring_muonIso','EF_muX')
    DQTLumiMonAlgConfigByTriggerChain(helper,'CATEGORY_primary_single_ele','EF_eX')
    return helper.result()

def DQTLumiMonAlgConfigByTriggerChain(helper, triggerChain='', triggerPath=''):
    from .DataQualityToolsConf import DQTLumiMonAlg
    monAlg = helper.addAlgorithm(DQTLumiMonAlg, 'DQTLumiMonAlg'+triggerPath)

    if triggerChain:
        monAlg.TriggerChain = triggerChain
        if not triggerPath:
            triggerPath = triggerChain
    else:
        triggerPath = 'AnyTrigger'

    groupName = 'default'
    groupPath = 'GLOBAL/Luminosity/'+triggerPath
    group = helper.addGroup(monAlg, groupName, groupPath, 'run')
    pixelgroup = helper.addGroup(monAlg, 'pixel', groupPath, 'run')
    nLooseT = 'Number of Loose Vertices'
    nTightT = 'Number of Tight Vertices'
    nPixClusterT = 'Number of Pixel Clusters'

    group.defineHistogram('LB,avgLumi', type='TProfile', xbins=1000, xmin=0, xmax=10000,
                          title='Average Lumi vs LB',opt='kAddBinsDynamically')
    group.defineHistogram('LB,avgIntPerXing', type='TProfile', xbins=1000, xmin=0, xmax=10000,
                          title='Average Mu  vs LB',opt='kAddBinsDynamically')
    group.defineHistogram('LB,lumiPerBCID', type='TProfile', xbins=1000, xmin=0, xmax=10000,
                          title='Instantaneous Luminosity vs LB',opt='kAddBinsDynamically')
    group.defineHistogram('LB,intPerXing', type='TProfile', xbins=1000, xmin=0, xmax=10000,
                          title='Instaneous interactions vs LB',opt='kAddBinsDynamically')
    group.defineHistogram('LB,duration', type='TProfile', xbins=1000, xmin=0, xmax=10000,
                          title='Lumi Block time in sec vs LB',opt='kAddBinsDynamically')
    group.defineHistogram('LB,avgLiveFrac', type='TProfile', xbins=1000, xmin=0, xmax=10000,
                          title='Average live fraction lumi over all BCIDs vs LB',
                          opt='kAddBinsDynamically')
    group.defineHistogram('LB,liveFracPerBCID', type='TProfile', xbins=1000, xmin=0, xmax=10000,
                          title='Current BCID lumi vs LB',opt='kAddBinsDynamically')
    group.defineHistogram('LB,lumiWeight', type='TProfile', xbins=1000, xmin=0, xmax=10000,
                          title='Current BCID lumi vs LB',opt='kAddBinsDynamically')

    group.defineHistogram('LB,avgMu', type='TProfile', xbins=200, xmin=0.5, xmax=200.5,
                          title='Number of interactions per event;LB;<#mu>_{LB}',
                          opt='kAddBinsDynamically')
    group.defineHistogram('LB,nLooseVtx', type='TProfile', xbins=200, xmin=0.5, xmax=200.5,
                          title=nLooseT+' per event;LB;<NlooseVtx/event>_{LB}',
                          opt='kAddBinsDynamically')
    group.defineHistogram('LB,nTightVtx', type='TProfile', xbins=200, xmin=0.5, xmax=200.5,
                          title=nTightT+' per event;LB;<NtightVtx/event>_{LB}',
                          opt='kAddBinsDynamically')
    group.defineHistogram('LB,nLooseVtx;nLooseVtx_perAvgMu', weight='avgMuInverse',
                          xbins=200, xmin=0.5, xmax=200.5,type='TProfile',
                          title=nLooseT+' per event per Mu;LB;<NlooseVtx/event/#mu>_{LB}',
                          opt='kAddBinsDynamically')
    group.defineHistogram('LB,nTightVtx;nTightVtx_perAvgMu', weight='avgMuInverse',
                          xbins=200, xmin=0.5, xmax=200.5,type='TProfile',
                          title=nTightT+' per event per Mu;LB;<NtightVtx/event/#mu>_{LB}',
                          opt='kAddBinsDynamically')

    group.defineHistogram('aveMu,nLooseVtx', type='TProfile', xbins=250, xmin=0, xmax=25,
                          title=nLooseT+' per event;#mu;NlooseVtx/event',
                          opt='kAddBinsDynamically')
    group.defineHistogram('aveMu,nTightVtx', type='TProfile', xbins=250, xmin=0, xmax=25,
                          title=nTightT+' per event;#mu;NtightVtx/event',
                          opt='kAddBinsDynamically')

    pixelgroup.defineHistogram('LB,nClustersAll', type='TProfile', xbins=200, xmin=0.5, xmax=200.5,
                          title=nPixClusterT+', all;LB;<NclustersAll/event>_{LB}',
                          opt='kAddBinsDynamically')
    pixelgroup.defineHistogram('LB,nClustersECA', type='TProfile', xbins=200, xmin=0.5, xmax=200.5,
                          title=nPixClusterT+', endcap A;LB;<NlustersECA/event>_{LB}',
                          opt='kAddBinsDynamically')
    pixelgroup.defineHistogram('LB,nClustersECC', type='TProfile', xbins=200, xmin=0.5, xmax=200.5,
                          title=nPixClusterT+', endcap C;LB;<NclustersECC/event>_{LB}',
                          opt='kAddBinsDynamically')
    pixelgroup.defineHistogram('LB,nClustersB0', type='TProfile', xbins=200, xmin=0.5, xmax=200.5,
                          title=nPixClusterT+', barrel layer 0;LB;<NclustersB0/event>_{LB}',
                          opt='kAddBinsDynamically')
    pixelgroup.defineHistogram('LB,nClustersB1', type='TProfile', xbins=200, xmin=0.5, xmax=200.5,
                          title=nPixClusterT+', barrel layer 1;LB;<NclustersB1/event>_{LB}',
                          opt='kAddBinsDynamically')
    pixelgroup.defineHistogram('LB,nClustersB2', type='TProfile', xbins=200, xmin=0.5, xmax=200.5,
                          title=nPixClusterT+', barrel layer 2;LB;<NclustersB2/event>_{LB}',
                          opt='kAddBinsDynamically')
    pixelgroup.defineHistogram('LB,nClustersAll;LB_vs_nClustersAll_perAvgMu', opt='kAddBinsDynamically',
                          xbins=200, xmin=0.5, xmax=200.5, type='TProfile', weight='avgMuInverse',
                          title=nPixClusterT+' per Mu, all;LB;<NclustersAll/event/#mu>_{LB}')
    pixelgroup.defineHistogram('LB,nClustersECA;LB_vs_nClustersECA_perAvgMu', opt='kAddBinsDynamically',
                          xbins=200, xmin=0.5, xmax=200.5, type='TProfile', weight='avgMuInverse',
                          title=nPixClusterT+' per Mu, endcap A;LB;<NlustersECA/event/#mu>_{LB}')
    pixelgroup.defineHistogram('LB,nClustersECC;LB_vs_nClustersECC_perAvgMu', opt='kAddBinsDynamically',
                          xbins=200, xmin=0.5, xmax=200.5, type='TProfile', weight='avgMuInverse',
                          title=nPixClusterT+' per Mu, endcap C;LB;<NclustersECC/event/#mu>_{LB}')
    pixelgroup.defineHistogram('LB,nClustersB0;LB_vs_nClustersB0_perAvgMu', opt='kAddBinsDynamically',
                          xbins=200, xmin=0.5, xmax=200.5, type='TProfile', weight='avgMuInverse',
                          title=nPixClusterT+' per Mu, barrel layer 0;LB;<NclustersB0/event/#mu>_{LB}')
    pixelgroup.defineHistogram('LB,nClustersB1;LB_vs_nClustersB1_perAvgMu', opt='kAddBinsDynamically',
                          xbins=200, xmin=0.5, xmax=200.5, type='TProfile', weight='avgMuInverse',
                          title=nPixClusterT+' per Mu, barrel layer 1;LB;<NclustersB1/event/#mu>_{LB}')
    pixelgroup.defineHistogram('LB,nClustersB2;LB_vs_nClustersB2_perAvgMu', opt='kAddBinsDynamically',
                          xbins=200, xmin=0.5, xmax=200.5, type='TProfile', weight='avgMuInverse',
                          title=nPixClusterT+' per Mu, barrel layer 2;LB;<NclustersB2/event/#mu>_{LB}')
    pixelgroup.defineHistogram('aveMu,nClustersAll', type='TProfile', xbins=250, xmin=0, xmax=25,
                          title=nPixClusterT+', all;#mu;NclustersAll/event',
                          opt='kAddBinsDynamically')
    pixelgroup.defineHistogram('aveMu,nClustersECA', type='TProfile', xbins=250, xmin=0, xmax=25,
                          title=nPixClusterT+', endcap A;#mu;NclustersECA/event',
                          opt='kAddBinsDynamically')
    pixelgroup.defineHistogram('aveMu,nClustersECC', type='TProfile', xbins=250, xmin=0, xmax=25,
                          title=nPixClusterT+', endcap C;#mu;NclustersECC/event',
                          opt='kAddBinsDynamically')
    pixelgroup.defineHistogram('aveMu,nClustersB0', type='TProfile', xbins=250, xmin=0, xmax=25,
                          title=nPixClusterT+', barrel layer 0;#mu;NclustersB0/event',
                          opt='kAddBinsDynamically')
    pixelgroup.defineHistogram('aveMu,nClustersB1', type='TProfile', xbins=250, xmin=0, xmax=25,
                          title=nPixClusterT+', barrel layer 1;#mu;NclustersB1/event',
                          opt='kAddBinsDynamically')
    pixelgroup.defineHistogram('aveMu,nClustersB2', type='TProfile', xbins=250, xmin=0, xmax=25,
                          title=nPixClusterT+', barrel layer 2;#mu;NclustersB2/event',
                          opt='kAddBinsDynamically')
