from AthenaCommon.AlgSequence                  import AlgSequence
job = AlgSequence()
from AthenaCommon import CfgGetter
job += CfgGetter.getAlgorithm("ZDC_DigiTop/ZDC_DigiTop", tryDefaultConfigurable=True)
zdc = job.ZDC_DigiTop.DigitizationTool
