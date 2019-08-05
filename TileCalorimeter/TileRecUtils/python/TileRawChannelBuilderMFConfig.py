# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile MF raw channel builder tool"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TileRawChannelBuilderMFCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile MF raw channel builder tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    kwargs.setdefault('Name', 'TileRawChannelBuilderMF')
    kwargs.setdefault('TileRawChannelContainer', 'TileRawChannelMF')

    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderMF
    kwargs['TileRawChannelBuilder'] = TileRawChannelBuilderMF

    if 'TileCondToolOfcOnFly' not in kwargs:
        from TileConditions.TileOFCConfig import TileCondToolOfcCfg
        ofcTool = acc.popToolsAndMerge( TileCondToolOfcCfg(flags) )
        kwargs['TileCondToolOfcOnFly'] = ofcTool

    if flags.Tile.OfcFromCOOL and 'TileCondToolOfc' not in kwargs:
        from TileConditions.TileOFCConfig import TileCondToolOfcCoolCfg
        ofcCoolTool = acc.popToolsAndMerge( TileCondToolOfcCoolCfg(flags, OfcType = 'OF2') )
        kwargs['TileCondToolOfc'] = ofcCoolTool

    if 'TileCondToolNoiseSample' not in kwargs:
        from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
        sampleNoiseTool = acc.popToolsAndMerge( TileCondToolNoiseSampleCfg(flags) )
        kwargs['TileCondToolNoiseSample'] = sampleNoiseTool

    if flags.Tile.BestPhaseFromCOOL: # can't correct time and use best phase at the same time
        kwargs['correctTime'] = False
    else:
        kwargs.setdefault('correctTime', flags.Tile.correctTime)

    kwargs.setdefault('BestPhase', flags.Tile.BestPhaseFromCOOL)
    kwargs.setdefault('MaxIterations', 5) # iterative mode on
    kwargs.setdefault('AmplitudeCorrection', False)
    kwargs.setdefault('TimeFromCOF', False)

    if flags.Tile.correctTime and 'TileCondToolTiming' not in kwargs:
        from TileConditions.TileTimingConfig import TileCondToolTimingCfg
        timingTool = acc.popToolsAndMerge( TileCondToolTimingCfg(flags) )
        kwargs['TileCondToolTiming'] = timingTool

    from TileRecUtils.TileRawChannelBuilderConfig import TileRawChannelBuilderCfg
    return TileRawChannelBuilderCfg(flags, **kwargs)



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

    print( acc.popToolsAndMerge( TileRawChannelBuilderMFCfg(ConfigFlags) ) )

    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileRawChannelBuilderMF.pkl','w') )
