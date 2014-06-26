#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################################
#
# A simple script demonstrating how to use MethodBDT, MethodCuts, etc. in python
#
################################################################################

import sys
import ROOT
ROOT.gSystem.Load("libTauDiscriminant.so")
from ROOT import TauID
from array import array
from itertools import izip
import math

ROOT.gInterpreter.GenerateDictionary("vector<vector<float> >", "vector")
ROOT.gInterpreter.GenerateDictionary("vector<vector<int> >", "vector")

dphi = lambda phi1, phi2 : abs(math.fmod((math.fmod(phi1, 2*math.pi) - math.fmod(phi2, 2*math.pi)) + 3*math.pi, 2*math.pi) - math.pi)

dR = lambda eta1, phi1, eta2, phi2: math.sqrt((eta1 - eta2)**2 + dphi(phi1, phi2)**2)

filename = sys.argv[1]

file = ROOT.TFile.Open(filename)
tree = file.Get("tauPerf")

bdt = TauID.MethodBDT()
bdtbits = TauID.MethodCuts()
cuts = TauID.MethodCuts()

variables = {
    "numTrack"                        : array('i',[0]),
    "num_pileup_and_primary_vertices" : array('i',[0]),
    "centFrac"                        : array('f',[0]),
    "etOverpTLeadTrk"                 : array('f',[0]),
    "trkAvgDist"                      : array('f',[0]),
    "effTopoInvMass"                  : array('f',[0]),
    "ipSigLeadTrk"                    : array('f',[0]),
    "lead3ClusterEOverAllClusterE"    : array('f',[0]),
    "numWideTrack"                    : array('i',[0]),
    "calRadius"                       : array('f',[0]),
    "drMax"                           : array('f',[0]),
    "massTrkSys"                      : array('f',[0]),
    "trFlightPathSig"                 : array('f',[0]),
    "pt"                              : array('f',[0]),
    "BDT"                             : array('f',[0]),
    "CALO_ISO_CORRECTED"              : array('f',[0])
}

for var, value in variables.items():
    bdt.addVariable(var.upper(), value, value.typecode.upper())
    bdtbits.addVariable(var.upper(), value, value.typecode.upper())
    cuts.addVariable(var.upper(), value, value.typecode.upper())

if not bdt.build("../share/jet.BDT.bin"):
    sys.exit(1)

if not bdtbits.build("../share/sig.bits.jet.BDT.txt"):
    sys.exit(1)

if not cuts.build("../share/cuts.txt"):
    sys.exit(1)

for entry in xrange(tree.GetEntries()):
    tree.GetEntry(entry)
    # update event variables
    nvtx = len(filter(lambda v: (tree.vxp_type[v]==1 and tree.vxp_nTracks[v]>=4) or \
                                (tree.vxp_type[v]==3 and tree.vxp_nTracks[v]>=2), range(tree.vxp_n)))
    variables["num_pileup_and_primary_vertices"][0] = nvtx

    for itau in xrange(tree.tau_n):
        # update tau variables
        variables["numTrack"][0] = tree.tau_seedCalo_numTrack[itau]
        variables["centFrac"][0] = tree.tau_seedCalo_centFrac[itau]
        variables["etOverpTLeadTrk"][0] = tree.tau_etOverPtLeadTrk[itau]
        variables["trkAvgDist"][0] = tree.tau_seedCalo_trkAvgDist[itau]
        variables["effTopoInvMass"][0] = tree.tau_effTopoInvMass[itau]
        variables["ipSigLeadTrk"][0] = tree.tau_ipSigLeadTrk[itau]
        variables["numWideTrack"][0] = tree.tau_seedCalo_wideTrk_n[itau]
        variables["calRadius"][0] = tree.tau_calcVars_calRadius[itau]
        variables["drMax"][0] = tree.tau_calcVars_drMax[itau]
        variables["massTrkSys"][0] = tree.tau_massTrkSys[itau]
        variables["trFlightPathSig"][0] = tree.tau_trFlightPathSig[itau]
        variables["pt"][0] = tree.tau_pt[itau]

        # calculate lead3ClusterEOverAllClusterE
        clusterE = sorted(tree.tau_cluster_E[itau], reverse=True)
        totE = sum(clusterE)
        variables["lead3ClusterEOverAllClusterE"][0] = sum(clusterE[:3]) / totE if totE > 0 else -1111.

        # calculate CALO_ISO_CORRECTED
        jvf = tree.tau_jet_jvtxf[itau]
        sumPtTrk = tree.tau_jet_sumPtTrk[itau]
        pt_pileup = (1. - jvf) * sumPtTrk
        calo_iso = 0.
        for E, eta, phi in izip(tree.tau_cluster_E[itau], tree.tau_cluster_eta[itau], tree.tau_cluster_phi[itau]):
            if .2 <= dR(tree.tau_seedCalo_eta[itau], tree.tau_seedCalo_phi[itau], eta, phi) < .4:
                calo_iso += E / math.cosh(eta)
        max_pileup_correction = 4000.
        alpha = 1.
        pileup_correction = alpha * pt_pileup
        if pileup_correction > max_pileup_correction:
            pileup_correction = max_pileup_correction
        calo_iso_corrected = calo_iso - pileup_correction
        variables["CALO_ISO_CORRECTED"][0] = calo_iso_corrected
        
        bdtScore = bdt.response()
        variables["BDT"][0] = bdtScore

        print "BDT Score: %.3f"% bdtScore
        print "BDT Loose: %i"% bdtbits.response(0)
        print "BDT Medium: %i"% bdtbits.response(1)
        print "BDT Tight: %i"% bdtbits.response(2)
        print "Cuts Loose: %i"% cuts.response(0)
        print "Cuts Medium: %i"% cuts.response(1)
        print "Cuts Tight: %i"% cuts.response(2)
        print "-"*30
