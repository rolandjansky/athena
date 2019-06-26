# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile raw channel builder tools using Optimal Filtering methods"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

_runTypes = {'PHY' : 1, 'LAS' : 2, 'BILAS' : 2, 'PED' : 4, 'CIS' : 8, 'MONOCIS' : 8}

def TileRawChannelBuilderOpt2FilterCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile raw channel builder tool (Opt2)

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
        Method -- flavour of Tile Optimal Filtering method. Defaults to Opt2. Possible values: Opt2, OptATLAS, OF1
    """

    acc = ComponentAccumulator()

    method = kwargs.get('Method', 'Opt2')
    if method not in ['Opt2', 'OptATLAS', 'OF1']:
        raise(Exception("Invalid Tile Optimal Filtering method: %s" % method))

    name = 'TileRawChannelBuilder' + method

    runType = flags.Tile.RunType
    runType = runType.upper()

    if runType not in _runTypes.keys():
        raise(Exception("Invalid Tile run type: %s" % runType))

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    acc.merge( TileDQstatusAlgCfg(flags) )

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )

    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderOpt2Filter
    tileRawChannelBuilderOpt = TileRawChannelBuilderOpt2Filter(name)

    if flags.Tile.OfcFromCOOL:
        ofcType = 'OF2' if method != 'OF1' else 'OF1'
        from TileConditions.TileOFCConfig import TileCondToolOfcCoolCfg
        ofcTool = acc.popToolsAndMerge( TileCondToolOfcCoolCfg(flags, OfcType = ofcType) )
    else:
        from TileConditions.TileOFCConfig import TileCondToolOfcCfg
        ofcTool = acc.popToolsAndMerge( TileCondToolOfcCfg(flags) )

    tileRawChannelBuilderOpt.TileCondToolOfc = ofcTool

    outputContainer = 'TileRawChannel' + {'OptATLAS':flags.Tile.RawChannelContainer}.get(method, method)

    tileRawChannelBuilderOpt.TileRawChannelContainer = outputContainer
    tileRawChannelBuilderOpt.RunType = _runTypes[runType]
    tileRawChannelBuilderOpt.calibrateEnergy = False
    tileRawChannelBuilderOpt.BestPhase = False if method == 'Opt2' else flags.Tile.BestPhaseFromCOOL
    tileRawChannelBuilderOpt.OF2 = True if method != 'OF1' else False
    tileRawChannelBuilderOpt.MaxIterations = 5 if method == 'Opt2' else 1
    tileRawChannelBuilderOpt.Minus1Iteration = True if method == 'Opt2' else False
    tileRawChannelBuilderOpt.AmplitudeCorrection = False if method == 'Opt2' else flags.Tile.correctAmplitude
    tileRawChannelBuilderOpt.TimeCorrection = False if method != 'OptATLAS' else flags.Tile.correctTimeNI
    tileRawChannelBuilderOpt.AmpMinForAmpCorrection = flags.Tile.AmpMinForAmpCorrection
    tileRawChannelBuilderOpt.TimeMinForAmpCorrection = flags.Tile.TimeMinForAmpCorrection
    tileRawChannelBuilderOpt.TimeMaxForAmpCorrection = flags.Tile.TimeMaxForAmpCorrection

    if (flags.Tile.BestPhaseFromCOOL and method != 'Opt2') or flags.Tile.correctTime:
        from TileConditions.TileTimingConfig import TileCondToolTimingCfg
        timingTool = acc.popToolsAndMerge( TileCondToolTimingCfg(flags) )
        tileRawChannelBuilderOpt.TileCondToolTiming = timingTool

    if flags.Tile.BestPhaseFromCOOL and method != 'Opt2':
        # Can't correct time and use best phase at the same time without iteration
        tileRawChannelBuilderOpt.correctTime = False
    else:
        tileRawChannelBuilderOpt.correctTime = flags.Tile.correctTime

    if method == 'OF1':
        tileRawChannelBuilderOpt.PedestalMode = -1

    tileRawChannelContainerDSP = ""
    if flags.Tile.NoiseFilter == 1:
        from TileRecUtils.TileRawChannelCorrectionConfig import TileRawChannelCorrectionToolsCfg
        correctionTools = acc.popToolsAndMerge( TileRawChannelCorrectionToolsCfg(flags) )
        tileRawChannelBuilderOpt.NoiseFilterTools = correctionTools

        if not (flags.Input.isMC or flags.Overlay.DataOverlay):
            tileRawChannelContainerDSP = 'TileRawChannelCntCorrected'
            from TileRecUtils.TileRawChannelCorrectionConfig import TileRawChannelCorrectionAlgCfg
            acc.merge( TileRawChannelCorrectionAlgCfg(flags) )

    tileRawChannelBuilderOpt.DSPContainer = tileRawChannelContainerDSP

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


