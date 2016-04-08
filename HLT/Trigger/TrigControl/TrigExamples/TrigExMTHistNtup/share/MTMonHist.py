###############################################################
#
# Job options file
#
#==============================================================
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *

svcMgr.THistSvc.OutputLevel = DEBUG

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
from TrigExMTHistNtup.TrigExMTHistNtupConf import MTTHist
topSequence = AlgSequence()
topSequence += MTTHist(OutputLevel = DEBUG)
