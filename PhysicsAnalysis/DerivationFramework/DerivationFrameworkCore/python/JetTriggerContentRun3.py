# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

JetTriggerContentRun3 = [
# Small-R EMTopo jets
# Small-R EMTopo jets w/ area-based PU subtraction + Jet Energy Scale corrections
"HLT_AntiKt4EMTopoJets_subjes",
"HLT_AntiKt4EMTopoJets_subjesAux.pt.eta.phi.m",
# Small-R EMTopo jets w/ area-based PU subtraction + Jet Energy Scale corrections + in-situ corrections + calo-only Global Sequential Calibration (GSC) (Run 2 default)
"HLT_AntiKt4EMTopoJets_subjesIS",
"HLT_AntiKt4EMTopoJets_subjesISAux.pt.eta.phi.m",
# Small-R EMTopo jets w/ area-based PU subtraction + Jet Energy Scale (JES) calibration + in-situ (IS) corrections + calo+track-based GSC
"HLT_AntiKt4EMTopoJets_subjesgscIS_ftf",
"HLT_AntiKt4EMTopoJets_subjesgscIS_ftfAux.pt.eta.phi.m",
# Small-R EMTopo jets w/ area-based PU subtraction + Jet Energy Scale (JES) calibration + in-situ (IS) corrections + calo+track-based GSC + residual PU calibration (mu and NPV-dependent)
"HLT_AntiKt4EMTopoJets_subresjesgscIS_ftf",
"HLT_AntiKt4EMTopoJets_subresjesgscIS_ftfAux.pt.eta.phi.m",

# Small-R EMPFlow jets w/ full scan HLT tracking
# Small-R EMPFlow jets w/ area-based PU subtraction + Jet Energy Scale corrections
"HLT_AntiKt4EMPFlowJets_subjes_ftf",
"HLT_AntiKt4EMPFlowJets_subjes_ftfAux.pt.eta.phi.m",
# Small-R EMPFlow jets w/ area-based PU subtraction + Jet Energy Scale corrections + in-situ corrections + calo-only Global Sequential Calibration (GSC) (Run 2 default)
"HLT_AntiKt4EMPFlowJets_subjesIS_ftf",
"HLT_AntiKt4EMPFlowJets_subjesIS_ftfAux.pt.eta.phi.m",
# Small-R EMPFlow jets w/ area-based PU subtraction + Jet Energy Scale (JES) calibration + in-situ (IS) corrections + calo+track-based GSC
"HLT_AntiKt4EMPFlowJets_subjesgscIS_ftf",
"HLT_AntiKt4EMPFlowJets_subjesgscIS_ftfAux.pt.eta.phi.m",
# Small-R EMPFlow jets w/ area-based PU subtraction + Jet Energy Scale (JES) calibration + in-situ (IS) corrections + calo+track-based GSC + residual PU calibration (mu and NPV-dependent) (Run 3 default)
"HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf",
"HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftfAux.pt.eta.phi.m",

# Large-R jets reclustered from small-R EMTopo jets
"HLT_AntiKt10EMTopoRCJets_subjesIS",
"HLT_AntiKt10EMTopoRCJets_subjesISAux.pt.eta.phi.m",

# Large-R EMTopo jets w/ local calibration and trimming + JES corrections
"HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jes",
"HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jesAux.pt.eta.phi.m",

# Large-R EMPFlow jets w/ constituent-level PU subtraction, uncalibrated (additional calibration variables can be added here)
"HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_nojcalib_ftf",
"HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_nojcalib_ftfAux.pt.eta.phi.m",

# Large-R EMPFlow jets w/ constituent-level PU subtraction, w/ (offline-based) JES calibration
"HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_jes_ftf",
"HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_jes_ftfAux.pt.eta.phi.m",

# L1 containers
# Small-R jFEX jets
"L1_jFexSRJetRoI",
"L1_jFexSRJetRoIAux.",

# Large-R jFEX jets
"L1_jFexLRJetRoI",
"L1_jFexLRJetRoIAux.",

# Small-R gFEX jets
"L1_gFexSRJetRoI",
"L1_gFexSRJetRoIAux.",

# Large-R gFEX jets
"L1_gFexLRJetRoI",
"L1_gFexLRJetRoIAux.",




]
