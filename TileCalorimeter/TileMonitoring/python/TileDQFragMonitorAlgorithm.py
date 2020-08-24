#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
@file TileDQFragMonitorAlgorithm.py
@brief Python configuration of TileDQFragMonitorAlgorithm algorithm for the Run III
'''

def TileDQFragMonitoringConfig(flags, **kwargs):

    ''' Function to configure TileDQFragMonitorAlgorithm algorithm in the monitoring system.'''

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
    result.merge(DetDescrCnvSvcCfg(flags))

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    result.merge( TileCablingSvcCfg(flags) )

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    result.merge( TileDQstatusAlgCfg(flags) )

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    result.merge( TileInfoLoaderCfg(flags) )

    from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
    badChanTool = result.popToolsAndMerge( TileBadChanToolCfg(flags) )
    kwargs['TileBadChanTool'] = badChanTool

    kwargs.setdefault('CheckDCS', flags.Tile.useDCS)
    if kwargs['CheckDCS']:
        from TileConditions.TileDCSConfig import TileDCSCondAlgCfg
        result.merge( TileDCSCondAlgCfg(flags) )

    rawChannelContainer = flags.Tile.RawChannelContainer
    if flags.Input.Format.lower() == 'pool':
        kwargs.setdefault('TileDigitsContainer', 'TileDigitsFlt')
        if rawChannelContainer not in flags.Input.Collections:
            rawChannelContainer = ''

    kwargs.setdefault('TileRawChannelContainer', rawChannelContainer)

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags, 'TileDQFragMonAlgCfg')

    runNumber = flags.Input.RunNumber[0]
    from AthenaConfiguration.ComponentFactory import CompFactory
    _TileDQFragMonitoringCore(helper, CompFactory.TileDQFragMonitorAlgorithm,
                             runNumber, **kwargs)

    accumalator = helper.result()
    result.merge(accumalator)
    return result


def TileDQFragMonitoringConfigOld(flags, **kwargs):

    ''' Function to configure TileDQFragMonitorAlgorithm algorithm in the old monitoring system.'''

    from AthenaMonitoring import AthMonitorCfgHelperOld
    from AthenaCommon.GlobalFlags import globalflags

    if globalflags.InputFormat().lower() == 'pool':
        kwargs.setdefault('TileDigitsContainer', 'TileDigitsFlt')

    from TileRecUtils.TileRecFlags import jobproperties
    rawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()
    kwargs.setdefault('TileRawChannelContainer', rawChannelContainer)

    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")
    kwargs.setdefault('CheckDCS', hasattr(condSequence, 'TileDCSCondAlg'))

    helper = AthMonitorCfgHelperOld(flags, 'TileDQFragMonAlgCfg')

    from RecExConfig.AutoConfiguration import GetRunNumber
    runNumber = GetRunNumber()

    from TileMonitoring.TileMonitoringConf import TileDQFragMonitorAlgorithm
    _TileDQFragMonitoringCore(helper, TileDQFragMonitorAlgorithm, runNumber, **kwargs)

    return helper.result()

def _TileDQFragMonitoringCore(helper, algConfObj, runNumber, **kwargs):

    ''' Function to configure TileDQFragMonitorAlgorithm algorithm in the monitoring system.'''

    run = str(runNumber)

    # Adding an TileDQFragMonitorAlgorithm algorithm to the helper; try to accommodate old/new configuration styles
    tileDQFragMonAlg = helper.addAlgorithm(algConfObj, 'TileDQFragMonAlg')

    for k, v in kwargs.items():
        setattr(tileDQFragMonAlg, k, v)

    # 1) Configure histogram with TileDQFragMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileDQFragMonAlg, 'TileDQFragMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = 'DMUErrors', type='TH1F',
                                     title = 'Time for execute TileDQFragMonAlg algorithm;time [#mus]',
                                     xbins = 300, xmin = 0, xmax = 300000)

    # 2) Configure histogram with Tile error state in EventInfo vs lumi block
    errorStateGroup = helper.addGroup(tileDQFragMonAlg, 'TileEventsWithErrEventInfoLB', 'Tile/DMUErrors')
    errorStateGroup.defineHistogram('lumiBlock;TileEventsWithErrEventInfo', path = 'BadDrawers', type='TH1F',
                                    title = '# events with Tile error state in EventInfo;LumiBlock;# events with error',
                                    xbins = 1000, xmin = -0.5, xmax = 999.5, opt = 'kAddBinsDynamically')

    # 3) Configure histogram with number of consecutive bad Tile modules
    consecutiveBadGroup = helper.addGroup(tileDQFragMonAlg, 'TileConsecutiveBadModules', 'Tile/DMUErrors')
    consecutiveBadGroup.defineHistogram('TileConsecutiveBad', path = 'BadDrawers', type='TH1F',
                                        title = 'Max # Tile consecutive bad modules;# consecutive bad modules;N',
                                        xbins = 17, xmin = -0.5, xmax = 16.5)

    # 4) Configure histogram with number of consecutive bad Tile modules per lumi block
    consecutiveBadLBGroup = helper.addGroup(tileDQFragMonAlg, 'TileConsecutiveBadModulesLB', 'Tile/DMUErrors')
    consecutiveBadLBGroup.defineHistogram('lumiBlock,TileConsecutiveBad;TileConsecutiveBadLB', path = 'BadDrawers', type='TH2F',
                                          title = 'Max # Tile consecutive bad modules;LumiBlock;# consecutive bad modules',
                                          xbins = 1000, xmin = -0.5, xmax = 999.5, ybins = 17, ymin = -0.5, ymax = 16.5,
                                          opt = 'kAddBinsDynamically')


    from TileMonitoring.TileMonitoringCfgHelper import getPartitionName
    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

    # 5) Configure histogram with mismatched L1 trigger type of Tile module
    moduleLabels = [str(module) for module in range(1, Tile.MAX_DRAWER + 1)]
    partitionLabels = [getPartitionName(ros) for ros in range(1, Tile.MAX_ROS)]

    mismatchedLVL1Group = helper.addGroup(tileDQFragMonAlg, 'TileMismatchedL1TiggerType', 'Tile/')
    mismatchedLVL1Group.defineHistogram('module,ROS;TileMismatchedL1TiggerType', path = 'DMUErrors',
                                        title = run + ': Tile mismatched L1 Trigger Type;Module;Partition',
                                        type = 'TH2F', xlabels = moduleLabels, ylabels = partitionLabels,
                                        xbins = Tile.MAX_DRAWER, xmin = -0.5, xmax = Tile.MAX_DRAWER - 0.5,
                                        ybins = Tile.MAX_ROS - 1, ymin = 1.0, ymax = Tile.MAX_ROS)


    # 6) Configure histogram with no all Tile digits in the case of Trigger Type = 0x82
    noAllDigitsGroup = helper.addGroup(tileDQFragMonAlg, 'TileNoAllDigits', 'Tile/')
    noAllDigitsGroup.defineHistogram('module,ROS;TileNoalldigits', path = 'DMUErrors', type = 'TH2F',
                                     title = run + ': No All Tile digits in event with Trigger Type = 0x82;Module;Partition',
                                     xlabels = moduleLabels, ylabels = partitionLabels,
                                     xbins = Tile.MAX_DRAWER, xmin = 0.0, xmax = Tile.MAX_DRAWER,
                                     ybins = Tile.MAX_ROS - 1, ymin = 1.0, ymax = Tile.MAX_ROS)



    # 7) Configure histograms with Tile DMU errors
    maxDMUs = 16
    dmuLabels = [str(dmu) for dmu in range(0, maxDMUs)]

    dmuErrorLabels = ['OK', 'HEADER_FORM', 'HEADER_PAR', 'MEMO_PAR', 'FE_CRC', 'ROD_CRC', 'BCID']
    dmuErrorLabels += ['SAMPLE_FORM', 'SAMPLE_PAR', 'DOUBLE_STB', 'SINGLE_STB', 'GLOBAL_CRC']
    dmuErrorLabels += ['DUMMY_FRAG', 'NO_RECO_FRAG', 'MASKED', 'ALL_M_BAD_DCS', 'ANY_CH_BAD_HV']

    dmuErrorLabels += ['0 -> 1023', 'Zeros', 'Two 1023 + ped', 'Jump 2 levels', 'Single Up + ped']
    dmuErrorLabels += ['Single Dn + ped', 'Single Up + sig', 'Single Dn + sig', 'Ped > 200 LG']
    dmuErrorLabels += ['Single Dn LG_s0', 'Single Dn LG_s6', 'Up LG_s0_s6 or Gap', 'Dn LG_s0_s6 or Gap']

    maxErrors = len(dmuErrorLabels)

    errorsArray = helper.addArray([int(Tile.MAX_ROS - 1), int(Tile.MAX_DRAWER)],
                                  tileDQFragMonAlg, 'TileDigiErrors', topPath = 'Tile/')
    for postfix, tool in errorsArray.Tools.items():
        ros, module = [int(x) for x in postfix.split('_')[1:]]

        moduleName = Tile.getDrawerString(ros + 1, module)
        title = 'Run ' + run + ': ' + moduleName + ' DMU Header Errors;DMU'
        name = 'DMU,Error;TileDigiErrors' + moduleName

        tool.defineHistogram(name, title = title, type = 'TH2F', path = 'DMUErrors',
                             xlabels = dmuLabels, ylabels = dmuErrorLabels,
                             xbins = maxDMUs, xmin = 0.0, xmax = maxDMUs,
                             ybins = maxErrors, ymin = 0.0, ymax = maxErrors)



    # 8) Configure histograms with fraction of events/DMUs Tile DMU errors vs lumi blocks
    errorsVsLBArray = helper.addArray([int(Tile.MAX_ROS - 1), int(Tile.MAX_DRAWER)],
                                      tileDQFragMonAlg, 'FracTileDigiErrors', topPath = 'Tile/')
    for postfix, tool in errorsVsLBArray.Tools.items():
        ros, module = [int(x) for x in postfix.split('_')[1:]]

        moduleName = Tile.getDrawerString(ros + 1, module)
        title = 'Run ' + run + ': ' + moduleName + ' (#total_events - #ok_events)/(#total_events)'
        title += ';LumiBlock;Fraction of Digital errors'
        name = 'lumiBlock,fractionOfBadDMUs;FracTileDigiErrors' + moduleName

        tool.defineHistogram(name, title = title, path = 'DMUErrors', type = 'TProfile',
                             xbins = 1000, xmin = -0.5, xmax = 999.5, opt = 'kAddBinsDynamically')



    from TileMonitoring.TileMonitoringCfgHelper import addTileModuleChannelMapsArray

    # 9) Configure histograms with # of jumps errors per Tile partition
    addTileModuleChannelMapsArray(helper, tileDQFragMonAlg, path = 'Tile/DMUErrors/BadDrawers',
                                  name = 'TileBadChannelsJumpMap', title = '# Jump errors',
                                  run = run)

    # 10) Configure histograms with # of not masked jumps errors per Tile partition
    addTileModuleChannelMapsArray(helper, tileDQFragMonAlg, path = 'Tile/DMUErrors/BadDrawers',
                                  name = 'TileBadChannelsJumpNotMaskMap', title = '# Not masked Jump errors',
                                  run = run)


    # 11) Configure histograms with Tile bad pulse shape
    addTileModuleChannelMapsArray(helper, tileDQFragMonAlg, path = 'Tile/DMUErrors/BadDrawers',
                                  name = 'TileBadPulseQualityMap', run = run,
                                  title = 'Bad pulse shape or #chi^{2} from Optimal Filtering algirithm')

    # 12) Configure histograms with Tile channel negative amplitudes below threshold
    addTileModuleChannelMapsArray(helper, tileDQFragMonAlg, path = 'Tile/DMUErrors/BadDrawers',
                                  name = 'TileBadChannelsNegMap', title = '# Negative amplitude',
                                  run = run)

    # 13) Configure histograms with not masked Tile channel negative amplitudes below threshold
    addTileModuleChannelMapsArray(helper, tileDQFragMonAlg, path = 'Tile/DMUErrors/BadDrawers',
                                  name = 'TileBadChannelsNegNotMaskMap', run = run,
                                  title = '# Not masked negative amplitude')




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
    ConfigFlags.Output.HISTFileName = 'TileDQFragMonitorOutput.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.Tile.doOptATLAS = True
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    tileTypeNames = ['TileRawChannelContainer/TileRawChannelCnt', 'TileDigitsContainer/TileDigitsCnt']
    cfg.merge( ByteStreamReadCfg(ConfigFlags, type_names = tileTypeNames) )

    from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerCfg
    cfg.merge( TileRawChannelMakerCfg(ConfigFlags) )

    cfg.merge( TileDQFragMonitoringConfig(ConfigFlags) )

    cfg.printConfig(withDetails = True, summariseProps = True)

    cfg.store( open('TileDQFragMonitorAlgorithm.pkl','wb') )

    sc = cfg.run(maxEvents = 3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
