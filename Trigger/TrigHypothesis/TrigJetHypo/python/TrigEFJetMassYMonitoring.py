# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 


class TrigEFJetMassYValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFJetMassYValidationMonitoring"):
        super(TrigEFJetMassYValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('NJet', type='TH1F', title="Number Jets", xbins=20, xmin=-0.5, xmax=19.5) ]
        self.Histograms += [ defineHistogram('Ystar', type='TH1F', title="Ystar leading-pt jets", xbins=50, xmin=0, xmax=3) ]
        self.Histograms += [ defineHistogram('Mjj',type='TH1F', title="Mjj leading-pt jets", xbins=200, xmin=0, xmax=3000000) ]
        self.Histograms += [ defineHistogram('Ystar, Mjj',type='TH2F', title="Mjj leading-pt jets vs. Ystar", xbins=50, xmin=0, xmax=3, ybins=200, ymin=0, ymax=3000000) ]
        self.Histograms += [ defineHistogram('Ystar_cuted', type='TH1F', title="Ystar leading-pt jets after cut", xbins=50, xmin=0, xmax=3) ]
        self.Histograms += [ defineHistogram('Mjj_cuted',type='TH1F', title="Mjj leading-pt jets after cut", xbins=200, xmin=0, xmax=3000000) ]
        self.Histograms += [ defineHistogram('Ystar_cuted, Mjj_cuted',type='TH2F', title="Mjj leading-pt jets vs. Ystar after cut", xbins=50, xmin=0, xmax=3, ybins=200, ymin=0, ymax=3000000) ]
        self.Histograms += [ defineHistogram('pt', type='TH1F', title="pt leading-pt jet", xbins=50, xmin=0, xmax=3000000) ]
        self.Histograms += [ defineHistogram('pt_cut', type='TH1F', title="pt leading-pt jet after cut", xbins=50, xmin=0, xmax=3000000) ]
        self.Histograms += [ defineHistogram('eta', type='TH1F', title="eta leading-pt jet", xbins=98, xmin=-4.9, xmax=4.9) ]
        self.Histograms += [ defineHistogram('eta_cut', type='TH1F', title="eta leading-pt jet after cut", xbins=98, xmin=-4.9, xmax=4.9) ]
        

class TrigEFJetMassYOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFJetMassYOnlineMonitoring"):
        """ defines histograms for online """
        super(TrigEFJetMassYOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('NJet', type='TH1F', title="Number Jets", xbins=20, xmin=-0.5, xmax=19.5) ]
        self.Histograms += [ defineHistogram('Ystar', type='TH1F', title="Ystar leading-pt jets", xbins=50, xmin=0, xmax=3) ]
        self.Histograms += [ defineHistogram('Mjj',type='TH1F', title="Mjj leading-pt jets", xbins=200, xmin=0, xmax=3000000) ]
        self.Histograms += [ defineHistogram('Ystar, Mjj',type='TH2F', title="Mjj leading-pt jets vs. Ystar", xbins=50, xmin=0, xmax=3, ybins=200, ymin=0, ymax=3000000) ]
        self.Histograms += [ defineHistogram('Ystar_cuted', type='TH1F', title="Ystar leading-pt jets after cut", xbins=50, xmin=0, xmax=3) ]
        self.Histograms += [ defineHistogram('Mjj_cuted',type='TH1F', title="Mjj leading-pt jets after cut", xbins=200, xmin=0, xmax=3000000) ]
        self.Histograms += [ defineHistogram('Ystar_cuted, Mjj_cuted',type='TH2F', title="Mjj leading-pt jets vs. Ystar after cut", xbins=50, xmin=0, xmax=3, ybins=200, ymin=0, ymax=3000000) ]
        self.Histograms += [ defineHistogram('pt', type='TH1F', title="pt leading-pt jet", xbins=50, xmin=0, xmax=3000000) ]
        self.Histograms += [ defineHistogram('pt_cut', type='TH1F', title="pt leading-pt jet after cut", xbins=50, xmin=0, xmax=3000000) ]
        self.Histograms += [ defineHistogram('eta', type='TH1F', title="eta leading-pt jet", xbins=98, xmin=-4.9, xmax=4.9) ]
        self.Histograms += [ defineHistogram('eta_cut', type='TH1F', title="eta leading-pt jet after cut", xbins=98, xmin=-4.9, xmax=4.9) ]
        
