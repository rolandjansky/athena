# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import numpy as np
import warnings
def fill_branch_by_arr(filename ,treename, list_of_branchname, list_of_entrylist):
    of = ROOT.TFile(filename, 'update')
    if of.GetListOfKeys().Contains(treename):
        t = of.Get(treename)
        warnings.warn('%r exsists.' % t, Warning)
    else:
        t = ROOT.TTree(treename, treename)
    for branchname, entrylist in zip(list_of_branchname, list_of_entrylist):
	    t.SetEntries(entrylist.size)
	    address = np.zeros(1, 'float32')
	    br = t.Branch(branchname, address, branchname+'/F')
	    for en in entrylist:
	        address[0] = en
	        br.Fill()
    of.Write("", 2)
    of.Close()