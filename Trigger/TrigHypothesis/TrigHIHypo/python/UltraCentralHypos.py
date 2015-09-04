# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHIHypo.TrigHIHypoConf import UltraCentralHypo

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
class UltraCentralMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="UltraCentralMonitoring"):
        super(UltraCentralMonitoring, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])
        self.Histograms += [ defineHistogram('FCalTotalEt',
                                             type='TH1F',
                                             title="Total ET in FCal; ET [MeV]",
                                             xbins = 100, xmin=0.0, xmax=4500000)]
        self.Histograms += [ defineHistogram('FCalTotalEtPassing',
                                             type='TH1F',
                                             title="Total ET in FCal for passing events; ET [MeV]",
                                             xbins = 100, xmin=0.0, xmax=4500000)]


class UltraCentral(UltraCentralHypo):
    __slots__ = []
    def __init__(self, name, min_et, max_et):
        super( UltraCentralHypo, self ).__init__( name )
        self.FcalEtLowerBound = min_et
        self.FcalEtUpperBound = max_et
        self.AthenaMonTools += [UltraCentralMonitoring(name="UltraCentralMonitoring")]

class UltraCentral_PT(UltraCentralHypo):
    __slots__ = []
    def __init__(self, name):
        super( UltraCentralHypo, self ).__init__( name )
        self.forceAccept     = True
        self.FcalEtLowerBound = 0.001
        self.AthenaMonTools += [UltraCentralMonitoring(name="UltraCentralMonitoring")]

