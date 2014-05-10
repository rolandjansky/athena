# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigGenericAlgs.TrigGenericAlgsConf import OverlapRemoval
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig




class OverlapRemovalOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="OverlapRemovalOnlineMonitoring"):
        super(OverlapRemovalOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('DeltaEta', type='TH1F', title="ROI overlap removal. Delta #eta; #Delta#eta; nevents", xbins=50, xmin=0.0, xmax=5.0) ]
        self.Histograms += [ defineHistogram('DeltaPhi', type='TH1F', title="ROI overlap removal. Delta #varphi.; #Delta#varphi; nevents", xbins=63, xmin=-3.15, xmax=3.15) ]
        self.Histograms += [ defineHistogram('DeltaRSq', type='TH1F', title="ROI overlap removal. Delta R^{2};#Delta #varphi^{2} + #Delta #eta^{2}; nevents", xbins=260, xmin=0, xmax=26) ]


########################################################

class OverlapRemovalValidationMonitoring(OverlapRemovalOnlineMonitoring):
    def __init__ (self, name="OverlapRemovalValidationMonitoring"):
        super(OverlapRemovalValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

########################################################
########################################################

class OverlapRemovalMonitored (OverlapRemoval) :
    def __init__(self, name = "OverlapRemovalMonitored"):
        super(OverlapRemovalMonitored , self ).__init__(name)

        self.SuccessfulCombinationsLimit = 1
        self.MinCentDist = 0.2

        online     = OverlapRemovalOnlineMonitoring()
        validation     = OverlapRemovalValidationMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, online, validation ]




