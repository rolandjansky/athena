# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile MuID conditions tool and algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileMuIDCondAlgCfg(flags, **kwargs):
    """Return component accumulator with configured Tile MuID conditions algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile MuID conditions (COOL, FILE). Defaults to COOL.
        TileMuID -- name of Tile MuID conditions object. Defaults to TileMuID.
    """

    acc = ComponentAccumulator()

    source = kwargs.get('Source', 'COOL')
    muID = kwargs.get('TileMuID', 'TileMuID')

    name = muID + 'CondAlg'

    if source == 'COOL':
        # Connect COOL Tile conditions proxies to the algorithm (default)

        from TileConditions.TileFolders import TileFolders
        folders = TileFolders(isMC = flags.Input.isMC, isOnline = flags.Common.isOnline)

        muIDFolder = folders.addSplitMC('/TILE/ONL01/MUID', '/TILE/ONL01/MUID')

        TileCondProxyCoolFlt=CompFactory.getComp("TileCondProxyCool<TileCalibDrawerFlt>")
        muIDProxy = TileCondProxyCoolFlt('TileCondProxyCool_MuID', Source = muIDFolder)
        
        from IOVDbSvc.IOVDbSvcConfig import addFolderList
        acc.merge( addFolderList(flags, folders.get()) )

    elif source == 'FILE':
        # Connect FILE Tile conditions proxies to the algorithm
        TileCondProxyFileFlt=CompFactory.getComp("TileCondProxyFile<TileCalibDrawerFlt>")
        muIDProxy = TileCondProxyFileFlt('TileCondProxyFile_MuID', Source = 'TileDefault.muid')
    else:
        raise(Exception("Invalid source: %s" % source))

    TileCalibFltCondAlg=CompFactory.getComp("TileCalibCondAlg<TileCalibDrawerFlt>")
    muIDCondAlg = TileCalibFltCondAlg( name = name,
                                       ConditionsProxy = muIDProxy,
                                       TileCalibData = muID)

    acc.addCondAlgo(muIDCondAlg)

    return acc


def TileCondToolMuIDCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile MuID conditions tool
    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile MuID conditions (COOL, FILE). Defaults to COOL.
        TileMuID -- name of Tile MuID conditions object. Defaults to TileMuID.
    """

    acc = ComponentAccumulator()

    kwargs.setdefault('Source', 'COOL')
    kwargs.setdefault('TileMuID', 'TileMuID')

    muID = kwargs['TileMuID']
    name = 'TileCondToolMuID'

    acc.merge( TileMuIDCondAlgCfg(flags, **kwargs) )

    TileCondToolMuID=CompFactory.TileCondToolMuID
    acc.setPrivateTools( TileCondToolMuID(name, TileMuID = muID) )

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

    muIDTool =  acc.popToolsAndMerge( TileCondToolMuIDCfg(ConfigFlags) )
    print(muIDTool)

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('IOVDbSvc'))
    acc.store( open('TileMuID.pkl','wb') )

    print('All OK')

