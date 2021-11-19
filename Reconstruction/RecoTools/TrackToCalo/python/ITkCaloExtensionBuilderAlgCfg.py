# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory

def ITkCaloExtensionBuilderAlgCfg(inputFlags):
    from TrkConfig.AtlasUpgradeExtrapolatorConfig import AtlasUpgradeExtrapolatorCfg
    Trk__ParticleCaloExtensionTool = CompFactory.Trk.ParticleCaloExtensionTool
    result = AtlasUpgradeExtrapolatorCfg(inputFlags)
    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = result.popPrivateTools())
        
    CaloExtensionBuilderAlg = CompFactory.Trk.CaloExtensionBuilderAlg 
    CaloExtensionBuilderAlg = CaloExtensionBuilderAlg(LastCaloExtentionTool = pcExtensionTool,
                                                      PixelDetEleCollKey="",
                                                      SCTDetEleCollKey="",
                                                      TRTDetEleContKey="")
    result.addEventAlgo (CaloExtensionBuilderAlg)
     
    return result
