# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile Fit raw channel builder tool"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileRawChannelBuilderFitFilterCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile Fit raw channel builder tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    name = kwargs.pop('name', 'TileRawChannelBuilderFitFilter')
    kwargs.setdefault('TileRawChannelContainer', 'TileRawChannelFit')
    kwargs.setdefault('correctTime', flags.Tile.correctTime)
    kwargs.setdefault('FrameLength', 7)

    acc = ComponentAccumulator()

    if 'TileCondToolNoiseSample' not in kwargs:
        from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
        sampleNoiseTool = acc.popToolsAndMerge( TileCondToolNoiseSampleCfg(flags) )
        kwargs['TileCondToolNoiseSample'] = sampleNoiseTool

    if flags.Tile.correctTime and 'TileCondToolTiming' not in kwargs:
        from TileConditions.TileTimingConfig import TileCondToolTimingCfg
        timingTool = acc.popToolsAndMerge( TileCondToolTimingCfg(flags) )
        kwargs['TileCondToolTiming'] = timingTool

    TileRawChannelBuilderFitFilter=CompFactory.TileRawChannelBuilderFitFilter
    from TileRecUtils.TileRawChannelBuilderConfig import TileRawChannelBuilderCfg
    rawChanBuilder = acc.popToolsAndMerge(TileRawChannelBuilderCfg(flags, name, TileRawChannelBuilderFitFilter, **kwargs))
    acc.setPrivateTools(rawChanBuilder)

    return acc


def TileRawChannelBuilderFitOverflowCfg(flags, **kwargs):
    return TileRawChannelBuilderFitFilterCfg(flags,
                                             name = 'TileRawChannelBuilderFitOverflow',
                                             TileRawChannelContainer = "")


if __name__ == "__main__":

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

    #printing Configurables isn't reliable with GaudiConfig2 
    acc.popToolsAndMerge( TileRawChannelBuilderFitFilterCfg(ConfigFlags) )
    acc.popToolsAndMerge( TileRawChannelBuilderFitOverflowCfg(ConfigFlags) )

    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileRawChannelBuilderFit.pkl','wb') )
