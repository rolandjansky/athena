# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
A series of classes that queries something.
The query is a shell command.
Replies can be expected and OK, undesired but known about, or a complete
surprise.

This module also has specialisation classes for querying batch queues,
and which have different behaviours if the answer to the query is not the
desired one.
"""

# import commands
import time, string
from ShellCommand import ShellCommand

class Querier:
    def __init__(self, logger, queryCmd):
        self.queryCmd  = queryCmd
        self.status    = 0
        self.output    = ''
        self.logger    = logger

    def query(self):

        try:
            sc = ShellCommand([self.queryCmd], self.logger, 100, None, True)
        except:
            self.status = -1
            self.output = 'query %s failed' % self.queryCmd
            return
        
        self.status = 0
        self.output = string.join(sc.getReply(), '\n')
        # self.status, self.output = commands.getstatusoutput('bjobs -q atlasrttmedium')

    def statusAndReply(self):
        if self.status:
            return (False, 'bad command status: %s output: %s' % (str(self.status), self.output))
        else:
            return (True, self.output)
        
class SleepingQuerier(Querier):
    def __init__(self, logger, queryCmd, maxtry, sleepTime):
        
        self.maxtry    = maxtry
        self.sleepTime = sleepTime
        Querier.__init__(self, logger, queryCmd)


    def statusAndReply(self):
        
        mtry = self.maxtry
        while mtry:
            self.query()
            ok, comment = Querier.statusAndReply(self)
            if not ok:
                self.logger.debug('cmd %s: query failure %d sleeping %d' % (self.queryCmd,
                                                                            self.maxtry-mtry,
                                                                            self.sleepTime))
                self.logger.debug('status %s reply %s' % (ok, comment))
                time.sleep(self.sleepTime)
                mtry -= 1
            else:
                return (ok, comment)

        return (ok, comment)


class LSF_bjobs1(SleepingQuerier):
    def __init__(self, logger, queue, maxtry = 3, sleepTime = 5):
        
        queryCmd = 'bqueues ' + queue
        SleepingQuerier.__init__(self, logger, queryCmd, maxtry, sleepTime)

    def statusAndReply(self):
        (status, comment) = SleepingQuerier.statusAndReply(self)
        if not status: return (status, comment)
        if self.output.find('No such queue')<0: return (True, self.output)
        return (False, self.output)

class LSF_bjobs2(SleepingQuerier):
    def __init__(self, logger, pid, maxtry = 3, sleepTime = 5):
        
        queryCmd = 'bjobs -l ' + str(pid) + ';'
        SleepingQuerier.__init__(self, logger, queryCmd, maxtry, sleepTime)

    def statusAndReply(self):
        (status, comment) = SleepingQuerier.statusAndReply(self)
        if not status: return (status, comment)
        if not self.output.find('The CPU time used is'): return (False, 'noCPUTime')
        return (True, self.output)

class LSF_bjobs3(SleepingQuerier):
    def __init__(self, logger, pid, maxtry = 3, sleepTime = 5):
        
        queryCmd = 'bjobs -a ' + str(pid) + ';'
        SleepingQuerier.__init__(self, logger, queryCmd, maxtry, sleepTime)

    def statusAndReply(self):
        (status, comment) = SleepingQuerier.statusAndReply(self)
        if not status: return (status, comment)
        tokens = self.output.split('\n')
        if len(tokens)<2: return (False, 'reply has bad format')
        tokens = tokens[1].split()
        if len(tokens)<3: return (False, 'reply has bad format')
        return (True, tokens[2])


class LSF_bhist(SleepingQuerier):
    def __init__(self, logger, pid, maxtry = 3, sleepTime = 5):
        
        queryCmd = 'bhist ' + str(pid) + ';'
        SleepingQuerier.__init__(self, logger, queryCmd, maxtry, sleepTime)

    def statusAndReply(self): return SleepingQuerier.statusAndReply(self)


