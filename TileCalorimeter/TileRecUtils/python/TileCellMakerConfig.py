# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile Cell maker algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from CaloCellCorrection.CaloCellCorrectionConfig import CaloCellNeighborsAverageCorrCfg


def CaloCellContainerCheckerToolCfg(flags):
    """Return component accumulator with configured private Calo cell container checker tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    acc = ComponentAccumulator()

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(flags))

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(flags))

    CaloCellContainerCheckerTool=CompFactory.CaloCellContainerCheckerTool
    acc.setPrivateTools( CaloCellContainerCheckerTool() )

    return acc


def TileCellMakerCfg(flags, **kwargs):
    """Return component accumulator with configured Tile Cell maker algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
        Name -- name of Tile cell maker algorithm. Defautls to TileCellMaker
                or TileCellMakerHG/TileCellMakerLG depending on only used gain.
        SkipGain - skip given gain. Defaults to -1 [use all gains]. Possible values: 0 [LG], 1 [HG].
        CaloCellsOutputName -- name of Tile cell maker algorithm. Defautls to AllCalo
                               or AllCaloHG/AllCaloLG depending on only used gain.
        DoCaloNeighborsCorrection -- correct dead cells. Assign as energy the average energy of
                                     the surrounding cells. Defaults to False.
    """

    acc = ComponentAccumulator()

    useGain = {0 : 'HG', 1 : 'LG'} # Key is skipped gain

    skipGain = kwargs.get('SkipGain', -1) # Never skip any gain by default

    defaultName = 'TileCellMaker' if skipGain == -1 else 'TileCellMaker' + useGain[skipGain]
    name = kwargs.get('Name', defaultName)

    defaultOutputCells = 'AllCalo' if skipGain == -1 else 'AllCalo' + useGain[skipGain]
    caloCellsOutputName = kwargs.get('Name', defaultOutputCells)

    doCaloNeighborsCorrection = kwargs.get('DoCaloNeighborsCorrection', False)

    from AthenaCommon.Logging import logging
    msg = logging.getLogger( 'TileCellMakerCfg' )

    if flags.Tile.readDigits:
        msg.info('Reconstruct Tile raw channels')
        from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerCfg
        acc.merge( TileRawChannelMakerCfg(flags) )

    CaloCellMaker, CaloCellContainerFinalizerTool=CompFactory.getComps("CaloCellMaker","CaloCellContainerFinalizerTool",)
    from TileRecUtils.TileCellBuilderConfig import TileCellBuilderCfg
    tileCellBuilder = acc.popToolsAndMerge( TileCellBuilderCfg(flags, SkipGain = skipGain) )

    cellMakerTools = [tileCellBuilder, CaloCellContainerFinalizerTool()]

    noiseFilter = flags.Tile.NoiseFilter

    doCellNoiseFilter = noiseFilter - noiseFilter % 100
    doRawChannelNoiseFilter = noiseFilter - doCellNoiseFilter - noiseFilter % 10

    if doRawChannelNoiseFilter == 10:
        msg.info('Use Tile raw channel noise filter')
        from TileRecUtils.TileRawChannelCorrectionConfig import TileRawChannelNoiseFilterCfg
        noiseFilter = acc.popToolsAndMerge( TileRawChannelNoiseFilterCfg(flags) )
        tileCellBuilder.NoiseFilterTools = [noiseFilter]

    if doCellNoiseFilter == 100:
        msg.info('Use Tile cell noise filter')
        from TileRecUtils.TileCellNoiseFilterConfig import TileCellNoiseFilterCfg
        cellNoiseFilter = acc.popToolsAndMerge( TileCellNoiseFilterCfg(flags) )
        cellMakerTools += [ cellNoiseFilter ]

    if doCaloNeighborsCorrection:
        msg.info('Use Calo cell neighbours average correction')
        caloCellNeighborsAverageCorrection = acc.popToolsAndMerge( CaloCellNeighborsAverageCorrCfg(flags) )
        cellMakerTools += [caloCellNeighborsAverageCorrection]


    caloCellContainerChecker = acc.popToolsAndMerge( CaloCellContainerCheckerToolCfg(flags) )
    cellMakerTools += [caloCellContainerChecker]

    cellMakerAlg = CaloCellMaker(name = name, CaloCellMakerToolNames = cellMakerTools,
                                 CaloCellsOutputName = caloCellsOutputName)

    acc.addEventAlgo(cellMakerAlg, primary = True)

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
    ConfigFlags.Tile.doOptATLAS = True
    ConfigFlags.Tile.correctTimeJumps = True
    ConfigFlags.Tile.NoiseFilter = 1
    ConfigFlags.Output.ESDFileName = "myESD.pool.root"
    ConfigFlags.Exec.MaxEvents=3
    ConfigFlags.fillFromArgs()

    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc.merge( ByteStreamReadCfg(ConfigFlags, ['TileRawChannelContainer/TileRawChannelCnt', 'TileDigitsContainer/TileDigitsCnt']) )

    acc.merge( TileCellMakerCfg(ConfigFlags) )

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(ConfigFlags, "ESD",
                              ItemList = [ 'CaloCellContainer#*', 'TileCellContainer#*']))

    acc.getEventAlgo("OutputStreamESD").ExtraInputs = [('CaloCellContainer', 'StoreGateSvc+AllCalo')]

    ConfigFlags.dump()
    acc.printConfig(withDetails = True, summariseProps = True)
    acc.store( open('TileCellMaker.pkl','wb') )

    sc = acc.run()

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())

