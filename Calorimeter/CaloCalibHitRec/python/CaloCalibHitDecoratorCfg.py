# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def CaloCalibHitDecoratorCfg():
    result=ComponentAccumulator()

    from CaloCalibHitRec.CaloCalibHitRecConf import CaloCalibClusterTruthMapMakerAlgorithm
    CaloCalibClusterTruthMapMakerAlgorithm = CaloCalibClusterTruthMapMakerAlgorithm()
    result.addEventAlgo(CaloCalibClusterTruthMapMakerAlgorithm)

    from CaloCalibHitRec.CaloCalibHitRecConf import CaloCalibClusterDecoratorAlgorithm
    CaloCalibClusterDecoratorAlgorithm = CaloCalibClusterDecoratorAlgorithm()

    from CaloCalibHitRec.CaloCalibHitRecConf import CaloCalibClusterTruthAttributerTool as CalibClusterTruthAttributerTool
    CaloCalibClusterTruthAttributerTool = CalibClusterTruthAttributerTool()
    CaloCalibClusterDecoratorAlgorithm.TruthAttributerTool = CaloCalibClusterTruthAttributerTool
    result.addEventAlgo(CaloCalibClusterDecoratorAlgorithm)

    return result