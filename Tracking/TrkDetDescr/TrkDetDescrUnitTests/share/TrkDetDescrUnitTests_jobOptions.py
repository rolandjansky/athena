###############################################################
#
# Job options 
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Use McEventSelector so we can run with AthenaMP
import AthenaCommon.AtlasUnixGeneratorJob

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add top algorithms to be run
from TrkEventUnitTests.TrkEventUnitTestsConf import Trk__TrkParametersTest
job += Trk__TrkParametersTest( 'TrkParamtersTest' )   # 1 alg, named "TrkParamtersTest"

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
#import ServiceMgr
from AthenaCommon.AppMgr import ServiceMgr  
svcMgr = theApp.serviceMgr()
svcMgr.MessageSvc.OutputLevel = VERBOSE
svcMgr.MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.defaultLimit=1000000


# Output level for HelloAlg only (note name: instance, not type)
job.TrkParamtersTest.OutputLevel = INFO

# You can set the global output level on the message svc (not
# recommended) or by using the -l athena CLI parameter

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax = 10

#==============================================================
#
# End of job options file
#
###############################################################
