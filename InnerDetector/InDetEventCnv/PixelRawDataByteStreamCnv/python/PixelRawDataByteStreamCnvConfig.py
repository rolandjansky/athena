#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from PixelConditionsAlgorithms.PixelConditionsConfig import PixelCablingCondAlgCfg, PixelHitDiscCnfgAlgCfg

def PixelRawDataProviderAlgCfg(flags, RDOKey="PixelRDOs"):
    """ Main function to configure Pixel raw data decoding """
    acc = PixelCablingCondAlgCfg(flags)
    acc.merge(PixelHitDiscCnfgAlgCfg(flags))

    from RegionSelector.RegSelToolConfig import regSelTool_Pixel_Cfg
    regSelTool = acc.popToolsAndMerge(regSelTool_Pixel_Cfg(flags))
    decoder = CompFactory.PixelRodDecoder()
    providerTool =  CompFactory.PixelRawDataProviderTool(Decoder = decoder)

    acc.addEventAlgo(CompFactory.PixelRawDataProvider(RDOKey = RDOKey,
                                                      RegSelTool = regSelTool, 
                                                      ProviderTool = providerTool))

    return acc
