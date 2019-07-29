# Author: Laurynas Mince
# Created on 26.07.2019

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class MuonCreatorAlgOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MuonCreatorAlgOnlineMonitoring"):
        super(MuonCreatorAlgOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [defineHistogram("muon_pt", type="TH1F", title="Muon pT", xbins=100, xmin=0, xmax=500)]

class MuonCreatorAlgValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MuonCreatorAlgValidationMonitoring"):
        super(MuonCreatorAlgValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [defineHistogram("muon_pt", type="TH1F", title="Muon pT", xbins=100, xmin=0, xmax=500)]