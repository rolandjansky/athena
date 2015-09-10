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
                                             xbins = 900, xmin=-500000.0, xmax=8500000)]
        self.Histograms += [ defineHistogram('FCalTotalEtPassing',
                                             type='TH1F',
                                             title="Total ET in FCal for passing events; ET [MeV]",
                                             xbins = 900, xmin=-500000.0, xmax=8500000)]


_EtCutsMin=[0.001, 4700.0, 4800.0, 4900.0, 5000.0, 5100.0, 
	    5200.0, 5300.0, 5400.0, 5500.0, 5600.0,
            5700.0, 5800.0, 5900.0, 6000.0, 6100.0,
            6200.0, 6300.0, 6400.0, 6500.0, 6600.0] 

_EtCutsMax=[10000.0] 


class UltraCentral(UltraCentralHypo):
    __slots__ = []
    def __init__(self, name):
        super( UltraCentralHypo, self ).__init__( name )
        self.FcalEtLowerBound = _EtCutsMin
        self.FcalEtUpperBound = _EtCutsMax
        self.LowerBound_index = 1
        self.UpperBound_index = 0 
        self.AthenaMonTools += [UltraCentralMonitoring(name="UltraCentralMonitoring")]

class UltraCentral_PT(UltraCentralHypo):
    __slots__ = []
    def __init__(self, name):
        super( UltraCentralHypo, self ).__init__( name )
        self.forceAccept     = True
        self.FcalEtLowerBound = _EtCutsMin
        self.LowerBound_index = 0
        self.AthenaMonTools += [UltraCentralMonitoring(name="UltraCentralMonitoring")]

