#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileMuIdConfig.py
@brief Python configuration of Tile look for mu algorithm for the Run III
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TileLookForMuAlgCfg(flags, **kwargs):

    acc = ComponentAccumulator()

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(flags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(flags))

    kwargs.setdefault('name', 'TileLookForMuAlg')
    kwargs.setdefault('TileMuTagsOutputName', 'TileMuObj')

    kwargs.setdefault('CellsNames', 'AllCalo')

    kwargs.setdefault('LowerTresh0MeV', 80.0)
    kwargs.setdefault('LowerTresh1MeV', 80.0)
    kwargs.setdefault('LowerTresh2MeV', 80.0)
    kwargs.setdefault('LowerTreshScinMeV', 160.0)


    # Configure energy thresholds in Tile D cells to look for muons
    nCellsD = 13
    UpperTresh2MeV=[2370.0, 2100.0, 900.0, 1050.0, 1050.0, 1110.0, 1590.0,
                    1110.0, 1050.0, 1050.0, 900.0,  2100.0, 2370.0]

    kwargs.setdefault('UpperTresh2MeV', UpperTresh2MeV)
    if len(kwargs['UpperTresh2MeV']) != nCellsD:
        raise RuntimeError('Energy thresholds for %s Tile D cells are needed' % nCellsD)

    # Configure energy thresholds in Tile BC cells to look for muons
    nCellsBC = 30
    UpperTresh1MeV=[1680.0, 1500.0, 1440.0, 1380.0, 1050.0, 390.0,
                    1110.0, 1860.0, 1890.0, 1800.0, 1860.0, 1890.0,
                    1770.0, 1980.0, 2550.0, 2550.0, 1980.0, 1770.0,
                    1890.0, 1860.0, 1800.0, 1890.0, 1860.0, 1110.0,
                    390.0,  1050.0, 1380.0, 1440.0, 1500.0, 1680.0]

    kwargs.setdefault('UpperTresh1MeV', UpperTresh1MeV)
    if len(kwargs['UpperTresh1MeV']) != nCellsBC:
        raise RuntimeError('Energy thresholds for %s Tile BC cells are needed' % nCellsBC)

    # Configure energy thresholds in Tile A cells to look for muons
    nCellsA = 30
    UpperTresh0MeV=[1680.0, 1380.0, 1230.0, 1140.0, 210.0, 900.0,
                    960.0,  840.0, 930.0,   840.0,  840.0, 750.0,
                    870.0,   960.0, 1350.0, 1350.0, 960.0, 870.0,
                    750.0,  840.0, 840.0,   930.0,  840.0,  960.0,
                    900.0,   210.0, 1140.0, 1230.0, 1380.0, 1680.0]

    kwargs.setdefault('UpperTresh0MeV', UpperTresh0MeV)
    if len(kwargs['UpperTresh0MeV']) != nCellsA:
        raise RuntimeError('Energy thresholds for %s Tile A cells are needed' % nCellsA)

    # Configure which Tile BC cells should be checked when looking for muons, starting from Tile D cells
    # Numbers are splitted by 6 [number of cells to be checked, followed by Tile BC cells to be checked]
    # For instance:  starting from the 7th cell in the 3rd (D) layer
    #                check 2 Tile BC cells: the 16th  and 17th in the 2nd (BC) layer etc.
    From3to2=[5,0,1,2,3,4,    5,2,3,4,5,6,    2,6,7,0,0,0,   3,7,8,9,0,0,
              3,9,10,11,0,0,  2,12,13,0,0,0,  2,14,15,0,0,0, 2,16,17,0,0,0,
              3,18,19,20,0,0, 3,20,21,22,0,0, 2,22,23,0,0,0, 5,23,24,25,26,27,
              5,25,26,27,28,29]

    kwargs.setdefault('From3to2', From3to2)
    if len(kwargs['From3to2']) != (nCellsD * 6):
        raise RuntimeError('6x%s numbers required for Tile D -> BC cells mapping' % nCellsD)

    # Configure which Tile A cells should be checked when looking for muons, starting from Tile BC cells
    # Numbers are splitted by 6 [number of cells to be checked, followed by Tile A cells to be checked]
    From2to1=[2,1,2,0,0,0,   3,1,2,3,0,0,    3,2,3,4,0,0,    3,3,4,5,0,0,    2,4,5,0,0,0,
              1,5,0,0,0,0,   1,6,0,0,0,0,    1,7,0,0,0,0,    1,8,0,0,0,0,    1,9,0,0,0,0,
              1,10,0,0,0,0,  1,11,0,0,0,0,   1,12,0,0,0,0,   1,13,0,0,0,0,   1,14,0,0,0,0,
              1,15,0,0,0,0,  1,16,0,0,0,0,   1,17,0,0,0,0,   1,18,0,0,0,0,   1,19,0,0,0,0,
              1,20,0,0,0,0,  1,21,0,0,0,0,   1,22,0,0,0,0,   1,23,0,0,0,0,   1,24,0,0,0,0,
              2,24,25,0,0,0, 3,24,25,26,0,0, 3,25,26,27,0,0, 3,26,27,28,0,0, 2,27,28,0,0,0]

    kwargs.setdefault('From2to1', From2to1)
    if len(kwargs['From2to1']) != (nCellsBC * 6):
        raise RuntimeError('6x%s numbers required for Tile BC -> A cells mapping' % nCellsBC)


    TileLookForMuAlg=CompFactory.TileLookForMuAlg
    acc.addEventAlgo(TileLookForMuAlg(**kwargs), primary = True)

    return acc


def TileLookForMuAlgOutputCfg(flags, streamName = 'ESD', **kwargs):

    acc = TileLookForMuAlgCfg(flags, **kwargs)
    lookForMuAlg = acc.getPrimary()

    muContainer = lookForMuAlg.TileMuTagsOutputName
    muContainer = muContainer.split('+').pop()
    outputItemList = ['TileMuContainer#' + muContainer]

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge( OutputStreamCfg(flags, streamName, ItemList = outputItemList) )

    return acc


if __name__=='__main__':

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.ESD
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    cfg.merge( TileLookForMuAlgCfg(ConfigFlags, TileMuTagsOutputName='TileMuObjTest') )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileLookForMuAlg.pkl','wb') )

    sc = cfg.run(maxEvents=3)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
