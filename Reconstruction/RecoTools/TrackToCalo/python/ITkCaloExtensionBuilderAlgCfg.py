# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def ITkCaloExtensionBuilderAlgCfg(inputFlags):
    #Run-3 uses
    #from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    from InDetConfig.ITkRecToolConfig import ITkExtrapolatorCfg
    Trk__ParticleCaloExtensionTool = CompFactory.Trk.ParticleCaloExtensionTool
    result = ComponentAccumulator()
    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = result.getPrimaryAndMerge(ITkExtrapolatorCfg(inputFlags)))
        
    CaloExtensionBuilderAlg = CompFactory.Trk.CaloExtensionBuilderAlg 
    CaloExtensionBuilderAlg = CaloExtensionBuilderAlg(LastCaloExtentionTool = pcExtensionTool,
                                                      PixelDetEleCollKey="",
                                                      SCTDetEleCollKey="",
                                                      TRTDetEleContKey="")
    result.addEventAlgo (CaloExtensionBuilderAlg)
     
    return result
