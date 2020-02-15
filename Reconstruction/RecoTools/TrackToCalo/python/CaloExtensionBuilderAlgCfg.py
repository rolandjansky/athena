# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory

def getCaloExtenstionBuilderAlgorithm(inputFlags, cutLevel = "TightPrimary", minPT = 100.0):
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg    
    Trk__ParticleCaloExtensionTool = CompFactory.Trk__ParticleCaloExtensionTool
    extrapPFlowCfg = AtlasExtrapolatorCfg(inputFlags)
    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = extrapPFlowCfg.popPrivateTools())
        
    CaloExtensionBuilderAlg = CompFactory.Trk__CaloExtensionBuilderAlg 
    CaloExtensionBuilderAlg = CaloExtensionBuilderAlg(LastCaloExtentionTool = pcExtensionTool)

    InDet__InDetTrackSelectionTool = CompFactory.InDet__InDetTrackSelectionTool    
    TrackSelectionToolHC = InDet__InDetTrackSelectionTool(name = "CaloExtensionBuilderTrackSelectionTool",minPt = minPT, CutLevel = cutLevel, minNSiHits = 7) # SiHits = PixelHits + SCTHits + PixelDeadSensors + SCTDeadSensors    
    CaloExtensionBuilderAlg.TrkSelection = TrackSelectionToolHC
     
    return CaloExtensionBuilderAlg
