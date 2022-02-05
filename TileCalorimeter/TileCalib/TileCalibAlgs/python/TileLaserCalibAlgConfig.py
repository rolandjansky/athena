#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileLaserCalibAlgConfig.py
@brief Python configuration of TileLaserDefaultCalibTool tool for the Run III
'''
def TileLaserDefaulCalibToolCfg(flags, **kwargs):

    ''' Function to configure TileLaserDefaultCalibTool tool'''

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge( TileCablingSvcCfg(flags) )

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg( flags ))

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    acc.merge( TileDQstatusAlgCfg(flags) )

    if 'TileBadChanTool' not in kwargs:
        from TileConditions.TileBadChannelsConfig import TileBadChanToolCfg
        badChanTool = acc.popToolsAndMerge( TileBadChanToolCfg(flags) )
        kwargs['TileBadChanTool'] = badChanTool

    if 'TileCondToolEmscale' not in kwargs:
        from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
        emScaleTool = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )
        kwargs['TileCondToolEmscale'] = emScaleTool

    if 'TileDCSTool' not in kwargs:
        from TileConditions.TileDCSConfig import TileDCSToolCfg
        kwargs['TileDCSTool'] = acc.popToolsAndMerge( TileDCSToolCfg(flags) )

    from AthenaConfiguration.ComponentFactory import CompFactory
    TileLaserDefalutCalibTool = CompFactory.TileLaserDefaultCalibTool

    acc.setPrivateTools(TileLaserDefalutCalibTool(**kwargs))

    return acc


'''
@brief Python configuration of TileLaserCalibAlg algorithm for the Run III
'''
def TileLaserCalibAlgCfg(flags, **kwargs):

    ''' Function to configure TileLaserCalibAlg algorithm'''

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    if 'Tools' not in kwargs:
        laserCalibTool = acc.popToolsAndMerge( TileLaserDefaulCalibToolCfg(flags) )
        kwargs['Tools'] = [laserCalibTool]

    from AthenaConfiguration.ComponentFactory import CompFactory
    TileLaserCalibAlg = CompFactory.TileLaserCalibAlg

    acc.addEventAlgo(TileLaserCalibAlg(**kwargs), primary=True)

    return acc

if __name__=='__main__':

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    inputDirectory = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TileByteStream/TileByteStream-02-00-00'
    inputFile = 'data18_tilecomm.00363899.calibration_tile.daq.RAW._lb0000._TileREB-ROS._0005-200ev.data'
    ConfigFlags.Input.Files = [inputDirectory + '/' + inputFile]
    ConfigFlags.Exec.MaxEvents = 3
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    tileTypeNames = ['TileRawChannelContainer/TileRawChannelCnt',
                     'TileBeamElemContainer/TileBeamElemCnt',
                     'TileDigitsContainer/TileDigitsCnt',
                     'TileLaserObject/TileLaserObject']
    cfg.merge( ByteStreamReadCfg(ConfigFlags, type_names = tileTypeNames) )
    cfg.getService('ByteStreamCnvSvc').ROD2ROBmap = [ "-1" ]

    runNumber = ConfigFlags.Input.RunNumber[0]
    from AthenaConfiguration.ComponentFactory import CompFactory
    cfg.addPublicTool( CompFactory.TileROD_Decoder(fullTileMode = runNumber) )

    from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerCfg
    cfg.merge( TileRawChannelMakerCfg(ConfigFlags) )

    cfg.merge( TileLaserCalibAlgCfg(ConfigFlags, ) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileLaserCalibAlg.pkl','wb') )

    sc = cfg.run()

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
