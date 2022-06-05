# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile DCS tool and conditions algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileDCSCondAlgCfg(flags, **kwargs):
    """Return component accumulator with configured Tile DCS conditions algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        ReadHV -- Read Tile DCS HV folder from DB. Defaults True.
        ReadHVSET -- Read Tile DCS HVSET folder from DB. Defaults to False.
        ReadSTATES -- Read Tile DCS STATES folder from DB. Defaults to True.
        TileDCS -- name of Tile DCS conditions object. Defaults to TileDCS.
    """

    isMC = flags.Input.isMC
    isOnline = flags.Common.isOnline

    if isOnline or isMC:
        raise(Exception('No Tile DCS information in online [%s] or MC [%s]' % (isOnline, isMC)))

    acc = ComponentAccumulator()

    dcs = kwargs.get('TileDCS', 'TileDCS')
    readHV = kwargs.get('ReadHV', True)
    readHVSET = kwargs.get('ReadHVSET', False)
    readSTATES = kwargs.get('ReadSTATES', True)

    name = dcs + 'CondAlg'

    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge( TileCablingSvcCfg(flags) )

    if not readHVSET:
        from TileConditions.TileEMScaleConfig import TileEMScaleCondAlgCfg
        acc.merge( TileEMScaleCondAlgCfg(flags) )

    from TileConditions.TileFolders import TileFolders
    folders = TileFolders(isMC = flags.Input.isMC, isOnline = flags.Common.isOnline)

    db = 'DCS_OFL'
    if readHV:
        folders.add('/TILE/DCS/HV', db)
    if readHVSET:
        folders.add('/TILE/DCS/HVSET', db)
    if readSTATES:
        folders.add('/TILE/DCS/STATES', db)

    from IOVDbSvc.IOVDbSvcConfig import addFolderList
    acc.merge( addFolderList(flags, folders.get()) )

    TileDCSCondAlg=CompFactory.TileDCSCondAlg
    dcsCondAlg = TileDCSCondAlg( name = name,
                                 ReadHV = readHV,
                                 ReadHVSET = readHVSET,
                                 ReadSTATES = readSTATES,
                                 TileDCS = dcs)

    acc.addCondAlgo(dcsCondAlg)

    return acc


def TileDCSToolCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile DCS tool
    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        ReadHV -- Read Tile DCS HV folder from DB. Defaults True.
        ReadHVSET -- Read Tile DCS HVSET folder from DB. Defaults to False.
        ReadSTATES -- Read Tile DCS STATES folder from DB. Defaults to True.
        TileDCS -- name of Tile DCS conditions object. Defaults to TileDCS.
    """

    acc = ComponentAccumulator()

    kwargs.setdefault('TileDCS', 'TileDCS')
    kwargs.setdefault('ReadHV', True)
    kwargs.setdefault('ReadHVSET', False)
    kwargs.setdefault('ReadSTATUS', True)

    dcs = kwargs['TileDCS']

    acc.merge( TileDCSCondAlgCfg(flags, **kwargs) )

    TileDCSTool=CompFactory.TileDCSTool
    acc.setPrivateTools( TileDCSTool(TileDCS = dcs) )

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

    dcsTool =  acc.popToolsAndMerge( TileDCSToolCfg(ConfigFlags) )
    print(dcsTool)

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('IOVDbSvc'))
    acc.store( open('TileDCS.pkl','wb') )

    print('All OK')
