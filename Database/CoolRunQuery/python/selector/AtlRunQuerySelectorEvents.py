# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from __future__ import print_function
from time import time
import sys

from CoolRunQuery.utils.AtlRunQueryIOV   import IOVRange
from CoolRunQuery.utils.AtlRunQueryTimer import timer
from CoolRunQuery.utils.AtlRunQueryUtils import coolDbConn, GetRanges

from .AtlRunQuerySelectorBase import RunLBBasedCondition

class EventSelector(RunLBBasedCondition):
    def __init__(self, name, events=None):
        self.events = events
        super(EventSelector,self).__init__(name=name,
                                           dbfolderkey='oracle::SFO-based',
                                           channelKeys = [(0,'#Events','EFEvents')])
        if events:
            self.cutRange = GetRanges(events)        
        


    def __str__(self):
        if self.applySelection:
            return "SELOUT Checking if number of events matches %s" % self.events
        else:
            return "Retrieving event numbers"

    def select(self, runlist):

        # some preparation: compile the show patterns
        start = time()

        print (self, end='')
        sys.stdout.flush()
        newrunlist = []
        connection = coolDbConn.GetSFODBConnection()
        cursor     = connection.cursor()
        cursor.arraysize = 1000
        
        runnrlist = [r.runNr for r in runlist]

        from CoolRunQuery.AtlRunQuerySFO import GetSFO_NphysicseventsAll
        with timer("GetSFO_NphysicseventsAll", disabled=True):
            events = GetSFO_NphysicseventsAll( cursor, runnrlist )  # { runnr: nPhysEvents }

        for run in runlist:

            iov = IOVRange(runStart=run.runNr, lbStart=1, runEnd=run.runNr+1, lbEnd=0)

            for k in self.ResultKey():
                if run.runNr not in events:
                    # the OVERLAP_EVENTS information is not yet available in the SFO (before Nov 15, 2009)
                    # use the inclusive number instead
                    run.addResult(k, "n.a.", iov, reject=False)
                    run.showDataIncomplete=True
                    continue

                nev = events[run.runNr]
                
                if self.ApplySelection(k) and not self.passes(nev,k):
                    run.addResult(k, self.prettyValue(nev,k), iov, reject=True)
                    continue

                run.addResult(k, self.prettyValue(nev,k), iov)

            rej = self.rejectRun(run)

            if not rej:
                newrunlist += [run.runNr]

        runlist = [r for r in runlist if r.runNr in newrunlist]
        
        duration = time() - start
        if self.applySelection:
            print (" ==> %i runs found (%.2f sec)" % (len(runlist),duration))
        else:
            print (" ==> Done (%g sec)" % duration)
        return runlist

    def passes(self,values,key):
        try:
            val = int(values)
        except ValueError: # if n.a.
            self.selDataMissing = True
            return True
        for cr in self.cutRange:
            if val>=cr[0] and val<=cr[1]:
                return True
        return False


class AllEventsSelector(RunLBBasedCondition):
    def __init__(self, name, events=None):
        self.events = events
        self.showAllevents = False
        super(AllEventsSelector,self).__init__(name=name,
                                               dbfolderkey='COOLONL_TDAQ::/TDAQ/RunCtrl/EventCounters',
                                               channelKeys = [(0,'#Events (incl. calib.)','EFEvents'),
                                                              (0,'#Events (streamed)','RecordedEvents'),
                                                              (0,'#L2A','L2Events')])
        if events:
            self.cutRange = GetRanges(events)        

    def __str__(self):
        if self.applySelection:
            return "SELOUT Checking if number of events matches %s" % self.events
        else:
            return "Retrieving event numbers"

    def passes(self,values,key):
        try:
            val = int(values)
        except ValueError: # if n.a.
            self.selDataMissing = True
            return True
        for cr in self.cutRange:
            if val>=cr[0] and val<=cr[1]:
                return True
        return False

class L1EventsSelector(RunLBBasedCondition):
    def __init__(self, name, events=None):
        self.events = events
        self.showL1events = False
        super(L1EventsSelector,self).__init__(name=name,
                                              dbfolderkey='COOLONL_TRIGGER::/TRIGGER/LVL1/CTPCORELBDATA',
                                              channelKeys = [(0,'#L1A','GlobalL1AcceptCounter')])
    def setShowL1events(self):
        self.showL1events  = True
        super(L1EventsSelector,self).setShowOutput()
        
    def __str__(self):
        if self.applySelection:
            return "SELOUT Checking if number of L1A events matches %s" % self.events
        else:
            return "Retrieving L1A numbers"

    def passes(self,values,key):
        return True

