# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import sys,os,copy, time

from PyCool import cool

class IOVTime:
    def __init__(self, run=None, lb=None, timerunlb=None,timebased=False):
        if isinstance(run,IOVTime):
            self.timebased = run.timebased
            self.run = run.run
            self.lb = run.lb
            self.time = run.time
        else:
            self.timebased=timebased
            self.run = self.lb = self.time = None
            if timebased:
                self.time=timerunlb
            else:
                if timerunlb!=None:
                    self.run = timerunlb>>32
                    self.lb = timerunlb&0xFFFFFFFF
                else:
                    self.run = run
                    self.lb  = lb 

    def __str__(self):
            if self.timebased:
                try:
                    return "%s" % time.strftime("%a %b %d %Y %X",time.gmtime(self.time/1E9))
                except:
                    return "infinity" 
            else:              return "%i/%i" % (self.run,self.lb)

    def __lt__(self, other):
        if self.timebased: return self.time<other.time
        else: return self.run<other.run or self.run==other.run and self.lb<other.lb

    def __cmp__(self, other):
        if self.timebased:
            return cmp(self.time,other.time)
        else:
            if self.run==other.run: return cmp(self.lb,other.lb)
            else: return cmp(self.run,other.run)

    def __eq__(self, other):
        if self.timebased: return self.time==other.time
        else: return self.run==other.run and self.lb==other.lb

    def __le__(self, other):
        return self<other or self==other    

    def __sub__(self, other):
        if isinstance(other, int):
            return IOVTime(timerunlb=self.timerunlb()-1)
        elif isinstance(other, IOVTime):
            return self.timerunlb()-other.timerunlb()
        

    def timerunlb(self):
        if self.timebased:
            return self.time
        res = long(self.run)<<32
        res += self.lb
        return res

    def inRange(self,iovrange):
        return iovrange.isInRange(self)

class IOVRange:
    def __init__(self, obj=None, starttime=None, endtime=None, runStart=None, lbStart=None, runEnd=None, lbEnd=None, timebased=False):
        if isinstance(obj, cool.IObject):
            self.startTime = IOVTime(timerunlb=obj.since(), timebased=timebased)
            self.endTime = IOVTime(timerunlb=obj.until(), timebased=timebased)
        elif isinstance(obj, IOVRange):
            self.startTime = obj.startTime
            self.endTime = obj.endTime
        elif isinstance(starttime, IOVTime) and isinstance(endtime, IOVTime):
            self.startTime = starttime
            self.endTime = endtime
        elif isinstance(starttime,long) and isinstance(endtime,long):
            self.startTime = IOVTime(timerunlb=starttime,timebased=timebased)
            self.endTime = IOVTime(timerunlb=endtime,timebased=timebased)
        elif runStart!=None and lbStart!=None and runEnd!=None and lbEnd!=None:
            self.startTime = IOVTime(runStart, lbStart, timebased=False)
            self.endTime = IOVTime(runEnd, lbEnd, timebased=False)
        else:
            print "IOVRange.__init__: Can't interpret arguments"

    def truncateToSingleRun(self,runnr):
        if self.startTime.run<runnr:
            self.startTime.run=runnr
            self.startTime.lb=1
        if self.endTime.run>runnr:
            self.endTime.run=runnr+1
            self.endTime.lb=0
            
    def __str__(self):
        return "[%s, %s)" % (self.startTime,self.endTime)
    def __repr__(self):
        return str(self)

    def __cmp__(self, other):
        return cmp(self.startTime,other.startTime)

    def length(self):
        return "%2.1f" % ((self.endTime-self.startTime)/1.E9)

    def isInRange(self, iovtime):
        return self.startTime<=iovtime and iovtime<self.endTime

    def overlaps(self, o):
        if o.startTime>=self.endTime or o.endTime<=self.startTime:
            return False
        return True


