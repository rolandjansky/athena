# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
from AthenaCommon.SystemOfUnits import GeV

cuts=['Input','Has EmuTopoInfo','Opposite Charge','Common Vertex','dPhi','dR','mass']

labelsDescription = ''
for c in cuts:
    labelsDescription += c+':'


class TrigEFElectronMuonAngleOnlineFexMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFElectronMuonAngleOnlineFexMonitoring"):
        """ defines hisograms for online """
        super(TrigEFElectronMuonAngleOnlineFexMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                            xbins=7, xmin=-0.5, xmax=6.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('dPhiOfAll', type='TH1F', title="Lepton dPhi: all",
                                             xbins=50, xmin=0., xmax=5.0) ]
        self.Histograms += [ defineHistogram('dROfAll', type='TH1F', title="Lepton dR: all",
                                             xbins=50, xmin=0., xmax=5.0) ]        
        self.Histograms += [ defineHistogram('massOfAll', type='TH1F', title="Mass in all combinations passing pre-selection",
                                             xbins=50, xmin=0.*GeV, xmax=30.*GeV) ]
        self.Histograms += [ defineHistogram('Z0pull', type='TH1F', title="Z0 pull:|electron_z0-muon_z0| / (3*sqrt(el_z0_err^2+mu_z0_err^2)) ",
                                             xbins=50, xmin=0., xmax=3.) ]
        self.Histograms += [ defineHistogram('Vx_state', type='TH1F', title="Vertex comb result (0=common 1= not common 2= unapplicable)",
                                             xbins=3, xmin=-0.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('massOfAll, dROfAll', type='TH2F', title="Mass vs dR; Mass[MeV]; dR",
                                             xbins=50, xmin=0.*GeV, xmax=50.*GeV, ybins=50, ymin=0., ymax=5.0) ]


class TrigEFElectronMuonAngleOnlineHypoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFElectronMuonAngleOnlineHypoMonitoring"):
        """ defines hisograms for online """
        super(TrigEFElectronMuonAngleOnlineHypoMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                            xbins=7, xmin=-0.5, xmax=7.5, opt="kCumulative", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('dPhiOfAll', type='TH1F', title="Lepton dPhi: all",
                                             xbins=50, xmin=0., xmax=5.0) ]
        self.Histograms += [ defineHistogram('dROfAll', type='TH1F', title="Lepton dR: all",
                                             xbins=50, xmin=0., xmax=5.0) ]        
        self.Histograms += [ defineHistogram('massOfAll', type='TH1F', title="Mass in all combinations passing pre-selection",
                                             xbins=50, xmin=0.*GeV, xmax=30.*GeV) ]
        self.Histograms += [ defineHistogram('Vx_state', type='TH1F', title="Vertex comb result (0=common 1= not common 2= unapplicable)",
                                             xbins=3, xmin=-0.5, xmax=2.5) ]


        

class TrigEFElectronMuonAngleValidationFexMonitoring_emutopo(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFElectronMuonAngleValidationFexMonitoring_emutopo"):
        """ defines hisograms for online """
        super(TrigEFElectronMuonAngleValidationFexMonitoring_emutopo, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                             xbins=7, xmin=-0.5, xmax=6.5, opt="kCumulative") ]
        self.Histograms += [ defineHistogram('dPhiOfAccepted', type='TH1F', title="Lepton dPhi in accepted combinations",
                                             xbins=50, xmin=0., xmax=2.5) ]
        self.Histograms += [ defineHistogram('dPhiOfAll', type='TH1F', title="Lepton dPhi: all",
                                             xbins=50, xmin=0., xmax=5.0) ]        
        self.Histograms += [ defineHistogram('dROfAccepted', type='TH1F', title="Lepton dR in accepted combinations",
                                             xbins=50, xmin=0., xmax=5.0) ]
        self.Histograms += [ defineHistogram('dROfAll', type='TH1F', title="Lepton dR: all",
                                             xbins=50, xmin=0., xmax=5.0) ]        
        self.Histograms += [ defineHistogram('massOfAccepted', type='TH1F', title="Mass in accepted combinations",
                                             xbins=50, xmin=0.*GeV, xmax=50.*GeV) ]
        self.Histograms += [ defineHistogram('massOfAll', type='TH1F', title="Mass in all combinations passing pre-selection",
                                             xbins=50, xmin=0.*GeV, xmax=30.*GeV) ]
        self.Histograms += [ defineHistogram('Z0pull', type='TH1F', title="Z0 pull:|electron_z0-muon_z0| / (3*sqrt(el_z0_err^2+mu_z0_err^2)) ",
                                             xbins=50, xmin=0., xmax=3.) ]
        self.Histograms += [ defineHistogram('Vx_state', type='TH1F', title="Vertex comb result (0=common 1= not common 2= unapplicable)",
                                             xbins=3, xmin=-0.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('massOfAll, dROfAll', type='TH2F', title="Mass vs dR; Mass[MeV]; dR",
                                             xbins=50, xmin=0.*GeV, xmax=50.*GeV, ybins=50, ymin=0., ymax=5.0) ]


class TrigEFElectronMuonAngleValidationHypoMonitoring_emutopo(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFElectronMuonAngleValidationHypoMonitoring_emutopo"):
        """ defines hisograms for online """
        super(TrigEFElectronMuonAngleValidationHypoMonitoring_emutopo, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                             xbins=7, xmin=-0.5, xmax=7.5, opt="kCumulative", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('dPhiOfAccepted', type='TH1F', title="Lepton dPhi in accepted combinations",
                                             xbins=50, xmin=0., xmax=2.5) ]
        self.Histograms += [ defineHistogram('dPhiOfAll', type='TH1F', title="Lepton dPhi: all",
                                             xbins=50, xmin=0., xmax=5.0) ]        
        self.Histograms += [ defineHistogram('dROfAccepted', type='TH1F', title="Lepton dR in accepted combinations",
                                             xbins=50, xmin=0., xmax=5.0) ]
        self.Histograms += [ defineHistogram('dROfAll', type='TH1F', title="Lepton dR: all",
                                             xbins=50, xmin=0., xmax=5.0) ]        
        self.Histograms += [ defineHistogram('massOfAccepted', type='TH1F', title="Mass in accepted combinations",
                                             xbins=50, xmin=0.*GeV, xmax=50.*GeV) ]
        self.Histograms += [ defineHistogram('massOfAll', type='TH1F', title="Mass in all combinations passing pre-selection",
                                             xbins=50, xmin=0.*GeV, xmax=30.*GeV) ]
        self.Histograms += [ defineHistogram('Vx_state', type='TH1F', title="Vertex comb result (0=common 1= not common 2= unapplicable)",
                                             xbins=3, xmin=-0.5, xmax=2.5) ]


#=======================================================================================

class TrigEFPhotonMuonAngleOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFPhotonMuonAngleOnlineMonitoring"):
        """ defines hisograms for online """
        super(TrigEFPhotonMuonAngleOnlineMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="Photon-Muon dPhi",
                                             xbins=50, xmin=0., xmax=2.5) ]        
        self.Histograms += [ defineHistogram('dR', type='TH1F', title="Photon-Muon dR",
                                             xbins=50, xmin=0., xmax=2.5) ]        
        self.Histograms += [ defineHistogram('mass', type='TH1F', title="Photon-Muon Mass ",
                                             xbins=60, xmin=0.*GeV, xmax=15.*GeV) ]

