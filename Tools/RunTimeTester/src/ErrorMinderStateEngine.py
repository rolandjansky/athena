# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'State enegine for Error Minders'

from MinderStateEngineBase import MinderStateEngineBase


class ErrorMinderStateEngine(MinderStateEngineBase):
    """
    Empty state machine. This will be stepped by calls to poll(),
    and will end up in state = success. Different behaviour is obtained
    by subclassing the xxxTest and xxxAction methods.
    """
    
    def __init__(self, minder, state='queued'):
        MinderStateEngineBase.__init__(self, state, minder.identifiedName)
        self.dispatchTable = {
            'queued':            minder.fromQueued,
            'operatorCloseDown': minder.fromOperatorCloseDown,
            'done':              minder.fromDone
            }
        
