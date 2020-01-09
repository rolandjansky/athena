# Author: Laurynas Mince
# Created on 26.07.2019

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class MuonCreatorAlgMonitoring(GenericMonitoringTool):
    def __init__ (self, name="MuonCreatorAlgMonitoring"):
        super(MuonCreatorAlgMonitoring, self).__init__(name)

        self.HistPath = name
        self.defineHistogram( "muon_n", type="TH1F", path="EXPERT", title="Number of muons", xbins=50, xmin=0, xmax=50)
        self.defineHistogram( "muon_pt", type="TH1F", path="EXPERT", title="Muon pT", xbins=100, xmin=0, xmax=300)
        self.defineHistogram( "muon_eta", type="TH1F", path="EXPERT", title="Muon eta", xbins=50, xmin=-5, xmax=5)
        self.defineHistogram( "muon_phi", type="TH1F", path="EXPERT", title="Muon phi", xbins=50, xmin=-5, xmax=5)

        self.defineHistogram( "satrks_n", type="TH1F", path="EXPERT", title="Number of SA tracks", xbins=50, xmin=0, xmax=50)
        self.defineHistogram( "satrks_pt", type="TH1F", path="EXPERT", title="Extrapolated Trk pT", xbins=100, xmin=0, xmax=300)
        self.defineHistogram( "satrks_eta", type="TH1F", path="EXPERT", title="Extrapolated Trk eta", xbins=50, xmin=-5, xmax=5)
        self.defineHistogram( "satrks_phi", type="TH1F", path="EXPERT", title="Extrapolated Trk phi", xbins=50, xmin=-5, xmax=5)

        if "CB" in name:
            self.defineHistogram( "cbtrks_n", type="TH1F", path="EXPERT", title="Number of CB tracks", xbins=50, xmin=0, xmax=50)
            self.defineHistogram( "cbtrks_pt", type="TH1F", path="EXPERT", title="Combined Trk pT", xbins=100, xmin=0, xmax=300)
            self.defineHistogram( "cbtrks_eta", type="TH1F", path="EXPERT", title="Combined Trk eta", xbins=50, xmin=-5, xmax=5)
            self.defineHistogram( "cbtrks_phi", type="TH1F", path="EXPERT", title="Combined Trk phi", xbins=50, xmin=-5, xmax=5)
