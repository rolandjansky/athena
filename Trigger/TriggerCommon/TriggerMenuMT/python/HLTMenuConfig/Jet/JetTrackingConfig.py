#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR, seqAND
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

        # Encapsulate the tracking algs in a view to avoid interference
        jetTrkViewSeq = seqAND( "JetTrackingViewSeq_"+trkopt, [] )
        jetTrkViewNode = parOR( "JetTrackingViewNode_"+trkopt, viewAlgs )

        # Alg to create the view
        import AthenaCommon.CfgMgr as CfgMgr
        jetTrkEVCA = CfgMgr.AthViews__MinimalViewAlg( "JetTrackingEVCA_"+trkopt )
        jetTrkEVCA.ViewNodeName = jetTrkViewNode.name()
        jetTrkViewSeq += jetTrkEVCA

        # View algs
        jetTrkViewSeq += jetTrkViewNode

        # Alg to retrieve the result
        jetTrkOutput = CfgMgr.AthViews__AliasOutOfView( "JetTrackingOutput_"+trkopt)
        jetTrkOutput.DataObjects = [ ('xAOD::TrackParticleContainer' , 'HLT_IDTrack_FS_FTF') ]
        jetTrkOutput.ViewName = jetTrkViewNode.name()
        jetTrkViewSeq += jetTrkOutput

        # Attach to main sequence
        jetTrkSeq += jetTrkViewSeq

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
