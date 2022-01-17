# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile hits to TTL1 algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from TileSimAlgs.TileHitVecToCntConfig import TileHitVecToCntCfg


def TileHitToTTL1Cfg(flags, **kwargs):
    """Return component accumulator with configured Tile hits to TTL1 algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    kwargs.setdefault('name', 'TileHitToTTL1')
    kwargs.setdefault('TileHitContainer', 'TileHitCnt')

    acc = TileHitVecToCntCfg(flags)

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge(TileCablingSvcCfg(flags))

    if 'RndmSvc' not in kwargs:
        from RngComps.RandomServices import AthRNGSvcCfg
        kwargs['RndmSvc'] = acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name

    if 'TileBadChanTool' not in kwargs:
        from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
        badChannelsTool = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )
        kwargs['TileBadChanTool'] = badChannelsTool

    if 'TileCondToolEmscale' not in kwargs:
        from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
        kwargs['TileCondToolEmscale'] = acc.popToolsAndMerge(TileCondToolEmscaleCfg(flags))

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault('TileTTL1Container', flags.Overlay.BkgPrefix + 'TileTTL1Cnt')
        if flags.Detector.EnableMBTS:
            kwargs.setdefault('TileMBTSTTL1Container', flags.Overlay.BkgPrefix + 'TileTTL1MBTS')
        else:
            kwargs.setdefault('TileMBTSTTL1Container', '')
    elif flags.Common.ProductionStep == ProductionStep.Overlay:
        kwargs.setdefault('TileTTL1Container', flags.Overlay.SigPrefix + 'TileTTL1Cnt')
        if flags.Detector.EnableMBTS:
            kwargs.setdefault('TileMBTSTTL1Container', flags.Overlay.SigPrefix + 'TileTTL1MBTS')
        else:
            kwargs.setdefault('TileMBTSTTL1Container', '')
    else:
        kwargs.setdefault('TileTTL1Container', 'TileTTL1Cnt')
        if flags.Detector.EnableMBTS:
            kwargs.setdefault('TileMBTSTTL1Container', 'TileTTL1MBTS')
        else:
            kwargs.setdefault('TileMBTSTTL1Container', '')

    TileHitToTTL1=CompFactory.TileHitToTTL1
    acc.addEventAlgo(TileHitToTTL1(**kwargs), primary = True)

    return acc


def TileHitToTTL1CosmicsCfg(flags, **kwargs):
    """Return component accumulator with configured Tile hits to TTL1 algorithm for cosmics

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    kwargs.setdefault('name', 'TileHitToTTL1_Cosmics')
    kwargs.setdefault('TileTTL1Type', 'Cosmics')
    kwargs.setdefault('TileTTL1Container', 'TileTTL1CosmicsCnt')
    if flags.Detector.EnableMBTS:
        kwargs.setdefault('TileMBTSTTL1Container', 'TileMBTSTTL1CosmicsContainer')
    else:
        kwargs.setdefault('TileMBTSTTL1Container', '')

    return TileHitToTTL1Cfg(flags, **kwargs)


def TileTTL1OutputCfg(flags, TileHitToTTL1):

    if hasattr(TileHitToTTL1, 'TileTTL1Container'):
        tileTTL1Container = TileHitToTTL1.TileTTL1Container
    else:
        tileTTL1Container = TileHitToTTL1.getDefaultProperty('TileTTL1Container')
    tileTTL1Container = str(tileTTL1Container).split('+').pop()
    outputItemList = ['TileTTL1Container#' + tileTTL1Container]

    if hasattr(TileHitToTTL1, 'TileMBTSTTL1Container'):
        mbtsTTL1Container = TileHitToTTL1.TileMBTSTTL1Container
    else:
        mbtsTTL1Container = TileHitToTTL1.getDefaultProperty('TileMBTSTTL1Container')
    mbtsTTL1Container = str(mbtsTTL1Container).split('+').pop()
    outputItemList += ['TileTTL1Container#' + mbtsTTL1Container]

    acc = ComponentAccumulator()
    if flags.Output.doWriteRDO:
        if flags.Digitization.TruthOutput:
            outputItemList += ["CaloCalibrationHitContainer#*"]
            from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
            acc.merge(TruthDigitizationOutputCfg(flags))
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(OutputStreamCfg(flags, streamName = 'RDO', ItemList = outputItemList))

    return acc


def TileHitToTTL1OutputCfg(flags, **kwargs):    
    """Return component accumulator with configured Tile hits to TTL1 algorithm and Output Stream

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    acc = TileHitToTTL1Cfg(flags, **kwargs)
    acc.merge( TileTTL1OutputCfg(flags, acc.getPrimary()) )

    return acc


def TileHitToTTL1CosmicsOutputCfg(flags, **kwargs):
    """Return component accumulator with configured Tile hits to TTL1 algorithm for cosmics and Output Stream

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    acc = TileHitToTTL1CosmicsCfg(flags, **kwargs)
    acc.merge( TileTTL1OutputCfg(flags, acc.getPrimary()) )

    return acc


if __name__ == "__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG

    # Test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.HITS
    ConfigFlags.IOVDb.GlobalTag = 'OFLCOND-MC16-SDR-16'
    ConfigFlags.Digitization.PileUp = False
    ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    # Construct our accumulator to run
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(ConfigFlags))

    if 'EventInfo' not in ConfigFlags.Input.Collections:
        from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
        acc.merge(EventInfoCnvAlgCfg(ConfigFlags,
                                     inputKey='McEventInfo',
                                     outputKey='EventInfo'))

    acc.merge( TileHitToTTL1OutputCfg(ConfigFlags) )
    ConfigFlags.dump()

    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileHitToTTL1.pkl','wb') )

    sc = acc.run(maxEvents=3)
    # Success should be 0
    import sys
    sys.exit(not sc.isSuccess())

