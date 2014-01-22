from AthenaCommon.AlgSequence                  import AlgSequence
job = AlgSequence()
from AthenaCommon import CfgGetter
job += CfgGetter.getAlgorithm("AFP_DigiTop/AFP_DigiTop", tryDefaultConfigurable=True)
afp = job.AFP_DigiTop.DigitizationTool
