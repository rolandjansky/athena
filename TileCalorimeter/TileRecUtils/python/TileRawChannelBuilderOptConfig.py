# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile raw channel builder tools using Optimal Filtering methods"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TileRawChannelBuilderOpt2FilterCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile raw channel builder tool (Opt2)

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Method -- flavour of Tile Optimal Filtering method. Defaults to Opt2. Possible values: Opt2, OptATLAS, OF1
        CreateContainer - flag ot create output container. Defaults to True.
    """

    method = kwargs.get('Method', 'Opt2')
    if method not in ['Opt2', 'OptATLAS', 'OF1']:
        raise(Exception("Invalid Tile Optimal Filtering method: %s" % method))

    kwargs.setdefault('CreateContainer', True)
    createContainer = kwargs['CreateContainer']

    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderOpt2Filter
    kwargs['TileRawChannelBuilder'] = TileRawChannelBuilderOpt2Filter

    name = 'TileRawChannelBuilder' + method
    kwargs.setdefault('Name', name)

    from TileRawChannelBuilderConfig import TileRawChannelBuilderCfg
    acc = TileRawChannelBuilderCfg(flags, **kwargs)
    tileRawChannelBuilderOpt = acc.getPrimary()

    useIterations = (method == 'Opt2')

    if flags.Tile.OfcFromCOOL:
        ofcType = 'OF2' if method != 'OF1' else 'OF1'
        from TileConditions.TileOFCConfig import TileCondToolOfcCoolCfg
        ofcTool = acc.popToolsAndMerge( TileCondToolOfcCoolCfg(flags, OfcType = ofcType) )
    else:
        from TileConditions.TileOFCConfig import TileCondToolOfcCfg
        ofcTool = acc.popToolsAndMerge( TileCondToolOfcCfg(flags) )

    tileRawChannelBuilderOpt.TileCondToolOfc = ofcTool

    outputContainer = flags.Tile.RawChannelContainer if method == 'OptATLAS' else 'TileRawChannel' + method

    tileRawChannelBuilderOpt.TileRawChannelContainer = outputContainer
    tileRawChannelBuilderOpt.BestPhase = False if useIterations else flags.Tile.BestPhaseFromCOOL
    tileRawChannelBuilderOpt.OF2 = True if method != 'OF1' else False
    tileRawChannelBuilderOpt.MaxIterations = 5 if useIterations else 1
    tileRawChannelBuilderOpt.Minus1Iteration = True if useIterations else False
    tileRawChannelBuilderOpt.AmplitudeCorrection = False if useIterations else flags.Tile.correctAmplitude
    tileRawChannelBuilderOpt.TimeCorrection = False if method != 'OptATLAS' else flags.Tile.correctTimeNI

    if (flags.Tile.BestPhaseFromCOOL and not useIterations) or flags.Tile.correctTime:
        from TileConditions.TileTimingConfig import TileCondToolTimingCfg
        timingTool = acc.popToolsAndMerge( TileCondToolTimingCfg(flags) )
        tileRawChannelBuilderOpt.TileCondToolTiming = timingTool

    if flags.Tile.BestPhaseFromCOOL and not useIterations:
        # Can't correct time and use best phase at the same time without iteration
        tileRawChannelBuilderOpt.correctTime = False
    else:
        tileRawChannelBuilderOpt.correctTime = flags.Tile.correctTime

    if method == 'OptATLAS':
        pedestalMode = 17
    else:
        pedestalMode = -1 if method == 'OF1' else 1

    tileRawChannelBuilderOpt.PedestalMode = pedestalMode

    if pedestalMode == -1: # Use pedestal from conditions DB
        from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
        sampleNoiseTool = acc.popToolsAndMerge( TileCondToolNoiseSampleCfg(flags) )
        tileRawChannelBuilderOpt.TileCondToolNoiseSample = sampleNoiseTool

    acc.setPrivateTools( tileRawChannelBuilderOpt )

    return acc



def TileRawChannelBuilderOF1Cfg(flags):
    return TileRawChannelBuilderOpt2FilterCfg(flags, Method = 'OF1')

def TileRawChannelBuilderOpt2Cfg(flags):
    return TileRawChannelBuilderOpt2FilterCfg(flags, Method = 'Opt2')

def TileRawChannelBuilderOptATLASCfg(flags):
    return TileRawChannelBuilderOpt2FilterCfg(flags, Method = 'OptATLAS')


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

    print( acc.popToolsAndMerge( TileRawChannelBuilderOpt2Cfg(ConfigFlags) ) )
    print( acc.popToolsAndMerge( TileRawChannelBuilderOptATLASCfg(ConfigFlags) ) )
    print( acc.popToolsAndMerge( TileRawChannelBuilderOF1Cfg(ConfigFlags) ) )

    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileRawChannelBuilder.pkl','w') )


