#==============================================================
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
# Add top algorithms to be run
from LArCalibDataQuality.LArCalibDataQualityConf import LArRampValidationAlg
theLArRampValidationAlg = LArRampValidationAlg( "LArRampValidationAlg" ) 
theLArRampValidationAlg.KeyList                  = GainList
theLArRampValidationAlg.RawRampTimeTolerance     = 1.    # in ns
theLArRampValidationAlg.RampTolerance            = 0.005 # in %
theLArRampValidationAlg.RampToleranceFEB         = 0.001 # in % 
theLArRampValidationAlg.IgnoreBadChannels        = False # Don't use bad-channel tool for now
job += theLArRampValidationAlg
#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
#job.LArRampValidationAlg.OutputLevel = ERROR
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
#theApp.EvtMax = 1
#==============================================================
