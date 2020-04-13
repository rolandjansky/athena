#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def DQTLumiMonAlgConfig(flags, isOld=False):
    if isOld: # replace with the old version of the tool
        from AthenaMonitoring import AthMonitorCfgHelperOld as AthMonitorCfgHelper
        from .DataQualityToolsConf import DQTLumiMonAlg
    else:
        from AthenaMonitoring import AthMonitorCfgHelper
        from AthenaConfiguration.ComponentFactory import CompFactory
        DQTLumiMonAlg = CompFactory.DQTLumiMonAlg


    helper = AthMonitorCfgHelper(flags, 'DQTLumiMonAlgCfg')
    # Three instances of the algorithm. One using any trigger, another using only muon
    # triggers, and the final using only electron triggers.
    DQTLumiMonAlgConfigByTriggerChain(helper, DQTLumiMonAlg)
    DQTLumiMonAlgConfigByTriggerChain(helper, DQTLumiMonAlg, 
                                      'CATEGORY_monitoring_muonIso','EF_muX')
    DQTLumiMonAlgConfigByTriggerChain(helper, DQTLumiMonAlg,
                                      'CATEGORY_primary_single_ele','EF_eX')
    if isOld:
        return helper.result()
    else:
        result = helper.result()
        from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
        result.merge(AtlasGeometryCfg(flags))
        return result

def DQTLumiMonAlgConfigByTriggerChain(helper, algConfObj, triggerChain='', triggerPath=''):
    monAlg = helper.addAlgorithm(algConfObj, 'DQTLumiMonAlg'+triggerPath)

    if triggerChain:
        monAlg.TriggerChain = triggerChain
        if not triggerPath:
            triggerPath = triggerChain
    else:
        triggerPath = 'AnyTrigger'

    groupPath = 'GLOBAL/Luminosity/'+triggerPath
    group = helper.addGroup(monAlg, 'default', groupPath, 'run')
    pixelgroup = helper.addGroup(monAlg, 'pixel', groupPath, 'run')
    nLooseT = 'Number of Loose Vertices'
    nTightT = 'Number of Tight Vertices'
    nPixClusterT = 'Number of Pixel Clusters'

    # Vertex parameters. A vertex
    monAlg.TightTrackWeight = 0.01
    monAlg.TightNTracks = 4

    # Raw plots of lumi variables
    group.defineHistogram('avgIntPerXing', type='TH1F', xbins=100, xmin=0, xmax=100,
                          title='Average number of interactions per bunch Xing')
    group.defineHistogram('intPerXing', type='TH1F', xbins=100, xmin=0, xmax=100,
                          title='Actual number of interactions per bunch Xing')

    # Plots of luminosity variables as a function of lb number
    group.defineHistogram('LB,avgLumi', type='TProfile', xbins=1000, xmin=0.5, xmax=1000.5,
                          title='Average Lumi vs LB', opt='kAddBinsDynamically', merge='merge')
    group.defineHistogram('LB,avgIntPerXing', type='TProfile', xbins=1000, xmin=0.5, xmax=1000.5,
                          title='Number of interactions per event;LB;<#mu>_{LB}',
                          opt='kAddBinsDynamically', merge='merge')
    group.defineHistogram('LB,lumiPerBCID', type='TProfile', xbins=1000, xmin=0.5, xmax=1000.5,
                          title='Instantaneous Luminosity vs LB', opt='kAddBinsDynamically', merge='merge')
    group.defineHistogram('LB,intPerXing', type='TProfile', xbins=1000, xmin=0.5, xmax=1000.5,
                          title='Instaneous interactions vs LB', opt='kAddBinsDynamically', merge='merge')
    group.defineHistogram('LB,duration', type='TProfile', xbins=1000, xmin=0.5, xmax=1000.5,
                          title='Lumi Block time in sec vs LB', opt='kAddBinsDynamically', merge='merge')
    group.defineHistogram('LB,avgLiveFrac', type='TProfile', xbins=1000, xmin=0.5, xmax=1000.5,
                          title='Average live fraction lumi over all BCIDs vs LB',
                          opt='kAddBinsDynamically', merge='merge')
    group.defineHistogram('LB,liveFracPerBCID', type='TProfile', xbins=1000, xmin=0.5, xmax=1000.5,
                          title='Current BCID lumi vs LB', opt='kAddBinsDynamically', merge='merge')
    group.defineHistogram('LB,lumiWeight', type='TProfile', xbins=1000, xmin=0.5, xmax=1000.5,
                          title='Current BCID lumi vs LB', opt='kAddBinsDynamically', merge='merge')

    # Vertex counts vs. lb number (loose and tight)
    group.defineHistogram('LB,nLooseVtx', type='TProfile', xbins=200, xmin=0.5, xmax=200.5,
                          title=nLooseT+' per event;LB;<NlooseVtx/event>_{LB}',
                          opt='kAddBinsDynamically', merge='merge')
    group.defineHistogram('LB,nTightVtx', type='TProfile', xbins=200, xmin=0.5, xmax=200.5,
                          title=nTightT+' per event;LB;<NtightVtx/event>_{LB}',
                          opt='kAddBinsDynamically', merge='merge')

    # Vertex counts per avg mu vs. lb number. (Same as above, just weighted.)
    group.defineHistogram('LB,nLooseVtxPerAvgMu', type='TProfile', opt='kAddBinsDynamically',
                          xbins=200, xmin=0.5, xmax=200.5,
                          title=nLooseT+' per event per Mu;LB;<NlooseVtx/event/#mu>_{LB}', merge='merge')
    group.defineHistogram('LB,nTightVtxPerAvgMu', type='TProfile', opt='kAddBinsDynamically',
                          xbins=200, xmin=0.5, xmax=200.5,
                          title=nTightT+' per event per Mu;LB;<NtightVtx/event/#mu>_{LB}', merge='merge')

    # Vertex counts vs. avg mu.
    group.defineHistogram('avgIntPerXing,nLooseVtx', type='TProfile', xbins=250, xmin=0, xmax=100,
                          title=nLooseT+' per event;#mu;NlooseVtx/event')
    group.defineHistogram('avgIntPerXing,nTightVtx', type='TProfile', xbins=250, xmin=0, xmax=100,
                          title=nTightT+' per event;#mu;NtightVtx/event')

    # Pixel clusters vs lb number. All, and then eparately for each endcap and barrel layer.
    pixelgroup.defineHistogram('LB,nClustersAll', type='TProfile', opt='kAddBinsDynamically',
                               xbins=200, xmin=0.5, xmax=200.5,
                               title=nPixClusterT+', all;LB;<NclustersAll/event>_{LB}', merge='merge')
    pixelgroup.defineHistogram('LB,nClustersECA', type='TProfile', opt='kAddBinsDynamically',
                               xbins=200, xmin=0.5, xmax=200.5,
                               title=nPixClusterT+', endcap A;LB;<NlustersECA/event>_{LB}', merge='merge')
    pixelgroup.defineHistogram('LB,nClustersECC', type='TProfile', opt='kAddBinsDynamically',
                               xbins=200, xmin=0.5, xmax=200.5,
                               title=nPixClusterT+', endcap C;LB;<NclustersECC/event>_{LB}', merge='merge')
    pixelgroup.defineHistogram('LB,nClustersB0', type='TProfile', opt='kAddBinsDynamically',
                               xbins=200, xmin=0.5, xmax=200.5,
                               title=nPixClusterT+', barrel layer 0;LB;<NclustersB0/event>_{LB}', merge='merge')
    pixelgroup.defineHistogram('LB,nClustersB1', type='TProfile', opt='kAddBinsDynamically',
                               xbins=200, xmin=0.5, xmax=200.5,
                               title=nPixClusterT+', barrel layer 1;LB;<NclustersB1/event>_{LB}', merge='merge')
    pixelgroup.defineHistogram('LB,nClustersB2', type='TProfile', opt='kAddBinsDynamically',
                               xbins=200, xmin=0.5, xmax=200.5,
                               title=nPixClusterT+', barrel layer 2;LB;<NclustersB2/event>_{LB}', merge='merge')

    # Pixel clusters per avg mu vs. lb number. (Same as above, just weighted.)
    pixelgroup.defineHistogram('LB,nClustersAllPerAvgMu', type='TProfile', opt='kAddBinsDynamically',
                               xbins=200, xmin=0.5, xmax=200.5,
                               title=nPixClusterT+' per Mu, all;LB;<NclustersAll/event/#mu>_{LB}', merge='merge')
    pixelgroup.defineHistogram('LB,nClustersECAPerAvgMu', type='TProfile', opt='kAddBinsDynamically',
                               xbins=200, xmin=0.5, xmax=200.5,
                               title=nPixClusterT+' per Mu, endcap A;LB;<NlustersECA/event/#mu>_{LB}', merge='merge')
    pixelgroup.defineHistogram('LB,nClustersECCPerAvgMu', type='TProfile', opt='kAddBinsDynamically',
                               xbins=200, xmin=0.5, xmax=200.5,
                               title=nPixClusterT+' per Mu, endcap C;LB;<NclustersECC/event/#mu>_{LB}', merge='merge')
    pixelgroup.defineHistogram('LB,nClustersB0PerAvgMu', type='TProfile', opt='kAddBinsDynamically',
                               xbins=200, xmin=0.5, xmax=200.5,
                               title=nPixClusterT+' per Mu, barrel layer 0;LB;<NclustersB0/event/#mu>_{LB}', merge='merge')
    pixelgroup.defineHistogram('LB,nClustersB1PerAvgMu', type='TProfile', opt='kAddBinsDynamically',
                               xbins=200, xmin=0.5, xmax=200.5,
                               title=nPixClusterT+' per Mu, barrel layer 1;LB;<NclustersB1/event/#mu>_{LB}', merge='merge')
    pixelgroup.defineHistogram('LB,nClustersB2PerAvgMu', type='TProfile', opt='kAddBinsDynamically',
                               xbins=200, xmin=0.5, xmax=200.5,
                               title=nPixClusterT+' per Mu, barrel layer 2;LB;<NclustersB2/event/#mu>_{LB}', merge='merge')

    # Pixel clusters vs. avg mu. All, then separately for each endcap and barrel layer.
    pixelgroup.defineHistogram('avgIntPerXing,nClustersAll', type='TProfile',
                               xbins=250, xmin=0, xmax=100,
                               title=nPixClusterT+', all;#mu;NclustersAll/event')
    pixelgroup.defineHistogram('avgIntPerXing,nClustersECA', type='TProfile',
                               xbins=250, xmin=0, xmax=100,
                               title=nPixClusterT+', endcap A;#mu;NclustersECA/event')
    pixelgroup.defineHistogram('avgIntPerXing,nClustersECC', type='TProfile',
                               xbins=250, xmin=0, xmax=100,
                               title=nPixClusterT+', endcap C;#mu;NclustersECC/event')
    pixelgroup.defineHistogram('avgIntPerXing,nClustersB0', type='TProfile',
                               xbins=250, xmin=0, xmax=100,
                               title=nPixClusterT+', barrel layer 0;#mu;NclustersB0/event')
    pixelgroup.defineHistogram('avgIntPerXing,nClustersB1', type='TProfile',
                               xbins=250, xmin=0, xmax=100,
                               title=nPixClusterT+', barrel layer 1;#mu;NclustersB1/event')
    pixelgroup.defineHistogram('avgIntPerXing,nClustersB2', type='TProfile',
                               xbins=250, xmin=0, xmax=100,
                               title=nPixClusterT+', barrel layer 2;#mu;NclustersB2/event')
