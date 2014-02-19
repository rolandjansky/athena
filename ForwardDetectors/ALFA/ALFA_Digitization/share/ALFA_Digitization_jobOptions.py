from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from AthenaCommon import CfgGetter
job += CfgGetter.getAlgorithm("ALFA_DigiAlg/ALFA_DigiAlg", tryDefaultConfigurable=True)
alfa = job.ALFA_DigiAlg.DigitizationTool
