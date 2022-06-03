# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured private Tile Cell noise filter tool"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def TileCellNoiseFilterCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile cell noise filter tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
        UseCaloNoise -- use Calo noise conditions object. Defaults to False.
    """

    acc = ComponentAccumulator()

    useCaloNoise = kwargs.get('UseCaloNoise', False)

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(flags))

    TileCellNoiseFilter=CompFactory.TileCellNoiseFilter
    tileCellNoiseFilter = TileCellNoiseFilter()

    from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
    emScaleTool = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )
    tileCellNoiseFilter.TileCondToolEmscale = emScaleTool

    if useCaloNoise:
        from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
        acc.merge( CaloNoiseCondAlgCfg(flags, 'electronicNoise') )
        tileCellNoiseFilter.CaloNoise = 'electronicNoise'
    else:
        from TileConditions.TileSampleNoiseConfig import TileCondToolNoiseSampleCfg
        sampleNoiseTool = acc.popToolsAndMerge( TileCondToolNoiseSampleCfg(flags) )
        tileCellNoiseFilter.TileCondToolNoiseSample = sampleNoiseTool

        from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
        badChanTool = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )
        tileCellNoiseFilter.TileBadChanTool = badChanTool

    acc.setPrivateTools( tileCellNoiseFilter )

    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG

    # Test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Tile.RunType = 'PHY'
    ConfigFlags.Tile.NoiseFilter = 111

    ConfigFlags.fillFromArgs()

    ConfigFlags.lock()

    acc = ComponentAccumulator()

    print( acc.popToolsAndMerge( TileCellNoiseFilterCfg(ConfigFlags) ) )

    ConfigFlags.dump()
    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileCellNoiseFilter.pkl','wb') )
