# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""State Engine for LinuxJobMinders: extends WorkerJobMinderStateEngine

overrides: fromQueuedTest
           fromRunningTest

"""

from MinderStateEngineBase  import MinderStateEngineBase

class BatchMinderStateEngine(MinderStateEngineBase):
    
    def __init__(self, minder, state = 'queued'):
        MinderStateEngineBase.__init__(self, state, minder.identifiedName)
        self.dispatchTable =  {
            'queued':                minder.fromQueued,
            'running':               minder.fromRunning,
            'operatorCloseDown':     minder.fromOperatorCloseDown,
            'postProcessingQueued':  minder.fromPostProcessingQueued,
            'postProcessingRunning': minder.fromPostProcessingRunning,
            'operatorCloseDown':     minder.fromOperatorCloseDown,
            'retry':                 minder.fromRetry,
            'batchError':            minder.fromBatchError,
            'timedOut':              minder.fromTimedOut,
            'done':                  minder.fromDone
            }

