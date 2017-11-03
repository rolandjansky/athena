# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigGenericAlgs.TrigGenericAlgsConf import OverlapRemoval, MergeTopoStarts, L1CorrelationAlgo, DetectorTimingAlgo
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class OverlapRemovalConfig(OverlapRemoval):
    def __init__(self, name="OverlapRemovalDefault", **kwargs):
        super(OverlapRemovalConfig , self ).__init__(name)
        self.SuccessfulCombinationsLimit = 1
        for n,v in kwargs.items():
            setattr(self, n, v)
    

class MergeTopoStartsConfig(MergeTopoStarts):
    def __init__(self, name="MergeTopoStartsDefault", **kwargs):
        super(MergeTopoStartsConfig , self ).__init__(name)
        monitoring = TrigGenericMonitoringToolConfig(name+"Monitoring")
        monitoring.Histograms += [ defineHistogram('reductionWrt1', type='TH1F', title="Combinations reduction wrt topo TE 1", xbins=20, xmin=0.0, xmax=1.0) ]
        monitoring.Histograms += [ defineHistogram('reductionWrt2', type='TH1F', title="Combinations reduction wrt topo TE 2", xbins=20, xmin=0.0, xmax=1.0) ]
        monitoring.defineTarget(["Validation", "Online"])
        
        self.AthenaMonTools = [monitoring]
    

class L1CorrelationAlgoConfig(L1CorrelationAlgo):
    def __init__(self, name="L1CorrelationAlgoDefault", **kwargs):
        super(L1CorrelationAlgoConfig , self ).__init__(name)

        from TrigGenericAlgs.TrigGenericAlgsMonitoring import L1CorrelationAlgoMonitoring

        validationMon = L1CorrelationAlgoMonitoring()
        self.AthenaMonTools = [ validationMon ]

class DetectorTimingAlgoConfig(DetectorTimingAlgo):
    def __init__(self, name="DetectorTimingAlgoDefault", **kwargs):
        super(DetectorTimingAlgoConfig , self ).__init__(name)

        from TrigGenericAlgs.TrigGenericAlgsMonitoring import DetectorTimingAlgoMonitoring
        validationMon = DetectorTimingAlgoMonitoring()
        self.AthenaMonTools = [ validationMon ]
