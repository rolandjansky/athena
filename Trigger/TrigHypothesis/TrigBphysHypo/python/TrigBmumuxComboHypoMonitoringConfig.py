# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

class TrigBmumuxComboHypoMonitoring(GenericMonitoringTool):
    def __init__ (self, name):
        super(TrigBmumuxComboHypoMonitoring, self).__init__(name)
        self.Histograms = [
        defineHistogram('nDimuon', type='TH1F', path='EXPERT', title="number of fitted dimuon vertices", xbins=10, xmin=0, xmax=10),
        defineHistogram('nTrk', type='TH1F', path='EXPERT', title="number of merged tracks in extended RoIs", xbins=200, xmin=0, xmax=200),
        defineHistogram('nSelectedTrk', type='TH1F', path='EXPERT', title="number of tracks in vicinity of dimuon vertex", xbins=200, xmin=0, xmax=200),
        defineHistogram('nBPhysObject', type='TH1F', path='EXPERT', title="number of fitted BPhysObjects", xbins=100, xmin=0, xmax=100),
        ]

class TrigBmumuxComboHypoToolMonitoring(GenericMonitoringTool):
    def __init__ (self, name):
        super(TrigBmumuxComboHypoToolMonitoring, self).__init__(name)
        self.Histograms = [
        defineHistogram('Chi2', type='TH1F', path='EXPERT', title="chi2 of the fitted vertex", xbins=100, xmin=0, xmax=100),
        defineHistogram('Fitmass', type='TH1F', path='EXPERT', title="mass of BPhys object", xbins=100, xmin=4000, xmax=8000),
        defineHistogram('Mass', type='TH1F', path='EXPERT', title="mass(BPhys object) - mass(dimuon) + PDG::mJpsi", xbins=100, xmin=4000, xmax=8000),
        defineHistogram('Pt', type='TH1F', path='EXPERT', title="p_{T} of BPhys object", xbins=100, xmin=0, xmax=40000),
        ]
