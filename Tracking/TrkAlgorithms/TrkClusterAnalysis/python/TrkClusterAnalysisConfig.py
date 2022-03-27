# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def TrkClusterAnalisysBaseCfg(flags, name, **kwargs):
    result = ComponentAccumulator()
    histsvc = CompFactory.THistSvc(name="THistSvc",
                                   Output=[ f"{name} DATAFILE='{name}.root' OPT='RECREATE'" ])
    result.addService(histsvc)
    return result


def ITkPixelClusterAnalysisCfg(flags, name="ITkPixelClusterAnalysis", **kwargs):
    result = TrkClusterAnalisysBaseCfg(flags, name, **kwargs)

    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    result.merge(ITkPixelReadoutGeometryCfg(flags))

    kwargs.setdefault("NtupleFileName", f"/{name}/")
    kwargs.setdefault("HistPath", f"/{name}/")
    kwargs.setdefault("InputKey", "ITkPixelClusters")

    result.addEventAlgo(CompFactory.PixelClusterAnalysis(name, **kwargs))
    return result


def ITkStripClusterAnalysisCfg(flags, name="ITkStripClusterAnalysis", **kwargs):
    result = TrkClusterAnalisysBaseCfg(flags, name, **kwargs)

    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    result.merge(ITkStripReadoutGeometryCfg(flags))

    kwargs.setdefault("NtupleFileName", f"/{name}/")
    kwargs.setdefault("HistPath", f"/{name}/")
    kwargs.setdefault("InputKey", "ITkStripClusters")

    result.addEventAlgo(CompFactory.SCT_ClusterAnalysis(name, **kwargs))
    return result


def ClusterAnalysisCfg(flags):
    acc = ComponentAccumulator()

    if flags.Detector.EnableITkPixel:
        acc.merge(ITkPixelClusterAnalysisCfg(flags))

    if flags.Detector.EnableITkStrip:
        acc.merge(ITkStripClusterAnalysisCfg(flags))

    return acc
