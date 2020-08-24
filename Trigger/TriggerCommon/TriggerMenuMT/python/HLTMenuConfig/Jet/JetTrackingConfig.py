#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR
from TrigEDMConfig.TriggerEDMRun3 import recordable

from JetRecTools.JetRecToolsConfig import getTrackSelTool, getTrackVertexAssocTool
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable

def JetTrackingSequence(dummyFlags,trkopt,RoIs):
    jetTrkSeq = parOR( "JetTrackingSeq_"+trkopt, [])
    tracksname = ""
    verticesname = ""

    if trkopt=="ftf":
        from TrigInDetConfig.InDetSetup import makeInDetAlgsNoView
        # Guess FS rather than making it jet-specific?
        viewAlgs = makeInDetAlgsNoView( "JetFS", "FS", rois=recordable(RoIs) )
        jetTrkSeq += viewAlgs
        tracksname = recordable("HLT_IDTrack_FS_FTF")
        verticesname = recordable("HLT_IDVertex_FS")

    from TrigInDetConfig.TrigInDetPriVtxConfig import makeVertices
    vtxAlgs = makeVertices( "jet", tracksname, verticesname )
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
    jettrackselloose = getTrackSelTool(trkopt,doWriteTracks=True)
    jettracksname = jettrackselloose.OutputContainer
    jettvassoc = getTrackVertexAssocTool(trkopt)

    trackcollectionmap[trkopt]["JetTracks"] = jettracksname
    trackcollectionmap[trkopt]["TVA"] = tvaname

    jettrkprepalg = CompFactory.JetAlgorithm("jetalg_TrackPrep")
    jettrkprepalg.Tools = [ jettrackselloose, jettvassoc ]
    jetTrkSeq += conf2toConfigurable( jettrkprepalg )

    label = "GhostTrack_{}".format(trkopt)
    ghosttracksname = "PseudoJet{}".format(label)

    trackcollectionmap[trkopt]["GhostTracks"] = ghosttracksname
    trackcollectionmap[trkopt]["GhostTracksLabel"] = label

    pjgalg = CompFactory.PseudoJetAlgorithm(
        "pjgalg_"+label,
        InputContainer=tracksname,
        OutputContainer=ghosttracksname,
        Label=label,
        SkipNegativeEnergy=True
        )
    jetTrkSeq += conf2toConfigurable( pjgalg )

    return jetTrkSeq, trkcolls
