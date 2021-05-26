#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR

from JetRecTools.JetRecToolsConfig import getTrackVertexAssocTool
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, conf2toConfigurable
from TrigInDetConfig.TrigInDetPriVtxConfig import makeVertices


# these keys are not used in this file, they are used elsewhere, so 
# wouldn;t it be better to actually define them in the file where they 
# are needed ?

trkcollskeys = ["Tracks", "Vertices", "TVA", "GhostTracks", "GhostTracksLabel", "JetTracks"]

def get_trackcolls(trkopt, adaptiveVertex):
    if trkopt == "notrk":
        return {}
    elif trkopt == "ftf":
        from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
        IDTrigConfig = getInDetTrigConfig('jet')

        label = f"GhostTrack_{trkopt}"

        return {
            "Tracks": IDTrigConfig.tracks_FTF(),
            "Vertices": IDTrigConfig.adaptiveVertex if adaptiveVertex else IDTrigConfig.vertex,
            "TVA": f"JetTrackVtxAssoc_{trkopt}",
            "GhostTracks": f"PseudoJet{label}",
            "GhostTracksLabel": label,
            "JetTracks": f"JetSelectedTracks_{trkopt}",
        }
    else:
        raise KeyError(f"Unknown trkopt '{trkopt}'")


def JetTrackingSequence(dummyFlags,trkopt,RoIs):
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags

    jetTrkSeq = parOR( "JetTrackingSeq_"+trkopt, [])

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'jet' )

    trackcollmap = None

    if trkopt=="ftf":
        from TrigInDetConfig.InDetSetup import makeInDetAlgsNoView
        viewAlgs = makeInDetAlgsNoView( config = IDTrigConfig, rois=RoIs)
        jetTrkSeq += viewAlgs

        # add the collections for the eflowRec reconstriction in the trigger

        from eflowRec.PFHLTSequence import trackvtxcontainers
        trackvtxcontainers["ftf"] =  ( IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex if flags.Trigger.Jet.doAMVFPriorityTTVA else IDTrigConfig.vertex_jet )

        # now run he actual vertex finders and TTVA tools
        if flags.Trigger.Jet.doAMVFPriorityTTVA:
            trackcollmap = jetVertex( "jet", jetTrkSeq, trkopt, IDTrigConfig, verticesname=IDTrigConfig.vertex,     adaptiveVertex=IDTrigConfig.adaptiveVertex, )
        else:
            vtxAlgs = makeVertices( "amvf", IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex, IDTrigConfig, IDTrigConfig.adaptiveVertex )
            jetTrkSeq += vtxAlgs[-1]
            trackcollmap = jetVertex( "jet", jetTrkSeq, trkopt, IDTrigConfig, verticesname=IDTrigConfig.vertex_jet,     adaptiveVertex=IDTrigConfig.adaptiveVertex_jet, )
            

    return jetTrkSeq, trackcollmap

def JetTrackingCfg(flags, trkopt, RoIs):
    """ Create the tracking CA and return it as well as the output name dictionary """
    acc = ComponentAccumulator()
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'jet' )
    trackcollmap = None
    if trkopt == "ftf":
        from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg
        from TrigInDetConfig.TrigInDetPriVtxConfig import vertexFinderCfg
        acc.merge(trigInDetFastTrackingCfg(flags, RoIs, signatureName="jet", in_view=False))

        # Figure out the output containers
        label = f"GhostTrack_{trkopt}"
        trackcollmap = {
            "Tracks": IDTrigConfig.tracks_FTF(),
            "TVA": f"JetTrackVtxAssoc_{trkopt}",
            "GhostTracks": f"PseudoJet{label}",
            "GhostTracksLabel": label,
            "JetTracks": f"JetSelectedTracks_{trkopt}",
        }

        if flags.Trigger.Jet.doAMVFPriorityTTVA:
            trackcollmap["Vertices"] = IDTrigConfig.vertex
            acc.merge(
                JetVertexCfg(
                    flags, trkopt, IDTrigConfig.adaptiveVertex, trackcollmap,
                )
            )
        else:
            trackcollmap["Vertices"] = IDTrigConfig.vertex_jet
            acc.merge(
                vertexFinderCfg(
                    flags,
                    signature="jet",
                    inputTracks=trackcollmap["Tracks"],
                    outputVertices=IDTrigConfig.vertex,
                    adaptiveVertexing=IDTrigConfig.adaptiveVertex,
                )
            )
            acc.merge(
                JetVertexCfg(
                    flags, trkopt, IDTrigConfig.adaptiveVertex_jet, trackcollmap,
                )
            )
    else:
        raise ValueError(f"Unknown trkopt {trkopt}")

    # Add the pseudo-jet creator
    acc.addEventAlgo(
        CompFactory.PseudoJetAlgorithm(
            f"pjgalg_{trackcollmap['GhostTracksLabel']}",
            InputContainer=trackcollmap["Tracks"],
            OutputContainer=trackcollmap["GhostTracks"],
            Label=trackcollmap["GhostTracksLabel"],
            SkipNegativeEnergy=True,
        )
    )
    return acc, trackcollmap


def jetVertex( signature, jetseq, trkopt, config, verticesname=None, adaptiveVertex=None, selector=None ):

    # run the vertex algorithm ...

    if verticesname is None:
        verticesname = config.vertex
    if adaptiveVertex is None:
        adaptiveVertex = config.adaptiveVertex

    tracksname = config.tracks_FTF()

    vtxAlgs = makeVertices( signature, tracksname, verticesname, config, adaptiveVertex )
    prmVtx  = vtxAlgs[-1]
    jetseq += prmVtx

    outmap = None

    # track to vertex association ...

    tvaname = "JetTrackVtxAssoc_"+trkopt
    label = "GhostTrack_{}".format(trkopt)
    ghosttracksname = "PseudoJet{}".format(label)
    
        
    from JetRecTools.JetRecToolsConfig import trackcollectionmap
    if trkopt not in trackcollectionmap.keys():
        trkcolls = {
            "Tracks":           tracksname,
            "Vertices":         verticesname,
            "TVA":              tvaname,
            "GhostTracks" :     ghosttracksname,
            "GhostTracksLabel": label 
        }
            
        trackcollectionmap[trkopt] = trkcolls
    # why is some of this adding of parameters to the trackcollectionmap
    # done here, and some in getTrackSelTool ? Could these two functions 
    # not be combined or broken up into more transparent smaller functions ?
        
    # Jet track selection
    jettrackselloose =  getTrackSelTool_Trig( trkopt, doWriteTracks=True )
    jettracksname    = jettrackselloose.OutputContainer
    
    trackcollectionmap[trkopt]["JetTracks"] = jettracksname
    
    prepname         = "jetalg_TrackPrep"+trkopt
    jettvassoc       = getTrackVertexAssocTool( trkopt, jetseq ,
                                                ttva_opts = { "WorkingPoint" : "Custom",
                                                              "d0_cut"       : 2.0, 
                                                              "dzSinTheta_cut" : 2.0, 
                                                              "doPVPriority": adaptiveVertex,
                                                            }
                                              )

    jettrkprepalg       = CompFactory.JetAlgorithm(prepname)
    jettrkprepalg.Tools = [ jettrackselloose, jettvassoc ]
    jetseq             += conf2toConfigurable( jettrkprepalg )
    
    pjgalg = CompFactory.PseudoJetAlgorithm(
        "pjgalg_"+label,
        InputContainer=tracksname,
        OutputContainer=ghosttracksname,
        Label=label,
        SkipNegativeEnergy=True
    )
    
    jetseq += conf2toConfigurable( pjgalg )

    outmap = trackcollectionmap[trkopt]

    return outmap

def JetVertexCfg(flags, trkopt, adaptiveVertex, trkcolls):
    """ Create the jet vertexing """
    from TrigInDetConfig.TrigInDetPriVtxConfig import vertexFinderCfg
    from TrackVertexAssociationTool.TTVAToolConfig import TTVAToolCfg

    acc = vertexFinderCfg(
        flags,
        signature = "jet",
        inputTracks = trkcolls["Tracks"],
        outputVertices = trkcolls["Vertices"],
        adaptiveVertexing = adaptiveVertex)

    # Tell the central jet code about these collections/names
    from JetRecTools.JetRecToolsConfig import trackcollectionmap
    trackcollectionmap.setdefault(trkopt, trkcolls)

    # Create the track selection tool
    # TODO - this is not used anywhere that I can see so I'm skipping it

    # TODO - it would be better not to use this strange JetAlgorithm approach
    # Create the TTVA
    acc.addEventAlgo(
        CompFactory.JetAlgorithm(
            f"jetalg_TrackPrep{trkopt}",
            Tools = [
                CompFactory.TrackVertexAssociationTool(
                    "jettvassoc",
                    TrackParticleContainer = trkcolls["Tracks"],
                    TrackVertexAssociation = trkcolls["TVA"],
                    VertexContainer = trkcolls["Vertices"],
                    TrackVertexAssoTool = acc.popToolsAndMerge(
                        TTVAToolCfg(
                            flags,
                            f"trigJetTTVA{trkopt}",
                            WorkingPoint = "Custom",
                            d0_cut = 2.0,
                            dzSinTheta_cut = 2.0,
                            doPVPriority = adaptiveVertex,
                            TrackContName = trkcolls["Tracks"],
                            VertexContName = trkcolls["Vertices"],
                        )
                    ),
                )
            ]
        )
    )
    return acc

def getTrackSelTool_Trig( trkopt="", doWriteTracks=False, cutLevel="Loose", minPt=500 ):

    from JetRecTools.JetRecToolsConfig import trackcollectionmap

    # Track selector needs its own hierarchical config getter in JetRecTools?
    idtrackselloose = CompFactory.getComp("InDet::InDetTrackSelectionTool")(
        "idtrackselloose",
        CutLevel         = cutLevel,
        minPt            = minPt,
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
        # hack to retain the curent track collection name with "trkopt"
        # extnsion, and extend onle for additional vertex collections
        if trkopt: 
            jettracksname += "_{}".format(trkopt)
        jettrackselloose.InputContainer  = trackcollectionmap[trkopt]["Tracks"]
        jettrackselloose.OutputContainer = jettracksname

    return jettrackselloose

