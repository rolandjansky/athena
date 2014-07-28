from AthenaCommon.DetFlags import DetFlags
if not(DetFlags.pileup.any_on()):
    AthError( "this *pileup* job needs some DetFlags.pileup on" )

from AthenaCommon.Logging import logging
logConfigPileUpEventLoopMgr = logging.getLogger( 'ConfigPileUpEventLoopMgr' )

#--------------------------------------------------------------
# setup the Pile-Up Event Loop Mgr
#--------------------------------------------------------------
theApp.EventLoop = "PileUpEventLoopMgr"

from AthenaCommon import CfgGetter
from AthenaCommon.AppMgr import ServiceMgr

ServiceMgr += CfgGetter.getService('PileUpEventLoopMgr')
pileUpEventLoopMgr = ServiceMgr.PileUpEventLoopMgr
