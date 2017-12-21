# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration                                                       

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigMuonEFTagandProbeMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFTagandProbeMonitoring"):
        super(TrigMuonEFTagandProbeMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation"] ) #Should this be Online rather than validation?

        self.Histograms += [ defineHistogram('EF_FS_InvMass', type='TH1F', title="Full Scan Invariant Mass [GeV]",
                                             xbins=51, xmin=0, xmax=140) ]  # May need to change range when more data included
        
        self.Histograms += [ defineHistogram('EF_FS_DeltaZ', type='TH1F', title="Full Scan Muons Delta Z [mm]",
                                             xbins=51, xmin=0, xmax=0.5) ]  # May need to change range when more data included
        
        #        self.Histograms += [ defineHistogram('LVL1_EF_DeltaR_ALL', type='TH1F', title="LVL1 RoI vs EF_CB muon deltaR",
        #                                             xbins=51, xmin=0, xmax=0.1) ]  # May need to change range when more data included
        
        self.Histograms += [ defineHistogram('LVL1_EF_DeltaR_TAG', type='TH1F', title="LVL1 RoI vs EF_CB tag_muon deltaR",
                                             xbins=51, xmin=0, xmax=0.1) ]  # May need to change range when more data included
        
        self.Histograms += [ defineHistogram('LVL1_EF_DeltaR_PROBE', type='TH1F', title="LVL1 RoI vs EF_CB Passing_Probe_muon deltaR",
                                             xbins=51, xmin=0, xmax=0.1) ]  # May need to change range when more data included
        
        self.Histograms += [ defineHistogram('LVL1_EF_DeltaR_Thresh1', type='TH1F', title="LVL1 RoI vs EF_CB Threshold 1 Probe deltaR",
                                             xbins=51, xmin=0, xmax=0.1) ]  # May need to change range when more data included
        self.Histograms += [ defineHistogram('LVL1_EF_DeltaR_Thresh2', type='TH1F', title="LVL1 RoI vs EF_CB Threshold 2 Probe deltaR",
                                             xbins=51, xmin=0, xmax=0.1) ]  # May need to change range when more data included
        self.Histograms += [ defineHistogram('LVL1_EF_DeltaR_Thresh3', type='TH1F', title="LVL1 RoI vs EF_CB Threshold 3 Probe deltaR",
                                             xbins=51, xmin=0, xmax=0.1) ]  # May need to change range when more data included
        self.Histograms += [ defineHistogram('LVL1_EF_DeltaR_Thresh4', type='TH1F', title="LVL1 RoI vs EF_CB Threshold 4 Probe deltaR",
                                             xbins=51, xmin=0, xmax=0.1) ]  # May need to change range when more data included
        self.Histograms += [ defineHistogram('LVL1_EF_DeltaR_Thresh5', type='TH1F', title="LVL1 RoI vs EF_CB Threshold 5 Probe deltaR",
                                             xbins=51, xmin=0, xmax=0.1) ]  # May need to change range when more data included
        self.Histograms += [ defineHistogram('LVL1_EF_DeltaR_Thresh6', type='TH1F', title="LVL1 RoI vs EF_CB Threshold 6 Probe deltaR",
                                             xbins=51, xmin=0, xmax=0.1) ]  # May need to change range when more data included

        
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr1_b', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 1 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr1_b', type='TH1F', title="Eta distribution of passing probe muons Threshold 1 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Total_thr1_b', type='TH1F', title="Phi distribution of all probe muon candidates Threshold 1 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Passing_thr1_b', type='TH1F', title="Phi distribution of passing probe muons Threshold 1 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Total_thr1_b', type='TH1F', title="Pt distribution of all probe muon candidates Threshold 1 Barrel",
                                             xbins=55, xmin=0, xmax=110) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Passing_thr1_b', type='TH1F', title="Pt distribution of passing probe muons Threshold 1 Barrel",
                                             xbins=55, xmin=0, xmax=110) ]  
        
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr1_e', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 1 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr1_e', type='TH1F', title="Eta distribution of passing probe muons Threshold 1 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Total_thr1_e', type='TH1F', title="Phi distribution of all probe muon candidates Threshold 1 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Passing_thr1_e', type='TH1F', title="Phi distribution of passing probe muons Threshold 1 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Total_thr1_e', type='TH1F', title="Pt distribution of all probe muon candidates Threshold 1 Endcap",
                                             xbins=55, xmin=0, xmax=110) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Passing_thr1_e', type='TH1F', title="Pt distribution of passing probe muons Threshold 1 Endcap",
                                             xbins=55, xmin=0, xmax=110) ]  
        

        self.Histograms += [ defineHistogram('EF_Eta_Total_thr1', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 1",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr1', type='TH1F', title="Eta distribution of passing probe muons Threshold 1",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]          


        
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr2_b', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 2 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr2_b', type='TH1F', title="Eta distribution of passing probe muons Threshold 2 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Total_thr2_b', type='TH1F', title="Phi distribution of all probe muon candidates Threshold 2 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Passing_thr2_b', type='TH1F', title="Phi distribution of passing probe muons Threshold 2 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Total_thr2_b', type='TH1F', title="Pt distribution of all probe muon candidates Threshold 2 Barrel",
                                             xbins=55, xmin=0, xmax=110) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Passing_thr2_b', type='TH1F', title="Pt distribution of passing probe muons Threshold 2 Barrel",
                                             xbins=55, xmin=0, xmax=110) ]  
        
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr2_e', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 2 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr2_e', type='TH1F', title="Eta distribution of passing probe muons Threshold 2 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Total_thr2_e', type='TH1F', title="Phi distribution of all probe muon candidates Threshold 2 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Passing_thr2_e', type='TH1F', title="Phi distribution of passing probe muons Threshold 2 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Total_thr2_e', type='TH1F', title="Pt distribution of all probe muon candidates Threshold 2 Endcap",
                                             xbins=55, xmin=0, xmax=110) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Passing_thr2_e', type='TH1F', title="Pt distribution of passing probe muons Threshold 2 Endcap",
                                             xbins=55, xmin=0, xmax=110) ]  
        
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr2', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 2",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr2', type='TH1F', title="Eta distribution of passing probe muons Threshold 2",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]          


        
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr3_b', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 3",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr3_b', type='TH1F', title="Eta distribution of passing probe muons Threshold 3",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Total_thr3_b', type='TH1F', title="Phi distribution of all probe muon candidates Threshold 3 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Passing_thr3_b', type='TH1F', title="Phi distribution of passing probe muons Threshold 3 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Total_thr3_b', type='TH1F', title="Pt distribution of all probe muon candidates Threshold 3 Barrel",
                                             xbins=55, xmin=0, xmax=110) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Passing_thr3_b', type='TH1F', title="Pt distribution of passing probe muons Threshold 3 Barrel",
                                             xbins=55, xmin=0, xmax=110) ]  
        
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr3_e', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 3 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr3_e', type='TH1F', title="Eta distribution of passing probe muons Threshold 3 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Total_thr3_e', type='TH1F', title="Phi distribution of all probe muon candidates Threshold 3 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Passing_thr3_e', type='TH1F', title="Phi distribution of passing probe muons Threshold 3 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Total_thr3_e', type='TH1F', title="Pt distribution of all probe muon candidates Threshold 3 Endcap",
                                             xbins=55, xmin=0, xmax=110) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Passing_thr3_e', type='TH1F', title="Pt distribution of passing probe muons Threshold 3 Endcap",
                                             xbins=55, xmin=0, xmax=110) ]  

        self.Histograms += [ defineHistogram('EF_Eta_Total_thr3', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 3",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr3', type='TH1F', title="Eta distribution of passing probe muons Threshold 3",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        
        
        
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr4_b', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 4 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr4_b', type='TH1F', title="Eta distribution of passing probe muons Threshold 4 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Total_thr4_b', type='TH1F', title="Phi distribution of all probe muon candidates Threshold 4 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Passing_thr4_b', type='TH1F', title="Phi distribution of passing probe muons Threshold 4 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Total_thr4_b', type='TH1F', title="Pt distribution of all probe muon candidates Threshold 4 Barrel",
                                             xbins=55, xmin=0, xmax=110) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Passing_thr4_b', type='TH1F', title="Pt distribution of passing probe muons Threshold 4 Barrel",
                                             xbins=55, xmin=0, xmax=110) ]  
        
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr4_e', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 4 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr4_e', type='TH1F', title="Eta distribution of passing probe muons Threshold 4 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Total_thr4_e', type='TH1F', title="Phi distribution of all probe muon candidates Threshold 4 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Passing_thr4_e', type='TH1F', title="Phi distribution of passing probe muons Threshold 4 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Total_thr4_e', type='TH1F', title="Pt distribution of all probe muon candidates Threshold 4 Endcap",
                                             xbins=55, xmin=0, xmax=110) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Passing_thr4_e', type='TH1F', title="Pt distribution of passing probe muons Threshold 4 Endcap",
                                             xbins=55, xmin=0, xmax=110) ]  

        self.Histograms += [ defineHistogram('EF_Eta_Total_thr4', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 4",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr4', type='TH1F', title="Eta distribution of passing probe muons Threshold 4",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  



        self.Histograms += [ defineHistogram('EF_Eta_Total_thr5_b', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 5 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr5_b', type='TH1F', title="Eta distribution of passing probe muons Threshold 5 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Total_thr5_b', type='TH1F', title="Phi distribution of all probe muon candidates Threshold 5 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Passing_thr5_b', type='TH1F', title="Phi distribution of passing probe muons Threshold 5 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Total_thr5_b', type='TH1F', title="Pt distribution of all probe muon candidates Threshold 5 Barrel",
                                             xbins=55, xmin=0, xmax=110) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Passing_thr5_b', type='TH1F', title="Pt distribution of passing probe muons Threshold 5 Barrel",
                                             xbins=55, xmin=0, xmax=110) ]  

        self.Histograms += [ defineHistogram('EF_Eta_Total_thr5_e', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 5 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr5_e', type='TH1F', title="Eta distribution of passing probe muons Threshold 5 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Total_thr5_e', type='TH1F', title="Phi distribution of all probe muon candidates Threshold 5 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Passing_thr5_e', type='TH1F', title="Phi distribution of passing probe muons Threshold 5 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Total_thr5_e', type='TH1F', title="Pt distribution of all probe muon candidates Threshold 5 Endcap",
                                             xbins=55, xmin=0, xmax=110) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Passing_thr5_e', type='TH1F', title="Pt distribution of passing probe muons Threshold 5 Endcap",
                                             xbins=55, xmin=0, xmax=110) ]  

        self.Histograms += [ defineHistogram('EF_Eta_Total_thr5', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 5",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr5', type='TH1F', title="Eta distribution of passing probe muons Threshold 5",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  



        self.Histograms += [ defineHistogram('EF_Eta_Total_thr6_b', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 6 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr6_b', type='TH1F', title="Eta distribution of passing probe muons Threshold 6 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Total_thr6_b', type='TH1F', title="Phi distribution of all probe muon candidates Threshold 6 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Passing_thr6_b', type='TH1F', title="Phi distribution of passing probe muons Threshold 6 Barrel",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Total_thr6_b', type='TH1F', title="Pt distribution of all probe muon candidates Threshold 6 Barrel",
                                             xbins=55, xmin=0, xmax=110) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Passing_thr6_b', type='TH1F', title="Pt distribution of passing probe muons Threshold 6 Barrel",
                                             xbins=55, xmin=0, xmax=110) ]  

        self.Histograms += [ defineHistogram('EF_Eta_Total_thr6_e', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 6 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr6_e', type='TH1F', title="Eta distribution of passing probe muons Threshold 6 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Total_thr6_e', type='TH1F', title="Phi distribution of all probe muon candidates Threshold 6 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Phi_Passing_thr6_e', type='TH1F', title="Phi distribution of passing probe muons Threshold 6 Endcap",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Total_thr6_e', type='TH1F', title="Pt distribution of all probe muon candidates Threshold 6 Endcap",
                                             xbins=55, xmin=0, xmax=110) ]  
        self.Histograms += [ defineHistogram('EF_Pt_Passing_thr6_e', type='TH1F', title="Pt distribution of passing probe muons Threshold 6 Endcap",
                                             xbins=55, xmin=0, xmax=110) ]  

        self.Histograms += [ defineHistogram('EF_Eta_Total_thr6', type='TH1F', title="Eta distribution of all probe muon candidates Threshold 6",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr6', type='TH1F', title="Eta distribution of passing probe muons Threshold 6",
                                             xbins=35, xmin=-3.5, xmax=3.5) ]  



        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr1_b,EF_Phi_Passing_thr1_b', type='TH2F', title="Eta v Phi distribution of passing probe muons Threshold 1 Barrel;Muon #eta;Muon #phi",
                                             xbins=11, xmin=-1.1, xmax=1.1, ybins=8, ymin=-3.4, ymax=3.4,) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr1_b,EF_Phi_Total_thr1_b', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 1 Barrel;Muon #eta;Muon #phi",
                                             xbins=11, xmin=-1.1, xmax=1.1, ybins=8, ymin=-3.4, ymax=3.4,) ]  

        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr1_e,EF_Phi_Passing_thr1_e', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 1 Endcap;Muon #eta;Muon #phi",
                                             xbins=27, xmin=-2.7, xmax=2.7, ybins=8, ymin=-3.4, ymax=3.4,) ]
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr1_e,EF_Phi_Total_thr1_e', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 1 Endcap;Muon #eta;Muon #phi",
                                             xbins=27, xmin=-2.7, xmax=2.7, ybins=8, ymin=-3.4, ymax=3.4,) ]


        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr2_b,EF_Phi_Passing_thr2_b', type='TH2F', title="Eta v Phi distribution of passing probe muons Threshold 2 Barrel;Muon #eta;Muon #phi",
                                             xbins=11, xmin=-1.1, xmax=1.1, ybins=8, ymin=-3.4, ymax=3.4,) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr2_b,EF_Phi_Total_thr2_b', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 2 Barrel;Muon #eta;Muon #phi",
                                             xbins=11, xmin=-1.1, xmax=1.1, ybins=8, ymin=-3.4, ymax=3.4,) ]  

        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr2_e,EF_Phi_Passing_thr2_e', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 2 Endcap;Muon #eta;Muon #phi",
                                             xbins=27, xmin=-2.7, xmax=2.7, ybins=8, ymin=-3.4, ymax=3.4,) ]
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr2_e,EF_Phi_Total_thr2_e', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 2 Endcap;Muon #eta;Muon #phi",
                                             xbins=27, xmin=-2.7, xmax=2.7, ybins=8, ymin=-3.4, ymax=3.4,) ]


        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr3_b,EF_Phi_Passing_thr3_b', type='TH2F', title="Eta v Phi distribution of passing probe muons Threshold 3 Barrel;Muon #eta;Muon #phi",
                                             xbins=11, xmin=-1.1, xmax=1.1, ybins=8, ymin=-3.4, ymax=3.4,) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr3_b,EF_Phi_Total_thr3_b', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 3 Barrel;Muon #eta;Muon #phi",
                                             xbins=11, xmin=-1.1, xmax=1.1, ybins=8, ymin=-3.4, ymax=3.4,) ]  

        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr3_e,EF_Phi_Passing_thr3_e', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 3 Endcap;Muon #eta;Muon #phi",
                                             xbins=27, xmin=-2.7, xmax=2.7, ybins=8, ymin=-3.4, ymax=3.4,) ]
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr3_e,EF_Phi_Total_thr3_e', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 3 Endcap;Muon #eta;Muon #phi",
                                             xbins=27, xmin=-2.7, xmax=2.7, ybins=8, ymin=-3.4, ymax=3.4,) ]


        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr4_b,EF_Phi_Passing_thr4_b', type='TH2F', title="Eta v Phi distribution of passing probe muons Threshold 4 Barrel;Muon #eta;Muon #phi",
                                             xbins=11, xmin=-1.1, xmax=1.1, ybins=8, ymin=-3.4, ymax=3.4,) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr4_b,EF_Phi_Total_thr4_b', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 4 Barrel;Muon #eta;Muon #phi",
                                             xbins=11, xmin=-1.1, xmax=1.1, ybins=8, ymin=-3.4, ymax=3.4,) ]  

        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr4_e,EF_Phi_Passing_thr4_e', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 4 Endcap;Muon #eta;Muon #phi",
                                             xbins=27, xmin=-2.7, xmax=2.7, ybins=8, ymin=-3.4, ymax=3.4,) ]
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr4_e,EF_Phi_Total_thr4_e', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 4 Endcap;Muon #eta;Muon #phi",
                                             xbins=27, xmin=-2.7, xmax=2.7, ybins=8, ymin=-3.4, ymax=3.4,) ]


        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr5_b,EF_Phi_Passing_thr5_b', type='TH2F', title="Eta v Phi distribution of passing probe muons Threshold 5 Barrel;Muon #eta;Muon #phi",
                                             xbins=11, xmin=-1.1, xmax=1.1, ybins=8, ymin=-3.4, ymax=3.4,) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr5_b,EF_Phi_Total_thr5_b', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 5 Barrel;Muon #eta;Muon #phi",
                                             xbins=11, xmin=-1.1, xmax=1.1, ybins=8, ymin=-3.4, ymax=3.4,) ]  

        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr5_e,EF_Phi_Passing_thr5_e', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 5 Endcap;Muon #eta;Muon #phi",
                                             xbins=27, xmin=-2.7, xmax=2.7, ybins=8, ymin=-3.4, ymax=3.4,) ]
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr5_e,EF_Phi_Total_thr5_e', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 5 Endcap;Muon #eta;Muon #phi",
                                             xbins=27, xmin=-2.7, xmax=2.7, ybins=8, ymin=-3.4, ymax=3.4,) ]


        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr6_b,EF_Phi_Passing_thr6_b', type='TH2F', title="Eta v Phi distribution of passing probe muons Threshold 6 Barrel;Muon #eta;Muon #phi",
                                             xbins=11, xmin=-1.1, xmax=1.1, ybins=8, ymin=-3.4, ymax=3.4,) ]  
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr6_b,EF_Phi_Total_thr6_b', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 6 Barrel;Muon #eta;Muon #phi",
                                             xbins=11, xmin=-1.1, xmax=1.1, ybins=8, ymin=-3.4, ymax=3.4,) ]  

        self.Histograms += [ defineHistogram('EF_Eta_Passing_thr6_e,EF_Phi_Passing_thr6_e', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 6 Endcap;Muon #eta;Muon #phi",
                                             xbins=27, xmin=-2.7, xmax=2.7, ybins=8, ymin=-3.4, ymax=3.4,) ]
        self.Histograms += [ defineHistogram('EF_Eta_Total_thr6_e,EF_Phi_Total_thr6_e', type='TH2F', title="Eta v Phi distribution of total probe muons Threshold 6 Endcap;Muon #eta;Muon #phi",
                                             xbins=27, xmin=-2.7, xmax=2.7, ybins=8, ymin=-3.4, ymax=3.4,) ]

  






