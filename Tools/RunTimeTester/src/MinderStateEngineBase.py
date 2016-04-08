# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Holds state 
"""


from State          import State
from exc2string2    import exc2string2
from RTTSException  import RTTCodingError
from Tools          import nowSecs


#from Tracer      import Tracer

class MinderStateEngineBase:
    """
    Empty state machine. This will be stepped by calls to poll(),
    and will end up in state = success. Different behaviour is obtained
    by subclassing the xxxTest and xxxAction methods.
    """

    def __init__(self, state, identifiedName):

        self.state          = State(state)

        self.pollEntryState = state
        self.pollExitState  = state
        self.identifiedName = identifiedName
        self.dispatchTable  = {} # filled in by derived classes
        
    def poll(self):
        # m = '%s Start %s::poll() for %s' % (nowSecs(), self.__class__.__name__, self.identifiedName)
        # print m

        self.pollEntryState  = self.state.state

        try:
            newState = self.dispatchTable[self.pollEntryState]()
        except KeyError:
            msg = 'State %s  not in dispatch table:\n%s' % (self.pollEntryState, str(self.dispatchTable))
            print msg
            raise RTTCodingError(msg)
        except:
            msg = 'Error in poll(): Traceback:\n%s' % exc2string2()
            print msg
            raise RTTCodingError(msg)
                                                            

        self.state.set(newState)
        self.pollExitState   = self.state.state
        transition = self.pollEntryState != self.pollExitState

        # m = '%s End %s::poll() for %s %s->%s' % (nowSecs(), self.__class__.__name__, self.identifiedName, self.pollEntryState, self.pollExitState)
        # print m
        return  transition

        
    def closedDownByOperator(self): return self.state.state == 'operatorCloseDown'

    def unlink(self): self.dispatchTable.clear()

    # def isDone(self):
    #    isDone = self.state.isDone()
    #    # print '   %s %s::isDone %s, state %s' % (self.identifiedName, self.__class__.__name__,  str(isDone), self.state.state)
    #    return isDone
