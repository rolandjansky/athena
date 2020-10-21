# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured base Tile raw channel builder tool"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

_runTypes = {'PHY' : 1, 'LAS' : 2, 'BILAS' : 2, 'PED' : 4, 'CIS' : 8, 'MONOCIS' : 8}

def TileRawChannelBuilderCfg(flags, name, TileRawChannelBuilder, **kwargs):
    """Return component accumulator with configured private base Tile raw channel builder tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
        name -- name of Tile raw channel builder
        TileRawChannelbuilder -- concrete Tile raw channel builder tool.
    """

    acc = ComponentAccumulator()

    runType = flags.Tile.RunType
    runType = runType.upper()

    if runType not in _runTypes.keys():
        raise(Exception("Invalid Tile run type: %s" % runType))

    createContainer = ( kwargs.get('TileRawChannelContainer', "") != "" )

    if createContainer:
        from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
        acc.merge( TileDQstatusAlgCfg(flags) )
    else:
        kwargs['TileDQstatus'] = ""

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge( TileCablingSvcCfg(flags) )

    kwargs['RunType'] = _runTypes[runType]
    kwargs['calibrateEnergy'] = False

    kwargs.setdefault('AmpMinForAmpCorrection', flags.Tile.AmpMinForAmpCorrection)
    kwargs.setdefault('TimeMinForAmpCorrection', flags.Tile.TimeMinForAmpCorrection)
    kwargs.setdefault('TimeMaxForAmpCorrection', flags.Tile.TimeMaxForAmpCorrection)

    tileRawChannelContainerDSP = ""
    if createContainer:
        if 'NoiseFilterTools' not in kwargs:
            from TileRecUtils.TileRawChannelCorrectionConfig import TileRawChannelCorrectionToolsCfg
            correctionTools = acc.popToolsAndMerge( TileRawChannelCorrectionToolsCfg(flags) )
            kwargs['NoiseFilterTools'] = correctionTools

        if not (flags.Input.isMC or flags.Overlay.DataOverlay) and len(kwargs['NoiseFilterTools']) > 0:
            tileRawChannelContainerDSP = 'TileRawChannelCntCorrected'
            from TileRecUtils.TileRawChannelCorrectionConfig import TileRawChannelCorrectionAlgCfg
            acc.merge( TileRawChannelCorrectionAlgCfg(flags) )

    kwargs.setdefault('DSPContainer', tileRawChannelContainerDSP)

    acc.setPrivateTools( TileRawChannelBuilder(name, **kwargs) )

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

    TileRawChannelBuilderFitFilter=CompFactory.TileRawChannelBuilderFitFilter
    rchBuilderFitAcc = TileRawChannelBuilderCfg(ConfigFlags,
                                                name = 'TileRawChannelBuilderFit',
                                                TileRawChannelBuilder = TileRawChannelBuilderFitFilter)
    print( acc.popToolsAndMerge(rchBuilderFitAcc) )

    TileRawChannelBuilderOpt2Filter=CompFactory.TileRawChannelBuilderOpt2Filter
    rchBuilderOpt2Acc = TileRawChannelBuilderCfg(ConfigFlags,
                                                 name = 'TileRawChannelBuilderOpt2',
                                                 TileRawChannelBuilder = TileRawChannelBuilderOpt2Filter)
    print( acc.popToolsAndMerge(rchBuilderOpt2Acc) )


    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileRawChannelBuilder.pkl','wb') )


