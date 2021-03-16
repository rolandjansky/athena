#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
'''@file TileTMDBRawChannelMonitorAlgorithm.py
@author
@date
@brief Python configuration of TileTMDBRawChannelMonitorAlgorithm algorithm for the Run III
'''

def TileMuRcvRawChannelMakerCfg(flags, **kwargs):

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )



    kwargs.setdefault('name', 'TileMuRcvRChMaker')
    kwargs.setdefault('TileDigitsContainer', 'MuRcvDigitsCnt')

    if 'TileRawChannelBuilder' not in kwargs:

        from TileConditions.TilePulseShapeConfig import TileCondToolMuRcvPulseShapeCfg
        pulseShapeTool = acc.popToolsAndMerge( TileCondToolMuRcvPulseShapeCfg(flags) )

        from TileConditions.TileOFCConfig import TileCondToolOfcCfg
        ofcTool = acc.popToolsAndMerge( TileCondToolOfcCfg(flags,
                                                           OptFilterDeltaCorrelation = True,
                                                           TileCondToolPulseShape = pulseShapeTool) )

        from TileRecUtils.TileRawChannelBuilderOptConfig import TileRawChannelBuilderOpt2FilterCfg
        rawChanBuilder = acc.popToolsAndMerge( TileRawChannelBuilderOpt2FilterCfg(flags,
                                                                                  method = 'Opt2',
                                                                                  PedestalMode = 1,
                                                                                  correctTime = False,
                                                                                  NoiseFilterTools = [],
                                                                                  TileCondToolOfc = ofcTool,
                                                                                  TileRawChannelContainer = "TileMuRcvRawChannelOpt2") )


        kwargs.setdefault('TileRawChannelBuilder', [rawChanBuilder])

    from AthenaConfiguration.ComponentFactory import CompFactory
    TileRawChannelMaker=CompFactory.TileRawChannelMaker
    acc.addEventAlgo(TileRawChannelMaker(**kwargs), primary = True)

    return acc

def TileTMDBRawChannelMonitorConfig(flags, MuRcvRawChCnt = "MuRcvRawChCnt", Efficiency = False):

    '''Function to configures some algorithms in the monitoring system.'''
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    result.merge( TileCablingSvcCfg(flags) )

    from TileConditions.TileTMDBConfig import TileTMDBCondAlgCfg
    result.merge( TileTMDBCondAlgCfg(flags) )

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags, 'TileTMDBRawChannelMonitoring')


    from AthenaConfiguration.ComponentFactory import CompFactory
    tileTMDBRawChannelMonitorAlg = helper.addAlgorithm(CompFactory.TileTMDBRawChannelMonitorAlgorithm, 'TileTMDBRawChannelMonitorAlg')

    tileTMDBRawChannelMonitorAlg.TriggerChain = ''


    run = str(flags.Input.RunNumber[0])
    from TileMonitoring.TileMonitoringCfgHelper import addTileTMDB_1DHistogramsArray, addTileTMDB_2DHistogramsArray, getLabels, getPartitionName

    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

    isDSP = (MuRcvRawChCnt == "MuRcvRawChCnt")
    tileTMDBRawChannelMonitorAlg.MuRcvRawChCnt = MuRcvRawChCnt
    tileTMDBRawChannelMonitorAlg.DSP = isDSP
    tileTMDBRawChannelMonitorAlg.Efficiency = Efficiency

    if isDSP:

      minAmplitude = 0
      maxAmplitude = 80000

    else:

      minAmplitude = -20.5
      maxAmplitude = 255.5


    addTileTMDB_2DHistogramsArray(helper, tileTMDBRawChannelMonitorAlg, name = 'TMDB_RawAmplitude',
                                  value = 'amplitude', title = 'Amplitude Map',
                                  path = 'Tile/TMDBRawChannel', type='TProfile2D', run = run)

    addTileTMDB_2DHistogramsArray(helper, tileTMDBRawChannelMonitorAlg, name = 'TMDB_RawTime',
                                  value = 'time', title = 'Time Map',
                                  path = 'Tile/TMDBRawChannel', type='TProfile2D', run = run)


    addTileTMDB_1DHistogramsArray(helper, tileTMDBRawChannelMonitorAlg, name = 'TMDB_RawCellTime',
                                    xvalue = 'time', title = 'Time;[ns]',
                                    path = 'Tile/TMDBRawChannel', type='TH1D', run = run,
                                    xbins = 120, xmin = -60, xmax = 60)

    addTileTMDB_1DHistogramsArray(helper, tileTMDBRawChannelMonitorAlg, name = 'TMDB_RawCellAmplitude',
                                    xvalue = 'amplitude', title = 'Amplitude;[ADC]',
                                    path = 'Tile/TMDBRawChannel', type='TH1D', run = run,
                                    xbins = 276, xmin = minAmplitude, xmax = maxAmplitude)



    if Efficiency:
        arrayTGC = helper.addArray([2], tileTMDBRawChannelMonitorAlg, 'TGC_TrSec_number_Good_Muons')
        for postfix, tool in arrayTGC.Tools.items():
            ros = int(postfix.split('_').pop()) + 3
            partition = getPartitionName(ros)

            fullName = 'sector;TGC_TrSec_number_Good_Muons_{}'.format(partition)
            fullTitle = 'Number of Good Muons in TGC trigger sectors {}'.format(partition)

            tool.defineHistogram(fullName, path = '', type = 'TH1D', title = fullTitle,
                                xbins = 48, xmin = -0.5, xmax = 47.5)


        cells = ['D6', 'D5+D6']
        thresholds = ['500MeV','600MeV']
        tileTMDBRawChannelMonitorAlg.numberOfThresholds = len(thresholds)
        arrayThreshD = helper.addArray([2, len(thresholds), len(cells)], tileTMDBRawChannelMonitorAlg, 'TMDB_coincidence_threshold')
        for postfix, tool in arrayThreshD.Tools.items():
            elements = postfix.split('_')
            cell = elements.pop()
            threshold = elements.pop()
            ros = int(elements.pop()) + 3
            partition = getPartitionName(ros)

            labels = getLabels(('modules'), partition)

            fullName = 'module;TMDB_{}_{}_coincidence_{}'.format(partition, thresholds[int(threshold)], cells[int(cell)])
            fullTitle = 'Number coincidence from all Good Muons in TMDB {} {}  over {} threshold'.format(partition, thresholds[int(threshold)], cells[int(cell)])

            tool.defineHistogram(fullName, path = '', type = 'TH1D', title = fullTitle, xlabels = labels,
                                xbins = Tile.MAX_DRAWER, xmin = -0.5, xmax = Tile.MAX_DRAWER - 0.5)

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

    ConfigFlags.Input.Files = ['root://eosatlas//eos/atlas/atlastier0/rucio/data18_13TeV/physics_Main/00357750/data18_13TeV.00357750.physics_Main.recon.ESD.f1073/data18_13TeV.00357750.physics_Main.recon.ESD.f1073._lb0119._SFO-6._0001.1']
    ConfigFlags.Output.HISTFileName = 'TileTMDBRawChannelMonitorOutput.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.Exec.MaxEvents = 500
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)


    # from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    # tileTypeNames = ['TileRawChannelContainer/MuRcvRawChCnt', 'TileDigitsContainer/MuRcvDigitsCnt']
    # tileTypeNames += ['TileRawChannelContainer/TileRawChannelCnt', 'TileDigitsContainer/TileDigitsCnt']
    # cfg.merge( ByteStreamReadCfg(ConfigFlags, type_names = tileTypeNames) )

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))


    cfg.merge(TileMuRcvRawChannelMakerCfg(ConfigFlags))
    # DSP Container
    # tileTMDBRawChannelMonitoringAccumulator  = TileTMDBRawChannelMonitorConfig(ConfigFlags, MuRcvRawChCnt = "TileMuRcvRawChannelOpt2", Efficiency = True)
    tileTMDBRawChannelMonitoringAccumulator = TileTMDBRawChannelMonitorConfig(ConfigFlags, MuRcvRawChCnt = "MuRcvRawChCnt", Efficiency = False)

    cfg.merge(tileTMDBRawChannelMonitoringAccumulator)

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()


    cfg.store( open('TileTMDBRawChannelMonitorAlgorithm.pkl','wb') )

    sc = cfg.run()

    import sys
    sys.exit(not sc.isSuccess())
