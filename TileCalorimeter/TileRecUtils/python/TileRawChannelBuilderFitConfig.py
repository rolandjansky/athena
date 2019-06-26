# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile Fit raw channel builder tool"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

_runTypes = {'PHY' : 1, 'LAS' : 2, 'BILAS' : 2, 'PED' : 4, 'CIS' : 8, 'MONOCIS' : 8}


def TileRawChannelBuilderFitFilterCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile Fit raw channel builder tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    acc = ComponentAccumulator()

    runType = flags.Tile.RunType
    runType = runType.upper()

    if runType not in _runTypes.keys():
        raise(Exception("Invalid Tile run type: %s" % runType))

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    acc.merge( TileDQstatusAlgCfg(flags) )

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )

    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderFitFilter
    tileRawChannelBuilderFit = TileRawChannelBuilderFitFilter()

    from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
    sampleNoiseTool = acc.popToolsAndMerge( TileCondToolNoiseSampleCfg(flags) )
    tileRawChannelBuilderFit.TileCondToolNoiseSample = sampleNoiseTool

    tileRawChannelBuilderFit.RunType = _runTypes[runType]
    tileRawChannelBuilderFit.calibrateEnergy = flags.Tile.calibrateEnergy
    tileRawChannelBuilderFit.correctTime = flags.Tile.correctTime
    tileRawChannelBuilderFit.FrameLength = 7
    tileRawChannelBuilderFit.TileRawChannelContainer = 'TileRawChannelFit'

    if flags.Tile.correctTime:
        from TileConditions.TileTimingConfig import TileCondToolTimingCfg
        timingTool = acc.popToolsAndMerge( TileCondToolTimingCfg(flags) )
        tileRawChannelBuilderFit.TileCondToolTiming = timingTool

    tileRawChannelContainerDSP = ""
    if flags.Tile.NoiseFilter == 1:
        from TileRecUtils.TileRawChannelCorrectionConfig import TileRawChannelCorrectionToolsCfg
        correctionTools = acc.popToolsAndMerge( TileRawChannelCorrectionToolsCfg(flags) )
        tileRawChannelBuilderFit.NoiseFilterTools = correctionTools

        if not (flags.Input.isMC or flags.Overlay.DataOverlay):
            tileRawChannelContainerDSP = 'TileRawChannelCntCorrected'
            from TileRecUtils.TileRawChannelCorrectionConfig import TileRawChannelCorrectionAlgCfg
            acc.merge( TileRawChannelCorrectionAlgCfg(flags) )

    tileRawChannelBuilderFit.DSPContainer = tileRawChannelContainerDSP

    acc.setPrivateTools( tileRawChannelBuilderFit )

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
    ConfigFlags.Tile.NoiseFilter = 1
    ConfigFlags.lock()

    ConfigFlags.dump()

    acc = ComponentAccumulator()

    print( acc.popToolsAndMerge( TileRawChannelBuilderFitFilterCfg(ConfigFlags) ) )

    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileRawChannelBuilderFit.pkl','w') )
