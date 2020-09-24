# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory

def getCaloExtenstionBuilderAlgorithm(inputFlags):
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg    
    Trk__ParticleCaloExtensionTool = CompFactory.Trk.ParticleCaloExtensionTool
    extrapPFlowCfg = AtlasExtrapolatorCfg(inputFlags)
    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = extrapPFlowCfg.popPrivateTools())
        
    CaloExtensionBuilderAlg = CompFactory.Trk.CaloExtensionBuilderAlg 
    CaloExtensionBuilderAlg = CaloExtensionBuilderAlg(LastCaloExtentionTool = pcExtensionTool)
     
    return CaloExtensionBuilderAlg
