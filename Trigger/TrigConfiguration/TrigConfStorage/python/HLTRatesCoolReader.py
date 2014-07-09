#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from collections import namedtuple
from struct import unpack_from, unpack
from PyCool import cool
from CoolRunQuery.AtlRunQueryTimer import timer

import TrigConfStorage.FastBlobReader

CountRec=namedtuple('CountRec','ChainCounter Input AfterPrecale Raw Total')

def __findPos(chainCounter, buf):
    ndata = len(buf)/20
    for pos in xrange(ndata):
        entryCounter = unpack_from('I', buf, 20*pos)[0]
        if entryCounter == chainCounter:
            return pos
        if entryCounter > chainCounter:
            return None
    return None

def __getCounts(pos, buf):
    if not pos or 20*pos>=len(buf): return None
    return CountRec._make( unpack_from('IIIII', buf, 20*pos) )

def __printCounts(buf):
    ndata = len(buf)/20
    print unpack(ndata*'IIIII', buf)
    
    
def getL2Counts(run, lb, chainCounter):
    """
    returns a list of Run/LB indexed CountRec objects
    if lb<0 then the list contains the entire run, otherwise it has just one entry for the given run/lb
    """

    with timer("access COOL"):
        from CoolConvUtilities.AtlCoolLib  import indirectOpen
        conn = indirectOpen( "COOLONL_TRIGGER/COMP200", True, True, False)
        folder=conn.getFolder('/TRIGGER/LUMI/LVL2COUNTERS')

        if lb>=0:
            runlb = ( run<<32 ) + lb
            objs = [ folder.findObject(runlb,0) ]
        else:
            start = run<<32
            end   = ((run+1)<<32)-1
            objs = folder.browseObjects(start,end,cool.ChannelSelection(0,0,cool.ChannelSelection.sinceBeforeChannel))

    res = []
    pos = None

    with timer("read the blob"):
        sl = [ (o.since(), o.payload()['Data'].read()) for o in objs ]

    with timer("unpack"):        
        for (runlb,s) in sl:
            run = runlb>>32
            lb = runlb&0xFFFFFFFF
            if len(s)==0:
                res += [ (run, lb, CountRec(chainCounter,0,0,0,0) )]
            else:
                if len(s)%20 != 0:
                    raise RuntimeError, "the length of the data vector %i is not a multiple of 20" % len(s)

                #__printCounts(s)
                if pos==None:
                    pos = __findPos(chainCounter, s)
                    if pos==None: return None

                res += [ (run, lb, __getCounts(pos,s)) ]

    return res



if __name__=="__main__":

    print getL2Counts(166786, 300, 7)[0]
    
    print [ (run, lb,cr.Raw) for (run, lb,cr) in getL2Counts(166786, None, 7)]
