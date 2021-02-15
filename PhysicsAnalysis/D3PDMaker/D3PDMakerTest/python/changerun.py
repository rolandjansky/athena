#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import sys

if len(sys.argv) < 4:
    print ("""Change the run number in a pool file.
Probably only works correctly for simple, single-run MC files.

Usage: changerun.py NEWRUN INFILE OUTFILE
""")

newrun = int (sys.argv[1])
infile = sys.argv[2]
outfile = sys.argv[3]


# Prevent AthenaBarCodeImpl from trying to create JobIDSvc.
import os
import uuid
os.environ['_ATHENABARCODEIMPL_JOBUUID'] = uuid.uuid1().hex

import ROOT
from PyUtils.Helpers import ROOT6Setup
ROOT6Setup()

ROOT.DataModelAthenaPool.CLHEPConverters.initialize()

f1=ROOT.TFile(infile)
t1=f1.CollectionTree
m1=f1.MetaData

cl = ROOT.TClass.GetClass('Analysis::TruthInfo_p1')
#ROOT.ROOT.ResetClassVersion(cl, 'Analysis::TruthInfo_p1', 1)

f2=ROOT.TFile(outfile, 'RECREATE')
t2 = t1.CloneTree (0)
m2 = m1.CloneTree (0)

kk = [k.GetName() for k in f1.GetListOfKeys()]
for k in kk:
    if k in ['CollectionTree', 'MetaData']: continue
    o1 = f1.Get (k)
    o2 = o1.CloneTree()
    o2.Write()

# Hack: suppress errors.
def bscan (b):
    ifo = b.GetInfo()
    types = ifo.GetTypes()
    elts = ifo.GetElements()
    for i in range(ifo.GetNdata()):
        if elts[i].GetName() == 'm_athenabarcode' and types[i] == 217:
            types[i] = 17
    for bb in b.GetListOfBranches():
        bscan (bb)
    return
for b in t2.GetListOfBranches():
    bscan (b)

oldrun = None
for i in range(t1.GetEntries()):
    t1.GetEntry(i)
    oldrun = t1.EventInfo_p3_McEventInfo.m_AllTheData[1]
    t1.EventInfo_p3_McEventInfo.m_AllTheData[1] = newrun
    t2.Fill()
t2.Write()

def adjust_run (x):
    xrun = x >> 32
    if xrun <= 0 or xrun > 1000000: return x
    return ((xrun-oldrun) + newrun) * (1<<32) + (x & ((1<<32)-1))
    

for i in range(m1.GetEntries()):
    m1.GetEntry(i)
    for b in m1.GetListOfBranches():
        bn = b.GetName()
        if bn.startswith ('IOVMetaDataContainer_'):
            pv = getattr (m1, bn).m_payload.m_payloadVec
            for p in pv:
                if p.m_hasRunLumiBlockTime:
                    p.m_start = adjust_run (p.m_start)
                    p.m_stop = adjust_run (p.m_stop)
                    for pa in p.m_attrLists:
                        r = pa.m_range
                        r.m_start = adjust_run (r.m_start)
                        r.m_stop = adjust_run (r.m_stop)
    m2.Fill()
m2.Write()


f2.Close()
