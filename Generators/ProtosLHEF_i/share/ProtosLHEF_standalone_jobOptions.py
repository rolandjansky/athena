#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Full job is a list of algorithms

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add top algorithms to be run

from ProtosLHEF_i.ProtosLHEF_iConf import protos2lhef
job += protos2lhef( "ProtosLHEF_i" )

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

job.ProtosLHEF_i.OutputLevel = INFO

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is 10)

theApp.EvtMax = 10
