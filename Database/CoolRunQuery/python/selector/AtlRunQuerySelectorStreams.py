# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from __future__ import print_function
import re, sys
try:
    Set = set
except NameError:
    from sets import Set
from time import time

from CoolRunQuery.utils.AtlRunQueryTimer import timer
from CoolRunQuery.utils.AtlRunQueryUtils import coolDbConn
from CoolRunQuery.AtlRunQueryRun import Run
from CoolRunQuery.selector.AtlRunQuerySelectorBase import Selector, DataKey


class StreamSelector(Selector):
    def __init__(self, name, streams=[]):
        # streams can be ["*RPC*", "phy* 10k", "deb*,cal* 100k"]
        #    will select runs that satiesfy an 'and' of all patterns in the list
        # a pattern without number are checked for existence
        # a pattern with number will be checked against the sum of the events in all matching streams

        self.streams = ' and '.join(streams)
        self.selstreamspatterns = []
        self.showstreampatterns = []

        # selstreampatterns is a list of (pattern,nevents) tuples
        for s in streams:
            pattern = s.split()[0].replace(',','|').replace('*','.*').replace('%','.*').replace('?','.')
            negate  = pattern[0]=='!'
            pattern = pattern.lstrip('!')
            p = re.compile(pattern)
            nevt = (s.split()+[None])[1]
            self.selstreamspatterns += [(p,nevt,negate,pattern)]
        
        super(StreamSelector,self).__init__(name)

    def setShowOutput(self):
        pass

    def addShowStreamPattern(self, streampattern="*"):
        self.showstreampatterns += streampattern.replace('*','.*').replace('%','.*').replace('?','.').split(',')

    def __str__(self):
        if len(self.streams)!=0:
            return 'SELOUT Checking if the stream name matches "%s"' % self.streams
        else:
            return "Retrieving stream names that match '%s'" % ','.join(self.showstreampatterns)

    def select(self, runlist):

        # some preparation: compile the show patterns
        start = time()

        if '.*' in self.showstreampatterns:
            compiledShowPatterns = [re.compile('.*')]
        else:
            compiledShowPatterns = [re.compile(p) for p in self.showstreampatterns]

        # we disable the access to everything that is not needed if we only select
        ShowStreams = False
        useTier0    = False
        if len(compiledShowPatterns)>0:
            ShowStreams = True
            useTier0 = False # ATLAS_T0 responsible refusing to allow access to their DB
        

        print (self, end='')
        sys.stdout.flush()
        newrunlist = []
        allStreams = Set()  # list of all the streams that are in the selected runs
        connection = coolDbConn.GetSFODBConnection()
        cursor     = connection.cursor()
        cursor.arraysize = 1000

        runnrlist = [r.runNr for r in runlist]

        from CoolRunQuery.AtlRunQuerySFO import GetSFO_streamsAll,GetSFO_filesAll
        with timer('GetSFO_streamsAll', disabled=True):
            streamsall = GetSFO_streamsAll( cursor, runnrlist )  # { runnr: [streams] }
        with timer('GetSFO_filesAll', disabled=True):
            filesall   = GetSFO_filesAll( cursor, runnrlist )    # [(COUNT(FILESIZE), SUM(FILESIZE), SUM(NREVENTS))]

        if ShowStreams:
            from CoolRunQuery.AtlRunQuerySFO import GetSFO_LBsAll,GetSFO_NeventsAll,GetSFO_overlapAll
            with timer('GetSFO_LBsAll', disabled=True):
                lbinfoall  = GetSFO_LBsAll( cursor, runnrlist )      # [(MIN(LUMIBLOCKNR), MAX(LUMIBLOCKNR), #LUMIBLOCKS)]
            with timer('GetSFO_overlapAll', disabled=True):
                overlapall = GetSFO_overlapAll( cursor, runnrlist )  # [(SUM(OVERLAP_EVENTS))]
            smallrunnrlist=[]
            for r in runnrlist: # go through old runlist and see
                if r not in streamsall:
                    continue
                for s in streamsall[r]:
                    if r in lbinfoall and s in lbinfoall[r] and lbinfoall[r][s][1]>0:
                        smallrunnrlist += [r]
                        break
            with timer('GetSFO_NeventsAll', disabled=True):
                neventsall = GetSFO_NeventsAll( cursor, smallrunnrlist )  # [(LUMIBLOCKNR, NREVENTS)]


        
        for run in runlist: # go through old runlist and see
            
            streams   = []  # names of all streams in this run
            strsize   = []  # size of streams in this run
            strevents = []  # nr of events in this run
            if run.runNr in streamsall:
                streams   = streamsall[run.runNr]
            for s in streams:
                try:
                    _, size, events = filesall[run.runNr][s]
                except IndexError:
                    _, size, events = (0,0,0)
                strsize += [size]
                strevents += [events]



            if ShowStreams:
                from CoolRunQuery.utils.AtlRunQueryUtils import Matrix

                nst       = len(streams)
                stovmat   = Matrix(nst,nst)  # overlap matrix
                for i,s in enumerate(streams):
                    run.stats['STR:'+s] = {}

                    # fill overlaps into matrix
                    for j,s2 in enumerate(streams):
                        try:
                            eventsij = overlapall[run.runNr][s][s2]
                        except KeyError:
                            eventsij = 0
                            if i==j and events:
                                eventsij = events
                        stovmat.setitem(i,j,float(eventsij))
                        stovmat.setitem(j,i,float(eventsij)) # symmetrise matrix


                    # read number of events per LB
                    minlb, maxlb, lbs = (0,0,1)
                    try:
                        minlb, maxlb, lbs = lbinfoall[run.runNr][s]
                    except KeyError:
                        pass

                    # if minlb==maxlb==0 -> no file closure at LB boundary
                    if minlb == 0 and maxlb == 0:
                        run.stats['STR:'+s]['LBRecInfo'] = None
                        continue
                    else:
                        lbevcount = '<tr>'
                        result = neventsall[run.runNr][s] #[ (lb,nev),... ]
                        lbold = -1
                        allnev = 0
                        ic = 0
                        ice = 0
                        allic = 0
                        lastElement = False
                        firstcall = True

                        for ice,(lb,nev) in enumerate(result):
                            if ice == len(result):
                                lastElement = True
                                allnev += nev
                            if lb != lbold or lastElement:
                                if lbold != -1:
                                    ic += 1
                                    allic += 1
                                    if allic < 101:
                                        if ic == 9:
                                            ic = 1
                                            lbevcount += '</tr><tr>'
                                        lbevcount += '<td style="font-size:75%%">%i&nbsp;(%s)</td>' % (lbold,allnev)
                                    else:
                                        if firstcall:
                                            lbevcount += '</tr><tr>'
                                            lbevcount += '<td style="font-size:75%%" colspan="8">... <i>too many LBs (> 100) to show</i></td>'
                                            firstcall = False                                        
                                allnev = nev
                                lbold = lb
                            else:
                                allnev += nev
                        lbevcount += '</tr>'
                        run.stats['STR:'+s]['LBRecInfo'] = lbevcount
                        run.stats['STR:'+s]['LBRecInfo'] = result

                # add overlap information to the run stats
                for i in range(nst):
                    statkey = 'STR:'+streams[i]
                    run.stats[statkey]['StrOverlap'] = []
                    denom = stovmat.getitem(i,i)
                    if denom==0:
                        continue
                    for j in range(nst):
                        if i == j or stovmat.getitem(i,j) == 0:
                            continue
                        fraction = 100
                        if stovmat.getitem(i,j) != denom:
                            fraction = float(stovmat.getitem(i,j))/float(denom)*100.0
                        run.stats[statkey]['StrOverlap'] += [(streams[j], fraction)]



            # selection...
            if not self.passes(zip(streams,strevents),0):
                continue
            newrunlist += [run.runNr]
            allStreams.update(streams)
            for k,v,s in zip(streams,strevents,strsize):
                run.addResult('STR:'+k, (v,s))

        allStreams = ['STR:'+s for s in allStreams]

        ### this should work as sort is "order safe"
        allStreams.sort(key = lambda x: (x[5:]) )
        allStreams.sort(key = lambda x: (x[4]), reverse=True )
   
        # fill the gaps
        for run in runlist:
            for s in allStreams:
                if s not in run.result:
                    run.addResult(s, 'n.a.')

        runlist = [r for r in runlist if r.runNr in newrunlist]

        # check if the streams in 'allStreams' match the show patterns
        for s in allStreams:
            if any( [p.match(s[4:]) is not None for p in compiledShowPatterns] ):
                Run.AddToShowOrder(DataKey(s, keytype=DataKey.STREAM))




        if useTier0:

            # retrieve Tier-0 information
            from CoolRunQuery.AtlRunQueryTier0 import GetTier0_datasetsAndTypes
            tier0connection  = coolDbConn.GetTier0DBConnection()
            cursor = tier0connection.cursor()
            cursor.arraysize = 1000
            tier0retdico     = GetTier0_datasetsAndTypes( cursor, runnrlist )

            # add Tier0 information
            for run in runlist:
                for s in allStreams:
                    if run.result[s]=='n.a.':
                        continue
                    run.stats[s]['StrTier0TypesRAW'] = {}
                    run.stats[s]['StrTier0TypesESD'] = {}
                    run.stats[s]['StrTier0AMI'] = {}
                    if run.runNr in tier0retdico.keys(): 
                        for dsname,t,pstates in tier0retdico[run.runNr]:
                            if s.replace('STR:','') in dsname:                                    
                                if '.RAW' in dsname:
                                    if '.merge' in dsname:
                                        prodstep = 'StrTier0TypesESD'
                                        t        = '(RAW)'
                                    else:
                                        prodstep = 'StrTier0TypesRAW'
                                        t        = '(RAW)'
                                else:
                                    if '.recon.' in dsname:
                                        prodstep = 'StrTier0TypesRAW'
                                    else:
                                        prodstep = 'StrTier0TypesESD'
                                    if prodstep not in run.stats[s]['StrTier0AMI']:
                                        dsnamesplit = dsname.split('.')
                                        if len(dsnamesplit)>5:
                                            amitag = dsnamesplit[5]
                                            run.stats[s]['StrTier0AMI'][prodstep] = amitag
                                        else:
                                            amitag = ''
                                # check if on CAF
                                oncaf = False
                                if pstates and 'replicate:done' in pstates:
                                    oncaf = True

                                # fill the run stats
                                if t not in run.stats[s][prodstep]:
                                    run.stats[s][prodstep][t] = oncaf


        # Done
        duration = time() - start
        if len(self.streams)!=0:
            print (" ==> %i runs found (%.2f sec)" % (len(runlist),duration))
        else:
            print (" ==> Done (%g sec)" % duration)
        return runlist

    def passes(self, streamevents, key):
        # streamevents is  [('physics_L1Calo', 87274, False), ('physics_RPCwBeam', 1075460, True), ('debug_hlterror', 151, False),...]
        for streampattern, neventreq, negate, pattern in self.selstreamspatterns:
            nevents = 0
            foundmatchingStream = False
            for se in streamevents:
                if streampattern.match(se[0]):
                    nevents += se[1]
                    foundmatchingStream = True
            if neventreq:
                if neventreq[-1] in '+-':
                    if neventreq[-1] == '-':
                        passreq = nevents < int( neventreq[:-1] )
                    else:
                        passreq = nevents > int( neventreq[:-1] )
                else:
                    passreq = nevents>int(neventreq)
            else:
                if negate:
                    passreq = not foundmatchingStream
                else:
                    passreq = foundmatchingStream
            if not passreq:
                return False
        return True

