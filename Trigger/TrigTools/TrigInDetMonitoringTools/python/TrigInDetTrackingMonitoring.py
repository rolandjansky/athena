# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

 
class TrigInDetTrackCnvMonitoring(GenericMonitoringTool):
    def __init__ (self, name="TrigInDetTrackCnvMonitoring"):
        super(TrigInDetTrackCnvMonitoring, self).__init__(name)

        #Example:
        #self.defineHistogram('EventStatistics', path='EXPERT', type='TH1F',
        #                                     title="EventStatistics",
        #                                     xbins=9, xmin=-0.5, xmax=8.5, opt='kLBN',
        #                                     #labels='All input : Has input TE : Has ROI : Has tracks : Has seed tracks : Has  enough tracks : Has track Z cluster : Has vertex : Has good vertex' )
        #                                     xlabels= ['All input', 'Has input TE', 'Has ROI',  'Has tracks', 'Has seed tracks',  'Has  enough tracks', 'Has track Z cluster', 'Has vertex', 'Has good vertex'] )
      #-------------------------------------------------------------------------------------------------
      #                             Monitoring track parameters

       #TODO need to revisit binning with higher stats

        self.defineHistogram('TrackPtPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Pt; p_{t} [GeV]; Number of tracks",
                                             xbins=200, xmin=0.0, xmax=1000.0)
        self.defineHistogram('TrackQOverPPass',path='EXPERT', type='TH1F',
                                             title="Acc. Track q/p; q/p; Number of tracks",
                                             xbins=1000, xmin=-500., xmax=500.0)
        self.defineHistogram('TrackEtaPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Eta; #eta; Number of tracks",
                                             xbins=50, xmin=-2.5, xmax=2.5)
        self.defineHistogram('TrackPhiPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Phi; #phi; Number of tracks",
                                             xbins=70, xmin=-3.5, xmax=3.5)
        self.defineHistogram('TrackThetaPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Theta; #theta; Number of tracks",
                                             xbins=70, xmin=-3.5, xmax=3.5)
        self.defineHistogram('TrackZ0Pass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Z0; Track z0 [mm]; Number of tracks",
                                             xbins=400, xmin=-20.0, xmax=20.0)
        self.defineHistogram('TrackD0Pass', path='EXPERT', type='TH1F',
                                             title="Acc. Track D0; Track d0 [mm]; Number of tracks",
                                             xbins=400, xmin=-20.0, xmax=20.0)
        #self.defineHistogram('TrackZ0errPass', path='EXPERT', type='TH1F',
        #                                     title="Acc. Track Z0err; Track z0 error [mm]; Number of tracks",
        #                                     xbins=100, xmin=0., xmax=5.)
        #self.defineHistogram('TrackD0errPass', path='EXPERT', type='TH1F',
        #                                     title="Acc. Track D0err; Track d0 error [mm]; Number of tracks",
        #                                     xbins=100, xmin=0., xmax=5.)
        #self.defineHistogram('TrackQualPass', path='EXPERT', type='TH1F',
        #                                     title="Acc. Track Qual; Track #chi^{2}/ndf; Number of tracks",
        #                                     xbins=50, xmin=0., xmax=10.)
        self.defineHistogram('TrackNDFPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track NDF; Track NDF; Number of tracks",
                                             xbins=10, xmin=-0.5, xmax=9.5)
        self.defineHistogram('TrackChi2ProbPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track #chi^{2} probability; Track #chi^{2} probability; Number of tracks",
                                             xbins=70, xmin=-0.2, xmax=1.2)
        #Accepted Track hits in ID
        self.defineHistogram('TrackTRTHitsPass', path='EXPERT', type='TH1I',
                                             title="Acc. Track TRT hits; N TRT hits; Number of tracks",
                                             xbins=50, xmin=0, xmax=50)
        self.defineHistogram('TrackPIXHitsPass', path='EXPERT', type='TH1I',
                                             title="Acc. Track PIX hits; N PIX hits; Number of tracks",
                                             xbins=8, xmin=0, xmax=8)
        self.defineHistogram('TrackSCTHitsPass', path='EXPERT', type='TH1I',
                                             title="Acc. Track SCT hits; N SCT hits; Number of tracks",
                                             xbins=13, xmin=0, xmax=13)


