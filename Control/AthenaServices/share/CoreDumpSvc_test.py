# @file: AthenaServices/share/CoreDumpSvc_test.py
# @purpose: make sure we can reliably dump core w/ CoreDumpSvc
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    January 2010

import AthenaPython.PyAthena as PyAthena
StatusCode = PyAthena.StatusCode

class CoreDumpTestAlg (PyAthena.Alg):
    """simple algorithm to test the CoreDumpSvc capabilities
    """

    def __init__(self, name="CoreDumpTestAlg", **kw):
        kw['name'] = name
        super(CoreDumpTestAlg, self).__init__(**kw)

        # handle to CoreDumpSvc
        self._svc = None
        return

    def initialize(self):
        self.msg.info ('retrieving the coredump svc...')
        self._svc = PyAthena.py_svc('CoreDumpSvc', iface='IService')
        if self._svc is None:
            self.msg.error ('could not retrieve the coredump svc')
            return StatusCode.Failure

        import signal
        import os
        self.msg.info('sending SIGBUS to pid [%s]...', os.getpid())
        os.kill(os.getpid(), signal.SIGBUS)
        return StatusCode.Success

    def execute(self):
        return StatusCode.Success

    def finalize(self):
        return StatusCode.Success
    pass # CoreDumpTestAlg

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += CoreDumpTestAlg(OutputLevel = Lvl.INFO)

theApp.EvtMax = 1
