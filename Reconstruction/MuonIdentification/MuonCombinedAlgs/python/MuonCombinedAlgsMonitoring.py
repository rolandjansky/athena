# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# Author: Laurynas Mince
# Created on 26.07.2019

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class MuonCreatorAlgMonitoring(GenericMonitoringTool):
    def __init__ (self, name="MuonCreatorAlgMonitoring"):
        super(MuonCreatorAlgMonitoring, self).__init__(name)

        self.HistPath = name
        self.defineHistogram( "muon_n", type="TH1F", path="EXPERT", title="Number of muons; N of muons", xbins=50, xmin=0, xmax=50)
        self.defineHistogram( "muon_pt", type="TH1F", path="EXPERT", title="Muon pT; p_T", xbins=100, xmin=0, xmax=300)
        self.defineHistogram( "muon_eta", type="TH1F", path="EXPERT", title="Muon eta; #eta", xbins=40, xmin=-3, xmax=3)
        self.defineHistogram( "muon_phi", type="TH1F", path="EXPERT", title="Muon phi; #phi", xbins=40, xmin=-3.2, xmax=3.2)

        # Turn-off SA track histograms for inside-out muon reconstruction
        if not ("InsideOut" in name or "Late" in name):
            self.defineHistogram( "satrks_n", type="TH1F", path="EXPERT", title="Number of SA tracks; N of tracks", xbins=50, xmin=0, xmax=50)
            self.defineHistogram( "satrks_pt", type="TH1F", path="EXPERT", title="Extrapolated Trk pT; p_T", xbins=100, xmin=0, xmax=300)
            self.defineHistogram( "satrks_eta", type="TH1F", path="EXPERT", title="Extrapolated Trk eta; #eta", xbins=40, xmin=-3, xmax=3)
            self.defineHistogram( "satrks_phi", type="TH1F", path="EXPERT", title="Extrapolated Trk phi; #phi", xbins=40, xmin=-3.2, xmax=3.2)

        if "CB" in name or "Late" in name or "InsideOut" in name:
            self.defineHistogram( "cbtrks_n", type="TH1F", path="EXPERT", title="Number of CB tracks; N of tracks", xbins=50, xmin=0, xmax=50)
            self.defineHistogram( "cbtrks_pt", type="TH1F", path="EXPERT", title="Combined Trk pT; p_T", xbins=100, xmin=0, xmax=300)
            self.defineHistogram( "cbtrks_eta", type="TH1F", path="EXPERT", title="Combined Trk eta; #eta", xbins=40, xmin=-3, xmax=3)
            self.defineHistogram( "cbtrks_phi", type="TH1F", path="EXPERT", title="Combined Trk phi; #phi", xbins=40, xmin=-3.2, xmax=3.2)
            
            self.defineHistogram( "idtrks_n", type="TH1F", path="EXPERT", title="Number of ID tracks; N of tracks", xbins=50, xmin=0, xmax=50)
            self.defineHistogram( "idtrks_pt", type="TH1F", path="EXPERT", title="ID tracks pT; p_T", xbins=100, xmin=0, xmax=300)
            self.defineHistogram( "idtrks_eta", type="TH1F", path="EXPERT", title="ID tracks eta; #eta", xbins=40, xmin=-3, xmax=3)
            self.defineHistogram( "idtrks_phi", type="TH1F", path="EXPERT", title="ID tracks phi; #phi", xbins=40, xmin=-3.2, xmax=3.2)
            self.defineHistogram( "idtrks_pt,idtrks_eta", type="TH2F", path="EXPERT", title="ID tracks pT vs. #eta; p_T; #eta", xbins=100, xmin=0, xmax=300, ybins=50, ymin=-5, ymax=5)
