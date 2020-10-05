#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileMuIdMonitorAlgorithm.py
@brief Python configuration of TileMuIdMonitorAlgorithm algorithm for the Run III
'''
def TileMuIdMonitoringConfig(flags, **kwargs):

    ''' Function to configure TileMuIdMonitorAlgorithm algorithm in the monitoring system.'''

    # Define one top-level monitoring algorithm. The new configuration
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(TileGMCfg(flags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    result.merge(LArGMCfg(flags))

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags,'TileMuIdMonitoring')

    # Adding an TileCellMonitorAlgorithm algorithm to the helper
    from AthenaConfiguration.ComponentFactory import CompFactory
    tileMuIdMonAlg = helper.addAlgorithm(CompFactory.TileMuIdMonitorAlgorithm, 'TileMuIdMonAlg')

    tileMuIdMonAlg.TriggerChain = ''

    # L1Trigger Type Bits:
    #     bit0_RNDM, bit1_ZeroBias, bit2_L1Cal, bit3_Muon,
    #     bit4_RPC, bit5_FTK, bit6_CTP, bit7_Calib, AnyPhysTrig
    kwargs.setdefault('fillHistogramsForL1Triggers', ['AnyPhysTrig', 'bit7_Calib'])
    l1Triggers = kwargs['fillHistogramsForL1Triggers']

    for k, v in kwargs.items():
        setattr(tileMuIdMonAlg, k, v)

    run = str(flags.Input.RunNumber[0])

    # 1) Configure histogram with TileMuIdMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileMuIdMonAlg, 'TileMuIdMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = 'MuId', type='TH1F',
                                     title = 'Time for execute TileMuIdMonAlg algorithm;time [#mus]',
                                     xbins = 100, xmin = 0, xmax = 1000)


    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

    from TileMonitoring.TileMonitoringCfgHelper import addTileEtaPhiMapsArray

    # 2) Configure histograms with most energetic Tile MuId muon position
    addTileEtaPhiMapsArray(helper, tileMuIdMonAlg, name = 'TileMuIdEtaPhi', type='TH2D',
                           title = 'Tile MuId muon position', path = 'Tile/MuId',
                           run = run, triggers = l1Triggers, perSample = False)

    # 3) Configure histograms with most energetic Tile MuId muon position when Tags=1
    addTileEtaPhiMapsArray(helper, tileMuIdMonAlg, name = 'TileMuId1TagEtaPhi', type='TH2D',
                           title = 'Tile MuId Tags=1 muon position', path = 'Tile/MuId',
                           run = run, triggers = l1Triggers, perSample = False)

    # 4) Configure histograms with most energetic Tile MuId muon position when Tags=2
    addTileEtaPhiMapsArray(helper, tileMuIdMonAlg, name = 'TileMuId2TagEtaPhi', type='TH2D',
                           title = 'Tile MuId Tags=2 muon position', path = 'Tile/MuId',
                           run = run, triggers = l1Triggers, perSample = False)


    from TileMonitoring.TileMonitoringCfgHelper import addTile2DHistogramsArray

    # 5) Configure histograms with Tile MuId muon quality vs energy
    addTile2DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuIdEneQuality',
                             xvalue = 'energy', yvalue = 'quality', type='TH2D',
                             title = 'Tile MuId muon quality vs energy [MeV];Energy [MeV];Quality',
                             path = 'Tile/MuId', xbins = 50, xmin = 0., xmax = 10000.,
                             ybins = 4, ymin = -0.5, ymax = 1.5, run = run, triggers = l1Triggers)

    # 6) Configure histograms with Tile  MuId muon quality vs energy when Tags=1
    addTile2DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuId1TagEneQuality',
                             xvalue = 'energy', yvalue = 'quality', type='TH2D',
                             title = 'Tile MuId Tags=1 muon quality vs energy [MeV];Energy [MeV];Quality',
                             path = 'Tile/MuId', xbins = 50, xmin = 0., xmax = 10000.,
                             ybins = 4, ymin = -0.5, ymax = 1.5, run = run, triggers = l1Triggers)

    # 7) Configure histograms with Tile  MuId muon quality vs energy when Tags=2
    addTile2DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuId2TagEneQuality',
                             xvalue = 'energy', yvalue = 'quality', type='TH2D',
                             title = 'Tile MuId Tags=2 muon quality vs energy [MeV];Energy [MeV];Quality',
                             path = 'Tile/MuId', xbins = 50, xmin = 0., xmax = 10000.,
                             ybins = 4, ymin = -0.5, ymax = 1.5, run = run, triggers = l1Triggers)


    from TileMonitoring.TileMonitoringCfgHelper import addTile1DHistogramsArray

    # 8) Configure histograms with Tile MuId number of tags
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuIdTags', xvalue = 'nTags',
                             title = 'Tile MuId number of tags', path = 'Tile/MuId',
                             xbins = 5, xmin = -0.5, xmax = 4.5, type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # 9) Configure histograms with all Tile MuId energy
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuIdEnergy', xvalue = 'energy',
                             title = 'Tile MuId Energy [MeV]', path = 'Tile/MuId',
                             xbins = 100, xmin = 0., xmax = 10000., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # 10) Configure histograms with all Tile MuId muon quality
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuIdQuality', xvalue = 'quality',
                             title = 'Tile MuId muon quality', path = 'Tile/MuId',
                             xbins = 4, xmin = -0.5, xmax = 1.5, type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # 11) Configure histograms with all Tile MuId muon eta position
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuIdEta', xvalue = 'eta',
                             title = 'Tile MuId muon #eta position;#eta', path = 'Tile/MuId',
                             xbins = 40, xmin = -2., xmax = 2., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # 12) Configure histograms with all Tile MuId muon phi position
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuIdPhi', xvalue = 'phi',
                             title = 'Tile MuId muon #phi position;#phi', path = 'Tile/MuId',
                             xbins = Tile.MAX_DRAWER, xmin = -3.15, xmax = 3.15, type = 'TH1D',
                             run = run, triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # 13) Configure histograms with all Tile MuId muon average energy vs eta
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuIdEneEta', xvalue = 'eta',
                             value = 'energy', title = 'Tile MuId muon average energy;#eta',
                             path = 'Tile/MuId', xbins = 40, xmin = -2., xmax = 2., type = 'TProfile',
                             run = run, triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # 14) Configure histograms with all Tile MuId muon average energy vs phi
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuIdEnePhi', xvalue = 'phi', value='energy',
                             title = 'Tile MuId muon everage energy;#phi', path = 'Tile/MuId',
                             xbins = Tile.MAX_DRAWER, xmin = -3.15, xmax = 3.15, type = 'TProfile',
                             run = run, triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)


    # 15) Configure histograms with all Tile MuId energy when Tags=1
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuId1TagEnergy', xvalue = 'energy',
                             title = 'Tile MuId Tags=1 Energy [MeV]', path = 'Tile/MuId',
                             xbins = 100, xmin = 0., xmax = 10000., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # 16) Configure histograms with all Tile MuId muon quality when Tags=1
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuId1TagQuality', xvalue = 'quality',
                             title = 'Tile MuId Tags=1 muon quality', path = 'Tile/MuId',
                             xbins = 4, xmin = -0.5, xmax = 1.5, type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # 17) Configure histograms with all Tile MuId muon average energy vs eta when Tags=1
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuId1TagEneEta', xvalue = 'eta',
                             value = 'energy', title = 'Tile MuId Tags=1 muon average energy;#eta',
                             path = 'Tile/MuId', xbins = 40, xmin = -2., xmax = 2., type = 'TProfile',
                             run = run, triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # 18) Configure histograms with all Tile MuId muon average energy vs phi when Tags=1
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuId1TagEnePhi', xvalue = 'phi',
                             value = 'energy', title = 'Tile MuId Tags=1 muon everage energy;#phi',
                             path = 'Tile/MuId', xbins = Tile.MAX_DRAWER, xmin = -3.15, xmax = 3.15,
                             type = 'TProfile', run = run, triggers = l1Triggers, perPartition = False,
                             perSample = False, perGain = False, subDirectory = False, allPartitions = False)


    # 19) Configure histograms with all Tile MuId energy when Tags=2
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuId2TagEnergy', xvalue = 'energy',
                             title = 'Tile MuId Tags=2 Energy [MeV]', path = 'Tile/MuId',
                             xbins = 100, xmin = 0., xmax = 10000., type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # 20) Configure histograms with all Tile MuId muon quality when Tags=2
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuId2TagQuality', xvalue = 'quality',
                             title = 'Tile MuId Tags=2 muon quality', path = 'Tile/MuId',
                             xbins = 4, xmin = -0.5, xmax = 1.5, type = 'TH1D', run = run,
                             triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # 21) Configure histograms with all Tile MuId muon average energy vs eta when Tags=2
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuId2TagEneEta', xvalue = 'eta',
                             value = 'energy', title = 'Tile MuId Tags=2 muon average energy;#eta',
                             path = 'Tile/MuId', xbins = 40, xmin = -2., xmax = 2., type = 'TProfile',
                             run = run, triggers = l1Triggers, perPartition = False, perSample = False,
                             perGain = False, subDirectory = False, allPartitions = False)

    # 22) Configure histograms with all Tile MuId muon average energy vs phi when Tags=2
    addTile1DHistogramsArray(helper, tileMuIdMonAlg, name = 'TileMuId2TagEnePhi', xvalue = 'phi',
                             value = 'energy', title = 'Tile MuId Tags=2 muon everage energy;#phi',
                             path = 'Tile/MuId', xbins = Tile.MAX_DRAWER, xmin = -3.15, xmax = 3.15,
                             type = 'TProfile', run = run, triggers = l1Triggers, perPartition = False,
                             perSample = False, perGain = False, subDirectory = False, allPartitions = False)

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
    ConfigFlags.Output.HISTFileName = 'TileMuIdMonitorOutput.root'
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

    cfg.merge( TileMuIdMonitoringConfig(ConfigFlags,
                                        fillHistogramsForL1Triggers = l1Triggers) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileMuIdMonitorAlgorithm.pkl','wb') )

    sc = cfg.run(maxEvents=-1)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
