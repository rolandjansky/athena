# Author: Laurynas Mince
# Created on 26.07.2019

from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool

class MuonCreatorAlgMonitoring(GenericMonitoringTool):
    def __init__ (self, name="MuonCreatorAlgMonitoring"):
        super(MuonCreatorAlgMonitoring, self).__init__(name)

        self.HistPath = name
        self.defineHistogram( "muon_pt", type="TH1F", path="EXPERT", title="Muon pT", xbins=100, xmin=0, xmax=500)
        self.defineHistogram( "muon_eta", type="TH1F", path="EXPERT", title="Muon eta", xbins=100, xmin=0, xmax=500)
        self.defineHistogram( "muon_phi", type="TH1F", path="EXPERT", title="Muon phi", xbins=100, xmin=0, xmax=500)

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class MuonCreatorAlgValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MuonCreatorAlgValidationMonitoring"):
        super(MuonCreatorAlgValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms += [defineHistogram("muon_pt", type="TH1F", title="Muon pT", xbins=100, xmin=0, xmax=500)]
        self.Histograms += [defineHistogram("muon_eta", type="TH1F", title="Muon eta", xbins=100, xmin=0, xmax=500)]
        self.Histograms += [defineHistogram("muon_phi", type="TH1F", title="Muon phi", xbins=100, xmin=0, xmax=500)]
