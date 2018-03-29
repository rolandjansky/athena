# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# $Id: ara_ptest.py,v 1.1 2008-04-18 02:52:57 ssnyder Exp $
#
# @file AthenaROOTAccess/test/ara_scantest.py
# @author sss
# @date Apr 2008
# @brief Test for AthenaROOTAccess.
#
# This does a regression test for reading non-t/p-separated data.
#

import ROOT
import cppyy
import AthenaROOTAccess.transientTree

cppyy.loadDictionary("libDataModelTestDataWriteDict")

f = ROOT.TFile ('../../../Control/DataModelTest/DataModelRunTests/run/SimplePoolFile.root')
tt = AthenaROOTAccess.transientTree.makeTree (f,
                                              branchNames = {'bder' : 'xder'})

def scan1 (tt, fname):
    tt.GetPlayer().SetScanFileName (fname)
    tt.GetPlayer().SetScanRedirect (1)
    tt.Scan ("xder.m_x")
    tt.GetPlayer().SetScanRedirect (0)
    return


def scan2 (tt, fname):
    fout = open (fname, 'w')
    xbr = tt.GetBranch('xder')
    print >> fout, "***********************************"
    print >> fout, "*    Row   * Instance *  xder.m_x *"
    print >> fout, "***********************************"
    for i in range (int(tt.GetEntries())):
        xbr.GetEntry (i)
        for (j,x) in enumerate (tt.xder):
            print >> fout, "* %8d * %8d * %9d *" %\
                  (i, j, x.m_x)
    print >> fout, "***********************************"
    return


scan1 (tt, 'scan1.out')
scan2 (tt, 'scan2.out')

import os
os.system ("diff -u scan1.out scan2.out")

