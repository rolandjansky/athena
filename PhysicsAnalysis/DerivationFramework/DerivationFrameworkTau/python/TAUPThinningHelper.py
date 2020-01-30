# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#############################################################
# Common code using for TAUP Thinning                       #
# reason: to keep the truth container, but only store       #
#         the tau and its decay products                    #
# it is modelled after                                      #
# DerivationFrameworkHiggs/trunk/python/HIGG4DxThinning.py  #
# special thanks to Zinonas Zinonos                         #
#                                                           #
# Nikolaos.Rompotis at cern.ch                              #
# 25.10.2016                                                #
#############################################################

from AthenaCommon.GlobalFlags import globalflags

DFisMC = (globalflags.DataSource()=='geant4')

def setup(TAUPxName, TAUPxThinningSvc, ToolSvc):
    thinningTools = []

    # if it is not MC do nothing
    if not DFisMC:
        return thinningTools

    # do nothing for TAUP2 because it is a data only derivation
    if (TAUPxName != "TAUP1") and (TAUPxName != "TAUP3") and (TAUPxName != "TAUP4") and (TAUPxName != "TAUP5"):
        return thinningTools

    truth_cond_tau = "((abs(TruthParticles.pdgId) == 15 || abs(TruthParticles.pdgId) == 16) && (TruthParticles.pt > 0.01*GeV) && (TruthParticles.barcode<200000))"
    truth_cond_lep = "((abs(TruthParticles.pdgId) >= 11 && abs(TruthParticles.pdgId) <= 14) && (TruthParticles.pt > 4.0*GeV) && (TruthParticles.status == 1) && (TruthParticles.barcode<200000))"
    truth_cond_comb = "("+truth_cond_lep+"||"+truth_cond_tau+")"

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning

    # thin taus
    TAUPxTruthTool_TAU = DerivationFramework__GenericTruthThinning(
        name                         = TAUPxName+"TruthTool_TAU",
        ThinningService              = TAUPxThinningSvc,
        ParticleSelectionString      = truth_cond_tau,
        PreserveDescendants          = False,
        PreserveGeneratorDescendants = True,
        PreserveAncestors            = False,
        TauHandling                  = False)

    ToolSvc += TAUPxTruthTool_TAU
    thinningTools.append(TAUPxTruthTool_TAU)

    #thin leptons and taus
    TAUPxTruthTool_COMB = DerivationFramework__GenericTruthThinning(
        name                         = TAUPxName+"TruthTool_COMB",
        ThinningService              = TAUPxThinningSvc,
        ParticleSelectionString      = truth_cond_comb,
        PreserveDescendants          = False,
        PreserveGeneratorDescendants = False,
        PreserveAncestors            = True,
        TauHandling                  = False)
    
    ToolSvc += TAUPxTruthTool_COMB
    thinningTools.append(TAUPxTruthTool_COMB)

    return thinningTools
