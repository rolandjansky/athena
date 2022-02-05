# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
""" 
This module defines the standard 'jet contexts'. 
A jet context is a set of options (mainly related to Tracks) shared by several 
components (modifier tools, input tools/algs, ...).

Other contexts are expected to be defined, for example in the trigger to deal with oher track collection, or for 
analysis having non-default PV0 choices.

Setting a context to a JetDefinition ensures identical properties are  consistently set across the components configured by the JetDefinition. 
"""
from JetRecConfig.Utilities import ldict


jetContextDic = ldict(
    #**********************************
    # The default set of common jet options :
    default = ldict(
        Tracks =               "InDetTrackParticles",
        JetTracks =            "JetSelectedTracks", #used for e.g. ghost tracks (no track quality criteria applied)
        JetTracksQualityCuts = "JetSelectedTracks_trackSelOpt", #used for track-jets (trackSelOpt quality criteria applied)
        Vertices =             "PrimaryVertices",
        TVA =                  "JetTrackVtxAssoc",
        GhostTracks =          "PseudoJetGhostTrack",
        GhostTracksLabel =     "GhostTrack",

        GhostTrackCutLevel =   'NoCut', # The track selection level for ghost-associated tracks. This is different from the cutlevel we apply when performing actual calculations such as JVT or tack moments.

        # options passed to InDet::InDetTrackSelectionTool.
        #   Note : these are the standard options used for track calculations. Tracks selected for ghost-associaton have CutLevel=NoCut by default : see ghostTrackCutLevel above
        trackSelOptions = ldict( CutLevel = "Loose", minPt=500 ),

    ),

    #**********************************
    # This is not a jet context, but the list of keys related to track.
    # We store it here since it is a related central place. Used in trigger config.  
    trackKeys = ["Tracks", "Vertices", "TVA", "GhostTracks", "GhostTracksLabel", "JetTracks", "JetTracksQualityCuts"],
)

jetContextDic[""] = jetContextDic["default"]
jetContextDic["notrk"] = jetContextDic["default"] # used in trigger


def propFromContext(propName):
    """Some properties might depend on the context for which jets are configured.
    This function returns a helper function which gives the value of the property propName according to the current context.
    
    """
    def getProp(jetdef, spec):
        context = jetdef.context
        if isinstance(spec, str):
            # user may have passed explicitly a str : allow to overide jetdef.context if non void
            context = spec or context
        return jetContextDic[context][propName]
    return getProp

def inputsFromContext(inputKey):
    """Some prerequisites might depend on the context for which jets are configured.
    This function returns a helper function which gives a list of input prerequisites according to 'inputKey' in the current jetdef.context.
    
    """
    def getPrereqs(jetdef):
        return ["input:"+jetContextDic[jetdef.context][inputKey] ]
    return getPrereqs
