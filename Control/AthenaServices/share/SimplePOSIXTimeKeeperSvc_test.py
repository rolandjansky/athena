## @file SimplePOSIXTimeKeeperSvc_test.py
# @brief  job opts for SimplePOSIXTimeKeeperSvc ATN test
#
# $Id: SimplePOSIXTimeKeeperSvc_test.py,v 1.4 2007-11-07 18:54:46 calaf Exp $

from AthenaPython.PyAthena import Alg
class WasteTimeAlg(Alg):
    def execute(self):
        printfunc ("waste some time")
        for i in range(200000) :
            foo = i*i/(i+1)/(i+2)
        return 1

waste=WasteTimeAlg("TimeWaster")

include( "AthenaServices/SimplePOSIXTimeKeeperOptions.py" )


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.Configurable import ConfigurablePyAlgorithm
top = AlgSequence()
top += waste 

from AthenaCommon.AppMgr import ServiceMgr
from AthenaServices.AthenaServicesConf import SimplePOSIXTimeKeeperSvc,AthenaEventLoopMgr

ServiceMgr += SimplePOSIXTimeKeeperSvc()
ServiceMgr.SimplePOSIXTimeKeeperSvc.OutputLevel = 1
ServiceMgr.SimplePOSIXTimeKeeperSvc.AllocTime = 6

ServiceMgr += AthenaEventLoopMgr(TimeKeeper="SimplePOSIXTimeKeeperSvc")

#from GaudiSvc.GaudiSvcConf import MessageSvc
#ServiceMgr.MessageSvc.OutputLevel = 2
