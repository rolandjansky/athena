# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile digits maker algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TileDigitsMakerCfg(flags, **kwargs):
    """Return component accumulator with configured Tile digits maker algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        name -- name of TileDigitsMaker algorithm. Defaults to TileDigitsMaker.
        UseCoolPulseShapes -- flag to use pulse shape from database. Defaults to True.
        RndmEvtOverlay -- flag to add PileUp or noise by overlaying random events.
                          Defaults to Detector.OverlayTile flag.
        MaskBadChannels -- flag to mask channels tagged bad. Defaults to False.
    """

    kwargs.setdefault('name', 'TileDigitsMaker')
    kwargs.setdefault('UseCoolPulseShapes', True)
    kwargs.setdefault('MaskBadChannels', False)
    kwargs.setdefault('RndmEvtOverlay', flags.Detector.OverlayTile)

    acc = ComponentAccumulator()

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    infoLoaderAcc = TileInfoLoaderCfg(flags)
    infoLoader = infoLoaderAcc.getPrimary()
    acc.merge( infoLoaderAcc )

    tileNoise = infoLoader.getDefaultProperty('TileNoise')
    tileCoherNoise = infoLoader.getDefaultProperty('TileCoherNoise')

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge(TileCablingSvcCfg(flags))

    if 'TileCondToolNoiseSample' not in kwargs:
        from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
        kwargs['TileCondToolNoiseSample'] = acc.popToolsAndMerge(TileCondToolNoiseSampleCfg(flags))

    if 'TileCondToolEmscale' not in kwargs:
        from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
        kwargs['TileCondToolEmscale'] = acc.popToolsAndMerge(TileCondToolEmscaleCfg(flags))

    if kwargs['RndmEvtOverlay']:

        tileNoise = False
        tileCoherNoise = False

        from PileUpTools.PileUpToolsConf import PileUpMergeSvc
        acc.addService( PileUpMergeSvc() )

        from TileRecUtils.TileDQstatusConfig import TileDQstatusToolCfg
        kwargs['TileDQstatusTool'] = acc.popToolsAndMerge(TileDQstatusToolCfg(flags))
    else:
        kwargs['TileDQstatusTool'] = None
        kwargs['TileDQstatus'] = ''

    if tileNoise or tileCoherNoise or kwargs['RndmEvtOverlay']:
        if 'RndmSvc' not in kwargs:
            from RngComps.RandomServices import RNG
            acc.merge( RNG(flags.Random.Engine) )
            kwargs['RndmSvc'] = acc.getService('AthRNGSvc')
    else:
        kwargs['RndmSvc'] = None

    if kwargs['UseCoolPulseShapes']:
        if 'TileCondToolPulseShape' not in kwargs:
            from TileConditions.TilePulseShapeConfig import TileCondToolPulseShapeCfg
            pulseShapeTool = acc.popToolsAndMerge( TileCondToolPulseShapeCfg(flags) )
            kwargs['TileCondToolPulseShape'] = pulseShapeTool
    else:
        kwargs['TileCondToolPulseShape'] = None


    if kwargs['MaskBadChannels'] or kwargs['RndmEvtOverlay']:
        if 'TileBadChanTool' not in kwargs:
            from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
            badChannelsTool = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )
            kwargs['TileBadChanTool'] = badChannelsTool
    else:
        kwargs['TileBadChanTool'] = None

    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault('TileDigitsContainer', flags.Overlay.BkgPrefix + 'TileDigitsCnt')
    else:
        kwargs.setdefault('TileDigitsContainer', 'TileDigitsCnt')


    kwargs.setdefault('DoHSTruthReconstruction', flags.Digitization.DoDigiTruth)
    if kwargs['DoHSTruthReconstruction']:
        kwargs.setdefault('TileHitContainer_DigiHSTruth', 'TileHitCnt_DigiHSTruth')
        kwargs.setdefault('TileDigitsContainer_DigiHSTruth', 'TileDigitsCnt_DigiHSTruth')
    else:
        kwargs.setdefault('TileHitContainer_DigiHSTruth', '')
        kwargs.setdefault('TileDigitsContainer_DigiHSTruth', '')


    kwargs.setdefault('IntegerDigits', not flags.Digitization.PileUpPremixing)

    from TileSimAlgs.TileSimAlgsConf import TileDigitsMaker
    digitsMaker = TileDigitsMaker(**kwargs)

    acc.addEventAlgo(digitsMaker, primary = True)

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

    ConfigFlags.Input.Files = defaultTestFiles.HITS
    ConfigFlags.Tile.RunType = 'PHY'

    ConfigFlags.fillFromArgs()

    ConfigFlags.lock()
    ConfigFlags.dump()

    acc = ComponentAccumulator()

    print( acc.popToolsAndMerge( TileDigitsMakerCfg(ConfigFlags) ) )

    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileDigitsMaker.pkl','w') )
