#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR
from TrigEDMConfig.TriggerEDMRun3 import recordable

from JetRecTools.JetRecToolsConfig import getTrackSelTool, getTrackVertexAssocTool

def JetTrackingSequence(dummyFlags,trkopt,RoIs):
    jetTrkSeq = parOR( "JetTrackingSeq_"+trkopt, [])
    tracksname = ""
    verticesname = ""

    if trkopt=="ftf":
        from TrigInDetConfig.InDetSetup import makeInDetAlgs
        # Guess FS rather than making it jet-specific?
        viewAlgs = makeInDetAlgs( "JetFS", "_FS", rois=RoIs )
        jetTrkSeq += viewAlgs
        tracksname = recordable("HLT_xAODTracks_FS")
        verticesname = recordable("HLT_EFHistoPrmVtx")

    from TrigInDetConfig.TrigInDetPriVtxConfig import makeVertices
    vtxAlgs = makeVertices( "jet", "HLT_xAODTracks_FS", verticesname )
    prmVtx = vtxAlgs[-1]
    jetTrkSeq += prmVtx

    tvaname = "JetTrackVtxAssoc_"+trkopt
    trkcolls = {
        "Tracks":           tracksname,
        "Vertices":         verticesname,
        "TVA":              tvaname,
    }

    from JetRecTools.JetRecToolsConfig import trackcollectionmap
    if trkopt not in trackcollectionmap.keys():
        trackcollectionmap[trkopt] = trkcolls

    # Jet track selection
    jettrackselloose = getTrackSelTool(trkopt)
    jettracksname = jettrackselloose.OutputContainer
    jettvassoc = getTrackVertexAssocTool(trkopt)

    trackcollectionmap[trkopt]["JetTracks"] = jettracksname
    trackcollectionmap[trkopt]["TVA"] = tvaname

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

    trackcollectionmap[trkopt]["GhostTracks"] = ghosttracksname
    trackcollectionmap[trkopt]["GhostTracksLabel"] = label

    pjgalg = JetRecConf.PseudoJetAlgorithm(
        "pjgalg_"+label,
        PJGetter=pjg
        )
    jetTrkSeq += pjgalg

    return jetTrkSeq, trkcolls
