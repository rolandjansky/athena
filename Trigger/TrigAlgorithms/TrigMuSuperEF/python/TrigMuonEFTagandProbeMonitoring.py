# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration                                                       

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigMuonEFTagandProbeMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFTagandProbeMonitoring"):
        super(TrigMuonEFTagandProbeMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation"] ) #Should this be Online rather than validation?

        self.Histograms += [ defineHistogram('EF_FS_InvMass', type='TH1F', title="Full Scan Invariant Mass [GeV]",
                                             xbins=51, xmin=0, xmax=200) ]  # May need to change range when more data included

        self.Histograms += [ defineHistogram('LVL1_CB_DeltaR', type='TH1F', title="LVL1 RoI vs FS_CB muon deltaR",
                                             xbins=26, xmin=0, xmax=5) ]  # May need to change range when more data included
