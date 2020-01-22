# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

class TrigMultiTrkHypoAlgMTMonitoring(GenericMonitoringTool):
    def __init__ (self, name):
        super(TrigMultiTrkHypoAlgMTMonitoring, self).__init__(name)
        self.Histograms = [
        defineHistogram('pairMass', type='TH1F', path='EXPERT', title="mass of track pairs; m_{#mu#mu} [GeV]", xbins=200, xmin=0, xmax=100),
        defineHistogram('nTrk', type='TH1F',path='EXPERT', title="number of tracks input; N tracks", xbins=100, xmin=0, xmax=100),
        defineHistogram('accepted_highptNTrk', type='TH1F', path='EXPERT', title="Number of tracks selected; N selected tracks", xbins=100, xmin=0, xmax=100),
        defineHistogram('nPair', type='TH1F',path='EXPERT', title="number of pairs input; N tracks", xbins=100, xmin=0, xmax=100),
        defineHistogram('accepted_nPair', type='TH1F', path='EXPERT', title="Number of pairs selected; N selected tracks", xbins=100, xmin=0, xmax=100),
        defineHistogram('totCharge', type='TH1F', path='EXPERT', title="Total Charge of tracks", xbins=21, xmin=-10, xmax=10),

        defineHistogram('nTrkMass', type='TH1F', path='EXPERT', title="mass of N tracks; mass of N selected tracks [GeV]", xbins=100, xmin=0, xmax=20),
        defineHistogram('nTrkFitMass', type='TH1F', path='EXPERT', title="fit mass of N tracks; fit mass of N selected tracks", xbins=100, xmin=0, xmax=20),
        defineHistogram('nTrkChi2', type='TH1F', path='EXPERT', title="chi2 fit of N tracks; fit chi2 of N selected tracks", xbins=100, xmin=0, xmax=100),
        ]

class TrigMultiTrkHypoToolMTMonitoring(GenericMonitoringTool):
    def __init__ (self, name):
        super(TrigMultiTrkHypoToolMTMonitoring, self).__init__(name)
        self.Histograms = [
        defineHistogram('totCharge', type='TH1F', path='EXPERT', title="Total Charge of tracks", xbins=21, xmin=-10, xmax=10),
        defineHistogram('CutCounter', type='TH1F', path='EXPERT', title="mass of track pairs; m_{#mu#mu} [GeV]", xbins=5, xmin=-0.5, xmax=4.5),
        defineHistogram('FitChi2', type='TH1F', path='EXPERT', title="chi2 fit of N tracks; fit chi2 of N selected tracks", xbins=100, xmin=0, xmax=100),
        defineHistogram('VertexMass', type='TH1F', path='EXPERT', title="Number of tracks selected; N selected tracks", xbins=100, xmin=0, xmax=20),
        defineHistogram('trackPts', type='TH1F', path='EXPERT', title="pair mass of N tracks; pair mass of N selected tracks [GeV]", xbins=100, xmin=0, xmax=20)
        ]
