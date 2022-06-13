# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaCommon.SystemOfUnits import GeV
def TrigHTTSGInputCfg(flags):
    """
    Configure HTT wrapers generation, outFile will be taken from flags in the future
    """
    acc = ComponentAccumulator()
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    acc.merge(ITkPixelReadoutGeometryCfg(flags))
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    acc.merge(ITkStripReadoutGeometryCfg(flags))
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    acc.addPublicTool(extrapolator)


    HTTSGInputTool = CompFactory.TrigHTTSGToRawHitsTool(maxEta=3.2, minPt=0.8 * GeV,
        Extrapolator = extrapolator )
    acc.addPublicTool(HTTSGInputTool)

    wrapperAlg = CompFactory.TrigHTTRawHitsWrapperAlg(
        InputTool=HTTSGInputTool, OutFileName=flags.Trigger.HTT.wrapperFileName
    )
    acc.addEventAlgo(wrapperAlg)

    return acc
# how to run? See README file