#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
print job
# Add top algorithms to be run
from TRT_DetectorPerformance.TRT_DetectorPerformanceConf import InDet__TRT_ClockNoise
job += InDet__TRT_ClockNoise( "TRT_ClockNoise" )   # 1 alg
#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
job.TRT_ClockNoise.OutputLevel = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
#theApp.EvtMax = 10
#==============================================================
