# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHIHypo.TrigHIHypoConf import UltraCentralHypo
# a side gaps
from AthenaCommon.SystemOfUnits import GeV
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
class GapHypoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="UltraCentralMonitoring"):
        super(GapHypoMonitoring, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])
        self.Histograms += [ defineHistogram('FCalTotalEt',
                                             type='TH1F',
                                             title="Total ET in FCal; ET [MeV]",
                                             xbins = 50, xmin=-10*GeV, xmax=10*GeV)]
        self.Histograms += [ defineHistogram('FCalTotalEtPassing',
                                             type='TH1F',
                                             title="Total ET in FCal for passing events; ET [MeV]",
                                             xbins = 50, xmin=-10*GeV, xmax=10*GeV)]



ttFgapA = UltraCentralHypo("TTFgapA", EtaMin=3.1, EtaMax=5., FcalEtUpperBound=2*GeV)
ttFgapA.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]
ttFgapC = UltraCentralHypo("TTFgapC", EtaMin=-5, EtaMax=-3.1,  FcalEtUpperBound=2*GeV)
ttFgapC.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]

cellFgapA = UltraCentralHypo("CellsFgapA", EtaMin=3.2, EtaMax=5., FcalEtUpperBound=2*GeV)
cellFgapA.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]
cellFgapC = UltraCentralHypo("CellsFgapC", EtaMin=-5, EtaMax=-3.2,  FcalEtUpperBound=2*GeV)
cellFgapC.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]


cellFgapAPerf = UltraCentralHypo("CellsFgapAPerf", EtaMin=3.2, EtaMax=5., FcalEtUpperBound=3*GeV)
cellFgapAPerf.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]
cellFgapCPerf = UltraCentralHypo("CellsFgapCperf", EtaMin=-5, EtaMax=-3.2,  FcalEtUpperBound=3*GeV)
cellFgapCPerf.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]



