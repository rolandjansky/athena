# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def TrkSpacePointAnalisysBaseCfg(flags, name, **kwargs):
    result = ComponentAccumulator()
    histsvc = CompFactory.THistSvc(name="THistSvc",
                                   Output=[ f"{name} DATAFILE='{name}.root' OPT='RECREATE'" ])
    result.addService(histsvc)
    return result


def ITkPixelSpacePointAnalysisCfg(flags, name="ITkPixelSpacePointAnalysis", **kwargs):
    result = TrkSpacePointAnalisysBaseCfg(flags, name, **kwargs)

    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    result.merge(ITkPixelReadoutGeometryCfg(flags))

    kwargs.setdefault("NtupleFileName", f"/{name}/")
    kwargs.setdefault("HistPath", f"/{name}/")
    kwargs.setdefault("InputKey", "ITkPixelSpacePoints")
    kwargs.setdefault("UsePixel", True)

    result.addEventAlgo(CompFactory.SpacePointAnalysis(name, **kwargs))
    return result


def ITkStripSpacePointAnalysisCfg(flags, name="ITkStripSpacePointAnalysis", **kwargs):
    result = TrkSpacePointAnalisysBaseCfg(flags, name, **kwargs)

    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    result.merge(ITkStripReadoutGeometryCfg(flags))

    kwargs.setdefault("NtupleFileName", f"/{name}/")
    kwargs.setdefault("HistPath", f"/{name}/")
    kwargs.setdefault("InputKey", "ITkStripSpacePoints")
    kwargs.setdefault("UsePixel", False)

    result.addEventAlgo(CompFactory.SpacePointAnalysis(name, **kwargs))
    return result


def SpacePointAnalysisCfg(flags):
    acc = ComponentAccumulator()

    if flags.Detector.EnableITkPixel:
        acc.merge(ITkPixelSpacePointAnalysisCfg(flags))

    if flags.Detector.EnableITkStrip:
        acc.merge(ITkStripSpacePointAnalysisCfg(flags))

    return acc
