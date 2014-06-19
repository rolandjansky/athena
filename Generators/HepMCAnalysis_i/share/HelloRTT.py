###############################################################
#
# Job options file
#
#==============================================================

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add top algorithms to be run
from AthExHelloWorld.AthExHelloWorldConf import HelloAlg

theApp.EvtMax = 1

