# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from time import time
import sys

from PyCool import cool

from CoolRunQuery.utils.AtlRunQueryUtils import coolDbConn, GetRanges

from CoolRunQuery.selector.AtlRunQuerySelectorBase import Selector

from CoolRunQuery.AtlRunQueryRun import Run

class RunTimeSelector(Selector):
    def __init__(self, name, runlist):
        super(RunTimeSelector,self).__init__(name)
        if not runlist:
            runlist = ['-']
        runlist = ','.join(runlist).split(',')
        runlist = [rr for rr in runlist if not rr.startswith('last')]
        self.runranges = GetRanges(','.join(runlist))

    def __str__(self):
        rr = []
        for r in self.runranges:
            if r[0]==r[1]:
                rr += [ str(r[0]) ]
            elif r[0]==r[1]-1:
                rr += [ '%i, %i' % tuple(r) ]
            else:
                rr += [ '%i-%i' % tuple(r) ]
        return "SELOUT Checking for runs in %s" % ', '.join(rr)


    def select(self):
        
        if len(self.runranges)==0: # no run specified
            return []

        runlist = []
        firstRun = self.runranges[0][0]
        start = time()
        folder = coolDbConn.GetDBConn(schema="COOLONL_TRIGGER", db = Selector.condDB(firstRun) ).getFolder('/TRIGGER/LUMI/LBLB')
        print (self, end='')
        sys.stdout.flush()
        currentRun = None
        currentEOR = None
        for rr in self.runranges:
            objs = folder.browseObjects( rr[0] << 32, ((rr[1]+1) << 32)-1, cool.ChannelSelection(0))
            while objs.goToNext():
                obj=objs.currentRef()
                payload=obj.payload()
                runNr,lbNr = RunTimeSelector.runlb(obj.since())
                if lbNr==0:
                    lbNr=1 # this is an aweful hack to make MC work (there only one LB exists in LBLB and it is nr 0) need to rethink this
                if not currentRun or runNr > currentRun.runNr:
                    if currentRun:
                        currentRun.eor = currentEOR
                        runlist.append(currentRun)
                    currentRun = Run(runNr)
                    currentRun.sor = payload['StartTime']
                currentRun.lbtimes.extend([(0,0)]*(lbNr-len(currentRun.lbtimes)))
                currentRun.lbtimes[lbNr-1] = ( payload['StartTime'], payload['EndTime'] )
                currentRun.lastlb = lbNr
                currentEOR = payload['EndTime']
        if currentRun:
            currentRun.eor = currentEOR
            runlist.append(currentRun)
        runlist.sort()

        duration = time() - start
        print (" ==> %i runs found (%.2f sec)" % (len(runlist),duration))
        return runlist

    @staticmethod
    def runlb(time):
        run = time>>32
        lb = time&0xFFFFFFFF
        return (run,lb)

    def runNrFromTime(self,timeiov):
        listOfCoveredRuns = []
        runlist = self.runTimes.keys()
        runlist.sort()
        lastEOR = 0
        for rt in runlist:
            x = self.runTimes[rt]
            if timeiov[0]>=x[0] and timeiov[1]<x[1] or timeiov[0]<x[0] and timeiov[1]>x[0]:
                listOfCoveredRuns += [rt]
                lastEOR = x[1]
        return (listOfCoveredRuns,lastEOR)

        
class TimeRunSelector(Selector):
    def __init__(self, name, timelist):
        super(TimeRunSelector,self).__init__(name)
        self.timelist = ','.join(timelist)
        
    def select(self):
        start = time()
        runlist = []
        folder = coolDbConn.GetDBConn(schema="COOLONL_TRIGGER", db=Selector.condDB()).getFolder('/TRIGGER/LUMI/LBTIME')
        print ('SELOUT Checking for runs in time range "%s"' % self.timelist, end='')
        sys.stdout.flush()
        ranges = GetRanges(self.timelist, maxval=int(time()*1E09))
        currentRun = None
        currentEOR = None
        for rr in ranges:
            objs = folder.browseObjects( rr[0], rr[1]+86400000000000, cool.ChannelSelection(0))
            while objs.goToNext():
                obj=objs.currentRef()
                payload=obj.payload()
                runNr = int(payload['Run'])
                if runNr==0:
                    continue # mistakenly runnr=0 was stored 
                
                if runNr>1<<30:
                    # there is a problem with runs between
                    # usetimes 2009-04-14:00:00:00 2009-04-16:13:00:00
                    # there the runnumbers are off the chart (> 1<<30)
                    continue

                if not currentRun or runNr != currentRun.runNr:
                    if currentRun:
                        currentRun.eor = currentEOR
                        runlist.append(currentRun)
                    currentRun = Run(runNr)
                    currentRun.sor = obj.since()
                lbNr = int(payload['LumiBlock'])
                currentRun.lbtimes.extend([(0,0)]*(lbNr-len(currentRun.lbtimes)))
                currentRun.lbtimes[lbNr-1] = ( obj.since(), obj.until() )
                currentRun.lastlb = lbNr
                currentEOR = obj.until()
        if currentRun:
            currentRun.eor = currentEOR
            runlist.append(currentRun)

        runlist.sort()
        duration = time() - start
        print (" ==> %i runs selected (%g sec)" % (len(runlist), duration))
        return runlist

