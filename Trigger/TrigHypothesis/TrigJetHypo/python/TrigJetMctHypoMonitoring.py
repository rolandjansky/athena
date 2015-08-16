# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from math import pi
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2JetMctHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2JetMctHypo_Validation"):
        super(TrigL2JetMctHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms =  [ defineHistogram('nTeMctHypoCounter',type='TH1F', title="L2JetMctHypo number of input TE",           xbins=6,   xmin=0,    xmax=6     )]
        self.Histograms += [ defineHistogram('nJetsTe1',         type='TH1F', title="L2JetMctHypo number of jets in TE1",        xbins=6,   xmin=0,    xmax=6     )]
        self.Histograms += [ defineHistogram('nJetsTe2',         type='TH1F', title="L2JetMctHypo number of jets in TE2",        xbins=6,   xmin=0,    xmax=6     )]
        self.Histograms += [ defineHistogram('mct',              type='TH1F', title="L2JetMctHypo Mct",                          xbins=200, xmin=0.0,  xmax=1000.0)]
        self.Histograms += [ defineHistogram('Passed_mct',       type='TH1F', title="L2JetMctHypo PASSED Mct",                   xbins=200, xmin=0.0,  xmax=1000.0)]
        self.Histograms += [ defineHistogram('dPhi',             type='TH1F', title="L2JetMctHypo deltaPhi",                     xbins=32,  xmin=0.0,  xmax=3.2   )]
        self.Histograms += [ defineHistogram('ptLeadingJet',     type='TH1F', title="L2JetMctHypo pt of Leading Jet (GeV)",      xbins=190, xmin=50.0, xmax=1000.0)]
        self.Histograms += [ defineHistogram('etaLeadingJet',    type='TH1F', title="L2JetMctHypo eta of Leading Jet",           xbins=100, xmin=-5.0, xmax=5.0   )]
        self.Histograms += [ defineHistogram('phiLeadingJet',    type='TH1F', title="L2JetMctHypo phi of Leading Jet",           xbins=64,  xmin=-3.2, xmax=3.2   )]
        self.Histograms += [ defineHistogram('ptNextLeadingJet', type='TH1F', title="L2JetMctHypo pt of Next Leading Jet (GeV)", xbins=197, xmin=15.0, xmax=1000.0)]
        self.Histograms += [ defineHistogram('etaNextLeadingJet',type='TH1F', title="L2JetMctHypo eta of Next Leading Jet",      xbins=100, xmin=-5.0, xmax=5.0   )]
        self.Histograms += [ defineHistogram('phiNextLeadingJet',type='TH1F', title="L2JetMctHypo phi of Next Leading Jet",      xbins=64,  xmin=-3.2, xmax=3.2   )]
        self.Histograms += [ defineHistogram('MctHypoResult',    type='TH1F', title="L2JetMctHypo Result",                       xbins=6,   xmin=0.0,  xmax=6.0   )]

class TrigL2JetMctHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2JetMctHypo_Online"):
        """ defines histograms for online """
        super(TrigL2JetMctHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms =  [ defineHistogram('nTeMctHypoCounter',type='TH1F', title="L2JetMctHypo number of input TE",           xbins=6,   xmin=0,    xmax=6     )]
        self.Histograms += [ defineHistogram('nJetsTe1',         type='TH1F', title="L2JetMctHypo number of jets in TE1",        xbins=6,   xmin=0,    xmax=6     )]
        self.Histograms += [ defineHistogram('nJetsTe2',         type='TH1F', title="L2JetMctHypo number of jets in TE2",        xbins=6,   xmin=0,    xmax=6     )]
        self.Histograms += [ defineHistogram('mct',              type='TH1F', title="L2JetMctHypo Mct",                          xbins=200, xmin=0.0,  xmax=1000.0)]
        self.Histograms += [ defineHistogram('Passed_mct',       type='TH1F', title="L2JetMctHypo PASSED Mct",                   xbins=200, xmin=0.0,  xmax=1000.0)]
        self.Histograms += [ defineHistogram('dPhi',             type='TH1F', title="L2JetMctHypo deltaPhi",                     xbins=32,  xmin=0.0,  xmax=3.2   )]
        self.Histograms += [ defineHistogram('ptLeadingJet',     type='TH1F', title="L2JetMctHypo pt of Leading Jet (GeV)",      xbins=190, xmin=50.0, xmax=1000.0)]
        self.Histograms += [ defineHistogram('etaLeadingJet',    type='TH1F', title="L2JetMctHypo eta of Leading Jet",           xbins=100, xmin=-5.0, xmax=5.0   )]
        self.Histograms += [ defineHistogram('phiLeadingJet',    type='TH1F', title="L2JetMctHypo phi of Leading Jet",           xbins=64,  xmin=-3.2, xmax=3.2   )]
        self.Histograms += [ defineHistogram('ptNextLeadingJet', type='TH1F', title="L2JetMctHypo pt of Next Leading Jet (GeV)", xbins=197, xmin=15.0, xmax=1000.0)]
        self.Histograms += [ defineHistogram('etaNextLeadingJet',type='TH1F', title="L2JetMctHypo eta of Next Leading Jet",      xbins=100, xmin=-5.0, xmax=5.0   )]
        self.Histograms += [ defineHistogram('phiNextLeadingJet',type='TH1F', title="L2JetMctHypo phi of Next Leading Jet",      xbins=64,  xmin=-3.2, xmax=3.2   )]
        self.Histograms += [ defineHistogram('MctHypoResult',    type='TH1F', title="L2JetMctHypo Result",                       xbins=6,   xmin=0.0,  xmax=6.0   )]

