# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from math import pi
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigL2JetDEtaMjjAllTEValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2JetDEtaMjjAllTE_Validation"):
        super(TrigL2JetDEtaMjjAllTEValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms =  [ defineHistogram('nJets', type='TH1F', title="nJets Passing Kinematics", xbins=20, xmin=-0.5, xmax=19.5)]
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="Events Pass/Fail", xbins=2, xmin=-0.5, xmax=1.5)]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="Passing Jets E", xbins=50, xmin=0, xmax=250*1000)]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="Passing Jets Et", xbins=50, xmin=0, xmax=250*1000)]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Passing Jets Eta", xbins=50, xmin=-5, xmax=5)]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Passing Jets Phi", xbins=30, xmin=-pi, xmax=pi)]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="dEta", xbins=20, xmin=0, xmax=10)]
        self.Histograms += [ defineHistogram('mjj', type='TH1F', title="Mjj", xbins=20, xmin=0, xmax=1000*1000)]

        
class TrigL2JetDEtaMjjAllTEOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2JetDEtaMjjAllTE_Online"):
        """ defines histograms for online """
        super(TrigL2JetDEtaMjjAllTEOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms =  [ defineHistogram('nJets', type='TH1F', title="nJets Passing Kinematics", xbins=20, xmin=-0.5, xmax=19.5)]
        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="Events Pass/Fail", xbins=2, xmin=-0.5, xmax=1.5)]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="Passing Jets E", xbins=50, xmin=0, xmax=250*1000)]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="Passing Jets Et", xbins=50, xmin=0, xmax=250*1000)]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="Passing Jets Eta", xbins=50, xmin=-5, xmax=5)]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="Passing Jets Phi", xbins=30, xmin=-pi, xmax=pi)]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="dEta", xbins=20, xmin=0, xmax=10)]
        self.Histograms += [ defineHistogram('mjj', type='TH1F', title="Mjj", xbins=20, xmin=0, xmax=1000*1000)]
