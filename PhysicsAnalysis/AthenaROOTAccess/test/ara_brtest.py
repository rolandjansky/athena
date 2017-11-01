# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# $Id: ara_brtest.py,v 1.1 2008-08-22 21:51:27 ssnyder Exp $
#
# @file AthenaROOTAccess/test/ara_brtest.py
# @author sss
# @date Aug 2008
# @brief Test for AthenaROOTAccess.
#
# Test creating a new TBranch for a type with a DV proxy.
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

ec=ROOT.ElectronContainer()
tt.Branch('br', 'ElectronContainer', ec)

assert tt.br.__class__.__name__ == 'ElectronContainer'
