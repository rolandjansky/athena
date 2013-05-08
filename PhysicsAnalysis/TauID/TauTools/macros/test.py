# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##########################################################
## Make the transient tree using ARA
import user  # look for .pythonrc.py for user init
import ROOT
import PyCintex
import AthenaROOTAccess.transientTree

# Put your AOD file here.
###### Do change this line to point to a proper AOD file.
f = ROOT.TFile.Open ('/cluster/data01/jyothsna/005188tid006904/AOD.pool_0_0.root')
assert f.IsOpen()

# Fill this in if you want to change the names of the transient branches.
branchNames = {}
#branchNames['ElectronCollection'] = 'ele'
#branchNames['PhotonCollection'] = 'gam'

tt = AthenaROOTAccess.transientTree.makeTree(f, branchNames = branchNames)
# tt is the transient tree "CollectionTree_trans" containing the (proxies) to
# all available transient data object in the file f.
# The original, persistent tree is declared as a friend
# of CollectionTree_trans, so that the transient tree will provide
# access to both transient data objects and to their persistent counterparts.
