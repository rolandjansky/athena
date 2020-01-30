# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# from
# https://stackoverflow.com/questions/3663450/python-remove-substring-only-at-the-end-of-string
def rchop(thestring, ending):
    if thestring.endswith(ending):
        return thestring[:-len(ending)]

    return thestring


JetStandardAux = \
    [ "pt"
    , "eta"
    , "btagging"
    , "btaggingLink"
    , "HadronConeExclTruthLabelID"
    , "HadronConeExclExtendedTruthLabelID"
    , "ConeExclBHadronsFinal"
    , "ConeExclCHadronsFinal"
    ]

BTaggingStandardAux = \
    [ "MV2c10_discriminant"
    , "MV2rmu_discriminant"
    , "MV2r_discriminant"
    , "MV2c100_discriminant"
    , "MV2cl100_discriminant"
    , "DL1_pu"
    , "DL1_pc"
    , "DL1_pb"
    , "DL1r_pu"
    , "DL1r_pc"
    , "DL1r_pb"
    , "DL1rmu_pu"
    , "DL1rmu_pc"
    , "DL1rmu_pb"
    , "DL1mu_pu"
    , "DL1mu_pc"
    , "DL1mu_pb"
    ]

# These are the inputs to DL1rmu + SMT
BTaggingHighLevelAux = [
    "softMuon_dR",
    "softMuon_pTrel",
    "softMuon_scatteringNeighbourSignificance",
    "softMuon_momentumBalanceSignificance",
    "softMuon_qOverPratio",
    "softMuon_ip3dD0",
    "softMuon_ip3dD0Significance",
    "softMuon_ip3dZ0",
    "softMuon_ip3dZ0Significance",
    "JetFitter_mass",
    "JetFitter_isDefaults",
    "JetFitter_energyFraction",
    "JetFitter_significance3d",
    "JetFitter_nVTX",
    "JetFitter_nSingleTracks",
    "JetFitter_nTracksAtVtx",
    "JetFitter_N2Tpair",
    "JetFitter_deltaR",
    "SV1_NGTinSvx",
    "SV1_masssvx",
    "SV1_isDefaults",
    "SV1_N2Tpair",
    "SV1_efracsvx",
    "SV1_deltaR",
    "SV1_Lxy",
    "SV1_L3d",
    "SV1_significance3d",
    "IP2D_bu",
    "IP2D_isDefaults",
    "IP2D_bc",
    "IP2D_cu",
    "IP3D_bu",
    "IP3D_isDefaults",
    "IP3D_bc",
    "IP3D_cu",
    "JetFitterSecondaryVertex_nTracks",
    "JetFitterSecondaryVertex_isDefaults",
    "JetFitterSecondaryVertex_mass",
    "JetFitterSecondaryVertex_energy",
    "JetFitterSecondaryVertex_energyFraction",
    "JetFitterSecondaryVertex_displacement3d",
    "JetFitterSecondaryVertex_displacement2d",
    "JetFitterSecondaryVertex_maximumTrackRelativeEta",
    "JetFitterSecondaryVertex_minimumTrackRelativeEta",
    "JetFitterSecondaryVertex_averageTrackRelativeEta",
    "maximumTrackRelativeEta",
    "minimumTrackRelativeEta",
    "averageTrackRelativeEta",
    "rnnip_pb",
    "rnnip_pc",
    "rnnip_pu",
    "softMuon_pb",
    "softMuon_pc",
    "softMuon_pu",
    "softMuon_isDefaults",
]

BTaggingXbbAux = [
    'JetFitter_N2Tpair','JetFitter_dRFlightDir','JetFitter_deltaeta',
    'JetFitter_deltaphi', 'JetFitter_energyFraction', 'JetFitter_mass',
    'JetFitter_massUncorr', 'JetFitter_nSingleTracks',
    'JetFitter_nTracksAtVtx',
    'JetFitter_nVTX', 'JetFitter_significance3d',
    'SV1_L3d','SV1_Lxy','SV1_N2Tpair','SV1_NGTinSvx','SV1_deltaR',
    'SV1_dstToMatLay', 'SV1_efracsvx', 'SV1_masssvx', 'SV1_significance3d',
    'rnnip_pb','rnnip_pc','rnnip_ptau','rnnip_pu'
]

JetGhostLabelAux = [
    "GhostBHadronsFinalCount",
    "GhostCHadronsFinalCount",
    "GhostTausFinalCount",
]

BTaggingExtendedAux = [
    "BTagTrackToJetAssociator",
]

JetExtendedAux = [
    "GhostBHadronsFinalCount",
    "GhostBHadronsFinalPt",
    "GhostCHadronsFinalCount",
    "GhostCHadronsFinalPt",
    "GhostTausFinalCount",
    "GhostTausFinalPt",
    "GhostTrack",
]

def BTaggingExpertContent(jetcol):

    btaggingtmp = "BTagging_" + jetcol.split('Jets')[0]
    if 'BTagging' in jetcol:
         stamp = jetcol.split('BTagging')[1]
         btaggingtmp += '_'+stamp

    # deal with name mismatch between PV0TrackJets and BTagging_Track
    btagging = btaggingtmp.replace("PV0Track", "Track")

    jetAllAux = JetStandardAux + JetExtendedAux
    jetcontent = [ ".".join( [ jetcol + "Aux" ] + jetAllAux ) ]

    # add aux variables
    btaggingAllAux = (BTaggingHighLevelAux
                      + BTaggingStandardAux
                      + BTaggingExtendedAux)
    btagcontent = [ ".".join( [ btagging + "Aux" ] + btaggingAllAux ) ]

    return [jetcol] + jetcontent + [ btagging ] + btagcontent


def BTaggingStandardContent(jetcol):

    btaggingtmp = "BTagging_" + jetcol.split('Jets')[0]
    if 'BTagging' in jetcol:
         stamp = jetcol.split('BTagging')[1]
         btaggingtmp += '_'+stamp
    # deal with name mismatch between PV0TrackJets and BTagging_Track
    btagging = btaggingtmp.replace("PV0Track", "Track")

    jetcontent = \
        [ jetcol ] \
        + [ ".".join( [ jetcol + "Aux" ] + JetStandardAux ) ]

    btagcontent = \
        [ btagging ] \
        + [ ".".join( [ btagging + "Aux" ] + BTaggingStandardAux ) ]
    return jetcontent + btagcontent


def BTaggingXbbContent(jetcol):

    btaggingtmp = "BTagging_" + jetcol.split('Jets')[0]
    if 'BTagging' in jetcol:
         stamp = jetcol.split('BTagging')[1]
         btaggingtmp += '_'+stamp

    # deal with name mismatch between PV0TrackJets and BTagging_Track
    btagging = btaggingtmp.replace("PV0Track", "Track")

    jetAllAux = JetStandardAux + JetGhostLabelAux
    jetcontent = [ ".".join( [ jetcol + "Aux" ] + jetAllAux ) ]

    # add aux variables
    btaggingAllAux = BTaggingHighLevelAux + BTaggingStandardAux
    btagcontent = [ ".".join( [ btagging + "Aux" ] + btaggingAllAux ) ]

    return [jetcol] + jetcontent + [ btagging ] + btagcontent
