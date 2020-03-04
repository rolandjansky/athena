## get a handle on the algsequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## add our alg
from AthenaCommon                       import CfgMgr
job += CfgMgr.SimpleCookedObjMaker()
job.SimpleCookedObjMaker.RawObjNames = ["reallyRaw"]
job.SimpleCookedObjMaker.CookedObjOutputName = "reallyCooked"
