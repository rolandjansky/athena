#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
# Add top algorithms to be run
from SCTExtension.SCTExtensionConf import SCTExtensionAlg
SCTExtension = SCTExtensionAlg( "SCTExtensionAlg" )   # 1 alg, named "MyNewPackage"
print SCTExtension
job += SCTExtension
#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
SCTExtension.OutputLevel = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
#theApp.EvtMax = 1
#=================================
