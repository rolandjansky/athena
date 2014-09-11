###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

###############################
# Load perf service
###############################
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.OutputFile = "athsequencer.pmon.gz"

# get a handle on the job main sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 10
    pass
theApp.EvtMax = EVTMAX

## create a py-alg
class RecoverableAlg(PyAthena.Alg):
    """
    a simple class to exercize the recoverable feature of the alg-sequence
    """
    def initialize(self):
        self.msg.info("configured to return RECOVERABLE after evt #5")
        self.evt_cnt = 0
        self.evt_cut = 5
        return PyAthena.StatusCode.Success

    def execute(self):
        self.msg.info("=> execute")
        self.evt_cnt += 1
        if self.evt_cnt >= self.evt_cut:
            self.msg.info("--> return RECOVERABLE")
            return PyAthena.StatusCode.Recoverable
        else:
            self.msg.info("--> return SUCCESS")
        return PyAthena.StatusCode.Success

    def finalize(self):
        self.msg.info("processed [%i] events", self.evt_cnt)
        return PyAthena.StatusCode.Success
    pass

topSequence += CfgMgr.AthEventCounter( 'Counter1' )
topSequence += RecoverableAlg("filter-alg")
topSequence += CfgMgr.AthEventCounter( 'Counter2' )

# change eventloopmgr behaviour to cut short the alg-sequence
# as soon as one algorithm returns RECOVERABLE
if not hasattr(svcMgr, theApp.EventLoop):
    svcMgr += getattr(CfgMgr, theApp.EventLoop)()
elm = getattr(svcMgr, theApp.EventLoop)
elm.FailureMode = 1

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
svcMgr.MessageSvc.defaultLimit = 4000000
#svcMgr.MessageSvc.OutputLevel  = ERROR
#==============================================================
#
# End of job options file
#
###############################################################
