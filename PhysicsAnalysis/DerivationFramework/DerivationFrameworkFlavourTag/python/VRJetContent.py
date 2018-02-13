
# NOTE: I've left some of these variables commented out because we
# might want them, but I'll remove them once we've had more time to
# review.
#
#  - DG, 13 Feb 2018
#
_vr_jet_aux_vars = [
    # "InputType",
    # "SizeParameter",
    # "AlgorithmType",
    # "VariableRMinRadius",
    # "VariableRMassScale",
    # "ConstituentScale",
    # "GhostWBosons",
    # "GhostZBosons",
    # "JetGhostArea",
    "GhostWBosonsPt",
    "GhostZBosonsPt",
    # "GhostTQuarksFinal",
    "GhostWBosonsCount",
    "GhostZBosonsCount",
    "GhostTQuarksFinalPt",
    "GhostTQuarksFinalCount",
    "GhostTausFinalCount",
    # "GhostCQuarksFinalCount",
    "GhostHBosonsCount",
    # "GhostTausFinalPt",
    # "GhostBHadronsInitialCount",
    "GhostBHadronsFinalCount",
    # "GhostBQuarksFinalCount",
    # "GhostCQuarksFinalPt",
    # "GhostTausFinal",
    "HadronConeExclTruthLabelID",
    # "ConeExclTausFinal",
    "GhostCHadronsFinalCount",
    # "GhostCHadronsInitialCount",
    "HadronConeExclExtendedTruthLabelID",
    # "OriginVertex",
    # "ConeExclCHadronsFinal",
    # "GhostCQuarksFinal",
    "btaggingLink",
    "GhostHBosonsPt",
    # "GhostHBosons",
    # "GhostPartonsCount",
    # "GhostTruthCount",
    "GhostBHadronsFinalPt",
    # "GhostBHadronsInitialPt",
    # "GhostBQuarksFinalPt",
    "GhostCHadronsFinalPt",
    # "GhostCHadronsInitialPt",
    # "GhostBQuarksFinal",
    # "GhostBHadronsInitial",
    # "ConeExclBHadronsFinal",
    # "GhostBHadronsFinal",
    # "GhostCHadronsFinal",
    # "GhostCHadronsInitial",
    # "GhostPartonsPt",
    "JetConstitScaleMomentum_m",
    "JetConstitScaleMomentum_pt",
    # "GhostTruthPt",
    "JetConstitScaleMomentum_phi",
    "JetConstitScaleMomentum_eta",
    # "GhostPartons",
    # "GhostTruth"
]

AntiKtVR30Rmax4Rmin02TrackJetsCPContent = [
    'AntiKtVR30Rmax4Rmin02TrackJets',
    '.'.join(['AntiKtVR30Rmax4Rmin02TrackJetsAux'] + _vr_jet_aux_vars)
]
