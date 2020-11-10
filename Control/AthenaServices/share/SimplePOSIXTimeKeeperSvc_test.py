## @file SimplePOSIXTimeKeeperSvc_test.py
# @brief  job opts for SimplePOSIXTimeKeeperSvc ATN test
#

from AthenaPython.PyAthena import Alg
class WasteTimeAlg(Alg):
    def execute(self):
        print ("waste some time")
        for i in range(200000) :
            foo = i*i/(i+1)/(i+2)
        return 1

waste=WasteTimeAlg("TimeWaster")

include( "AthenaServices/SimplePOSIXTimeKeeperOptions.py" )


from AthenaCommon.AlgSequence import AlgSequence
top = AlgSequence()
top += waste 

from AthenaCommon.AppMgr import ServiceMgr
from AthenaServices.AthenaServicesConf import SimplePOSIXTimeKeeperSvc,AthenaEventLoopMgr

ServiceMgr += SimplePOSIXTimeKeeperSvc()
ServiceMgr.SimplePOSIXTimeKeeperSvc.OutputLevel = 1
ServiceMgr.SimplePOSIXTimeKeeperSvc.AllocTime = 6

ServiceMgr += AthenaEventLoopMgr(TimeKeeper="SimplePOSIXTimeKeeperSvc")
