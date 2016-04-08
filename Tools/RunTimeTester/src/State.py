# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Timer import timer
"""
A class to hold status information.
Contains two statuses: primay and secondary.
Primary status    = job monitor status
Secondary status  = check status (for, eg regression tests).
A time of every set operation is noted.
"""

class State:
    def __init__(self, state):
        self.history = []
        self.state = ''
        self.set(state)
    def set(self, state):
        if state != self.state:
            self.state = state
            self.timeStamp = timer()
            self.history.append( (self.state, self.timeStamp) )

    def isDone(self):
        return self.state == 'done'
