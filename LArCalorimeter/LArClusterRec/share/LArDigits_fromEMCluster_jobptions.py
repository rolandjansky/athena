from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from LArClusterRec.LArClusterRecConf import LArDigitThinnerFromEMClust
theLArDigitThinnerFromEMClust = LArDigitThinnerFromEMClust("LArDigitThinnerFromEMClust")
theLArDigitThinnerFromEMClust.ClusterContainer = "LArClusterEM7_11Nocorr"

topSequence += theLArDigitThinnerFromEMClust
