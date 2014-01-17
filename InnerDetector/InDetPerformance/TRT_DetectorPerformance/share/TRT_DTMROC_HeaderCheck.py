#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
# Add top algorithms to be run
from TRT_DetectorPerformance.TRT_DetectorPerformanceConf import InDet__TRT_DTMROC_HeaderCheck
job += InDet__TRT_DTMROC_HeaderCheck( "TRT_DTMROC_HeaderCheck" )   # 1 alg
#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
job.TRT_DTMROC_HeaderCheck.OutputLevel = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
#theApp.EvtMax = -1
#==============================================================
