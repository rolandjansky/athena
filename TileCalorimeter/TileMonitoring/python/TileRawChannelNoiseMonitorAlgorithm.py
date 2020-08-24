#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileRawChannelNoiseMonitorAlgorithm.py
@brief Python configuration of TileRawChannelNoiseMonitorAlgorithm algorithm for the Run III
'''
def TileRawChannelNoiseMonitoringConfig(flags, **kwargs):

    ''' Function to configure TileRawChannelNoiseMonitorAlgorithm algorithm in the monitoring system.'''

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

    if 'TileCondToolEmscale' not in kwargs:
        from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
        emScaleTool = result.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )
        kwargs['TileCondToolEmscale'] = emScaleTool

    kwargs.setdefault('CheckDCS', flags.Tile.useDCS)
    if kwargs['CheckDCS']:
        from TileConditions.TileDCSConfig import TileDCSCondAlgCfg
        result.merge( TileDCSCondAlgCfg(flags) )

    #kwargs.setdefault('TriggerChain', 'HLT_noalg_cosmiccalo_L1RD1_EMPTY') #FIXME
    kwargs.setdefault('TriggerTypes', [0x82])
    kwargs.setdefault('Gain', 1)
    kwargs.setdefault('TileRawChannelContainer', flags.Tile.RawChannelContainer)

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags,'TileRawChanNoiseMonitoring')

    # Adding an TileCellMonitorAlgorithm algorithm to the helper
    from AthenaConfiguration.ComponentFactory import CompFactory
    TileRawChannelNoiseMonitorAlgorithm = CompFactory.TileRawChannelNoiseMonitorAlgorithm
    tileRawChanNoiseMonAlg = helper.addAlgorithm(TileRawChannelNoiseMonitorAlgorithm, 'TileRawChanNoiseMonAlg')

    for k, v in kwargs.items():
        setattr(tileRawChanNoiseMonAlg, k, v)

    run = str(flags.Input.RunNumber[0])

    # 1) Configure histogram with TileRawChanNoiseMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileRawChanNoiseMonAlg, 'TileRawChanNoiseMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = 'RawChannelNoise', type='TH1F',
                                     title = 'Time for execute TileRawChanNoiseMonAlg algorithm;time [#mus]',
                                     xbins = 100, xmin = 0, xmax = 100000)


    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

    from TileMonitoring.TileMonitoringCfgHelper import getPartitionName, getCellName, getGainName

    # 2) Configure histograms with Tile raw channel amplitude per channel
    gainName = getGainName(kwargs['Gain'])
    dimensions = [int(Tile.MAX_ROS) - 1, int(Tile.MAX_DRAWER), int(Tile.MAX_CHAN)]
    chanAmpArray = helper.addArray(dimensions, tileRawChanNoiseMonAlg,
                                   'TileRawChannelNoise', topPath = 'Tile/RawChannelNoise')
    for postfix, tool in chanAmpArray.Tools.items():
        ros, module, channel = [int(x) for x in postfix.split('_')[1:]]

        partition = getPartitionName(ros + 1)
        moduleName = Tile.getDrawerString(ros + 1, module)
        cellName = getCellName(partition, channel)

        title = 'Run %s %s: Tile cell %s / channel %s amplitude (%s);Amplitude [ADC]'
        title = title % (run, moduleName, cellName, str(channel), gainName)
        name = 'amplitude;TileRawChannelNoise_%s_%s_ch_%s_%s' % (moduleName, cellName, str(channel), gainName)

        tool.defineHistogram(name, title = title, path = partition, type = 'TH1F',
                             xbins = 81, xmin = -20.25, xmax = 20.25)

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

    ConfigFlags.Input.Files = ['root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/det-tile/test/data14_cos.00239908.physics_CosmicCalo.merge.RAW._lb0004._SFO-ALL._0001.1']
    ConfigFlags.IOVDb.GlobalTag = 'CONDBR2-ES1PA-2018-02'

    ConfigFlags.Output.HISTFileName = 'TileRawChannelNoiseMonitorOutput.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    tileTypeNames = ['TileRawChannelContainer/TileRawChannelCnt', 'TileDigitsContainer/TileDigitsCnt']
    cfg.merge( ByteStreamReadCfg(ConfigFlags, type_names = tileTypeNames) )

    from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerCfg
    cfg.merge( TileRawChannelMakerCfg(ConfigFlags) )

    cfg.merge( TileRawChannelNoiseMonitoringConfig(ConfigFlags) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileRawChannelNoiseMonitorAlgorithm.pkl','wb') )

    sc = cfg.run(maxEvents=3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
