# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigEFDPhiMetJetAllTEValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigEFDPhiMetJetAllTEHypoValidationMonitoring"):
        super(TrigEFDPhiMetJetAllTEValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms =  [ defineHistogram('NJets',       type='TH1F', title="NJets No cut", xbins=10, xmin=0, xmax=10) ]
        self.Histograms +=  [ defineHistogram('jet1et',      type='TH1F', title="EFCentralJet1 Et  No cut", xbins=50, xmin=0, xmax=500000) ]
        self.Histograms += [ defineHistogram('jet1phi',     type='TH1F', title="EFCentralJet1 Phi No cut", xbins=50, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('jet1eta',     type='TH1F', title="EFCentralJet1 Eta No cut", xbins=25, xmin=0, xmax=4) ]
        self.Histograms += [ defineHistogram('jet_met_phi1',type='TH1F', title="DPhi1 No cut", xbins=25, xmin=0, xmax=4) ] 
        self.Histograms +=  [ defineHistogram('jet2et',      type='TH1F', title="EFCentralJet2 Et  No cut", xbins=50, xmin=0, xmax=500000) ]
        self.Histograms += [ defineHistogram('jet2phi',     type='TH1F', title="EFCentralJet2 Phi No cut", xbins=50, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('jet2eta',     type='TH1F', title="EFCentralJet2 Eta No cut", xbins=25, xmin=0, xmax=4) ]
        self.Histograms += [ defineHistogram('jet_met_phi2',type='TH1F', title="DPhi2 No cut", xbins=25, xmin=0, xmax=4) ]
        
        self.Histograms +=  [ defineHistogram('NJets_Pass',       type='TH1F', title="NJets", xbins=10, xmin=0, xmax=10) ]
        self.Histograms +=  [ defineHistogram('jet1et_Pass',      type='TH1F', title="EFCentralJet1 Et", xbins=50, xmin=0, xmax=500000) ]
        self.Histograms += [ defineHistogram('jet1phi_Pass',     type='TH1F', title="EFCentralJet1 Phi", xbins=50, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('jet1eta_Pass',     type='TH1F', title="EFCentralJet1 Eta", xbins=25, xmin=0, xmax=4) ]
        self.Histograms += [ defineHistogram('jet_met_phi1_Pass',type='TH1F', title="DPhi1", xbins=25, xmin=0, xmax=4) ] 
        self.Histograms +=  [ defineHistogram('jet2et_Pass',      type='TH1F', title="EFCentralJet2 Et", xbins=50, xmin=0, xmax=500000) ]
        self.Histograms += [ defineHistogram('jet2phi_Pass',     type='TH1F', title="EFCentralJet2 Phi", xbins=50, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('jet2eta_Pass',     type='TH1F', title="EFCentralJet2 Eta", xbins=25, xmin=0, xmax=4) ]
        self.Histograms += [ defineHistogram('jet_met_phi2_Pass',type='TH1F', title="DPhi2", xbins=25, xmin=0, xmax=4) ]
        

        
class TrigEFDPhiMetJetAllTEOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name="TrigEFDPhiMetJetAllTEOnlineMonitoring"):
        super(TrigEFDPhiMetJetAllTEOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms =  [ defineHistogram('NJets',       type='TH1F', title="NJets No cut", xbins=10, xmin=0, xmax=10) ]
        self.Histograms +=  [ defineHistogram('jet1et',      type='TH1F', title="EFCentralJet1 Et  No cut", xbins=50, xmin=0, xmax=500000) ]
        self.Histograms += [ defineHistogram('jet1phi',     type='TH1F', title="EFCentralJet1 Phi No cut", xbins=50, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('jet1eta',     type='TH1F', title="EFCentralJet1 Eta No cut", xbins=25, xmin=0, xmax=4) ]
        self.Histograms += [ defineHistogram('jet_met_phi1',type='TH1F', title="DPhi1 No cut", xbins=25, xmin=0, xmax=4) ] 
        self.Histograms +=  [ defineHistogram('jet2et',      type='TH1F', title="EFCentralJet2 Et  No cut", xbins=50, xmin=0, xmax=500000) ]
        self.Histograms += [ defineHistogram('jet2phi',     type='TH1F', title="EFCentralJet2 Phi No cut", xbins=50, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('jet2eta',     type='TH1F', title="EFCentralJet2 Eta No cut", xbins=25, xmin=0, xmax=4) ]
        self.Histograms += [ defineHistogram('jet_met_phi2',type='TH1F', title="DPhi2 No cut", xbins=25, xmin=0, xmax=4) ]
        
        self.Histograms +=  [ defineHistogram('NJets_Pass',       type='TH1F', title="NJets", xbins=10, xmin=0, xmax=10) ]
        self.Histograms +=  [ defineHistogram('jet1et_Pass',      type='TH1F', title="EFCentralJet1 Et", xbins=50, xmin=0, xmax=500000) ]
        self.Histograms += [ defineHistogram('jet1phi_Pass',     type='TH1F', title="EFCentralJet1 Phi", xbins=50, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('jet1eta_Pass',     type='TH1F', title="EFCentralJet1 Eta", xbins=25, xmin=0, xmax=4) ]
        self.Histograms += [ defineHistogram('jet_met_phi1_Pass',type='TH1F', title="DPhi1", xbins=25, xmin=0, xmax=4) ] 
        self.Histograms +=  [ defineHistogram('jet2et_Pass',      type='TH1F', title="EFCentralJet2 Et", xbins=50, xmin=0, xmax=500000) ]
        self.Histograms += [ defineHistogram('jet2phi_Pass',     type='TH1F', title="EFCentralJet2 Phi", xbins=50, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('jet2eta_Pass',     type='TH1F', title="EFCentralJet2 Eta", xbins=25, xmin=0, xmax=4) ]
        self.Histograms += [ defineHistogram('jet_met_phi2_Pass',type='TH1F', title="DPhi2", xbins=25, xmin=0, xmax=4) ]
        
