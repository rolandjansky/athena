from CaloCalibHitRec.CaloCalibHitRecConf import CaloCalibClusterTruthMapMakerAlgorithm
CaloCalibClusterTruthMapMakerAlgorithm = CaloCalibClusterTruthMapMakerAlgorithm()
topSequence += CaloCalibClusterTruthMapMakerAlgorithm

from CaloCalibHitRec.CaloCalibHitRecConf import CaloCalibClusterDecoratorAlgorithm
CaloCalibClusterDecoratorAlgorithm = CaloCalibClusterDecoratorAlgorithm()

from CaloCalibHitRec.CaloCalibHitRecConf import CaloCalibClusterTruthAttributerTool
CaloCalibClusterTruthAttributerTool = CaloCalibClusterTruthAttributerTool()
CaloCalibClusterDecoratorAlgorithm.TruthAttributerTool = CaloCalibClusterTruthAttributerTool
topSequence += CaloCalibClusterDecoratorAlgorithm
