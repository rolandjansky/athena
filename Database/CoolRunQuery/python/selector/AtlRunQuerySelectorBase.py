# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from __future__ import print_function
from PyCool import cool
from copy import deepcopy

from CoolRunQuery.utils.AtlRunQueryUtils  import coolDbConn, MergeRanges, SmartRangeCalulator  #, runsOnServer, GetRanges
from CoolRunQuery.utils.AtlRunQueryIOV    import IOVTime, IOVRange

import sys
from time import time
from collections import namedtuple, defaultdict

from itertools import groupby
from operator import itemgetter


OOO = namedtuple('OOO', 'channel payload iovrange isvf')
def coolgen(coolobjs):
    while coolobjs.goToNext():
        obj=coolobjs.currentRef()
        yield OOO(obj.channelId(),obj.payloadValue, IOVRange(obj), False)

ChDesc = namedtuple('ChDesc','CoolPayloadKey,CoolChannel,ResultKey,Header,SelectShowRetrieve')

class DataKey(object):
    SELECT   = 0x1
    SHOW     = 0x2
    RETRIEVE = 0x4

    UNDEFINED = 0
    STREAM = 1
    DETECTOR = 2
    DQDEFECTS = 3

    def __init__(self, x, keytype=UNDEFINED, selecttype = SELECT):
        if isinstance(x,str):
            self._cool_channel = 0
            self._cool_attr = ''
            self._internal_key = x.strip()
            self._second_internal_key = ''
            self._table_header = x.strip()
        else:
            self._cool_channel = x[0]
            self._cool_attr = x[2]
            self._internal_key = x[1].strip()
            self._second_internal_key = ''
            self._table_header = x[1] if len(x)<4 else x[3]
        self._type = keytype
        self._select_show_retrieve = selecttype # 0-retrieve only, 1-select, 2-show, 3-select&show

    @property
    def _seltype(self):
        if self._select_show_retrieve == DataKey.SELECT:
            return "SELECT"
        if self._select_show_retrieve == DataKey.SHOW:
            return "SHOW"
        if self._select_show_retrieve == DataKey.RETRIEVE:
            return "RETRIEVE"
        return ""

    def __repr__(self):
        return "<DataKey '%s' type %i %s>" % (self._internal_key, self._type, self._seltype)

    def __eq__(self,other):
        if isinstance(other,DataKey):
            #print ("Compare (%s) with (%s) ==> %r" % (self._internal_key,other._internal_key, self._internal_key==other._internal_key))
            eq = self._internal_key==other._internal_key and self._second_internal_key==other._second_internal_key
            return eq
        else:
            #print ("Compare (%s) with '%s' ==> %r" % (self._internal_key, other, self._internal_key==other))
            eq = self._internal_key==other
            return eq

    def __cmp__(self,other):
        if isinstance(other,DataKey):
            if self._internal_key==other._internal_key and self._second_internal_key==other._second_internal_key:
                return 0
            if self._internal_key==other._internal_key:
                return self._second_internal_key.__cmp__(other._second_internal_key)
            return self._internal_key.__cmp__(other._internal_key)
        else:
            #print ("Compare (%s) with '%s' ==> %r" % (self._internal_key, other, self._internal_key==other))
            return self._internal_key==other

    def __hash__(self):
        return hash( (self._internal_key,self._second_internal_key) )

    def pickled(self):
        if self._second_internal_key != '':
            return (self._internal_key,self._second_internal_key)
        return self._internal_key
    
    
    @property
    def CoolPayloadKey(self):
        return self._cool_attr
        
    @property
    def CoolChannel(self):
        return self._cool_channel
    
    @property
    def ResultKey(self):
        return self._internal_key

    @property
    def Header(self):
        return self._table_header

    @property
    def SelectShowRetrieve(self):
        return self._select_show_retrieve

    @property
    def Type(self):
        return self._type




class Selector(object):
    __conddb = None
    condtag = ""

    @staticmethod
    def setCondDBMC():
        Selector.__conddb = 'OFLP200'

    @staticmethod
    def condDB(run_number = None ):
        if not run_number:
            if Selector.__conddb:
                return Selector.__conddb
            raise RuntimeError("CondDB not yet set")

        Selector.__conddb = "CONDBR2" if run_number > 236100 else "COMP200"
        print ("Determinded %s, based on run number %i" % (Selector.__conddb, run_number))
        return Selector.__conddb

    @staticmethod
    def isRun2(run_number = None ):
        return Selector.condDB(run_number)=="CONDBR2"



    def __init__(self, name):
        self.name = name
        self.priority = 50
        self.verbose = False
        self.applySelection = True
        self.mcenabled = False
    def select(self, runlist):
        return runlist
    def prettyValue(self, value, key):
        return value
    def ApplySelection(self, key):
        return self.applySelection
    def runAfterQuery(self, runlist):
        pass



class Condition(Selector):
    def __init__(self, name, dbfolderkey, channelKeys):
        super(Condition,self).__init__(name)
        self.selDataMissing = False
        self.setSchemaFolderTag(dbfolderkey)
        self.setChannelKeys(channelKeys)

    def setSchemaFolderTag(self,dbfoldertag):
        # setting schema, folder, and tag
        self.schema, foldertag = dbfoldertag.split('::')
        self.folder, self.tagname = (foldertag.split('#')+[''])[0:2]
        if self.tagname=="":
            self.tagname = self.condtag
        
        
    def setChannelKeys(self,channelKeys,ssr=None):
        self.data_keys = [DataKey(x) for x in channelKeys]
        if ssr:
            assert len(ssr)==len(self.data_keys)
            for (x,_ssr) in zip(self.data_keys,ssr):
                x._select_show_retrieve = _ssr
        
        self._channelKeys    = channelKeys
        self._coolpayloadkey = [x[2] for x in channelKeys]
        self._coolchannel    = [x[0] for x in channelKeys]
        self._resultKey      = [x[1] for x in channelKeys]
        self._headerKey      = [x[1] for x in channelKeys]
        if not ssr:
            self._doSelectShowRetrieve = len(channelKeys)*[1] # 0-retrieve only, 1-select, 2-show, 3-select&show
        else:
            self._doSelectShowRetrieve = ssr
        self._channeldesc = self.data_keys

    def _getFolder(self):
        f = coolDbConn.GetDBConn(schema = self.schema,db=Selector.condDB()).getFolder(self.folder)
        if f.versioningMode()==0:
            self.tagname=""
        if self.tagname not in ["HEAD", ""]:
            self.tagname = f.resolveTag(self.tagname)
        return f

    def addChannelKey(self,channelKey,ssr=None):
        """
        channelKey: (0,'SMK','MasterConfigurationKey')
        ssr: 0-retrieve only, 1-select, 2-show, 3-select&show
        """
        
        if DataKey(channelKey) in self.data_keys:
            # key already exists
            return
        
        self.data_keys += [ DataKey(channelKey) ]
        if ssr:
            self.data_keys[-1]._select_show_retrieve = ssr
        
        self._channelKeys    += [ channelKey ]
        self._coolpayloadkey += [ channelKey[2] ]
        self._coolchannel    += [ channelKey[0] ]
        self._resultKey      += [ channelKey[1] ]
        self._headerKey      += [ channelKey[1] ]
        if ssr:
            self._doSelectShowRetrieve += [ ssr ]
        else:
            self._doSelectShowRetrieve += [ 1 ] 
        self._channeldesc = self.data_keys

        

    def ResultKey(self):
        return self._resultKey

    def ChannelKeys(self):
        return self._channelKeys

    def CoolChannels(self):
        return self._coolchannel

    def HeaderKeys(self):
        return self._headerKey

    def DoSelectShowRetrieve(self):
        return self._doSelectShowRetrieve

    def ChannelDesc(self):
        return self._channeldesc

    def setShowOutput(self, listofchans=None ):
        for cd in self.ChannelDesc():
            if listofchans and cd.ResultKey not in listofchans:
                continue
            cd._select_show_retrieve |= 2 
        self._doSelectShowRetrieve = [cd.SelectShowRetrieve for cd in self._channeldesc]
        self.updateShowOrder()
        
    def updateShowOrder(self):
        from CoolRunQuery.AtlRunQueryRun import Run
        for cd in self.ChannelDesc():
            if cd.SelectShowRetrieve<2:
                continue
            Run.AddToShowOrder(cd)


class RunLBBasedCondition(Condition):
    def __init__(self, name, dbfolderkey, channelKeys=None):
        super(RunLBBasedCondition,self).__init__(name, dbfolderkey, channelKeys)

    def _retrieve(self, iovmin, iovmax, f, sortedRanges):
        chansel = None
        for ch1,ch2 in sortedRanges:
            if chansel is None:
                chansel = cool.ChannelSelection(ch1,ch2,cool.ChannelSelection.sinceBeforeChannel)
            else:
                chansel.addRange(ch1,ch2)
        print (self.name,"browsing objects with tag",self.tagname)
        return coolgen(f.browseObjects( iovmin, iovmax, chansel, self.tagname))

    def findPayload(self, runNr, iovpllist):
        """
        iovpllist is a list [(IOV,payload),(IOV,payload),..,(IOV,payload)]
        """
        for x in iovpllist:
            if IOVTime(runNr,1).inRange(x[0]):
                return x[1]
        return 'n.a.'

    def getSortedAndCompletedPayloadDict(self, iovpllist, runlist, key):
        # reduce to data in the run 'runNr' and sort by iov start time

        pld = defaultdict(list)
        if not iovpllist:
            return pld
        if not runlist:
            return pld
        
        runnrlist = [r.runNr for r in runlist]
        nlb = dict([(r.runNr,r.lastlb) for r in runlist])

        # first and last run of the runlist
        firstrun = runnrlist[0]
        lastrun  = runnrlist[-1]

        for iov, pl in iovpllist:
            first = max(firstrun,iov.startTime.run)
            last =  min(lastrun,(iov.endTime-1).run)
            for r in range(first,last+1):
                if r in runnrlist:
                    pld[r].append((deepcopy(iov),pl))

        # adjustments of IOV
        for runnr, iovplbyrun in pld.items():
            # adjust lb 0 to lb 1
            if iovplbyrun[0][0].startTime.lb==0:
                iovplbyrun[0][0].startTime.lb=1

            # truncate first IOV to a single run
            iovplbyrun[0][0].truncateToSingleRun(runnr)

            # sometimes and IOV has [RunLB1 - RunLB2), where RunLB2 has LB==1
            # -> that gets truncated to [RunLB2 - RunLB2), which is obviously not valid
            # -> so we slice that right out
            if iovplbyrun[0][0].startTime == iovplbyrun[0][0].endTime:
                iovplbyrun[:1] = []

            # truncate last IOV to a single run
            lastiov = iovplbyrun[-1][0]
            lastiov.truncateToSingleRun(runnr)

            # insert IOVs for missing ranges (value n.a.)
            idx=0
            curTime = IOVTime(runnr,1)
            while curTime<lastiov.startTime:
                iov = iovplbyrun[idx][0]
                if curTime < iov.startTime:
                    # insert n.a. for missing iov
                    missingIOV = IOVRange(starttime=curTime, endtime=iov.startTime)
                    iovplbyrun.insert( idx, (missingIOV, "n.a.") )
                curTime = IOVTime(iov.endTime)
                idx += 1

            # also check if IOV at the end is missing 
            runEndTime = IOVTime(runnr,nlb[runnr]+1) # last lb + 1
            if  lastiov.endTime<runEndTime:
                missingIOV = IOVRange(starttime=lastiov.endTime, endtime=runEndTime)
                iovplbyrun.append( (missingIOV, "n.a.") )

        return pld



    def readCondData(self, runranges, f, sortedRanges):
        # get the data from cool
        condData = defaultdict(list)

        keys = dict([(ch,set(k)) for ch, k in groupby(sorted(self.ChannelKeys(), key=itemgetter(0)), itemgetter(0))])

        for rr in runranges:
            iovmin=(rr[0] << 32)+0
            iovmax=((rr[1]+1) << 32)-1

            # access to COOL
            objs = self._retrieve(iovmin, iovmax, f, sortedRanges)

            for obj in objs:
                ch  = obj.channel
                for chtmp, internalKey, payloadKey in keys[ch]:
                    if type(payloadKey)==tuple:
                        if obj.isvf:
                            payload = obj.payload
                        else:
                            payload = tuple(map(obj.payload, payloadKey))
                    else:
                        payload = obj.payload(payloadKey)

                    condData[internalKey].append( (IOVRange(obj.iovrange), payload) )

        return condData




    def select(self, runlist):
        print (self, end='')
        sys.stdout.flush()
        start = time()
        newrunlist = []

        sortedChannel = sorted( list( set( self.CoolChannels() ) ) )
        sortedRanges = MergeRanges([(x,x) for x in sortedChannel])
        
        runranges = SmartRangeCalulator(runlist)

        f = self._getFolder()

        condData = self.readCondData(runranges, f, sortedRanges)

        # for each key sort the data by IOV start time
        for k in self.ResultKey():
            condData[k].sort()
            #if k.startswith('ofllumi'):
            #    for x in condData[k]:
            #        print (k,x)

        condDataDict = {}
        for k in self.ResultKey():
            condDataDict[k] = self.getSortedAndCompletedPayloadDict(condData[k],runlist,k)

        for run in runlist: # go through old runlist and see

            rejectSomething = False
            for k in self.ResultKey():
                if run.runNr not in condDataDict[k]:
                    if self.ApplySelection(k):
                        rejectSomething = True
                    run.addResult(k, "n.a.")
                    continue

                datavec = condDataDict[k][run.runNr]

                if not datavec or len(datavec)==0:
                    run.addResult(k, "n.a.")
                    continue

                if 'n.a.' in datavec:
                    run.showDataIncomplete = True

                for iov, data in datavec:
                    #if k=="DQ":
                    #    print ("CCCCCCCCCCCC",k,data)
                    self.selDataMissing = False
                    if self.ApplySelection(k) and not self.passes(data,k):
                        run.addResult(k, self.prettyValue(data,k), iov, reject=True)
                        rejectSomething = True
                    else:
                        run.addResult(k, self.prettyValue(data,k), iov)
                        if self.selDataMissing:
                            run.selDataIncomplete = True

            if not (rejectSomething and self.rejectRun(run)):
                newrunlist += [run.runNr]
                        
        runlist = [r for r in runlist if r.runNr in newrunlist]

        duration = time() - start

        if self.applySelection:
            print (" ==> %i runs found (%.2f sec)" % (len(runlist),duration))
        else:
            print (" ==> Done (%g sec)" % duration)

        return runlist


    def rejectRun(self,run):
        return run.data.isRejected


class TimeBasedCondition(Condition):
    def __init__(self, name, dbfolderkey, channelKeys=None):
        super(TimeBasedCondition,self).__init__(name, dbfolderkey, channelKeys)


    def getSortedAndCompletedPayloadDict(self, iovpllist, runlist,key):
        # reduce to data in the run 'runNr' and sort by iov start time
        
        pld = {}
        if not iovpllist or not runlist:
            return pld

        startiovindex = 0
        for run in runlist:
            pld[run.runNr] = []

            if startiovindex<-1:
                continue

            # find the first iov that overlaps
            iovindex = startiovindex
            while True:
                try:
                    iov      = iovpllist[iovindex][0]
                    iovstart = iov.startTime.time
                    iovend   = iov.endTime.time

                    if iovend>run.sor:
                        startiovindex = iovindex
                        break

                    if iovstart>run.eor:
                        startiovindex = -1
                        break

                    if iovend<run.sor and iovindex==len(iovpllist)-1:
                        startiovindex = -2
                        break

                    iovindex += 1
                except IndexError:
                    startiovindex = -1

            if startiovindex<0:
                continue

            # find the last iov that overlaps
            iovindex = startiovindex
            while True:
                iov      = iovpllist[iovindex][0]
                iovstart = iov.startTime.time
                iovend   = iov.endTime.time

                if iovend>=run.eor or iovindex==len(iovpllist)-1:
                    endiovindex = iovindex
                    break

                iovindex += 1

            # now we have the first and last iov that overlap with that run: startiovindex, endiovindex
            lbindex = 0
            for iov,pl in iovpllist[startiovindex:endiovindex+1]:
                iovstart = iov.startTime.time
                iovend   = iov.endTime.time
                endrun   = run.runNr

                while lbindex<len(run.lbtimes) and run.lbtimes[lbindex][0]<iovstart:
                    lbindex += 1
                startlb = lbindex+1

                while lbindex<len(run.lbtimes) and run.lbtimes[lbindex][0]<iovend:
                    lbindex += 1
                lbindex -= 1

                # now lbindex points to the last lb that starts within the iov or to one after the last lb
                if lbindex==len(run.lbtimes)-1:
                    endlb   = 0
                    endrun += 1
                else:
                    endlb = lbindex+2 # +1 for lb-index->lb-number, +1 for closed interval


                # append info of this IOV
                lastIOV,lastpl=None,None
                if len(pld[run.runNr])>0:
                    lastIOV,lastpl = pld[run.runNr][-1]
                if lastpl==pl:
                    lastIOV.endTime.run = endrun
                    lastIOV.endTime.lb = endlb
                else:
                    pld[run.runNr] += [(IOVRange(runStart=run.runNr, lbStart=startlb, runEnd=endrun, lbEnd=endlb), pl)] # in this case the end LB is inclusive


            # for the next run we start looking at:
            startiovindex = endiovindex

        return pld


    def select(self, runlist):
        runlistNo = []
        for run in runlist:
           runlistNo.append(run.runNr)
        print('runlistNo: ', runlistNo)
        print (self, end='')
        sys.stdout.flush()
        start = time()
        newrunlist = []
        f = coolDbConn.GetDBConn(schema=self.schema, db=Selector.condDB()).getFolder(self.folder)
        sortedChannel = sorted( list( set( self.CoolChannels() ) ) )
        chansel = None
        for ch in sortedChannel:
            if chansel is None:
                chansel = cool.ChannelSelection(ch,ch,cool.ChannelSelection.sinceBeforeChannel)
            else:
                chansel.addChannel(ch)

        runranges = SmartRangeCalulator(runlist,True)
        condData = defaultdict(list)

            
        # access COOL
        for rr in runranges:
            firstrun = runlist[runlistNo.index(rr[0])]
            lastrun = runlist[runlistNo.index(rr[1])]
            iovmin=firstrun.sor
            iovmax=lastrun.eor

            objs = f.browseObjects( iovmin, iovmax, chansel)
            while objs.goToNext():
                obj= objs.currentRef()
                ch = obj.channelId()
                for chKey in self.ChannelKeys():
                    (channelNumber, resultKey, payloadKey) = chKey
                    if channelNumber != ch:
                        continue
                    isBlob = (resultKey == 'olc:bcidmask')
                    if isBlob:
                        payloadvalue = obj.payload()[chKey[2]].read()
                    else:
                        payloadvalue = obj.payloadValue(chKey[2])

                    condData[resultKey].append( (IOVRange(obj=obj, timebased=True), payloadvalue) )


        # for each key sort the data by IOV start time
        for k in self.ResultKey():
            condData[k].sort()
        
        condDataDict = {}
        for k in self.ResultKey():
            condDataDict[k] = self.getSortedAndCompletedPayloadDict(condData[k],runlist,k)

        for run in runlist:

            rejectSomething = False
            for k in self.ResultKey():

                if run.runNr not in condDataDict[k]:
                    run.addResult(k, "n.a.")
                    continue

                datavec = condDataDict[k][run.runNr]

                if not datavec or len(datavec)==0:
                    run.addResult(k, "n.a.")
                    continue

                if 'n.a.' in datavec:
                    run.showDataIncomplete=True

                for iov, data in datavec:
                    self.selDataMissing = False
                    if self.ApplySelection(k) and not self.passes(data,k):
                        run.addResult(k, self.prettyValue(data,k), iov, reject=True)
                        rejectSomething = True
                        continue

                    run.addResult(k, self.prettyValue(data,k), iov)

                    if self.selDataMissing:
                        run.selDataIncomplete = True

            if not (rejectSomething and self.rejectRun(run)):
                newrunlist += [run.runNr]


        runlist = [r for r in runlist if r.runNr in newrunlist]

        duration = time() - start

        if self.applySelection:
            print (" ==> %i runs found (%.2f sec)" % (len(runlist),duration))
        else:
            print (" ==> Done (%g sec)" % duration)

        return runlist

    def rejectRun(self,run):
        return run.data.isRejected

