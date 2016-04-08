## get a handle on the algsequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## add our algorithm
job += CfgMgr.SimpleRawObjMaker()
job.SimpleRawObjMaker.RawObjOutputName = "reallyRaw"
