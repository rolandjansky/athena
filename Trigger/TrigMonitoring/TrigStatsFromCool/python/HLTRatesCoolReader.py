#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
from collections import namedtuple,defaultdict
from struct import unpack_from, unpack
from PyCool import cool
from CoolRunQuery.AtlRunQueryTimer import timer
from TrigStatsFromCool.HLTTriggerNamesReader import getSMK,getTriggerNames
import TrigConfStorage.FastBlobReader


CountRec = namedtuple('CountRec','ChainCounter Input AfterPrecale Raw Total')
LBI      = namedtuple('LBI','lb starttime duration')

# timers are switched on by setting env ARQDEBUG=1


def getL2Counts(runs,pattern=None):
    return __getCounts(runs=runs,pattern=pattern, levels=0x1)

def getEFCounts(runs,pattern=None):
    return __getCounts(runs=runs,pattern=pattern, levels=0x2)

def getCounts(runs,pattern=None):
    return __getCounts(runs=runs,pattern=pattern, levels=0x3)



class TriggerRates:
    def __init__(self,level,triggernames,rates):
        self._level    = level
        self._rates    = rates
        self._triggers = dict([(tn.name,tn) for tn in triggernames])
        for tn in triggernames:
            self._rates[tn.counter].triggername=tn.name

    def  __repr__(self):
        return "Trigger rates for %s\n%r" % (self._level,self._rates)

    def triggers(self):
        """list of all triggers that match pattern"""
        return self._triggers

    def triggernames(self):
        """names of all triggers that match pattern"""
        return self._triggers.keys()

    def triggercounters(self):
        """counters of all triggers where the name matches pattern"""
        return self._rates.keys()

    def __getitem__(self,k):
        """access to rates for any trigger, where k can be the trigger name or counter"""
        if type(k)==int:
            return self._rates[k]
        if type(k)==str:
            return self._rates[self._triggers[k].counter]
        

    
class TriggerRate:
    def __init__(self):
        self._triggername = "not existing"
        self._rates = []
        self._totalcount = 0
        self._totalduration = 0
        
    def add(self,rate):
        idx = rate.lb()-1 # LB 0 doesn't exist
        self._rates.extend( (1+idx-len(self._rates))*[None])
        if self._rates[idx] != None:
            raise RuntimeError("Found twice data for the same trigger and same LB")
        self._rates[idx] = rate
        self._totalcount += rate.counts(0)
        self._totalduration += rate.duration()

    def  __repr__(self):
        return "%s (%i/%1.fsec)" % (self._triggername,self._totalcount,self._totalduration)

    def rates(self):
        return self._rates



class Rate:
    def __init__(self,lbinfo,countrecord):
        self._lbinfo = lbinfo
        self._countrecord = countrecord

    def __str__(self):
        return "%r" % self

    def __repr__(self):
        if self._countrecord:
            return "%1.fHz (%i/%1.fs)" % (self.rate(), self.counts(), self._lbinfo.duration)
        return "0Hz (0L1A/%1.fs)" % self._lbinfo.duration

    def rate(self,type=0):
        return self.counts(type) / self._lbinfo.duration

    def lb(self):
        return self._lbinfo.lb

    def duration(self):
        return self._lbinfo.duration

    def counts(self,type=0):
        if self._countrecord:
            return self._countrecord.Total
        return 0

    def hasL1A(self):
        return self._countrecord != None






def __findPos(chaincounters, buf):
    counterpositions = []
    ndata = len(buf)/20
    for pos in xrange(ndata):
        entryCounter = unpack_from('I', buf, 20*pos)[0]  # get counter that is at position 'pos'
        if entryCounter in chaincounters:
            counterpositions += [(entryCounter,pos)]
    return counterpositions



def __decodeCounts(pos, buf):
    if not pos or 20*pos>=len(buf): return None
    return CountRec._make( unpack_from('IIIII', buf, 20*pos) )



def __printCounts(buf):
    ndata = len(buf)/20
    print unpack(ndata*'IIIII', buf)


def _getLBInfo(runlist, folder):
    lbinfo = {}
    with timer("read the lbinfo"):
        for run in runlist:
            start = run<<32
            end   = ((run+1)<<32)-1
            objs = folder.browseObjects(start,end,cool.ChannelSelection(0,0,cool.ChannelSelection.sinceBeforeChannel))
            lbinfo[run] = [0]+[ LBI(o.since()&0xFFFFFFFF, int(o.payload()['StartTime']/1.e9), int( (o.payload()['EndTime']-o.payload()['StartTime'])/1.e9 ) ) for o in objs ]
    return lbinfo
    
    
def __getCounts(runs,pattern=None, levels=0x3):

    if type(runs)==int: runs = [runs]
    smks = getSMK(runs)

    triggernames = getTriggerNames(smks.values(),pattern)

    return __getCountsForSMKandTrigNames(smks,triggernames, levels)
    

def __getCountsForSMKandTrigNames(smkByRun, triggerNames, levels):

    doL2 = (levels & 1) != 0
    doEF = (levels & 2) != 0

    if doL2:
        l2Names = {}
        l2counts = {}

    if doEF:
        efNames = {}
        efcounts = {}

    runlist = sorted(smkByRun.keys())
    smks = set(smkByRun.values())
    if doL2:
        for smk in smks:
            l2Names[smk] = [tn for tn in triggerNames[smk] if tn.level=='L2']
    if doEF:
        for smk in smks:
            efNames[smk] = [tn for tn in triggerNames[smk] if tn.level=='EF']
    
    from CoolConvUtilities.AtlCoolLib  import indirectOpen
    conn = indirectOpen( "COOLONL_TRIGGER/COMP200", True, True, False)
    if doL2: l2folder=conn.getFolder("/TRIGGER/LUMI/LVL2COUNTERS")
    if doEF: effolder=conn.getFolder("/TRIGGER/LUMI/EFCOUNTERS")
    lbfolder=conn.getFolder("/TRIGGER/LUMI/LBLB")

    lbinfo = _getLBInfo(runlist, lbfolder)

    for run in runlist:
        smk = smkByRun[run]
        if doL2:
            l2names_thissmk = l2Names[smk]
            if l2names_thissmk:
                l2counts[run] = TriggerRates("L2", l2names_thissmk, _getCountsInFolder(run, -1, [tn.counter for tn in l2names_thissmk], l2folder, lbinfo[run]))
        if doEF:
            efnames_thissmk = efNames[smk]
            if efnames_thissmk:
                efcounts[run] = TriggerRates("EF", efnames_thissmk, _getCountsInFolder(run, -1, [tn.counter for tn in efnames_thissmk], effolder, lbinfo[run]))

    if levels&3==1:
        return l2counts
    elif levels&3==2:
        return efcounts
    elif levels&3==3:
        return l2counts, efcounts
    return None


def _getCountsInFolder(run, lb, chainCounters, folder, lbinfo):
    """
    returns a list of Run/LB indexed CountRec objects
    if lb<0 then the list contains the entire run, otherwise it has just one entry for the given run/lb
    """

    with timer("access COOL"):
        if lb>=0:
            runlb = ( run<<32 ) + lb
            objs = [ folder.findObject(runlb,0) ]
        else:
            start = run<<32
            end   = ((run+1)<<32)-1
            objs = folder.browseObjects(start,end,cool.ChannelSelection(0,0,cool.ChannelSelection.sinceBeforeChannel))

    res = defaultdict(TriggerRate)
    pos = None
    nocounts = []

    with timer("read the blob for run %i" % run):
        sl = [ (o.since(), o.payload()['Data'].read()) for o in objs ]

    with timer("unpack"):
        for (runlb,s) in sl:
            #run = runlb>>32
            lb = runlb&0xFFFFFFFF

            lbinfo_thislb = lbinfo[lb]
            if lbinfo_thislb.lb != lb:
                print "LBInfo",lbinfo_thislb,"is at the wrong place in the array",lb
                sys.exit(1)

            if len(s)==0:
                nocounts += [ Rate(lbinfo_thislb, None) ]
            else:
                if len(s)%20 != 0:
                    raise RuntimeError, "the length of the data vector %i is not a multiple of 20" % len(s)

                #__printCounts(s)
                if pos==None:
                    counterpositions = __findPos(chainCounters, s)

                for counter,pos in counterpositions:
                    res[counter].add( Rate(lbinfo_thislb, __decodeCounts(pos,s)) )
                    
        for trigrates in res.values():
            for r in nocounts:
                trigrates.add( r )
    return res


if __name__=="__main__":

    run = 166786
    triggerpattern = "L2_2e"
    
    l2rates, efrates = getCounts(run, triggerpattern)

    print l2rates[run].triggernames()

    for r in l2rates[run]['L2_2e5_tight'].rates():
        #print "LB = %3i: %r" % (r.lb(),r)
        print "LB=",r.lb(),", rate=",r.rate(),", counts=",r.counts(),", duration=",r.duration()
    
