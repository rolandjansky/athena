###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from AthenaServices.AthenaServicesConf import AtDSFMTGenSvc
ServiceMgr += AtDSFMTGenSvc("AtDSFMTGenSvc")

# Add top algorithms to be run
from  AtlasCLHEP_RandomGenerators.AtlasCLHEP_RandomGenerators_testConf import AtlasCLHEP_RandomGenerators__AtlasCLHEP_RandomGenerators_test
job += AtlasCLHEP_RandomGenerators__AtlasCLHEP_RandomGenerators_test("AtlasCLHEP_RandomGenerators_test")   

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax = 1

#--------------------------------------------------------------
# Algorithms Private Options (all optional)
#--------------------------------------------------------------

# For convenience, get a reference to the HelloAlg Algorithm
# named "HelloWorld" in the job

AtlasCLHEP_RandomGenerators_test = job.AtlasCLHEP_RandomGenerators_test
AtlasCLHEP_RandomGenerators_test.OutputLevel = DEBUG

#-------------------------------------------------------------------------
# Histogram and Tree Service
#-------------------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output+= [ "PLOTS DATAFILE='%s' OPT='RECREATE'" % "random_gen.root" ]

#==============================================================
#
# End of job options file
#
###############################################################

