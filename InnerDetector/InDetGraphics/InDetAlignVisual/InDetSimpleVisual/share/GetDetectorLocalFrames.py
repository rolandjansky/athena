#--------------------------------------------------------------
# These jobOptions call PrintAlignmentLocalFrames and can be included
#  in any running of InDetRecExample
#--------------------------------------------------------------
# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add top algorithms to be run
from InDetSimpleVisual.InDetSimpleVisualConf import GetDetectorLocalFrames
job += GetDetectorLocalFrames(name="GetDetectorLocalFrames")

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
job.GetDetectorLocalFrames.OutputLevel = ERROR
