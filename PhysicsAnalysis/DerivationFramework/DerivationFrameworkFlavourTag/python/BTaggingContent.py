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
    , "MV2c10mu_discriminant"
    , "MV2c10rnn_discriminant"
    , "MV2c100_discriminant"
    , "MV2cl100_discriminant"
    , "DL1_pu"
    , "DL1_pc"
    , "DL1_pb"
    , "DL1mu_pu"
    , "DL1mu_pc"
    , "DL1mu_pb"
    , "DL1rnn_pu"
    , "DL1rnn_pc"
    , "DL1rnn_pb"
    ]

# These are roughly the inputs to MV2 or DL1
BTaggingHighLevelAux = [
    "IP2D_pb", "IP2D_pc", "IP2D_pu",
    "IP3D_pb", "IP3D_pc", "IP3D_pu",
    "SV1_pu", "SV1_pb", "SV1_pc",
    "rnnip_pu", "rnnip_pc", "rnnip_pb", "rnnip_ptau",
    "JetFitter_energyFraction", "JetFitter_mass",
    "JetFitter_significance3d", "JetFitter_deltaphi", "JetFitter_deltaeta",
    "JetFitter_massUncorr", "JetFitter_dRFlightDir",
    "SV1_masssvx", "SV1_efracsvx", "SV1_significance3d", "SV1_dstToMatLay",
    "SV1_deltaR", "SV1_Lxy", "SV1_L3d",
    "JetFitter_nVTX", "JetFitter_nSingleTracks", "JetFitter_nTracksAtVtx",
    "JetFitter_N2Tpair",
    "SV1_N2Tpair", "SV1_NGTinSvx"
]

def BTaggingExpertContent(jetcol):
    btaggingtmp = "BTagging_" + rchop(jetcol, "Jets")
    # deal with name mismatch between PV0TrackJets and BTagging_Track
    btagging = btaggingtmp.replace("PV0Track", "Track")

    # add aux variables
    btagging_aux = [ ".".join( [ btagging + "Aux" ] + BTaggingHighLevelAux ) ]

    return BTaggingStandardContent(jetcol) + [ btagging ] + btagging_aux


def BTaggingStandardContent(jetcol):
    btaggingtmp = "BTagging_" + rchop(jetcol, "Jets")

    # deal with name mismatch between PV0TrackJets and BTagging_Track
    btagging = btaggingtmp.replace("PV0Track", "Track")


    jetcontent = \
        [ jetcol ] \
        + [ ".".join( [ jetcol + "Aux" ] + JetStandardAux ) ]

    btagcontent = \
        [ btagging ] \
        + [ ".".join( [ btagging + "Aux" ] + BTaggingStandardAux ) ]

    return jetcontent + btagcontent
