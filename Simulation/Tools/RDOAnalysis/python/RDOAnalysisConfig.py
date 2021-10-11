# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def RDOAnalysisBaseCfg(flags, name, **kwargs):
    result = ComponentAccumulator()

    histsvc = CompFactory.THistSvc(name="THistSvc",
                                   Output=[ f"{name} DATAFILE='{name}.root' OPT='RECREATE'" ])
    result.addService(histsvc)
    
    return result


def ITkPixelRDOAnalysisCfg(flags, name="ITkPixelRDOAnalysis", **kwargs):
    result = RDOAnalysisBaseCfg(flags, name, **kwargs)

    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelGeometryCfg
    result.merge(ITkPixelGeometryCfg(flags))

    kwargs.setdefault("NtupleFileName", f"/{name}/")
    kwargs.setdefault("HistPath", f"/{name}/")

    result.addEventAlgo(CompFactory.ITkPixelRDOAnalysis(name, **kwargs))
    return result


def ITkStripRDOAnalysisCfg(flags, name="ITkStripRDOAnalysis", **kwargs):
    result = RDOAnalysisBaseCfg(flags, name, **kwargs)

    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripGeometryCfg
    result.merge(ITkStripGeometryCfg(flags))

    kwargs.setdefault("NtupleFileName", f"/{name}/")
    kwargs.setdefault("HistPath", f"/{name}/")

    result.addEventAlgo(CompFactory.StripRDOAnalysis(name, **kwargs))
    return result


def RDOAnalysisCfg(flags):
    acc = ComponentAccumulator()

    if flags.Detector.EnableITkPixel:
        acc.merge(ITkPixelRDOAnalysisCfg(flags))
    
    if flags.Detector.EnableITkStrip:
        acc.merge(ITkStripRDOAnalysisCfg(flags))

    return acc
