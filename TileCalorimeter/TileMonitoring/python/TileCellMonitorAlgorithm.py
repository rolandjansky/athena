#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileCellMonitorAlgorithm.py
@brief Python configuration of TileCellMonitorAlgorithm algorithm for the Run III
'''
def TileCellMonitoringConfig(flags, **kwargs):

    ''' Function to configure TileCellMonitorAlgorithm algorithm in the monitoring system.'''

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

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags,'TileMonitoring')

    # Adding an TileCellMonitorAlgorithm algorithm to the helper
    from AthenaConfiguration.ComponentFactory import CompFactory
    tileCellMonAlg = helper.addAlgorithm(CompFactory.TileCellMonitorAlgorithm, 'TileCellMonAlg')

    tileCellMonAlg.TriggerChain = ''

    from AthenaCommon.SystemOfUnits import MeV, GeV, ns
    kwargs.setdefault('EnergyThreshold', 300.0 * MeV)
    kwargs.setdefault('NegativeEnergyThreshold', -2000.0 * MeV)
    kwargs.setdefault('EnergyBalanceThreshold', 3)
    kwargs.setdefault('TimeBalanceThreshold', 25 * ns)
    kwargs.setdefault('fillTimeAndEnergyDiffHistograms', False)

    if flags.Beam.Type in ('cosmics', 'singlebeam'):
        kwargs.setdefault('fillTimeHistograms', True)
        kwargs.setdefault('EnergyThresholdForTime', 150.0 * MeV)
    else:
        kwargs.setdefault('fillTimeHistograms', False)
        kwargs.setdefault('EnergyThresholdForTime', 500.0 * MeV)

    # L1Trigger Type Bits:
    #     bit0_RNDM, bit1_ZeroBias, bit2_L1Cal, bit3_Muon,
    #     bit4_RPC, bit5_FTK, bit6_CTP, bit7_Calib, AnyPhysTrig
    kwargs.setdefault('fillHistogramsForL1Triggers', ['AnyPhysTrig', 'bit7_Calib'])
    l1Triggers = kwargs['fillHistogramsForL1Triggers']

    for k, v in kwargs.items():
        setattr(tileCellMonAlg, k, v)

    run = str(flags.Input.RunNumber[0])

    # 1) Configure histogram with TileCellMonAlg algorithm execution time
    executeTimeGroup = helper.addGroup(tileCellMonAlg, 'TileCellMonExecuteTime', 'Tile/')
    executeTimeGroup.defineHistogram('TIME_execute', path = 'Cell', type='TH1F',
                                     title = 'Time for execute TileCellMonAlg algorithm;time [#mus]',
                                     xbins = 100, xmin = 0, xmax = 100000)


    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile
    from TileMonitoring.TileMonitoringCfgHelper import getPartitionName

    # 2) Configure histogram with average number of Tile bad cells in partition
    labelsPartitions = [getPartitionName(ros) for ros in range(1, Tile.MAX_ROS)]
    badCellGroup = helper.addGroup(tileCellMonAlg, 'TileBadCell', 'Tile/')
    badCellGroup.defineHistogram('Partition,nBadCells;TileBadCell', path = 'Cell', type='TProfile',
                                 xlabels = labelsPartitions, title = ('Run %s: Average number of Bad Tile Cells' % run),
                                 xbins = Tile.MAX_ROS - 1, xmin = -0.5, xmax = Tile.MAX_ROS - 1.5)


    from TileMonitoring.TileMonitoringCfgHelper import addTileModulePartitionMapsArray

    # 3) Configure histograms with energy ratio > threshold vs module and partion
    eneBalModPartTitle = ('Tile Cell Energy Ratio > %s' % kwargs['EnergyBalanceThreshold'])
    addTileModulePartitionMapsArray(helper, tileCellMonAlg, name = 'TileCellEneBalModPart',
                                    title = eneBalModPartTitle, path = 'Tile/Cell', type='TH2D',
                                    run = run, triggers = l1Triggers, separator = '_')

    # 4) Configure histograms with Tile cell time difference over threshold vs module and partition
    timeBalModPartTitle = ('Tile Cell Time difference > %s ns. E_{ch} > %s [MeV]')
    timeBalModPartTitle = (timeBalModPartTitle % (kwargs['TimeBalanceThreshold'], kwargs['EnergyThresholdForTime']))
    addTileModulePartitionMapsArray(helper, tileCellMonAlg, name = 'TileCellTimeBalModPart',
                                    title = timeBalModPartTitle, path = 'Tile/Cell', type='TH2D',
                                    run = run, triggers = l1Triggers, separator = '_')


    from TileMonitoring.TileMonitoringCfgHelper import addTileModuleChannelMapsArray, addTileModuleDigitizerMapsArray

    # 5) Configure histograms with everagy Tile channel time per partition
    chanTimeTitle = ('TileCal Average Channel Time [ns].  E_{ch} > %s MeV' % kwargs['EnergyThresholdForTime'])
    addTileModuleChannelMapsArray(helper, tileCellMonAlg, name = 'TileChanPartTime', type='TProfile2D',
                                  value = 'time', title = chanTimeTitle, path = 'Tile/Cell',
                                  subDirectory = True, run = run, triggers = l1Triggers, separator = '_')

    # 6) Configure histograms with everagy Tile digitizer time per partition
    digiTimeTitle = ('TileCal Average Digitizer Time [ns].  E_{ch} > %s MeV' % kwargs['EnergyThresholdForTime'])
    addTileModuleDigitizerMapsArray(helper, tileCellMonAlg, name = 'TileDigiPartTime', type='TProfile2D',
                                    value = 'time', title = digiTimeTitle, path = 'Tile/Cell',
                                    subDirectory = True, run = run, triggers = l1Triggers, separator = '_')


    # 7) Configure histograms with everagy energy maps per partition
    addTileModuleChannelMapsArray(helper, tileCellMonAlg, name = 'TileCellDetailOccMap', type='TProfile2D',
                                  value = 'energy', title = 'Occupancy Map [MeV]', path = 'Tile/Cell',
                                  subDirectory = True, run = run, triggers = l1Triggers, separator = '_')


    # 8) Configure histograms with occupancy maps over threshold per partition
    titleMapOvThr = ('Occupancy Map Over Threshod %s MeV' % kwargs['EnergyThreshold'])
    addTileModuleChannelMapsArray(helper, tileCellMonAlg, name = 'TileCellDetailOccMapOvThr',
                                  weight = 'weight', title = titleMapOvThr, path = 'Tile/Cell',
                                  subDirectory = True, run = run, triggers = l1Triggers, separator = '_')


    # 9) Configure histograms with occupancy maps over threshold 30GeV per partition
    addTileModuleChannelMapsArray(helper, tileCellMonAlg, name = 'TileCellDetailOccMapOvThr30GeV',
                                  title = 'Occupancy Map Over Threshod 30 GeV', path = 'Tile/Cell',
                                  subDirectory = True, run = run, triggers = l1Triggers, separator = '_')


    # 10) Configure histograms with occupancy maps over threshold 300GeV per partition
    addTileModuleChannelMapsArray(helper, tileCellMonAlg, name = 'TileCellDetailOccMapOvThr300GeV',
                                  title = 'Occupancy Map Over Threshod 300 GeV', path = 'Tile/Cell',
                                  subDirectory = True, run = run, triggers = l1Triggers, separator = '_')

    # 11) Configure histograms with occupancy maps over threshold per partition
    addTileModuleChannelMapsArray(helper, tileCellMonAlg, name = 'TileCellDetailOccMapOvThrGain',
                                  weight = 'weight', title = titleMapOvThr, path = 'Tile/Cell', subDirectory = True,
                                  run = run, triggers = l1Triggers, perGain = True, separator = '_')


    # 12) Configure histograms with status of Tile channels in DB per partition
    addTileModuleChannelMapsArray(helper, tileCellMonAlg, name = 'TileCellStatusInDB',
                                  title = 'Cell channel status in DB', path = 'Tile/Cell',
                                  run = run, perGain = True, separator = '_')

    # 13) Configure histograms with Tile channels masked on the fly per partition
    addTileModuleChannelMapsArray(helper, tileCellMonAlg, name = 'TileCellStatusOnFly',
                                  title = 'Channels masked on the fly', path = 'Tile/Cell',
                                  run = run, perGain = False, separator = '_')


    # 14) Configure histograms with occupancy map below negative threshold per partition
    titleNegOcc = 'Occupancy Map Below %s GeV' % (kwargs['NegativeEnergyThreshold'] / GeV)
    addTileModuleChannelMapsArray(helper, tileCellMonAlg, name = 'TileCellDetailNegOccMap',
                                  title = titleNegOcc, path = 'Tile/Cell', run = run, separator = '_')

    # 15) Configure histograms with Tile module correlation per partition
    from TileMonitoring.TileMonitoringCfgHelper import addTileModuleCorrelionMapsArray
    addTileModuleCorrelionMapsArray(helper, tileCellMonAlg, name = 'TileCellModuleCorrelation',
                                    title = 'Tile Cell Module correlation', path = 'Tile/Cell',
                                    weight = 'weight', subDirectory = True, run = run,
                                    triggers = l1Triggers, allPartitions = True, separator = '_')


    from TileMonitoring.TileMonitoringCfgHelper import addTile1DHistogramsArray

    # 16) Configure histograms with number of masked Tile channels on the fly vs lumi block per partition
    titleMaskOnFlyLB = 'Number of masked channels on the fly'
    titleMaskOnFlyLB += ';LumiBlock;Number of masked channels'
    addTile1DHistogramsArray(helper, tileCellMonAlg, name = 'TileMaskChannelOnFlyLB', path = 'Tile/Cell',
                             xvalue = 'lumiBlock', value= 'nMaskedChannelsOnFly', title = titleMaskOnFlyLB,
                             xbins = 1000, xmin = -0.5, xmax = 999.5, type = 'TProfile', run = run, triggers = [],
                             perPartition = True, perSample = False, perGain = False, subDirectory = False,
                             opt = 'kAddBinsDynamically')

    # 17) Configure histograms with number of masked Tile cells on the fly vs lumi block per partition
    titleMaskCellLB = 'Number of masked cells on the fly'
    titleMaskCellLB += ';LumiBlock;Number of masked cells'
    addTile1DHistogramsArray(helper, tileCellMonAlg, name = 'TileMaskCellLB', path = 'Tile/Cell',
                             xvalue = 'lumiBlock', value = 'nMaskedCells', title = titleMaskCellLB,
                             xbins = 1000, xmin = -0.5, xmax = 999.5, type='TProfile', run = run, triggers = [],
                             subDirectory = False, perPartition = True, perSample = False, perGain = False,
                             opt = 'kAddBinsDynamically')

    # 18) Configure histograms with number of masked Tile channels on the fly due to bad DQ status vs lumi block per partition
    titleMaskDueDQ = 'Number of masked channels on the fly due to bad DQ status'
    titleMaskDueDQ += ';LumiBlock;Number of masked channels'
    addTile1DHistogramsArray(helper, tileCellMonAlg, name = 'TileMaskChannelDueDQvsLB', path = 'Tile/Cell',
                             xvalue = 'lumiBlock', value = 'nMaskedChannelsDueDQ', title = titleMaskDueDQ,
                             xbins = 1000, xmin = -0.5, xmax = 999.5, type='TProfile', run = run, triggers = [],
                             subDirectory = False, perPartition = True, perSample = False, perGain = False,
                             opt = 'kAddBinsDynamically')

    # 19) Configure histograms with number of masked Tile cells on the fly due to bad DQ status vs lumi block per partition
    titleMaskCellDueDQ = 'Number of masked cells on the fly due to bad DQ status'
    titleMaskCellDueDQ += ';LumiBlock;Number of masked cells'
    addTile1DHistogramsArray(helper, tileCellMonAlg, name = 'TileMaskedCellDueDQvsLB', path = 'Tile/Cell',
                             xvalue = 'lumiBlock', value = 'nMaskedCellsDueDQ', title = titleMaskCellDueDQ,
                             xbins = 1000, xmin = -0.5, xmax = 999.5, type='TProfile', run = run, triggers = [],
                             subDirectory = False, perPartition = True, perSample = False, perGain = False,
                             opt = 'kAddBinsDynamically')

    # 20) Configure histograms with energy difference between Tile cells' PMTs per partition and sample
    addTile1DHistogramsArray(helper, tileCellMonAlg, name = 'TileCellEneDiff', xvalue = 'energyDiff',
                             title = 'Energy difference [MeV] between PMTs;Energy difference [MeV]',
                             path = 'Tile/Cell', xbins = 50, xmin = -1000., xmax = 1000., type='TH1D',
                             run = run, triggers = l1Triggers, subDirectory = True,
                             perPartition = True, perSample = True, perGain = False)

    # 21) Configure histograms with time difference between Tile cells' PMTs per partition and sample
    titleTimeDiffSamp = 'Time difference [ns] between PMTs with '
    titleTimeDiffSamp += 'E_{ch} > %s MeV' % (kwargs['EnergyThresholdForTime'] / MeV)
    titleTimeDiffSamp += ';time [ns]'
    addTile1DHistogramsArray(helper, tileCellMonAlg, name = 'TileCellTimeDiff',
                             xvalue = 'timeDiff', title = titleTimeDiffSamp, path = 'Tile/Cell',
                             xbins = 50, xmin = -10., xmax = 10., type='TH1D', run = run, triggers = l1Triggers,
                             subDirectory = True, perPartition = True, perSample = True, perGain = False)

    # 22) Configure histograms with number of Tile cells over threshold vs BCID per partition
    titleCellsOvThrBCID = 'Tile Cell Occupancy over Threshold %s MeV' % (kwargs['EnergyThresholdForTime'] / MeV)
    titleCellsOvThrBCID += ';BCID;Average number of cells over threshold'
    addTile1DHistogramsArray(helper, tileCellMonAlg, name = 'TileCellOccOvThrBCID',
                             xvalue = 'BCID', value = 'nCells', title = titleCellsOvThrBCID, path = 'Tile/Cell',
                             xbins = 3565, xmin = 0., xmax = 3565., type='TProfile', run = run, triggers = l1Triggers,
                             subDirectory = True, perPartition = True, perSample = False, perGain = False, allPartitions = True)

    # 23) Configure histograms with number of Tile E cell's energy  per partition
    titleEvEnergy = 'Tile Event SampE Energy;Event Energy [MeV]'
    addTile1DHistogramsArray(helper, tileCellMonAlg, name = 'TileCellEventEnergySampE',
                             xvalue = 'energy', title = titleEvEnergy, path = 'Tile/Cell',
                             xbins = 120, xmin = -2000., xmax = 10000., type='TH1D', run = run, triggers = l1Triggers,
                             subDirectory = True, perPartition = True, perSample = False, perGain = False, allPartitions = True)

    # 24) Configure histograms with number of Tile E cell's energy
    titleSynch = 'Tile Time of Flight - Tile measured;Time of Flight - Tile measured [ns]'
    addTile1DHistogramsArray(helper, tileCellMonAlg, name = 'TileCellSynch',
                             xvalue = 'timeDifference', title = titleSynch, path = 'Tile/Cell',
                             xbins = 50, xmin = -100., xmax = 100., type='TH1D', run = run, triggers = l1Triggers,
                             subDirectory = False, perPartition = False, perSample = False, perGain = False)


    from TileMonitoring.TileMonitoringCfgHelper import addTileEtaPhiMapsArray

    # 25) Configure histograms with everage Tile cell energy vs eta and phy per sample
    addTileEtaPhiMapsArray(helper, tileCellMonAlg, name = 'TileCellEneEtaPhi', type='TProfile2D',
                           value = 'energy', title = 'Energy Average depostion [MeV]',
                           path = 'Tile/Cell', run = run, triggers = l1Triggers, perSample = True)

    # 26) Configure histograms with number of Tile cells over threshold vs eta and phy per sample
    titleEtaPhiOvThr = ('Position of cells over threshold %s MeV' % kwargs['EnergyThreshold'])
    addTileEtaPhiMapsArray(helper, tileCellMonAlg, name = 'TileCellEtaPhiOvThr',
                           type='TH2D', title = titleEtaPhiOvThr, path = 'Tile/Cell',
                           run = run, triggers = l1Triggers, perSample = True)


    from TileMonitoring.TileMonitoringCfgHelper import addTileModuleArray

    # 27) Configure histograms with energy difference between Tile cell's PMTs vs module per partition
    titleEnergyBal = "Cell's PMTs Energy Balance"
    titleEnergyBal += ";;Energy balance between cell's PMTs (u-d)/(u+d)"
    addTileModuleArray(helper, tileCellMonAlg, name = 'TileCellEnergyBalance', type='TProfile',
                       title = titleEnergyBal, path = 'Tile/Cell', value = 'energyBalance', run = run)


    # 28) Configure histograms with time difference between Tile cell's PMTs vs module per partition
    titleTimeBal = "Cell's PMTs Time Difference with "
    titleTimeBal += 'E_{ch} > %s MeV' % (kwargs['EnergyThresholdForTime'] / MeV)
    titleTimeBal += ";;Time balance between cell's PMTs [ns]"
    addTileModuleArray(helper, tileCellMonAlg, name = 'TileCellTimeBalance', type='TProfile',
                       title = titleTimeBal, path = 'Tile/Cell', value = 'timeBalance', run = run)


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
    ConfigFlags.Output.HISTFileName = 'TileCellMonitorOutput.root'
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

    cfg.merge( TileCellMonitoringConfig(ConfigFlags,
                                        fillHistogramsForL1Triggers = l1Triggers,
                                        fillTimeAndEnergyDiffHistograms = True) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileCellMonitorAlgorithm.pkl','wb') )

    sc = cfg.run(maxEvents=3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
