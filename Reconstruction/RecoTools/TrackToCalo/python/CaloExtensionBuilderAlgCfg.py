# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory

def CaloExtensionBuilderAlgCfg(inputFlags):
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg    
    Trk__ParticleCaloExtensionTool = CompFactory.Trk.ParticleCaloExtensionTool
    result = AtlasExtrapolatorCfg(inputFlags)
    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = result.popPrivateTools())
        
    CaloExtensionBuilderAlg = CompFactory.Trk.CaloExtensionBuilderAlg 
    CaloExtensionBuilderAlg = CaloExtensionBuilderAlg(LastCaloExtentionTool = pcExtensionTool)
    result.addEventAlgo (CaloExtensionBuilderAlg)
     
    return result
