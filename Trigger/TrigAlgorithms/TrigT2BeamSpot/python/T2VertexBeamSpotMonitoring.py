# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool


class BaseMonitoringTool(GenericMonitoringTool):
    """Base class which defines few useful methods to cope with defineHistogram
    madness.
    """

    def makeHisto1D(self, name, type, xbins, xmin, xmax, title, path='EXPERT', opt=None, **kw):
        self.defineHistogram(
            name, path=path, type=type, title=title, opt=opt,
            xbins=xbins, xmin=xmin, xmax=xmax, **kw
        )

    def makeLBNHisto1D(self, name, type, xbins, xmin, xmax, title, path='EXPERT', opt="", **kw):
        opt = "kLBNHistoryDepth=1 " + opt if opt else "kLBNHistoryDepth=1"
        self.makeHisto1D(
            name, type, xbins, xmin, xmax, title, path=path, opt=opt, **kw,
        )

    def makeHisto2D(self, nameX, nameY, type, xbins, xmin, xmax,
                    ybins, ymin, ymax, title, path='EXPERT', opt=None, **kw):
        name = ", ".join([nameX, nameY])
        self.defineHistogram(
            name, path=path, type=type, title=title, opt=opt,
            xbins=xbins, xmin=xmin, xmax=xmax,
            ybins=ybins, ymin=ymin, ymax=ymax,
            **kw
        )

    def makeLBNHisto2D(self, nameX, nameY, type, xbins, xmin, xmax,
                       ybins, ymin, ymax, title, path='EXPERT', opt="", **kw):
        opt = "kLBNHistoryDepth=1 " + opt if opt else "kLBNHistoryDepth=1"
        self.makeHisto2D(
            nameX, nameY, type, xbins, xmin, xmax, ybins, ymin, ymax, title,
            path=path, opt=opt, **kw,
        )

    def makeProfile(self, nameX, nameY, xbins, xmin, xmax, title, path='EXPERT', opt=None, **kw):
        name = ", ".join([nameX, nameY])
        self.defineHistogram(
            name, path=path, type="TProfile", title=title, opt=opt,
            xbins=xbins, xmin=xmin, xmax=xmax, **kw,
        )

    def makeLBNProfile(self, nameX, nameY, xbins, xmin, xmax, title, path='EXPERT', opt="", **kw):
        opt = "kLBNHistoryDepth=1 " + opt if opt else "kLBNHistoryDepth=1"
        self.makeProfile(
            nameX, nameY, xbins, xmin, xmax, title, path=path, opt=opt, **kw,
        )


class T2VertexBeamSpotMonitoring(BaseMonitoringTool):
    def __init__ (self, name="T2VertexBeamSpotMonitoring"):
        super(T2VertexBeamSpotMonitoring, self).__init__(name)

        # monitored variables for hltExecute():
        #  - nTotalTracks
        #  - nTotalPassedTracks
        #  - nTotalHighPTTracks
        #  - nPerVecTETracks
        #  - nPerVecTEPassedTracks
        #  - nPerVecTEHighPTTracks
        #  - nPerTETracks
        #  - nPerTEPassedTracks
        #  - nPerTEHighPTTracks
        #  - TIME_TotalTime
        #  - TIME_EachTE
        #  - TIME_LoopAllTEs
        #  - TIME_toCreateOutput
        #
        # monitored variables for execute():
        #  - nTotalTracks
        #  - nTotalPassedTracks
        #  - nTotalHighPTTracks
        #  - TIME_TotalTime
        #  - TIME_SelectingTracks
        #  - TIME_ReconstructVertices

        self.makeHisto1D('nTotalTracks', 'TH1F', 100, 0., 2000.,
                         title="nTotalTracks; N all tracks per event; Number of events")

        self.makeHisto1D('nTotalPassedTracks', 'TH1F', 100, 0., 1000.,
                         title="nTotalPassedTracks; N selected tracks per event; Number of events")

        self.makeHisto1D('nTotalHighPTTracks', 'TH1F', 100, 0., 1000.,
                         title="nTotalHighPTTracks; N all tracks per event; Number of events")

        # These are not useful in online
        # self.makeHisto1D('nPerVecTETracks', 'TH1F', 100, 0., 1000.,
        #                  title="nPerVecTETracks; N all tracks per event; Number of events")

        # self.makeHisto1D('nPerVecTEPassedTracks', 'TH1F', 100, 0., 1000.,
        #                  title="nPerVecTEPassedTracks; N selected tracks per event; Number of events")

        # self.makeHisto1D('nPerVecTEHighPTTracks', 'TH1F', 100, 0., 1000.,
        #                  title="nPerVecTEHighPTTracks; N all tracks per event; Number of events")

        # self.makeHisto1D('nPerTETracks', 'TH1F', 100, 0., 1000.,
        #                  title="nPerTETracks; N all tracks per event; Number of events")

        # self.makeHisto1D('nPerTEPassedTracks', 'TH1F', 100, 0., 1000.,
        #                  title="nPerTEPassedTracks; N selected tracks per event; Number of events")

        # self.makeHisto1D('nPerTEHighPTTracks', 'TH1F', 100, 0., 1000.,
        #                  title="nPerTEHighPTTracks; N all tracks per event; Number of events")

        # --------------- Timing histograms -----------------
        timers = ["TotalTime", "LoopAllTEs", "EachTE", "toCreateOutput", "SelectingTracks", "ReconstructVertices"]
        for timer in timers:
            self.makeHisto1D('TIME_' + timer, 'TH1I', 50, 0, 25000,
                             title="Timing " + timer + "; time [#mus];")


class T2VertexBeamSpotToolMonitoring(BaseMonitoringTool):
    def __init__ (self, name="T2VertexBeamSpotToolMonitoring", detail=2):
        """
        Parameters
        ----------
        detail : int
            Selects which level of detail to monitor. 0 will produce minimal
            set of histograms. 1 will make a reasonable set of histograms for
            online running. 2 (default) will make all possible histograms,
            this should be suitable for offline.
        """
        super(T2VertexBeamSpotToolMonitoring, self).__init__(name)

        self.defineTimingHistos(detail)
        self.defineTrackHistos(detail)
        self.defineClusterHistos(detail)
        self.defineVertexHistos(detail)
        self.defineSplitVertexHistos(detail)

    def defineTimingHistos(self, detail):
        # monitored timers:
        #  - TIME_SplitVertexReconstruction
        #  - TIME_toSortSplitTracks
        #  - TIME_toSortTracks
        #  - TIME_toVertexFitSplit
        #  - TIME_toZCluster
        #  - TIME_toZClusterSplit
        #  - TIME_TrackSelection
        #  - TIME_VertexReconstruction

        timers = [
            ("SplitVertexReconstruction", 10000.),
            ("toSortSplitTracks", 10000.),
            ("toSortTracks", 1000.),
            ("toVertexFitSplit", 10000.),
            ("toZCluster", 100.),
            ("toZClusterSplit", 100.),
            ("TrackSelection", 5000.),
            ("VertexReconstruction", 20000.),
        ]
        for timer, xmax in timers:
            self.makeHisto1D('TIME_' + timer, 'TH1I', 50, 0., xmax,
                             title="Timing " + timer + "; time [#mus];")

    def defineTrackHistos(self, detail):
        # track-related variables
        #  - TracksPerROI
        #  - SelectedTracksPerROI
        #  - SelectedHiPTTracksPerROI
        #  - Track<param>Pass, where <param> is one of the:
        #    - Pt Eta Phi Z0 D0 Z0err D0err NDF Qual Chi2Prob SiHits PIXHits SCTHits TRTHits

        #Track counts  (Per ROI)
        self.makeHisto1D('TracksPerROI', 'TH1F', 100, 0., 2000.,
                         title="TracksPerROI; N tracks per ROI; Number of ROIs")

        self.makeHisto1D('SelectedTracksPerROI', 'TH1F', 100, 0., 1000.,
                         title="TracksPerROIPass; N accepted tracks per ROI; Number of ROIs")

        self.makeHisto1D('SelectedHiPTTracksPerROI', 'TH1F', 100, 0., 1000.,
                         title="HiPtTracksPerROIPass; N accepted High pT tracks per ROI; Number of ROIs")

        # Track d0 vs phi: another method for extracting the beam position on average
        if detail > 0:
            # Profile
            # Total number of bins: 70
            self.makeLBNProfile('TrackPhiPass', 'TrackD0Pass', 70, -3.5, 3.5,
                                title="Selected Track d_{0} vs #phi; Track  #phi; Track d_{0}")

        # ACCEPTED Tracks parameters
        #-------------------------------------------------------------------------------------------------
        if detail > 2:
            self.makeHisto1D('TrackPhiPass', 'TH1F', 70, -3.5, 3.5,
                             title="Acc. Track Phi; #phi; Number of tracks")
            self.makeHisto1D('TrackZ0Pass', 'TH1F', 100, -150.0, 150.0,
                             title="Acc. Track Z0; Track z0 [mm]; Number of tracks")
            self.makeHisto1D('TrackD0Pass', 'TH1F', 100, -10.0, 10.0,
                             title="Acc. Track D0; Track d0 [mm]; Number of tracks")
            self.makeHisto1D('TrackPtPass', 'TH1F', 50, 0.0, 50.0,
                             title="Acc. Track Pt; p_{t} [GeV]; Number of tracks")
            self.makeHisto1D('TrackEtaPass', 'TH1F', 60, -3.0, 3.0,
                             title="Acc. Track Eta; #eta; Number of tracks")
            self.makeHisto1D('TrackZ0errPass', 'TH1F', 100, 0., 5.,
                             title="Acc. Track Z0err; Track z0 error [mm]; Number of tracks")
            self.makeHisto1D('TrackD0errPass', 'TH1F', 100, 0., 2.,
                             title="Acc. Track D0err; Track d0 error [mm]; Number of tracks")
            self.makeHisto1D('TrackNDFPass', 'TH1F', 30, 0., 30.,
                             title="Acc. Track NDF; Track NDF; Number of tracks")
            self.makeHisto1D('TrackQualPass', 'TH1F', 50, 0., 10.,
                             title="Acc. Track Qual; Track #chi^{2}/ndf; Number of tracks")
            self.makeHisto1D('TrackChi2ProbPass', 'TH1F', 60, -0.1, 1.1,
                             title="Acc. Track #chi^{2} probability; Track #chi^{2} probability; Number of tracks")
            #Accepted Track hits in ID
            self.makeHisto1D('TrackSiHitsPass', 'TH1I', 15, -0.5, 14.5,
                             title="Acc. Track Silicon hits; N Si hits; Number of tracks")
            self.makeHisto1D('TrackTRTHitsPass', 'TH1I', 50, -0.5, 49.5,
                             title="Acc. Track TRT hits; N TRT hits; Number of tracks")
            self.makeHisto1D('TrackPIXHitsPass', 'TH1I', 15, -0.5, 14.5,
                             title="Acc. Track PIX hits; N PIX hits; Number of tracks")
            self.makeHisto1D('TrackSCTHitsPass', 'TH1I', 10, -0.5, 9.5,
                             title="Acc. Track SCT hits; N SCT hits; Number of tracks")

    def defineClusterHistos(self, detail):
        # cluster-related variables:
        #  - NClusters
        #  - ClusterZ
        #  - ClusterNTracks
        #  - ClusterNTracksUnused
        #  - ClusterDeltaZ0
        #  - ClusterZ0Pull

        if detail >= 2:
            self.makeHisto1D('NClusters', 'TH1I', 30, 0., 60.,
                             title="NClusters; Number of clusters per event; Number of events")
            self.makeHisto1D('ClusterNTracks', 'TH1I', 100, 0.0, 100.0,
                             title="ClusterNTracks; Number tracks per cluster; Number of events")
            self.makeHisto1D('ClusterNTracksUnused', 'TH1I', 100, 0.0, 400.0,
                             title="ClusterNTracksUnused; Number of unused tracks per cluster; Number of events")
            self.makeHisto2D('ClusterNTracks', 'ClusterNTracksUnused', 'TH2I', 50, 0.0, 50.0, 50, 0.0, 400.0,
                             title="Number of tracks in cluster unused vs used; Number of tracks per cluster; Number of unused tracks per cluster")
            self.makeHisto1D('ClusterZ', 'TH1I', 100, -250.0, 250.0,
                             title="ClusterZ; Cluster Z position [mm]; Number of clusters")
            self.makeHisto1D('ClusterDeltaZ0', 'TH1I', 100, -2.0, 2.0,
                             title="ClusterDeltaZ0; Delta of track Z0 and cluster Z position [mm]; Number of tracks")
            self.makeHisto1D('ClusterZ0Pull', 'TH1I', 100, -10.0, 10.0,
                             title="ClusterZ0Pull; Pull of track Z0 with respect to cluster Z position; Number of tracks")
            self.makeHisto1D('ClusterClusterDeltaZ0', 'TH1I', 200, -100.0, 100.0,
                             title="ClusterClusterDeltaZ0; Delta of cluster-cluster Z position [mm]; Number of clusters")

    def defineVertexHistos(self, detail):

        # vertex-related variables:
        #  - Nvtx
        #  - NvtxPass
        #  - NvtxPassBCID
        #  - ClusterDeltaVertexZ
        #  - Vertex<param>, where <param> is one of
        #    - NTrks SumPt SumPt2 Mass Qual Chi2Prob X Y Z XZoom YZoom ZZoom Xerr Yerr Zerr XY Pull NTrksInVtx
        #  - Vertex<param>Pass, with the same <param>
        #  - Vertex<param>PassBCID, with the same <param>

        # Number of vertices
        self.makeHisto1D('Nvtx', 'TH1F', 50, 0.0, 100.0,
                         title="Nvtx; N vertices per event; Number of events")
        # per-LB
        self.makeLBNHisto1D('NvtxPass', 'TH1I', 50, 0.0, 50.0,
                            title="NvtxPass; N accepted vertices per event; Number of events")

        if detail > 1:
            self.makeHisto1D('ClusterDeltaVertexZ', 'TH1F', 100, -10.0, 10.0,
                             title="ClusterDeltaVertexZ; Delta of cluster Z position and Vertex Z position [mm]; Number of vertices")

        if detail >= 0:
            # Beam position in X, Y, Z: provides mean position and can extract width (convoluted with vertexing precision)
            # Total number of bins: 2500
            # 10 um bins in X and Y
            #  5 mm bins in Z
            self.makeLBNHisto1D('VertexXPass','TH1F', 1000, -5.0, 5.0,
                                title="Acc. Vertex X; Vertex x [mm]; Number of vertices")
            self.makeLBNHisto1D('VertexYPass','TH1F', 1000, -5.0, 5.0,
                                title="Acc. Vertex Y; Vertex y [mm]; Number of vertices")
            self.makeLBNHisto1D('VertexZPass','TH1F', 500, -500.0, 500.0,
                                title="Acc. Vertex Z; Vertex z [mm]; Number of vertices")

            # Beam position in X, Y, Z with respect to the nominal position
            # Total number of bins: 300
            # 10 um bins in X and Y
            #  5 mm bins in Z
            self.makeLBNHisto1D('VertexXZoomPass','TH1F', 100, -0.500, 0.500,
                                title="Acc. Vertex X-X_{nominal}; Vertex X-X_{nominal} [mm]; Number of vertices")
            self.makeLBNHisto1D('VertexYZoomPass','TH1F', 100, -0.500, 0.500,
                                title="Acc. Vertex Y-Y_{nominal}; Vertex Y-Y_{nominal} [mm]; Number of vertices")
            self.makeLBNHisto1D('VertexZZoomPass','TH1F', 100, -250.0, 250.0,
                                title="Acc. Vertex Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Number of vertices")

        if detail >= 1:

            # Beam profile plots: X-Y, X-Z, Y-Z
            # Total number of bins: 2700
            # 100 um bins in X and Y
            self.makeLBNHisto2D('VertexXPass', 'VertexYPass', 'TH2F', 50, -2.5, 2.5, 50, -2.5, 2.5,
                                title="Acc. Vertex Y vs X; Vertex X [mm]; Vertex Y [mm]")
            self.makeLBNProfile('VertexZPass', 'VertexXPass', 100, -250.0, 250.0,
                                title="Acc. Vertex X vs Z; Vertex Z [mm]; Vertex X [mm]")
            self.makeLBNProfile('VertexZPass', 'VertexYPass', 100, -250.0, 250.0,
                                title="Acc. Vertex Y vs Z; Vertex Z [mm]; Vertex Y [mm]")

            # Beam profile w.r.t. the nominal position: X - X_nominal , etc
            # Total number of bins: 2700
            #  50 um bins in X and Y
            self.makeLBNHisto2D('VertexXZoomPass', 'VertexYZoomPass', 'TH2F', 50, -1.25, 1.25, 50, -1.25, 1.25,
                                title="Acc. Vertex Y-Y_{nominal} vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Y-Y_{nominal} [mm]")
            self.makeLBNProfile('VertexZZoomPass', 'VertexXZoomPass', 100, -250.0, 250.0,
                                title="Acc. Vertex X-X_{nominal} vs Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Vertex X-X_{nominal} [mm]")
            self.makeLBNProfile('VertexZZoomPass', 'VertexYZoomPass', 100, -250.0, 250.0,
                                title="Acc. Vertex Y-Y_{nominal} vs Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Vertex Y-Y_{nominal} [mm]")

            # Beam position in X, Y, Z vs. Ntrk in vertex: allows to correct width for each vertex multiplicity
            # Total number of bins: 85,000 -> 105,000
            # 10 um bins in X and Y
            #  2 mm bins in Z
            self.makeLBNHisto2D('VertexNTrksPass', 'VertexXPass', 'TH2F', 100, 0.0, 100.0, 400, -2.00, 2.00,
                                title="Acc. Vertex X vs. NTrks; N trk per vertex; Vertex x [mm]")
            self.makeLBNHisto2D('VertexNTrksPass', 'VertexYPass', 'TH2F', 100, 0.0, 100.0, 400, -2.00, 2.00,
                                title="Acc. Vertex Y vs. NTrks; N trk per vertex; Vertex y [mm]")
            self.makeLBNHisto2D('VertexNTrksPass', 'VertexZPass', 'TH2F', 100, 0.0, 100.0, 250, -250.0, 250.0,
                                title="Acc. Vertex Z vs. NTrks; N trk per vertex; Vertex z [mm]")

            self.makeProfile('VertexZPass', 'VertexNTrksPass', 100, -250.0, 250.0,
                             title="Acc. Vertex NTrks vs. Vertex Z; Vertex Z [mm]; N trk per vertex")
            self.makeProfile('VertexZPass', 'VertexXerrPass', 100, -250.0, 250.0,
                             title="Vertex Xerr vs Z; Vertex Z [mm]; Vertex Xerr [mm]")
            self.makeProfile('VertexZPass', 'VertexYerrPass', 100, -250.0, 250.0,
                             title="Vertex Yerr vs Z; Vertex Z [mm]; Vertex Yerr [mm]")

            self.makeProfile('VertexNTrksPass', 'VertexXerrPass', 100, 0.0, 100.0,
                             title="Acc. Vertex Xerr vs NTrks; Vertex N trks; Vertex Xerr [mm]")
            self.makeProfile('VertexNTrksPass', 'VertexYerrPass', 100, 0.0, 100.0,
                             title="Acc. Vertex Yerr vs NTrks; Vertex N trks; Vertex Yerr [mm]")
            self.makeProfile('VertexNTrksPass', 'VertexZerrPass', 100, 0.0, 100.0,
                             title="Acc. Vertex Zerr vs NTrks; Vertex N trks; Vertex Zerr [mm]")

        #---------------------------------------
        # Per BCID, per luminosity block histograms
        #---------------------------------------
        # Total number of per-BCID, per-LBN bins:
        #                  552,420
        #---------------------------------------

        if detail >= 1:

            # Bunch-to-bunch luminosity and bunch-profile information
            # Total number of bins: 17,820 (5*3,564)
            self.makeLBNProfile('BCID', 'TotalTracksPass', 3564, 0, 3564,
                                title="Acc. N Tracks vs BCID; BCID; N tracks vertices")
            self.makeLBNProfile('BCID', 'NvtxPassBCID', 3564, 0, 3564,
                                title="Acc. N Vertices vs BCID; BCID; N accepted vertices")
            self.makeLBNProfile('BCID', 'VertexXPassBCID', 3564, 0, 3564,
                                title="Acc. Vertex X vs BCID (profile); BCID; Vertex X [mm]")
            self.makeLBNProfile('BCID', 'VertexYPassBCID', 3564, 0, 3564,
                                title="Acc. Vertex Y vs BCID (profile); BCID; Vertex Y [mm]")
            self.makeLBNProfile('BCID', 'VertexZPassBCID', 3564, 0, 3564,
                                title="Acc. Vertex Z vs BCID (profile); BCID; Vertex Z [mm]")

        if detail >= 2:
            # Bunch-to-bunch 2D histograms
            # Total number of bins: 534,600 ((50+50+50) * 3,564)
            # 20 um bins in X and Y (compared to 10 um for 1D version)
            # 10 mm bins in Z (compared to 5 mm bins for 1D version)
            self.makeLBNHisto2D('BCID', 'VertexXZoomPassBCID', 'TH2F', 3564, 0, 3564, 50, -0.5, 0.5,
                                title="Acc. Vertex X-X_{nominal} vs. BCID; BCID; Vertex X-X_{nominal} [mm]")
            self.makeLBNHisto2D('BCID', 'VertexYZoomPassBCID', 'TH2F', 3564, 0, 3564, 50, -0.5, 0.5,
                                title="Acc. Vertex Y-Y_{nominal} vs. BCID; BCID; Vertex Y-Y_{nominal} [mm]")
            self.makeLBNHisto2D('BCID', 'VertexZZoomPassBCID', 'TH2F', 3564, 0, 3564, 50, -250.0, 250.0,
                                title="Acc. Vertex Z-Z_{nominal} vs. BCID; BCID; Vertex Z-Z_{nominal} [mm]")

        if detail >= 3:

            # All Vertices parameters
            #-------------------------------------------------------------------------------------------------
            #Number of all vertices in an events
            self.makeHisto1D('VertexNTrks', 'TH1F', 100, 0., 100.,
                             title="Vertex NTrks; N trk per vertex; Number of vertices")
            self.makeHisto1D('VertexSumPt', 'TH1F', 100, 0., 100.,
                             title="Vertex SumPt; Vertex sum p_{T} [GeV]; Number of vertices")
            self.makeHisto1D('VertexSumPt2', 'TH1F', 100, 0., 100.,
                             title="Vertex SumPt^2; Vertex sum p_{T}^{2} [GeV^{2}]; Number of vertices")
            self.makeHisto1D('VertexQual', 'TH1F', 50, 0., 50.,
                             title="Vertex Qual; Vertex #chi^{2}/ndf; Number of vertices")
            self.makeHisto1D('VertexMass', 'TH1F', 300, 0., 3000.,
                             title="Vertex mass; Vertex Mass; Number of vertices")
            self.makeHisto1D('VertexChi2Prob', 'TH1F', 70, -0.2, 1.2,
                             title="Vertex Probability; Vertex #chi^{2} probability; Number of vertices")

            # Beam position in X, Y, Z: before vertex cleaning cuts
            # Total number of bins: 1500
            self.makeHisto1D('VertexX', 'TH1F', 500, -5.0, 5.0,
                             title="Vertex X; Vertex X [mm]; Number of vertices")
            self.makeHisto1D('VertexXerr', 'TH1F', 50, 0., 0.5,
                             title="Vertex Xerr; Vertex x error [mm]; Number of vertices")
            self.makeHisto1D('VertexY', 'TH1F', 500, -5.0, 5.0,
                             title="Vertex Y; Vertex Y [mm]; Number of vertices")
            self.makeHisto1D('VertexYerr', 'TH1F', 50, 0., 0.5,
                             title="Vertex Yerr; Vertex y error [mm]; Number of vertices")
            self.makeHisto1D('VertexZ', 'TH1F', 500, -250.0, 250.0,
                             title="Vertex Z; Vertex Z [mm]; Number of vertices")
            self.makeHisto1D('VertexZerr', 'TH1F', 100, 0., 1.,
                             title="Vertex Zerr; Vertex z error [mm]; Number of vertices")

            self.makeHisto1D('VertexXZoom', 'TH1F', 100, -0.250, 0.250,
                             title="Vertex X-X_{nominal}; Vertex X-X_{nominal} [mm]; Number of vertices")
            self.makeHisto1D('VertexYZoom', 'TH1F', 100, -0.250, 0.250,
                             title="Vertex Y-Y_{nominal}; Vertex Y-Y_{nominal} [mm]; Number of vertices")
            self.makeHisto1D('VertexZZoom', 'TH1F', 100, -250., 250.,
                             title="Vertex Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Number of vertices")

            self.makeHisto1D('VertexPull', 'TH1F', 100, -5.0, 5.0,
                             title="Vertex 'pull'; Vertex Z_{PV} - Z_{seed} [mm]; Number of vertices")

            self.makeLBNHisto1D('VertexNTrksInVtx', 'TH1F', 50, 0, 50,
                                title="Acc. NTrks in vertex; Vertex number of tracks; Number of tracks in vertex track list")

        if detail >=3:

            # Passed Vertices parameters
            #-------------------------------------------------------------------------------------------------

            self.makeLBNHisto1D('VertexNTrksPass', 'TH1F', 100, 0, 100,
                                title="Vertex NTrks; N trk per vertex; Number of vertices")
            self.makeHisto1D('VertexSumPtPass', 'TH1F', 100, 0., 100.,
                             title="Vertex SumPt; Vertex sum p_{T} [GeV]; Number of vertices")
            self.makeHisto1D('VertexSumPt2Pass', 'TH1F', 100, 0., 100.,
                             title="Vertex SumPt^2; Vertex sum p_{T}^{2} [GeV^{2}]; Number of vertices")
            self.makeHisto1D('VertexQualPass', 'TH1F', 50, 0., 50.,
                             title="Vertex Qual; Vertex #chi^{2}/ndf; Number of vertices")
            self.makeHisto1D('VertexMassPass', 'TH1F', 100, 0., 3000.,
                             title="Vertex mass; Vertex Mass; Number of vertices")
            self.makeHisto1D('VertexChi2ProbPass', 'TH1F', 70, -0.2, 1.2,
                             title="Vertex Probability; Vertex #chi^{2} probability; Number of vertices")

            # Beam position in X, Y, Z: before vertex cleaning cuts
            # Total number of bins: 1500
            # self.makeHisto1D('VertexXPass', 'TH1F', 500, -5.0, 5.0,
            #                  title="Vertex X; Vertex X [mm]; Number of vertices")
            self.makeHisto1D('VertexXerrPass', 'TH1F', 50, 0., 0.5,
                             title="Vertex Xerr; Vertex x error [mm]; Number of vertices")
            # self.makeHisto1D('VertexYPass', 'TH1F', 500, -5.0, 5.0,
            #                  title="Vertex Y; Vertex Y [mm]; Number of vertices")
            self.makeHisto1D('VertexYerrPass', 'TH1F', 50, 0., 0.5,
                             title="Vertex Yerr; Vertex y error [mm]; Number of vertices")
            # self.makeHisto1D('VertexZPass', 'TH1F', 500, -250.0, 250.0,
            #                  title="Vertex Z; Vertex Z [mm]; Number of vertices")
            self.makeHisto1D('VertexZerrPass', 'TH1F', 100, 0., 1.,
                             title="Vertex Zerr; Vertex z error [mm]; Number of vertices")

            self.makeHisto1D('VertexXZoomPass', 'TH1F', 100, -0.250, 0.250,
                             title="Vertex X-X_{nominal}; Vertex X-X_{nominal} [mm]; Number of vertices")
            self.makeHisto1D('VertexYZoomPass', 'TH1F', 100, -0.250, 0.250,
                             title="Vertex Y-Y_{nominal}; Vertex Y-Y_{nominal} [mm]; Number of vertices")
            self.makeHisto1D('VertexZZoomPass', 'TH1F', 100, -250., 250.,
                             title="Vertex Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Number of vertices")

            self.makeHisto1D('VertexPullPass', 'TH1F', 100, -5.0, 5.0,
                             title="Vertex 'pull'; Vertex Z_{PV} - Z_{seed} [mm]; Number of vertices")

            self.makeLBNHisto1D('VertexNTrksInVtxPass', 'TH1F', 50, 0, 50,
                                title="Acc. NTrks in vertex; Vertex number of tracks; Number of tracks in vertex track list")

            #-------------------------------------------------------------------------------------------------

        if detail >= 3:

            # 2D Histos
            self.makeHisto2D('VertexX', 'VertexY', 'TH2F', 50, -5.0, 5.0, 50, -5.0, 5.0,
                             title="Vertex Y vs X; Vertex X [mm]; Vertex Y [mm]")
            self.makeHisto2D('VertexZ', 'VertexX', 'TH2F', 500, -250.0, 250.0, 50, -5.0, 5.0,
                             title="Vertex X vs Z; Vertex Z [mm]; Vertex X [mm]")
            self.makeHisto2D('VertexZ', 'VertexY', 'TH2F', 500, -250.0, 250.0, 50, -5.0, 5.0,
                             title="Vertex Y vs Z; Vertex Z [mm]; Vertex Y [mm]")
            self.makeHisto2D('VertexXZoom', 'VertexYZoom', 'TH2F', 100, -0.250, 0.250, 100, -0.250, 0.250,
                             title="Vertex Y-Y_{nominal} vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Y-Y_{nominal} [mm]")

            self.makeHisto2D('VertexZ', 'VertexXerr', 'TH2F', 500, -250.0, 250.0, 100, 0.0, 1.0,
                             title="Vertex Xerr vs Z; Vertex Z [mm]; Vertex Xerr [mm]")
            self.makeHisto2D('VertexZ', 'VertexYerr', 'TH2F', 500, -250.0, 250.0, 100, 0.0, 1.0,
                             title="Vertex Yerr vs Z; Vertex Z [mm]; Vertex Yerr [mm]")

        if detail >= 2:
            # X(Y) vs Xerr(Yerr) for Bayesian Inference method
            self.makeHisto2D('VertexXZoomPass', 'VertexXerrPass', 'TH2F', 400, -0.200, 0.200, 100, 0.0, 0.100,
                             title="Vertex Xerr vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Xerr [mm]")
            self.makeHisto2D('VertexYZoomPass', 'VertexYerrPass', 'TH2F', 400, -0.200, 0.200, 100, 0.0, 0.100,
                             title="Vertex Yerr vs Y-Y_{nominal}; Vertex Y-Y_{nominal} [mm]; Vertex Yerr [mm]")

        if detail >= 3:
            # former validation
            self.makeHisto2D('VertexNTrks', 'VertexXerr', 'TH2F', 25, 5.0, 30.0, 100, 0.0, 1.0,
                             title="Vertex Xerr vs NTrks; Vertex N trks; Vertex Xerr [mm]")
            self.makeHisto2D('VertexNTrks', 'VertexYerr', 'TH2F', 25, 5.0, 30.0, 100, 0.0, 1.0,
                             title="Vertex Yerr vs NTrks; Vertex N trks; Vertex Yerr [mm]")
            self.makeHisto2D('VertexNTrks', 'VertexZerr', 'TH2F', 50, 0.0, 50.0, 100, 0.0, 1.0,
                             title="Vertex Zerr vs NTrks; Vertex N trks; Vertex Zerr [mm]")


    def defineSplitVertexHistos(self, detail):
        # split vertex-related variables:
        #  - SplitVertex1<param>Pass, where <param> is one of
        #    - NTrks X Y Z Xerr Yerr Zerr
        #  - SplitVertex2<param>Pass, same set of params
        #  - SplitVertexD<param>Pass, where <param> is one of
        #    - NTrks X Y Z Xerr Yerr Zerr Xpull Ypull Zpull

        if detail >= 1:
            # Delta between the Split Vertex positions in X, Y, Z vs. Ntrk in vertex:
            # allows to correct width for each vertex multiplicity
            # Total number of bins: 150,000 -> 75,000
            # 10 um bins in X and Y
            # 2 mm bins in Z
            self.makeLBNHisto2D('SplitVertexDNTrksPass', 'SplitVertexDXPass', 'TH2F', 100, 0.0, 50.0, 250, -1.25, 1.25,
                                title="Split Vertex DX vs. NTrks; N trk per split vertex; #Deltax between split vertices [mm]")
            self.makeLBNHisto2D('SplitVertexDNTrksPass', 'SplitVertexDYPass', 'TH2F', 100, 0.0, 50.0, 250, -1.25, 1.25,
                                title="Split Vertex DY vs. NTrks; N trk per split vertex; #Deltay between split vertices [mm]")
            self.makeLBNHisto2D('SplitVertexDNTrksPass', 'SplitVertexDZPass', 'TH2F', 100, 0.0, 50.0, 250, -2.5, 2.5,
                                title="Split Vertex DZ vs. NTrks; N trk per split vertex; #Deltaz between split vertices [mm]")

            # Pull in X, Y, Z vs. Ntrk in split vertices: Monitors quality of tracking information
            # Total number of bins: 30,000
            # 0.1 sigma bins
            self.makeHisto2D('SplitVertexDNTrksPass', 'SplitVertexDXpullPass', 'TH2F',
                             100, 0.0, 100.0, 100, -5.0, 5.0,
                             title="Acc. Split Vertex D X Pull vs. NTrks; N trk per vertex; Split Vertex x Pull")
            self.makeHisto2D('SplitVertexDNTrksPass', 'SplitVertexDYpullPass', 'TH2F',
                             100, 0.0, 100.0, 100, -5.0, 5.0,
                             title="Acc. Split Vertex D Y Pull vs. NTrks; N trk per vertex; Split Vertex y Pull")
            self.makeHisto2D('SplitVertexDNTrksPass', 'SplitVertexDZpullPass', 'TH2F',
                             100, 0.0, 100.0, 100, -5.0, 5.0,
                             title="Acc. Split Vertex D Z Pull vs. NTrks; N trk per vertex; Split Vertex z Pull")

            # Split vertex properties
            self.makeHisto2D('SplitVertex2NTrksPass', 'SplitVertex1NTrksPass', 'TH2F',
                             50, 0, 50, 50, 0, 50,
                             title="Split Vertex 2 NTrks vs Split Vertex 1 NTrks")


class T2BSTrackFilterToolMonitoring(BaseMonitoringTool):
    """Monitoring for T2BSTrackFilter tool

    Variables defined by tool:
    - TIME_TrackSelection
    - TracksPerCollection
    - SelectedTracksPerCollection
    - TrackRejectReason
    - TrackPtPass
    - TrackEtaPass
    - TrackPhiPass
    - TrackZ0Pass
    - TrackD0Pass
    - TrackZ0ErrPass
    - TrackD0ErrPass
    - TrackNDFPass
    - TrackQualPass
    - TrackChi2ProbPass
    - TrackSiHitsPass
    - TrackPIXHitsPass
    - TrackSCTHitsPass
    - TrackTRTHitsPass
    """
    def __init__ (self, name="T2BSTrackFilterToolMonitoring", detail=1):
        super(T2BSTrackFilterToolMonitoring, self).__init__(name)

        self.makeHisto1D('TIME_TrackSelection', 'TH1I', 100, 0, 100000,
                         title="Timing track selection; time [#mus];")

        self.makeHisto1D('TracksPerCollection', 'TH1F', 100, 0., 2000.,
                         title="TracksPerCollection; N tracks per collection; Number of collections")
        self.makeHisto1D('SelectedTracksPerCollection', 'TH1F', 100, 0., 400.,
                         title="TracksPerCollectionPass; N accepted tracks per collection; Number of collections")

        # labels correspond to T2TrackBeamSpotImpl::TrackRejectReason (number of reasons is numRejectResons)
        self.makeLBNHisto1D('TrackRejectReason', 'TH1I', 15, xmin=0, xmax=15,
                            opt='kVec', title="TrackRejectReason ; Reason ; # of tracks",
                            xlabels=['PT', 'SiHits', 'PIXHits', 'SCTHits', 'TRTHits',
                                     'NDF', 'D0', 'Z0', 'D0err', 'Z0err',
                                     'Eta', 'MinQual', 'MaxQual', 'Chi2Prob', 'D0Chi2'])

        # Track d0 vs phi: another method for extracting the beam position on average
        # Total number of bins: 70
        self.makeLBNHisto2D('TrackPhiPass', 'TrackD0Pass', 'TH2I',
                50, -3.5, 3.5, 50, -2, 2,
                title="Selected Track d_{0} vs #phi; Track  #phi; Track d_{0}")
        self.makeLBNHisto2D('TrackPhiFilterBS', 'TrackD0FilterBS', 'TH2I',
                50, -3.5, 3.5, 50, -2, 2,
                title="Filtered Track d_{0} vs #phi; Track  #phi; Track d_{0}")

        # ACCEPTED Tracks parameters
        #-------------------------------------------------------------------------------------------------
        self.makeHisto1D('TrackPtPass', 'TH1F', 50, 0.0, 50.0,
                         title="Acc. Track Pt; p_{t} [GeV]; Number of tracks")
        self.makeHisto1D('TrackEtaPass', 'TH1F', 60, -3.0, 3.0,
                         title="Acc. Track Eta; #eta; Number of tracks")
        self.makeHisto1D('TrackPhiPass', 'TH1F', 70, -3.5, 3.5,
                         title="Acc. Track Phi; #phi; Number of tracks")
        self.makeHisto1D('TrackZ0Pass', 'TH1F', 100, -200.0, 200.0,
                         title="Acc. Track Z0; Track z0 [mm]; Number of tracks")
        self.makeHisto1D('TrackD0Pass', 'TH1F', 100, -10.0, 10.0,
                         title="Acc. Track D0; Track d0 [mm]; Number of tracks")
        self.makeHisto1D('TrackZ0errPass', 'TH1F', 100, 0., 5.,
                         title="Acc. Track Z0err; Track z0 error [mm]; Number of tracks")
        self.makeHisto1D('TrackD0errPass', 'TH1F', 100, 0., 5.,
                         title="Acc. Track D0err; Track d0 error [mm]; Number of tracks")
        self.makeHisto1D('TrackNDFPass', 'TH1F', 10, -0.5, 19.5,
                         title="Acc. Track NDF; Track NDF; Number of tracks")
        self.makeHisto1D('TrackQualPass', 'TH1F', 50, 0., 10.,
                         title="Acc. Track Qual; Track #chi^{2}/ndf; Number of tracks")
        self.makeHisto1D('TrackChi2ProbPass', 'TH1F', 70, -0.2, 1.2,
                         title="Acc. Track #chi^{2} probability; Track #chi^{2} probability; Number of tracks")
        #Accepted Track hits in ID
        self.makeHisto1D('TrackSiHitsPass', 'TH1I', 12, 0, 12,
                         title="Acc. Track Silicon hits; N Si hits; Number of tracks")
        self.makeHisto1D('TrackTRTHitsPass', 'TH1I', 50, 0, 50,
                         title="Acc. Track TRT hits; N TRT hits; Number of tracks")
        self.makeHisto1D('TrackPIXHitsPass', 'TH1I', 7, 0, 7,
                         title="Acc. Track PIX hits; N PIX hits; Number of tracks")
        self.makeHisto1D('TrackSCTHitsPass', 'TH1I', 9, 0, 9,
                         title="Acc. Track SCT hits; N SCT hits; Number of tracks")


class T2TrackBeamSpotToolMonitoring(BaseMonitoringTool):
    """Monitoring for T2TrackBeamSpot tool

    Variables defined by tool:
    - BeamLSMatrices
    - BeamLSMatricesBCID
    - TrackLLPolyCoeff
    """
    def __init__ (self, name="T2TrackBeamSpotToolMonitoring", detail=1):
        super(T2TrackBeamSpotToolMonitoring, self).__init__(name)

        self.makeHisto1D('TIME_updateBS', 'TH1I', 100, 0, 10000,
                         title="Timing beamspot update; time [#mus];")

        self.makeLBNHisto1D('BeamLSMatrices', 'TH1D', 18, 0, 18, opt='kVec',
                            title="BeamLSMatrices ; Element ; Value",
                            xlabels=['A_x_x', 'A_x_y', 'A_y_y', 'A_x_tx', 'A_y_tx',
                                     'A_tx_tx', 'A_x_ty', 'A_y_ty', 'A_tx_ty', 'A_ty_ty',
                                     'B_x', 'B_y', 'B_tx', 'B_ty', 'd0^2',
                                     'z0', 'z0^2', '#'])

        self.makeLBNHisto1D('BeamLSMatricesBCID', 'TH1D', 3564*8, 0, 3564*8, opt='kVec',
                            title="BeamLSMatricesBCID ; Element ; Value")

        # for number of bins see T2TrackBSLLPoly::nbins()
        self.makeLBNHisto1D('TrackLLPolyCoeff', 'TH1D', 100, 0, 100, opt='kVec',
                             title="TrackLLPolyCoeff ; Coefficient ; Value")
