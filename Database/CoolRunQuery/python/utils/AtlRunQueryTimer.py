# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from __future__ import print_function
from contextlib import contextmanager
from time import time

class TimeCount:
    def __init__(self, name):
        self.name = name
        self.totaltime = 0
        self.subcounts = []

    def __repr__(self):
        return "%s: %f" % (self.name, self.totaltime)

    def __str__(self):
        return "%s: %f" % (self.name, self.totaltime)

    def printRecursive(self, lvl):
        from operator import attrgetter
        # print myself
        if self.subcounts:
            # sum up sub counts
            ts = sum([x.totaltime for x in self.subcounts])
            print ("%s%-70s : %f (sub sum %f)" % (5*lvl*" ", self.name, self.totaltime, ts))
        else:
            print ("%s%-70s : %f" % (5*lvl*" ", self.name, self.totaltime))
        # sort sub counters
        sortedByTime = sorted(self.subcounts,key=attrgetter('totaltime'),reverse=True)
        # call print for sub counters
        for subtc in sortedByTime:
            subtc.printRecursive(lvl+1)

    #def __eq__(self, r):
    #    return self.name == r
        

class TimerStats:
    level=0
    total = TimeCount('total')
    totalFlat = {}
    context = []

    @classmethod
    def saveTimeFlat(cls, exectime):
        n = cls.context[-1]
        if n not in cls.totalFlat:
            cls.totalFlat[n] = [0,0]
        cls.totalFlat[n][0] += exectime
        cls.totalFlat[n][1] += 1


    @classmethod
    def saveTime(cls, exectime):
        cur = cls.total # a TimeCount instance
        for n in cls.context:
            if n=='total':
                cur = cls.total
                continue
            try:
                idx = cur.subcounts.index(n)
                print ('index', idx)
                cur = cur.subcounts[idx]
            except ValueError:
                cur.subcounts += [TimeCount(n)]
                cur = cur.subcounts[-1]
        cur.totaltime += exectime
        cls.saveTimeFlat(exectime)

    @classmethod
    def printTimeSummary(cls):
        #import pickle
        #f = open("timecount.pickle","w")
        #pickle.dump(cls.total,f)
        #f.close()
        cls.total.printRecursive(0)

    @classmethod
    def printTimeFlat(cls):
        for name, [_time, callcount] in sorted(cls.totalFlat.items(),key=lambda x: x[1][0]):
            print ("%-70s : %f (%i)" % (name, _time, callcount))


@contextmanager
def timer(name, disabled = False):
    "A context manager which spits out how long the block took to execute"
    if disabled: #not environ.has_key("ARQDEBUG"):
        yield
        return

    from CoolRunQuery.utils.AtlRunQueryTimer import TimerStats as TS

    start = time()
    TS.level += 1
    TS.context.append(name)
    try:
        yield
    finally:
        end = time()
        execTime = end - start
        TS.saveTime(execTime)
        TS.level -= 1
        TS.context.pop()
        print ("%*s took %.2f sec to %s" % (5*TS.level, "...", execTime, name))
