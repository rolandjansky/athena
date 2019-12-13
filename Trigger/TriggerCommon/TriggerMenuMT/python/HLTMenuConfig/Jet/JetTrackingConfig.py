#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR
from TrigEDMConfig.TriggerEDMRun3 import recordable

from JetRecTools import JetRecToolsConf

# From JetRecToolsConfig -- needs more than offline, though could extend that setup with more options
# For now, leave until this has stabilised
def getTrackSelTool(trkopt, tracksname):
    jettracksname = "JetSelectedTracks_{}".format("trkopt")

    # Track selector needs its own hierarchical config getter in JetRecTools?
    from InDetTrackSelectionTool import InDetTrackSelectionToolConf
    idtrackselloose = InDetTrackSelectionToolConf.InDet__InDetTrackSelectionTool(
        "idtrackselloose",
        CutLevel         = "Loose",
        minPt            = 500,
        UseTrkTrackTools = False,
        Extrapolator     = "",
        TrackSummaryTool = ""
    )
    jettrackselloose = JetRecToolsConf.JetTrackSelectionTool(
        "jettrackselloose",
        InputContainer  = tracksname,
        OutputContainer = jettracksname,
        Selector        = idtrackselloose
    )
    return jettrackselloose, jettracksname

def getTrackVertexAssocTool(trkopt,tracksname,verticesname):
    tvaname = "JetTrackVtxAssoc_{}".format(trkopt)
    # Track-vertex association
    from TrackVertexAssociationTool import TrackVertexAssociationToolConf
    idtvassoc = TrackVertexAssociationToolConf.CP__TrackVertexAssociationTool("idloosetvassoc")

    jettvassoc = JetRecToolsConf.TrackVertexAssociationTool(
        "jettvassoc",
        TrackParticleContainer  = tracksname,
        TrackVertexAssociation  = tvaname,
        VertexContainer         = verticesname,
        TrackVertexAssoTool     = idtvassoc,
    )
    return jettvassoc, tvaname


def JetTrackingSequence(dummyFlags,trkopt,RoIs):
    jetTrkSeq = parOR( "JetTrackingSeq_"+trkopt, [])
    tracksname = ""
    verticesname = ""

    if trkopt=="ftf":
        from TrigInDetConfig.InDetSetup import makeInDetAlgs
        # Guess FS rather than making it jet-specific?
        viewAlgs = makeInDetAlgs( "FS", rois=RoIs )

        for alg in viewAlgs:
            if "RoIs" in alg.properties():
                alg.RoIs = RoIs
            if "roiCollectionName" in alg.properties():
                alg.roiCollectionName = RoIs
            jetTrkSeq += alg
        tracksname = recordable("HLT_xAODTracks_FS")
        verticesname = recordable("HLT_EFHistoPrmVtx")

    from TrigInDetConfig.TrigInDetPriVtxConfig import makeVertices
    vtxAlgs = makeVertices( "jet", "HLT_xAODTracks_FS", verticesname )
    prmVtx = vtxAlgs[-1]
    jetTrkSeq += prmVtx

    # Jet track selection
    jettrackselloose, jettracksname = getTrackSelTool(trkopt, tracksname)
    jettvassoc, tvaname = getTrackVertexAssocTool(trkopt, tracksname, verticesname)

    from JetRec import JetRecConf
    jettrkprepalg = JetRecConf.JetAlgorithm("jetalg_TrackPrep")
    jettrkprepalg.Tools = [ jettrackselloose, jettvassoc ]
    jetTrkSeq += jettrkprepalg

    label = "GhostTrack_{}".format(trkopt)
    ghosttracksname = "PseudoJet{}".format(label)
    pjg = JetRecConf.PseudoJetGetter("pjg_{}".format(label),
                                     InputContainer=tracksname,                                     
                                     OutputContainer=ghosttracksname,
                                     Label=label,
                                     SkipNegativeEnergy=True,
                                     GhostScale=1e-40)

    pjgalg = JetRecConf.PseudoJetAlgorithm(
        "pjgalg_"+label,
        PJGetter=pjg
        )
    jetTrkSeq += pjgalg

    trkcolls = {
        "Tracks":           tracksname,
        "Vertices":         verticesname,
        "JetTracks":        jettracksname,
        "TVA":              tvaname,
        "GhostTracks":      ghosttracksname,
        "GhostTracksLabel": label
    }

    from JetMomentTools.JetMomentToolsConfig import trackcollectionmap
    if trkopt not in trackcollectionmap.keys():
        trackcollectionmap[trkopt] = trkcolls

    return jetTrkSeq, trkcolls
