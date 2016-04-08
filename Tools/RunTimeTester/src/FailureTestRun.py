# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from MethodTimer              import MethodTimer
from MethodTracer        import MetaTracer

from TestRun import TestRun
import threading
# uncomment to monitor all method calls (shows args + return values)
# __metaclass__ = MetaTracer
# -------------------------------------------------------------------------
# -------------------------------------------------------------------------

class FailureTestRun(TestRun):
    'Created when a UnifiedTestRun cannot be created (due to some error).'
    
    # def __init__(self, packageName, paths, rttRunStartTime, closeDownKeyMaker, logger, failText, vetoFlag):
    def __init__(self, packageName, closedownKey, rttRunStartTime, closeDownKeyMaker, logger, failText, vetoFlag):
        # pass minders as an empty list
        TestRun.__init__(self, [], packageName, closedownKey, closeDownKeyMaker, logger, failText)

    # def groupsInTestRun(self):
    #    return []



# ------------------------------------------------------------------------- 

    def run(self):
        """split the jobs into various phases. Each phase runs to completion
         before starting the next one."""
        self.logger.info("FailureTestRun run method starts")
        self.xmlConverter.pushData()
        self.logger.info("FailureTestRun run method ends")

