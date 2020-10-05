#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileTopoClusterMakerConfig.py
@brief Python configuration of Tile topo cluster maker algorithm for the Run III
'''
from AthenaCommon.SystemOfUnits import MeV, deg
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TileTopoClusterCfg(flags, **kwargs):

    acc = ComponentAccumulator()

    kwargs.setdefault('name', 'TileTopoClusterAlg')
    kwargs.setdefault('ClustersOutputName', 'TileTopoCluster')

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(flags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(flags))

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    # Schedule total noise cond alg
    acc.merge(CaloNoiseCondAlgCfg(flags, 'totalNoise'))

    # Configure Tile topo cluster maker
    from AthenaConfiguration.ComponentFactory import CompFactory
    topoClusterMaker = CompFactory.CaloTopoClusterMaker()
    topoClusterMaker.CellsName = 'AllCalo'
    topoClusterMaker.CalorimeterNames = ['TILE']
    # Cells from the following samplings will be able to form seeds.
    # By default no sampling is excluded
    topoClusterMaker.SeedSamplingNames = ['TileBar0', 'TileBar1', 'TileBar2',
                                          'TileExt0', 'TileExt1', 'TileExt2',
                                          'TileGap1', 'TileGap2', 'TileGap3']

    topoClusterMaker.NeighborOption = 'super3D'
    topoClusterMaker.RestrictHECIWandFCalNeighbors = False
    topoClusterMaker.CellThresholdOnEorAbsEinSigma = 0.0
    topoClusterMaker.NeighborThresholdOnEorAbsEinSigma = 2.0
    topoClusterMaker.SeedThresholdOnEorAbsEinSigma = 4.0

    topoClusterMaker.SeedCutsInAbsE = True
    topoClusterMaker.ClusterEtorAbsEtCut = 0.0 * MeV
    topoClusterMaker.TwoGaussianNoise = flags.Calo.TopoCluster.doTwoGaussianNoise

    kwargs['ClusterMakerTools'] = [topoClusterMaker]

    # Configure Tile topo cluster splitter
    topoClusterSpliter = CompFactory.CaloTopoClusterSplitter()
    topoClusterSpliter.SamplingNames = ['TileBar0', 'TileBar1', 'TileBar2',
                                        'TileExt0', 'TileExt1', 'TileExt2' ,
                                        'TileGap1', 'TileGap2', 'TileGap3']

    topoClusterSpliter.ShareBorderCells = True
    topoClusterSpliter.RestrictHECIWandFCalNeighbors = False

    kwargs['ClusterMakerTools'] += [topoClusterSpliter]

    # Configure Tile topo cluster moments maker
    clsuterMomentsMaker = CompFactory.CaloClusterMomentsMaker()
    clsuterMomentsMaker.MaxAxisAngle = 30 * deg
    clsuterMomentsMaker.MomentsNames = ['FIRST_PHI'
                                        , 'FIRST_ETA'
                                        , 'SECOND_R'
                                        , 'SECOND_LAMBDA'
                                        , 'DELTA_PHI'
                                        , 'DELTA_THETA'
                                        , 'DELTA_ALPHA'
                                        , 'CENTER_X'
                                        , 'CENTER_Y'
                                        , 'CENTER_Z'
                                        , 'CENTER_MAG'
                                        , 'CENTER_LAMBDA'
                                        , 'LATERAL'
                                        , 'LONGITUDINAL'
                                        , 'FIRST_ENG_DENS'
                                        , 'ENG_FRAC_EM'
                                        , 'ENG_FRAC_MAX'
                                        , 'ENG_FRAC_CORE'
                                        , 'FIRST_ENG_DENS'
                                        , 'SECOND_ENG_DENS'
                                        , 'ISOLATION'
                                        , 'ENG_BAD_CELLS'
                                        , 'N_BAD_CELLS'
                                        , 'N_BAD_CELLS_CORR'
                                        , 'BAD_CELLS_CORR_E'
                                        , 'BADLARQ_FRAC'
                                        , 'ENG_POS'
                                        , 'SIGNIFICANCE'
                                        , 'CELL_SIGNIFICANCE'
                                        , 'CELL_SIG_SAMPLING'
                                        , 'AVG_LAR_Q'
                                        , 'AVG_TILE_Q'
                                        , 'PTD'
                                        , 'MASS']


    kwargs['ClusterCorrectionTools'] = [clsuterMomentsMaker]

    acc.addEventAlgo(CompFactory.CaloClusterMaker(**kwargs), primary = True)

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
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    cfg.merge( TileTopoClusterCfg(ConfigFlags) )

    cfg.printConfig(withDetails = True, summariseProps = True)
    ConfigFlags.dump()

    cfg.store( open('TileTopoClusterMaker.pkl','wb') )

    sc = cfg.run(maxEvents=-1)

    import sys
    # Success should be 0
    sys.exit(not sc.isSuccess())
