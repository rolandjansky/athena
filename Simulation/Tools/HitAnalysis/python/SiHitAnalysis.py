# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def SiHitAnalysisOutputCfg(flags):
    acc = ComponentAccumulator()

    svc = CompFactory.THistSvc(name="THistSvc")
    svc.Output = ["SiHitAnalysis DATAFILE='SiHitAnalysis.root' OPT='RECREATE'"]
    acc.addService(svc)

    return acc


def ITkPixelHitAnalysisCfg(flags):
    from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelGeometryCfg
    acc = ITkPixelGeometryCfg(flags)

    alg = CompFactory.SiHitAnalysis('ITkPixelHitAnalysis')
    alg.CollectionName = 'ITkPixelHits'
    alg.HistPath='/SiHitAnalysis/Histos/'
    alg.NtupleFileName='/SiHitAnalysis/Ntuples/'
    acc.addEventAlgo(alg)

    acc.merge(SiHitAnalysisOutputCfg(flags))

    return acc


def ITkStripHitAnalysisCfg(flags):
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripGeometryCfg
    acc = ITkStripGeometryCfg(flags)

    alg = CompFactory.SiHitAnalysis('ITkStripHitAnalysis')
    alg.CollectionName = 'ITkStripHits'
    alg.HistPath='/SiHitAnalysis/Histos/'
    alg.NtupleFileName='/SiHitAnalysis/Ntuples/'
    acc.addEventAlgo(alg)

    acc.merge(SiHitAnalysisOutputCfg(flags))

    return acc


def SiHitAnalysisCfg(flags):
    acc = ComponentAccumulator()

    if flags.Detector.EnableITkPixel:
        acc.merge(ITkPixelHitAnalysisCfg(flags))
    
    if flags.Detector.EnableITkStrip:
        acc.merge(ITkStripHitAnalysisCfg(flags))

    return acc
