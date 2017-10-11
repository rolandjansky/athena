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


def BTaggingExpertContent(jetcol):
    # TODO
    return BTaggingStandardContent(jetcol)


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
