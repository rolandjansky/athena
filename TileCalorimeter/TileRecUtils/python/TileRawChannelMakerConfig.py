# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile raw channel maker algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileRawChannelMakerCfg(flags, **kwargs):
    """Return component accumulator with configured Tile raw channel maker algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    acc = ComponentAccumulator()

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )

    kwargs.setdefault('name', 'TileRChMaker')
    name = kwargs['name']

    kwargs.setdefault('TileDigitsContainer', 'TileDigitsCnt')

    from AthenaCommon.Logging import logging
    mlog = logging.getLogger( 'TileRawChannelMakerCfg' )

    if flags.Tile.doOverflowFit:
        kwargs.setdefault('FitOverflow', True)
        from TileRecUtils.TileRawChannelBuilderFitConfig import TileRawChannelBuilderFitOverflowCfg
        tileRawChannelBuilderFitOverflow = acc.popToolsAndMerge( TileRawChannelBuilderFitOverflowCfg(flags) )
        kwargs.setdefault('TileRawChannelBuilderFitOverflow', tileRawChannelBuilderFitOverflow)
    else:
        kwargs.setdefault('FitOverflow', False)

    tileRawChannelBuilder = []

    if flags.Tile.doFit:
        from TileRecUtils.TileRawChannelBuilderFitConfig import TileRawChannelBuilderFitFilterCfg
        tileRawChannelBuilderFitFilter = acc.popToolsAndMerge( TileRawChannelBuilderFitFilterCfg(flags) )
        tileRawChannelBuilder += [tileRawChannelBuilderFitFilter]
        mlog.info(" adding now TileRawChannelBuilderFitFilter with name %s to the algorithm: %s",
                  tileRawChannelBuilderFitFilter.name, name)

    if flags.Tile.doOF1:
        from TileRecUtils.TileRawChannelBuilderOptConfig import TileRawChannelBuilderOF1Cfg
        tileRawChannelBuilderOF1 = acc.popToolsAndMerge( TileRawChannelBuilderOF1Cfg(flags) )
        tileRawChannelBuilder += [tileRawChannelBuilderOF1]
        mlog.info(" adding now TileRawChannelBuilderOpt2Filter with name %s to the algorithm: %s",
                  tileRawChannelBuilderOF1.name, name)

    if flags.Tile.doOpt2:
        from TileRecUtils.TileRawChannelBuilderOptConfig import TileRawChannelBuilderOpt2Cfg
        tileRawChannelBuilderOpt2 = acc.popToolsAndMerge( TileRawChannelBuilderOpt2Cfg(flags) )
        tileRawChannelBuilder += [tileRawChannelBuilderOpt2]
        mlog.info(" adding now TileRawChannelBuilderOpt2Filter with name %s to the algorithm: %s",
                  tileRawChannelBuilderOpt2.name, name)

    if flags.Tile.doOptATLAS:
        from TileRecUtils.TileRawChannelBuilderOptConfig import TileRawChannelBuilderOptATLASCfg
        tileRawChannelBuilderOptATLAS = acc.popToolsAndMerge( TileRawChannelBuilderOptATLASCfg(flags) )
        tileRawChannelBuilder += [tileRawChannelBuilderOptATLAS]
        mlog.info(" adding now TileRawChannelBuilderOpt2Filter with name %s to the algorithm: %s",
                  tileRawChannelBuilderOptATLAS.name, name)

    kwargs.setdefault('TileRawChannelBuilder', tileRawChannelBuilder)

    TileRawChannelMaker=CompFactory.TileRawChannelMaker
    acc.addEventAlgo(TileRawChannelMaker(**kwargs), primary = True)

    return acc


def TileRawChannelMakerDigiHSTruthCfg(flags, **kwargs):
    """Return component accumulator with configured Tile raw channel maker algorithm for HS

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    kwargs.setdefault('name', 'TileRChMaker_DigiHSTruth')
    kwargs.setdefault('TileDigitsContainer', 'TileDigitsCnt_DigiHSTruth')

    acc = TileRawChannelMakerCfg(flags, **kwargs)
    rawChannelMaker = acc.getPrimary()

    rawChannelbuilders = rawChannelMaker.TileRawChannelBuilder

    for rawChannelBuilder in rawChannelbuilders:
        rawChannelContainer = rawChannelBuilder.TileRawChannelContainer
        rawChannelBuilder.TileRawChannelContainer = rawChannelContainer + '_DigiHSTruth'

    return acc


def TileRawChannelOutputCfg(flags, tileRawChannelMaker, streamName):
    """Return component accumulator with configured Output stream for Tile raw channel maker algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
        tileRawChannelMaker -- Tile raw channel maker algorithm
        streamName -- name of output stream.
    """

    outputItemList = []
    rawChannelbuilders = tileRawChannelMaker.TileRawChannelBuilder

    for rawChannelBuilder in rawChannelbuilders:
        rawChannelContainer = rawChannelBuilder.TileRawChannelContainer
        outputItemList += ['TileRawChannelContainer#' + rawChannelContainer]

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc = OutputStreamCfg(flags, streamName, ItemList = outputItemList)

    return acc


def TileRawChannelMakerOutputCfg(flags, streamName = 'ESD', **kwargs):
    """Return component accumulator with configured Tile raw channel maker algorithm and Output stream

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
        streamName -- name of output stream. Defaults to ESD.
    """

    acc = TileRawChannelMakerCfg(flags, **kwargs)
    acc.merge( TileRawChannelOutputCfg(flags, acc.getPrimary(), streamName) )

    return acc


def TileRawChannelMakerDigiHSTruthOutputCfg(flags, streamName = 'ESD', **kwargs):
    """Return component accumulator with configured Tile raw channel maker algorithm and Output stream

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
        streamName -- name of output stream. Defaults to ESD.
    """

    acc = TileRawChannelMakerDigiHSTruthCfg(flags, **kwargs)
    acc.merge( TileRawChannelOutputCfg(flags, acc.getPrimary(), streamName) )

    return acc


if __name__ == "__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG

    # Test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Tile.RunType = 'PHY'
    ConfigFlags.Tile.doFit = True
    ConfigFlags.Tile.doOF1 = True
    ConfigFlags.Tile.doOpt2 = True
    ConfigFlags.Tile.doOptATLAS = True
    ConfigFlags.Tile.correctTimeJumps = True
    ConfigFlags.Tile.NoiseFilter = 1
    ConfigFlags.Output.ESDFileName = "myESD.pool.root"
    ConfigFlags.Exec.MaxEvents=3
    ConfigFlags.fillFromArgs()

    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc.merge( ByteStreamReadCfg(ConfigFlags, ['TileRawChannelContainer/TileRawChannelCnt', 'TileDigitsContainer/TileDigitsCnt']) )

    acc.merge( TileRawChannelMakerOutputCfg(ConfigFlags) )

    ConfigFlags.dump()
    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileRawChannelMaker.pkl','wb') )

    sc = acc.run()

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())

