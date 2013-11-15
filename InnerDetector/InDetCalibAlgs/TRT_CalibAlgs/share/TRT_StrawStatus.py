#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
# Add top algorithms to be run
from TRT_CalibAlgs.TRT_CalibAlgsConf import InDet__TRT_StrawStatus
job += InDet__TRT_StrawStatus( "TRT_StrawStatus" ) 
#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
job.TRT_StrawStatus.OutputLevel = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
#theApp.EvtMax = -1
#==============================================================

