## @file SimplePOSIXTimeKeeperSvc_test.py
# @brief  job opts for SimplePOSIXTimeKeeperSvc ATN test
#
# $Id: SimplePOSIXTimeKeeperSvc_test.py,v 1.4 2007-11-07 18:54:46 calaf Exp $

from gaudimodule import PyAlgorithm
class WasteTimeAlg(PyAlgorithm):
    def execute(self):
        print "waste some time"
        for i in range(200000) :
            foo = i*i/(i+1)/(i+2)
        return 1

waste=WasteTimeAlg()
import ROOT
ROOT.SetOwnership(waste,0)

include( "AthenaServices/SimplePOSIXTimeKeeperOptions.py" )


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.Configurable import ConfigurablePyAlgorithm
top = AlgSequence()
top += ConfigurablePyAlgorithm('WasteTimeAlg')

from AthenaCommon.AppMgr import ServiceMgr
from AthenaServices.AthenaServicesConf import SimplePOSIXTimeKeeperSvc,AthenaEventLoopMgr

ServiceMgr += SimplePOSIXTimeKeeperSvc()
ServiceMgr.SimplePOSIXTimeKeeperSvc.OutputLevel = 1
ServiceMgr.SimplePOSIXTimeKeeperSvc.AllocTime = 6

ServiceMgr += AthenaEventLoopMgr(TimeKeeper="SimplePOSIXTimeKeeperSvc")

from GaudiSvc.GaudiSvcConf import MessageSvc
ServiceMgr.MessageSvc.OutputLevel = 2
