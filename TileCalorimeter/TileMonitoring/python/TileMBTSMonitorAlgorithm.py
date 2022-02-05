#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileMBTSMonitorAlgorithm.py
@brief Python configuration of TileMBTSMonitorAlgorithm algorithm for the Run III
'''


def TileMBTSMonitoringConfig(flags, **kwargs):

    ''' Function to configure TileMBTSMonitorAlgorithm algorithm in the monitoring system.'''

    kwargs.setdefault('useTrigger', False)

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

    if kwargs['useTrigger']:
        from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
        result.merge( L1ConfigSvcCfg(flags) )

    from AthenaConfiguration.Enums import Format
    if flags.Input.Format is Format.POOL:
        kwargs.setdefault('TileDigitsContainer', 'TileDigitsFlt')

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags, 'TileMBTSMonAlgCfg')

    runNumber = flags.Input.RunNumber[0]
    isCosmics = flags.Beam.Type == 'cosmics'
    from AthenaConfiguration.ComponentFactory import CompFactory
    _TileMBTSMonitoringConfigCore(helper, CompFactory.TileMBTSMonitorAlgorithm,
                                  runNumber, isCosmics, **kwargs)

    accumalator = helper.result()
    result.merge(accumalator)
    return result


def TileMBTSMonitoringConfigOld(flags, **kwargs):

    ''' Function to configure TileMBTSMonitorAlgorithm algorithm in the old monitoring system.'''

    from AthenaMonitoring import AthMonitorCfgHelperOld
    from AthenaCommon.GlobalFlags import globalflags

    kwargs.setdefault('useTrigger', False)

    if globalflags.InputFormat().lower() == 'pool':
        kwargs.setdefault('TileDigitsContainer', 'TileDigitsFlt')

    helper = AthMonitorCfgHelperOld(flags, 'TileMBTSMonAlgCfg')

    from RecExConfig.AutoConfiguration import GetRunNumber
    runNumber = GetRunNumber()

    from AthenaCommon.BeamFlags import jobproperties
    isCosmics = jobproperties.Beam.beamType() == 'cosmics'

    from TileMonitoring.TileMonitoringConf import TileMBTSMonitorAlgorithm
    _TileMBTSMonitoringConfigCore(helper, TileMBTSMonitorAlgorithm, runNumber, isCosmics, **kwargs)

    return helper.result()


def _TileMBTSMonitoringConfigCore(helper, algConfObj, runNumber, isCosmics, **kwargs):

    ''' Function to configure TileMBTSMonitorAlgorithm algorithm in the monitoring system.'''

    run = str(runNumber)

    # Adding an TileMBTSMonitorAlgorithm algorithm to the helper
    tileMBTSMonAlg = helper.addAlgorithm(algConfObj, 'TileMBTSMonAlg')

    tileMBTSMonAlg.TriggerChain = ''

    numberOfMBTS = 32
    energyCuts = [60. / 222 for mbts in range(0, numberOfMBTS)]
    kwargs.setdefault("EnergyCuts", energyCuts)

    for k, v in kwargs.items():
        setattr(tileMBTSMonAlg, k, v)

    if 'FillHistogramsPerMBTS' in kwargs:
        fillHistogramPerMBTS = kwargs['FillHistogramsPerMBTS']
    elif hasattr(tileMBTSMonAlg,"_descriptors"): #GaudiConfig2 config
        fillHistogramPerMBTS = tileMBTSMonAlg._descriptors['FillHistogramsPerMBTS'].default
    else: #Run1/2 config
        fillHistogramPerMBTS = tileMBTSMonAlg.getDefaultProperty('FillHistogramsPerMBTS')

    useTrigger = kwargs['useTrigger']

    # 1) Configure histogram with TileMBTSMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileMBTSMonAlg, 'TileMBTSMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = 'MBTS', type='TH1F',
                                     title = 'Time for execute TileMBTSMonAlg algorithm;time [#mus]',
                                     xbins = 100, xmin = 0, xmax = 1000)

    nMBTSPerSide = numberOfMBTS // 2
    labelsMBTS  =  ['MBTSA' + ('0' if x < 10 else '') + str(x) for x in range(0, nMBTSPerSide)]
    labelsMBTS +=  ['MBTSC' + ('0' if x < 10 else '') + str(x) for x in range(0, nMBTSPerSide)]


    # 2) Configure MBTS occupancy histogram
    occupancyGroup = helper.addGroup(tileMBTSMonAlg, 'TileOccupancyMBTS', 'Tile')
    occupancyGroup.defineHistogram('HitCounter;Occupancy', path = 'MBTS/Cell', type='TH1F',
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

        nEnergyBins = 550 if isCosmics else 400
        maxEnergy = 5 if isCosmics else 80

        # 11) Configure histogram with MBTS counter energy
        energyArray = helper.addArray([numberOfMBTS], tileMBTSMonAlg, 'TileEnergyMBTS', topPath = 'Tile/MBTS')
        for postfix, tool in energyArray.Tools.items():
            mbtsCounter = int( postfix.split('_')[1] )
            mbtsName = labelsMBTS[mbtsCounter]
            title = 'Run ' + run + ': Energy of ' + mbtsName + ';Energy [pC]'
            name = 'Energy;Energy_' + mbtsName
            tool.defineHistogram(name, title = title, type = 'TH1F', path = 'Cell',
                                 xbins = nEnergyBins, xmin = -0.5, xmax = maxEnergy)

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


    if useTrigger:

        tileMBTSMonAlg.CTP_ID = [-1] # Auto configuration

        # 17) Configure MBTS occupancy histogram with TBP fired
        occupancyTrigGroup = helper.addGroup(tileMBTSMonAlg, 'TileTriggerOccupancyMBTS', 'Tile')
        occupancyTrigGroup.defineHistogram('HitCounter;Occupancy_wTBP', path = 'MBTS/Cell', type='TH1F',
                                           xlabels = labelsMBTS, title = f'Run {run}: MBTS Occupancy with TBP fired',
                                           xbins = numberOfMBTS, xmin = 0, xmax = numberOfMBTS)

        # 18) Configure summary energy histogram for MBTS with TBP fired
        energySummaryTrigGroup = helper.addGroup(tileMBTSMonAlg, 'TileTriggerEnergySummaryMBTS', 'Tile')
        energySummaryTrigGroup.defineHistogram('HitCounter,SummaryEnergy;SummaryEnergy_TBP', path = 'MBTS/Cell', type='TProfile',
                                               title = f'Run {run}: Average MBTS Energy with TBP fired;;Average Energy [pC]',
                                               xlabels = labelsMBTS, xbins = numberOfMBTS, xmin = 0, xmax = numberOfMBTS)

        # 19) Configure histogram with average MBTS time with TBP fired
        timeSummaryTrigGroup = helper.addGroup(tileMBTSMonAlg, 'TileTriggerTimeSummaryMBTS', 'Tile')
        timeSummaryTrigGroup.defineHistogram('HitCounter,SummaryTime;SummaryTime_TBP', path = 'MBTS/Cell', type='TProfile',
                                             title = f'Run {run}: Average MBTS Time with TBP fired;;Average Time [ns]',
                                             xlabels = labelsMBTS, xbins = numberOfMBTS, xmin = 0, xmax = numberOfMBTS)

        # 20) Configure histogram with average MBTS triggers multiplicities per side with TBP fired
        sides = ['A', 'C']
        for side in sides:
            multiplicityGroup = helper.addGroup(tileMBTSMonAlg, 'TileTriggerMultiplicityMBTS' + side, 'Tile')
            multiplicityTitle = f'Run {run}: Average number of MBTS triggers on {side} side;BCID;Average # of Triggers'
            multiplicityGroup.defineHistogram(f'bcid,multiplicity{side};MBTS{side}TriggersVsBCID', path = 'MBTS/Trigger',
                                              type='TProfile', title = multiplicityTitle, xbins = 3565, xmin = -0.5, xmax = 3564.5)

        # 21) Configure histogram with MBTS triggers multiplicities per side with TBP fired
        multiplicityPerSideGroup = helper.addGroup(tileMBTSMonAlg, 'TileTriggerMultiplicityPerSideMBTS', 'Tile')
        multiplicityPerSideTitle = f'Run {run}: Number of MBTS triggers per side with TBP fired'
        multiplicityPerSideTitle += ';# of Triggers on A side; # of Triggers on C side'
        multiplicityPerSideGroup.defineHistogram('multiplicityA,multiplicityC;MBTSTriggersPerSide',
                                                 path = 'MBTS/Trigger', type='TH2F', title = multiplicityPerSideTitle,
                                                 xbins = nMBTSPerSide, xmin = -0.5, xmax = nMBTSPerSide - 0.5,
                                                 ybins = nMBTSPerSide, ymin = -0.5, ymax = nMBTSPerSide - 0.5)

        # 22) Configure histogram with coincident Hits (energy) between two MBTS counters
        triggerNames = ['TBP', 'TAP', 'TAV']
        triggerDescriptions = ['before prescale', 'after prescale', 'after veto']
        triggersArray = helper.addArray([len(triggerNames)], tileMBTSMonAlg, 'MBTS_Triggers', topPath = 'Tile/MBTS')
        for postfix, tool in triggersArray.Tools.items():
            triggerIdx = int( postfix.split('_').pop() )
            trigger = triggerNames[triggerIdx]
            description = triggerDescriptions[triggerIdx]
            tool.defineHistogram(f'TriggerCounter;MBTS_Triggers_{trigger}', path = 'Trigger', type='TH1F',
                                 xlabels = labelsMBTS, title = f'Run {run}: MBTS triggers {description} ({trigger})',
                                 xbins = numberOfMBTS, xmin = 0, xmax = numberOfMBTS)

        # 23) Configure histogram with coincident Hits (energy) between two MBTS counters in CTP window
        triggersInWinArray = helper.addArray([len(triggerNames)], tileMBTSMonAlg, 'MBTS_TriggersInWindow', topPath = 'Tile/MBTS')
        for postfix, tool in triggersInWinArray.Tools.items():
            triggerIdx = int( postfix.split('_').pop() )
            trigger = triggerNames[triggerIdx]
            description = triggerDescriptions[triggerIdx]
            tool.defineHistogram(f'TriggerCounter;MBTS_TriggersInWindow_{trigger}', path = 'Trigger', type='TH1F',
                                 xlabels = labelsMBTS, title = f'Run {run}: MBTS triggers {description} ({trigger}) in CTP window',
                                 xbins = numberOfMBTS, xmin = 0, xmax = numberOfMBTS)

        # 24) Configure histogram with sum of MBTS triggers
        triggersSumLables = ['A-Inner', 'A-Outer', 'A-Total', 'C-Inner', 'C-Outer', 'C-Total']
        triggersSumArray = helper.addArray([len(triggerNames)], tileMBTSMonAlg, 'MBTS_TriggersSum', topPath = 'Tile/MBTS')
        for postfix, tool in triggersSumArray.Tools.items():
            triggerIdx = int( postfix.split('_').pop() )
            trigger = triggerNames[triggerIdx]
            description = triggerDescriptions[triggerIdx]
            tool.defineHistogram(f'TriggerType;MBTS_TriggersSum_{trigger}', path = 'Trigger', type='TH1F',
                                 xlabels = triggersSumLables, title = f'Run {run}: Sum of MBTS triggers {description} ({trigger})',
                                 xbins = len(triggersSumLables), xmin = 0, xmax = len(triggersSumLables))

        # 25) Configure histogram with coincident Hits (energy) between two MBTS counters
        coinTrigArray = helper.addArray([len(triggerNames)], tileMBTSMonAlg, 'MBTS_CoincidentTriggers', topPath = 'Tile/MBTS')
        for postfix, tool in coinTrigArray.Tools.items():
            triggerIdx = int( postfix.split('_').pop() )
            trigger = triggerNames[triggerIdx]
            tool.defineHistogram(f'CoincidentTrigger1,CoincidentTrigger2;MBTS_CoincidentTriggers_{trigger}',
                                 path = 'Trigger', type='TH2F', xlabels = labelsMBTS, ylabels = labelsMBTS,
                                 title = f'Run {run}: Coincident {trigger}s fired between two MBTS counters',
                                 xbins = numberOfMBTS, xmin = 0, xmax = numberOfMBTS,
                                 ybins = numberOfMBTS, ymin = 0, ymax = numberOfMBTS)

        # 26) Configure histogram with BCID difference between trigger and Event BCID
        deltaBCIDSummaryArray = helper.addArray([len(triggerNames)], tileMBTSMonAlg, 'MBTS_DeltaBCID_Summary', topPath = 'Tile/MBTS')
        for postfix, tool in deltaBCIDSummaryArray.Tools.items():
            triggerIdx = int( postfix.split('_').pop() )
            trigger = triggerNames[triggerIdx]
            tool.defineHistogram(f'TriggerCounter,DeltaBCID;MBTS_DeltaBCID_Summary_{trigger}', path = 'Trigger', type='TProfile',
                                 title = f'Run {run}: BCID difference of {trigger} from Event BCID;;Average BCID delta',
                                 xlabels = labelsMBTS, xbins = numberOfMBTS, xmin = 0, xmax = numberOfMBTS)

        # 27) Configure histogram with BCID difference between TBP fire and L1Accept
        deltaBCIDArray = helper.addArray([len(triggerNames), numberOfMBTS], tileMBTSMonAlg, 'MBTS_DeltaBCID', topPath = 'Tile/MBTS')
        for postfix, tool in deltaBCIDArray.Tools.items():
            elements = postfix.split('_')
            mbtsCounter = int( elements.pop() )
            triggerIdx = int( elements.pop() )
            mbtsName = labelsMBTS[mbtsCounter]
            trigger = triggerNames[triggerIdx]
            deltaBCIDTitle = f'Run {run}: {mbtsName} BCID difference of {trigger} signal'
            deltaBCIDTitle += f';Bunch crossing between {trigger} fire and L1Accept'
            tool.defineHistogram(f'DeltaBCID;MBTS_DeltaBCID_{trigger}_{mbtsName}', path = 'Trigger', type='TH1F',
                                 title = deltaBCIDTitle, xbins = 19, xmin = -9.5, xmax = 9.5)

        # 28) Configure histogram with MBTS counter energy with TBP fired
        energyTrigArray = helper.addArray([numberOfMBTS], tileMBTSMonAlg, 'TileEnergyTrigMBTS', topPath = 'Tile/MBTS')
        for postfix, tool in energyTrigArray.Tools.items():
            mbtsCounter = int( postfix.split('_')[1] )
            mbtsName = labelsMBTS[mbtsCounter]
            title = f'Run {run}: Energy with TBP fired of {mbtsName};Energy [pC]'
            name = f'Energy;Energy_TBP_{mbtsName}'
            tool.defineHistogram(name, title = title, type = 'TH1F', path = 'Cell',
                                 xbins = 550, xmin = -0.5, xmax = 5)

        # 29) Configure histogram with MBTS counter energy vs luminosity block with TBP fired
        energyTrigLBArray = helper.addArray([numberOfMBTS], tileMBTSMonAlg, 'TileEnergyTrigLBMBTS', topPath = 'Tile/MBTS')
        for postfix, tool in energyTrigLBArray.Tools.items():
            mbtsCounter = int( postfix.split('_')[1] )
            mbtsName = labelsMBTS[mbtsCounter]
            title = f'Run {run}: Energy with TBP fired of {mbtsName} per lumiblock;Lumiblocks;Energy [pC]'
            name = f'lumiBlock,Energy;EnergyLB_TBP_{mbtsName}'
            tool.defineHistogram(name, title = title, type = 'TProfile', path = 'Cell',
                                 xbins = 1000, xmin = -0.5, xmax = 999.5, opt = 'kAddBinsDynamically')

        # 30) Configure histogram with MBTS counter time with TBP
        timeTrigArray = helper.addArray([numberOfMBTS], tileMBTSMonAlg, 'TileTimeTrigMBTS', topPath = 'Tile/MBTS')
        for postfix, tool in timeTrigArray.Tools.items():
            mbtsCounter = int( postfix.split('_')[1] )
            mbtsName = labelsMBTS[mbtsCounter]
            title = f'Run {run}: Time with TBP fired of {mbtsName};Time [ns]'
            name = f'Time;Time_TBP_{mbtsName}'
            tool.defineHistogram(name, title = title, type = 'TH1F', path = 'Cell',
                                 xbins = 151, xmin = -75.5, xmax = 75.5)

        # 31) Configure histogram with efficiency of MBTS counter with TBP
        efficiencyTrigArray = helper.addArray([numberOfMBTS], tileMBTSMonAlg, 'TileEfficiencyTrigMBTS', topPath = 'Tile/MBTS')
        for postfix, tool in efficiencyTrigArray.Tools.items():
            mbtsCounter = int( postfix.split('_')[1] )
            mbtsName = labelsMBTS[mbtsCounter]
            title = f'Run {run}: Efficiency of LVL1 vs readout {mbtsName}'
            title += ';Energy [pc];Fraction of events with TBP'
            name = f'Energy,Efficiency;Efficiency_TBP_{mbtsName}'
            tool.defineHistogram(name, title = title, type = 'TProfile', path = 'Cell',
                                 xbins = 100, xmin = -0.55, xmax = 9.55)

        # 32) Configure histogram with average pulse shape of MBTS counter with TBP
        averageTrigPulseArray = helper.addArray([numberOfMBTS], tileMBTSMonAlg, 'TileAveragePulseTrigMBTS', topPath = 'Tile/MBTS')
        for postfix, tool in averageTrigPulseArray.Tools.items():
            mbtsCounter = int( postfix.split('_')[1] )
            mbtsName = labelsMBTS[mbtsCounter]
            title = f'Run {run}: Average Pulse with TBP fired for digitized signals of {mbtsName}'
            title += ';TileCal Digital Sample;Average number of ADC counts'
            name = f'SampleNumbers,Samples;AveragePulse_TBP_{mbtsName}'
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
    ConfigFlags.Exec.MaxEvents = 3
    ConfigFlags.fillFromArgs()
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

    sc = cfg.run()

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
