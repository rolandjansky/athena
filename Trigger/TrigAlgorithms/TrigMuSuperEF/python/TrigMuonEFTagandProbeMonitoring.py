# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration                                                       

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigMuonEFTagandProbeMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFTagandProbeMonitoring"):
        super(TrigMuonEFTagandProbeMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation"] ) #Should this be Online rather than validation?

        self.Histograms += [ defineHistogram('EF_FS_InvMass', type='TH1F', title="Full Scan Invariant Mass [GeV]",
                                             xbins=51, xmin=0, xmax=200) ]  # May need to change range when more data included

        self.Histograms += [ defineHistogram('EF_FS_DeltaZ', type='TH1F', title="Full Scan Muons Delta Z [mm]",
                                             xbins=51, xmin=0, xmax=0.5) ]  # May need to change range when more data included

        self.Histograms += [ defineHistogram('LVL1_CB_DeltaR', type='TH1F', title="LVL1 RoI vs FS_CB muon deltaR",
                                             xbins=51, xmin=0, xmax=0.1) ]  # May need to change range when more data included

        self.Histograms += [ defineHistogram('EF_Eta_Tag', type='TH1F', title="Eta distribution of probe muon candidates",
                                             xbins=25, xmin=-5.5, xmax=5.5) ]  

        self.Histograms += [ defineHistogram('EF_Eta_Probe', type='TH1F', title="Eta distribution of sucessful probe muons",
                                             xbins=25, xmin=-5.5, xmax=5.5) ]  

        self.Histograms += [ defineHistogram('EF_Phi_Tag', type='TH1F', title="Phi distribution of probe muon candidates",
                                             xbins=25, xmin=-5.0, xmax=5.0) ]  

        self.Histograms += [ defineHistogram('EF_Phi_Probe', type='TH1F', title="Phi distribution of sucessful probe muons",
                                             xbins=25, xmin=-5.0, xmax=5.0) ]  

        self.Histograms += [ defineHistogram('EF_Pt_Tag', type='TH1F', title="PT distribution of probe muon candidates",
                                             xbins=25, xmin=0, xmax=110) ]  

        self.Histograms += [ defineHistogram('EF_Pt_Probe', type='TH1F', title="PT distribution of sucessful probe muons",
                                             xbins=25, xmin=0, xmax=110) ]  
