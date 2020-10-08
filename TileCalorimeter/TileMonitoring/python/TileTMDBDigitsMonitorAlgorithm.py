#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file TileTMDBDigitsMonitorAlgorithm.py
@author
@date
@brief Python configuration of TileTMDBDigitsMonitorAlgorithm algorithm for the Run III
'''

def TileTMDBDigitsMonitoringConfig(flags):
    '''Function to configures some algorithms in the monitoring system.'''
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    result.merge( TileCablingSvcCfg(flags) )

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags, 'TileTMDBDigitsMonitoring')


    from AthenaConfiguration.ComponentFactory import CompFactory
    tileTMDBDigitsMonAlg = helper.addAlgorithm(CompFactory.TileTMDBDigitsMonitorAlgorithm, 'TileTMDBDigitsMonAlg')

    tileTMDBDigitsMonAlg.TriggerChain = ''


    run = str(flags.Input.RunNumber[0])
    from TileMonitoring.TileMonitoringCfgHelper import addTileTMDB_1DHistogramsArray, addTileTMDB_2DHistogramsArray




    addTileTMDB_2DHistogramsArray(helper, tileTMDBDigitsMonAlg, name = 'TMDBDigitsPedestal',
                                  value = 'pedestal', title = 'Mean pedestal (sample[0])',
                                  path = 'Tile/TMDBDigits', type='TProfile2D', run = run)   
    
    addTileTMDB_2DHistogramsArray(helper, tileTMDBDigitsMonAlg, name = 'TMDBDigitsAmplitude',
                                  value = 'amplitude', title = 'Difference between maximum and minimum sample',
                                  path = 'Tile/TMDBDigits', type='TProfile2D', run = run)
                                    
    addTileTMDB_2DHistogramsArray(helper, tileTMDBDigitsMonAlg, name = 'TMDBDigitsHFN',
                                  value = 'HFN', title = 'Mean RMS of 7 samples (HFN)',
                                  path = 'Tile/TMDBDigits', type='TProfile2D', run = run)                                     
     

    addTileTMDB_1DHistogramsArray(helper, tileTMDBDigitsMonAlg, name = 'TMDBDigitsCellPedestal', 
                                    xvalue = 'pedestal', title = 'Pedestal (sample[0]);[ADC]', 
                                    path = 'Tile/TMDBDigits', type='TH1D', run = run,
                                    xbins = 101, xmin = -0.5, xmax = 100.5)

    addTileTMDB_1DHistogramsArray(helper, tileTMDBDigitsMonAlg, name = 'TMDBDigitsCellHFN',
                                    xvalue = 'HFN', title = 'Mean RMS (HFN);[ADC]', 
                                    path = 'Tile/TMDBDigits', type='TH1D', run = run,
                                    xbins = 41, xmin = -0.5, xmax = 40.5)

    addTileTMDB_1DHistogramsArray(helper, tileTMDBDigitsMonAlg, name = 'TMDBDigitsCellAmplitude', 
                                    xvalue = 'amplitude', title = 'Difference between maximum and minimum sample;[ADC]',
                                    path = 'Tile/TMDBDigits', type='TH1D', run = run,
                                    xbins = 101, xmin = -0.5, xmax = 100.5)


    accumalator = helper.result()
    result.merge(accumalator)
    return result



if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Output.HISTFileName = 'TileTMDBDigitsMonitorOutput.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False

    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    cfg = MainServicesCfg(ConfigFlags)


    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    tileTypeNames = ['TileDigitsContainer/MuRcvDigitsCnt']
    cfg.merge( ByteStreamReadCfg(ConfigFlags, type_names = tileTypeNames) )

  

    tileTMDBDigitsMonitorAccumulator  = TileTMDBDigitsMonitoringConfig(ConfigFlags)

    cfg.merge(tileTMDBDigitsMonitorAccumulator)

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    
    cfg.store( open('TileTMDBDigitsMonitorAlgorithm.pkl','wb') )

    sc = cfg.run(maxEvents=-1)

    import sys
    sys.exit(not sc.isSuccess())
