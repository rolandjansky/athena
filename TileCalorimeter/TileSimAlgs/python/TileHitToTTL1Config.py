# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile hits to TTL1 algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TileHitToTTL1Cfg(flags, **kwargs):
    """Return component accumulator with configured Tile hits to TTL1 algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    kwargs.setdefault('name', 'TileHitToTTL1')
    kwargs.setdefault('TileHitContainer', 'TileHitCnt')

    acc = ComponentAccumulator()

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge(TileCablingSvcCfg(flags))

    if 'RndmSvc' not in kwargs:
        from RngComps.RandomServices import RNG
        acc.merge( RNG(flags.Random.Engine) )
        kwargs['RndmSvc'] = acc.getService('AthRNGSvc')
    
    if 'TileBadChanTool' not in kwargs:
        from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
        badChannelsTool = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )
        kwargs['TileBadChanTool'] = badChannelsTool

    if 'TileCondToolEmscale' not in kwargs:
        from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
        kwargs['TileCondToolEmscale'] = acc.popToolsAndMerge(TileCondToolEmscaleCfg(flags))

    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault('TileTTL1Container', flags.Overlay.BkgPrefix + 'TileTTL1Cnt')
        kwargs.setdefault('TileMBTSTTL1Container', flags.Overlay.BkgPrefix + 'TileTTL1MBTS')
    else:
        kwargs.setdefault('TileTTL1Container', 'TileTTL1Cnt')
        kwargs.setdefault('TileMBTSTTL1Container', 'TileTTL1MBTS')

    from TileSimAlgs.TileSimAlgsConf import TileHitToTTL1
    acc.addEventAlgo(TileHitToTTL1(**kwargs))

    return acc


def TileHitToTTL1CosmicsCfg(flags, **kwargs):
    """Return component accumulator with configured Tile hits to TTL1 algorithm for cosmics

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    kwargs.setdefault('name', 'TileHitToTTL1_Cosmics')
    kwargs.setdefault('TileTTL1Type', 'Cosmics')
    kwargs.setdefault('TileTTL1Container', 'TileTTL1CosmicsCnt')
    kwargs.setdefault('TileMBTSTTL1Container', 'TileMBTSTTL1CosmicsContainer')

    return TileHitToTTL1Cfg(flags, **kwargs)


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
    ConfigFlags.Digitization.Pileup = False

    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    acc = ComponentAccumulator()

    acc.merge( TileHitToTTL1Cfg(ConfigFlags) )
    acc.merge( TileHitToTTL1CosmicsCfg(ConfigFlags) )
    ConfigFlags.dump()

    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileHitToTTL1.pkl','w') )
