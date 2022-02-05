# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def TrigBmumuxComboHypoMonitoring(name):
    montool = GenericMonitoringTool(name)

    montool.defineHistogram('nDimuon', type='TH1F', path='EXPERT', title="number of fitted dimuon vertices", xbins=10, xmin=0, xmax=10)
    montool.defineHistogram('nTrk', type='TH1F', path='EXPERT', title="number of merged tracks in extended RoIs", xbins=200, xmin=0, xmax=200)
    montool.defineHistogram('nSelectedTrk', type='TH1F', path='EXPERT', title="number of tracks in vicinity of dimuon vertex", xbins=200, xmin=0, xmax=200)
    montool.defineHistogram('nBPhysObject', type='TH1F', path='EXPERT', title="number of fitted BPhysObjects", xbins=100, xmin=0, xmax=100),

    return montool

def TrigBmumuxComboHypoToolMonitoring(name):
    montool = GenericMonitoringTool(name)

    montool.defineHistogram('Chi2', type='TH1F', path='EXPERT', title="chi2 of the fitted vertex", xbins=100, xmin=0, xmax=100)
    montool.defineHistogram('Fitmass', type='TH1F', path='EXPERT', title="mass of BPhys object", xbins=100, xmin=4000, xmax=8000)
    montool.defineHistogram('Mass', type='TH1F', path='EXPERT', title="mass(BPhys object) - mass(dimuon) + PDG::mJpsi", xbins=100, xmin=4000, xmax=8000)
    montool.defineHistogram('Pt', type='TH1F', path='EXPERT', title="p_{T} of BPhys object [MeV]", xbins=100, xmin=0, xmax=40000)
    montool.defineHistogram('Eta', type='TH1F', path='EXPERT', title="#eta_{T} of BPhys object", xbins=100, xmin=-3.15, xmax=3.15)

    return montool
