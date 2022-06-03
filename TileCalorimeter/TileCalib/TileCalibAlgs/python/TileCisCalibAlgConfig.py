#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileCisCalibAlgConfig.py
@brief Python configuration of TileCisDefaultCalibTool tool for the Run III
'''
def TileCisDefaulCalibToolCfg(flags, **kwargs):

    ''' Function to configure TileCisDefaultCalibTool tool'''

    kwargs.setdefault('removePed', True)

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

    from AthenaConfiguration.ComponentFactory import CompFactory
    TileCisDefalutCalibTool = CompFactory.TileCisDefaultCalibTool

    acc.setPrivateTools(TileCisDefalutCalibTool(**kwargs))

    return acc


'''
@brief Python configuration of TileCisCalibAlg algorithm for the Run III
'''
def TileCisCalibAlgCfg(flags, **kwargs):

    ''' Function to configure TileCisCalibAlg algorithm'''

    kwargs.setdefault('name', 'TileCisCalibAlg')
    kwargs.setdefault('RunType', 8)
    kwargs.setdefault('RunNumber', flags.Input.RunNumber[0])

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    if 'TileCalibTools' not in kwargs:
        cisCalibTool = acc.popToolsAndMerge( TileCisDefaulCalibToolCfg(flags) )
        kwargs['TileCalibTools'] = [cisCalibTool]

    from AthenaConfiguration.ComponentFactory import CompFactory
    TileTopCalibAlg = CompFactory.TileTopCalibAlg

    acc.addEventAlgo(TileTopCalibAlg(**kwargs), primary=True)

    return acc

if __name__=='__main__':

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Tile.doFit = True
    ConfigFlags.Exec.MaxEvents = 3
    ConfigFlags.Tile.RunType = 'CIS'
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    tileTypeNames = ['TileRawChannelContainer/TileRawChannelCnt',
                     'TileBeamElemContainer/TileBeamElemCnt',
                     'TileDigitsContainer/TileDigitsCnt']
    cfg.merge( ByteStreamReadCfg(ConfigFlags, type_names = tileTypeNames) )
    cfg.getService('ByteStreamCnvSvc').ROD2ROBmap = [ "-1" ]

    runNumber = ConfigFlags.Input.RunNumber[0]
    from AthenaConfiguration.ComponentFactory import CompFactory
    cfg.addPublicTool( CompFactory.TileROD_Decoder(fullTileMode = runNumber) )

    from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerCfg
    cfg.merge( TileRawChannelMakerCfg(ConfigFlags) )

    cfg.merge( TileCisCalibAlgCfg(ConfigFlags) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileCisCalibAlg.pkl','wb') )

    sc = cfg.run()

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
