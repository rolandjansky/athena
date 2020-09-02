#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileClusterMonitorAlgorithm.py
@brief Python configuration of TileClusterMonitorAlgorithm algorithm for the Run III
'''
def TileClusterMonitoringConfig(flags, **kwargs):

    ''' Function to configure TileClusterMonitorAlgorithm algorithm in the monitoring system.'''

    # Define one top-level monitoring algorithm. The new configuration
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from TileMonitoring.TileTopoClusterConfig import TileTopoClusterCfg
    result.merge( TileTopoClusterCfg(flags) )

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags,'TileClusterMonitoring')

    # Adding an TileCellMonitorAlgorithm algorithm to the helper
    from AthenaConfiguration.ComponentFactory import CompFactory
    tileClusterMonAlg = helper.addAlgorithm(CompFactory.TileClusterMonitorAlgorithm, 'TileClusterMonAlg')

    tileClusterMonAlg.TriggerChain = ''

    #    from AthenaCommon.SystemOfUnits import MeV
    #kwargs.setdefault('EnergyThreshold', 50.0 * MeV)

    # L1Trigger Type Bits:
    #     bit0_RNDM, bit1_ZeroBias, bit2_L1Cal, bit3_Muon,
    #     bit4_RPC, bit5_FTK, bit6_CTP, bit7_Calib, AnyPhysTrig
    kwargs.setdefault('fillHistogramsForL1Triggers', ['AnyPhysTrig', 'bit7_Calib'])
    l1Triggers = kwargs['fillHistogramsForL1Triggers']

    for k, v in kwargs.items():
        setattr(tileClusterMonAlg, k, v)

    run = str(flags.Input.RunNumber[0])

    # 1) Configure histogram with TileClusterMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileClusterMonAlg, 'TileClusterMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = 'Cluster', type='TH1F',
                                     title = 'Time for execute TileClusterMonAlg algorithm;time [#mus]',
                                     xbins = 100, xmin = 0, xmax = 1000)


    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile


    from TileMonitoring.TileMonitoringCfgHelper import addTileEtaPhiMapsArray

    # ) Configure histograms with most energetic Tile tower position
    addTileEtaPhiMapsArray(helper, tileClusterMonAlg, name = 'TileClusterEtaPhi', type='TH2D',
                           title = 'Most energetic Tile Cluster position', path = 'Tile/Cluster',
                           run = run, triggers = l1Triggers, perSample = False)

    # ) Configure histograms with most energetic Tile tower position
    addTileEtaPhiMapsArray(helper, tileClusterMonAlg, name = 'TileAllClusterEtaPhi', type='TH2D',
                           title = 'All Tile Cluster position', path = 'Tile/Cluster',
                           run = run, triggers = l1Triggers, perSample = False)

    # ) Configure histograms with most energetic Tile tower position
    addTileEtaPhiMapsArray(helper, tileClusterMonAlg, name = 'TileAllClusterEneEtaPhi', type='TProfile2D',
                           value = 'energy', title = 'All Tile Cluster everage energy [MeV]',
                           path = 'Tile/Cluster', run = run, triggers = l1Triggers, perSample = False)


    # ) Configure histograms with position correlation of Tile cluster opposite to most energetic cluster
    addTileEtaPhiMapsArray(helper, tileClusterMonAlg, name = 'TileClusterEtaPhiDiff', type='TH2D',
                           title = 'Position correlation of Tile Cluster opposite to most energetic cluster',
                           path = 'Tile/Cluster', run = run, triggers = l1Triggers, perSample = False,
                           etaTitle = '#Delta #eta', etabins = 21, etamin = -2.025, etamax = 2.025,
                           phiTitle = '#Delta #phi', phibins = Tile.MAX_DRAWER, phimin = 0.0, phimax = 6.4)


    from TileMonitoring.TileMonitoringCfgHelper import addTile1DHistogramsArray

    # ) Configure histograms with all Tile towers energy per partition
    addTile1DHistogramsArray(helper, tileClusterMonAlg, name = 'TileClusterEnergy', xvalue = 'energy',
                             title = 'Energy in most energetic Tile Cluster [MeV]', path = 'Tile/Cluster',
                             xbins = 80, xmin = 0., xmax = 20000., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = True, perSample = False,
                             perGain = False, subDirectory = True, allPartitions = True)

    # ) Configure histograms with Et in most energetic Tile tower per partition
    addTile1DHistogramsArray(helper, tileClusterMonAlg, name = 'TileClusterEt', xvalue = 'Et',
                             title = 'E_{T} [MeV] in most energetic Tile Cluster', path = 'Tile/Cluster',
                             xbins = 80, xmin = 0., xmax = 20000., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # ) Configure histograms with all Tile towers energy per partition
    addTile1DHistogramsArray(helper, tileClusterMonAlg, name = 'TileClusterNCells', xvalue = 'nCells',
                             title = 'Number of cells in most energetic Tile Cluster', path = 'Tile/Cluster',
                             xbins = 100, xmin = 0., xmax = 100., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)


    # ) Configure histograms with all Tile towers energy per partition
    addTile1DHistogramsArray(helper, tileClusterMonAlg, name = 'TileAllClusterEnergy', xvalue = 'energy',
                             title = 'All Tile Cluster Energy [MeV]', path = 'Tile/Cluster',
                             xbins = 80, xmin = 0., xmax = 20000., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # ) Configure histograms with all Tile towers energy per partition
    addTile1DHistogramsArray(helper, tileClusterMonAlg, name = 'TileNClusters', xvalue = 'nClusters',
                             title = 'Number of Tile Clusters', path = 'Tile/Cluster',
                             xbins = 200, xmin = 0., xmax = 200., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # ) Configure histograms with all Tile towers energy per partition
    addTile1DHistogramsArray(helper, tileClusterMonAlg, name = 'TileClusterSumPx', xvalue = 'sumPx',
                             title = 'Tile Clusters SumPx [MeV]', path = 'Tile/Cluster',
                             xbins = 101, xmin = -10000., xmax = 10000., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # ) Configure histograms with all Tile towers energy per partition
    addTile1DHistogramsArray(helper, tileClusterMonAlg, name = 'TileClusterSumPy', xvalue = 'sumPy',
                             title = 'Tile Clusters SumPy [MeV]', path = 'Tile/Cluster',
                             xbins = 101, xmin = -10000., xmax = 10000., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # ) Configure histograms with all Tile towers energy per partition
    addTile1DHistogramsArray(helper, tileClusterMonAlg, name = 'TileClusterSumEt', xvalue = 'sumEt',
                             title = 'Tile Clusters SumEt [MeV]', path = 'Tile/Cluster',
                             xbins = 100, xmin = 0., xmax = 20000., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # ) Configure histograms with all Tile towers energy per partition
    timeDiffTitle = 'Tile time correlation of cluster opposite to most energetic cluster; Time difference [ns]'
    addTile1DHistogramsArray(helper, tileClusterMonAlg, name = 'TileClusterTimeDiff', xvalue = 'timeDiff',
                             title = timeDiffTitle, path = 'Tile/Cluster', xbins = 200, xmin = -100., xmax = 100.,
                             type = 'TH1D', run = run, triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # ) Configure histograms with all Tile towers energy per partition
    eneDiffTitle = 'Tile energy correlation of cluster opposite to most energetic cluster; Time energy [MeV]'
    addTile1DHistogramsArray(helper, tileClusterMonAlg, name = 'TileClusterEneDiff', xvalue = 'energyDiff',
                             title = eneDiffTitle, path = 'Tile/Cluster', xbins = 200, xmin = -10000., xmax = 10000.,
                             type = 'TH1D', run = run, triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)


    accumalator = helper.result()
    result.merge(accumalator)
    return result

if __name__=='__main__':

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.ESD

    ConfigFlags.Output.HISTFileName = 'TileClusterMonitorOutput.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    l1Triggers = ['bit0_RNDM', 'bit1_ZeroBias', 'bit2_L1Cal', 'bit3_Muon',
                  'bit4_RPC', 'bit5_FTK', 'bit6_CTP', 'bit7_Calib', 'AnyPhysTrig']

    cfg.merge( TileClusterMonitoringConfig(ConfigFlags,
                                           fillHistogramsForL1Triggers = l1Triggers) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileClusterMonitorAlgorithm.pkl','wb') )

    sc = cfg.run(maxEvents=-1)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
