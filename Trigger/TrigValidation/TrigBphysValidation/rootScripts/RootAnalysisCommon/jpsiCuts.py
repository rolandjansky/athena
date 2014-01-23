#!/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
##-----------------------------------------------------------
##
## runAnalyzeJpsi.py
##
## Configuration script for TriggerJpsiAnalysis
## Cuts for J/psi selection
## Python fragment to be included by
## runAnalyzeJpsi.py, runAnalyzeChain.py, and runCompareChains.py
##
## author: Daniel Scheirich <scheiric@cern.ch>
## Part of TriggerJpsiAnalysis in TrigBphysValidation package
##
##-----------------------------------------------------------
import string, os

##-----------------------------------------------------------
## J/psi selection cuts
##-----------------------------------------------------------
## To disable cut, just comment out that line
## Cut strings have the following format:
##    CutName(value)
##    CutName(min,max)

cuts = []

#cuts += [ "Level1(L1_MU0)"       ]    # Level-1 trigger
#cuts += [ "Level1(L2_mu0_missingRoi)"       ]    # Level-1 trigger

cuts += [ "JpsiMass(2000,5000)"  ]    # Invariant mass cut
cuts += [ "JpsiChi2Prob(0.05)"   ]    # Chi2 probability of J/psi vertex
cuts += [ "TrkChi2OnNdf(3.5)"    ]    # Track fit chi2/ndf cut

#cuts += [ "MuonPt(2800)"         ]    # Jpsi muon pT cut
## Yi's cuts
cuts += [ "MuonPt(2500)"         ]    # Jpsi muon pT cut
cuts += [ "MuonPt2(4000)"         ]   # Jpsi muon pT cut
cuts += [ "MuonEta(2.5)"         ]    # Jpsi opening angle in eta

cuts += [ "MuonE(3000)"          ]    # Jpsi muon energy cut
cuts += [ "MuonPixHits(1)"       ]    # Jpsi muon ID track number of pixel or b-layer hits
cuts += [ "MuonSctHits(6)"       ]    # Jpsi muon ID track number of SCT hits
#cuts += [ "MuonMdtHits(5)"       ]    # Jpsi muon number of MDT hits
cuts += [ "MuonTypes(ONE_COMB)"  ]    # Type of muons. Can be ALL == all muons, ONE_COMB == at least one combined, BOTH_COMB == both combined
#cuts += [ "DetPart(BB)"      ]    # Detector part: BB, EE, BE

#cuts += [ "JpsiDPhi(.75)"    ]    # Jpsi opening angle in phi
#cuts += [ "JpsiDEta(.75)"    ]    # Jpsi opening angle in eta


## end of fragment
