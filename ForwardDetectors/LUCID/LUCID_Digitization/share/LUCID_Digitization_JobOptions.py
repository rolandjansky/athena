from AthenaCommon.AlgSequence                  import AlgSequence
job = AlgSequence()
from AthenaCommon import CfgGetter
job += CfgGetter.getAlgorithm("LUCID_DigiTop/LUCID_DigiTop", tryDefaultConfigurable=True)
lucid = job.LUCID_DigiTop
