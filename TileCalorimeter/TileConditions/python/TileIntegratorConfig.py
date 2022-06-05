# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define methods to construct configured Tile integrator conditions tool and algorithm"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileIntegratorCondAlgCfg(flags, **kwargs):
    """Return component accumulator with configured Tile integrator conditions algorithm

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile integrator conditions (COOL, FILE). Defaults to COOL.
        TileIntegrator -- name of Tile integrator conditions object. Defaults to TileIntegrator.
    """

    acc = ComponentAccumulator()

    source = kwargs.get('Source', 'COOL')
    integrator = kwargs.get('TileIntegrator', 'TileIntegrator')

    name = integrator + 'CondAlg'

    if source == 'COOL':
        # Connect COOL Tile conditions proxies to the algorithm (default)

        from TileConditions.TileFolders import TileFolders
        folders = TileFolders(isMC = flags.Input.isMC, isOnline = flags.Common.isOnline)

        if flags.IOVDb.DatabaseInstance  == 'CONDBR2':
            integratorFolder = folders.addSplitOnline('/TILE/ONL01/INTEGRATOR', '/TILE/OFL02/INTEGRATOR')
        else:
            integratorFolder = folders.addSplitOnline('/TILE/OFL01/INTEGRATOR', '/TILE/OFL02/INTEGRATOR')

        TileCondProxyCoolFlt=CompFactory.getComp("TileCondProxyCool<TileCalibDrawerFlt>")
        integratorProxy = TileCondProxyCoolFlt('TileCondProxyCool_Integrator', Source = integratorFolder)
        
        from IOVDbSvc.IOVDbSvcConfig import addFolderList
        acc.merge( addFolderList(flags, folders.get()) )

    elif source == 'FILE':
        # Connect FILE Tile conditions proxies to the algorithm
        TileCondProxyFileFlt=CompFactory.getComp("TileCondProxyFile<TileCalibDrawerFlt>")
        integratorProxy = TileCondProxyFileFlt('TileCondProxyFile_Integrator', Source = 'TileDefault.int')
    else:
        raise(Exception("Invalid source: %s" % source))

    TileCalibFltCondAlg=CompFactory.getComp("TileCalibCondAlg<TileCalibDrawerFlt>")
    integratorCondAlg = TileCalibFltCondAlg( name = name,
                                             ConditionsProxy = integratorProxy,
                                             TileCalibData = integrator)

    acc.addCondAlgo(integratorCondAlg)

    return acc


def TileCondToolIntegratorCfg(flags, **kwargs):
    """Return component accumulator with configured private Tile integrator tool
    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    Keyword arguments:
        Source -- source of Tile integrator conditions (COOL, FILE). Defaults to COOL.
        TileIntegrator -- name of Tile integrator conditions object. Defaults to TileIntegrator.
    """

    acc = ComponentAccumulator()

    kwargs.setdefault('Source', 'COOL')
    kwargs.setdefault('TileIntegrator', 'TileIntegrator')

    integrator = kwargs['TileIntegrator']
    name = 'TileCondToolIntegrator'

    acc.merge( TileIntegratorCondAlgCfg(flags, **kwargs) )

    TileCondToolIntegrator=CompFactory.TileCondToolIntegrator
    acc.setPrivateTools( TileCondToolIntegrator(name, TileIntegrator = integrator) )

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

    integratorTool = acc.popToolsAndMerge( TileCondToolIntegratorCfg(ConfigFlags) )
    print(integratorTool)

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('IOVDbSvc'))
    acc.store( open('TileIntegrator.pkl','wb') )

    print('All OK')
