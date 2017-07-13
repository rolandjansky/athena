#--------------------------------------------------------------
# These jobOptions call PrintAlignmentPositions and can be included
#  in any running of InDetRecExample
#--------------------------------------------------------------
# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add top algorithms to be run
from InDetSimpleVisual.InDetSimpleVisualConf import GetDetectorPositions
job += GetDetectorPositions(name="GetDetectorPositions",
                            DetailLevel = 0,
                            DoTRT = False)

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
job.GetDetectorPositions.OutputLevel = INFO
