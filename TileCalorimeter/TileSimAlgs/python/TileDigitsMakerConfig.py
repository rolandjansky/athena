# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile digits maker algorithm"""

from TileSimAlgs.TileHitVecToCntConfig import TileHitVecToCntCfg
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep

def TileDigitsMakerCfg(flags, **kwargs):
    """Return component accumulator with configured Tile digits maker algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        name -- name of TileDigitsMaker algorithm. Defaults to TileDigitsMaker.
        UseCoolPulseShapes -- flag to use pulse shape from database. Defaults to True.
        RndmEvtOverlay -- flag to add PileUp or noise by overlaying random events.
                          True if Common.ProductionStep equals to ProductionStep.Overlay.
        MaskBadChannels -- flag to mask channels tagged bad. Defaults to False.
    """

    kwargs.setdefault('name', 'TileDigitsMaker')
    kwargs.setdefault('UseCoolPulseShapes', True)
    kwargs.setdefault('MaskBadChannels', False)
    kwargs.setdefault('RndmEvtOverlay', flags.Common.ProductionStep == ProductionStep.Overlay)
    kwargs.setdefault('OnlyUseContainerName', flags.Common.ProductionStep != ProductionStep.Overlay)

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

    from TileConditions.TileSampleNoiseConfig import TileSampleNoiseCondAlgCfg
    acc.merge( TileSampleNoiseCondAlgCfg(flags) )

    from TileConditions.TileEMScaleConfig import TileEMScaleCondAlgCfg
    acc.merge( TileEMScaleCondAlgCfg(flags) )

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
            from RngComps.RandomServices import AthRNGSvcCfg
            kwargs['RndmSvc'] = acc.getPrimaryAndMerge( AthRNGSvcCfg(flags) ).name
    else:
        kwargs['RndmSvc'] = None

    if kwargs['UseCoolPulseShapes']:
        from TileConditions.TilePulseShapeConfig import TilePulseShapeCondAlgCfg
        acc.merge( TilePulseShapeCondAlgCfg(flags) )

    if kwargs['MaskBadChannels'] or kwargs['RndmEvtOverlay']:
        from TileConditions.TileBadChannelsConfig import TileBadChannelsCondAlgCfg
        acc.merge( TileBadChannelsCondAlgCfg(flags) )

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
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


    kwargs.setdefault('IntegerDigits', flags.Common.ProductionStep != ProductionStep.PileUpPresampling)

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
                          True if Common.ProductionStep equals to ProductionStep.Overlay.
        MaskBadChannels -- flag to mask channels tagged bad. Defaults to False.
    """

    acc = TileDigitsMakerCfg(flags, **kwargs)
    tileDigitsMaker = acc.getPrimary()

    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        if hasattr(tileDigitsMaker, 'TileDigitsContainer'):
            tileDigitsContainer = tileDigitsMaker.TileDigitsContainer
        else:
            tileDigitsContainer = tileDigitsMaker.getDefaultProperty('TileDigitsContainer')
    else:
        if hasattr(tileDigitsMaker, 'TileFilteredContainer'):
            tileDigitsContainer = tileDigitsMaker.TileFilteredContainer
        else:
            tileDigitsContainer = tileDigitsMaker.getDefaultProperty('TileFilteredContainer')

    tileDigitsContainer = str(tileDigitsContainer).split('+').pop()
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

    ConfigFlags.Input.Files = defaultTestFiles.HITS_RUN2
    ConfigFlags.Tile.RunType = 'PHY'
    ConfigFlags.Output.RDOFileName = 'myRDO.pool.root'
    ConfigFlags.IOVDb.GlobalTag = 'OFLCOND-MC16-SDR-16'
    ConfigFlags.Digitization.PileUp = False

    ConfigFlags.fillFromArgs()

    ConfigFlags.lock()
    ConfigFlags.dump()

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

    acc.merge( TileDigitsMakerOutputCfg(ConfigFlags) )

    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileDigitsMaker.pkl','wb') )

    sc = acc.run(maxEvents=3)
    # Success should be 0
    import sys
    sys.exit(not sc.isSuccess())

