# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: T2VertexBeamSpotMonitoring.py 648108 2015-02-19 13:15:50Z smh $
#

################# Validation, DQ checks
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

#================================================================================
# Online
#================================================================================
class T2VertexBeamSpotOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2VertexBeamSpotOnlineMonitoring", detail=1):
        super(T2VertexBeamSpotOnlineMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation", "Cosmic"] )

        #########################################################################
        # The most important plots
        #------------------------------------------------------------------------
        # Total number of bins:
        #                31,500 ->  31,500 (run average histograms)
        #             + 243,404 -> 188,404 (per-LBN beam average histograms)
        #             + 552,420 -> 552,420 (per-LBN per-BCID histograms)
        #             = 827,324 -> 772,324 (total)
        #########################################################################

        if detail >= 1:

            #---------------------------------------
            # Cumulative histograms
            #---------------------------------------
            # Total number of cumulative bins:
            #                  31,500
            #---------------------------------------

            # Number of tracks per event
            # Total number of bins: 400
            self.Histograms += [ defineHistogram('TotalTracks', type='TH1F',
                                                 title="TotalTracks; N tracks per event; Number of events",
                                                 xbins=200, xmin=0.0, xmax=1000.0) ]
            self.Histograms += [ defineHistogram('TotalTracksPass', type='TH1F',
                                                 title="TotalTracksPass;  N accepted tracks per event; Number of events",
                                                 xbins=200, xmin=0.0, xmax=1000.0) ]

            # Track Z position
            # Total number of Bins: 200
            self.Histograms += [ defineHistogram('TrackZ0Pass', type='TH1F',
                                                 title="Acc. Track Z0; Track z0 [mm]; Number of tracks",
                                                 xbins=200, xmin=-400., xmax=400.) ]

            # Vertex properties vs Z and Ntrks: Ntrks, Xerr, Yerr, etc
            # Total number of bins: 600
            self.Histograms += [ defineHistogram('VertexZPass, VertexNTrksPass', type='TProfile',
                                                 title="Acc. Vertex NTrks vs. Vertex Z; Vertex Z [mm]; N trk per vertex",
                                                 xbins=100, xmin=-250.0, xmax=250.0) ]
            self.Histograms += [ defineHistogram('VertexZPass, VertexXerrPass',type='TProfile',
                                                 title="Vertex Xerr vs Z; Vertex Z [mm]; Vertex Xerr [mm]",
                                                 xbins=100, xmin=-250.0, xmax=250.0) ]
            self.Histograms += [ defineHistogram('VertexZPass, VertexYerrPass',type='TProfile',
                                                 title="Vertex Yerr vs Z; Vertex Z [mm]; Vertex Yerr [mm]",
                                                 xbins=100, xmin=-250.0, xmax=250.0) ]

            self.Histograms += [ defineHistogram('VertexNTrksPass, VertexXerrPass',type='TProfile',
                                                 title="Acc. Vertex Xerr vs NTrks; Vertex N trks; Vertex Xerr [mm]",
                                                 xbins=100, xmin=0.0, xmax=100.0) ]
            self.Histograms += [ defineHistogram('VertexNTrksPass, VertexYerrPass',type='TProfile',
                                                 title="Acc. Vertex Yerr vs NTrks; Vertex N trks; Vertex Yerr [mm]",
                                                 xbins=100, xmin=0.0, xmax=100.0) ]
            self.Histograms += [ defineHistogram('VertexNTrksPass, VertexZerrPass',type='TProfile',
                                                 title="Acc. Vertex Zerr vs NTrks; Vertex N trks; Vertex Zerr [mm]",
                                                 xbins=100, xmin=0.0, xmax=100.0) ]


            # X(Y) vs Xerr(Yerr) for Bayesian Inference method
            self.Histograms += [ defineHistogram('VertexXZoomPass, VertexXerrPass',type='TH2F',
                                                 title="Vertex Xerr vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Xerr [mm]",
                                                 xbins=400, xmin=-0.200, xmax=0.200, ybins=100, ymin=0.0, ymax=0.100, opt='kLBN') ]
            self.Histograms += [ defineHistogram('VertexYZoomPass, VertexYerrPass',type='TH2F',
                                                 title="Vertex Yerr vs Y-Y_{nominal}; Vertex Y-Y_{nominal} [mm]; Vertex Yerr [mm]",
                                                 xbins=400, xmin=-0.200, xmax=0.200, ybins=100, ymin=0.0, ymax=0.100, opt='kLBN') ]


            # Pull in X, Y, Z vs. Ntrk in split vertices: Monitors quality of tracking information
            # Total number of bins: 30,000
            # 0.1 sigma bins
            self.Histograms += [ defineHistogram('SplitVertexDNTrksPass, SplitVertexDXpullPass', type='TH2F',
                                                 title="Acc. Split Vertex D X Pull vs. NTrks; N trk per vertex; Split Vertex x Pull",
                                                 xbins=100, xmin=0.0, xmax=100.0, ybins=100, ymin=-5.0, ymax=5.0) ]
            self.Histograms += [ defineHistogram('SplitVertexDNTrksPass, SplitVertexDYpullPass', type='TH2F',
                                                 title="Acc. Split Vertex D Y Pull vs. NTrks; N trk per vertex; Split Vertex y Pull",
                                                 xbins=100, xmin=0.0, xmax=100.0, ybins=100, ymin=-5.0, ymax=5.0) ]
            self.Histograms += [ defineHistogram('SplitVertexDNTrksPass, SplitVertexDZpullPass', type='TH2F',
                                                 title="Acc. Split Vertex D Z Pull vs. NTrks; N trk per vertex; Split Vertex z Pull",
                                                 xbins=100, xmin=0.0, xmax=100.0, ybins=100, ymin=-5.0, ymax=5.0) ]

            # Split Vertex position in X, Y, Z vs. Ntrk in vertex: allows to correct width for each vertex multiplicity
            # Total number of bins: 300
            self.Histograms += [ defineHistogram('SplitVertex1NTrksPass, SplitVertex1XPass', type='TProfile',
                                                 title="Acc. Split Vertex 1 X vs. NTrks; N trk per vertex; x of split vertex 1 [mm]",
                                                 xbins=50, xmin=0.0, xmax=50.0) ]
            self.Histograms += [ defineHistogram('SplitVertex1NTrksPass, SplitVertex1YPass', type='TProfile',
                                                 title="Acc. Vertex 1 Y vs. NTrks; N trk per vertex; y of split vertex 1 [mm]",
                                                 xbins=50, xmin=0.0, xmax=50.0) ]
            self.Histograms += [ defineHistogram('SplitVertex1NTrksPass, SplitVertex1ZPass', type='TProfile',
                                                 title="Acc. Vertex 1 Z vs. NTrks; N trk per vertex; z of split vertex 1 [mm]",
                                                 xbins=50, xmin=0.0, xmax=50.0) ]

            self.Histograms += [ defineHistogram('SplitVertex2NTrksPass, SplitVertex2XPass', type='TProfile',
                                                 title="Acc. Split Vertex 2 X vs. NTrks; N trk per vertex; x of split vertex 2 [mm]",
                                                 xbins=50, xmin=0.0, xmax=50.0) ]
            self.Histograms += [ defineHistogram('SplitVertex2NTrksPass, SplitVertex2YPass', type='TProfile',
                                                 title="Acc. Vertex 2 Y vs. NTrks; N trk per vertex; y of split vertex 2 [mm]",
                                                 xbins=50, xmin=0.0, xmax=50.0) ]
            self.Histograms += [ defineHistogram('SplitVertex2NTrksPass, SplitVertex2ZPass', type='TProfile',
                                                 title="Acc. Vertex 2 Z vs. NTrks; N trk per vertex; z of split vertex 2 [mm]",
                                                 xbins=50, xmin=0.0, xmax=50.0) ]

        if detail >= 0:

            #---------------------------------------
            # Per luminosity block histograms
            #---------------------------------------
            # Specified using: opt='kLBN'
            # NOTE: The "runsummary" histograms from
            #       per-LB histos should be cumulative
            #---------------------------------------
            # Total number of per-LBN bins:
            #                  243,404 -> 188,404
            #---------------------------------------

            # Event survival: tells you what selections are cutting out events
            # Total number of bins: 9
            self.Histograms += [ defineHistogram('EventStatistics', type='TH1F',
                                                 title="EventStatistics",
                                                 xbins=9, xmin=-0.5, xmax=8.5, opt='kLBN',
                                                 labels='All input : Has input TE : Has ROI : Has tracks : Has seed tracks : Has  enough tracks : Has track Z cluster : Has vertex : Has good vertex' ) ]

            # Number of vertices
            # Total number of bins: 25
            self.Histograms += [ defineHistogram('NvtxPass', type='TH1F',
                                                 title="NvtxPass; N accepted vertices per event; Number of events",
                                                 xbins=30, xmin=0.0, xmax=30.0, opt='kLBN') ]

            # Number of tracks per vertex
            # Total number of bins: 100
            self.Histograms += [ defineHistogram('VertexNTrksPass', type='TH1F',
                                                 title="Acc. Vertex NTrks; N trk per vertex; Number of vertices",
                                                 xbins=100, xmin=0, xmax=100, opt='kLBN')]

            # Beam position in X, Y, Z: provides mean position and can extract width (convoluted with vertexing precision)
            # Total number of bins: 2500
            # 10 um bins in X and Y
            #  5 mm bins in Z
            self.Histograms += [ defineHistogram('VertexXPass', type='TH1F',
                                                 title="Acc. Vertex X; Vertex x [mm]; Number of vertices",
                                                 xbins=1000, xmin=-5.0, xmax=5.0,   opt='kLBN') ]
            self.Histograms += [ defineHistogram('VertexYPass', type='TH1F',
                                                 title="Acc. Vertex Y; Vertex y [mm]; Number of vertices",
                                                 xbins=1000, xmin=-5.0, xmax=5.0,   opt='kLBN') ]
            self.Histograms += [ defineHistogram('VertexZPass', type='TH1F',
                                                 title="Acc. Vertex Z; Vertex z [mm]; Number of vertices",
                                                 xbins=500, xmin=-500.0, xmax=500.0, opt='kLBN') ]

            # Beam position in X, Y, Z with respect to the nominal position
            # Total number of bins: 300
            # 10 um bins in X and Y
            #  5 mm bins in Z
            self.Histograms += [ defineHistogram('VertexXZoomPass', type='TH1F',
                                                 title="Acc. Vertex X-X_{nominal}; Vertex X-X_{nominal}  [mm]; Number of vertices",
                                                 xbins=100, xmin=-0.500, xmax=0.500, opt='kLBN') ]
            self.Histograms += [ defineHistogram('VertexYZoomPass', type='TH1F',
                                                 title="Acc. Vertex Y-Y_{nominal}; Vertex Y-Y_{nominal}  [mm]; Number of vertices",
                                                 xbins=100, xmin=-0.500, xmax=0.500, opt='kLBN') ]
            self.Histograms += [ defineHistogram('VertexZZoomPass', type='TH1F',
                                                 title="Acc. Vertex Z-Z_{nominal}; Vertex Z-Z_{nominal}  [mm]; Number of vertices",
                                                 xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN') ]

        if detail >= 1:

            # Track d0 vs phi: another method for extracting the beam position on average
            # Total number of bins: 70
            self.Histograms += [ defineHistogram('TrackPhiPass, TrackD0Pass',type='TProfile',
                                                 title="Selected Track d_{0} vs #phi; Track  #phi; Track d_{0}",
                                                 xbins=70, xmin=-3.5, xmax=3.5, opt='kLBN') ]

            # Beam profile plots: X-Y, X-Z, Y-Z
            # Total number of bins: 2700
            # 100 um bins in X and Y
            self.Histograms += [ defineHistogram('VertexXPass, VertexYPass',type='TH2F',
                                                 title="Acc. Vertex Y vs X; Vertex X [mm]; Vertex Y [mm]",
                                                 xbins=50, xmin=-2.5, xmax=2.5, ybins=50, ymin=-2.5, ymax=2.5, opt='kLBN') ]
            self.Histograms += [ defineHistogram('VertexZPass, VertexXPass',type='TProfile',
                                                 title="Acc. Vertex X vs Z; Vertex Z [mm]; Vertex X [mm]",
                                                 xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN') ]
            self.Histograms += [ defineHistogram('VertexZPass, VertexYPass',type='TProfile',
                                                 title="Acc. Vertex Y vs Z; Vertex Z [mm]; Vertex Y [mm]",
                                                 xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN') ]

            # Beam profile w.r.t. the nominal position: X - X_nominal , etc
            # Total number of bins: 2700
            #  50 um bins in X and Y
            self.Histograms += [ defineHistogram('VertexXZoomPass, VertexYZoomPass',type='TH2F',
                                                 title="Acc. Vertex Y-Y_{nominal} vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Y-Y_{nominal} [mm]",
                                                 xbins=50, xmin=-1.25, xmax=1.25, ybins=50, ymin=-1.25, ymax=1.25, opt='kLBN') ]
            self.Histograms += [ defineHistogram('VertexZZoomPass, VertexXZoomPass',type='TProfile',
                                                 title="Acc. Vertex X-X_{nominal} vs Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Vertex X-X_{nominal} [mm]",
                                                 xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN') ]
            self.Histograms += [ defineHistogram('VertexZZoomPass, VertexYZoomPass',type='TProfile',
                                                 title="Acc. Vertex Y-Y_{nominal} vs Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Vertex Y-Y_{nominal} [mm]",
                                                 xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN') ]

            # Beam position in X, Y, Z vs. Ntrk in vertex: allows to correct width for each vertex multiplicity
            # Total number of bins: 85,000 -> 105,000
            # 10 um bins in X and Y
            #  2 mm bins in Z
            self.Histograms += [ defineHistogram('VertexNTrksPass, VertexXPass', type='TH2F',
                                                 title="Acc. Vertex X vs. NTrks; N trk per vertex; Vertex x [mm]",
                                                 xbins=100, xmin=0.0, xmax=100.0, ybins=400, ymin=-2.00, ymax=2.00, opt='kLBN') ]
            self.Histograms += [ defineHistogram('VertexNTrksPass, VertexYPass', type='TH2F',
                                                 title="Acc. Vertex Y vs. NTrks; N trk per vertex; Vertex y [mm]",
                                                 xbins=100, xmin=0.0, xmax=100.0, ybins=400, ymin=-2.00, ymax=2.00, opt='kLBN') ]
            self.Histograms += [ defineHistogram('VertexNTrksPass, VertexZPass', type='TH2F',
                                                 title="Acc. Vertex Z vs. NTrks; N trk per vertex; Vertex z [mm]",
                                                 xbins=100, xmin=0.0, xmax=100.0, ybins=250, ymin=-250.0, ymax=250.0, opt='kLBN') ]

            # Delta between the Split Vertex positions in X, Y, Z vs. Ntrk in vertex:
            # allows to correct width for each vertex multiplicity
            # Total number of bins: 150,000 -> 75,000
            # 10 um bins in X and Y
            # 2 mm bins in Z
            self.Histograms += [ defineHistogram('SplitVertexDNTrksPass, SplitVertexDXPass', type='TH2F',
                                                 title="Split Split Vertex DX vs. NTrks; N trk per split vertex; #Deltax between split vertices [mm]",
                                                 xbins=100, xmin=0.0, xmax=50.0, ybins=250, ymin=-1.25, ymax=1.25, opt='kLBN') ]
            self.Histograms += [ defineHistogram('SplitVertexDNTrksPass, SplitVertexDYPass', type='TH2F',
                                                 title="Split Vertex DY vs. NTrks; N trk per split vertex; #Deltay between split vertices [mm]",
                                                 xbins=100, xmin=0.0, xmax=50.0, ybins=250, ymin=-1.25, ymax=1.25, opt='kLBN') ]
            self.Histograms += [ defineHistogram('SplitVertexDNTrksPass, SplitVertexDZPass', type='TH2F',
                                                 title="Split Vertex DZ vs. NTrks; N trk per split vertex; #Deltaz between split vertices [mm]",
                                                 xbins=100, xmin=0.0, xmax=50.0, ybins=250, ymin=-2.5, ymax=2.5, opt='kLBN') ]

            #---------------------------------------
            # Per BCID, per luminosity block histograms
            #---------------------------------------
            # Total number of per-BCID, per-LBN bins:
            #                  552,420
            #---------------------------------------

            # Bunch-to-bunch luminosity and bunch-profile information
            # Total number of bins: 17,820 (5*3,564)
            self.Histograms += [ defineHistogram('BCID, TotalTracksPass',type='TProfile',
                                                 title="Acc. N Tracks vs BCID; BCID; N tracks vertices",
                                                 xbins=3564, xmin=0, xmax=3564, opt='kLBN') ]

            self.Histograms += [ defineHistogram('BCID, NvtxPassBCID',type='TProfile',
                                                 title="Acc. N Vertices vs BCID; BCID; N accepted vertices",
                                                 xbins=3564, xmin=0, xmax=3564, opt='kLBN') ]

            self.Histograms += [ defineHistogram('BCID, VertexXPassBCID',type='TProfile',
                                                 title="Acc. Vertex X vs BCID (profile); BCID; Vertex X [mm]",
                                                 xbins=3564, xmin=0, xmax=3564, opt='kLBN') ]
            self.Histograms += [ defineHistogram('BCID, VertexYPassBCID',type='TProfile',
                                                 title="Acc. Vertex Y vs BCID (profile); BCID; Vertex Y [mm]",
                                                 xbins=3564, xmin=0, xmax=3564, opt='kLBN') ]
            self.Histograms += [ defineHistogram('BCID, VertexZPassBCID',type='TProfile',
                                                 title="Acc. Vertex Z vs BCID (profile); BCID; Vertex Z [mm]",
                                                 xbins=3564, xmin=0, xmax=3564, opt='kLBN') ]

        if detail >= 2:

            # Bunch-to-bunch 2D histograms
            # Total number of bins: 534,600 ((50+50+50) * 3,564)
            # 20 um bins in X and Y (compared to 10 um for 1D version)
            # 10 mm bins in Z (compared to 5 mm bins for 1D version)

            self.Histograms += [ defineHistogram('BCID, VertexXZoomPassBCID', type='TH2F',
                                                 title="Acc. Vertex X-X_{nominal} vs. BCID; BCID; Vertex X-X_{nominal} [mm]",
                                                 xbins=3564, xmin=0, xmax=3564, ybins=50, ymin=-0.5, ymax=0.5, opt='kLBN') ]
            self.Histograms += [ defineHistogram('BCID, VertexYZoomPassBCID', type='TH2F',
                                                 title="Acc. Vertex Y-Y_{nominal} vs. BCID; BCID; Vertex Y-Y_{nominal} [mm]",
                                                 xbins=3564, xmin=0, xmax=3564, ybins=50, ymin=-0.5, ymax=0.5, opt='kLBN') ]
            self.Histograms += [ defineHistogram('BCID, VertexZZoomPassBCID', type='TH2F',
                                                 title="Acc. Vertex Z-Z_{nominal} vs. BCID; BCID; Vertex Z-Z_{nominal} [mm]",
                                                 xbins=3564, xmin=0, xmax=3564, ybins=50, ymin=-250.0, ymax=250.0, opt='kLBN') ]

        if detail >= 2:

            # Track Z cluster properties
            # Total number of bins: 425
            # FIXME: addt'l number of bins below still need to be added to tally

            self.Histograms += [ defineHistogram('NClusters', type='TH1F',
                                                 title="NClusters; Number of clusters per event; Number of events",
                                                 xbins=30, xmin=0.0, xmax=30.0) ]
            self.Histograms += [ defineHistogram('ClusterNTracks', type='TH1F',
                                                 title="ClusterNTracks; Number tracks per cluster; Number of events",
                                                 xbins=100, xmin=0.0, xmax=100.0) ]
            self.Histograms += [ defineHistogram('ClusterNTracks, ClusterNTracksUnused', type='TH2F',
                                                 title="Number of tracks in cluster unused vs used; Number of tracks per cluster; Number of unused tracks per cluster",
                                                 xbins=50, xmin=0.0, xmax=50.0, ybins=50, ymin=0.0, ymax=50.0) ]
            self.Histograms += [ defineHistogram('ClusterZ', type='TH1F',
                                                 title="ClusterZ; Cluster Z position [mm]; Number of clusters",
                                                 xbins=200, xmin=-250.0, xmax=250.0) ]
            self.Histograms += [ defineHistogram('ClusterDeltaZ0', type='TH1F',
                                                 title="ClusterDeltaZ0; Delta of track Z0 and cluster Z position [mm]; Number of tracks",
                                                 xbins=200, xmin=-10.0, xmax=10.0) ]
            self.Histograms += [ defineHistogram('ClusterZ0Pull', type='TH1F',
                                                 title="ClusterZ0Pull; Pull of track Z0 with respect to cluster Z position; Number of tracks",
                                                 xbins=200, xmin=-10.0, xmax=10.0) ]
            self.Histograms += [ defineHistogram('ClusterDeltaVertexZ', type='TH1F',
                                                 title="ClusterDeltaVertexZ; Delta of cluster Z position and Vertex Z position [mm]; Number of vertices",
                                                 xbins=200, xmin=-10.0, xmax=10.0) ]

            # Vertex properties
            # Total number of bins: 200
            self.Histograms += [ defineHistogram('VertexSumPtPass', type='TH1F',
                                                 title="Acc. Vertex Sum p_{T}; Vertex #sum p_{T} [GeV]; Number of vertices",
                                                 xbins=100, xmin=0., xmax=100.) ]

            self.Histograms += [ defineHistogram('VertexSumPt2Pass', type='TH1F',
                                                 title="Acc. Vertex #sqrt{Sum p_{T}^{2}}; Vertex #sqrt{#sum p_{T}^{2}} [GeV]; Number of vertices",
                                                 xbins=100, xmin=0., xmax=100.) ]

            # Split vertex properties
            self.Histograms += [ defineHistogram('SplitVertex2NTrksPass, SplitVertex1NTrksPass',type='TH2F',
                                                 title="Split Vertex 2 NTrks vs Split Vertex 1 NTrks",
                                                 xbins=50, xmin=0, xmax=50, ybins=50, ymin=0, ymax=50) ]

            self.Histograms += [ defineHistogram('VertexNTrksPass, VertexNTrksInVtxPass', type='TH2F',
                                                 title="Acc. Vertex NTrks vs NTrks in vertex; Vertex number of tracks; Number of tracks in vertex track list",
                                                 xbins=50, xmin=0, xmax=50, ybins=50, ymin=0., ymax=50, opt='kLBN')]
                       
#================================================================================
# Cosmic
#================================================================================
class T2VertexBeamSpotCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2VertexBeamSpotCosmicMonitoring"):
        super(T2VertexBeamSpotCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")



#================================================================================
# Validation
#================================================================================
########## ##############################################################
#  add validation specific histograms.
#  If you ever remove histograms from Online - move them into Validation
#
#########################################################################
class T2VertexBeamSpotValidationMonitoring( TrigGenericMonitoringToolConfig ):
    def __init__ (self, name="T2VertexBeamSpotValidationMonitoring"):
        super(T2VertexBeamSpotValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        # All Track algorithms

        # ACCEPTED Tracks
        self.Histograms += [ defineHistogram('TrackSiHitsPass', type='TH1I',
                                             title="Acc. Track Silicon hits; N Si hits; Number of tracks",
                                             xbins=12, xmin=0, xmax=12) ]
        self.Histograms += [ defineHistogram('TrackTRTHitsPass', type='TH1I',
                                             title="Acc. Track TRT hits; N TRT hits; Number of tracks",
                                             xbins=50, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('TrackPIXHitsPass', type='TH1I',
                                             title="Acc. Track PIX hits; N PIX hits; Number of tracks",
                                             xbins=7, xmin=0, xmax=7) ]
        self.Histograms += [ defineHistogram('TrackSCTHitsPass', type='TH1I',
                                             title="Acc. Track SCT hits; N SCT hits; Number of tracks",
                                             xbins=9, xmin=0, xmax=9) ]

        # Per TE
        self.Histograms += [ defineHistogram('ROIperTE', type='TH1F',
                                             title="ROIperTE; N ROIs per TE; Number of TEs",
                                             xbins=20, xmin=0, xmax=20) ]
        self.Histograms += [ defineHistogram('TracksPerTE', type='TH1F',
                                             title="TracksPerTE; N tracks per TE; Number of TEs",
                                             xbins=75, xmin=0, xmax=150) ]
        self.Histograms += [ defineHistogram('TracksPerTEPass', type='TH1F',
                                             title="TracksPerTEPass; N accepted tracks per TE; Number of TEs",
                                             xbins=75, xmin=0, xmax=150) ]

        # Per ROI
        self.Histograms += [ defineHistogram('TracksPerROI', type='TH1F',
                                             title="TracksPerROI; N tracks per ROI; Number of ROIs",
                                             xbins=75, xmin=0, xmax=150) ]
        self.Histograms += [ defineHistogram('TracksPerROIPass', type='TH1F',
                                             title="TracksPerROIPass; N accepted tracks per ROI; Number of ROIs",
                                             xbins=25, xmin=0, xmax=50) ]

        # ACCEPTED Tracks
        self.Histograms += [ defineHistogram('TrackPtPass', type='TH1F',
                                             title="Acc. Track Pt; p_{t} [GeV]; Number of tracks",
                                             xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('TrackEtaPass', type='TH1F',
                                             title="Acc. Track Eta; #eta; Number of tracks",
                                             xbins=60, xmin=-3.0, xmax=3.0) ]
        self.Histograms += [ defineHistogram('TrackPhiPass', type='TH1F',
                                             title="Acc. Track Phi; #phi; Number of tracks",
                                             xbins=70, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram('TrackD0Pass', type='TH1F',
                                             title="Acc. Track D0; Track d0 [mm]; Number of tracks",
                                             xbins=100, xmin=-10.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('TrackZ0errPass', type='TH1F',
                                             title="Acc. Track Z0err; Track z0 error [mm]; Number of tracks",
                                             xbins=100, xmin=0., xmax=5.) ]
        self.Histograms += [ defineHistogram('TrackD0errPass', type='TH1F',
                                             title="Acc. Track D0err; Track d0 error [mm]; Number of tracks",
                                             xbins=100, xmin=0., xmax=5.) ]
        self.Histograms += [ defineHistogram('TrackQualPass', type='TH1F',
                                             title="Acc. Track Qual; Track #chi^{2}/ndf; Number of tracks",
                                             xbins=50, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('TrackChi2ProbPass', type='TH1F',
                                             title="Acc. Track #chi^{2} probability; Track #chi^{2} probability; Number of tracks",
                                             xbins=70, xmin=-0.2, xmax=1.2) ]

        # ALL Vertices and clusters
        self.Histograms += [ defineHistogram('Nvtx', type='TH1F',
                                             title="Nvtx; N vertices per event; Number of events",
                                             xbins=10, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('VertexNTrks', type='TH1F',
                                             title="Vertex NTrks; N trk per vertex; Number of vertices",
                                             xbins=100, xmin=0, xmax=100)]
        self.Histograms += [ defineHistogram('VertexSumPt', type='TH1F',
                                             title="Vertex SumPt; Vertex sum p_{T} [GeV]; Number of vertices",
                                             xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('VertexQual', type='TH1F',
                                             title="Vertex Qual; Vertex #chi^{2}/ndf; Number of vertices",
                                             xbins=50, xmin=0., xmax=50.) ]
        #self.Histograms += [ defineHistogram('VertexProb', type='TH1F',
        #                                     title="Vertex Probability; Vertex #chi^{2} probability; Number of vertices",
        #                                     xbins=70, xmin=-0.2, xmax=1.2) ]
        self.Histograms += [ defineHistogram('VertexXerr', type='TH1F',
                                             title="Vertex Xerr; Vertex x error [mm]; Number of vertices",
                                             xbins=50, xmin=0., xmax=0.5) ]
        self.Histograms += [ defineHistogram('VertexYerr', type='TH1F',
                                             title="Vertex Yerr; Vertex y error [mm]; Number of vertices",
                                             xbins=50, xmin=0., xmax=0.5) ]
        self.Histograms += [ defineHistogram('VertexZerr', type='TH1F',
                                             title="Vertex Zerr; Vertex z error [mm]; Number of vertices",
                                             xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('VertexXZoom', type='TH1F',
                                             title="Vertex X-X_{nominal}; Vertex X-X_{nominal} [mm]; Number of vertices",
                                             xbins=100, xmin=-0.250, xmax=0.250) ]
        self.Histograms += [ defineHistogram('VertexYZoom', type='TH1F',
                                             title="Vertex Y-Y_{nominal}; Vertex Y-Y_{nominal} [mm]; Number of vertices",
                                             xbins=100, xmin=-0.250, xmax=0.250) ]
        self.Histograms += [ defineHistogram('VertexZZoom', type='TH1F',
                                             title="Vertex Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Number of vertices",
                                             xbins=100, xmin=-250., xmax=250.) ]
        self.Histograms += [ defineHistogram('VertexPull', type='TH1F',
                                             title="Vertex 'pull'; Vertex Z_{PV} - Z_{seed} [mm]; Number of vertices",
                                             xbins=100, xmin=-5.0, xmax=5.0) ]

        # Beam position in X, Y, Z: before vertex cleaning cuts
        # Total number of bins: 1500
        self.Histograms += [ defineHistogram('VertexX', type='TH1F',
                                             title="Vertex X; Vertex X [mm]; Number of vertices",
                                             xbins=500, xmin=-5.0, xmax=5.0) ]
        self.Histograms += [ defineHistogram('VertexY', type='TH1F',
                                             title="Vertex Y; Vertex Y [mm]; Number of vertices",
                                             xbins=500, xmin=-5.0, xmax=5.0) ]
        self.Histograms += [ defineHistogram('VertexZ', type='TH1F',
                                             title="Vertex Z; Vertex Z [mm]; Number of vertices",
                                             xbins=500, xmin=-250.0, xmax=250.0) ]

        # ACCEPTED Vertices
        self.Histograms += [ defineHistogram('VertexMassPass', type='TH1F',
                                             title="Acc. Vertex Mass; Vertex mass [GeV]; Number of vertices",
                                             xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('VertexQualPass', type='TH1F',
                                             title="Acc. Vertex Qual; Vertex #chi^{2}/ndf; Number of vertices",
                                             xbins=50, xmin=0., xmax=50.) ]
        #self.Histograms += [ defineHistogram('VertexProbPass', type='TH1F',
        #                                     title="Acc. Vertex Probability; Vertex #chi^{2} probability; Number of vertices",
        #                                     xbins=70, xmin=-0.2, xmax=1.2) ]
        self.Histograms += [ defineHistogram('VertexXerrPass', type='TH1F',
                                             title="Acc. Vertex Xerr; Vertex X error [mm]; Number of vertices",
                                             xbins=50, xmin=0., xmax=0.5) ]
        self.Histograms += [ defineHistogram('VertexYerrPass', type='TH1F',
                                             title="Acc. Vertex Yerr; Vertex Y error [mm]; Number of vertices",
                                             xbins=50, xmin=0., xmax=0.5) ]
        self.Histograms += [ defineHistogram('VertexZerrPass', type='TH1F',
                                             title="Acc. Vertex Zerr; Vertex Z error [mm]; Number of vertices",
                                             xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('VertexPullPass', type='TH1F',
                                             title="Acc. Vertex 'pull'; Vertex Z_{PV} - Z_{seed} [mm]; Number of vertices",
                                             xbins=100, xmin=-5.0, xmax=5.0) ]

        # 2D Histos
        self.Histograms += [ defineHistogram('VertexX, VertexY',type='TH2F',
                                             title="Vertex Y vs X; Vertex X [mm]; Vertex Y [mm]",
                                             xbins=50, xmin=-5.0, xmax=5.0, ybins=50, ymin=-5.0, ymax=5.0) ]
        self.Histograms += [ defineHistogram('VertexZ, VertexX',type='TH2F',
                                             title="Vertex X vs Z; Vertex X [mm]; Vertex Z [mm]",
                                             xbins=500, xmin=-250.0, xmax=250.0, ybins=50, ymin=-5.0, ymax=5.0) ]
        self.Histograms += [ defineHistogram('VertexZ, VertexY',type='TH2F',
                                             title="Vertex Y vs Z; Vertex Y [mm]; Vertex Z [mm]",
                                             xbins=500, xmin=-250.0, xmax=250.0, ybins=50, ymin=-5.0, ymax=5.0) ]
        self.Histograms += [ defineHistogram('VertexXZoom, VertexYZoom',type='TH2F',
                                             title="Vertex Y-Y_{nominal} vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Y-Y_{nominal} [mm]",
                                             xbins=100, xmin=-0.250, xmax=0.250, ybins=100, ymin=-0.250, ymax=0.250) ]

        self.Histograms += [ defineHistogram('VertexZ, VertexXerr',type='TH2F',
                                             title="Vertex Xerr vs Z; Vertex Z [mm]; Vertex Xerr [mm]",
                                             xbins=500, xmin=-250.0, xmax=250.0, ybins=100, ymin=0.0, ymax=1.0) ]
        self.Histograms += [ defineHistogram('VertexZ, VertexYerr',type='TH2F',
                                             title="Vertex Yerr vs Z; Vertex Z [mm]; Vertex Yerr [mm]",
                                             xbins=500, xmin=-250.0, xmax=250.0, ybins=100, ymin=0.0, ymax=1.0) ]

        # X(Y) vs Xerr(Yerr) for Bayesian Inference method
        self.Histograms += [ defineHistogram('VertexXZoomPass, VertexXerrPass',type='TH2F',
                                             title="Vertex Xerr vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Xerr [mm]",
                                             xbins=400, xmin=-0.200, xmax=0.200, ybins=100, ymin=0.0, ymax=0.100) ]
        self.Histograms += [ defineHistogram('VertexYZoomPass, VertexYerrPass',type='TH2F',
                                             title="Vertex Yerr vs Y-Y_{nominal}; Vertex Y-Y_{nominal} [mm]; Vertex Yerr [mm]",
                                             xbins=400, xmin=-0.200, xmax=0.200, ybins=100, ymin=0.0, ymax=0.100) ]

        self.Histograms += [ defineHistogram('VertexNTrks, VertexXerr',type='TH2F',
                                             title="Vertex Xerr vs NTrks; Vertex N trks; Vertex Xerr [mm]",
                                             xbins=25, xmin=5.0, xmax=30.0, ybins=100, ymin=0.0, ymax=1.0) ]
        self.Histograms += [ defineHistogram('VertexNTrks, VertexYerr',type='TH2F',
                                             title="Vertex Yerr vs NTrks; Vertex N trks; Vertex Yerr [mm]",
                                             xbins=25, xmin=5.0, xmax=30.0, ybins=100, ymin=0.0, ymax=1.0) ]
        self.Histograms += [ defineHistogram('VertexNTrks, VertexZerr',type='TH2F',
                                             title="Vertex Zerr vs NTrks; Vertex N trks; Vertex Zerr [mm]",
                                             xbins=50, xmin=0.0, xmax=50.0, ybins=100, ymin=0.0, ymax=1.0) ]


    def defineAlgoSpecificHistograms( self, prefix='' ):
        self.Histograms += [ defineHistogram(prefix+'Pt', type='TH1F',
                                             title="Track Pt; p_{t} [GeV]; Number of tracks",
                                             xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram(prefix+'Eta', type='TH1F',
                                             title="Track Eta; #eta; Number of tracks",
                                             xbins=60, xmin=-3.0, xmax=3.0) ]
        self.Histograms += [ defineHistogram(prefix+'Phi', type='TH1F',
                                             title="Track Phi; #phi; Number of tracks",
                                             xbins=70, xmin=-3.5, xmax=3.5) ]
        self.Histograms += [ defineHistogram(prefix+'Z0', type='TH1F',
                                             title="Track Z0; Track z0 [mm]; Number of tracks",
                                             xbins=100, xmin=-200., xmax=200.) ]
        self.Histograms += [ defineHistogram(prefix+'D0', type='TH1F',
                                             title="Track D0; Track d0 [mm]; Number of tracks",
                                             xbins=100, xmin=-10.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram(prefix+'Z0err', type='TH1F',
                                             title="Track Z0err; Track z0 error [mm]; Number of tracks",
                                             xbins=100, xmin=0., xmax=5.) ]
        self.Histograms += [ defineHistogram(prefix+'D0err', type='TH1F',
                                             title="Track D0err; Track d0 error [mm]; Number of tracks",
                                             xbins=100, xmin=0., xmax=5.) ]
        self.Histograms += [ defineHistogram(prefix+'Qual', type='TH1F',
                                             title="Track Qual; Track #chi^{2}/ndf; Number of tracks",
                                             xbins=50, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram(prefix+'Chi2Prob', type='TH1F',
                                             title="Track #chi^{2} probability; Track #chi^{2} probability; Number of tracks",
                                             xbins=70, xmin=-0.2, xmax=1.2) ]
        self.Histograms += [ defineHistogram(prefix+'SiHits', type='TH1I',
                                             title="Track Silicon hits; N Si hits; Number of tracks",
                                             xbins=12, xmin=0, xmax=12) ]
        self.Histograms += [ defineHistogram(prefix+'TRTHits', type='TH1I',
                                             title="Track TRT hits; N TRT hits; Number of tracks",
                                             xbins=50, xmin=0, xmax=50) ]
