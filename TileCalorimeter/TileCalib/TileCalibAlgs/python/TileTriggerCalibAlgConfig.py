#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileTriggerCalibAlgConfig.py
@brief Python configuration of TileTriggerDefaultCalibTool tool for the Run III
'''

from AthenaConfiguration.Enums import Format
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TileTriggerDefaulCalibToolCfg(flags, **kwargs):

    ''' Function to configure TileTriggerDefaultCalibTool tool'''

    kwargs.setdefault('TileRawChannelContainer', 'TileRawChannelFit')
    kwargs.setdefault('TriggerTowerContainer', 'xAODTriggerTowers')

    acc = ComponentAccumulator()

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge( TileCablingSvcCfg(flags) )

    if 'TileCondToolEmscale' not in kwargs:
        from TileConditions.TileEMScaleConfig import TileCondToolEmscaleCfg
        emScaleTool = acc.popToolsAndMerge( TileCondToolEmscaleCfg(flags) )
        kwargs['TileCondToolEmscale'] = emScaleTool

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg( flags ))

    from TileRecUtils.TileDQstatusConfig import TileDQstatusAlgCfg
    acc.merge( TileDQstatusAlgCfg(flags) )

    # For running on bytestream data
    if flags.Input.Format is Format.BS:
        from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
        acc.merge(ByteStreamReadCfg(flags, type_names=['xAOD::TriggerTowerContainer/xAODTriggerTowers',
                                                       'xAOD::TriggerTowerAuxContainer/xAODTriggerTowersAux.']))

    from AthenaConfiguration.ComponentFactory import CompFactory
    TileTriggerDefalutCalibTool = CompFactory.TileTriggerDefaultCalibTool

    acc.setPrivateTools(TileTriggerDefalutCalibTool(**kwargs))

    return acc


'''
@brief Python configuration of TileTriggerCalibAlg algorithm for the Run III
'''
def TileTriggerCalibAlgCfg(flags, **kwargs):

    ''' Function to configure TileTriggerCalibAlg algorithm'''

    kwargs.setdefault('name', 'TileTriggerCalibAlg')
    kwargs.setdefault('RunType', 8)
    kwargs.setdefault('RunNumber', flags.Input.RunNumber[0])

    acc = ComponentAccumulator()

    if 'TileCalibTools' not in kwargs:
        triggerCalibTool = acc.popToolsAndMerge( TileTriggerDefaulCalibToolCfg(flags) )
        kwargs['TileCalibTools'] = [triggerCalibTool]

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

    cfg.merge( TileTriggerCalibAlgCfg(ConfigFlags) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileTriggerCalibAlg.pkl','wb') )

    sc = cfg.run()

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
