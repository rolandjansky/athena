###############################################################
#
# FortranExample Python script
#
#==============================================================
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Get hold of the algorithms that we want to run
from AthExFortranAlgorithm.AthExFortranAlgorithmConf import FortranAlgorithm
#    ^^package name  ^^generated file           ^^Gaudi component

job += FortranAlgorithm(LUN=42, fileName="FortranAlgorithmInput.data")


#--------------------------------------------------------------
# job-level parameters
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = 1
MessageSvc.OutputLevel = INFO


