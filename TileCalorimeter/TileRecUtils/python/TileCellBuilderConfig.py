# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile Cell builder tool"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileCellBuilderCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile Cell builder tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
        SkipGain - skip given gain. Defaults to -1 [use all gains]. Possible values: 0 [LG], 1 [HG].
    """

    acc = ComponentAccumulator()
    kwargs.setdefault('CheckDCS', flags.Tile.useDCS)
    kwargs.setdefault('TileRawChannelContainer', flags.Tile.RawChannelContainer)
    kwargs.setdefault('SkipGain', -1) # Never skip any gain by default

    if kwargs['SkipGain'] not in [-1, 0, 1]:
        raise(Exception("Invalid Tile gain requsted to be skipped: %s" % kwargs['SkipGain']))

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    acc.merge( TileDQstatusAlgCfg(flags) )

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(flags))

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(flags))

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge(TileCablingSvcCfg(flags))

    if 'TileBadChanTool' not in kwargs:
        from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
        kwargs['TileBadChanTool'] = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )

    if 'TileCondToolEmscale' not in kwargs:
        from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
        kwargs['TileCondToolEmscale'] = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )

    if 'TileCondToolTiming' not in kwargs:
        from TileConditions.TileTimingConfig import TileCondToolTimingCfg
        kwargs['TileCondToolTiming'] = acc.popToolsAndMerge( TileCondToolTimingCfg(flags) )

    if kwargs['CheckDCS'] and 'TileDCSTool' not in kwargs:
        from TileConditions.TileDCSConfig import TileDCSToolCfg
        kwargs['TileDCSTool'] = acc.popToolsAndMerge( TileDCSToolCfg(flags) )

    if 'NoiseFilterTools' not in kwargs:
        from TileRecUtils.TileRawChannelCorrectionConfig import TileRawChannelCorrectionToolsCfg
        correctionTools = acc.popToolsAndMerge( TileRawChannelCorrectionToolsCfg(flags) )
        kwargs['NoiseFilterTools'] = correctionTools

    if len(kwargs['NoiseFilterTools']) > 0:
        if not (flags.Input.isMC or flags.Overlay.DataOverlay) and 'TileDSPRawChannelContainer' not in kwargs:
            from TileRecUtils.TileRawChannelCorrectionConfig import TileRawChannelCorrectionAlgCfg
            corrAlgAcc = TileRawChannelCorrectionAlgCfg(flags)
            tileRawChannelCorrectionAlg = corrAlgAcc.getPrimary()
            tileRawChannelContainerDSP = tileRawChannelCorrectionAlg.OutputRawChannelContainer
            kwargs['TileDSPRawChannelContainer'] = tileRawChannelContainerDSP
            acc.merge( corrAlgAcc )

    TileCellBuilder=CompFactory.TileCellBuilder
    acc.setPrivateTools( TileCellBuilder(**kwargs) )

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
    acc.store( open('TileCellBuilder.pkl','wb') )
