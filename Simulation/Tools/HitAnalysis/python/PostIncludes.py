# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#Adding SiHitValidation for whichever parts of ITk are running
def ITkHitAnalysis(configFlags):
    
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory

    result = ComponentAccumulator()
    
    if configFlags.Detector.EnableITkStrip:
        StripHitAnalysis = CompFactory.SiHitAnalysis('StripHitAnalysis')
        StripHitAnalysis.CollectionName = 'SCT_Hits'
        StripHitAnalysis.HistPath='/SiHitAnalysis/Histos/'
        StripHitAnalysis.NtupleFileName='/SiHitAnalysis/Ntuples/'
        result.addEventAlgo(StripHitAnalysis)

    if configFlags.Detector.EnableITkPixel:
        PixelHitAnalysis = CompFactory.SiHitAnalysis('PixelHitAnalysis')
        PixelHitAnalysis.CollectionName = 'PixelHits'
        PixelHitAnalysis.HistPath='/SiHitAnalysis/Histos/'
        PixelHitAnalysis.NtupleFileName='/SiHitAnalysis/Ntuples/'
        result.addEventAlgo(PixelHitAnalysis)

    THistSvc= CompFactory.THistSvc
    histsvc = THistSvc(name="THistSvc")
    result.addService(histsvc)
    result.getService("THistSvc").Output = [ "SiHitAnalysis DATAFILE='SiHitAnalysis.root' OPT='RECREATE'" ] 

    return result
