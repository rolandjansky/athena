# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile correction tools and algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TileRawChannelOF1CorrectorCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile OF1 raw channel correction tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    acc = ComponentAccumulator()

    from TileRecUtils.TileRecUtilsConf import TileRawChannelOF1Corrector
    tileRawChannelOF1Corrector = TileRawChannelOF1Corrector()
    tileRawChannelOF1Corrector.CorrectPedestalDifference = flags.Tile.correctPedestalDifference
    tileRawChannelOF1Corrector.ZeroAmplitudeWithoutDigits = flags.Tile.zeroAmplitudeWithoutDigits

    if flags.Tile.correctPedestalDifference:
        from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
        sampleNoiseAcc = TileCondToolNoiseSampleCfg(flags, TileSampleNoise = 'TileSampleNoise',
                                                    TileOnlineSampleNoise = 'TileOnlineSampleNoise')

        sampleNoiseTool = acc.popToolsAndMerge( sampleNoiseAcc )
        tileRawChannelOF1Corrector.TileCondToolNoiseSample = sampleNoiseTool

        from TileConditions.TileTimingConfig import TileCondToolOnlineTimingCfg
        onlineTimingTool = acc.popToolsAndMerge( TileCondToolOnlineTimingCfg(flags) )
        tileRawChannelOF1Corrector.TileCondToolTiming = onlineTimingTool

        from TileConditions.TileOFCConfig import TileCondToolOfcCoolCfg
        ofcOF1CoolTool = acc.popToolsAndMerge( TileCondToolOfcCoolCfg(flags, OfcType = 'OF1') )
        tileRawChannelOF1Corrector.TileCondToolOfc = ofcOF1CoolTool


    if flags.Tile.zeroAmplitudeWithoutDigits:
        from TileConditions.TileDSPThresholdConfig import TileCondToolDspThresholdCfg
        dspThresholdTool = acc.popToolsAndMerge( TileCondToolDspThresholdCfg(flags) )
        tileRawChannelOF1Corrector.TileCondToolDspThreshold = dspThresholdTool

        from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
        emScaleTool = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )
        tileRawChannelOF1Corrector.TileCondToolEmscale = emScaleTool


    acc.setPrivateTools( tileRawChannelOF1Corrector )

    return acc



def TileRawChannelNoiseFilterCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile raw channel noise filter tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    acc = ComponentAccumulator()

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    acc.merge( TileDQstatusAlgCfg(flags) )

    from TileRecUtils.TileRecUtilsConf import TileRawChannelNoiseFilter
    tileRawChannelNoiseFilter = TileRawChannelNoiseFilter()

    from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
    emScaleTool = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )
    tileRawChannelNoiseFilter.TileCondToolEmscale = emScaleTool

    from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
    sampleNoiseTool = acc.popToolsAndMerge( TileCondToolNoiseSampleCfg(flags) )
    tileRawChannelNoiseFilter.TileCondToolNoiseSample = sampleNoiseTool

    from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
    badChanTool = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )
    tileRawChannelNoiseFilter.TileBadChanTool = badChanTool

    acc.setPrivateTools( tileRawChannelNoiseFilter )

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

    inputRawChannelContainer = kwargs.get('InputRawChannelContainer', 'TileRawChannelCnt')
    outputRawChannelContainer = kwargs.get('OutputRawChannelContainer', 'TileRawChannelCntCorrected')

    correctionTools = acc.popToolsAndMerge( TileRawChannelCorrectionToolsCfg(flags) )

    from TileRecUtils.TileRecUtilsConf import TileRawChannelCorrectionAlg
    tileRawChannelCorrectionAlg = TileRawChannelCorrectionAlg(InputRawChannelContainer = inputRawChannelContainer,
                                                              OutputRawChannelContainer = outputRawChannelContainer)

    tileRawChannelCorrectionAlg.NoiseFilterTools = correctionTools

    acc.addEventAlgo(tileRawChannelCorrectionAlg, primary = True)

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

    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    acc = MainServicesSerialCfg()

    from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
    acc.merge( TrigBSReadCfg(ConfigFlags) )
    acc.getService('ByteStreamAddressProviderSvc').TypeNames += ['TileDigitsContainer/TileDigitsCnt']

    acc.merge( TileRawChannelCorrectionAlgCfg(ConfigFlags) )

    ConfigFlags.dump()
    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileRawChannelCorrection.pkl','w') )

    sc = acc.run(maxEvents = 3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())

