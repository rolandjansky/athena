# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from math import pi
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2JetDphiHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2JetDphiHypo_Validation"):
        super(TrigL2JetDphiHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms = [ defineHistogram('nTeDphiHypoCounter', type='TH1F', title="L2JetDphiHypo number of input TE", xbins=6, xmin=0, xmax=6)]
        self.Histograms += [ defineHistogram('nJetsTe1', type='TH1F', title="L2JetDphiHypo number of jets in TE1", xbins=6, xmin=0, xmax=6)]
        self.Histograms += [ defineHistogram('nJetsTe2', type='TH1F', title="L2JetDphiHypo number of jets in TE2", xbins=6, xmin=0, xmax=6)]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="L2JetDphiHypo deltaPhi", xbins=32, xmin=0.0, xmax=3.2)]
        self.Histograms += [ defineHistogram('Passed_dPhi', type='TH1F', title="L2JetDphiHypo PASSED deltaPhi", xbins=32, xmin=0.0, xmax=3.2)]
        self.Histograms += [ defineHistogram('ptLeadingJet', type='TH1F', title="L2JetDphiHypo pt of Leading Jet (GeV)", xbins=190, xmin=50.0, xmax=1000.0)]
        self.Histograms += [ defineHistogram('etaLeadingJet', type='TH1F', title="L2JetDphiHypo eta of Leading Jet", xbins=100, xmin=-5.0, xmax=5.0)]
        self.Histograms += [ defineHistogram('phiLeadingJet', type='TH1F', title="L2JetDphiHypo phi of Leading Jet", xbins=64, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('ptNextLeadingJet', type='TH1F', title="L2JetDphiHypo pt of Next Leading Jet (GeV)", xbins=197, xmin=15.0, xmax=1000.0)]
        self.Histograms += [ defineHistogram('etaNextLeadingJet', type='TH1F', title="L2JetDphiHypo eta of Next Leading Jet", xbins=100, xmin=-5.0, xmax=5.0)]
        self.Histograms += [ defineHistogram('phiNextLeadingJet', type='TH1F', title="L2JetDphiHypo phi of Next Leading Jet", xbins=64, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('dphiHypoResult', type='TH1F', title="L2JetDphiHypo Result", xbins=6, xmin=0.0, xmax=6.0)]


class TrigL2JetDphiHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2JetDphiHypo_Online"):
        """ defines histograms for online """
        super(TrigL2JetDphiHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('nTeDphiHypoCounter', type='TH1F', title="L2JetDphiHypo number of input TE", xbins=6, xmin=0, xmax=6)]
        self.Histograms += [ defineHistogram('nJetsTe1', type='TH1F', title="L2JetDphiHypo number of jets in TE1", xbins=6, xmin=0, xmax=6)]
        self.Histograms += [ defineHistogram('nJetsTe2', type='TH1F', title="L2JetDphiHypo number of jets in TE2", xbins=6, xmin=0, xmax=6)]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="L2JetDphiHypo deltaPhi", xbins=32, xmin=0.0, xmax=3.2)]
        self.Histograms += [ defineHistogram('Passed_dPhi', type='TH1F', title="L2JetDphiHypo PASSED deltaPhi", xbins=32, xmin=0.0, xmax=3.2)]
        self.Histograms += [ defineHistogram('ptLeadingJet', type='TH1F', title="L2JetDphiHypo pt of Leading Jet (GeV)", xbins=190, xmin=50.0, xmax=1000.0)]
        self.Histograms += [ defineHistogram('etaLeadingJet', type='TH1F', title="L2JetDphiHypo eta of Leading Jet", xbins=100, xmin=-5.0, xmax=5.0)]
        self.Histograms += [ defineHistogram('phiLeadingJet', type='TH1F', title="L2JetDphiHypo phi of Leading Jet", xbins=64, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('ptNextLeadingJet', type='TH1F', title="L2JetDphiHypo pt of Next Leading Jet (GeV)", xbins=197, xmin=15.0, xmax=1000.0)]
        self.Histograms += [ defineHistogram('etaNextLeadingJet', type='TH1F', title="L2JetDphiHypo eta of Next Leading Jet", xbins=100, xmin=-5.0, xmax=5.0)]
        self.Histograms += [ defineHistogram('phiNextLeadingJet', type='TH1F', title="L2JetDphiHypo phi of Next Leading Jet", xbins=64, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('dphiHypoResult', type='TH1F', title="L2JetDphiHypo Result", xbins=6, xmin=0.0, xmax=6.0)]
