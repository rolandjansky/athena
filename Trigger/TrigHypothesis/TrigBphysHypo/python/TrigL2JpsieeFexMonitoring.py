# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2JpsieeFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2JpsieeFexValidation"):
        super(TrigL2JpsieeFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms  = [ defineHistogram('Ntrack', type='TH1F', title="L2JpsieeFex Ntrack",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('trackPt', type='TH1F', title="L2JpsieeFex trackPt; P_{t} [GeV]",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="L2JpsieeFex track deta ",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="L2JpsieeFex track dphi",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('Eta, Phi', type = 'TH2F', title = 'L2JpsieeFex - track direction ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram('dz0', type='TH1F', title="L2JpsieeFex track dz0",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('Ptsum', type='TH1F', title="L2JpsieeFex track pTsum",
                                           xbins=50, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('JpsiMass', type='TH1F', title="L2JpsieeFex JpsiMass; JpsiMass[GeV]",
                                           xbins=100, xmin=1., xmax=4.) ]
        self.Histograms += [ defineHistogram('JpsiFitMass', type='TH1F', title="L2JpsieeFex FitJpsiMass; FitJpsiMass [GeV]",
                                           xbins=100, xmin=1., xmax=4.) ]   
        self.Histograms += [ defineHistogram('RoIEta', type='TH1F', title="L2JpsieeFex RoI eta",
                                           xbins=20, xmin=-3., xmax=3.) ]
        self.Histograms += [ defineHistogram('RoIPhi', type='TH1F', title="L2JpsieeFex RoI Phi",
                                           xbins=20, xmin=-3.5, xmax=3.5)]    
        self.Histograms += [ defineHistogram('calo_dEta', type='TH1F', title="L2JpsieeFex Eta(RoI)-Eta(Calo)",
                                           xbins=20, xmin=0., xmax=4.) ]
        self.Histograms += [ defineHistogram('calo_dPhi', type='TH1F', title="L2JpsieeFex Phi(RoI)-Phi(Calo)",
                                           xbins=20, xmin=0., xmax=4.) ] 
        self.Histograms += [ defineHistogram('TRTRatio', type='TH1F', title="L2JpsieeFex TRTRatio",
                                           xbins=20, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('etoverpt', type='TH1F', title="L2JpsieeFex et/pt",
                                           xbins=20, xmin=0., xmax=1.) ]  
        self.Histograms += [ defineHistogram('dEtaCalo', type='TH1F', title="L2JpsieeFex dEtaCalo",
                                           xbins=20, xmin=0., xmax=4.) ]
        self.Histograms += [ defineHistogram('dPhiCalo', type='TH1F', title="L2JpsieeFex dPhiCalo",
                                           xbins=20, xmin=0., xmax=4.) ]



class TrigL2JpsieeFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2JpsieeFexOnline"):
        super(TrigL2JpsieeFexOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('Ntrack', type='TH1F', title="L2JpsieeFex Ntrack",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('trackPt', type='TH1F', title="L2JpsieeFex trackPt; P_{t} [GeV]",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="L2JpsieeFex track deta ",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="L2JpsieeFex track dphi",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('Eta, Phi', type = 'TH2F', title = 'L2JpsieeFex - track direction ; #eta ; #phi ; # Events',
                                               xbins = 60, xmin = -3.0, xmax = 3.0,
                                               ybins = 64, ymin = -3.2, ymax = 3.2 ) ]
        self.Histograms += [ defineHistogram('dz0', type='TH1F', title="L2JpsieeFex track dz0",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('Ptsum', type='TH1F', title="L2JpsieeFex track pTsum",
                                           xbins=50, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('JpsiMass', type='TH1F', title="L2JpsieeFex JpsiMass; JpsiMass[GeV]",
                                           xbins=100, xmin=1., xmax=4.) ]
        self.Histograms += [ defineHistogram('JpsiFitMass', type='TH1F', title="L2JpsieeFex FitJpsiMass; FitJpsiMass [GeV]",
                                           xbins=100, xmin=1., xmax=4.) ]   
        self.Histograms += [ defineHistogram('RoIEta', type='TH1F', title="L2JpsieeFex RoI eta",
                                           xbins=20, xmin=-3., xmax=3.) ]
        self.Histograms += [ defineHistogram('RoIPhi', type='TH1F', title="L2JpsieeFex RoI Phi",
                                           xbins=20, xmin=-3.5, xmax=3.5)]    
        self.Histograms += [ defineHistogram('calo_dEta', type='TH1F', title="L2JpsieeFex Eta(RoI)-Eta(Calo)",
                                           xbins=20, xmin=0., xmax=4.) ]
        self.Histograms += [ defineHistogram('calo_dPhi', type='TH1F', title="L2JpsieeFex Phi(RoI)-Phi(Calo)",
                                           xbins=20, xmin=0., xmax=4.) ] 
        self.Histograms += [ defineHistogram('TRTRatio', type='TH1F', title="L2JpsieeFex TRTRatio",
                                           xbins=20, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('etoverpt', type='TH1F', title="L2JpsieeFex et/pt",
                                           xbins=20, xmin=0., xmax=1.) ]  
        self.Histograms += [ defineHistogram('dEtaCalo', type='TH1F', title="L2JpsieeFex dEtaCalo",
                                           xbins=20, xmin=0., xmax=4.) ]
        self.Histograms += [ defineHistogram('dPhiCalo', type='TH1F', title="L2JpsieeFex dPhiCalo",
                                           xbins=20, xmin=0., xmax=4.) ]


        
