# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile correction tools and algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileRawChannelOF1CorrectorCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile OF1 raw channel correction tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    acc = ComponentAccumulator()

    kwargs.setdefault('CorrectPedestalDifference', flags.Tile.correctPedestalDifference)
    kwargs.setdefault('ZeroAmplitudeWithoutDigits', flags.Tile.zeroAmplitudeWithoutDigits)
    kwargs.setdefault('TileDigitsContainer', 'TileDigitsCnt')

    if kwargs['CorrectPedestalDifference']:
        if 'TileCondToolNoiseSample' not in kwargs:
            from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
            sampleNoiseAcc = TileCondToolNoiseSampleCfg(flags, TileSampleNoise = 'TileSampleNoise',
                                                        TileOnlineSampleNoise = 'TileOnlineSampleNoise')

            kwargs['TileCondToolNoiseSample'] = acc.popToolsAndMerge( sampleNoiseAcc )

        if 'TileCondToolTiming' not in kwargs:
            from TileConditions.TileTimingConfig import TileCondToolOnlineTimingCfg
            kwargs['TileCondToolTiming'] = acc.popToolsAndMerge( TileCondToolOnlineTimingCfg(flags) )

        if 'TileCondToolOfc' not in kwargs:
            from TileConditions.TileOFCConfig import TileCondToolOfcCoolCfg
            kwargs['TileCondToolOfc'] = acc.popToolsAndMerge( TileCondToolOfcCoolCfg(flags, OfcType = 'OF1') )

    if kwargs['ZeroAmplitudeWithoutDigits']:

        if 'TileCondToolDspThreshold' not in kwargs:
            from TileConditions.TileDSPThresholdConfig import TileCondToolDspThresholdCfg
            kwargs['TileCondToolDspThreshold'] = acc.popToolsAndMerge( TileCondToolDspThresholdCfg(flags) )

        if 'TileCondToolEmscale' not in kwargs:
            from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
            kwargs['TileCondToolEmscale'] = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )


    TileRawChannelOF1Corrector=CompFactory.TileRawChannelOF1Corrector
    acc.setPrivateTools( TileRawChannelOF1Corrector(**kwargs) )

    return acc



def TileRawChannelNoiseFilterCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile raw channel noise filter tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    acc = ComponentAccumulator()

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    acc.merge( TileDQstatusAlgCfg(flags) )

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )

    if 'TileCondToolEmscale' not in kwargs:
        from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
        emScaleTool = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )
        kwargs['TileCondToolEmscale'] = emScaleTool

    if 'TileCondToolNoiseSample' not in kwargs:
        from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
        sampleNoiseTool = acc.popToolsAndMerge( TileCondToolNoiseSampleCfg(flags) )
        kwargs['TileCondToolNoiseSample'] = sampleNoiseTool

    if 'TileBadChanTool' not in kwargs:
        from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
        badChanTool = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )
        kwargs['TileBadChanTool'] = badChanTool

    TileRawChannelNoiseFilter=CompFactory.TileRawChannelNoiseFilter
    acc.setPrivateTools( TileRawChannelNoiseFilter(**kwargs) )

    return acc



def TileRawChannelCorrectionToolsCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile raw channel correction tools

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    acc = ComponentAccumulator()

    noiseFilterTools = []

    if flags.Tile.correctPedestalDifference or flags.Tile.zeroAmplitudeWithoutDigits:
        noiseFilterTools += [ acc.popToolsAndMerge( TileRawChannelOF1CorrectorCfg(flags) ) ]

    noiseFilterTools += [ acc.popToolsAndMerge( TileRawChannelNoiseFilterCfg(flags) ) ]

    acc.setPrivateTools( noiseFilterTools )

    return acc


def TileRawChannelCorrectionAlgCfg(flags, **kwargs):
    """Return component accumulator with configured Tile raw channel correction algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)

    Keyword arguments:
        InputRawChannelContainer -- input Tile raw channel container. Defaults to TileRawChannelCnt.
        OutputRawChannelContainer -- output Tile raw channel container. Defaults to TileRawChannelCntCorrected.
    """

    acc = ComponentAccumulator()

    kwargs.setdefault('InputRawChannelContainer', 'TileRawChannelCnt')
    kwargs.setdefault('OutputRawChannelContainer', 'TileRawChannelCntCorrected')

    if 'NoiseFilterTools' not in kwargs:
        kwargs['NoiseFilterTools'] = acc.popToolsAndMerge( TileRawChannelCorrectionToolsCfg(flags) )

    TileRawChannelCorrectionAlg=CompFactory.TileRawChannelCorrectionAlg
    acc.addEventAlgo(TileRawChannelCorrectionAlg(**kwargs), primary = True)

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
    ConfigFlags.Tile.correctPedestalDifference = True
    ConfigFlags.Tile.zeroAmplitudeWithoutDigits = True
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc.merge( ByteStreamReadCfg(ConfigFlags, ['TileRawChannelContainer/TileRawChannelCnt', 'TileDigitsContainer/TileDigitsCnt']) )

    acc.merge( TileRawChannelCorrectionAlgCfg(ConfigFlags) )

    ConfigFlags.dump()
    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileRawChannelCorrection.pkl','wb') )

    sc = acc.run(maxEvents = 3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())

