#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

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

numStores = 1

from StoreGate.StoreGateConf import SG__HiveMgrSvc
svcMgr += SG__HiveMgrSvc("EventDataSvc")
svcMgr.EventDataSvc.NSlots = numStores

#svcMgr.ForwardSchedulerSvc.AlgosDependencies = [[],['8000/WriteData'],[],[]]

from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.setThreadPoolSize( 1 )

svcMgr += AthenaHiveEventLoopMgr()
svcMgr.AthenaHiveEventLoopMgr.WhiteboardSvc = "EventDataSvc"
svcMgr.AthenaHiveEventLoopMgr.OutputLevel = DEBUG
svcMgr.AthenaHiveEventLoopMgr.SchedulerSvc = AlgScheduler.getScheduler().getName()

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
