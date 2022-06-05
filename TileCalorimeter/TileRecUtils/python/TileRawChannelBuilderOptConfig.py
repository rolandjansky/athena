# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile raw channel builder tools using Optimal Filtering methods"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileRawChannelBuilderOpt2FilterCfg(flags, method = 'Opt2', **kwargs):
    """Return component accumulator with configured private Tile raw channel builder tool (Opt2)

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
        method -- flavour of Tile Optimal Filtering method. Defaults to Opt2. Possible values: Opt2, OptATLAS, OF1
    """

    if method not in ['Opt2', 'OptATLAS', 'OF1']:
        raise(Exception("Invalid Tile Optimal Filtering method: %s" % method))

    name = kwargs.pop('name', 'TileRawChannelBuilder' + method)
    useIterations = (method == 'Opt2')

    acc = ComponentAccumulator()

    if 'TileCondToolOfc' not in kwargs:
        if flags.Tile.OfcFromCOOL:
            ofcType = 'OF2' if method != 'OF1' else 'OF1'
            from TileConditions.TileOFCConfig import TileCondToolOfcCoolCfg
            kwargs['TileCondToolOfc'] = acc.popToolsAndMerge( TileCondToolOfcCoolCfg(flags, OfcType = ofcType) )
        else:
            from TileConditions.TileOFCConfig import TileCondToolOfcCfg
            kwargs['TileCondToolOfc'] = acc.popToolsAndMerge( TileCondToolOfcCfg(flags) )

    outputContainer = flags.Tile.RawChannelContainer if method == 'OptATLAS' else 'TileRawChannel' + method
    kwargs.setdefault('TileRawChannelContainer', outputContainer)

    kwargs['BestPhase'] = False if useIterations else flags.Tile.BestPhaseFromCOOL
    kwargs['OF2'] = True if method != 'OF1' else False
    kwargs['MaxIterations'] = 5 if useIterations else 1
    kwargs['Minus1Iteration'] = True if useIterations else False
    kwargs['AmplitudeCorrection'] = False if useIterations else flags.Tile.correctAmplitude
    kwargs['TimeCorrection'] = False if method != 'OptATLAS' else flags.Tile.correctTimeNI

    if (flags.Tile.BestPhaseFromCOOL and not useIterations) or flags.Tile.correctTime:
        if 'TileCondToolTiming' not in kwargs:
            from TileConditions.TileTimingConfig import TileCondToolTimingCfg
            kwargs['TileCondToolTiming'] = acc.popToolsAndMerge( TileCondToolTimingCfg(flags) )

    if flags.Tile.BestPhaseFromCOOL and not useIterations:
        # Can't correct time and use best phase at the same time without iteration
        kwargs['correctTime'] = False
    else:
        kwargs.setdefault('correctTime', flags.Tile.correctTime)

    if method == 'OptATLAS':
        pedestalMode = 17
    else:
        pedestalMode = -1 if method == 'OF1' else 1

    kwargs.setdefault('PedestalMode', pedestalMode)

    if kwargs['PedestalMode'] == -1 and 'TileCondToolNoiseSample' not in kwargs:
        # Use pedestal from conditions DB
        from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
        sampleNoiseTool = acc.popToolsAndMerge( TileCondToolNoiseSampleCfg(flags) )
        kwargs['TileCondToolNoiseSample'] = sampleNoiseTool

    TileRawChannelBuilderOpt2Filter=CompFactory.TileRawChannelBuilderOpt2Filter
    from TileRecUtils.TileRawChannelBuilderConfig import TileRawChannelBuilderCfg
    builderTool = acc.popToolsAndMerge( TileRawChannelBuilderCfg(flags, name, TileRawChannelBuilderOpt2Filter, **kwargs) )

    acc.setPrivateTools( builderTool )
    return acc



def TileRawChannelBuilderOF1Cfg(flags):
    return TileRawChannelBuilderOpt2FilterCfg(flags, method = 'OF1')

def TileRawChannelBuilderOpt2Cfg(flags):
    return TileRawChannelBuilderOpt2FilterCfg(flags, method = 'Opt2')

def TileRawChannelBuilderOptATLASCfg(flags):
    return TileRawChannelBuilderOpt2FilterCfg(flags, method = 'OptATLAS')


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

    #Comment for now, priting configurables isn't reliable with GaudiConfig2
    acc.popToolsAndMerge( TileRawChannelBuilderOpt2Cfg(ConfigFlags) ) 
    acc.popToolsAndMerge( TileRawChannelBuilderOptATLASCfg(ConfigFlags) ) 
    acc.popToolsAndMerge( TileRawChannelBuilderOF1Cfg(ConfigFlags) ) 

    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileRawChannelBuilder.pkl','wb') )


