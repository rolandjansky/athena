#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileRODMonitorAlgorithm.py
@brief Python configuration of TileRODMonitorAlgorithm algorithm for the Run III
'''

def TileRODMonitoringConfig(flags, **kwargs):
    ''' Function to configure TileRODMonitorAlgorithm algorithm in the monitoring system.'''

    # Define one top-level monitoring algorithm. The new configuration
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    result.merge( TileDQstatusAlgCfg(flags) )

    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(TileGMCfg(flags))

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    result.merge( TileCablingSvcCfg(flags) )

    from TileConditions.TileBadChannelsConfig import TileBadChannelsCondAlgCfg
    result.merge( TileBadChannelsCondAlgCfg(flags, **kwargs) )

    kwargs.setdefault('CheckDCS', flags.Tile.useDCS)
    if kwargs['CheckDCS']:
        from TileConditions.TileDCSConfig import TileDCSCondAlgCfg
        result.merge( TileDCSCondAlgCfg(flags) )

    from AthenaConfiguration.ComponentFactory import CompFactory

    kwargs.setdefault('fillRODFragmentSizeHistograms', True)
    fillRODFragmentSizeHistograms = kwargs['fillRODFragmentSizeHistograms']
    if fillRODFragmentSizeHistograms:
        result.addService(CompFactory.ROBDataProviderSvc())

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags,'TileRODMonitoring')

    # Adding an TileRODMonitorAlgorithm algorithm to the helper
    tileRODMonAlg = helper.addAlgorithm(CompFactory.TileRODMonitorAlgorithm, 'TileRODMonAlg')

    # L1Trigger Type Bits:
    #     bit0_RNDM, bit1_ZeroBias, bit2_L1Cal, bit3_Muon,
    #     bit4_RPC, bit5_FTK, bit6_CTP, bit7_Calib, AnyPhysTrig
    kwargs.setdefault('fillHistogramsForL1Triggers', ['AnyPhysTrig', 'bit7_Calib'])
    l1Triggers = kwargs['fillHistogramsForL1Triggers']

    kwargs.setdefault('EnergyThreshold', 300.)
    energyThreshold = kwargs['EnergyThreshold']

    kwargs.setdefault('TileRawChannelContainer', flags.Tile.RawChannelContainer)
    kwargs.setdefault('NumberOfROBFragmets', 32)
    nROBs = kwargs['NumberOfROBFragmets']

    for k, v in kwargs.items():
        setattr(tileRODMonAlg, k, v)

    run = str(flags.Input.RunNumber[0])

    # Configure histogram with TileRODMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileRODMonAlg, 'TileRODMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = 'ROD', type='TH1F',
                                     title = 'Time for execute TileRODMonAlg algorithm;time [#ms]',
                                     xbins = 100, xmin = 0, xmax = 100000)


    from TileMonitoring.TileMonitoringCfgHelper import addTile1DHistogramsArray

    # Configure histograms with relative difference between Tile DSP and offline energies
    titleEnergyDiff = 'Relative difference between E_{DSP} and E_{OFFLINE}'
    titleEnergyDiff += ';(E_{DSP}- E_{OFFLINE})/E_{OFFLINE}'
    addTile1DHistogramsArray(helper, tileRODMonAlg, name = 'TileDspEnergyDiff',
                             xvalue = 'energyDiff', title = titleEnergyDiff, path = 'Tile/ROD',
                             xbins = 41, xmin = -0.205, xmax = 0.205, type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = True)

    # Configure histograms with difference between Tile DSP and offline times
    titleTimeDiff = 'Difference between t_{DSP} and t_{OFFLINE}'
    titleTimeDiff += ';t_{DSP}- t_{OFFLINE} [ns]'
    addTile1DHistogramsArray(helper, tileRODMonAlg, name = 'TileDspTimeDiff',
                             xvalue = 'timeDiff', title = titleTimeDiff, path = 'Tile/ROD',
                             xbins = 101, xmin = -50.5, xmax = 50.5, type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = True)

    # Configure histograms with difference between Tile DSP and offline times vs offline time
    titleTimeDiffVsTime = 'Difference between t_{DSP} and t_{OFFLINE}'
    titleTimeDiffVsTime += ';t_{OFFLINE} [ns];t_{DSP}- t_{OFFLINE} [ns]'
    addTile1DHistogramsArray(helper, tileRODMonAlg, name = 'TileDspTimeDiffVsTime',
                             xvalue = 'time', value = 'timeDiff', title = titleTimeDiffVsTime,
                             path = 'Tile/ROD', xbins = 51, xmin = -25.5, xmax = 25.5,
                             type = 'TProfile', run = run, triggers = l1Triggers, perPartition = True)

    # Configure histograms with difference between Tile DSP and offline times vs offline energy
    titleTimeDiffVsEnergy = 'Difference between t_{DSP} and t_{OFFLINE}'
    titleTimeDiffVsEnergy += ';E_{OFFLINE} [MeV];t_{DSP}- t_{OFFLINE}'
    addTile1DHistogramsArray(helper, tileRODMonAlg, name = 'TileDspTimeDiffVsEnergy',
                             xvalue = 'energy', value = 'timeDiff', title = titleTimeDiffVsEnergy,
                             path = 'Tile/ROD', xbins = 75, xmin = energyThreshold, xmax = 15000.,
                             type = 'TProfile', run = run, triggers = l1Triggers, perPartition = True)


    from TileMonitoring.TileMonitoringCfgHelper import addTile2DHistogramsArray

    # Configure histograms with relative difference between Tile DSP and offline energies vs offline time
    titleEnergyDiffVsTime = 'Relative difference between E_{DSP} and E_{OFFLINE}'
    titleEnergyDiffVsTime += ';t_{DSP} [ns];(E_{DSP}- E_{OFFLINE})/E_{OFFLINE}'
    addTile2DHistogramsArray(helper, tileRODMonAlg, name = 'TileDspEnergyDiffVsTime',
                             xvalue = 'time', yvalue = 'energyDiff',
                             title = titleEnergyDiffVsTime, path = 'Tile/ROD',
                             xbins = 51, xmin = -25.5, xmax = 25.5,
                             ybins = 30, ymin = -0.35, ymax = 0.1,
                             type = 'TH2D', run = run, triggers = l1Triggers, perPartition = True)

    # Configure histograms with relative difference between Tile DSP and offline energies vs offline energy
    titleEnergyDiffVsEnergy = 'Relative difference between E_{DSP} and E_{OFFLINE}'
    titleEnergyDiffVsEnergy += ';E_{OFFLINE} [MeV];(E_{DSP}- E_{OFFLINE})/E_{OFFLINE}'
    addTile2DHistogramsArray(helper, tileRODMonAlg, name = 'TileDspEnergyDiffVsEnergy',
                             xvalue = 'energy', yvalue = 'energyDiff',
                             title = titleEnergyDiffVsEnergy, path = 'Tile/ROD',
                             xbins = 75, xmin = energyThreshold, xmax = 15000.,
                             ybins = 82, ymin = -0.205, ymax = 0.205,
                             type = 'TH2D', run = run, triggers = l1Triggers, perPartition = True)


    from TileMonitoring.TileMonitoringCfgHelper import addTileModuleChannelMapsArray

    # Configure histograms with everagy Tile channel time per partition
    titleChanTime = ('Tile DSP Channel Time (t_{DSP}) [ns], E_{ch} > %s MeV' % energyThreshold)
    addTileModuleChannelMapsArray(helper, tileRODMonAlg, name = 'TileDspChannelTime', type = 'TProfile2D',
                                  value = 'time', title = titleChanTime, path = 'Tile/ROD',
                                  run = run, triggers = l1Triggers, separator = '_')


    if fillRODFragmentSizeHistograms:
        # Configure histogram with all Tile ROD fragments size vs luminosity block
        titleAllRodFragSize = 'All Tile ROD fragments size vs luminosity block;LumiBlock;# words'
        addTile1DHistogramsArray(helper, tileRODMonAlg, name = 'TileRodFragmentSizeLB',
                                 xvalue = 'lumiBlock', value = 'allRodFragsSize',
                                 title = titleAllRodFragSize, path = 'Tile/ROD',
                                 xbins = 1000, xmin = -0.5, xmax = 999.5,
                                 type = 'TProfile', run = run, triggers = l1Triggers,
                                 perPartition = False, opt = 'kAddBinsDynamically')

        from TileCalibBlobObjs.Classes import TileCalibUtils as Tile
        from TileMonitoring.TileMonitoringCfgHelper import getPartitionName

        # Configure histogram with Tile ROD fragment size vs fragment number and partition
        titleRodFragSize = 'Tile ROD fragment size [word]; Fragment;'
        partitionLabels = [getPartitionName(ros) for ros in range(1, Tile.MAX_ROS)]
        addTile2DHistogramsArray(helper, tileRODMonAlg, name = 'TileRodFragmentMapSize',
                                 xvalue = 'fragment', yvalue = 'partition', value = 'rodFragSize',
                                 title = titleRodFragSize, path = 'Tile/ROD',
                                 xbins = nROBs, xmin = -0.5, xmax = nROBs - 0.5,
                                 ybins = Tile.MAX_ROS - 1, ymin = 0.5, ymax = Tile.MAX_ROS - 0.5,
                                 type = 'TProfile2D', run = run, triggers = l1Triggers, ylabels = partitionLabels)


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
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Output.HISTFileName = 'TileRODMonitorOutput.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.Tile.doOptATLAS = True
    ConfigFlags.Exec.MaxEvents = 3
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    tileTypeNames = ['TileRawChannelContainer/TileRawChannelCnt', 'TileDigitsContainer/TileDigitsCnt']
    cfg.merge( ByteStreamReadCfg(ConfigFlags, type_names = tileTypeNames) )

    from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerCfg
    cfg.merge( TileRawChannelMakerCfg(ConfigFlags) )

    cfg.merge( TileRODMonitoringConfig(ConfigFlags) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileRODMonitorAlgorithm.pkl','wb') )

    sc = cfg.run()

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
