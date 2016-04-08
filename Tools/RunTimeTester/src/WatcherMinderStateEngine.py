# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from exc2string2            import exc2string2
from MinderStateEngineBase  import MinderStateEngineBase

import time

class WatcherMinderStateEngine(MinderStateEngineBase):
    """
    Empty state machine. This will be stepped by calls to poll(),
    and will end up in state = success. Different behaviour is obtained
    by subclassing the xxxTest and xxxAction methods.
    """

    
    def __init__(self, minder, state='queued'):
        MinderStateEngineBase.__init__(self, state, minder.identifiedName)

        self.dispatcher = {
            'queued'               :  minder.fromQueued,
            'running'              :  minder.fromRunning,
            'operatorCloseDown'    :  minder.fromOperatorCloseDown,
            'postProcessingQueued' :  minder.fromPostProcessingQueued,
            'postProcessingRunning':  minder.fromPostProcessingRunning,
            'done'                 :  minder.fromDone
            }

 
