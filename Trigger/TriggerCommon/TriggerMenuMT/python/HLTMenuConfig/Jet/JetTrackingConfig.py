#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR

from JetRecTools import JetRecToolsConfig as jrtcfg
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, conf2toConfigurable
from TrigInDetConfig.InDetTrigVertices import makeInDetTrigVertices

from AthenaConfiguration.AccumulatorCache import AccumulatorCache

def retrieveJetContext(trkopt):
    """Tell the standard jet config about the specific track related options we are using here.

     This is done by defining a new jet context into jetContextDic.
     Then, later, passing this context name in the JetDefinition and standard helper functions will ensure
    these options will consistently be used everywhere.

    returns the context dictionnary and the list of keys related to tracks in this dic.
    """

    from JetRecConfig.StandardJetContext import jetContextDic
    if trkopt not in jetContextDic:
        # *****************
        # Set the options corresponding to trkopt to a new entry in jetContextDic 
        from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
        IDTrigConfig = getInDetTrigConfig( 'jet' )

        tracksname = IDTrigConfig.tracks_FTF()
        verticesname = IDTrigConfig.vertex_jet
            
        tvaname = f"JetTrackVtxAssoc_{trkopt}"
        label = f"GhostTrack_{trkopt}"
        ghosttracksname = f"PseudoJet{label}"
        
        jetContextDic[trkopt] = jetContextDic['default'].clone(
            Tracks           = tracksname,
            Vertices         = verticesname,
            TVA              = tvaname,
            GhostTracks      = ghosttracksname,
            GhostTracksLabel = label ,
            JetTracks        = f'JetSelectedTracks_{trkopt}',
        )


        # also declare some JetInputExternal corresponding to trkopt
        # This ensures the JetRecConfig helpers know about them.
        # We declare simplistic JetInputExternal, without algoBuilder, because the rest of the trigger config is in charge of producing these containers.
        from JetRecConfig.StandardJetConstits import stdInputExtDic, JetInputExternal, xAODType
        stdInputExtDic[tracksname]   =  JetInputExternal( tracksname, xAODType.TrackParticle)
        stdInputExtDic[verticesname] =  JetInputExternal( verticesname, xAODType.Vertex)
        
    return jetContextDic[trkopt], jetContextDic["trackKeys"]

def JetFSTrackingSequence(dummyFlags,trkopt,RoIs):

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    # We really want jet, but that does more stuff (Hit-based DV wants L1 JET RoIs) so try bjet for now
    # Alternatively we could add the L1 JET RoIs to the ViewDataVerifier
    IDTrigConfig = getInDetTrigConfig( 'jet' )

    trackcollmap = None

    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTrackingNoView
    viewAlgs = makeInDetTrigFastTrackingNoView( config = IDTrigConfig, rois=RoIs)

    # add the collections for the eflowRec reconstriction in the trigger

    from eflowRec.PFHLTSequence import trackvtxcontainers
    trackvtxcontainers[trkopt] =  ( IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex_jet ) 

    vtxAlgs = makeInDetTrigVertices( "jet", IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex_jet, IDTrigConfig, IDTrigConfig.adaptiveVertex_jet )

    # now run he actual vertex finders and TTVA tools
    if IDTrigConfig.vertex_jet != IDTrigConfig.vertex:
        vtxAlgs += makeInDetTrigVertices( "amvf", IDTrigConfig.tracks_FTF(), IDTrigConfig.vertex, IDTrigConfig, IDTrigConfig.adaptiveVertex )

    jetTrkSeq = parOR( "JetFSTrackingSeq_"+trkopt, viewAlgs+vtxAlgs)
    trackcollmap = jetTTVA( "jet", jetTrkSeq, trkopt, IDTrigConfig, verticesname=IDTrigConfig.vertex_jet,  adaptiveVertex=IDTrigConfig.adaptiveVertex_jet )

    return jetTrkSeq, trackcollmap

def JetRoITrackingSequence(dummyFlags,trkopt,RoIs):

    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'jetSuper' )

    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTracking
    viewAlgs, viewVerify = makeInDetTrigFastTracking( config = IDTrigConfig, rois=RoIs)
    viewVerify.DataObjects += [( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs )]

    jetTrkSeq = parOR( "JetRoITrackingSeq_"+trkopt, viewAlgs)

    return jetTrkSeq


@AccumulatorCache
def JetFSTrackingCfg(flags, trkopt, RoIs):
    """ Create the tracking CA and return it as well as the output name dictionary """
    acc = ComponentAccumulator()
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( 'jet' )
    if trkopt == "ftf":
        from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg
        from TrigInDetConfig.TrigInDetPriVtxConfig import vertexFinderCfg
        acc.merge(trigInDetFastTrackingCfg(flags, RoIs, signatureName="jet", in_view=False))

        # get the jetContext for trkopt (and build it if not existing yet)
        jetContext, trkKeys = retrieveJetContext(trkopt)

        if IDTrigConfig.vertex_jet == IDTrigConfig.vertex:
            acc.merge(
                JetVertexCfg(
                    flags, trkopt, IDTrigConfig.adaptiveVertex, jetContext,
                )
            )
        else:
            acc.merge(
                vertexFinderCfg(
                    flags,
                    signature="jet",
                    inputTracks=jetContext["Tracks"],
                    outputVertices=IDTrigConfig.vertex,
                    adaptiveVertexing=IDTrigConfig.adaptiveVertex,
                )
            )
            acc.merge(
                JetVertexCfg(
                    flags, trkopt, IDTrigConfig.adaptiveVertex_jet, jetContext,
                )
            )
    else:
        raise ValueError(f"Unknown trkopt {trkopt}")

    # Add the pseudo-jet creator
    acc.addEventAlgo(
        CompFactory.PseudoJetAlgorithm(
            f"pjgalg_{jetContext['GhostTracksLabel']}",
            InputContainer=jetContext["Tracks"],
            OutputContainer=jetContext["GhostTracks"],
            Label=jetContext["GhostTracksLabel"],
            SkipNegativeEnergy=True,
        )
    )

    # make sure we output only the key,value related to tracks (otherwise, alg duplication issues)
    outmap = { k:jetContext[k] for k in trkKeys }
    
    return acc, outmap


def jetTTVA( signature, jetseq, trkopt, config, verticesname=None, adaptiveVertex=None, selector=None ):

    tracksname = config.tracks_FTF()

    label = f"GhostTrack_{trkopt}"

    # get the jetContext for trkopt (and build it if not existing yet)
    jetContext, trkKeys = retrieveJetContext(trkopt)

    # *****************************
    # Jet track selection algorithm
    jettrackselalg = jrtcfg.getTrackSelAlg( trkopt )

    # Track-vtx association. We create a TrackVertexAssocTool then call it through a
    # JetAlgorithm which just calls its execute() method. In the future the plan is to
    # convert this TrackVertexAssocTool in a simple alg just as for track selection.
    jettvassoc       = jrtcfg.getTrackVertexAssocTool( trkopt, jetseq ,
                                                       ttva_opts = { "WorkingPoint" : "Custom",
                                                                     "d0_cut"       : 2.0, 
                                                                     "dzSinTheta_cut" : 2.0, 
                                                                     "doPVPriority": adaptiveVertex,
                                                                    }
                                                                 )    
    jettrkprepalg       = CompFactory.JetAlgorithm("jetalg_TrackPrep"+trkopt,
                                                   Tools = [  jettvassoc ])

    # Pseudojets for ghost tracks
    pjgalg = CompFactory.PseudoJetAlgorithm(
        "pjgalg_"+label,
        InputContainer=tracksname,
        OutputContainer=jetContext["GhostTracks"],
        Label=label,
        SkipNegativeEnergy=True
    )

    # Add the 3 algs to the sequence :
    jetseq += conf2toConfigurable( jettrackselalg )
    jetseq += conf2toConfigurable( jettrkprepalg )
    jetseq += conf2toConfigurable( pjgalg )


    # make sure we output only the key,value related to tracks (otherwise, alg duplication issues)
    outmap = { k:jetContext[k] for k in trkKeys }
    return outmap

@AccumulatorCache
def JetVertexCfg(flags, trkopt, adaptiveVertex, jetContext):
    """ Create the jet vertexing """
    from TrigInDetConfig.TrigInDetPriVtxConfig import vertexFinderCfg
    from TrackVertexAssociationTool.TTVAToolConfig import TTVAToolCfg

    acc = vertexFinderCfg(
        flags,
        signature = "jet",
        inputTracks = jetContext["Tracks"],
        outputVertices = jetContext["Vertices"],
        adaptiveVertexing = adaptiveVertex)

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
                    TrackParticleContainer = jetContext["Tracks"],
                    TrackVertexAssociation = jetContext["TVA"],
                    VertexContainer = jetContext["Vertices"],
                    TrackVertexAssoTool = acc.popToolsAndMerge(
                        TTVAToolCfg(
                            flags,
                            f"trigJetTTVA{trkopt}",
                            WorkingPoint = "Custom",
                            d0_cut = 2.0,
                            dzSinTheta_cut = 2.0,
                            doPVPriority = adaptiveVertex,
                            TrackContName = jetContext["Tracks"],
                            VertexContName = jetContext["Vertices"],
                        )
                    ),
                )
            ]
        )
    )
    return acc

