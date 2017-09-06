# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# M. Hodgkinson, University of Sheffield
# Created July 16 2009, based on AthenaROOTAccess/test.py
# This is used by the countEvents.C macro...

import user  # look for .pythonrc.py for user init
import ROOT
import AthenaROOTAccess.transientTree
#Change this to your aod file.
aodFile='AOD.pool.root'

f = ROOT.TFile.Open (aodFile)
assert f.IsOpen()

branchNames = {}

tt = AthenaROOTAccess.transientTree.makeTree(f,persTreeName="MetaData", dhTreeName = 'MetaDataHdrDataHeader',branchNames = branchNames)
