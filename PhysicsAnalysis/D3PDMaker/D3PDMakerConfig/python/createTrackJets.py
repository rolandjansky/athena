# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# @file egammaD3PDMaker/python/createTrackJets.py
# @author Mike Flowerdew
# @date Nov, 2010
# @brief Create track jet collection
#

from JetRec.JetGetters import *

def createTrackJets (seq):
    
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
    theEGammaTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(
        name                 = "egammaTrackJetTrackSelectorTool",
        pTMin                = 300.,
        etaMax               = 9999.,
        nHitBLayer           = 0,
        nHitPix              = 0,
        nHitBLayerPlusPix    = 0,
        nHitSct              = 0,
        nHitSi               = 5,
        nHitTrt              = 0,
        IPd0Max              = 100.,  # d0 cut
        IPz0Max              = 100.,  # z0*sin(theta) cut
        z0Max                = 1000., # z0 cut
        fitChi2OnNdfMax      = 1000, 
        d0significanceMax    = -1.,
        z0significanceMax    = -1.,
        TrackSummaryTool     = None,
        OutputLevel          = 3)

    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += theEGammaTrackSelectorTool
        
    # JetFinder (Anti-Kt)
    import JetRec.JetFinderConfig as thebtagJetFinder
    theEGammaFastJetKt = thebtagJetFinder.getFastKt(jetalgname = "egammaTrackJetFinder",
                                                    radius     = 0.5,
                                                    algorithm  = 'anti-kt',
                                                    flipvalues = 2)
    ToolSvc += theEGammaFastJetKt
    
    # Setup clustering tool
    from JetRecTools.JetRecToolsConf import JetTrackZClusterTool
    egammaTrackZClusterTool = JetTrackZClusterTool(
        name                       = "egammaTrackZClusterTool",
        TrackJetMinMulti           = 2,
        TrackJetMinPt              = 2000.,
        UseVtxSeeding              = True,
        DeltaZRange                = 10000.0,
        TrackParticleContainerName = "TrackParticleCandidate",
        VxContainerName            = "VxPrimaryCandidate",
        TrackSelector              = theEGammaTrackSelectorTool,
        JetFinder                  = theEGammaFastJetKt,
        OutputLevel                = 3)
    
    egammaToolList  = [ egammaTrackZClusterTool,
                        JetSorterTool (SortOrder = 'ByEtDown') ]

    make_StandardJetGetter('AntiKt', 0.5, 'Track', seq,
                           outputCollectionName = 'AntiKtZ5TrackJets',
                           allTools = egammaToolList)

    return

