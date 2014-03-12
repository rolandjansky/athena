
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AnalysisTriggerAlgs.AnalysisTriggerAlgsConfig import RoIBResultToAOD
topSequence += RoIBResultToAOD("RoIBResultToAOD")
