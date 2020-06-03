# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHIHypo.TrigHIHypoConf import UltraCentralHypo
from AthenaCommon.SystemOfUnits import GeV
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
class UltraCentralMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="UltraCentralMonitoring"):
        super(UltraCentralMonitoring, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])
        self.Histograms += [ defineHistogram('FCalTotalEt',
                                             type='TH1F',
                                             title="Total ET in FCal; ET [MeV]",
                                             xbins = 900, xmin=-500000.0, xmax=8500*GeV)]
        self.Histograms += [ defineHistogram('FCalTotalEtPassing',
                                             type='TH1F',
                                             title="Total ET in FCal for passing events; ET [MeV]",
                                             xbins = 900, xmin=-500000.0, xmax=8500*GeV)]


class UltraCentral(UltraCentralHypo):
    __slots__ = []
    def __init__(self, name, minCut, maxCut):
        super( UltraCentralHypo, self ).__init__( name )
        self.FcalEtLowerBound = minCut
        self.FcalEtUpperBound = maxCut
        self.EtaMin = 3.2
        self.isFgap = False
        self.AthenaMonTools += [UltraCentralMonitoring(name="UltraCentralMonitoring")]

class UltraCentral_PT(UltraCentralHypo):
    __slots__ = []
    def __init__(self, name):
        super( UltraCentralHypo, self ).__init__( name )
        self.forceAccept     = True
        self.FcalEtLowerBound = 5000.
        self.EtaMin = 3.2
        self.isFgap = False
        self.AthenaMonTools += [UltraCentralMonitoring(name="UltraCentralMonitoring")]


UCC_th = {"th1": UltraCentral("UCCHypo_th1", 3610*GeV, 4080*GeV), 
          "th2": UltraCentral("UCCHypo_th2", 4080*GeV, 4450*GeV), 
          "th3": UltraCentral("UCCHypo_th3", 4450*GeV, 5200*GeV) }






