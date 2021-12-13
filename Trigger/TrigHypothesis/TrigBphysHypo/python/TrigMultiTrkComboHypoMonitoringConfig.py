# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

def TrigMultiTrkComboHypoMonitoring(name):

    montool = GenericMonitoringTool(name)

    if 'Streamer' in name:
        montool.Histograms = [
        defineHistogram('nAcceptedTrk', type='TH1F', path='EXPERT', title="number of selected input tracks", xbins=100, xmin=0, xmax=100),
        defineHistogram('nVertexFitterCalls', type='TH1F', path='EXPERT', title="number of calls for vertex fit", xbins=200, xmin=0, xmax=200),
        defineHistogram('acceptance', type='TH1F',path='EXPERT', title="filter acceptance", xbins=2, xmin=0, xmax=2),
        defineHistogram('TIME_all', type='TH1F', path='EXPERT', title='execution time; [microseconds]', xbins=100, xmin=0, xmax=1000),
        ]
    else:
        montool.Histograms = [
        defineHistogram('nAcceptedTrk', type='TH1F', path='EXPERT', title="number of selected input tracks", xbins=100, xmin=0, xmax=100),
        defineHistogram('nCombination', type='TH1F',path='EXPERT', title="number of track combinations before mass preselection", xbins=100, xmin=0, xmax=100),
        defineHistogram('nCombinationBeforeFit', type='TH1F', path='EXPERT', title="number of inputs to the vertex fitter", xbins=100, xmin=0, xmax=100),
        defineHistogram('nBPhysObject', type='TH1F', path='EXPERT', title="number of fitted BPhysObjects", xbins=100, xmin=0, xmax=100),
        defineHistogram('trkMassBeforeFit', type='TH1F', path='EXPERT', title="mass of track combinations BEFORE fit [GeV]", xbins=200, xmin=0, xmax=100),
        defineHistogram('bphysChi2', type='TH1F', path='EXPERT', title="chi2 fit of N tracks; fit chi2 of N selected tracks", xbins=100, xmin=0, xmax=100),
        defineHistogram('bphysFitMass', type='TH1F', path='EXPERT', title="fit mass of N tracks; fit mass of N selected tracks [GeV]", xbins=100, xmin=0, xmax=20),
        defineHistogram('bphysMass', type='TH1F', path='EXPERT', title="mass of N tracks; mass of N selected tracks [GeV]", xbins=100, xmin=0, xmax=20),
        defineHistogram('bphysCharge', type='TH1F', path='EXPERT', title="total charge of N tracks; Total Charge", xbins=20, xmin=-10, xmax=10),
        defineHistogram('bphysLxy', type='TH1F', path='EXPERT', title="Lxy of Vertex; [mm]", xbins=30, xmin=-10, xmax=20),
        defineHistogram('TIME_all', type='TH1F', path='EXPERT', title='execution time; [microseconds]', xbins=100, xmin=0, xmax=1000),
        defineHistogram('bphysd0_trk1', type='TH1F', path='EXPERT', title="d0 of Vertex first track; [mm]", xbins=30, xmin=-10, xmax=10),
        defineHistogram('bphysd0_trk2', type='TH1F', path='EXPERT', title="d0 of Vertex second track; [mm]", xbins=30, xmin=-10, xmax=10),
        defineHistogram('bphysPt_trk1', type='TH1F', path='EXPERT', title="p_{T} of Vertex first track; [GeV]", xbins=60, xmin=0, xmax=60),
        defineHistogram('bphysPt_trk2', type='TH1F', path='EXPERT', title="p_{T} of Vertex second track; [GeV]", xbins=60, xmin=0, xmax=60),
        defineHistogram('bphysEtatrack1', type='TH1F', path='EXPERT', title="Eta of Vertex first track; [rad]", xbins=50, xmin=-3.15, xmax=3.15),
        defineHistogram('bphysEtatrack2', type='TH1F', path='EXPERT', title="Eta of Vertex second track; [rad]", xbins=50, xmin=-3.15, xmax=3.15),
        ]

    return montool


def TrigMultiTrkComboHypoToolMonitoring(name):

    montool = GenericMonitoringTool(name)

    montool.Histograms = [
    defineHistogram('totalCharge', type='TH1F', path='EXPERT', title="Total Charge of N tracks; total charge", xbins=20, xmin=-10, xmax=10),
    defineHistogram('chi2', type='TH1F', path='EXPERT', title="chi2 fit of N tracks; vertex chi2", xbins=100, xmin=0, xmax=100),
    defineHistogram('mass', type='TH1F', path='EXPERT', title="mass of track pairs; m_{#mu#mu} [GeV]", xbins=100, xmin=0, xmax=20),
    defineHistogram('Lxy', type='TH1F', path='EXPERT', title="Lxy of Vertex; [mm]", xbins=30, xmin=-10, xmax=20),
    defineHistogram('pT_trk1', type='TH1F', path='EXPERT', title="p_{T} of the first track; p_{T}(#mu_{1}) [GeV]", xbins=100, xmin=0, xmax=40),
    defineHistogram('pT_trk2', type='TH1F', path='EXPERT', title="p_{T} of the second track; p_{T}(#mu_{2}) [GeV]", xbins=100, xmin=0, xmax=40),
    defineHistogram('d0_trk1', type='TH1F', path='EXPERT', title="d0 of the first track; d0(#mu_{1}) [mm]", xbins=100, xmin=-10, xmax=10),
    defineHistogram('d0_trk2', type='TH1F', path='EXPERT', title="d0 of the second track; d0(#mu_{2}) [mm]", xbins=100, xmin=-10, xmax=10),
    defineHistogram('eta_trk1', type='TH1F', path='EXPERT', title="Eta of Vertex first track; [rad]", xbins=50, xmin=-3.15, xmax=3.15),
    defineHistogram('eta_trk2', type='TH1F', path='EXPERT', title="Eta of Vertex second track; [rad]", xbins=50, xmin=-3.15, xmax=3.15),
    ]

    return montool
