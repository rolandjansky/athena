#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Extract necessary info (pLB, tstart, tend, sep, acqFlag) from VdM ntuple.
Ntuples are usually found at /afs/cern.ch/atlas/project/LumiWG/CoolScanNtuple/
"""

import sys
import ROOT

if len(sys.argv) != 2:
    sys.exit('Usage: extractScanInfo.py <filename>')

filename = sys.argv[1]
f = ROOT.TFile(filename)
t = f.Get('vdMScanData')

run = None
for e in t:
    if e.ScanRun != 0:
        run = e.ScanRun
        break

fout = open('r%s_pLB.txt' %run, 'w')
foutacq = open('r%s_pLB.acq.txt' % run, 'w')

for e in t:
    fout.write('%s %s %s %s\n' %(e.ScanLB, e.StartTime, e.EndTime, e.NominalSeparation))
    foutacq.write('%s %s %s %s %s\n' %(e.ScanLB, e.StartTime, e.EndTime, e.NominalSeparation, e.AcquisitionFlag))

fout.close()
foutacq.close()
