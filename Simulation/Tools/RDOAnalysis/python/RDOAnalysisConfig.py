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

    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    result.merge(ITkPixelReadoutGeometryCfg(flags))

    kwargs.setdefault("NtupleFileName", f"/{name}/")
    kwargs.setdefault("HistPath", f"/{name}/")

    result.addEventAlgo(CompFactory.ITk.PixelRDOAnalysis(name, **kwargs))
    return result


def ITkStripRDOAnalysisCfg(flags, name="ITkStripRDOAnalysis", **kwargs):
    result = RDOAnalysisBaseCfg(flags, name, **kwargs)

    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    result.merge(ITkStripReadoutGeometryCfg(flags))

    kwargs.setdefault("NtupleFileName", f"/{name}/")
    kwargs.setdefault("HistPath", f"/{name}/")

    result.addEventAlgo(CompFactory.ITk.StripRDOAnalysis(name, **kwargs))
    return result


def RDOAnalysisCfg(flags):
    acc = ComponentAccumulator()

    if flags.Detector.EnableITkPixel:
        acc.merge(ITkPixelRDOAnalysisCfg(flags))
    
    if flags.Detector.EnableITkStrip:
        acc.merge(ITkStripRDOAnalysisCfg(flags))

    return acc
