#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileTowerBuilderConfig.py
@brief Python configuration of Tile Tower builder algorithm for the Run III
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
def TileTowerBuilderAlgCfg(flags, **kwargs):

    acc = ComponentAccumulator()

    kwargs.setdefault('name', 'TileTowerBldrAlg')
    kwargs.setdefault('TowerContainerName', 'TileTower')
    kwargs.setdefault('NumberOfPhiTowers', 64)
    kwargs.setdefault('NumberOfEtaTowers', 34)
    kwargs.setdefault('EtaMin', -1.7)
    kwargs.setdefault('EtaMax', 1.7)

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(flags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(flags))

    TileTowerBuilderTool=CompFactory.TileTowerBuilderTool
    tileCmbTwrBldr = TileTowerBuilderTool( name = 'TileCmbTwrBldr',
                                           CellContainerName = 'AllCalo',
                                           IncludedCalos = ['TILE'])

    kwargs['TowerBuilderTools'] = [ tileCmbTwrBldr ]

    CaloTowerAlgorithm=CompFactory.CaloTowerAlgorithm
    acc.addEventAlgo(CaloTowerAlgorithm(**kwargs), primary = True)

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
    ConfigFlags.Output.HISTFileName = 'TileTowerMonitorOutput.root'
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    cfg.merge( TileTowerBuilderAlgCfg(ConfigFlags) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileTowerBuilder.pkl','wb') )

    sc = cfg.run(maxEvents=-1)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
