# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile bad channels"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

_validSources = ['COOL','FILE']

def TileBadChannelsCondAlgCfg(flags, **kwargs):
    """Return component accumulator with configured Tile bad channels conditions algorithm

    Arguments:
        flags:  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source: -- source of Tile bad channels (COOL, FILE). Defaults to COOL.
        TileBadChannels -- name of Tile bad channels object. Defaults to TileBadChannels.
    """

    acc = ComponentAccumulator()

    source = kwargs.get('Source', 'COOL')
    badChannels = kwargs.get('TileBadChannels', 'TileBadChannels')

    if source not in _validSources:
        raise(Exception("Invalid source: %s" % source))

    name = badChannels + 'CondAlg'

    if source == 'COOL':
        # Connect COOL Tile conditions proxies to the algorithm (default)

        from TileConditions.TileFolders import TileFolders
        folders = TileFolders(isMC = flags.Input.isMC, isOnline = flags.Common.isOnline)

        TileCondProxyCoolBch=CompFactory.getComp("TileCondProxyCool<TileCalibDrawerBch>")
        onlineFolder = folders.addSplitMC('/TILE/ONL01/STATUS/ADC', '/TILE/ONL01/STATUS/ADC')
        onlineBadChannelsProxy = TileCondProxyCoolBch('TileCondProxyCool_OnlBch', Source = onlineFolder)

        if flags.IOVDb.DatabaseInstance  == 'CONDBR2':
            if not flags.Common.isOnline:
                oflineFolder = folders.add('/TILE/OFL02/STATUS/ADC', 'TILE_OFL')
                offlineBadChannelsProxy = TileCondProxyCoolBch('TileCondProxyCool_OflBch', Source = oflineFolder)
            else:
                offlineBadChannelsProxy = None
        else:
            oflineFolder = folders.addSplitOnline('/TILE/OFL01/STATUS/ADC', '/TILE/OFL02/STATUS/ADC')
            offlineBadChannelsProxy = TileCondProxyCoolBch('TileCondProxyCool_OflBch', Source = oflineFolder)
        
        from IOVDbSvc.IOVDbSvcConfig import addFolderList
        acc.merge( addFolderList(flags, folders.get()) )

    else:
        # Connect FILE Tile conditions proxies to the algorithm
        TileCondProxyFileBch=CompFactory.getComp("TileCondProxyFile<TileCalibDrawerBch>")
        onlineBadChannelsProxy = TileCondProxyFileBch('TileCondProxyFile_OnlBch', Source = 'TileDefault.onlBch')
        offlineBadChannelsProxy = TileCondProxyFileBch('TileCondProxyFile_OflBch', Source = 'TileDefault.oflBch')
        

    TileCondIdTransforms=CompFactory.TileCondIdTransforms
    TileBadChannelsCondAlg=CompFactory.TileBadChannelsCondAlg
    badChannelsCondAlg = TileBadChannelsCondAlg( name = name,
                                                 OnlBchProxy = onlineBadChannelsProxy,
                                                 OflBchProxy = offlineBadChannelsProxy,
                                                 TileCondIdTransforms = TileCondIdTransforms(),
                                                 TileBadChannels = badChannels)

    acc.addCondAlgo(badChannelsCondAlg)

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge( TileCablingSvcCfg(flags) )
    
    return acc


def TileBadChanToolCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile bad channel tool
        Arguments:
        flags:  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source: -- source of Tile bad channels (COOL, FILE). Defaults to COOL.
        TileBadChannels -- name of Tile bad channels object. Defaults to TileBadChannels.
    """

    acc = ComponentAccumulator()

    kwargs.setdefault('Source', 'COOL')
    kwargs.setdefault('TileBadChannels', 'TileBadChannels')

    badChannels = kwargs['TileBadChannels']
    name = 'TileBadChanTool'

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge( TileGMCfg(flags) )
    
    from TileConditions.TileBadChannelsConfig import TileBadChannelsCondAlgCfg
    acc.merge( TileBadChannelsCondAlgCfg(flags, **kwargs) )

    TileBadChanTool=CompFactory.TileBadChanTool
    acc.setPrivateTools( TileBadChanTool(name, TileBadChannels = badChannels) )

    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    
    # Test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = ComponentAccumulator()

    badChanTool =  acc.popToolsAndMerge( TileBadChanToolCfg(ConfigFlags) )
    print(badChanTool)

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('IOVDbSvc'))
    acc.store( open('TileBadChannels.pkl','wb') )

    print('All OK')
