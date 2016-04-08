# helper alg to print PID
from ValgrindRTTJobs.ValgrindRTTJobsConf import ValgrindHelperAlg
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence.insert(0,ValgrindHelperAlg())
