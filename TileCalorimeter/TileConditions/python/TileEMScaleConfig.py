# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile EM scale"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

_validSources = ['COOL','FILE']

def TileEMScaleCondAlgCfg(flags, **kwargs):
    """Return component accumulator with configured Tile EM scale conditions algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of EM scale conditions (COOL, FILE). Defaults to COOL.
        TileEMScale -- name of Tile EM scale conditions object. Defaults to TileEMScale.
    """

    acc = ComponentAccumulator()
    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg

    acc.merge( TileCablingSvcCfg(flags) )

    source = kwargs.get('Source', 'COOL')
    emScale = kwargs.get('TileEMScale', 'TileEMScale')

    if source not in _validSources:
        raise(Exception("Invalid source: %s" % source))

    name = emScale + 'CondAlg'
    onlCacheUnit = "Invalid"

    if source == 'COOL':
        # Connect COOL Tile conditions proxies to the tool
        from TileConditions.TileFolders import TileFolders
        folders = TileFolders(isMC = flags.Input.isMC, isOnline = flags.Common.isOnline)

        if not flags.Input.isMC:
            onlCacheUnit = 'OnlineMegaElectronVolts'

        oflLasLinFolder = folders.addSplitOnline('/TILE/ONL01/CALIB/LAS/LIN', '/TILE/OFL02/CALIB/LAS/LIN')
        oflCesFolder =    folders.addSplitOnline('/TILE/ONL01/CALIB/CES',     '/TILE/OFL02/CALIB/CES')
        oflEmsFolder =    folders.addSplitOnline('/TILE/ONL01/CALIB/EMS',     '/TILE/OFL02/CALIB/EMS')

        onlCisLinFolder = folders.addSplitMC('/TILE/ONL01/CALIB/CIS/LIN', '/TILE/OFL02/CALIB/CIS/FIT/LIN')
        onlLasLinFolder = folders.addSplitMC('/TILE/ONL01/CALIB/LAS/LIN', '/TILE/OFL02/CALIB/LAS/LIN')
        onlCesFolder =    folders.addSplitMC('/TILE/ONL01/CALIB/CES',     '/TILE/OFL02/CALIB/CES')
        onlEmsFolder =    folders.addSplitMC('/TILE/ONL01/CALIB/EMS',     '/TILE/OFL02/CALIB/EMS')


        if flags.IOVDb.DatabaseInstance  == 'CONDBR2':
            oflCisLinFolder = folders.addSplitOnline('/TILE/ONL01/CALIB/CIS/LIN', '/TILE/OFL02/CALIB/CIS/LIN')
            oflCisNlnFolder = folders.addSplitOnline('/TILE/ONL01/CALIB/CIS/NLN', '/TILE/OFL02/CALIB/CIS/NLN')
            oflLasNlnFolder = folders.addSplitOnline('/TILE/ONL01/CALIB/LAS/NLN', '/TILE/OFL02/CALIB/LAS/NLN')
            if not flags.Common.isOnline:
                oflLasFibFolder = folders.add('/TILE/OFL02/CALIB/LAS/FIBER', 'TILE_OFL')
            else: 
                oflLasFibFolder = None
        else:
            oflCisLinFolder = folders.addSplitOnline('/TILE/ONL01/CALIB/CIS/LIN',     '/TILE/OFL02/CALIB/CIS/FIT/LIN')
            oflCisNlnFolder = folders.addSplitOnline('/TILE/OFL01/CALIB/CIS/FIT/NLN', '/TILE/OFL02/CALIB/CIS/FIT/NLN')
            oflLasNlnFolder = folders.addSplitOnline('/TILE/OFL01/CALIB/LAS/NLN',     '/TILE/OFL02/CALIB/LAS/NLN')
            oflLasFibFolder = folders.addSplitOnline('/TILE/OFL01/CALIB/LAS/FIBER',   '/TILE/OFL02/CALIB/LAS/FIBER')

        from IOVDbSvc.IOVDbSvcConfig import addFolderList
        acc.merge( addFolderList(flags, folders.get()) )

        TileCondProxyCoolFlt = CompFactory.getComp("TileCondProxyCool<TileCalibDrawerFlt>")
        if not oflLasFibFolder: 
            oflLasFibProxy = None
        else: 
            oflLasFibProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflLasFib', Source = oflLasFibFolder)
        oflCisLinProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflCisLin', Source = oflCisLinFolder)
        oflCisNlnProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflCisNln', Source = oflCisNlnFolder)
        oflLasLinProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflLasLin', Source = oflLasLinFolder)
        oflLasNlnProxy = TileCondProxyCoolFlt('TileCondProxyCool_OflLasNln', Source = oflLasNlnFolder)
        oflCesProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OflCes', Source = oflCesFolder)
        oflEmsProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OflEms', Source = oflEmsFolder)
        onlCisProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OnlCis', Source = onlCisLinFolder)
        onlLasProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OnlLas', Source = onlLasLinFolder)
        onlCesProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OnlCes', Source = onlCesFolder)
        onlEmsProxy    = TileCondProxyCoolFlt('TileCondProxyCool_OnlEms', Source = onlEmsFolder)
        
    else:
        # Connect FILE Tile conditions proxies to the tool
        # Undoing "online" calibrations makes no sense here and is disabled
        TileCondProxyFileFlt = CompFactory.getComp("TileCondProxyFile<TileCalibDrawerFlt>")
        oflCisLinProxy = TileCondProxyFileFlt('TileCondProxyFile_OflCisLin', Source = 'TileDefault.cisFitLin')
        oflCisNlnProxy = TileCondProxyFileFlt('TileCondProxyFile_OflCisNln', Source = 'TileDefault.cisFitNln')
        oflLasLinProxy = TileCondProxyFileFlt('TileCondProxyFile_OflLasLin', Source = 'TileDefault.lasLin')
        oflLasNlnProxy = TileCondProxyFileFlt('TileCondProxyFile_OflLasNln', Source = 'TileDefault.lasNln')
        oflLasFibProxy = TileCondProxyFileFlt('TileCondProxyFile_OflLasFib', Source = 'TileDefault.lasFib')
        oflCesProxy    = TileCondProxyFileFlt('TileCondProxyFile_OflCes', Source = 'TileDefault.ces')
        oflEmsProxy    = TileCondProxyFileFlt('TileCondProxyFile_OflEms', Source = 'TileDefault.ems')
        onlCisProxy    = TileCondProxyFileFlt('TileCondProxyFile_OflCisLin', Source = 'TileDefault.cisFitLin')
        onlLasProxy    = TileCondProxyFileFlt('TileCondProxyFile_OflLasLin', Source = 'TileDefault.lasLin')
        onlCesProxy    = TileCondProxyFileFlt('TileCondProxyFile_OflCes', Source = 'TileDefault.ces')
        onlEmsProxy    = TileCondProxyFileFlt('TileCondProxyFile_OflEms', Source = 'TileDefault.ems')


    TileEMScaleCondAlg=CompFactory.TileEMScaleCondAlg
    emScaleCondAlg = TileEMScaleCondAlg(name = name,
                                        OnlCacheUnit = onlCacheUnit,
                                        OflCisLinProxy = oflCisLinProxy,
                                        OflCisNlnProxy = oflCisNlnProxy,
                                        OflLasLinProxy = oflLasLinProxy,
                                        OflLasNlnProxy = oflLasNlnProxy,
                                        OflLasFibProxy = oflLasFibProxy,
                                        OflCesProxy = oflCesProxy,
                                        OflEmsProxy = oflEmsProxy,
                                        OnlCisProxy = onlCisProxy,
                                        OnlLasProxy = onlLasProxy,
                                        OnlCesProxy = onlCesProxy,
                                        OnlEmsProxy = onlEmsProxy,
                                        TileEMScale = emScale)



    acc.addCondAlgo(emScaleCondAlg)
    
    return acc


def TileCondToolEmscaleCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile EM scale tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of EM scale conditions (COOL, FILE). Defaults to COOL.
        TileEMScale -- name of Tile EM scale conditions object. Defaults to TileEMScale.
    """

    acc = ComponentAccumulator()

    kwargs.setdefault('Source', 'COOL')
    kwargs.setdefault('TileEMScale', 'TileEMScale')

    source = kwargs['Source']
    if source not in _validSources:
        raise(Exception("Invalid source: %s" % source))

    emScale = kwargs['TileEMScale']
    name = 'TileCondToolEmscale'

    acc.merge( TileEMScaleCondAlgCfg(flags, **kwargs) )

    TileCondToolEmscale=CompFactory.TileCondToolEmscale
    acc.setPrivateTools( TileCondToolEmscale(name, TileEMScale = emScale) )

    return acc


def TileExpertToolEmscaleCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile expert EM scale tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of EM scale conditions (COOL, FILE). Defaults to COOL.
        TileEMScale -- name of Tile EM scale conditions object. Defaults to TileEMScale.
    """

    acc = ComponentAccumulator()

    kwargs.setdefault('Source', 'COOL')
    kwargs.setdefault('TileEMScale', 'TileEMScale')

    source = kwargs['Source']
    if source not in _validSources:
        raise(Exception("Invalid source: %s" % source))

    emScale = kwargs['TileEMScale']
    name = 'TileExpertToolEmscale'

    acc.merge( TileEMScaleCondAlgCfg(flags, **kwargs) )

    TileExpertToolEmscale=CompFactory.TileExpertToolEmscale
    acc.setPrivateTools( TileExpertToolEmscale(name, TileEMScale = emScale) )

    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    
    # test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = ComponentAccumulator()

    emScaleTool =  acc.popToolsAndMerge( TileCondToolEmscaleCfg(ConfigFlags) )
    print(emScaleTool)

    emScaleExpertTool =  acc.popToolsAndMerge( TileExpertToolEmscaleCfg(ConfigFlags) )
    print(emScaleExpertTool)

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('IOVDbSvc'))
    acc.store( open('TileEMScale.pkl','wb') )

    print('All OK')
