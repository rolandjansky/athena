# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# $Id: ara_scantest.py,v 1.4 2008-07-14 21:02:50 ssnyder Exp $
#
# @file AthenaROOTAccess/test/ara_scantest.py
# @author sss
# @date Oct 2007
# @brief Test for AthenaROOTAccess.
#
# This does a regression test of TTree::Scan, using ARA.
#

import ROOT
import AthenaROOTAccess.transientTree

import sys
def make_fin (fin):
    if len(sys.argv) > 1:
        if fin.startswith ('AOD-'):
            fin = 'AOD-' + sys.argv[1] + fin[3:]
        fin = 'AOD-' + sys.argv[1] + '/' + fin
    return fin

f = ROOT.TFile (make_fin ('AOD-full.pool.root'))
tt = AthenaROOTAccess.transientTree.makeTree (f)

def scan1 (tt, fname):
    tt.GetPlayer().SetScanFileName (fname)
    tt.GetPlayer().SetScanRedirect (1)
    tt.Scan ("ElectronAODCollection.pt():ElectronAODCollection.cluster().pt()")
    tt.GetPlayer().SetScanRedirect (0)
    return

def f9(f):
    s = '%9.9g'%f
    return s[:9]
def scan2 (tt, fname):
    fout = open (fname, 'w')
    elebr = tt.GetBranch('ElectronAODCollection')
    print >> fout, "***********************************************"
    print >> fout, "*    Row   * Instance * ElectronA * ElectronA *"
    print >> fout, "***********************************************"
    for i in range (int(tt.GetEntries())):
        elebr.GetEntry (i)
        for (j,ele) in enumerate (tt.ElectronAODCollection):
            print >> fout, "* %8d * %8d * %9s * %9s *" %\
                  (i, j, f9(ele.pt()), f9(ele.cluster().pt()))
    print >> fout, "***********************************************"
    return

def scan3 (tt, fname):
    tt.GetPlayer().SetScanFileName (fname)
    tt.GetPlayer().SetScanRedirect (1)
    tt.Scan ("fabs(ElectronAODCollection.pt())")
    tt.GetPlayer().SetScanRedirect (0)
    return


scan1 (tt, 'scan1.out')
scan2 (tt, 'scan2.out')
scan3 (tt, 'scan3.out')

import os
os.system ("diff -u scan1.out scan2.out")
