#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileTMDBMonitorAlgorithm.py
@brief Python configuration of TileTMDBMonitorAlgorithm algorithm for the Run III
'''

def TileTMDBMonitoringConfig(flags, **kwargs):
    ''' Function to configure TileTMDBMonitorAlgorithm algorithm in the monitoring system.'''
    histogram_limits = {
        "PHY": {
            "Energy": [-1010, 1010],
            "Error": [-1010, 1010]
        },
        "LAS": {
            "Energy": [-1010, 15010],
            "Error": [-15010, 15010]
        },
        "BILAS": {
            "Energy": [-1010, 15010],
            "Error": [-15010, 15010]
        },
        "CIS": {
            "Energy": [-1010, 15000],
            "Error": [-15010, 15000]
        },
        "MONOCIS": {
            "Energy": [-1010, 15000],
            "Error": [-15010, 15000]
        },
        "PED": {
            "Energy": [-1010, 10000],
            "Error": [-15010, 10000]
        }
    }


    # Define one top-level monitoring algorithm. The new configuration
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    result.merge( TileCablingSvcCfg(flags) )

    from TileConditions.TileTMDBConfig import TileTMDBCondAlgCfg
    result.merge( TileTMDBCondAlgCfg(flags) )

    kwargs.setdefault('fillDetailedHistograms', False)
    #fillDetailedHistograms = kwargs['fillDetailedHistograms']

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags,'TileTMDBMonitoring')

    # Adding an TileTMDBMonitorAlgorithm algorithm to the helper
    from AthenaConfiguration.ComponentFactory import CompFactory
    tileTMDBMonAlg = helper.addAlgorithm(CompFactory.TileTMDBMonitorAlgorithm, 'TileTMDBMonAlg')

    for k, v in kwargs.items():
        setattr(tileTMDBMonAlg, k, v)

    run = str(flags.Input.RunNumber[0])

    # Configure histogram with TileTMDBMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileTMDBMonAlg, 'TileTMDBMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = f'TMDB/{flags.Tile.RunType}', type='TH1F',
                                     title = 'Time for execute TileTMDBMonAlg algorithm;time [#ms]',
                                     xbins = 100, xmin = 0, xmax = 100000)

    from TileMonitoring.TileMonitoringCfgHelper import addTileTMDB_1DHistogramsArray

    addTileTMDB_1DHistogramsArray(helper, tileTMDBMonAlg, name = 'TMDB_MeanPulse',
                                  xvalue = 'sampleNumber', value = 'sample', path = 'Tile/TMDB/MeanPulse',
                                  title = 'Mean pulse shape in TMDB;#sample;[ADC]', type = 'TProfile',
                                  run = run, xbins = 7, xmin = -0.5, xmax = 6.5, perModule = True)

    addTileTMDB_1DHistogramsArray(helper, tileTMDBMonAlg, name = 'TMDB_Energy',
                                  xvalue = 'energy', path = 'Tile/TMDB/NoiseAnalysis',
                                  title = 'Energy in TMDB;E_{TMDB} [MeV]', type = 'TH1D', run = run,
                                  xbins = 101, 
                                  xmin = histogram_limits[flags.Tile.RunType]["Energy"][0], 
                                  xmax = histogram_limits[flags.Tile.RunType]["Energy"][1], perModule = True)

    addTileTMDB_1DHistogramsArray(helper, tileTMDBMonAlg, name = 'TMDB_Peak',
                                    xvalue = 'peak', path = 'Tile/TMDB/PeakPosition',
                                    title = 'Peak Position in TMDB;E_{TMDB}', type = 'TH1D', run = run,
                                    xbins = 7, xmin = 0, xmax = 7, perModule = True)

    from TileMonitoring.TileMonitoringCfgHelper import addTileTMDB2DScatterHistogramsArray

    addTileTMDB2DScatterHistogramsArray(helper, tileTMDBMonAlg, name = 'TMDB_ChanNoise',
                                            xvalue = 'channX', yvalue = 'channY', path = 'Tile/TMDB/ChannelNoise',
                                            title = 'Channel CrossProduct TMDB;', type = 'TH2D', run = run,
                                            xbins = 101, xmin = 0, xmax = 255)

    errorTitle = 'Energy difference between TMDB and correspoding Tile Cell (D) PMT;E_{D_PMT} - E_{TMDB} [MeV]'
    addTileTMDB_1DHistogramsArray(helper, tileTMDBMonAlg, name = 'TMDB_CalibrationError',
                                  xvalue = 'error', path = 'Tile/TMDB/CalibError',
                                  title = errorTitle, type = 'TH1D', run = run,
                                  xbins = 101, 
                                  xmin = histogram_limits[flags.Tile.RunType]["Error"][0], 
                                  xmax = histogram_limits[flags.Tile.RunType]["Error"][1], 
                                  perModule = True)

    from TileMonitoring.TileMonitoringCfgHelper import addTileTMDB_2DHistogramsArray

    addTileTMDB_2DHistogramsArray(helper, tileTMDBMonAlg, name = 'TMDB_Energy',
                                  value = 'energy', title = 'Energy [MeV] in TMDB',
                                  path = 'Tile/TMDB', type='TProfile2D', run = run)

    addTileTMDB_2DHistogramsArray(helper, tileTMDBMonAlg, name = 'TMDB_PeakPosition',
                                  value = 'peakPosition', title = 'Position of peak sample in TMDB',
                                  path = 'Tile/TMDB', type='TProfile2D', run = run)

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
    ConfigFlags.Output.HISTFileName = 'TileTMDBMonitorOutput.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.Exec.MaxEvents = 3
    ConfigFlags.fillFromArgs()
    ConfigFlags.Tile.RunType = "CIS"
    ConfigFlags.lock()


    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    tileTypeNames = ['TileRawChannelContainer/TileRawChannelCnt',
                     'TileRawChannelContainer/MuRcvRawChCnt',
                     'TileDigitsContainer/MuRcvDigitsCnt']
    cfg.merge( ByteStreamReadCfg(ConfigFlags, type_names = tileTypeNames) )

    cfg.merge( TileTMDBMonitoringConfig(ConfigFlags) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileTMDBMonitorAlgorithm.pkl','wb') )

    sc = cfg.run()

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
