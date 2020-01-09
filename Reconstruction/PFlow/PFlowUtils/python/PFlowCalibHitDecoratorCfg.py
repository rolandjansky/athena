# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def PFlowCalibHitDecoratorCfg():
    result=ComponentAccumulator()

    from CaloCalibHitRec.CaloCalibHitDecoratorCfg import CaloCalibHitDecoratorCfg 
    result.merge(CaloCalibHitDecoratorCfg())

    from PFlowUtils.PFlowUtilsConf import PFlowCalibPFODecoratorAlgorithm
    PFlowCalibPFODecoratorAlgorithm = PFlowCalibPFODecoratorAlgorithm()
    from CaloCalibHitRec.CaloCalibHitRecConf import CaloCalibClusterTruthAttributerTool as PFlowClusterTruthAttributerTool
    PFlowCalibPFOTruthAttributerTool = PFlowClusterTruthAttributerTool("PFlowCalibPFOTruthAttributerTool")
    PFlowCalibPFODecoratorAlgorithm.TruthAttributerTool = PFlowCalibPFOTruthAttributerTool
    result.addEventAlgo(PFlowCalibPFODecoratorAlgorithm)

    return result
