# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile pulse shape conditions tool and algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TilePulseShapeCondAlgCfg(flags, **kwargs):
    """Return component accumulator with configured Tile pulse shape conditions algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile pulse shape conditions (COOL, FILE). Defaults to COOL.
        TilePulseShape -- name of Tile pulse shape conditions object. Defaults to TilePulseShape.
        PulseType -- type of Tile pulse shape. Defaults to run type (PED->PHY, BILAS->LAS).
                     Possible Tile pulse shape types:
                        PHY, LAS, CIS, CIS/PULSE100, CIS/PULSE5P2, CIS/LEAK100, CIS/LEAK5P2, MURCV.
    """

    acc = ComponentAccumulator()

    runType = flags.Tile.RunType
    runType = runType.upper()
    if runType not in ['PHY', 'PED', 'CIS', 'MONOCIS', 'LAS', 'BILAS']:
        raise(Exception("Invalid Tile run type: %s" % runType))

    source = kwargs.get('Source', 'COOL')
    pulseShape = kwargs.get('TilePulseShape', 'TilePulseShape')
    pulseType = kwargs.get('PulseType', runType)

    actualPulseType = {'PHY' : 'PHY', 'PED' : 'PHY', 'LAS' : 'LAS', 'BILAS' : 'LAS', 'CIS' : 'CIS/PULSE100',
                       'CISPULSE100' : 'CIS/PULSE100', 'CISPULSE5P2' : 'CIS/PULSE5P2',
                       'CISLEAK100' : 'CIS/LEAK100', 'CISLEAK5P2' : 'CIS/LEAK5P2'}

    pulse = actualPulseType.get(pulseType, pulseType)

    if pulse not in ['PHY', 'LAS', 'CIS/PULSE100', 'CIS/PULSE5P2', 'CIS/LEAK100', 'CIS/LEAK5P2', 'MURCV']:
        raise(Exception("Invalid Tile pulse shape type: %s" % pulse))

    name = pulseShape + 'CondAlg'

    if source == 'COOL':
        # Connect COOL Tile conditions proxies to the algorithm (default)

        if pulse in ['MURCV']:
            raise(Exception('No Tile pulse shape [%s] in source: %s' % (pulse, source)))

        from TileConditions.TileFolders import TileFolders
        folders = TileFolders(isMC = flags.Input.isMC, isOnline = flags.Common.isOnline)

        if flags.IOVDb.DatabaseInstance  == 'CONDBR2':
            if not flags.Common.isOnline:
                pulseFolder = folders.add('/TILE/OFL02/PULSESHAPE/' + pulse, 'TILE_OFL')
            else:
                raise(Exception('No Tile pulse shapes in online CONDBR2'))
        else:
            pulseFolder = folders.addSplitOnline('/TILE/OFL01/PULSESHAPE/' + pulse, '/TILE/OFL02/PULSESHAPE/' + pulse)

        TileCondProxyCoolFlt=CompFactory.getComp("TileCondProxyCool<TileCalibDrawerFlt>")
        pulseProxy = TileCondProxyCoolFlt('TileCondProxyCool_PulseShape', Source = pulseFolder)

        from IOVDbSvc.IOVDbSvcConfig import addFolderList
        acc.merge( addFolderList(flags, folders.get()) )

    elif source == 'FILE':
        # Connect FILE Tile conditions proxies to the algorithm
        fileExtention = {'PHY' : 'plsPhy', 'LAS' : 'plsLas', 'MURCV' : 'plsMuRcv',
                         'CIS/PULSE100' : 'plsCisPulse100', 'CIS/PULSE5P2' : 'plsCisPulse5p2',
                         'CIS/LEAK100' : 'plsCisLeak100', 'CIS/LEAK5P2' : 'plsCisLeak5p2'}

        TileCondProxyFileFlt=CompFactory.getComp("TileCondProxyFile<TileCalibDrawerFlt>")
        pulseProxy = TileCondProxyFileFlt('TileCondProxyFile_PulseShape',
                                          Source = 'TileDefault.' + fileExtention[pulse])
    else:
        raise(Exception("Invalid source: %s" % source))

    TileCalibFltCondAlg=CompFactory.getComp("TileCalibCondAlg<TileCalibDrawerFlt>")
    pulseShapeCondAlg = TileCalibFltCondAlg( name = name,
                                             ConditionsProxy = pulseProxy,
                                             TileCalibData = pulseShape)

    acc.addCondAlgo(pulseShapeCondAlg)

    return acc


def TileCondToolPulseShapeCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile pulse shape conditions tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile pulse shape conditions (COOL, FILE). Defaults to COOL.
        TilePulseShape -- name of Tile pulse shape conditions object. Defaults to TilePulseShape.
        PulseType -- type of Tile pulse shape. Defaults to run type (PED->PHY, BILAS->LAS).
                     Possible Tile pulse shape types:
                        PHY, LAS, CIS/PULSE100, CIS/PULSE5P2, CIS/LEAK100, CIS/LEAK5P2, MURCV.
    """

    acc = ComponentAccumulator()

    kwargs.setdefault('Source', 'COOL')
    kwargs.setdefault('TilePulseShape', 'TilePulseShape')

    pulseShape = kwargs['TilePulseShape']
    name = 'TileCondToolPulseShape'

    acc.merge( TilePulseShapeCondAlgCfg(flags, **kwargs) )

    TileCondToolPulseShape=CompFactory.TileCondToolPulseShape
    acc.setPrivateTools( TileCondToolPulseShape(name, TilePulseShape = pulseShape) )

    return acc


def TileCondToolMuRcvPulseShapeCfg(flags, **kwargs):

    kwargs.setdefault('Source', 'FILE')
    kwargs.setdefault('TilePulseShape', 'TileMuRcvPulseShape')
    kwargs['PulseType'] = 'MURCV'

    pulseShape = kwargs['TilePulseShape']
    name = 'TileCondToolPulseShape'

    acc = TilePulseShapeCondAlgCfg(flags, **kwargs)

    TileCondToolPulseShape=CompFactory.TileCondToolPulseShape
    acc.setPrivateTools( TileCondToolPulseShape(name, TilePulseShape = pulseShape) )

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

    pulseShapeTool =  acc.popToolsAndMerge( TileCondToolPulseShapeCfg(ConfigFlags) )
    print(pulseShapeTool)

    muRcvPulseShapeTool =  acc.popToolsAndMerge( TileCondToolMuRcvPulseShapeCfg(ConfigFlags) )
    print(muRcvPulseShapeTool)

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('IOVDbSvc'))
    acc.store( open('TilePulseShape.pkl','wb') )

    print('All OK')

