#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
@file TileMBTSMonitorAlgorithm.py
@brief Python configuration of TileMBTSMonitorAlgorithm algorithm for the Run III
'''


def TileMBTSMonitoringConfig(flags, **kwargs):

    ''' Function to configure TileMBTSMonitorAlgorithm algorithm in the monitoring system.'''

    # Define one top-level monitoring algorithm. The new configuration
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    result.merge( TileDQstatusAlgCfg(flags) )

    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(TileGMCfg(flags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    result.merge(LArGMCfg(flags))

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    result.merge( TileCablingSvcCfg(flags) )

    if flags.Input.Format.lower() == 'pool':
        kwargs.setdefault('TileDigitsContainer', 'TileDigitsFlt')

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags, 'TileMBTSMonAlgCfg')

    runNumber = flags.Input.RunNumber[0]
    from AthenaConfiguration.ComponentFactory import CompFactory
    _TileMBTSMonitoringConfigCore(helper, CompFactory.TileMBTSMonitorAlgorithm,
                                runNumber, **kwargs)

    accumalator = helper.result()
    result.merge(accumalator)
    return result


def TileMBTSMonitoringConfigOld(flags, **kwargs):

    ''' Function to configure TileMBTSMonitorAlgorithm algorithm in the old monitoring system.'''

    from AthenaMonitoring import AthMonitorCfgHelperOld
    from AthenaCommon.GlobalFlags import globalflags

    if globalflags.InputFormat().lower() == 'pool':
        kwargs.setdefault('TileDigitsContainer', 'TileDigitsFlt')

    helper = AthMonitorCfgHelperOld(flags, 'TileMBTSMonAlgCfg')

    from RecExConfig.AutoConfiguration import GetRunNumber
    runNumber = GetRunNumber()

    from TileMonitoring.TileMonitoringConf import TileMBTSMonitorAlgorithm
    _TileMBTSMonitoringConfigCore(helper, TileMBTSMonitorAlgorithm, runNumber, **kwargs)

    return helper.result()


def _TileMBTSMonitoringConfigCore(helper, algConfObj, runNumber, **kwargs):

    ''' Function to configure TileMBTSMonitorAlgorithm algorithm in the monitoring system.'''

    run = str(runNumber)

    # Adding an TileMBTSMonitorAlgorithm algorithm to the helper
    tileMBTSMonAlg = helper.addAlgorithm(algConfObj, 'TileMBTSMonAlg')

    tileMBTSMonAlg.TriggerChain = ''

    for k, v in kwargs.items():
        setattr(tileMBTSMonAlg, k, v)

    if 'FillHistogramsPerMBTS' in kwargs:
        fillHistogramPerMBTS = kwargs['FillHistogramsPerMBTS']
    elif hasattr(tileMBTSMonAlg,"_descriptors"): #GaudiConfig2 config
        fillHistogramPerMBTS = tileMBTSMonAlg._descriptors['FillHistogramsPerMBTS'].default
    else: #Run1/2 config
        fillHistogramPerMBTS = tileMBTSMonAlg.getDefaultProperty('FillHistogramsPerMBTS')


    # 1) Configure histogram with TileMBTSMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileMBTSMonAlg, 'TileMBTSMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = 'MBTS', type='TH1F',
                                     title = 'Time for execute TileMBTSMonAlg algorithm;time [#mus]',
                                     xbins = 100, xmin = 0, xmax = 1000)



    numberOfMBTS = 32

    labelsMBTS  =  ['MBTSA' + ('0' if x < 10 else '') + str(x) for x in range(0, numberOfMBTS // 2)]
    labelsMBTS +=  ['MBTSC' + ('0' if x < 10 else '') + str(x) for x in range(0, numberOfMBTS // 2)]


    # 2) Configure MBTS occupancy histogram
    occupancyGroup = helper.addGroup(tileMBTSMonAlg, 'TileOccupancyMBTS', 'Tile')
    occupancyGroup.defineHistogram('EnergyCounter;Occupancy', path = 'MBTS/Cell', type='TH1F', weight = 'SummaryEnergy',
                                   xlabels = labelsMBTS, title = 'Run ' + run + ': MBTS Occupancy',
                                   xbins = numberOfMBTS, xmin = 0, xmax = numberOfMBTS)

    # 3) Configure summary energy histogram
    energySummaryGroup = helper.addGroup(tileMBTSMonAlg, 'TileEnergySummaryMBTS', 'Tile')
    energySummaryGroup.defineHistogram('EnergyCounter,SummaryEnergy;SummaryEnergy', path = 'MBTS/Cell', type='TProfile',
                                       title = 'Run ' + run + ': Average MBTS Energy;;Average Energy [pC]',
                                       xlabels = labelsMBTS, xbins = numberOfMBTS, xmin = 0, xmax = numberOfMBTS)

    # 4) Configure histogram with average MBTS time (energy above threshold)
    timeSummaryGroup = helper.addGroup(tileMBTSMonAlg, 'TileTimeSummaryMBTS', 'Tile')
    timeSummaryGroup.defineHistogram('TimeCounter,SummaryTime;SummaryTime', path = 'MBTS/Cell', type='TProfile',
                                     title = 'Run ' + run + ': Average MBTS Time (Energy above threshold);;Average Time [ns]',
                                     xlabels = labelsMBTS, xbins = numberOfMBTS, xmin = 0, xmax = numberOfMBTS)

    # 5) Configure histogram with time of MBTS on A side
    timeAGroup = helper.addGroup(tileMBTSMonAlg, 'TileTimeMBTSA', 'Tile')
    timeAGroup.defineHistogram('TimeA;Time_A', path = 'MBTS/Cell', type='TH1F',
                               title = 'Run ' + run + ': Time of MBTS on A side;Time [ns]',
                               xbins = 151, xmin = -75.5, xmax = 75.5)

    # 6) Configure histogram time of MBTS on C side
    timeCGroup = helper.addGroup(tileMBTSMonAlg, 'TileTimeMBTSC', 'Tile')
    timeCGroup.defineHistogram('TimeC;Time_C', path = 'MBTS/Cell', type='TH1F',
                               title = 'Run ' + run + ': Time of MBTS on C side;Time [ns]',
                               xbins = 151, xmin = -75.5, xmax = 75.5)

    # 7) Configure histogram with time difference between MBTS on A and C sides
    timeDifferenceGroup = helper.addGroup(tileMBTSMonAlg, 'TileTimeDifferenceMBTS', 'Tile')
    timeDifferenceGroup.defineHistogram('TimeDifference;TimeDiff_A-C', path = 'MBTS/Cell', type='TH1F',
                                        title = 'Run ' + run + ': Time difference between MBTS on A and C sides;Time [ns]',
                                        xbins = 151, xmin = -75.5, xmax = 75.5)

    # 8) Configure histogram with time difference between MBTS on A and C sides vs luminosity block
    timeDifferenceLBGroup = helper.addGroup(tileMBTSMonAlg, 'TileTimeDifferenceMBTSLB', 'Tile')
    timeDifferenceLBGroup.defineHistogram('lumiBlock,TimeDifference;TimeDiff_A-C_LB', path = 'MBTS/Cell', type='TH2F',
                                          title = ('Run ' + run + ': Time difference between MBTS on A and C sides vs LumiBlock'
                                                   + ';Luminosity Block;Time difference A-side - C-side [ns]'),
                                          xbins = 1000, xmin = -0.5, xmax = 999.5, ybins = 151, ymin = -75.5, ymax = 75.5,
                                          opt = 'kAddBinsDynamically')

    # 9) Configure histogram with coincident Hits (energy) between two MBTS counters
    coincidentHitsGroup = helper.addGroup(tileMBTSMonAlg, 'TileCoincidentHitsMBTS', 'Tile')
    coincidentHitsGroup.defineHistogram('CoincidentCounter1,CoincidentCounter2;CoincidentEnergyHits',
                                        path = 'MBTS/Cell', type='TH2F', xlabels = labelsMBTS, ylabels = labelsMBTS,
                                        title = 'Run ' + run + ': Coincident Hits (energy) between two MBTS counters',
                                        xbins = numberOfMBTS, xmin = 0, xmax = numberOfMBTS,
                                        ybins = numberOfMBTS, ymin = 0, ymax = numberOfMBTS)



    # 10) Configure histogram with Tile readout errors for MBTS counters
    errorLabels = ['General', 'Global CRC', 'ROD CRC', 'Frontend CRC', 'BCID', 'Header Format',
                   'Header Parity', 'Sample Format', 'Sample Parity', 'Memory Parity']
    numberOfErrors = len(errorLabels)
    errorsGroup = helper.addGroup(tileMBTSMonAlg, 'TileErrorsMBTS', 'Tile')
    errorsGroup.defineHistogram('ErrorCounter,Error;ReadOutErrors', path = 'MBTS/Digit',
                                type='TH2F', xlabels = labelsMBTS, ylabels = errorLabels,
                                title = 'Run ' + run + ': Tile Readout Errors for MBTS counters',
                                xbins = numberOfMBTS, xmin = 0, xmax = numberOfMBTS,
                                ybins = numberOfErrors, ymin = 0, ymax = numberOfErrors)



    if fillHistogramPerMBTS:

        # 11) Configure histogram with MBTS counter energy
        energyArray = helper.addArray([numberOfMBTS], tileMBTSMonAlg, 'TileEnergyMBTS', topPath = 'Tile/MBTS')
        for postfix, tool in energyArray.Tools.items():
            mbtsCounter = int( postfix.split('_')[1] )
            mbtsName = labelsMBTS[mbtsCounter]
            title = 'Run ' + run + ': Energy of ' + mbtsName + ';Energy [pC]'
            name = 'Energy;Energy_' + mbtsName
            tool.defineHistogram(name, title = title, type = 'TH1F', path = 'Cell',
                                 xbins = 400, xmin = -0.5, xmax = 80)

        # 12) Configure histogram with MBTS counter energy vs luminosity block
        energyLBArray = helper.addArray([numberOfMBTS], tileMBTSMonAlg, 'TileEnergyLBMBTS', topPath = 'Tile/MBTS')
        for postfix, tool in energyLBArray.Tools.items():
            mbtsCounter = int( postfix.split('_')[1] )
            mbtsName = labelsMBTS[mbtsCounter]
            title = 'Run ' + run + ': Energy of ' + mbtsName + ' per lumiblock;Lumiblocks;Energy [pC]'
            name = 'lumiBlock,Energy;EnergyLB_' + mbtsName
            tool.defineHistogram(name, title = title, type = 'TProfile', path = 'Cell',
                                 xbins = 1000, xmin = -0.5, xmax = 999.5, opt = 'kAddBinsDynamically')

        # 13) Configure histogram with MBTS counter time
        timeArray = helper.addArray([numberOfMBTS], tileMBTSMonAlg, 'TileTimeMBTS', topPath = 'Tile/MBTS')
        for postfix, tool in timeArray.Tools.items():
            mbtsCounter = int( postfix.split('_')[1] )
            mbtsName = labelsMBTS[mbtsCounter]
            title = 'Run ' + run + ': Time of ' + mbtsName + ';Time [ns]'
            name = 'Time;Time_' + mbtsName
            tool.defineHistogram(name, title = title, type = 'TH1F', path = 'Cell',
                                 xbins = 151, xmin = -75.5, xmax = 75.5)

        # 14) Configure histogram with MBTS counter pedestal
        pedestalArray = helper.addArray([numberOfMBTS], tileMBTSMonAlg, 'TilePedestalMBTS', topPath = 'Tile/MBTS')
        for postfix, tool in pedestalArray.Tools.items():
            mbtsCounter = int( postfix.split('_')[1] )
            mbtsName = labelsMBTS[mbtsCounter]
            title = 'Run ' + run + ': Pedestal of ' + mbtsName + ';Channel pedestal in ADC counts'
            name = 'Pedestal;Pedestal_' + mbtsName
            tool.defineHistogram(name, title = title, type = 'TH1F', path = 'Digit',
                                 xbins = 100, xmin = 20.5, xmax = 120.5)

        # 15) Configure histogram with high frequency noise of MBTS counter
        hfnArray = helper.addArray([numberOfMBTS], tileMBTSMonAlg, 'TileHFNoiseMBTS', topPath = 'Tile/MBTS')
        for postfix, tool in hfnArray.Tools.items():
            mbtsCounter = int( postfix.split('_')[1] )
            mbtsName = labelsMBTS[mbtsCounter]
            title = 'Run ' + run + ': High Frequency Noise of ' + mbtsName + ';Inter-sample RMS in ADC counts'
            name = 'HFN;HFN_' + mbtsName
            tool.defineHistogram(name, title = title, type = 'TH1F', path = 'Digit',
                                 xbins = 100, xmin = 0, xmax = 5)

        # 16) Configure histogram with average pulse shape of MBTS counter
        averagePulseArray = helper.addArray([numberOfMBTS], tileMBTSMonAlg, 'TileAveragePulseMBTS', topPath = 'Tile/MBTS')
        for postfix, tool in averagePulseArray.Tools.items():
            mbtsCounter = int( postfix.split('_')[1] )
            mbtsName = labelsMBTS[mbtsCounter]
            title = 'Run ' + run + ': Average Pulse for digitized signals of ' + mbtsName
            title += ';TileCal Digital Sample;Average number of ADC counts'
            name = 'SampleNumbers,Samples;AveragePulse_' + mbtsName
            tool.defineHistogram(name, title = title, type = 'TProfile', path = 'Digit',
                                 xbins = 7, xmin = -0.5, xmax = 6.5)



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
    ConfigFlags.Output.HISTFileName = 'TileMBTSMonitorOutput.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    cfg.merge( TileMBTSMonitoringConfig(ConfigFlags, FillHistogramsPerMBTS = True) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileMBTSMonitorAlgorithm.pkl','wb') )

    sc = cfg.run(maxEvents=3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
