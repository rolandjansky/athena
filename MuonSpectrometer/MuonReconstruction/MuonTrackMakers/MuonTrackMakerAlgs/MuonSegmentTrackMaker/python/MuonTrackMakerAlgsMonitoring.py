# Author: LAurynas Mince
# Created on 15.10.2019

from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool

class MuPatTrackBuilderMonitoring(GenericMonitoringTool):
    def __init__ (self, name="MuPatTrackBuilderMonitoring"):
        super(MuPatTrackBuilderMonitoring, self).__init__(name)

        self.HistPath = name
        self.defineHistogram("mstrks_n", type="TH1F", path="EXPERT", title="MS track n", xbins=50, xmin=0, xmax=50)
        self.defineHistogram("mstrks_pt", type="TH1F", path="EXPERT", title="MS track pT", xbins=100, xmin=0, xmax=300))
