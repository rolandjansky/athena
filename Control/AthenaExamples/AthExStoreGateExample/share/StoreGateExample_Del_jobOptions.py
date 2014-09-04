###############################################################
#
# AthExStoreGateExample Job options file showing how to delete selected
# data objects
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
theApp.EvtMax = 2


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

topSeq += CfgMgr.WriteData()
delAlg=CfgMgr.SGDeleteAlg()
delAlg.ToDelete.ItemList = ["MyDataObj#WriteData", "193758082#"]
delAlg.OutputLevel = VERBOSE
topSeq += delAlg

# setup output level
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr (svcMgr, 'StoreGateSvc'):
    svcMgr += CfgMgr.StoreGateSvc()
svcMgr.StoreGateSvc.OutputLevel = VERBOSE
svcMgr.StoreGateSvc.Dump = True  #True will dump data store contents

svcMgr.MessageSvc.useColors = True
#svcMgr.MessageSvc.OutputLevel = Lvl.DEBUG


#==============================================================
#
# End of AthExStoreGateExample Job options file
#
###############################################################
