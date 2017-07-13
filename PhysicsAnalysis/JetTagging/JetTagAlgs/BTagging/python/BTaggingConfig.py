# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetRec.JetAlgConfiguration import editParm,nameBuilder

def performBTagging(finder, mainParam, input, **options):
    """Check if the jet collection defined by the input parameters needs btagging.
    input  : the same as defined in JetAlgConfiguration
    output : (result, key) where result is True or False and key is a str identifying the collection"""
    
    # super simple ... 
    key = options['outputCollectionName']
    if key is None:
        key = nameBuilder(finder,  editParm(mainParam), options['finderSuff'],
                                 input,options['inputSuff'], options['globalSuff'],)
    else:
        key = key.replace('Jets','')
    
    from BTagging.BTaggingFlags import BTaggingFlags
    
    return ( key in BTaggingFlags.Jets , key ) 

def getJetBTaggerTool(finder, mainParam, input, **options):
    """ Copy from JetGetters.py and modified by Yoshikazu NAGAI to run two muon chains parallel"""

    from BTagging.BTaggingFlags import BTaggingFlags
    from AthenaCommon.AppMgr import ToolSvc
    from JetRecTools.JetRecToolsConf import JetBTaggerTool
    
    btagger = JetBTaggerTool(
        name                       = 'JetBTagger',
        BTagTool                   = ToolSvc.myBTagTool,
        TrackParticleContainerName = BTaggingFlags.TrackParticleCollectionName,
        ParticleToJetAssociator    = ToolSvc.BTagTrackToJetAssociator,
        OutputLevel                = BTaggingFlags.OutputLevel
        )
    
    if ( BTaggingFlags.SoftMu | BTaggingFlags.SoftMuChi2 ):
        btagger.MuonContainerName       = BTaggingFlags.MuonCollectionName

    if ( BTaggingFlags.SoftMu | BTaggingFlags.SoftMuChi2 ):
        btagger.MuonToJetAssociator     = ToolSvc.BTagMuonToJetAssociator

    if BTaggingFlags.MultiSV:
        btagger.AdditionalTrackToJetAssocName = [ "TracksBB" ]
        btagger.AdditionalTrackToJetAssociator = [ ToolSvc.BTagTrackToJetAssociatorBB ]

    return (btagger)
