# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" T2IDTau Monitoring """

__author__  = 'O.Igonkina, A.Kalinowski'
__version__=""
__doc__="Implementation of T2IDTau  Monitoring"

################# Validation, DQ checks
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
  
class T2IDTauOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2IDTauOnlineMonitoring"):
        super(T2IDTauOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

##in order to reduce the totla number of bins, this histogram is moved to T2TauFinal
##         self.Histograms += [ defineHistogram('EtaL1, PhiL1', type='TH2F', title="L1 ROI Eta vs Phi in T2IDTau FEX; #eta; #varphi; nevents",
##                                              xbins=51, xmin=-2.55, xmax=2.55,
##                                              ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)]
        self.Histograms += [ defineHistogram('NTrk', type='TH1F', title="Number of tracks;Number of tracks; nevents", xbins=17, xmin=-2.0, xmax=15.0) ]
        #self.Histograms += [ defineHistogram('charge', type='TH1F', title="charge;charge; nevents", xbins=10, xmin=-5.5, xmax=4.5) ]
        #self.Histograms += [ defineHistogram('NTrkSlow', type='TH1F', title="Number of slow tracks;Number of slow tracks; nevents", xbins=17, xmin=-2.0, xmax=15.0) ]
        #self.Histograms += [ defineHistogram('NTrkIso', type='TH1F', title="Number of tracks in isolation area;Number of tracks in isolation area; nevents", xbins=17, xmin=-2.0, xmax=15.0) ]

        self.Histograms += [ defineHistogram('SumPtCore',  type='TH1F', title="SumPt Core;#sum p_{T} [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('SumPtIso',   type='TH1F', title="sumPt Iso; #sum p_{T} [MeV]; nevents", xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('sumPtRatio', type='TH1F', title="sumPt_core over sumPt_isol;sumPt ratio; nevents", xbins=40, xmin=0., xmax=2.) ]
        self.Histograms += [ defineHistogram('alldZ0',     type='TH1F', title="#DeltaZ0 lead trk-all trk;#Delta z0 [mm]; nevents", xbins=100, xmin=-20., xmax=20.) ]        

        #self.Histograms += [ defineHistogram('PtMaxTrk', type='TH1F', title="Pt of Highest Pt Track;Pt of Highest Pt Track [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]

        #self.Histograms += [ defineHistogram('TrkRadius', type='TH1F', title="Pt weighted Track Radius w.r.t leading track direction;Pt weighted Track Radius; nevents", xbins=101, xmin=-0.001, xmax=0.2) ]
########## add validation specific histograms 
class T2IDTauValidationMonitoring(T2IDTauOnlineMonitoring):
    def __init__ (self, name="T2IDTauValidationMonitoring"):
        super(T2IDTauValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        
