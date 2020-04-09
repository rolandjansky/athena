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
        from TrigInDetConfig.InDetSetup import makeInDetAlgs
        # Guess FS rather than making it jet-specific?
        viewAlgs = makeInDetAlgs( "JetFS", "_FS", rois=RoIs )
        jetTrkSeq += viewAlgs
        tracksname = recordable("HLT_IDTrack_FS_FTF")
        verticesname = recordable("HLT_EFHistoPrmVtx")

    from TrigInDetConfig.TrigInDetPriVtxConfig import makeVertices
    vtxAlgs = makeVertices( "jet", "HLT_IDTrack_FS_FTF", verticesname )
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

    #from JetRec import JetRecConf
    #jettrkprepalg = JetRecConf.JetAlgorithm("jetalg_TrackPrep")


    jettrkprepalg = CompFactory.JetAlgorithm("jetalg_TrackPrep")
    jettrkprepalg.Tools = [ jettrackselloose, jettvassoc ]
    jetTrkSeq += conf2toConfigurable( jettrkprepalg )

    label = "GhostTrack_{}".format(trkopt)
    ghosttracksname = "PseudoJet{}".format(label)
    pjg = CompFactory.PseudoJetGetter("pjg_{}".format(label),
                                     InputContainer=tracksname,
                                     OutputContainer=ghosttracksname,
                                     Label=label,
                                     SkipNegativeEnergy=True,
                                     GhostScale=1e-40)

    trackcollectionmap[trkopt]["GhostTracks"] = ghosttracksname
    trackcollectionmap[trkopt]["GhostTracksLabel"] = label

    pjgalg = CompFactory.PseudoJetAlgorithm(
        "pjgalg_"+label,
        PJGetter=pjg
        )
    jetTrkSeq += conf2toConfigurable( pjgalg )

    return jetTrkSeq, trkcolls
