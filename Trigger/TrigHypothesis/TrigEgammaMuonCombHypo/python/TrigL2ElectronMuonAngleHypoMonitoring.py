# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
from AthenaCommon.SystemOfUnits import GeV

cuts=['Input','Has EmuTopoInfo','Opposite Charge','Common Vertex','dPhi','dR','mass']

labelsDescription = ''
for c in cuts:
    labelsDescription += c+':'


class TrigL2ElectronMuonAngleOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2ElectronMuonAngleOnlineMonitoring"):
        """ defines hisograms for online """
        super(TrigL2ElectronMuonAngleOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                            xbins=8, xmin=-0.5, xmax=7.5, opt="kCumulative", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('dPhiOfAll', type='TH1F', title="Lepton dPhi: all",
                                             xbins=50, xmin=0., xmax=5.0) ]        
        self.Histograms += [ defineHistogram('dROfAll', type='TH1F', title="Lepton dR: all",
                                             xbins=50, xmin=0., xmax=5.0) ]        
        self.Histograms += [ defineHistogram('massOfAll', type='TH1F', title="Mass in all combinations passing pre-selection",
                                             xbins=50, xmin=0.*GeV, xmax=50.*GeV) ]
        self.Histograms += [ defineHistogram('massOfAll, dROfAll', type='TH2F', title="Mass vs dR; Mass[MeV]; dR",
                                             xbins=50, xmin=0.*GeV, xmax=50.*GeV, ybins=50, ymin=0., ymax=5.0) ]

class TrigL2ElectronMuonAngleValidationMonitoring_emutopo(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2ElectronMuonAngleValidationMonitoring_emutopo"):
        """ defines hisograms for online """
        super(TrigL2ElectronMuonAngleValidationMonitoring_emutopo, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                             xbins=8, xmin=-0.5, xmax=7.5, opt="kCumulative", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('dPhiOfAccepted', type='TH1F', title="Lepton dPhi in accepted combinations",
                                             xbins=50, xmin=0., xmax=2.5) ]
        self.Histograms += [ defineHistogram('dPhiOfAll', type='TH1F', title="Lepton dPhi: all",
                                             xbins=50, xmin=0., xmax=5.0) ]        
        self.Histograms += [ defineHistogram('dROfAccepted', type='TH1F', title="Lepton dR in accepted combinations",
                                             xbins=50, xmin=0., xmax=2.5) ]
        self.Histograms += [ defineHistogram('dROfAll', type='TH1F', title="Lepton dR: all",
                                             xbins=50, xmin=0., xmax=5.0) ]        
        self.Histograms += [ defineHistogram('massOfAccepted', type='TH1F', title="Mass in accepted combinations",
                                             xbins=50, xmin=0.*GeV, xmax=50.*GeV) ]
        self.Histograms += [ defineHistogram('massOfAll', type='TH1F', title="Mass in all combinations passing pre-selection",
                                             xbins=50, xmin=0.*GeV, xmax=50.*GeV) ]

#=======================================================================================

class TrigL2PhotonMuonAngleOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2PhotonMuonAngleOnlineMonitoring"):
        """ defines hisograms for online """
        super(TrigL2PhotonMuonAngleOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="Photon-Muon dPhi",
                                             xbins=50, xmin=0., xmax=2.5) ]        
        self.Histograms += [ defineHistogram('dR', type='TH1F', title="Photon-Muon dR",
                                             xbins=50, xmin=0., xmax=2.5) ]        
        self.Histograms += [ defineHistogram('mass', type='TH1F', title="Photon-Muon Mass ",
                                             xbins=60, xmin=0.*GeV, xmax=15.*GeV) ]

class TrigL2PhotonMuonAngleValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2PhotonMuonAngleValidationMonitoring"):
        """ defines hisograms for validation """
        super(TrigL2PhotonMuonAngleValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")


        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="Photon-Muon dPhi",
                                             xbins=50, xmin=0., xmax=2.5) ]        
        self.Histograms += [ defineHistogram('dR', type='TH1F', title="Photon-Muon dR",
                                             xbins=50, xmin=0., xmax=2.5) ]        
        self.Histograms += [ defineHistogram('mass', type='TH1F', title="Photon-Muon Mass ",
                                             xbins=60, xmin=0.*GeV, xmax=15.*GeV) ]
