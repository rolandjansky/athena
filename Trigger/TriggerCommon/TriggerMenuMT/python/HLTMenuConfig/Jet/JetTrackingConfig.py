#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR
from TrigEDMConfig.TriggerEDMRun3 import recordable

from JetRecTools.JetRecToolsConfig import getTrackSelTool, getTrackVertexAssocTool, getTrackUsedInFitTool
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable

trkcollskeys = ["Tracks", "Vertices", "TVA", "GhostTracks", "GhostTracksLabel", "JetTracks"]

def JetTrackingSequence(dummyFlags,trkopt,RoIs):
    jetTrkSeq = parOR( "JetTrackingSeq_"+trkopt, [])
    tracksname = ""
    verticesname = ""

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'jet' )

    if trkopt=="ftf":
        from TrigInDetConfig.InDetSetup import makeInDetAlgsNoView
        viewAlgs = makeInDetAlgsNoView( config = IDTrigConfig, rois=RoIs)
        jetTrkSeq += viewAlgs
        tracksname =  IDTrigConfig.FT.tracksFTF( doRecord = IDTrigConfig.isRecordable ) 
        verticesname = recordable("HLT_IDVertex_FS")

    from TrigInDetConfig.TrigInDetPriVtxConfig import makeVertices
    vtxAlgs = makeVertices( "jet", tracksname, verticesname, IDTrigConfig )
    prmVtx = vtxAlgs[-1]
    jetTrkSeq += prmVtx

    tvaname = "JetTrackVtxAssoc_"+trkopt
    label = "GhostTrack_{}".format(trkopt)
    ghosttracksname = "PseudoJet{}".format(label)
    trkcolls = {
        "Tracks":           tracksname,
        "Vertices":         verticesname,
        "TVA":              tvaname,
        "GhostTracks" :     ghosttracksname,
        "GhostTracksLabel": label,
    }

    from JetRecTools.JetRecToolsConfig import trackcollectionmap
    if trkopt not in trackcollectionmap.keys():
        trackcollectionmap[trkopt] = trkcolls

    # Jet track selection
    jettrackselloose = getTrackSelTool(trkopt,doWriteTracks=True)
    jettracksname = jettrackselloose.OutputContainer
    jettvassoc = getTrackVertexAssocTool(trkopt)
    JetUsedInFitTrkDecoTool = getTrackUsedInFitTool(trkopt)

    trackcollectionmap[trkopt]["JetTracks"] = jettracksname

    jettrkprepalg = CompFactory.JetAlgorithm("jetalg_TrackPrep")
    jettrkprepalg.Tools = [ jettrackselloose, JetUsedInFitTrkDecoTool, jettvassoc ]
    jetTrkSeq += conf2toConfigurable( jettrkprepalg )

    pjgalg = CompFactory.PseudoJetAlgorithm(
        "pjgalg_"+label,
        InputContainer=tracksname,
        OutputContainer=ghosttracksname,
        Label=label,
        SkipNegativeEnergy=True
        )
    jetTrkSeq += conf2toConfigurable( pjgalg )

    return jetTrkSeq, trackcollectionmap[trkopt]
