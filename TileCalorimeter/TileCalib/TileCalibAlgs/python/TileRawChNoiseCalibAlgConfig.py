#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileRawChNoiseCalibAlgConfig.py
@brief Python configuration of TileRawChNoiseCalibAlg algorithm for the Run III
'''
def TileRawChNoiseCalibAlgCfg(flags, **kwargs):

    ''' Function to configure TileRawChNoiseCalibAlg algorithm'''

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge( TileCablingSvcCfg(flags) )

    #from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    #acc.merge( TileInfoLoaderCfg(flags) )

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg( flags ))

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    acc.merge( TileDQstatusAlgCfg(flags) )

    if 'TileBadChanTool' not in kwargs:
        from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
        badChanTool = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )
        kwargs['TileBadChanTool'] = badChanTool

    if 'TileCondToolEmscale' not in kwargs:
        from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
        emScaleTool = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )
        kwargs['TileCondToolEmscale'] = emScaleTool

    kwargs.setdefault('name', 'TileRawChNoiseCalibAlg')
    kwargs.setdefault('doDsp', True)
    kwargs.setdefault('doFit', flags.Tile.doFit)
    kwargs.setdefault('doOpt', flags.Tile.doOpt2)
    kwargs.setdefault('doFixed', flags.Tile.doOptATLAS)
    kwargs.setdefault('doMF', flags.Tile.doMF)
    kwargs.setdefault('doOF1', flags.Tile.doOF1)

    from AthenaConfiguration.ComponentFactory import CompFactory
    TileRawChNoiseCalibAlgCfg = CompFactory.TileRawChNoiseCalibAlg

    acc.addEventAlgo(TileRawChNoiseCalibAlgCfg(**kwargs), primary=True)

    return acc

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

    from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerCfg
    cfg.merge( TileRawChannelMakerCfg(ConfigFlags) )

    cfg.merge( TileRawChNoiseCalibAlgCfg(ConfigFlags, ) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileRawChNoiseCalibAlg.pkl','wb') )

    sc = cfg.run()

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
