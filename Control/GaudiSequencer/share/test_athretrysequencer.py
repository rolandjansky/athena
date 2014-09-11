###############################################################
#
# Job options file
#
#==============================================================

import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# get a handle on the job main sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 10
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Sequence(s)
#--------------------------------------------------------------

## Sequencer configuration ##
job += CfgMgr.AthRetrySequencer ('seq', MaxRetries = 5)

import AthenaPython.PyAthena as PyAthena

class PyPush(PyAthena.Alg):
    def __init__(self, name='PyPush', **kw):
        ## init base class
        kw['name'] = name
        super(PyPush, self).__init__(**kw)
    def initialize(self):
        self.evts = 0
        return PyAthena.StatusCode.Success
    def execute(self):
        self.setFilterPassed(True)
        self.evts += 1
        self.msg.info("events seen: %s" % self.evts)
        return PyAthena.StatusCode.Success
    def finalize(self):
        self.msg.info("finalize ==> total events: %s" % self.evts)
        return PyAthena.StatusCode.Success
    pass

class PyPull(PyAthena.Alg):
    def __init__(self, name='PyPull', **kw):
        ## init base class
        kw['name'] = name
        super(PyPull, self).__init__(**kw)

    def initialize(self):
        self.evts = 0
        return PyAthena.StatusCode.Success
    def execute(self):
        self.evts += 1
        self.setFilterPassed(True)
        import random
        if random.random() < 0.8:
            self.msg.info("requesting more events!!")
            self.setFilterPassed(False)
        else:
            self.msg.info("event quite satisfying...")
        self.msg.info("seen %s event(s)" % self.evts)
        return PyAthena.StatusCode.Success
    def finalize(self):
        self.msg.info("finalize ==> total events: %s" % self.evts)
        return PyAthena.StatusCode.Success
    pass

job.seq += PyPush("push", OutputLevel = Lvl.INFO)
job.seq += PyPull("pull", OutputLevel = Lvl.INFO)

job += CfgMgr.AthEventCounter("counter")

#svcMgr.MessageSvc.OutputLevel = Lvl.INFO
#==============================================================
#
# End of job options file
#
###############################################################
