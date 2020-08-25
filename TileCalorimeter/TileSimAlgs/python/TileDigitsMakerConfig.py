# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile digits maker algorithm"""

from TileSimAlgs.TileHitVecToCntConfig import TileHitVecToCntCfg
from AthenaConfiguration.ComponentFactory import CompFactory

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
    kwargs.setdefault('OnlyUseContainerName', not flags.Detector.OverlayTile)

    acc = TileHitVecToCntCfg(flags)

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    infoLoaderAcc = TileInfoLoaderCfg(flags)
    infoLoader = infoLoaderAcc.getPrimary()
    acc.merge( infoLoaderAcc )

    infoLoaderProperties = infoLoader._properties.items()

    if 'TileNoise' in infoLoaderProperties:
        tileNoise = infoLoaderProperties['TileNoise']
    else:
        tileNoise = infoLoader._descriptors['TileNoise'].default

    if 'TileCoherNoise' in infoLoaderProperties:
        tileCoherNoise = infoLoaderProperties['TileCoherNoise']
    else:
        tileCoherNoise = infoLoader._descriptors['TileCoherNoise'].default

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

        if flags.Overlay.DataOverlay:
            from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
            acc.merge(ByteStreamReadCfg(flags, type_names=[
                'TileDigitsContainer/' + flags.Overlay.BkgPrefix + 'TileDigitsCnt',
                'TileRawChannelContainer/' + flags.Overlay.BkgPrefix + 'TileRawChannelCnt']
            ))

        from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
        acc.merge(TileDQstatusAlgCfg(flags))

        kwargs['InputTileDigitContainer'] = flags.Overlay.BkgPrefix + 'TileDigitsCnt'
        kwargs['TileDQstatus'] = 'TileDQstatus'

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

    TileDigitsMaker=CompFactory.TileDigitsMaker
    digitsMaker = TileDigitsMaker(**kwargs)

    acc.addEventAlgo(digitsMaker, primary = True)

    return acc

def TileDigitsMakerOutputCfg(flags, **kwargs):
    """Return component accumulator with configured Tile digits maker algorithm and Output Stream

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        name -- name of TileDigitsMaker algorithm. Defaults to TileDigitsMaker.
        UseCoolPulseShapes -- flag to use pulse shape from database. Defaults to True.
        RndmEvtOverlay -- flag to add PileUp or noise by overlaying random events.
                          Defaults to Detector.OverlayTile flag.
        MaskBadChannels -- flag to mask channels tagged bad. Defaults to False.
    """

    acc = TileDigitsMakerCfg(flags, **kwargs)
    tileDigitsMaker = acc.getPrimary()

    if flags.Digitization.PileUpPremixing:
        if hasattr(tileDigitsMaker, 'TileDigitsContainer'):
            tileDigitsContainer = tileDigitsMaker.TileDigitsContainer
        else:
            tileDigitsContainer = tileDigitsMaker.getDefaultProperty('TileDigitsContainer')
    else:
        if hasattr(tileDigitsMaker, 'TileFilteredContainer'):
            tileDigitsContainer = tileDigitsMaker.TileFilteredContainer
        else:
            tileDigitsContainer = tileDigitsMaker.getDefaultProperty('TileFilteredContainer')

    tileDigitsContainer = tileDigitsContainer.split('+').pop()
    if flags.Digitization.AddCaloDigi:
        outputItemList = ['TileDigitsContainer#*']
    else:
        outputItemList = ['TileDigitsContainer#' + tileDigitsContainer]

    if flags.Output.doWriteRDO:
        if flags.Digitization.TruthOutput:
            outputItemList += ["CaloCalibrationHitContainer#*"]
            from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
            acc.merge(TruthDigitizationOutputCfg(flags))
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        acc.merge(  OutputStreamCfg(flags, streamName = 'RDO', ItemList = outputItemList) )

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
    ConfigFlags.Output.RDOFileName = 'myRDO.pool.root'
    ConfigFlags.IOVDb.GlobalTag = 'OFLCOND-MC16-SDR-16'
    ConfigFlags.Digitization.Pileup = False

    ConfigFlags.fillFromArgs()

    ConfigFlags.lock()
    ConfigFlags.dump()

    # Construct our accumulator to run
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(ConfigFlags))

    acc.merge( TileDigitsMakerOutputCfg(ConfigFlags) )

    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileDigitsMaker.pkl','wb') )

    sc = acc.run(maxEvents=3)
    # Success should be 0
    import sys
    sys.exit(not sc.isSuccess())

