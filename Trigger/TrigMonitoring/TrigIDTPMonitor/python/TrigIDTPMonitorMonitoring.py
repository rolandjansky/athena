# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig


class TrigIDTPMonitorMonitorBase(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigIDTPMonitorMonitorBase"):
        super(TrigIDTPMonitorMonitorBase, self).__init__(name)
        #self.defineTarget("Validation")
        
        self.Histograms = [ defineHistogram('InvMassD', type='TH1F', title="TrigIDTPMonitor InvMass of TrigBphys object; invMass [MeV]",
                                            xbins=30, xmin=65000, xmax=115000) ]

        self.Histograms += [ defineHistogram('InvMassN', type='TH1F', title="TrigIDTPMonitor InvMass of TrigBphys (if ID Track found); calcinvMass [MeV]",
                                             xbins=30, xmin=65000, xmax=115000) ]
        
        self.Histograms += [ defineHistogram('InvMass , LinesIM ', type='TH2F', title="TrigIDTPMonitor InvMass of TrigBphys object; invMass [MeV]",
                                             xbins=30, xmin=65000, xmax=115000,
                                             ybins = 3 ,  ymin=0, ymax=3) ]
        
        
        self.Histograms += [ defineHistogram('InvMassTPD',type='TH1F', title="TrigIDTPMonitor CalculatedInvMass of Tag and Probe; CinvMassTP [MeV]",
                                             xbins=30, xmin=65000, xmax=115000) ]

        self.Histograms += [ defineHistogram('InvMassTPN',type='TH1F', title="TrigIDTPMonitor CalculatedInvMass of Tag and Probe (if ID Track found); CinvMass [MeV]",
                                             xbins=30, xmin=65000, xmax=115000) ]
         
        self.Histograms += [ defineHistogram('InvMassTP , LinesTP ', type='TH2F', title="TrigIDTPMonitor InvMass of Tag & Probe; invMass [MeV]",
                                             xbins=30, xmin=65000, xmax=115000,
                                             ybins = 3 ,  ymin=0, ymax=3) ] 
                
        self.Histograms += [ defineHistogram('ProbePTD',type='TH1F', title="TrigIDTPMonitor ProbePT; pt [MeV]",
                                             xbins=20, xmin=0, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('ProbePTN',type='TH1F', title="TrigIDTPMonitor ProbePT (if ID Track found); pt [MeV]",
                                             xbins=20, xmin=0, xmax=100000) ]
     
        self.Histograms += [ defineHistogram('ProbePt , LinesPt ', type='TH2F', title="TrigIDTPMonitor Probe pt; pt [MeV]",
                                             xbins=20, xmin=0, xmax=100000,
                                             ybins = 3,  ymin=0, ymax=3) ] 
     
        self.Histograms += [ defineHistogram('ProbeEtaD',type='TH1F', title="TrigIDTPMonitor ProbeEta; eta ",
                                             xbins=15, xmin=-3, xmax=3) ]
        
        self.Histograms += [ defineHistogram('ProbeEtaN',type='TH1F', title="TrigIDTPMonitor ProbeEta if (ID Track found); eta ",
                                             xbins=15, xmin=-3, xmax=3) ]
        
        self.Histograms += [ defineHistogram('ProbeEta , LinesEta ', type='TH2F', title="TrigIDTPMonitor Probe eta; eta",
                                             xbins=15, xmin=-3, xmax=3, 
                                             ybins = 3,  ymin=0, ymax=3) ]
        
        #self.Histograms += [ defineHistogram('ProbeD0D',type='TH1F', title="TrigIDTPMonitor Probe d0; d0 [m]",                                                         xbins=20, xmin=-0.002, xmax=0.002) ]
        
        #self.Histograms += [ defineHistogram('ProbeD0N',type='TH1F', title="TrigIDTPMonitor Probe d0 (if ID Track found); d0 [m]",                                                         xbins=20, xmin=-0.002, xmax=0.002) ]
        
        #self.Histograms += [ defineHistogram('ProbeD0 , LinesD0 ', type='TH2F', title="TrigIDTPMonitor Probe d0[m]; d0", xbins=10, xmin=-0.002, xmax=0.002,  ybins = 6 ,  ymin=0, ymax=6) ] 
        
        self.Histograms += [ defineHistogram('ProbeD0D',type='TH1F', title="TrigIDTPMonitor Probe d0; d0 [m]",
                                             xbins=15, xmin=-30, xmax=30) ]
        
        self.Histograms += [ defineHistogram('ProbeD0N',type='TH1F', title="TrigIDTPMonitor Probe d0 (if ID Track found); d0 [mm]",
                                             xbins=15, xmin=-30, xmax=30) ]
 

        self.Histograms += [ defineHistogram('ProbeD0 , LinesD0 ', type='TH2F', title="TrigIDTPMonitor Probe d0; d0 [m]",
                                             xbins=15, xmin=-30, xmax=30,
                                             ybins = 3,  ymin=0, ymax=3) ] 
        
        self.Histograms += [ defineHistogram('ProbePhiD',type='TH1F', title="TrigIDTPMonitor Probe Phi; phi [rad]",
                                             xbins=15, xmin=-3.2, xmax=3.2) ]
        
        self.Histograms += [ defineHistogram('ProbePhiN',type='TH1F', title="TrigIDTPMonitor Probe Phi (if ID Track found); phi [rad]",
                                             xbins=15, xmin=-3.2, xmax=3.2) ]
        
        self.Histograms += [ defineHistogram('ProbePhi , LinesPhi ', type='TH2F', title="TrigIDTPMonitor Probe phi; phi[rad]",
                                             xbins=15, xmin=-3.2, xmax=3.2,
                                             ybins = 3,  ymin=0, ymax=3) ]
        
        self.Histograms += [ defineHistogram('deltaMass', type='TH1F', title="TrigIDTPMonitor mass difference of t&p to Z mass; deltaM [MeV]",
                                             xbins=25, xmin=-10000, xmax=10000) ]
        
        self.Histograms += [ defineHistogram('ID-TagMass', type='TH1F', title="TrigIDTPMonitor InvMass PT ID and Tag; calcinvMass [MeV]",
                                             xbins=20, xmin=60000, xmax=130000) ]
        
        self.Histograms += [ defineHistogram('Efficiencies',type='TH1F', title="TrigIDTPMonitor Overall Eff; strat",
                                             xbins=3, xmin=0, xmax=3) ]
        
        #self.Histograms += [ defineHistogram('TPcounts',type='TH1F', title="TrigIDTPMonitor Used Tags and Probes during the analysis; strat",                                                         xbins=3, xmin=0, xmax=3) ]


class TrigIDTPMonitorOnlineMonitor(TrigIDTPMonitorMonitorBase):
    def __init__ (self, name="TrigIDTPMonitorOnlineMonitor"):
        super(TrigIDTPMonitorOnlineMonitor, self).__init__(name)
        self.defineTarget("Online")

class TrigIDTPMonitorValidationMonitor(TrigIDTPMonitorMonitorBase):
    def __init__ (self, name="TrigIDTPMonitorValidationMonitor"):
        super(TrigIDTPMonitorValidationMonitor, self).__init__(name)
        self.defineTarget("Validation")
