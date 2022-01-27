# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

 
def TrigInDetTrackCnvMonitoring(name="TrigInDetTrackCnvMonitoring"):

    montool = GenericMonitoringTool(name)

    #TODO need to revisit binning with higher stats
    if 'minBias' in name or 'MinBias' in name:
        montool.defineHistogram('TrackPtPass', path='EXPERT', type='TH1F',
                                         title="Acc. Track Pt; p_{t} [GeV]; Number of tracks",
                                         xbins=200, xmin=0, xmax=20)
        montool.defineHistogram('TrackQPtPass', path='EXPERT', type='TH1F',
                                         title="Acc. Track q*Pt; q*pt [GeV]; Number of tracks",
                                         xbins=400, xmin=-40, xmax=40)
    else:
        montool.defineHistogram('TrackPtPass', path='EXPERT', type='TH1F',
                                         title="Acc. Track Pt; p_{t} [GeV]; Number of tracks",
                                         xbins=200, xmin=0, xmax=1000)
        montool.defineHistogram('TrackQPtPass', path='EXPERT', type='TH1F',
                                         title="Acc. Track q*Pt; q*pt [GeV]; Number of tracks",
                                         xbins=400, xmin=-1000, xmax=1000)
    #
    montool.defineHistogram('TrackQOverPPass',path='EXPERT', type='TH1F',
                                         title="Acc. Track q/p; q/p [GeV^{-1}]; Number of tracks",
                                         xbins=1000, xmin=-10., xmax=10.0)
    montool.defineHistogram('TrackEtaPass', path='EXPERT', type='TH1F',
                                         title="Acc. Track Eta; #eta; Number of tracks",
                                         xbins=50, xmin=-2.5, xmax=2.5)
    montool.defineHistogram('TrackPhiPass', path='EXPERT', type='TH1F',
                                         title="Acc. Track Phi; #phi; Number of tracks",
                                         xbins=64, xmin=-3.2, xmax=3.2)
    montool.defineHistogram('TrackThetaPass', path='EXPERT', type='TH1F',
                                         title="Acc. Track Theta; #theta; Number of tracks",
                                         xbins=64, xmin=0., xmax=3.2)
    montool.defineHistogram('TrackZ0Pass', path='EXPERT', type='TH1F',
                                         title="Acc. Track Z0; Track z0 [mm]; Number of tracks",
                                         xbins=300, xmin=-300.0, xmax=300.0)
    montool.defineHistogram('TrackD0Pass', path='EXPERT', type='TH1F',
                                         title="Acc. Track D0; Track d0 [mm]; Number of tracks",
                                         xbins=300, xmin=-300.0, xmax=300.0)
    #montool.defineHistogram('TrackZ0errPass', path='EXPERT', type='TH1F',
    #                                     title="Acc. Track Z0err; Track z0 error [mm]; Number of tracks",
    #                                     xbins=100, xmin=0., xmax=5.)
    #montool.defineHistogram('TrackD0errPass', path='EXPERT', type='TH1F',
    #                                     title="Acc. Track D0err; Track d0 error [mm]; Number of tracks",
    #                                     xbins=100, xmin=0., xmax=5.)
    #montool.defineHistogram('TrackQualPass', path='EXPERT', type='TH1F',
    #                                     title="Acc. Track Qual; Track #chi^{2}/ndf; Number of tracks",
    #                                     xbins=50, xmin=0., xmax=10.)
    montool.defineHistogram('TrackNDFPass', path='EXPERT', type='TH1F',
                                         title="Acc. Track NDF; Track NDF; Number of tracks",
                                         xbins=10, xmin=-0.5, xmax=9.5)
    montool.defineHistogram('TrackChi2ProbPass', path='EXPERT', type='TH1F',
                                         title="Acc. Track #chi^{2} probability; Track #chi^{2} probability; Number of tracks",
                                         xbins=70, xmin=-0.2, xmax=1.2)
    #Accepted Track hits in ID
    montool.defineHistogram('TrackTRTHitsPass', path='EXPERT', type='TH1I',
                                         title="Acc. Track TRT hits; N TRT hits; Number of tracks",
                                         xbins=71, xmin=-0.5, xmax=70.5)
    montool.defineHistogram('TrackPIXHitsPass', path='EXPERT', type='TH1I',
                                         title="Acc. Track PIX hits; N PIX hits; Number of tracks",
                                         xbins=13, xmin=-0.5, xmax=12.5)
    montool.defineHistogram('TrackSCTHitsPass', path='EXPERT', type='TH1I',
                                         title="Acc. Track SCT hits; N SCT hits; Number of tracks",
                                         xbins=21, xmin=-0.5, xmax=20.5)
    # Track counting
    montool.defineHistogram('TrackCountingPass', path='EXPERT', type='TH1I',
                                         title="Track Counting; Number of tracks per event; Count",
                                         xbins=300, xmin=-0.5, xmax=299.5)

    return montool
