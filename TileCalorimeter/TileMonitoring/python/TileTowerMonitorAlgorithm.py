#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileTowerMonitorAlgorithm.py
@brief Python configuration of TileTowerMonitorAlgorithm algorithm for the Run III
'''
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileTowerMonitoringConfig(flags, **kwargs):

    ''' Function to configure TileTowerMonitorAlgorithm algorithm in the monitoring system.'''

    # Define one top-level monitoring algorithm. The new configuration
    # framework uses a component accumulator.

    result = ComponentAccumulator()

    from TileMonitoring.TileTowerBuilderConfig import TileTowerBuilderAlgCfg
    result.merge(TileTowerBuilderAlgCfg(flags))

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags,'TileTowerMonitoring')

    # Adding an TileTowerMonitorAlgorithm algorithm to the helper
    TileTowerMonitorAlgorithm=CompFactory.TileTowerMonitorAlgorithm
    tileTowerMonAlg = helper.addAlgorithm(TileTowerMonitorAlgorithm, 'TileTowerMonAlg')

    tileTowerMonAlg.TriggerChain = ''

    # L1Trigger Type Bits:
    #     bit0_RNDM, bit1_ZeroBias, bit2_L1Cal, bit3_Muon,
    #     bit4_RPC, bit5_FTK, bit6_CTP, bit7_Calib, AnyPhysTrig
    kwargs.setdefault('fillHistogramsForL1Triggers', ['AnyPhysTrig', 'bit7_Calib'])
    l1Triggers = kwargs['fillHistogramsForL1Triggers']

    for k, v in kwargs.items():
        setattr(tileTowerMonAlg, k, v)

    run = str(flags.Input.RunNumber[0])

    # 1) Configure histogram with TileTowerMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileTowerMonAlg, 'TileTowerMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = 'Tower', type='TH1F',
                                     title = 'Time for execute TileTowerMonAlg algorithm;time [#mus]',
                                     xbins = 100, xmin = 0, xmax = 100000)


    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile
    from TileMonitoring.TileMonitoringCfgHelper import addTileEtaPhiMapsArray

    # ) Configure histograms with most energetic Tile tower position
    addTileEtaPhiMapsArray(helper, tileTowerMonAlg, name = 'TileTowerEtaPhi', type='TH2D',
                           title = 'Most energetic Tile Tower position', path = 'Tile/Tower',
                           run = run, triggers = l1Triggers, perSample = False)

    # ) Configure histograms with position correlation of Tile tower opposite to most energetic tower
    addTileEtaPhiMapsArray(helper, tileTowerMonAlg, name = 'TileTowerEtaPhiDiff', type='TH2D',
                           title = 'Position correlation of Tile Tower opposite to most energetic tower',
                           path = 'Tile/Tower', run = run, triggers = l1Triggers, perSample = False,
                           etaTitle = '#Delta #eta', etabins = 41, etamin = -2.05, etamax = 2.05,
                           phiTitle = '#Delta #phi', phibins = Tile.MAX_DRAWER + 1, phimin = -0.05, phimax = 6.45)


    from TileMonitoring.TileMonitoringCfgHelper import addTile1DHistogramsArray

    # ) Configure histograms with Et in most energetic Tile tower per partition
    addTile1DHistogramsArray(helper, tileTowerMonAlg, name = 'TileTowerEt', xvalue = 'Et',
                             title = 'E_{T} [MeV] in most energetic Tile Tower', path = 'Tile/Tower',
                             xbins = 80, xmin = 0., xmax = 20000., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = True, perSample = False,
                             perGain = False, subDirectory = True, allPartitions = True)

    # ) Configure histograms with all Tile towers energy per partition
    addTile1DHistogramsArray(helper, tileTowerMonAlg, name = 'TileAllTowerEnergy', xvalue = 'energy',
                             title = 'All Tile Towers Energy [MeV]', path = 'Tile/Tower',
                             xbins = 80, xmin = 0., xmax = 20000., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = True, perSample = False,
                             perGain = False, subDirectory = True, allPartitions = True)


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
    ConfigFlags.Input.Files = defaultTestFiles.ESD
    ConfigFlags.Output.HISTFileName = 'TileTowerMonitorOutput.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    l1Triggers = ['bit0_RNDM', 'bit1_ZeroBias', 'bit2_L1Cal', 'bit3_Muon',
                  'bit4_RPC', 'bit5_FTK', 'bit6_CTP', 'bit7_Calib', 'AnyPhysTrig']

    cfg.merge( TileTowerMonitoringConfig(ConfigFlags,
                                         fillHistogramsForL1Triggers = l1Triggers) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileTowerMonitorAlgorithm.pkl','wb') )

    sc = cfg.run(maxEvents=-1)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
