###############################################################
#
# AthExStoreGateExample Job options file reading Generated events
#
#==============================================================
#import AthenaCommon.AtlasUnixStandardJob
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

theApp.MessageSvcType = "TBBMessageSvc"

from AthenaServices.AthenaServicesConf import AthenaHiveEventLoopMgr

svcMgr += AthenaHiveEventLoopMgr()
# svcMgr.AthenaHiveEventLoopMgr.OutputLevel = DEBUG

theApp.EventLoop = "AthenaHiveEventLoopMgr"

svcMgr.StatusCodeSvc.AbortOnError = False

numStores = 1

from StoreGate.StoreGateConf import SG__HiveMgrSvc
svcMgr += SG__HiveMgrSvc("EventDataSvc")
svcMgr.EventDataSvc.NSlots = numStores

from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
svcMgr += ForwardSchedulerSvc()
svcMgr.ForwardSchedulerSvc.OutputLevel = DEBUG
svcMgr.ForwardSchedulerSvc.MaxEventsInFlight = numStores
svcMgr.ForwardSchedulerSvc.MaxAlgosInFlight = 1
svcMgr.ForwardSchedulerSvc.ThreadPoolSize = 1

svcMgr.ForwardSchedulerSvc.AlgosDependencies = [[],['8000/WriteData'],[],[]]


svcMgr += AthenaHiveEventLoopMgr()
svcMgr.AthenaHiveEventLoopMgr.WhiteboardSvc = "EventDataSvc"
svcMgr.AthenaHiveEventLoopMgr.OutputLevel = DEBUG

from StoreGate.StoreGateConf import SG__HiveMgrSvc
svcMgr += SG__HiveMgrSvc("EventDataSvc")
svcMgr.EventDataSvc.NSlots = numStores
svcMgr.EventDataSvc.OutputLevel = DEBUG

from StoreGate.StoreGateConf import StoreGateSvc
svcMgr += StoreGateSvc()
svcMgr.StoreGateSvc.OutputLevel = VERBOSE
svcMgr.StoreGateSvc.Dump = True

from StoreGate.StoreGateConf import SGImplSvc
svcMgr += SGImplSvc("SGImplSvc")
svcMgr.SGImplSvc.OutputLevel = VERBOSE


# dummy event loop
#theApp.EvtSel = "NONE"
# Number of events to be processed (default is 10)
theApp.EvtMax = 500


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

topSeq += CfgMgr.WriteDataReentrant()
topSeq += CfgMgr.ReadDataReentrant()

# setup output level
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr (svcMgr, 'StoreGateSvc'):
    svcMgr += CfgMgr.StoreGateSvc()
svcMgr.StoreGateSvc.OutputLevel = VERBOSE
#svcMgr.StoreGateSvc.Dump = False  #True will dump data store contents

svcMgr.MessageSvc.useColors = True
#svcMgr.MessageSvc.OutputLevel = Lvl.DEBUG


#==============================================================
#
# End of AthExStoreGateExample Job options file
#
###############################################################
