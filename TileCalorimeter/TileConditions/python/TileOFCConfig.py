# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile OFC conditions tool and algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileOFCCondAlgCfg(flags, **kwargs):
    """Return component accumulator with configured Tile OFC conditions algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile OFC conditions (COOL, FILE). Defaults to COOL.
        TileOfc -- name of Tile OFC conditions object starts with. Defaults to TileOfc. 
        OfcType -- type of Tile OFC. Defaults to OF2. Possible OFC types: OF1, OF2.
    """

    acc = ComponentAccumulator()

    runType = flags.Tile.RunType
    runType = runType.upper()
    if runType not in ['PHY', 'PED', 'CIS', 'MONOCIS', 'LAS', 'BILAS']:
        raise(Exception("Invalid Tile run type: %s" % runType))

    actualRunType = {'PHY' : 'PHY', 'PED' : 'PHY', 
                     'LAS' : 'LAS', 'BILAS' : 'LAS',
                     'CIS' : 'CIS', 'MONOCIS' : 'CIS'}

    runType = actualRunType.get(runType, runType)

    source = kwargs.get('Source', 'COOL')
    ofc = kwargs.get('TileOfc', 'TileOfc')
    ofcType = kwargs.get('OfcType', 'OF2')

    ofcType = ofcType.upper()
    if ofcType not in ['OF1', 'OF2']:
        raise(Exception("Invalid Tile OFC type: %s" % ofcType))

    name = ofc + 'CondAlg'

    if source == 'COOL':
        # Connect COOL Tile conditions proxies to the algorithm (default)

        from TileConditions.TileFolders import TileFolders
        folders = TileFolders(isMC = flags.Input.isMC, isOnline = flags.Common.isOnline)

        ofcType = ofcType + '/' + runType
        
        runNumber = flags.Input.RunNumber[0]
        runSplitOnline = 314449 #Use OFC stored in online folder for all runs before 2017
        if flags.IOVDb.DatabaseInstance  == 'CONDBR2' and runType == 'PHY' and runNumber > runSplitOnline:
            ofcFolder = folders.addSplitOnline('/TILE/ONL01/FILTER/' + ofcType, '/TILE/OFL02/FILTER/' + ofcType)
        else:
            ofcFolder = folders.addSplitMC('/TILE/ONL01/FILTER/' + ofcType, '/TILE/ONL01/FILTER/' + ofcType)

        TileCondProxyCoolOfc=CompFactory.getComp("TileCondProxyCool<TileCalibDrawerOfc>")
        ofcProxy = TileCondProxyCoolOfc('TileCondProxyCool_Ofc', Source = ofcFolder)
        
        from IOVDbSvc.IOVDbSvcConfig import addFolderList
        acc.merge( addFolderList(flags, folders.get()) )

    else:
        raise(Exception("Invalid source: %s" % source))

    TileCalibOfcCondAlg=CompFactory.getComp("TileCalibCondAlg<TileCalibDrawerOfc>")
    ofcCondAlg = TileCalibOfcCondAlg( name = name, ConditionsProxy = ofcProxy, TileCalibData = ofc)

    acc.addCondAlgo(ofcCondAlg)

    return acc


def TileCondToolOfcCoolCfg(flags, **kwargs):
    """Return component accumulator with configured Tile OFC conditions algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile OFC conditions (COOL, FILE). Defaults to COOL.
        TileOfc -- name of Tile OFC conditions. Defaults to TileOfc + OfcType (capitalized).
        OfcType -- type of Tile OFC. Defaults to OF2. Possible OFC types: OF1, OF2.
    """

    acc = ComponentAccumulator()

    kwargs.setdefault('Source', 'COOL')
    kwargs.setdefault('OfcType', 'OF2')

    ofcType = kwargs['OfcType']
    ofc = 'TileOfc' + ofcType.capitalize()

    kwargs.setdefault('TileOfc', ofc)

    ofc = kwargs['TileOfc']
    name = 'TileCondToolOfcCool'

    acc.merge( TileOFCCondAlgCfg(flags, **kwargs) )

    TileCondToolOfcCool=CompFactory.TileCondToolOfcCool
    acc.setPrivateTools( TileCondToolOfcCool(name, TileOfc = ofc) )

    return acc


def TileCondToolOfcCfg(flags, **kwargs):
    """Return component accumulator with configured Tile OFC conditions algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        OfcType -- type of Tile OFC. Defaults to OF2. Possible OFC types: OF1, OF2.
        OptFilterDeltaCorrelation -- flag to use delta correlation. Defaults to False.
                                     If it is False auto correlation obtained from data will be used.
        TileCondToolPulseShape - Tile conditions tool to get pulse shape.
                                 Provided it will be used. By default new one will be configured.
        TileCondToolAutoCr - Tile conditions tool to get auto correlation.
                             Provided it will be used. By default new one will be configured.
    """

    acc = ComponentAccumulator()

    optFilterDeltaCorrelation = kwargs.get('OptFilterDeltaCorrelation', False)

    from TileConditions.TileInfoLoaderConfig import TileInfoLoaderCfg
    acc.merge( TileInfoLoaderCfg(flags) )

    if 'TileCondToolPulseShape' in kwargs:
        pulseShapeTool = kwargs['TileCondToolPulseShape']
    else:
        from TileConditions.TilePulseShapeConfig import TileCondToolPulseShapeCfg
        pulseShapeTool = acc.popToolsAndMerge( TileCondToolPulseShapeCfg(flags) )

    if optFilterDeltaCorrelation:
        autoCorrelationTool = None
    else:
        if 'TileCondToolAutoCr' in kwargs:
            autoCorrelationTool = kwargs['TileCondToolAutoCr']
        else:
            from TileConditions.TileAutoCorrelationConfig import TileCondToolAutoCrCfg
            autoCorrelationTool = acc.popToolsAndMerge( TileCondToolAutoCrCfg(flags) )

    name = 'TileCondToolOfc'
    TileCondToolOfc=CompFactory.TileCondToolOfc
    acc.setPrivateTools( TileCondToolOfc(name,
                                         OptFilterDeltaCorrelation = optFilterDeltaCorrelation,
                                         TileCondToolPulseShape = pulseShapeTool,
                                         TileCondToolAutoCr = autoCorrelationTool) )

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
    ConfigFlags.lock()

    acc = ComponentAccumulator()

    ofcCoolTool = acc.popToolsAndMerge( TileCondToolOfcCoolCfg(ConfigFlags) )
    print(ofcCoolTool)

    ofcTool = acc.popToolsAndMerge( TileCondToolOfcCfg(ConfigFlags) )
    print(ofcTool)

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('IOVDbSvc'))
    acc.store( open('TileOFC.pkl','wb') )

    print('All OK')

