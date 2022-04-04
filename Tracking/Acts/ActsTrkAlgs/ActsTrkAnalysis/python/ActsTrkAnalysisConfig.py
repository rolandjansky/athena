# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def ActsTrkAnalisysOutputCfg(flags, output_name="ActsTrkAnalysis", **kwargs):
    result = ComponentAccumulator()
    histsvc = CompFactory.THistSvc(name="THistSvc",
                                   Output=[ f"{output_name} DATAFILE='{output_name}.root' OPT='RECREATE'" ])
    result.addService(histsvc)
    return result


def ActsTrkPixelClusterAnalysisCfg(flags, name="ActsTrkPixelClusterAnalysis", **kwargs):
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    result = ITkPixelReadoutGeometryCfg(flags)
    kwargs.setdefault("ClusterContainerKey", "ITkPixelClusters")
    result.addEventAlgo(CompFactory.ActsTrk.PixelClusterAnalysis(name, **kwargs))
    result.merge(ActsTrkAnalisysOutputCfg(flags))
    return result


def ActsTrkStripClusterAnalysisCfg(flags, name="ActsTrkStripClusterAnalysis", **kwargs):
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    result = ITkStripReadoutGeometryCfg(flags)
    kwargs.setdefault("ClusterContainerKey", "ITkStripClusters")
    result.addEventAlgo(CompFactory.ActsTrk.StripClusterAnalysis(name, **kwargs))
    result.merge(ActsTrkAnalisysOutputCfg(flags))
    return result


def ActsTrkPixelSpacePointAnalysisCfg(flags, name="ActsTrkPixelSpacePointAnalysis", **kwargs):
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
    result = ITkPixelReadoutGeometryCfg(flags)

    kwargs.setdefault("HistName", "PixelSpacePoints")
    kwargs.setdefault("NtupleName", "PixelSpacePoints")
    kwargs.setdefault("SpacePointContainerKey", "ITkPixelSpacePoints")
    kwargs.setdefault("PixelClusterContainerKey", "ITkPixelClusters")
    kwargs.setdefault("UsePixel", True)
    kwargs.setdefault("UseOverlap", False)
    result.addEventAlgo(CompFactory.ActsTrk.SpacePointAnalysis(name, **kwargs))

    result.merge(ActsTrkAnalisysOutputCfg(flags))
    return result


def ActsTrkStripSpacePointAnalysisCfg(flags, name="ActsTrkStripSpacePointAnalysis", **kwargs):
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    result = ITkStripReadoutGeometryCfg(flags)

    kwargs.setdefault("HistName", "StripSpacePoints")
    kwargs.setdefault("NtupleName", "StripSpacePoints")
    kwargs.setdefault("SpacePointContainerKey", "ITkStripSpacePoints")
    kwargs.setdefault("StripClusterContainerKey", "ITkStripClusters")
    kwargs.setdefault("UsePixel", False)
    kwargs.setdefault("UseOverlap", False)
    result.addEventAlgo(CompFactory.ActsTrk.SpacePointAnalysis(name, **kwargs))

    result.merge(ActsTrkAnalisysOutputCfg(flags))
    return result


def ActsTrkStripOverlapSpacePointAnalysisCfg(flags, name="ActsTrkStripOverlapSpacePointAnalysis", **kwargs):
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    result = ITkStripReadoutGeometryCfg(flags)

    kwargs.setdefault("HistName", "StripOverlapSpacePoints")
    kwargs.setdefault("NtupleName", "StripOverlapSpacePoints")
    kwargs.setdefault("SpacePointContainerKey", "ITkStripOverlapSpacePoints")
    kwargs.setdefault("StripClusterContainerKey", "ITkStripClusters")
    kwargs.setdefault("UsePixel", False)
    kwargs.setdefault("UseOverlap", True)
    result.addEventAlgo(CompFactory.ActsTrk.SpacePointAnalysis(name, **kwargs))

    result.merge(ActsTrkAnalisysOutputCfg(flags))
    return result


def ActsTrkClusterAnalysisCfg(flags):
    acc = ComponentAccumulator()
    if flags.Detector.EnableITkPixel:
        acc.merge(ActsTrkPixelClusterAnalysisCfg(flags))
    if flags.Detector.EnableITkStrip:
        acc.merge(ActsTrkStripClusterAnalysisCfg(flags))
    return acc


def ActsTrkSpacePointAnalysisCfg(flags):
    acc = ComponentAccumulator()
    if flags.Detector.EnableITkPixel:
        acc.merge(ActsTrkPixelSpacePointAnalysisCfg(flags))
    if flags.Detector.EnableITkStrip:
        acc.merge(ActsTrkStripSpacePointAnalysisCfg(flags))
        acc.merge(ActsTrkStripOverlapSpacePointAnalysisCfg(flags))
    return acc
