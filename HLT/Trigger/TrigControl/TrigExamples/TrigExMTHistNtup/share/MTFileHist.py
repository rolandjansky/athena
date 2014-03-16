###############################################################
#
# Job options file
#
#==============================================================
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *

from TrigL2Services.Lvl2THistSvcConfig import Lvl2THistSvcConfig
THistSvc = Lvl2THistSvcConfig()
THistSvc.OutputLevel = DEBUG
ServiceMgr += THistSvc

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from TrigExMTHistNtup.TrigExMTHistNtupConf import MTTHist
MTHist = MTTHist()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += MTHist

MTHist.OutputLevel = DEBUG


print ServiceMgr
