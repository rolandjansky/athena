#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileRawChannelTimeMonitorAlgorithm.py
@brief Python configuration of TileRawChannelTimeMonitorAlgorithm algorithm for the Run III
'''
def TileRawChannelTimeMonitoringConfig(flags, **kwargs):

    ''' Function to configure TileRawChannelTimeMonitorAlgorithm algorithm in the monitoring system.'''

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

    kwargs.setdefault('TriggerChain', '')

    # Partition pairs to monitor average time difference between partitions (ROS - 1)
    partitionPairs = [[0, 1], [0, 2], [0, 3], [1, 2], [1, 3], [2, 3]]
    kwargs.setdefault('PartitionTimeDiffferncePairs', partitionPairs)

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags,'TileRawChannelTimeMonitoring')

    # Adding an TileCellMonitorAlgorithm algorithm to the helper
    from AthenaConfiguration.ComponentFactory import CompFactory
    TileRawChannelTimeMonitorAlgorithm = CompFactory.TileRawChannelTimeMonitorAlgorithm
    tileRawChanTimeMonAlg = helper.addAlgorithm(TileRawChannelTimeMonitorAlgorithm, 'TileRawChanTimeMonAlg')

    for k, v in kwargs.items():
        setattr(tileRawChanTimeMonAlg, k, v)

    run = str(flags.Input.RunNumber[0])

    # 1) Configure histogram with TileRawChannelTimeMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileRawChanTimeMonAlg, 'TileRawChanTimeMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = 'RawChannelTime', type='TH1F',
                                     title = 'Time for execute TileRawChanTimeMonAlg algorithm;time [#mus]',
                                     xbins = 100, xmin = 0, xmax = 100000)


    from TileMonitoring.TileMonitoringCfgHelper import addTileModuleChannelMapsArray

    # 2) Configure histograms with status of Tile channel time per partition
    addTileModuleChannelMapsArray(helper, tileRawChanTimeMonAlg, name = 'TileAverageTime',
                                  title = 'Tile average time', path = 'Tile/RawChannelTime/Summary',
                                  type = 'TProfile2D', value = 'time', run = run)

    from TileMonitoring.TileMonitoringCfgHelper import addTile2DHistogramsArray

    # 3) Configure histograms with Tile partition average time vs luminosity block per partition
    addTile2DHistogramsArray(helper, tileRawChanTimeMonAlg, name = 'TileAverageTimeLB',
                             xvalue = 'lumiBlock', yvalue = 'time', type='TH2D',
                             title = 'Tile Average time vs LumiBlock;LumiBlock;t [ns]',
                             path = 'Tile/RawChannelTime/Summary', run = run, perPartition = True,
                             xbins = 3000, xmin = -0.5, xmax = 2999.5, ybins = 149, ymin = -74.5, ymax = 74.5)


    from TileMonitoring.TileMonitoringCfgHelper import getPartitionName

    # 4) Configure histograms with Tile partition average time difference vs luminosity block
    partitionPairs = kwargs['PartitionTimeDiffferncePairs']
    partTimeDiffVsLBArray = helper.addArray([len(partitionPairs)], tileRawChanTimeMonAlg,
                                            'TileAverageTimeDifferenceLB', topPath = 'Tile/RawChannelTime')
    for postfix, tool in partTimeDiffVsLBArray.Tools.items():
        pairIdx = int(postfix.split('_').pop())
        partitionName1, partitionName2 = [getPartitionName(ros + 1) for ros in partitionPairs[pairIdx]]

        title = 'Run %s: Average time between %s and %s' % (run, partitionName1, partitionName2)
        title += ' vs luminosity block;LumiBlock;t [ns]'
        name = 'lumiBlock,time;TileAverageTimeDifferenceLB_%s-%s' % (partitionName1, partitionName2)

        tool.defineHistogram(name, title = title, path = 'Summary', type = 'TProfile',
                             xbins = 1000, xmin = -0.5, xmax = 999.5, opt = 'kAddBinsDynamically')


    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

    # 5) Configure histograms with Tile digitizer time vs luminosity block per digitizer
    maxDigitizer = 8
    digiTimeVsLBArray = helper.addArray([int(Tile.MAX_ROS - 1), int(Tile.MAX_DRAWER), maxDigitizer],
                                        tileRawChanTimeMonAlg, 'TileDigitizerTimeLB', topPath = 'Tile/RawChannelTime')
    for postfix, tool in digiTimeVsLBArray.Tools.items():
        ros, module, digitizer = [int(x) for x in postfix.split('_')[1:]]

        moduleName = Tile.getDrawerString(ros + 1, module)
        title = 'Run ' + run + ' ' + moduleName + ' Digitizer ' + str(digitizer)
        title +=  ': Time vs luminosity block;LumiBlock;t [ns]'
        name = 'lumiBlock,time;TileDigitizerTimeLB_' + moduleName + '_DIGI_' + str(digitizer)
        path = getPartitionName(ros + 1) + '/' + moduleName

        tool.defineHistogram(name, title = title, path = path, type = 'TProfile',
                             xbins = 1000, xmin = -0.5, xmax = 999.5, opt = 'kAddBinsDynamically')

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

    inputDirectory = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TileByteStream/TileByteStream-02-00-00'
    inputFile = 'data18_tilecomm.00363899.calibration_tile.daq.RAW._lb0000._TileREB-ROS._0005-200ev.data'
    ConfigFlags.Input.Files = [inputDirectory + '/' + inputFile]
    ConfigFlags.Output.HISTFileName = 'TileRawChannelTimeMonitorOutput.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False

    ConfigFlags.Tile.RunType = 'LAS'
    ConfigFlags.Tile.TimingType = 'GAP/LAS'
    ConfigFlags.Tile.doFit = True
    ConfigFlags.Tile.correctTime = True
    ConfigFlags.Tile.doOverflowFit = False
    ConfigFlags.Tile.BestPhaseFromCOOL = True
    ConfigFlags.Tile.NoiseFilter = 1

    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    tileTypeNames = ['TileRawChannelContainer/TileRawChannelCnt',
                     'TileDigitsContainer/TileDigitsCnt',
                     'TileBeamElemContainer/TileBeamElemCnt']
    cfg.merge( ByteStreamReadCfg(ConfigFlags, type_names = tileTypeNames) )

    from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerCfg
    cfg.merge( TileRawChannelMakerCfg(ConfigFlags) )

    cfg.merge( TileRawChannelTimeMonitoringConfig(ConfigFlags) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileRawChannelTimeMonitorAlgorithm.pkl','wb') )

    sc = cfg.run(maxEvents=3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
