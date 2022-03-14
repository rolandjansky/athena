# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def SiHitAnalysisOutputCfg(flags, output_name='SiHitAnalysis'):
    acc = ComponentAccumulator()

    svc = CompFactory.THistSvc(name="THistSvc")
    svc.Output = [f"{output_name} DATAFILE='{output_name}.root' OPT='RECREATE'"]
    acc.addService(svc)

    return acc


def ITkPixelHitAnalysisCfg(flags):
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelSimulationGeometryCfg
    acc = ITkPixelSimulationGeometryCfg(flags)

    alg = CompFactory.SiHitAnalysis('ITkPixelHitAnalysis')
    alg.CollectionName = 'ITkPixelHits'
    alg.HistPath='/SiHitAnalysis/histos/'
    alg.NtuplePath='/SiHitAnalysis/ntuples/'
    acc.addEventAlgo(alg)

    acc.merge(SiHitAnalysisOutputCfg(flags))

    return acc


def ITkStripHitAnalysisCfg(flags):
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripSimulationGeometryCfg
    acc = ITkStripSimulationGeometryCfg(flags)

    alg = CompFactory.SiHitAnalysis('ITkStripHitAnalysis')
    alg.CollectionName = 'ITkStripHits'
    alg.HistPath='/SiHitAnalysis/histos/'
    alg.NtuplePath='/SiHitAnalysis/ntuples/'
    acc.addEventAlgo(alg)

    acc.merge(SiHitAnalysisOutputCfg(flags))

    return acc


def HGTD_HitAnalysisCfg(flags):
    if flags.HGTD.Geometry.useGeoModelXml:
        from HGTD_GeoModelXml.HGTD_GeoModelConfig import HGTD_SimulationGeometryCfg
    else:
        from HGTD_GeoModel.HGTD_GeoModelConfig import HGTD_SimulationGeometryCfg
    acc = HGTD_SimulationGeometryCfg(flags)

    alg = CompFactory.SiHitAnalysis('HGTD_HitAnalysis')
    alg.CollectionName = 'HGTD_Hits'
    alg.HistPath='/SiHitAnalysis/histos/'
    alg.NtuplePath='/SiHitAnalysis/ntuples/'
    acc.addEventAlgo(alg)

    acc.merge(SiHitAnalysisOutputCfg(flags))

    return acc


def PLRHitAnalysisCfg(flags):
    from PLRGeoModelXml.PLRGeoModelConfig import PLRGeometryCfg
    acc = PLRGeometryCfg(flags)

    alg = CompFactory.SiHitAnalysis('PLRHitAnalysis')
    alg.CollectionName = 'PLRHits'
    alg.HistPath='/SiHitAnalysis/histos/'
    alg.NtuplePath='/SiHitAnalysis/ntuples/'
    acc.addEventAlgo(alg)

    acc.merge(SiHitAnalysisOutputCfg(flags))

    return acc


def SiHitAnalysisCfg(flags):
    acc = ComponentAccumulator()

    if flags.Detector.EnableITkPixel:
        acc.merge(ITkPixelHitAnalysisCfg(flags))
    
    if flags.Detector.EnableITkStrip:
        acc.merge(ITkStripHitAnalysisCfg(flags))

    if flags.Detector.EnableHGTD:
        acc.merge(HGTD_HitAnalysisCfg(flags))

    if flags.Detector.EnablePLR:
        acc.merge(PLRHitAnalysisCfg(flags))

    return acc
