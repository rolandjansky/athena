# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile Cell builder tool"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TileCellBuilderCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile Cell builder tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
        SkipGain - skip given gain. Defaults to -1 [use all gains]. Possible values: 0 [LG], 1 [HG].
    """

    acc = ComponentAccumulator()

    skipGain = kwargs.get('SkipGain', -1) # Never skip any gain by default
    if skipGain not in [-1, 0, 1]:
        raise(Exception("Invalid Tile gain requsted to be skipped: %s" % skipGain))

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    acc.merge( TileDQstatusAlgCfg(flags) )

    from TileRecUtils.TileRecUtilsConf import TileCellBuilder
    tileCellBuilder = TileCellBuilder(SkipGain = skipGain)

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(flags))

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(flags))

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge(TileCablingSvcCfg(flags))

    from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
    badChanTool = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )
    tileCellBuilder.TileBadChanTool = badChanTool

    from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
    emScaleTool = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )
    tileCellBuilder.TileCondToolEmscale = emScaleTool

    from TileConditions.TileTimingConfig import TileCondToolTimingCfg
    timingTool = acc.popToolsAndMerge( TileCondToolTimingCfg(flags) )
    tileCellBuilder.TileCondToolTiming = timingTool

    if flags.Tile.useDCS:
        from TileConditions.TileDCSConfig import TileDCSToolCfg
        dcsTool = acc.popToolsAndMerge( TileDCSToolCfg(flags) )
        tileCellBuilder.TileDCSTool = dcsTool
        tileCellBuilder.CheckDCS = True

    tileCellBuilder.TileRawChannelContainer = flags.Tile.RawChannelContainer

    if flags.Tile.NoiseFilter == 1:
        if not (flags.Input.isMC or flags.Overlay.DataOverlay):
            from TileRecUtils.TileRawChannelCorrectionConfig import TileRawChannelCorrectionAlgCfg
            corrAlgAcc = TileRawChannelCorrectionAlgCfg(flags)
            tileRawChannelCorrectionAlg = corrAlgAcc.getPrimary()
            tileRawChannelContainerDSP = tileRawChannelCorrectionAlg.OutputRawChannelContainer
            tileCellBuilder.TileDSPRawChannelContainer = tileRawChannelContainerDSP
            acc.merge( corrAlgAcc )

    acc.setPrivateTools( tileCellBuilder )

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

    ConfigFlags.fillFromArgs()

    ConfigFlags.lock()

    acc = ComponentAccumulator()

    print( acc.popToolsAndMerge( TileCellBuilderCfg(ConfigFlags) ) )

    ConfigFlags.dump()
    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileCellBuilder.pkl','w') )
