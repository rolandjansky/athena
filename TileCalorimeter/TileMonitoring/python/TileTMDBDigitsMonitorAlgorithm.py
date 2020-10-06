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

    
    ### STEP 1 ###
    # If you need to set up special tools, etc., you will need your own ComponentAccumulator;
    # uncomment the following 2 lines and use the last three lines of this function instead of the ones
    # just before
    # from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    # result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags, 'TileTMDBDigitsMonitoring')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the flags. The returned object 
    # is the algorithm.
    from AthenaConfiguration.ComponentFactory import CompFactory
    tileTMDBDigitsMonAlg = helper.addAlgorithm(CompFactory.TileTMDBDigitsMonitorAlgorithm, 'TileTMDBDigitsMonAlg')
    

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # tileTMDBDigitsMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    # tileTMDBDigitsMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'
    tileTMDBDigitsMonAlg.TriggerChain = ''

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # First, add a tool that's set up by a different configuration function. 
    # # In this case, CaloNoiseToolCfg returns its own component accumulator, 
    # # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(flags)
    # result.merge(caloNoiseAcc)
    # tileTMDBDigitsMonAlg.CaloNoiseTool = caloNoiseTool

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # tileTMDBDigitsMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    # myGroup = helper.addGroup(
    #     tileTMDBDigitsMonAlg,
    #     'ExampleMonitor',
    #     'OneRing/'
    #)

    # Add a GMT for the other example monitor algorithm
    # anotherGroup = helper.addGroup(anotherExampleMonAlg,'ExampleMonitor')


    # Configure histogram with tileTMDBDigitsMonAlg algorithm execution time



    ### STEP 5 ###
    # Configure histograms



    run = str(flags.Input.RunNumber[0])
    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile
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


    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    # return helper.result()
    
    # # Otherwise, merge with result object and return
    accumalator = helper.result()
    result.merge(accumalator)
    return result



if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO, VERBOSE
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
    # from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    # cfg.merge(PoolReadCfg(ConfigFlags))

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
    # Success should be 0
    sys.exit(not sc.isSuccess())
