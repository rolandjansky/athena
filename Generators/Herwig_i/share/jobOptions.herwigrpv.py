###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# Private Application Configuration option
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 5
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["HERWIG 390020611 821000366", "HERWIG_INIT 820021 2347532"]

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from Herwig_i.HerwigRpv_iConf import HerwigRpv
job +=HerwigRpv()
job.HerwigRpv.HerwigCommand = ["iproc 11500","modpdf 10042", "autpdf HWLHAPDF", "msflag 1", "jmbug 0", "ptmin 10.", "iprint 2"]

#from TruthExamples.TruthExamplesConf import DumpMC
#job += DumpMC()

#==============================================================
#
# End of job options file
#
###############################################################
