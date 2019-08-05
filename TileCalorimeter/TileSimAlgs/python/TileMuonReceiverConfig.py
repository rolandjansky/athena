# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile pulse for muon receiver algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def TilePulseForTileMuonReceiverCfg(flags, **kwargs):
    """Return component accumulator with configured Tile muon receiver algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    kwargs.setdefault('name', 'TilePulseForTileMuonReceiver')
    kwargs.setdefault('TileHitContainer', 'TileHitCnt')
    kwargs.setdefault('MuonReceiverRawChannelContainer', 'MuRcvRawChCnt')
    kwargs.setdefault('MaskBadChannels', False)
    kwargs.setdefault('UseCoolPulseShapes', True)
    kwargs.setdefault('UseCoolPedestal', False)

    acc = ComponentAccumulator()

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )
    infoLoader = acc.getService('TileInfoLoader')
    pedestal = infoLoader.getDefaultProperty('MuRcvPed')

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge(TileCablingSvcCfg(flags))

    if 'RndmSvc' not in kwargs:
        from RngComps.RandomServices import RNG
        acc.merge( RNG(flags.Random.Engine) )
        kwargs['RndmSvc'] = acc.getService('AthRNGSvc')

    if 'TileCondToolNoiseSample' not in kwargs:
        from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
        kwargs['TileCondToolNoiseSample'] = acc.popToolsAndMerge(TileCondToolNoiseSampleCfg(flags))

    if 'TileCondToolEmscale' not in kwargs:
        from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
        kwargs['TileCondToolEmscale'] = acc.popToolsAndMerge(TileCondToolEmscaleCfg(flags))

    if kwargs['MaskBadChannels']:
        if 'TileBadChanTool' not in kwargs:
            from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
            badChannelsTool = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )
            kwargs['TileBadChanTool'] = badChannelsTool
    else:
        kwargs['TileBadChanTool'] = None

    if 'TileCondToolPulseShape' not in kwargs:
        from TileConditions.TilePulseShapeConfig import TileCondToolMuRcvPulseShapeCfg
        pulseShapeTool = acc.popToolsAndMerge( TileCondToolMuRcvPulseShapeCfg(flags) )
        if kwargs['UseCoolPulseShapes']:
            kwargs['TileCondToolPulseShape'] = pulseShapeTool
        else:
            kwargs['TileCondToolPulseShape'] = None
    else:
        pulseShapeTool = kwargs['TileCondToolPulseShape']

    if 'TileRawChannelBuilderMF' not in kwargs:
        from TileConditions.TileOFCConfig import TileCondToolOfcCfg
        ofcTool = acc.popToolsAndMerge( TileCondToolOfcCfg(flags,
                                                           OptFilterDeltaCorrelation = True,
                                                           TileCondToolPulseShape = pulseShapeTool) )


        from TileRecUtils.TileRawChannelBuilderMFConfig import TileRawChannelBuilderMFCfg
        rawChanBuilder = acc.popToolsAndMerge( TileRawChannelBuilderMFCfg(flags, MF = 1,
                                                                          PedestalMode = 0,
                                                                          DefaultPedestal = pedestal,
                                                                          TileCondToolOfcOnFly = ofcTool,
                                                                          TileCondToolOfc = ofcTool,
                                                                          TileRawChannelContainer = "") )
        kwargs['TileRawChannelBuilderMF'] = rawChanBuilder


    kwargs.setdefault('IntegerDigits', not flags.Digitization.PileUpPremixing)

    if flags.Digitization.PileUpPremixing:
        kwargs.setdefault('MuonReceiverDigitsContainer', flags.Overlay.BkgPrefix + 'MuRcvDigitsCnt')
    else:
        kwargs.setdefault('MuonReceiverDigitsContainer', 'MuRcvDigitsCnt')

    from TileSimAlgs.TileSimAlgsConf import TilePulseForTileMuonReceiver
    acc.addEventAlgo(TilePulseForTileMuonReceiver(**kwargs))

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
    ConfigFlags.Output.RDOFileName = 'myRDO.pool.root'
    ConfigFlags.IOVDb.GlobalTag = 'OFLCOND-MC16-SDR-16'
    ConfigFlags.Digitization.Pileup = False

    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    acc = ComponentAccumulator()

    print( acc.merge( TilePulseForTileMuonReceiverCfg(ConfigFlags) ) )

    acc.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()
    acc.store( open('TileMuonReceiver.pkl','w') )
