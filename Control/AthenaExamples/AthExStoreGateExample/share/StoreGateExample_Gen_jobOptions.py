###############################################################
#
# AthExStoreGateExample Job options file reading Generated events
#
#==============================================================
#import AthenaCommon.AtlasUnixStandardJob
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
# dummy event loop
theApp.EvtSel = "NONE"
# Number of events to be processed (default is 10)
theApp.EvtMax = 3


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

topSeq += CfgMgr.WriteData()
topSeq += CfgMgr.ReadData(DataProducer = "WriteData")

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
