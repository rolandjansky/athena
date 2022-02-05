# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
# Author: Laurynas Mince
# Created on 15.10.2019

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def MuPatTrackBuilderMonitoring(name = "MuPatTrackBuilderMonitoring"):

    montool = GenericMonitoringTool(name, HistPath = name)

    montool.defineHistogram("mstrks_n", type="TH1F", path="EXPERT", title="MS track n; N of tracks", xbins=50, xmin=0, xmax=50)
    montool.defineHistogram("mstrks_pt", type="TH1F", path="EXPERT", title="MS track pT; pT", xbins=100, xmin=0, xmax=300)
    montool.defineHistogram("mstrks_eta", type="TH1F", path="EXPERT", title="MS track eta; #eta", xbins=50, xmin=-5, xmax=5)
    montool.defineHistogram("mstrks_phi", type="TH1F", path="EXPERT", title="MS track phi; #phi", xbins=40, xmin=-3.2, xmax=3.2)
    montool.defineHistogram("mstrks_eta,mstrks_phi", type="TH2F", path="EXPERT", title="MS track #eta vs. #phi; #eta; #phi", xbins=50, xmin=-5, xmax=5, ybins=40, ymin=-3.2, ymax=3.2)

    montool.defineHistogram("mssegs_n", type="TH1F", path="EXPERT", title="MS segment n; N of segments", xbins=50, xmin=0, xmax=50)
    montool.defineHistogram("mssegs_eta", type="TH1F", path="EXPERT", title="MS segment eta; #eta", xbins=50, xmin=-5, xmax=5)
    montool.defineHistogram("mssegs_phi", type="TH1F", path="EXPERT", title="MS segment phi; #phi", xbins=40, xmin=-3.2, xmax=3.2)

    return montool
