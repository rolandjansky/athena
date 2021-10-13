#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileDigitsMonitorAlgorithm.py
@brief Python configuration of TileDigitsMonitorAlgorithm algorithm for the Run III
'''
def TileDigitsMonitoringConfig(flags, **kwargs):

    ''' Function to configure TileDigitsMonitorAlgorithm algorithm in the monitoring system.'''

    # Define one top-level monitoring algorithm. The new configuration
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    result.merge( TileDQstatusAlgCfg(flags) )

    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(TileGMCfg(flags))

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    result.merge(TileInfoLoaderCfg(flags))

    runType = flags.Tile.RunType
    runType = runType.upper()

    kwargs.setdefault('name', 'TileDigitsMonAlg')
    kwargs.setdefault('RunType', {'PHY' : 1, 'LAS' : 2, 'PED' : 4, 'CIS' : 8, 'MONOCIS' : 9}.get(runType, 0))

    kwargs.setdefault('fillErrorsHistograms', True)
    kwargs.setdefault('fillHighFrequencyNoiseHistograms', True)
    kwargs.setdefault('fillEventModule32Histograms', True)
    kwargs.setdefault('fillSamplesHistograms', True)
    kwargs.setdefault('fillProfileHistograms', True)
    kwargs.setdefault('fillPedestalHistograms', True)
    kwargs.setdefault('fillCorrelationsHistograms', True if runType == 'PED' else False)
    kwargs.setdefault('TileRawChannelContainer', 'TileRawChannelCnt')

    kwargs.setdefault('fillPedestalDifference', True)
    if kwargs['fillPedestalDifference'] and 'TileCondToolNoiseSample' not in kwargs:
        from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
        sampleNoiseTool = result.popToolsAndMerge( TileCondToolNoiseSampleCfg(flags) )
        kwargs['TileCondToolNoiseSample'] = sampleNoiseTool

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags,'TileDigitsMonitoring')

    # Adding an TileCellMonitorAlgorithm algorithm to the helper
    from AthenaConfiguration.ComponentFactory import CompFactory
    tileDigitsMonAlg = helper.addAlgorithm(CompFactory.TileDigitsMonitorAlgorithm, kwargs['name'])

    for k, v in kwargs.items():
        setattr(tileDigitsMonAlg, k, v)

    run = str(flags.Input.RunNumber[0])

    # 1) Configure histogram with TileDigiNoiseMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileDigitsMonAlg, 'TileDigitsMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = 'Digits', type='TH1F',
                                     title = 'Time for execute TileDigitsMonAlg algorithm;time [#mus]',
                                     xbins = 200, xmin = 0, xmax = 200000)


    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile


    def addTileChannel1DHistogramsArray(helper, algorithm, name, title, path,
                                         xvalue, xbins, xmin, xmax, type = 'TH1D',
                                         run = '', value = '', aliasSuffix = ''):
        '''
        This function configures 1D histograms with Tile monitored value per module, channel, gain.

        Arguments:
             helper    -- Helper
             algorithm -- Monitoring algorithm
             name      -- Name of histogram, actual name is constructed dynamicaly like:
                          name + mudule + channel + gain
             title     -- Title of histogram, actual title is constructed dynamicaly like:
                             run + module + channel + gain + title
             path      -- Path in file for histogram (relative to the path of given group)
             xvalue    -- Name of monitored value for x axis
             type      -- Type of histogram (TH1D, TProfile)
             value     -- Name of monitored value (needed for TProfile)
             run       -- Run number (given it will be put into the title)
             xlabels    -- List of bin labels
        '''

        dimensions = [int(Tile.MAX_ROS) - 1, int(Tile.MAX_DRAWER)]
        array = helper.addArray(dimensions, algorithm, name, topPath = path)

        for postfix, tool in array.Tools.items():
            ros, module = [int(x) for x in postfix.split('_')[1:]]
            moduleName = Tile.getDrawerString(ros + 1, module)
            fullPath = moduleName

            for channel in range(0, int(Tile.MAX_CHAN)):
                channelName = f'0{channel}' if channel < 10 else str(channel)
                for gain in range(0, Tile.MAX_GAIN):
                    gainName = {0 : 'low', 1 : 'high'}.get(gain)
                    nameSuffix = aliasSuffix if aliasSuffix else xvalue
                    fullName = f'{xvalue}_{channel}_{gain}'
                    fullName += f',{value}_{channel}_{gain};' if 'Profile' in type else ';'
                    fullName += f'{moduleName}_ch_{channelName}_{gainName[:2]}_{nameSuffix}'
                    fullTitle = f'Run {run} {moduleName} Channel {channelName} {gainName} gain: {title}'

                    tool.defineHistogram(fullName, title = fullTitle, path = fullPath, type = type,
                                         xbins = xbins, xmin = xmin, xmax = xmax)

    if kwargs['fillErrorsHistograms']:

        # Configure histogram with overflows in high gain
        overflowGroup = helper.addGroup(tileDigitsMonAlg, 'TileDigitsOverflow', 'Tile/')
        overflowGroup.defineHistogram('charge;OverflowInHighGain', path = 'Digits/Summary', type='TH1D',
                                      title = f'Run {run}: Injected charge with overflow in high gain;charge [pC]',
                                      xbins = 10010, xmin = -0.5, xmax = 1000.5)


        # Configure histograms with all Tile DMU Global CRC errors
        crcGlobalDimensions = [int(Tile.MAX_ROS) - 1, int(Tile.MAX_DRAWER)]
        crcGlobalArray = helper.addArray(crcGlobalDimensions, tileDigitsMonAlg, 'TileDigitsGlobalCRC', topPath = 'Tile/Digits')

        for postfix, tool in crcGlobalArray.Tools.items():
            ros, module = [int(x) for x in postfix.split('_')[1:]]
            moduleName = Tile.getDrawerString(ros + 1, module)

            fullName = f'CRC_global;{moduleName}_CRC_global'
            fullTitle = f'Run {run} {moduleName}: Global CRC errors'

            tool.defineHistogram(fullName, title = fullTitle, path = 'Summary', type = 'TH1I',
                                 xbins = 3, xmin = -0.5, xmax = 2.5)


        # Configure histograms with Tile DMU errors
        maxDMUs = 16
        dmuLabels = [str(dmu) for dmu in range(0, maxDMUs)]

        dmuErrorLabels = ['OK', 'Format', 'Parity', 'Memory', 'SingleStrobe', 'DoubleStrobe', 'DummyFrag', 'NoDataFrag']
        maxErrors = len(dmuErrorLabels)

        errorsArray = helper.addArray([int(Tile.MAX_ROS - 1), int(Tile.MAX_DRAWER), int(Tile.MAX_GAIN)],
                                      tileDigitsMonAlg, 'TileDigitsErrors', topPath = 'Tile/')
        for postfix, tool in errorsArray.Tools.items():
            ros, module, gain = [int(x) for x in postfix.split('_')[1:]]
            gainName = {0 : 'low', 1 : 'high'}.get(gain)
            moduleName = Tile.getDrawerString(ros + 1, module)

            fullTitle = f'Run {run} {moduleName}: DMU Header {gainName} gain errors;DMU'
            fullName = f'DMU,Error;{moduleName}_DMU_head__{gainName[:2]}_err'
            fullPath = f'Digits/{moduleName}'

            tool.defineHistogram(fullName, title = fullTitle, type = 'TH2F', path = fullPath,
                                 xlabels = dmuLabels, ylabels = dmuErrorLabels,
                                 xbins = maxDMUs, xmin = 0.0, xmax = maxDMUs,
                                 ybins = maxErrors, ymin = 0.0, ymax = maxErrors)


        # Configure histograms with all Tile DMU CRC errors
        crcErrDimensions = [int(Tile.MAX_ROS) - 1, int(Tile.MAX_DRAWER)]
        crcErrArray = helper.addArray(crcErrDimensions, tileDigitsMonAlg, 'TileDigitsCRCErrors', topPath = 'Tile/Digits')

        for postfix, tool in crcErrArray.Tools.items():
            ros, module = [int(x) for x in postfix.split('_')[1:]]
            moduleName = Tile.getDrawerString(ros + 1, module)
            fullPath = moduleName

            for dmu in range(0, maxDMUs):
                dmuName = f'0{dmu}' if dmu < 10 else str(dmu)
                fullName = f'CRC_err_{dmu};{moduleName}_dmu_{dmuName}_CRC_err'
                fullTitle = f'Run {run} {moduleName} DMU {dmuName}: CRC errors'

                tool.defineHistogram(fullName, title = fullTitle, path = fullPath, type = 'TH1I',
                                     xbins = 5, xmin = -0.5, xmax = 4.5)


        # Configure histograms with all Tile DMU BCID
        bcidDimensions = [int(Tile.MAX_ROS) - 1, int(Tile.MAX_DRAWER), int(Tile.MAX_GAIN)]
        bcidArray = helper.addArray(bcidDimensions, tileDigitsMonAlg, 'TileDigitsBCID', topPath = 'Tile/Digits')

        for postfix, tool in bcidArray.Tools.items():
            ros, module, gain = [int(x) for x in postfix.split('_')[1:]]
            moduleName = Tile.getDrawerString(ros + 1, module)
            gainName = {0 : 'low', 1 : 'high'}.get(gain)
            fullPath = moduleName

            for dmu in range(0,maxDMUs):
                dmuName = f'0{dmu}' if dmu < 10 else str(dmu)
                fullName = f'BCID_{dmu};{moduleName}_dmu_{dmuName}_{gainName[:2]}_BCID'
                fullTitle = f'Run {run} {moduleName} DMU {dmuName} {gainName} gain: BCID'

                tool.defineHistogram(fullName, title = fullTitle, path = fullPath, type = 'TH1I',
                                     xbins = 100, xmin = 2050.5, xmax = 2150.5)


        # Configure histograms with all Tile DMU BCID Errors
        bcidErrDimensions = [int(Tile.MAX_ROS) - 1, int(Tile.MAX_DRAWER), int(Tile.MAX_GAIN)]
        bcidErrArray = helper.addArray(bcidErrDimensions, tileDigitsMonAlg, 'TileDigitsBCIDErrors', topPath = 'Tile/Digits')

        for postfix, tool in bcidErrArray.Tools.items():
            ros, module, gain = [int(x) for x in postfix.split('_')[1:]]
            moduleName = Tile.getDrawerString(ros + 1, module)
            gainName = {0 : 'low', 1 : 'high'}.get(gain)
            fullPath = moduleName

            for dmu in range(0, maxDMUs):
                dmuName = f'0{dmu}' if dmu < 10 else str(dmu)
                fullName = f'BCID_err_{dmu};{moduleName}_dmu_{dmuName}_{gainName[:2]}_BCID_err'
                fullTitle = f'Run {run} {moduleName} DMU {dmuName} {gainName} gain: BCID errors'

                tool.defineHistogram(fullName, title = fullTitle, path = fullPath, type = 'TH1I',
                                     xbins = 3, xmin = -0.5, xmax = 2.5)


    if kwargs['fillCorrelationsHistograms']:
        # Configure histograms with average sample in Tile channels per module and gain
        meanSampleArray = helper.addArray([2], tileDigitsMonAlg, 'TileDigitsMeanSample', topPath = 'Tile/Digits')
        for postfix, tool in meanSampleArray.Tools.items():
            gain = int(postfix.split('_').pop())
            gainName = {0 : 'low', 1 : 'high'}.get(gain)
            for ros in range(1, Tile.MAX_ROS):
                for module in range(0, Tile.MAX_DRAWER):
                    moduleName = Tile.getDrawerString(ros, module)

                    fullName = f'channel_{moduleName},meanSample_{moduleName};{moduleName}_{gainName[:2]}_mean_sample'
                    fullTitle = f'Run {run} {moduleName} {gainName} gain: Average sample'

                    tool.defineHistogram(fullName, title = fullTitle, path = moduleName, type = 'TProfile',
                                         xbins = Tile.MAX_CHAN, xmin = -0.5, xmax = Tile.MAX_CHAN - 0.5)

        # Configure histograms with average sum of samples in Tile channels per module and gain
        meanSampleProdArray = helper.addArray([2], tileDigitsMonAlg, 'TileDigitsMeanSampleProd', topPath = 'Tile/Digits')
        for postfix, tool in meanSampleProdArray.Tools.items():
            gain = int(postfix.split('_').pop())
            gainName = {0 : 'low', 1 : 'high'}.get(gain)
            for ros in range(1, Tile.MAX_ROS):
                for module in range(0, Tile.MAX_DRAWER):
                    moduleName = Tile.getDrawerString(ros, module)

                    fullName = f'channel1_{moduleName},channel2_{moduleName},meanSampleProd_{moduleName};'
                    fullName += f'{moduleName}_{gainName[:2]}_mean_sample_prod'
                    fullTitle = f'Run {run} {moduleName} {gainName} gain: Average samples product'

                    tool.defineHistogram(fullName, title = fullTitle, path = moduleName, type = 'TProfile2D',
                                         xbins = Tile.MAX_CHAN, xmin = -0.5, xmax = Tile.MAX_CHAN - 0.5,
                                         ybins = Tile.MAX_CHAN, ymin = -0.5, ymax = Tile.MAX_CHAN - 0.5)


    if kwargs['fillPedestalHistograms']:
        # Configure histograms with Tile channel pedestals per module, channel and gain
        addTileChannel1DHistogramsArray(helper, tileDigitsMonAlg, name = 'TileDigitsSample0',
                                        title = 'Pedestal, sample[0]', path = 'Tile/Digits', type = 'TH1I',
                                        xvalue = 'sample0', xbins = 151, xmin = -0.5, xmax = 150.5, run = run)

    if kwargs['fillHighFrequencyNoiseHistograms']:
        # Configure histograms with Tile channel HFN per module, channel and gain
        addTileChannel1DHistogramsArray(helper, tileDigitsMonAlg, name = 'TileDigitsMeanRMS',
                                        title = 'Mean RMS in event', path = 'Tile/Digits', type = 'TH1F',
                                        xvalue = 'meanRMS', xbins = 101, xmin = -0.05, xmax = 10.5, run = run)

    if kwargs['fillSamplesHistograms']:
        # Configure histograms with all Tile channel samples per module, channel and gain
        addTileChannel1DHistogramsArray(helper, tileDigitsMonAlg, name = 'TileDigitsSamples',
                                        title = 'All samples', path = 'Tile/Digits', type = 'TH1I',
                                        xvalue = 'samples', xbins = 1025, xmin = -0.5, xmax = 1024.5, run = run)

    if kwargs['fillProfileHistograms']:
        # Configure histograms with Tile channel average profile per module, channel and gain
        addTileChannel1DHistogramsArray(helper, tileDigitsMonAlg, name = 'TileDigitsProfile',
                                        title = 'Average profile', path = 'Tile/Digits', type = 'TProfile',
                                        xvalue = 'sampleNumbers', xbins = 7, xmin = -0.5, xmax = 6.5,
                                        run = run, value = 'samples', aliasSuffix = 'prof')

    if kwargs['fillEventModule32Histograms']:
        # Configure histograms with all Tile channel samples per module, channel and gain
        addTileChannel1DHistogramsArray(helper, tileDigitsMonAlg, name = 'TileDigitsEvtMod32',
                                        title = 'Event number % 32', path = 'Tile/Digits', type = 'TH1I',
                                        xvalue = 'evtn_mod32', xbins = 32, xmin = -0.5, xmax = 31.5, run = run)

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
    ConfigFlags.Output.HISTFileName = 'TileDigitsMonitorOutput2.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.Exec.MaxEvents = 3
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    tileTypeNames = ['TileRawChannelContainer/TileRawChannelCnt',
                     'TileBeamElemContainer/TileBeamElemCnt',
                     'TileDigitsContainer/TileDigitsCnt']
    cfg.merge( ByteStreamReadCfg(ConfigFlags, type_names = tileTypeNames) )
    cfg.getService('ByteStreamCnvSvc').ROD2ROBmap = [ "-1" ]

    runNumber = ConfigFlags.Input.RunNumber[0]
    from AthenaConfiguration.ComponentFactory import CompFactory
    cfg.addPublicTool( CompFactory.TileROD_Decoder(fullTileMode = runNumber) )

    cfg.merge( TileDigitsMonitoringConfig(ConfigFlags, name = 'TileDigitsMonAlg',
                                          fillPedestalDifference = False) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileDigitsMonitorAlgorithm.pkl','wb') )

    sc = cfg.run()

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
