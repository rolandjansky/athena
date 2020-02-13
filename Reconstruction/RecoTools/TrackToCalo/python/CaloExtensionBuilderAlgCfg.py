# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory

def getCaloExtenstionBuilderAlgorithm(inputFlags, cutLevel = "TightPrimary", minPT = 100.0):
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg    
    Trk__ParticleCaloExtensionTool = CompFactory.Trk__ParticleCaloExtensionTool
    extrapPFlowCfg = AtlasExtrapolatorCfg(inputFlags)
    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = extrapPFlowCfg.popPrivateTools())

    
    #from TrackToCalo.TrackToCaloConf import Trk__CaloExtensionBuilderAlg as CaloExtensionBuilderAlg
    CaloExtensionBuilderAlg = CompFactory.Trk__CaloExtensionBuilderAlg 
    CaloExtensionBuilderAlg = CaloExtensionBuilderAlg(LastCaloExtentionTool = pcExtensionTool)

    if (inputFlags.Beam.Type != "collisions"):
        CaloExtensionBuilderAlg.vertexInputContainer = ""

    InDet__InDetTrackSelectionTool = CompFactory.InDet__InDetTrackSelectionTool
    InDet__InDetDetailedTrackSelectorTool = CompFactory.InDet__InDetDetailedTrackSelectorTool

    TrackSelectionToolHC = InDet__InDetTrackSelectionTool(name = "CaloExtensionBuilderTrackSelectionTool",minPt = minPT, CutLevel = cutLevel)  

    extrapIDSelectorCfg = AtlasExtrapolatorCfg(inputFlags)
    TrackDetailedSelectionToolHC = InDet__InDetDetailedTrackSelectorTool(name = "CaloExtensionBuilderDetailedTrackSelectionTool",
                                                                         pTMin                = minPT,
                                                                         IPd0Max              = 1.,
                                                                         IPz0Max              = 1.5, 
                                                                         useTrackSummaryInfo  = True,
                                                                         nHitBLayer           = 0, 
                                                                         nHitPix              = 2,  # PixelHits + PixelDeadSensors
                                                                         nHitSct              = 0,  # SCTHits + SCTDeadSensors
                                                                         nHitSi               = 7,  # PixelHits + SCTHits + PixelDeadSensors + SCTDeadSensors
                                                                         nHitTrt              = 0,  # nTRTHits
                                                                         useSharedHitInfo     = False,
                                                                         nSharedBLayer        = 99999,
                                                                         nSharedPix           = 99999,
                                                                         nSharedSct           = 99999,
                                                                         nSharedSi            = 99999,
                                                                         useTrackQualityInfo  = True,
                                                                         fitChi2OnNdfMax      = 99999,
                                                                         TrackSummaryTool     = None,
                                                                         Extrapolator         = extrapIDSelectorCfg.popPrivateTools())

    CaloExtensionBuilderAlg.TrkSelection = TrackSelectionToolHC
    CaloExtensionBuilderAlg.TrkDetailedSelection = TrackDetailedSelectionToolHC
 
    return CaloExtensionBuilderAlg
