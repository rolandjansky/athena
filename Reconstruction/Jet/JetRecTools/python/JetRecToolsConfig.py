# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# JetModConfig: A helper module for configuring tools that support     #
# jet reconstruction                                                   #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################

from AthenaCommon import Logging
jrtlog = Logging.logging.getLogger('JetRecToolsConfig')

from AthenaConfiguration.ComponentFactory import CompFactory

# May need to specify non-standard tracking collections, e.g. for trigger
# Testing code -- move to another module and perhaps allow extensions
# e.g. in a dedicated trigger collections module to keep online/offline
# code more factorised
trackcollectionmap = {
    # Offline track collections
    "": {
        "Tracks":           "InDetTrackParticles",
        "JetTracks":        "JetSelectedTracks",
        "Vertices":         "PrimaryVertices",
        "TVA":              "JetTrackVtxAssoc",
        "GhostTracks":      "PseudoJetGhostTrack",
        "GhostTracksLabel": "GhostTrack",
    }
}

def getTrackSelTool(trkopt="",doWriteTracks=False):

    # Track selector needs its own hierarchical config getter in JetRecTools?
    idtrackselloose = CompFactory.getComp("InDet::InDetTrackSelectionTool")(
        "idtrackselloose",
        CutLevel         = "Loose",
        minPt            = 500,
        UseTrkTrackTools = False,
        Extrapolator     = "",
        TrackSummaryTool = ""
    )
    jettrackselloose = CompFactory.JetTrackSelectionTool(
        "jettrackselloose",
        Selector        = idtrackselloose
    )
    # Should phase this out completely!
    # Make a jet track selection alg
    # Elsewhere just use the ID track tool directly
    if doWriteTracks:
        jettracksname = "JetSelectedTracks"
        if trkopt: jettracksname += "_{}".format("trkopt")
        jettrackselloose.InputContainer  = trackcollectionmap[trkopt]["Tracks"]
        jettrackselloose.OutputContainer = jettracksname

    return jettrackselloose

def getTrackVertexAssocTool(trkopt=""):
    if trkopt: "_{}".format(trkopt)
    # Track-vertex association
    # This is to be deprecated
    # In fact can probably be switched already to match legacy master
    # but for a future MR
    idtvassoc = CompFactory.getComp("CP::TrackVertexAssociationTool")(
        "idloosetvassoc",
        VertexContainer         = trackcollectionmap[trkopt]["Vertices"],
    )

    jettvassoc = CompFactory.TrackVertexAssociationTool(
        "jettvassoc",
        TrackParticleContainer  = trackcollectionmap[trkopt]["Tracks"],
        TrackVertexAssociation  = trackcollectionmap[trkopt]["TVA"],
        VertexContainer         = trackcollectionmap[trkopt]["Vertices"],
        TrackVertexAssoTool     = idtvassoc,
    )
    return jettvassoc

