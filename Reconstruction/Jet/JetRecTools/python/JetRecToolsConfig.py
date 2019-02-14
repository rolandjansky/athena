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

# Package configurable imports
from InDetTrackSelectionTool import InDetTrackSelectionToolConf
from TrackVertexAssociationTool import TrackVertexAssociationToolConf
from JetRecTools import JetRecToolsConf

# Could be made more configurable,
# e.g. specify CutLevel via modspec
def getTrackSelTool():
    # Track selector needs its own hierarchical config getter in JetRecTools?
    idtrackselloose = InDetTrackSelectionToolConf.InDet__InDetTrackSelectionTool(
        "idtrackselloose",
        CutLevel = "Loose",
        minPt    = 500
    )
    jettrackselloose = JetRecToolsConf.JetTrackSelectionTool(
        "jettrackselloose",
        InputContainer  = "InDetTrackParticles",
        OutputContainer = "JetSelectedTracks",
        Selector        = idtrackselloose
    )
    return jettrackselloose

# Could be made more configurable,
# e.g. specify association type
def getTrackVertexAssocTool():
    # Track-vertex association
    # In R21 and prior this was the "tight" tool, but that performed poorly
    # In fact, keeping it "tight" in R21 was a mistake
    # idtvassoc = getUniquePublicTool(algseq,"CP::LooseTrackVertexAssociationTool",
    #     "idloosetvassoc")
    idtvassoc = TrackVertexAssociationToolConf.CP__TightTrackVertexAssociationTool("idtighttvassoc")

    jettvassoc = JetRecToolsConf.TrackVertexAssociationTool(
        "jettvassoc",
        TrackParticleContainer  = "InDetTrackParticles",
        TrackVertexAssociation  = "JetTrackVtxAssoc",
        VertexContainer         = "PrimaryVertices",
        TrackVertexAssoTool     = idtvassoc,
    )
    return jettvassoc

