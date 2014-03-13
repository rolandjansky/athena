## EtmissGoodness alg
from MissingETGoodness.MissingETGoodnessConf import *

## This Athena job consists of algorithms that loop over events;
## here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()

job += EtmissGoodnessManagerAlg()
job.EtmissGoodnessManagerAlg.SetAndLockVersion = -1 # latest defined cut set
job.EtmissGoodnessManagerAlg.SetAndLockQuality = -1 # unset, fall back on medium

