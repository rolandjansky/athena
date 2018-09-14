#!/usr/bin/env python
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
#

import ROOT

def write_derived (fout, egamma, nder):
    egamma_der = egamma.CloneTree (0)
    egamma_der.SetName ('egamma_der')
    for i in range(nder):
        egamma.GetEntry(i)
        for j in range(egamma.el_n):
            egamma.el_E[j] *= 1.1
            egamma.el_eta[j] *= 1.05
            egamma.el_phi[j] *= 0.95
            for k in range(len(egamma.el_jetcone_dr[j])):
                egamma.el_jetcone_dr[j][k] *= 0.9
        for j in range(egamma.mu_n):
            egamma.mu_E[j] *= 1.2
            egamma.mu_pt[j] *= 1.1
            egamma.mu_m[j] *= 1.05
            egamma.mu_eta[j] *= 0.9
            egamma.mu_phi[j] *= 0.95
        egamma_der.Fill()
    egamma_der.Write()
    return
    

def ntuple_slim (infile, outfile, nder):
    fin = ROOT.TFile.Open (infile)
    CollectionTree = fin.CollectionTree
    egamma = fin.egamma
    egamma.SetBranchStatus ('*', 0)
    egamma.SetBranchStatus ('RunNumber', 1)
    egamma.SetBranchStatus ('EventNumber', 1)
    egamma.SetBranchStatus ('el_n', 1)
    egamma.SetBranchStatus ('el_eta', 1)
    egamma.SetBranchStatus ('el_phi', 1)
    egamma.SetBranchStatus ('el_E', 1)
    egamma.SetBranchStatus ('el_jetcone_dr', 1)
    egamma.SetBranchStatus ('mu_n', 1)
    egamma.SetBranchStatus ('mu_E', 1)
    egamma.SetBranchStatus ('mu_pt', 1)
    egamma.SetBranchStatus ('mu_m', 1)
    egamma.SetBranchStatus ('mu_eta', 1)
    egamma.SetBranchStatus ('mu_phi', 1)
    fout = ROOT.TFile.Open (outfile, 'recreate')
    CollectionTree_new = CollectionTree.CloneTree()
    CollectionTree_new.Write()
    egamma_new = egamma.CloneTree()
    egamma_new.Write()

    if nder > 0:
        write_derived (fout, egamma, nder)
    fout.Close()
    fin.Close()
    return


import sys
nder = 0
if len(sys.argv) >= 4:
    nder = int(sys.argv[3])
ntuple_slim (sys.argv[1], sys.argv[2], nder)

