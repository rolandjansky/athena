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

ttFgapA3 = UltraCentralHypo("TTFgapA3", EtaMin=3.1, EtaMax=5., FcalEtUpperBound=3*GeV)
ttFgapA3.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]
ttFgapC3 = UltraCentralHypo("TTFgapC3", EtaMin=-5, EtaMax=-3.1,  FcalEtUpperBound=3*GeV)
ttFgapC3.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]

cellFgapA3 = UltraCentralHypo("CellsFgapA3", EtaMin=3.2, EtaMax=5., FcalEtUpperBound=3*GeV)
cellFgapA3.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]
cellFgapC3 = UltraCentralHypo("CellsFgapC3", EtaMin=-5, EtaMax=-3.2,  FcalEtUpperBound=3*GeV)
cellFgapC3.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]

ttFgapA5 = UltraCentralHypo("TTFgapA5", EtaMin=3.1, EtaMax=5., FcalEtUpperBound=5*GeV)
ttFgapA5.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]
ttFgapC5 = UltraCentralHypo("TTFgapC5", EtaMin=-5, EtaMax=-3.1,  FcalEtUpperBound=5*GeV)
ttFgapC5.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]

cellFgapA5 = UltraCentralHypo("CellsFgapA5", EtaMin=3.2, EtaMax=5., FcalEtUpperBound=5*GeV)
cellFgapA5.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]
cellFgapC5 = UltraCentralHypo("CellsFgapC5", EtaMin=-5, EtaMax=-3.2,  FcalEtUpperBound=5*GeV)
cellFgapC5.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]

ttFgapA10 = UltraCentralHypo("TTFgapA10", EtaMin=3.1, EtaMax=5., FcalEtUpperBound=10*GeV)
ttFgapA10.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]
ttFgapC10 = UltraCentralHypo("TTFgapC10", EtaMin=-5, EtaMax=-3.1,  FcalEtUpperBound=10*GeV)
ttFgapC10.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]

cellFgapA10 = UltraCentralHypo("CellsFgapA10", EtaMin=3.2, EtaMax=5., FcalEtUpperBound=10*GeV)
cellFgapA10.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]
cellFgapC10 = UltraCentralHypo("CellsFgapC10", EtaMin=-5, EtaMax=-3.2,  FcalEtUpperBound=10*GeV)
cellFgapC10.AthenaMonTools += [ GapHypoMonitoring(name="GapHypoMonitoring")]

