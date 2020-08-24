#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileMuonFitMonitorAlgorithm.py
@brief Python configuration of TileMuonFitMonitorAlgorithm algorithm for the Run III
'''
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileMuonFitMonitoringConfig(flags, **kwargs):

    ''' Function to configure TileMuonFitMonitorAlgorithm algorithm in the monitoring system.'''

    # Define one top-level monitoring algorithm. The new configuration
    # framework uses a component accumulator.

    result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags,'TileMuonFitMonitoring')

    # Adding an TileMuonFitMonitorAlgorithm algorithm to the helper
    TileMuonFitMonitorAlgorithm=CompFactory.TileMuonFitMonitorAlgorithm
    tileMuonFitMonAlg = helper.addAlgorithm(TileMuonFitMonitorAlgorithm, 'TileMuonFitMonAlg')

    tileMuonFitMonAlg.TriggerChain = ''

    # L1Trigger Type Bits:
    #     bit0_RNDM, bit1_ZeroBias, bit2_L1Cal, bit3_Muon,
    #     bit4_RPC, bit5_FTK, bit6_CTP, bit7_Calib, AnyPhysTrig
    kwargs.setdefault('fillHistogramsForL1Triggers', ['AnyPhysTrig', 'bit7_Calib'])
    l1Triggers = kwargs['fillHistogramsForL1Triggers']

    for k, v in kwargs.items():
        setattr(tileMuonFitMonAlg, k, v)

    run = str(flags.Input.RunNumber[0])

    # 1) Configure histogram with TileMuonFitMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileMuonFitMonAlg, 'TileMuonFitMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = 'MuonFit', type='TH1F',
                                     title = 'Time for execute TileMuonFitMonAlg algorithm;time [#mus]',
                                     xbins = 100, xmin = 0, xmax = 10000)


    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile
    from TileMonitoring.TileMonitoringCfgHelper import addTile2DHistogramsArray

    # 2) Configure histograms with Tile MuonFit position at y=0
    addTile2DHistogramsArray(helper, tileMuonFitMonAlg, name = 'TileMuonFitPosition',
                             xvalue = 'zPosition', yvalue = 'xPosition',
                             title = 'Tile MuonFit position at y=0;z [mm];x [mm]', path = 'Tile/MuonFit',
                             xbins = 69, xmin = -6900, xmax = 6900., ybins = 44, ymin = -4400., ymax = 4400.,
                             type='TH2D', run = run, triggers = l1Triggers)

    # 3) Configure histograms with Tile MuonFit sinus of the angle in the y-z plane vs position
    addTile2DHistogramsArray(helper, tileMuonFitMonAlg, name = 'TileMuonFitPosDirectionTheta',
                             xvalue = 'zPosition', yvalue = 'theta', path = 'Tile/MuonFit',
                             title = 'Tile MuonFit vertical axis component;z [mm];sin(#theta)',
                             xbins = 49, xmin = -4900, xmax = 4900., ybins = 50, ymin = -0., ymax = 1.,
                             type='TH2D', run = run, triggers = l1Triggers)

    from TileMonitoring.TileMonitoringCfgHelper import addTile1DHistogramsArray

    # 4) Configure histograms with number of Tile MuonFits reconstructed
    addTile1DHistogramsArray(helper, tileMuonFitMonAlg, name = 'TileMuonFitNum', xvalue = 'nMuons',
                             title = 'Number of Tile MuonFit reconstructed', path = 'Tile/MuonFit',
                             xbins = 10, xmin = -0.5, xmax = 9.5, type = 'TH1F', run = run,
                             triggers = l1Triggers, perPartition = False)

    # 5) Configure histograms with number of Tile cells per MuonFit track
    addTile1DHistogramsArray(helper, tileMuonFitMonAlg, name = 'TileMuonFitNCells', xvalue = 'nCells',
                             title = 'Number of Tile cells per MuonFit track', path = 'Tile/MuonFit',
                             xbins = 10, xmin = -0.5, xmax = 9.5, type = 'TH1F', run = run,
                             triggers = l1Triggers, perPartition = False)

    # 6) Configure histograms with Tile MuonFit total energy
    addTile1DHistogramsArray(helper, tileMuonFitMonAlg, name = 'TileMuonFitEnergy', xvalue = 'energy',
                             title = 'Tile MuonFit total energy;Muon energy [MeV]', path = 'Tile/MuonFit',
                             xbins = 50, xmin = 0., xmax = 10000., type = 'TH1F', run = run,
                             triggers = l1Triggers, perPartition = False)

    # 7) Configure histograms with Tile MuonFit time at y=0 plane
    addTile1DHistogramsArray(helper, tileMuonFitMonAlg, name = 'TileMuonFitTime', xvalue = 'time',
                             title = 'Tile MuonFit time at y=0 plane;Muon time [ns]', path = 'Tile/MuonFit',
                             xbins = 49, xmin = -73.5, xmax = 73.5, type = 'TH1F', run = run,
                             triggers = l1Triggers, perPartition = False)

    # 8) Configure histograms with Tile MuonFit path
    addTile1DHistogramsArray(helper, tileMuonFitMonAlg, name = 'TileMuonFitPath', xvalue = 'path',
                             title = 'Tile MuonFit total path length;Muon path length [mm]', path = 'Tile/MuonFit',
                             xbins = 50, xmin = 0., xmax = 10000., type = 'TH1F', run = run,
                             triggers = l1Triggers, perPartition = False)

    # 9) Configure histograms with Tile MuonFit energy density
    addTile1DHistogramsArray(helper, tileMuonFitMonAlg, name = 'TileMuonFitEneDensity', xvalue = 'eneDensity',
                             title = 'Tile MuonFit energy density;Muon energy loss [MeV/mm]', path = 'Tile/MuonFit',
                             xbins = 50, xmin = 0., xmax = 10., type = 'TH1F', run = run,
                             triggers = l1Triggers, perPartition = False)

    # 10) Configure histograms with Tile MuonFit sinus of the angle in the x-y plane
    addTile1DHistogramsArray(helper, tileMuonFitMonAlg, name = 'TileMuonFitDirectionPhi', xvalue = 'phi',
                             title = 'Tile MuonFit sinus of the angle in the x-y plane;sin(#phi)', path = 'Tile/MuonFit',
                             xbins = 100, xmin = -1., xmax = 1., type = 'TH1F', run = run,
                             triggers = l1Triggers, perPartition = False)

    # 11) Configure histograms with Tile MuonFit sinus of the angle in the y-z plane
    addTile1DHistogramsArray(helper, tileMuonFitMonAlg, name = 'TileMuonFitDirectionTheta', xvalue = 'theta',
                             title = 'Tile MuonFit sinus of the angle in the y-z plane;sin(#theta)', path = 'Tile/MuonFit',
                             xbins = 50, xmin = 0., xmax = 1., type = 'TH1F', run = run,
                             triggers = l1Triggers, perPartition = False)

    # 12) Configure histograms with Tile MuonFit sinus of the angle in the y-z plane
    addTile1DHistogramsArray(helper, tileMuonFitMonAlg, name = 'TileMuonFitPositionTime', xvalue = 'zPosition', value = 'time',
                             title = 'Tile MuonFit average time vs z position at y=0 plane;z [mm]; t [ns]', path = 'Tile/MuonFit',
                             xbins = 79, xmin = -7900., xmax = 7900., type = 'TProfile', run = run,
                             triggers = l1Triggers, perPartition = False)

    from TileMonitoring.TileMonitoringCfgHelper import getPartitionName

    # 13) Configure histograms with Tile MuonFit partition time
    partitionLabels = [getPartitionName(ros) for ros in range(1, Tile.MAX_ROS)]
    addTile1DHistogramsArray(helper, tileMuonFitMonAlg, name = 'TileMuonFitPartitionTime', xvalue = 'partition', value = 'time',
                             title = 'Tile MuonFit average time vs partition at y=0 plane;; t [ns]', path = 'Tile/MuonFit',
                             xbins = Tile.MAX_ROS - 1, xmin = -0.5, xmax = Tile.MAX_ROS - 1.5, xlabels = partitionLabels,
                             type = 'TProfile', run = run, triggers = l1Triggers, perPartition = False)


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
    ConfigFlags.Output.HISTFileName = 'TileMuonFitMonitorOutput.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    from TileCosmicAlgs.TileMuonFitterConfig import TileMuonFitterCfg
    cfg.merge(TileMuonFitterCfg(ConfigFlags))

    l1Triggers = ['bit0_RNDM', 'bit1_ZeroBias', 'bit2_L1Cal', 'bit3_Muon',
                  'bit4_RPC', 'bit5_FTK', 'bit6_CTP', 'bit7_Calib', 'AnyPhysTrig']

    cfg.merge( TileMuonFitMonitoringConfig(ConfigFlags,
                                           fillHistogramsForL1Triggers = l1Triggers) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileMuonFitMonitorAlgorithm.pkl','wb') )

    sc = cfg.run(maxEvents=3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
