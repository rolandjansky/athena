# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile raw channel maker algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TileRawChannelMakerCfg(flags, **kwargs):
    """Return component accumulator with configured Tile raw channel maker algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    acc = ComponentAccumulator()

    from AthenaCommon.Logging import logging
    mlog = logging.getLogger( 'TileRawChannelMakerCfg' )

    from TileRecUtils.TileRecUtilsConf import TileRawChannelMaker
    tileRawChannelMaker = TileRawChannelMaker("TileRChMaker")

    if flags.Tile.doOverflowFit or flags.Tile.doFit:
        from TileRecUtils.TileRawChannelBuilderFitConfig import TileRawChannelBuilderFitFilterCfg
        tileRawChannelBuilderFitFilter = acc.popToolsAndMerge( TileRawChannelBuilderFitFilterCfg(flags) )

    if flags.Tile.doOverflowFit:
        tileRawChannelMaker.FitOverflow = True
        tileRawChannelMaker.TileRawChannelBuilderFitOverflow = tileRawChannelBuilderFitFilter
    else:
        tileRawChannelMaker.FitOverflow = False

    if flags.Tile.doFit:
        tileRawChannelMaker.TileRawChannelBuilder += [tileRawChannelBuilderFitFilter]
        mlog.info(" adding now TileRawChannelBuilderFitFilter with name %s to the algorithm: %s",
                  tileRawChannelBuilderFitFilter.name(), tileRawChannelMaker.name())

    if flags.Tile.doOF1:
        from TileRecUtils.TileRawChannelBuilderOptConfig import TileRawChannelBuilderOF1Cfg
        tileRawChannelBuilderOF1 = acc.popToolsAndMerge( TileRawChannelBuilderOF1Cfg(flags) )
        tileRawChannelMaker.TileRawChannelBuilder += [tileRawChannelBuilderOF1]
        mlog.info(" adding now TileRawChannelBuilderOpt2Filter with name %s to the algorithm: %s",
                  tileRawChannelBuilderOF1.name(), tileRawChannelMaker.name())

    if flags.Tile.doOpt2:
        from TileRecUtils.TileRawChannelBuilderOptConfig import TileRawChannelBuilderOpt2Cfg
        tileRawChannelBuilderOpt2 = acc.popToolsAndMerge( TileRawChannelBuilderOpt2Cfg(flags) )
        tileRawChannelMaker.TileRawChannelBuilder += [tileRawChannelBuilderOpt2]
        mlog.info(" adding now TileRawChannelBuilderOpt2Filter with name %s to the algorithm: %s",
                  tileRawChannelBuilderOpt2.name(), tileRawChannelMaker.name())

    if flags.Tile.doOptATLAS:
        from TileRecUtils.TileRawChannelBuilderOptConfig import TileRawChannelBuilderOptATLASCfg
        tileRawChannelBuilderOptATLAS = acc.popToolsAndMerge( TileRawChannelBuilderOptATLASCfg(flags) )
        tileRawChannelMaker.TileRawChannelBuilder += [tileRawChannelBuilderOptATLAS]
        mlog.info(" adding now TileRawChannelBuilderOpt2Filter with name %s to the algorithm: %s",
                  tileRawChannelBuilderOptATLAS.name(), tileRawChannelMaker.name())


    acc.addEventAlgo(tileRawChannelMaker, primary = True)

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
    ConfigFlags.Tile.NoiseFilter = 1
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    acc = MainServicesSerialCfg()

    from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
    acc.merge( TrigBSReadCfg(ConfigFlags) )
    acc.getService('ByteStreamAddressProviderSvc').TypeNames += ['TileDigitsContainer/TileDigitsCnt']

    acc.merge( TileRawChannelMakerCfg(ConfigFlags) )

    ConfigFlags.dump()
    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileRawChannelMaker.pkl','w') )

    sc = acc.run(maxEvents = 3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())

