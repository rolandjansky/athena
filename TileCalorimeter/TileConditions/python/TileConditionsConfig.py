#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
def tileCondCfg( flags ):

    acc = ComponentAccumulator()

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    def __addFolder(f):        
        acc.merge( addFolders( flags, '%s <key>%s</key>' %(f,f),   'TILE_OFL' if '/OFL' in f else 'TILE_ONL',  className="CondAttrListCollection") )

    # from TileConditions.TileConditionsConf import TileCondProxyFile_TileCalibDrawerFlt_ as TileCondProxyFileFlt
    # from TileConditions.TileConditionsConf import TileCondProxyFile_TileCalibDrawerBch_ as TileCondProxyFileBch
    from TileConditions.TileConditionsConf import TileCondProxyCool_TileCalibDrawerFlt_ as TileCondProxyCoolFlt
    from TileConditions.TileConditionsConf import TileCondProxyCool_TileCalibDrawerBch_ as TileCondProxyCoolBch
    # from TileConditions.TileConditionsConf import TileCondProxyCool_TileCalibDrawerOfc_ as TileCondProxyCoolOfc

    emScale = 'TileEMScale'
    from TileConditions.TileConditionsConf import TileEMScaleCondAlg
    emScaleCondAlg = TileEMScaleCondAlg(name = emScale + 'CondAlg', TileEMScale = emScale)
    emScaleCondAlg.OnlCacheUnit      = 'OnlineMegaElectronVolts'
    emScaleCondAlg.OflCesProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OflCes',    Source = '/TILE/OFL02/CALIB/CES') 
    emScaleCondAlg.OflCisLinProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflCisLin', Source = '/TILE/OFL02/CALIB/CIS/LIN') 
    emScaleCondAlg.OflCisNlnProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflCisNln', Source = '/TILE/OFL02/CALIB/CIS/NLN') 
    emScaleCondAlg.OflEmsProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OflEms',    Source = '/TILE/OFL02/CALIB/EMS') 
    emScaleCondAlg.OflLasFibProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflLasFib', Source = '/TILE/OFL02/CALIB/LAS/FIBER') 
    emScaleCondAlg.OflLasLinProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflLasLin', Source = '/TILE/OFL02/CALIB/LAS/LIN') 
    emScaleCondAlg.OflLasNlnProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflLasNln', Source = '/TILE/OFL02/CALIB/LAS/NLN') 
    emScaleCondAlg.OnlCesProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OnlCes',    Source = '/TILE/ONL01/CALIB/CES') 
    emScaleCondAlg.OnlCisProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OnlCis',    Source = '/TILE/ONL01/CALIB/CIS/LIN') 
    emScaleCondAlg.OnlEmsProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OnlEms',    Source = '/TILE/ONL01/CALIB/EMS') 
    emScaleCondAlg.OnlLasProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OnlLas',    Source = '/TILE/ONL01/CALIB/LAS/LIN')

    __addFolder( '/TILE/OFL02/CALIB/CES')       
    __addFolder( '/TILE/OFL02/CALIB/CIS/LIN')   
    __addFolder( '/TILE/OFL02/CALIB/CIS/NLN')   
    __addFolder( '/TILE/OFL02/CALIB/EMS')       
    __addFolder( '/TILE/OFL02/CALIB/LAS/FIBER') 
    __addFolder( '/TILE/OFL02/CALIB/LAS/LIN')   
    __addFolder( '/TILE/OFL02/CALIB/LAS/NLN')   
    __addFolder( '/TILE/ONL01/CALIB/CES')       
    __addFolder( '/TILE/ONL01/CALIB/CIS/LIN')   
    __addFolder( '/TILE/ONL01/CALIB/EMS')       
    __addFolder( '/TILE/ONL01/CALIB/LAS/LIN')   

    acc.addCondAlgo( emScaleCondAlg )

    from TileConditions.TileConditionsConf import TileCondToolEmscale
    emScaleTool = TileCondToolEmscale(name = 'TileCondToolEmscale', TileEMScale = emScale)
    acc.addPublicTool( emScaleTool )


    from TileConditions.TileConditionsConf import TileCalibCondAlg_TileCalibDrawerFlt_ as TileCalibFltCondAlg
    def __addCondAlg(calibData, proxy):
        calibCondAlg = calibData + 'CondAlg'
        condAlg = TileCalibFltCondAlg( name = calibCondAlg, ConditionsProxy = proxy, TileCalibData = calibData)
        acc.addCondAlgo( condAlg )


    sampleNoise = 'TileSampleNoise'
    sampleNoiseProxy = TileCondProxyCoolFlt( 'TileCondProxyCool_NoiseSample', Source = '/TILE/OFL02/NOISE/SAMPLE' )
    __addFolder( '/TILE/OFL02/NOISE/SAMPLE' )
    __addCondAlg(sampleNoise, sampleNoiseProxy)

    onlineSampleNoise = 'TileOnlineSampleNoise'
    onlineSampleNoiseProxy = TileCondProxyCoolFlt( 'TileCondProxyCool_OnlineNoiseSample', Source = '/TILE/ONL01/NOISE/SAMPLE' )
    __addFolder( '/TILE/ONL01/NOISE/SAMPLE' )    
    __addCondAlg(onlineSampleNoise, onlineSampleNoiseProxy)

    from TileConditions.TileConditionsConf import TileCondToolNoiseSample
    noiseSampleTool = TileCondToolNoiseSample(name = 'TileCondToolNoiseSample', 
                                              TileSampleNoise = sampleNoise, 
                                              TileOnlineSampleNoise = onlineSampleNoise)
    acc.addPublicTool( noiseSampleTool )


    timing = 'TileTiming'
    timingProxy = TileCondProxyCoolFlt('TileCondProxyCool_AdcOffset', Source = '/TILE/OFL02/TIME/CHANNELOFFSET/PHY' )
    __addFolder( '/TILE/OFL02/TIME/CHANNELOFFSET/PHY' )
    __addCondAlg(timing, timingProxy)

    from TileConditions.TileConditionsConf import TileCondToolTiming
    timingTool = TileCondToolTiming(name = 'TileCondToolTiming', TileTiming = timing)
    acc.addPublicTool( timingTool )


    badChannels = 'TileBadChannels'
    from TileConditions.TileConditionsConf import TileBadChannelsCondAlg
    badChannelsCondAlg = TileBadChannelsCondAlg( name = badChannels + 'CondAlg', TileBadChannels = badChannels)
    badChannelsCondAlg.OflBchProxy = TileCondProxyCoolBch('TileCondProxyCool_OflBch', Source = '/TILE/OFL02/STATUS/ADC' )
    badChannelsCondAlg.OnlBchProxy = TileCondProxyCoolBch('TileCondProxyCool_OnlBch', Source = '/TILE/ONL01/STATUS/ADC' )
    __addFolder( '/TILE/OFL02/STATUS/ADC' )
    __addFolder( '/TILE/ONL01/STATUS/ADC' )
    acc.addCondAlgo( badChannelsCondAlg )

    from TileConditions.TileConditionsConf import TileBadChanTool
    badChanTool = TileBadChanTool(name = 'TileBadChanTool', TileBadChannels = badChannels)
    acc.addPublicTool( badChanTool )
    

    from TileConditions.TileConditionsConf import TileInfoLoader, TileCablingSvc
    infoLoaderSvc = TileInfoLoader()
    infoLoaderSvc.NoiseScaleIndex= 2
    acc.addService( infoLoaderSvc ) 
#    acc.addToAppProperty('CreateSvc', [infoLoaderSvc.getFullName()] )


    cablingSvc = TileCablingSvc()
    cablingSvc.CablingType = 4
    acc.addService( cablingSvc )

    return acc

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    acc.merge( tileCondCfg(ConfigFlags) )

    f=open('TileCond.pkl','w')
    acc.store(f)
    f.close()
