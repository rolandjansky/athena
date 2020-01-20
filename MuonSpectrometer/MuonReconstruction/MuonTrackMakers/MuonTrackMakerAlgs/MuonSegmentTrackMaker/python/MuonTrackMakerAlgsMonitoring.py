# Author: Laurynas Mince
# Created on 15.10.2019

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class MuPatTrackBuilderMonitoring(GenericMonitoringTool):
    def __init__ (self, name="MuPatTrackBuilderMonitoring"):
        super(MuPatTrackBuilderMonitoring, self).__init__(name)

        self.HistPath = name
        self.defineHistogram("mstrks_n", type="TH1F", path="EXPERT", title="MS track n", xbins=50, xmin=0, xmax=50)
        self.defineHistogram("mstrks_pt", type="TH1F", path="EXPERT", title="MS track pT", xbins=100, xmin=0, xmax=300)
        self.defineHistogram("mstrks_eta", type="TH1F", path="EXPERT", title="MS track eta", xbins=50, xmin=-5, xmax=5)
        self.defineHistogram("mstrks_phi", type="TH1F", path="EXPERT", title="MS track phi", xbins=50, xmin=-5, xmax=5)
    
        self.defineHistogram("mssegs_n", type="TH1F", path="EXPERT", title="MS segment n", xbins=50, xmin=0, xmax=50)
        self.defineHistogram("mssegs_eta", type="TH1F", path="EXPERT", title="MS segment eta", xbins=50, xmin=-5, xmax=5)
        self.defineHistogram("mssegs_phi", type="TH1F", path="EXPERT", title="MS segment phi", xbins=50, xmin=-5, xmax=5)
