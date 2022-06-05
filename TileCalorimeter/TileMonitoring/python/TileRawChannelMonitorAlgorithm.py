#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileRawChannelMonitorAlgorithm.py
@brief Python configuration of TileRawChannelMonitorAlgorithm algorithm for the Run III
'''
def TileRawChannelMonitoringConfig(flags, overlapHistograms=None, **kwargs):

    ''' Function to configure TileRawChannelMonitorAlgorithm algorithm in the monitoring system.'''

    # Define one top-level monitoring algorithm. The new configuration
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    result.merge(TileCablingSvcCfg(flags))
    run3Period = (result.getService('TileCablingSvc').CablingType == 6)
    
    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    result.merge( TileDQstatusAlgCfg(flags) )

    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(TileGMCfg(flags))

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    result.merge( TileInfoLoaderCfg(flags) )
    is12bit = (result.getService('TileInfoLoader').ADCmax == 4095)

    from TileConditions.TileEMScaleConfig import TileEMScaleCondAlgCfg
    result.merge( TileEMScaleCondAlgCfg(flags) )
    
    runType = flags.Tile.RunType
    runType = runType.upper()

    kwargs.setdefault('name', 'TileRawChannelMonAlg')
    kwargs.setdefault('RunType', {'PHY' : 1, 'LAS' : 2, 'PED' : 4, 'CIS' : 8, 'MONOCIS' : 9}.get(runType, 0))

    if 'TileRawChannelContainer' not in kwargs:
        if flags.Tile.readDigits:
            if flags.Tile.doOpt2:
                rawChannelContainer = 'TileRawChannelOpt2'
            elif flags.Tile.doFit:
                rawChannelContainer = 'TileRawChannelFit'
            elif flags.Tile.doOptATLAS:
                rawChannelContainer = 'TileRawChannelFixed'
            elif flags.Tile.doOptOF1:
                rawChannelContainer = 'TileRawChannelOF1'
        else:
            # default for simulation
            rawChannelContainer = 'TileRawChannelCnt'
                
        kwargs.setdefault('TileRawChannelContainer', rawChannelContainer)

    if overlapHistograms is None:
        overlapHistograms = True if 'LAS' in runType else False

    if 'fillHistogramsForDSP' not in kwargs:
        # Don't fill DSP histograms for bi gain runs by default
        fillHistogramsForDSP = False if runType in ['CIS', 'PED'] else True
        kwargs.setdefault('fillHistogramsForDSP', fillHistogramsForDSP)
        
    if 'CalibUnit' not in kwargs:
        # Put everything in PicoCoulomb (1) by default for all run types, but Physics
        # For Physcs calibrate in CesiumPicoCoulomb (2) for all channels, but MBTS channels,
        # for which we keep the calibration in PicoCoulombCesium pC for consistency (no Cs calibration is possible)
        calibUnit = 2 if 'PHY' in runType else 1
        kwargs.setdefault('CalibUnit', calibUnit)

    if run3Period:
        demonstratorFragID = 0x10d # LBA14 is demonstrator in RUN3
        if 'FragIDsDemonstrators' not in kwargs:
            kwargs.setdefault('FragIDsDemonstrators', [demonstratorFragID])

        if 'FragIDsToIgnoreDMUErrors' not in kwargs:
            kwargs.setdefault('FragIDsToIgnoreDMUErrors', [demonstratorFragID])

    unit = {1: '[pC]', 2 : '[Cesium pC], but MBTS [pc]'}.get(kwargs['CalibUnit'], "")
        
    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags,'TileRawChannelMonitoring')

    # Adding an TileCellMonitorAlgorithm algorithm to the helper
    from AthenaConfiguration.ComponentFactory import CompFactory
    tileRawChannelMonAlg = helper.addAlgorithm(CompFactory.TileRawChannelMonitorAlgorithm, kwargs['name'])

    for k, v in kwargs.items():
        setattr(tileRawChannelMonAlg, k, v)

    run = str(flags.Input.RunNumber[0])

    # 1) Configure histogram with TileRawChannelMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileRawChannelMonAlg, 'TileRawChannelMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = 'RawChannel', type='TH1F',
                                     title = 'Time for execute TileRawChannelMonAlg algorithm;time [#mus]',
                                     xbins = 200, xmin = 0, xmax = 200000)


    from TileMonitoring.TileMonitoringCfgHelper import addTileChannelHistogramsArray

    if kwargs['fillHistogramsForDSP']:
        # Configure histograms with Tile DSP raw channel amplitude per module, channel and gain
        addTileChannelHistogramsArray(helper, tileRawChannelMonAlg, name='TileRawChannelDspAmp',
                                      title=f'DSP Amplitude;{unit}', path='Tile/RawChannel', type='TH1I', run=run,
                                      xvalue='dsp_amp', xbins=[491,413], xmin=[-50.5,-1.01], xmax=[1049.34,15.51])

        # Configure histograms with Tile DSP raw channel time per module, channel and gain
        addTileChannelHistogramsArray(helper, tileRawChannelMonAlg, name='TileRawChannelDspTime',
                                        title='DSP Time;[ns]', path='Tile/RawChannel', type='TH1I',
                                        xvalue='dsp_time', xbins=101, xmin=-100.5, xmax= 100.5, run=run)

        # Configure histograms with Tile DSP raw channel amplitude difference per module, channel and gain
        addTileChannelHistogramsArray(helper, tileRawChannelMonAlg, name='TileRawChannelAmpDiff',
                                      title=f'DSP-OF Amplitude difference;{unit}', path='Tile/RawChannel', type='TH1I',
                                      run=run, xvalue='dsp-fit_amp_diff', xbins=404, xmin=-1.01, xmax=1.01)

        # Configure histograms with Tile DSP raw channel time difference per module, channel and gain
        addTileChannelHistogramsArray(helper, tileRawChannelMonAlg, name='TileRawChannelTimeDiff',
                                      title='DSP-OF Time difference;[ns]', path='Tile/RawChannel', type='TH1I',
                                      xvalue='dsp-fit_time_diff', xbins=101, xmin=-2.02, xmax=2.02, run=run)
    
        # Configure histograms with Tile DSP raw channel quality per module, channel and gain
        addTileChannelHistogramsArray(helper, tileRawChannelMonAlg, name='TileRawChannelDspChi2',
                                      title='DSP #chi^{2}', path='Tile/RawChannel', type='TH1I',
                                      xvalue='dsp_chi2', xbins=16, xmin=-0.5, xmax=15.5, run=run)

        addTileChannelHistogramsArray(helper, tileRawChannelMonAlg, name='TileRawChannelDspChi2VsAmp', type='TH2F',
                                      title=f'DSP #chi^{2} vs Amplitude;{unit}', path='Tile/RawChannel', aliasSuffix='chi2_amp',
                                      xvalue='dsp_amp', xbins=[200,150], xmin=[-45.1,-7.0], xmax=[855.1,12.0],
                                      yvalue='dsp_chi2', ybins=16, ymin=-0.5, ymax=15.5, run=run)

    if 'CIS' not in runType:

        # Configure histograms with Tile raw channel amplitude per module, channel and gain
        if 'PED' in runType:
            ampXbins = [101, 101]
            ampXmin = [-10.1, -0.404]
            ampXmax = [10.1, 0.404]
        elif 'PHY' in runType:
            ampXbins = 206
            ampXmin = -0.55
            ampXmax = 20.05
        elif overlapHistograms:
            lgXbins = [-50.5 + 1.0 * i for i in range(0, 50) ]
            lgXbins += [-1.5 + 0.05 * i for i in range(1, 501)]
            lgXbins += [23.5 + 1.0 * i for i in range(1, 1028)]
            ampXbins = [lgXbins, 500]
            ampXmin = [lgXbins[0], -1.5]
            ampXmax = [lgXbins[len(lgXbins) - 1], 23.5]
        else:
            ampXbins = [1101, 826]
            ampXmin = [-50.5, -1.01]
            ampXmax = [1050.5, 15.51]

        addTileChannelHistogramsArray(helper, tileRawChannelMonAlg, name='TileRawChannelAmp',
                                      title=f'Amplitude;{unit}', path='Tile/RawChannel', type='TH1I',
                                      xvalue='amp', xbins=ampXbins, xmin=ampXmin, xmax=ampXmax, run=run)

        # Configure histograms with Tile raw channel time per module, channel and gain
        addTileChannelHistogramsArray(helper, tileRawChannelMonAlg, name='TileRawChannelTime',
                                      title='Time;[ns]', path='Tile/RawChannel', type='TH1I',
                                      xvalue='time', xbins=201, xmin=-100.5, xmax= 100.5, run=run)
                    
        # Configure histograms with Tile raw channel time corrected per module, channel and gain
        addTileChannelHistogramsArray(helper, tileRawChannelMonAlg, name='TileRawChannelTimeCorr',
                                      title='Time corrected;[ns]', path='Tile/RawChannel', type='TH1I',
                                      xvalue='time_corr', xbins=201, xmin=-100.5, xmax= 100.5, run=run)

    else: # CIS run

        chargeFactor = 2. if is12bit else 1.
        adcFactor    = 4. if is12bit else 1.
        
        chargeXmin = { 5 : [-0.5, -0.0625 * chargeFactor], 100 : [-4., -0.0625 * chargeFactor]}
        chargeXmax = { 5 : [50.5, 12.5625 * chargeFactor], 100 : [804., 12.5625 * chargeFactor]}

        if kwargs['CalibUnit'] == 2: # Cesium pC
            ampYmin = { 5 : [-5.3125, -0.391 * adcFactor], 100 : [-25., -0.391 * adcFactor]}
            ampYmax = { 5 : [60.3125,  16.02 * adcFactor], 100 : [1025., 16.02 * adcFactor]}
        else:
            ampYmin = { 5 : [-5.3125,  -25. * adcFactor], 100 : [-25.,   -25. * adcFactor]}
            ampYmax = { 5 : [60.3125, 1025. * adcFactor], 100 : [1025., 1025. * adcFactor]}

        for capacitor in [5, 100]:
            # Configure histograms with Tile raw channel amplitude vs charge per module, channel and gain
            addTileChannelHistogramsArray(helper, tileRawChannelMonAlg, name=f'TileRawChannelAmpVsQ{capacitor}',
                                          title=f'Amplitude vs Charge for {capacitor} pF;Charge, [pC];Amplitude, {unit}',
                                          path='Tile/RawChannel',type='TH2F', run=run,
                                          aliasSuffix=f'amp_vs_q_{capacitor}', xvalue='charge', yvalue='amp',
                                          xbins=51, xmin=chargeXmin[capacitor], xmax=chargeXmax[capacitor],
                                          ybins=160, ymin=ampYmin[capacitor], ymax=ampYmax[capacitor])

            # Configure histograms with Tile raw channel time vs injection time per module, channel and gain
            addTileChannelHistogramsArray(helper, tileRawChannelMonAlg, name=f'TileRawChannelTimeVsTime{capacitor}',
                                          title=f'Time vs Injection Time for {capacitor} pF;Injection Time, [ns];Time, [ns]',
                                          path='Tile/RawChannel', type='TH2F', run=run,
                                          xvalue='inj_time', yvalue='time', aliasSuffix=f'time_vs_time_{capacitor}',
                                          xbins=51, xmin=-0.25, xmax=25.25, ybins=160, ymin=-64., ymax=32.)
            
            # Configure histograms with Tile raw channel amplitude over charge per module, channel and gain
            addTileChannelHistogramsArray(helper, tileRawChannelMonAlg, name=f'TileRawChannelAmpOverQ{capacitor}',
                                          title=f'Amplitude over charge ratio for {capacitor} pF;{unit}/[pC]', path='Tile/RawChannel',
                                          type='TH1I', xvalue='amp_ratio', run=run, aliasSuffix=f'amp_ratio_{capacitor}',
                                          xbins=101, xmin=-0.01, xmax=2.01)

            # Configure histograms with Tile raw channel time per module, channel and gain
            addTileChannelHistogramsArray(helper, tileRawChannelMonAlg, name=f'TileRawChannelTime{capacitor}', run=run,
                                          title=f'Time for {capacitor};[ns]', path='Tile/RawChannel', type='TH1I',
                                          xvalue='time', xbins=101, xmin=-101, xmax=101, aliasSuffix=f'time_{capacitor}')


    accumalator = helper.result()
    result.merge(accumalator)
    return result

if __name__=='__main__':

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Output.HISTFileName = 'TileRawChannelMonitorOutput.root'
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

    cfg.merge( TileRawChannelMonitoringConfig(ConfigFlags,
                                              TileRawChannelContainer='TileRawChannelCnt',
                                              fillHistogramsForDSP=True) )

    ConfigFlags.dump()

    cfg.store( open('TileRawChannelMonitorAlgorithm.pkl','wb') )

    sc = cfg.run()

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
