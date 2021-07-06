#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
'''@file TileTMDBRawChannelMonitorAlgorithm.py
@author
@date
@brief Python configuration of TileTMDBRawChannelMonitorAlgorithm algorithm for the Run III
'''
def TileMuRcvRawChannelMakerCfg(flags, MuRcvRawChCnt = "TileMuRcvRawChannelOpt2", **kwargs):

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
                                                                                  TileRawChannelContainer = MuRcvRawChCnt) )


        kwargs.setdefault('TileRawChannelBuilder', [rawChanBuilder])

    from AthenaConfiguration.ComponentFactory import CompFactory
    TileRawChannelMaker=CompFactory.TileRawChannelMaker
    acc.addEventAlgo(TileRawChannelMaker(**kwargs), primary = True)

    return acc

def TileTMDBRawChannelMonitoringConfig(flags, MuRcvRawChCnt = "MuRcvRawChCnt", FillRawChannelHistograms = True, FillEfficiencyHistograms = False, **kwargs):

    '''Function to configure Tile TMDB raw channel monitoring algorithm.'''
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    result.merge( TileCablingSvcCfg(flags) )

    isDSP = (MuRcvRawChCnt == "MuRcvRawChCnt")
    if not isDSP:
        result.merge(TileMuRcvRawChannelMakerCfg(flags, MuRcvRawChCnt = MuRcvRawChCnt))

    name = kwargs.get('name', 'TileTMDBRawChanDspMonAlg' if isDSP else 'TileTMDBRawChanMonAlg')

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags, 'TileTMDBRawChannelDspMonitoring' if isDSP else 'TileTMDBRawChannelMonitoring')

    from AthenaConfiguration.ComponentFactory import CompFactory
    tileTMDBRawChanMonAlg = helper.addAlgorithm(CompFactory.TileTMDBRawChannelMonitorAlgorithm, name)
    tileTMDBRawChanMonAlg.TriggerChain = ''
    tileTMDBRawChanMonAlg.MuRcvRawChCnt = MuRcvRawChCnt
    tileTMDBRawChanMonAlg.DSP = isDSP
    tileTMDBRawChanMonAlg.FillEfficiencyHistograms = FillEfficiencyHistograms
    tileTMDBRawChanMonAlg.FillRawChannelHistograms = FillRawChannelHistograms

    topPath = 'Tile/TMDBRawChannel/DSP' if isDSP else 'Tile/TMDBRawChannel'
    run = str(flags.Input.RunNumber[0])

    if FillRawChannelHistograms:

        if isDSP:
            minAmplitude = 0
            maxAmplitude = 80000
        else:
            minAmplitude = -20.5
            maxAmplitude = 255.5

        from TileMonitoring.TileMonitoringCfgHelper import addTileTMDB_1DHistogramsArray, addTileTMDB_2DHistogramsArray

        addTileTMDB_2DHistogramsArray(helper, tileTMDBRawChanMonAlg, name = 'TMDB_RawAmplitude',
                                      value = 'amplitude', title = 'Amplitude Map',
                                      path = topPath, type='TProfile2D', run = run)

        addTileTMDB_2DHistogramsArray(helper, tileTMDBRawChanMonAlg, name = 'TMDB_RawTime',
                                      value = 'time', title = 'Time Map',
                                      path = topPath, type='TProfile2D', run = run)

        addTileTMDB_1DHistogramsArray(helper, tileTMDBRawChanMonAlg, name = 'TMDB_RawCellTime',
                                      xvalue = 'time', title = 'Time;[ns]',
                                      path = topPath, type='TH1D', run = run,
                                      xbins = 120, xmin = -60, xmax = 60)

        addTileTMDB_1DHistogramsArray(helper, tileTMDBRawChanMonAlg, name = 'TMDB_RawCellAmplitude',
                                      xvalue = 'amplitude', title = 'Amplitude;[ADC]',
                                      path = topPath, type='TH1D', run = run,
                                      xbins = 276, xmin = minAmplitude, xmax = maxAmplitude)


    if FillEfficiencyHistograms:

        from TileConditions.TileTMDBConfig import TileTMDBCondAlgCfg
        result.merge( TileTMDBCondAlgCfg(flags) )

        from TileCalibBlobObjs.Classes import TileCalibUtils as Tile
        from TileMonitoring.TileMonitoringCfgHelper import getLabels, getPartitionName

        arrayTGC = helper.addArray([2], tileTMDBRawChanMonAlg, 'TGC_TrSec_number_Good_Muons')
        for postfix, tool in arrayTGC.Tools.items():
            ros = int(postfix.split('_').pop()) + 3
            partition = getPartitionName(ros)

            fullName = 'sector;TGC_TrSec_number_Good_Muons_{}'.format(partition)
            fullTitle = 'Number of Good Muons in TGC trigger sectors {}'.format(partition)

            tool.defineHistogram(fullName, path = topPath, type = 'TH1D', title = fullTitle,
                                xbins = 48, xmin = -0.5, xmax = 47.5)


        cells = ['D6', 'D5+D6']
        thresholds = ['500MeV','600MeV']
        tileTMDBRawChanMonAlg.NumberOfThresholds = len(thresholds)
        arrayThreshD = helper.addArray([2, len(thresholds), len(cells)], tileTMDBRawChanMonAlg, 'TMDB_coincidence_threshold')
        for postfix, tool in arrayThreshD.Tools.items():
            elements = postfix.split('_')
            cell = int(elements.pop())
            threshold = int(elements.pop())
            ros = int(elements.pop()) + 3
            partition = getPartitionName(ros)

            labels = getLabels(('modules'), partition)

            fullName = 'module;TMDB_{}_{}_coincidence_{}'.format(partition, cells[cell], thresholds[threshold])
            fullTitle = 'Number coincidence from all Good Muons in TMDB {} {}  over {} threshold'.format(partition, cells[cell], thresholds[threshold])

            tool.defineHistogram(fullName, path = topPath, type = 'TH1D', title = fullTitle, xlabels = labels,
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

    ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q431/22.0/v1/myESD.pool.root']
    ConfigFlags.Output.HISTFileName = 'TileTMDBRawChannelMonitorOutput.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.Exec.MaxEvents = 3
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    cfg.merge( TileTMDBRawChannelMonitoringConfig(ConfigFlags, MuRcvRawChCnt = "MuRcvRawChCnt", FillEfficiencyHistograms = True) )
    cfg.merge( TileTMDBRawChannelMonitoringConfig(ConfigFlags, MuRcvRawChCnt = "TileMuRcvRawChannelOpt2") )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileTMDBRawChannelMonitorAlgorithm.pkl','wb') )

    sc = cfg.run()

    import sys
    sys.exit(not sc.isSuccess())
