# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def TrigL2MuonSAMonitoring(name = "TrigL2MuonSAMonitoring"):

    montool = GenericMonitoringTool(name, HistPath = name)

    montool.defineHistogram('InnMdtHits',    type='TH1F', path='EXPERT', title="Hit multiplicity in the INNER2 road; MDT hits", xbins=50, xmin=-0.5, xmax=50.5)
    montool.defineHistogram('MidMdtHits',    type='TH1F', path='EXPERT', title="Hit multiplicity in the MIDDLE road; MDT hits", xbins=50, xmin=-0.5, xmax=50.5)
    montool.defineHistogram('OutMdtHits',    type='TH1F', path='EXPERT', title="Hit multiplicity in the OUTER road; MDT hits", xbins=50, xmin=-0.5, xmax=50.5)
    montool.defineHistogram('FitResiduals',  type='TH1F', path='EXPERT', title="Fit Residual; Residuals (cm)", xbins=400, xmin=-0.4, xmax=0.4)
    montool.defineHistogram('Efficiency',    type='TH1F', path='EXPERT', title="Track finding efficiency", xbins=2, xmin=-0.5, xmax=1.5)
    montool.defineHistogram('Address',       type='TH1F', path='EXPERT', title="S_address;S_address", xbins=5, xmin=-1.5, xmax=3.5 )
    montool.defineHistogram('AbsPt',         type='TH1F', path='EXPERT', title="absolute P_{T};P_{T} (GeV)", xbins=100, xmin=0, xmax=100 )
    montool.defineHistogram('TrackPt',       type='TH1F', path='EXPERT', title="P_{T};P_{T} (GeV)", xbins=100, xmin=-100, xmax=100 )
    montool.defineHistogram('AbsPt, SagInv', type='TH2F', path='EXPERT', title="1/s as a function of P_{T}; P_{T} (GeV); 1/s (cm^{-1})", xbins=50, xmin=0, xmax=100, ybins=15, ymin=0, ymax=3 )
    montool.defineHistogram('Sagitta',       type='TH1F', path='EXPERT', title="Reconstructed Sagitta; Sagitta (cm)", xbins=100, xmin=-10., xmax=10.)
    montool.defineHistogram('ResInner',      type='TH1F', path='EXPERT', title="Residual from Trigger track in INNER Station; Residuals (cm)", xbins=100, xmin=-10., xmax=10. )
    montool.defineHistogram('ResMiddle',     type='TH1F', path='EXPERT', title="Residual from Trigger track in MIDDLE Station; Residuals (cm)", xbins=100, xmin=-10., xmax=10. )
    montool.defineHistogram('ResOuter',      type='TH1F', path='EXPERT', title="Residual from Trigger track in OUTER Station; Residuals (cm)", xbins=100, xmin=-10., xmax=10. )
    montool.defineHistogram('TrackEta, TrackPhi',         type='TH2F', path='EXPERT', title="Distribution of reconstructed LVL2 tracks; Eta; Phi", xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 )
    montool.defineHistogram('FailedRoIEta, FailedRoIPhi', type='TH2F', path='EXPERT', title="Location of LVL2 track failure; Eta; Phi", xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 )
    montool.defineHistogram('TIME_Total',                 type='TH1F', path='EXPERT', title="Total processing time (us)", xbins=100, xmin=0, xmax=100000 )
    montool.defineHistogram('TIME_Data_Preparator',       type='TH1F', path='EXPERT', title="Data preparator time (us)", xbins=100, xmin=0, xmax=50000 )
    montool.defineHistogram('TIME_Pattern_Finder',        type='TH1F', path='EXPERT', title="Pattern finder time (us)", xbins=100, xmin=0, xmax=5000 )
    montool.defineHistogram('TIME_Station_Fitter',        type='TH1F', path='EXPERT', title="Station fitter time (us)", xbins=100, xmin=0, xmax=5000 )
    montool.defineHistogram('TIME_Track_Fitter',          type='TH1F', path='EXPERT', title="Track fitter time (us)", xbins=100, xmin=0, xmax=300 )
    montool.defineHistogram('TIME_Track_Extrapolator',    type='TH1F', path='EXPERT', title="Track extrapolator time (us)", xbins=100, xmin=0, xmax=300 )
    montool.defineHistogram('TIME_Calibration_Streamer',  type='TH1F', path='EXPERT', title="Calibration streamer time (us)", xbins=100, xmin=0, xmax=50000 )
    montool.defineHistogram('InvalidRpcRoINumber',        type='TH1F', path='EXPERT', title="RoI Number of Invalid RPC RoI; RoI Number", xbins=150, xmin=-0.5, xmax=150.5)

    return montool
