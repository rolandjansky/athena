#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
def tileCondCfg( flags ):

    acc = ComponentAccumulator()

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    def __addFolder(f):        
        acc.merge( addFolders( flags, '%s <key>%s</key>' %(f,f),   'TILE_OFL' if '/OFL' in f else 'TILE_ONL',  className="CondAttrListCollection") )

    #TileCondProxyFileFlt=CompFactory.getComp("TileCondProxyFile<TileCalibDrawerFlt>")
    #TileCondProxyFileBch=CompFactory.getComp("TileCondProxyFile<TileCalibDrawerBch>")
    TileCondProxyCoolFlt=CompFactory.getComp("TileCondProxyFile<TileCalibDrawerFlt>")
    TileCondProxyCoolBch=CompFactory.getComp("TileCondProxyCool<TileCalibDrawerBch>")
    #TileCondProxyCoolOfc=CompFactory.getComp("TileCondProxyCool<TileCalibDrawerOfc>")

    emScale = 'TileEMScale'
    TileEMScaleCondAlg=CompFactory.TileEMScaleCondAlg
    emScaleCondAlg = TileEMScaleCondAlg(name = emScale + 'CondAlg', TileEMScale = emScale)

    # Defaults for offline data
    folder_OFL_CALIB_CIS = "CALIB/CIS/"
    emScaleCondAlg.OnlCacheUnit   = 'OnlineMegaElectronVolts'
    if flags.Input.isMC:
        folder_OFL_CALIB_CIS = "CALIB/CIS/FIT/"
        emScaleCondAlg.OnlCacheUnit   = 'Invalid'

    def getTileCondProxyCoolFlt(name, folder):
        __addFolder(folder)
        return TileCondProxyCoolFlt(name, Source=folder)

    emScaleCondAlg.OflCesProxy    = getTileCondProxyCoolFlt('TileCondProxyCool_OflCes',    '/TILE/OFL02/CALIB/CES')
    emScaleCondAlg.OflCisLinProxy = getTileCondProxyCoolFlt('TileCondProxyCool_OflCisLin', '/TILE/OFL02/'+folder_OFL_CALIB_CIS+'LIN')
    emScaleCondAlg.OflCisNlnProxy = getTileCondProxyCoolFlt('TileCondProxyCool_OflCisNln', '/TILE/OFL02/'+folder_OFL_CALIB_CIS+'NLN')
    emScaleCondAlg.OflEmsProxy    = getTileCondProxyCoolFlt('TileCondProxyCool_OflEms',    '/TILE/OFL02/CALIB/EMS')
    emScaleCondAlg.OflLasFibProxy = getTileCondProxyCoolFlt('TileCondProxyCool_OflLasFib', '/TILE/OFL02/CALIB/LAS/FIBER')
    emScaleCondAlg.OflLasLinProxy = getTileCondProxyCoolFlt('TileCondProxyCool_OflLasLin', '/TILE/OFL02/CALIB/LAS/LIN')
    emScaleCondAlg.OflLasNlnProxy = getTileCondProxyCoolFlt('TileCondProxyCool_OflLasNln', '/TILE/OFL02/CALIB/LAS/NLN')
    emScaleCondAlg.OnlCesProxy    = getTileCondProxyCoolFlt('TileCondProxyCool_OnlCes',    '/TILE/ONL01/CALIB/CES')
    emScaleCondAlg.OnlCisProxy    = getTileCondProxyCoolFlt('TileCondProxyCool_OnlCis',    '/TILE/ONL01/CALIB/CIS/LIN')
    emScaleCondAlg.OnlEmsProxy    = getTileCondProxyCoolFlt('TileCondProxyCool_OnlEms',    '/TILE/ONL01/CALIB/EMS')
    emScaleCondAlg.OnlLasProxy    = getTileCondProxyCoolFlt('TileCondProxyCool_OnlLas',    '/TILE/ONL01/CALIB/LAS/LIN')

    acc.addCondAlgo( emScaleCondAlg )

    TileCondToolEmscale=CompFactory.TileCondToolEmscale
    emScaleTool = TileCondToolEmscale(name = 'TileCondToolEmscale', TileEMScale = emScale)
    acc.addPublicTool( emScaleTool )

    TileCalibFltCondAlg=CompFactory.getComp("TileCalibCondAlg<TileCalibDrawerFlt>")
    def __addCondAlg(calibData, proxy):
        calibCondAlg = calibData + 'CondAlg'
        condAlg = TileCalibFltCondAlg( name = calibCondAlg, ConditionsProxy = proxy, TileCalibData = calibData)
        acc.addCondAlgo( condAlg )


    sampleNoise = 'TileSampleNoise'
    sampleNoiseProxy = getTileCondProxyCoolFlt( 'TileCondProxyCool_NoiseSample', '/TILE/OFL02/NOISE/SAMPLE' )
    __addCondAlg(sampleNoise, sampleNoiseProxy)

    TileCondToolNoiseSample=CompFactory.TileCondToolNoiseSample
    noiseSampleTool = TileCondToolNoiseSample(name = 'TileCondToolNoiseSample', 
                                              TileSampleNoise = sampleNoise)

    if flags.Common.isOnline:
        onlineSampleNoise = 'TileOnlineSampleNoise'
        onlineSampleNoiseProxy = getTileCondProxyCoolFlt( 'TileCondProxyCool_OnlineNoiseSample', '/TILE/ONL01/NOISE/SAMPLE' )
        __addCondAlg(onlineSampleNoise, onlineSampleNoiseProxy)
        noiseSampleTool.TileOnlineSampleNoise = onlineSampleNoise
    acc.addPublicTool( noiseSampleTool )


    timing = 'TileTiming'
    timingProxy = getTileCondProxyCoolFlt('TileCondProxyCool_AdcOffset', '/TILE/OFL02/TIME/CHANNELOFFSET/PHY' )
    __addCondAlg(timing, timingProxy)

    TileCondToolTiming=CompFactory.TileCondToolTiming
    timingTool = TileCondToolTiming(name = 'TileCondToolTiming', TileTiming = timing)
    acc.addPublicTool( timingTool )

    def getTileCondProxyCoolBch(name, folder):
        __addFolder(folder)
        return TileCondProxyCoolBch(name, Source=folder)


    badChannels = 'TileBadChannels'
    TileBadChannelsCondAlg=CompFactory.TileBadChannelsCondAlg
    badChannelsCondAlg = TileBadChannelsCondAlg( name = badChannels + 'CondAlg', TileBadChannels = badChannels)
    badChannelsCondAlg.OflBchProxy = getTileCondProxyCoolBch('TileCondProxyCool_OflBch', '/TILE/OFL02/STATUS/ADC' )
    badChannelsCondAlg.OnlBchProxy = getTileCondProxyCoolBch('TileCondProxyCool_OnlBch', '/TILE/ONL01/STATUS/ADC' )
    acc.addCondAlgo( badChannelsCondAlg )

    TileBadChanTool=CompFactory.TileBadChanTool
    badChanTool = TileBadChanTool(name = 'TileBadChanTool', TileBadChannels = badChannels)
    acc.addPublicTool( badChanTool )

    # Defaults for offline data
    dbname = "LAR_OFL"
    badchannelsfolder = "BadChannelsOfl"
    if flags.Common.isOnline:
        dbname = "LAR"
    if flags.Common.isOnline or flags.Input.isMC:
        badchannelsfolder = "BadChannels"
    acc.merge( addFolders(flags, ['/LAR/'+badchannelsfolder+'/BadChannels', 
                                  '/LAR/'+badchannelsfolder+'/MissingFEBs'], dbname))
    

    TileInfoLoader, TileCablingSvc=CompFactory.getComps("TileInfoLoader","TileCablingSvc",)
    infoLoaderSvc = TileInfoLoader()
    infoLoaderSvc.NoiseScaleIndex= 2
    acc.addService(infoLoaderSvc, create = True)
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

    acc = ComponentAccumulator()
    acc.merge( tileCondCfg(ConfigFlags) )

    f=open('TileCond.pkl','wb')
    acc.store(f)
    f.close()
