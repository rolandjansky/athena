# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: T2VertexBeamSpotMonitoring.py 648108 2015-02-19 13:15:50Z smh $
#

################# Validation, DQ checks
#from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig


#Adding new monitoring tool
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
#monTool = GenericMonitoringTool("MonTool")



#================================================================================
# Online
#================================================================================
#class T2VertexBeamSpotOnlineMonitoring(TrigGenericMonitoringToolConfig):
class T2VertexBeamSpotOnlineMonitoring(GenericMonitoringTool):
    def __init__ (self, name="T2VertexBeamSpotOnlineMonitoring", detail=1):
        super(T2VertexBeamSpotOnlineMonitoring, self).__init__(name)
        #self.defineTarget( ["Online", "Validation", "Cosmic"] )

        #########################################################################
        # The most important plots
        #------------------------------------------------------------------------
        # Total number of bins:
        #                31,500 ->  31,500 (run average histograms)
        #             + 243,404 -> 188,404 (per-LBN beam average histograms)
        #             + 552,420 -> 552,420 (per-LBN per-BCID histograms)
        #             = 827,324 -> 772,324 (total)
        #########################################################################

        #self.defineHistogram('nTracksMAT', path ='EXPERT', type='TH1F',
        #                                         title="nTracksMAT; N tracks per event; Number of events",
        #                                         xbins=200, xmin=0.0, xmax=1000.0) 
        if detail >= 1:

            #---------------------------------------
            # Cumulative histograms
            #---------------------------------------
            # Total number of cumulative bins:
            #                  31,500
            #---------------------------------------

            # Number of tracks per event
            # Total number of bins: 400
           # self.defineHistogram('TotalTracks', path='EXPERT', type='TH1F',
           #                                      title="TotalTracks; N tracks per event; Number of events",
           #                                      xbins=200, xmin=0.0, xmax=1000.0)

           # self.defineHistogram('nTracksMAT', path='EXPERT', type='TH1F',
           #                                      title="nTracksMAT; N tracks per event; Number of events",
           #                                      xbins=200, xmin=0.0, xmax=1000.0)

           # self.defineHistogram('TotalTracksPass', path='EXPERT', type='TH1F',
           #                                      title="TotalTracksPass;  N accepted tracks per event; Number of events",
           #                                      xbins=200, xmin=0.0, xmax=1000.0)


#TODO:
#These histograms are not even defined in the code! Are they obsolete?
            # Track Z position
            # Total number of Bins: 200
            
            #self.defineHistogram('TrackZ0Pass', path='EXPERT', type='TH1F',
            #                                     title="Acc. Track Z0; Track z0 [mm]; Number of tracks",
            #                                     xbins=200, xmin=-400., xmax=400.)

            # Vertex properties vs Z and Ntrks: Ntrks, Xerr, Yerr, etc
            # Total number of bins: 600
            #self.defineHistogram('VertexZPass, VertexNTrksPass', path='EXPERT', type='TProfile',
            #                                     title="Acc. Vertex NTrks vs. Vertex Z; Vertex Z [mm]; N trk per vertex",
            #                                     xbins=100, xmin=-250.0, xmax=250.0)
            #self.defineHistogram('VertexZPass, VertexXerrPass',path='EXPERT', type='TProfile',
            #                                     title="Vertex Xerr vs Z; Vertex Z [mm]; Vertex Xerr [mm]",
            #                                     xbins=100, xmin=-250.0, xmax=250.0)
            #self.defineHistogram('VertexZPass, VertexYerrPass',path='EXPERT', type='TProfile',
            #                                     title="Vertex Yerr vs Z; Vertex Z [mm]; Vertex Yerr [mm]",
            #                                     xbins=100, xmin=-250.0, xmax=250.0)

            #self.defineHistogram('VertexNTrksPass, VertexXerrPass',path='EXPERT', type='TProfile',
            #                                     title="Acc. Vertex Xerr vs NTrks; Vertex N trks; Vertex Xerr [mm]",
            #                                     xbins=100, xmin=0.0, xmax=100.0)
            #self.defineHistogram('VertexNTrksPass, VertexYerrPass',path='EXPERT', type='TProfile',
            #                                     title="Acc. Vertex Yerr vs NTrks; Vertex N trks; Vertex Yerr [mm]",
            #                                     xbins=100, xmin=0.0, xmax=100.0)
            #self.defineHistogram('VertexNTrksPass, VertexZerrPass',path='EXPERT', type='TProfile',
            #                                     title="Acc. Vertex Zerr vs NTrks; Vertex N trks; Vertex Zerr [mm]",
            #                                     xbins=100, xmin=0.0, xmax=100.0)


            # X(Y) vs Xerr(Yerr) for Bayesian Inference method
            self.defineHistogram('VertexXZoomPass, VertexXerrPass',path='EXPERT', type='TH2F',
                                                 title="Vertex Xerr vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Xerr [mm]",
                                                 xbins=400, xmin=-0.200, xmax=0.200, ybins=100, ymin=0.0, ymax=0.100, opt='kLBN')
            self.defineHistogram('VertexYZoomPass, VertexYerrPass',path='EXPERT', type='TH2F',
                                                 title="Vertex Yerr vs Y-Y_{nominal}; Vertex Y-Y_{nominal} [mm]; Vertex Yerr [mm]",
                                                 xbins=400, xmin=-0.200, xmax=0.200, ybins=100, ymin=0.0, ymax=0.100, opt='kLBN')


            # Pull in X, Y, Z vs. Ntrk in split vertices: Monitors quality of tracking information
            # Total number of bins: 30,000
            # 0.1 sigma bins
            self.defineHistogram('SplitVertexDNTrksPass, SplitVertexDXpullPass', path='EXPERT', type='TH2F',
                                                 title="Acc. Split Vertex D X Pull vs. NTrks; N trk per vertex; Split Vertex x Pull",
                                                 xbins=100, xmin=0.0, xmax=100.0, ybins=100, ymin=-5.0, ymax=5.0)
            self.defineHistogram('SplitVertexDNTrksPass, SplitVertexDYpullPass', path='EXPERT', type='TH2F',
                                                 title="Acc. Split Vertex D Y Pull vs. NTrks; N trk per vertex; Split Vertex y Pull",
                                                 xbins=100, xmin=0.0, xmax=100.0, ybins=100, ymin=-5.0, ymax=5.0)
            self.defineHistogram('SplitVertexDNTrksPass, SplitVertexDZpullPass', path='EXPERT', type='TH2F',
                                                 title="Acc. Split Vertex D Z Pull vs. NTrks; N trk per vertex; Split Vertex z Pull",
                                                 xbins=100, xmin=0.0, xmax=100.0, ybins=100, ymin=-5.0, ymax=5.0)

            # Split Vertex position in X, Y, Z vs. Ntrk in vertex: allows to correct width for each vertex multiplicity
            # Total number of bins: 300
            self.defineHistogram('SplitVertex1NTrksPass, SplitVertex1XPass', path='EXPERT', type='TProfile',
                                                 title="Acc. Split Vertex 1 X vs. NTrks; N trk per vertex; x of split vertex 1 [mm]",
                                                 xbins=50, xmin=0.0, xmax=50.0)
            self.defineHistogram('SplitVertex1NTrksPass, SplitVertex1YPass', path='EXPERT', type='TProfile',
                                                 title="Acc. Vertex 1 Y vs. NTrks; N trk per vertex; y of split vertex 1 [mm]",
                                                 xbins=50, xmin=0.0, xmax=50.0)
            self.defineHistogram('SplitVertex1NTrksPass, SplitVertex1ZPass', path='EXPERT', type='TProfile',
                                                 title="Acc. Vertex 1 Z vs. NTrks; N trk per vertex; z of split vertex 1 [mm]",
                                                 xbins=50, xmin=0.0, xmax=50.0)

            self.defineHistogram('SplitVertex2NTrksPass, SplitVertex2XPass', path='EXPERT', type='TProfile',
                                                 title="Acc. Split Vertex 2 X vs. NTrks; N trk per vertex; x of split vertex 2 [mm]",
                                                 xbins=50, xmin=0.0, xmax=50.0)
            self.defineHistogram('SplitVertex2NTrksPass, SplitVertex2YPass', path='EXPERT', type='TProfile',
                                                 title="Acc. Vertex 2 Y vs. NTrks; N trk per vertex; y of split vertex 2 [mm]",
                                                 xbins=50, xmin=0.0, xmax=50.0)
            self.defineHistogram('SplitVertex2NTrksPass, SplitVertex2ZPass', path='EXPERT', type='TProfile',
                                                 title="Acc. Vertex 2 Z vs. NTrks; N trk per vertex; z of split vertex 2 [mm]",
                                                 xbins=50, xmin=0.0, xmax=50.0)

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


            # Number of vertices
            # Total number of bins: 25
            self.defineHistogram('NvtxPass', path='EXPERT', type='TH1F',
                                                 title="NvtxPass; N accepted vertices per event; Number of events",
                                                 xbins=30, xmin=0.0, xmax=30.0, opt='kLBN')

            # Number of tracks per vertex
            # Total number of bins: 100
            self.defineHistogram('VertexNTrksPass', path='EXPERT', type='TH1F',
                                                 title="Acc. Vertex NTrks; N trk per vertex; Number of vertices",
                                                 xbins=100, xmin=0, xmax=100, opt='kLBN')

            # Beam position in X, Y, Z: provides mean position and can extract width (convoluted with vertexing precision)
            # Total number of bins: 2500
            # 10 um bins in X and Y
            #  5 mm bins in Z
            self.defineHistogram('VertexXPass', path='EXPERT', type='TH1F',
                                                 title="Acc. Vertex X; Vertex x [mm]; Number of vertices",
                                                 xbins=1000, xmin=-5.0, xmax=5.0,   opt='kLBN')
            self.defineHistogram('VertexYPass', path='EXPERT', type='TH1F',
                                                 title="Acc. Vertex Y; Vertex y [mm]; Number of vertices",
                                                 xbins=1000, xmin=-5.0, xmax=5.0,   opt='kLBN')
            self.defineHistogram('VertexZPass', path='EXPERT', type='TH1F',
                                                 title="Acc. Vertex Z; Vertex z [mm]; Number of vertices",
                                                 xbins=500, xmin=-500.0, xmax=500.0, opt='kLBN')

            # Beam position in X, Y, Z with respect to the nominal position
            # Total number of bins: 300
            # 10 um bins in X and Y
            #  5 mm bins in Z
            self.defineHistogram('VertexXZoomPass', path='EXPERT', type='TH1F',
                                                 title="Acc. Vertex X-X_{nominal}; Vertex X-X_{nominal}  [mm]; Number of vertices",
                                                 xbins=100, xmin=-0.500, xmax=0.500, opt='kLBN')
            self.defineHistogram('VertexYZoomPass', path='EXPERT', type='TH1F',
                                                 title="Acc. Vertex Y-Y_{nominal}; Vertex Y-Y_{nominal}  [mm]; Number of vertices",
                                                 xbins=100, xmin=-0.500, xmax=0.500, opt='kLBN')
            self.defineHistogram('VertexZZoomPass', path='EXPERT', type='TH1F',
                                                 title="Acc. Vertex Z-Z_{nominal}; Vertex Z-Z_{nominal}  [mm]; Number of vertices",
                                                 xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN')

        if detail >= 1:

            # Track d0 vs phi: another method for extracting the beam position on average
            # Total number of bins: 70
            self.defineHistogram('TrackPhiPass, TrackD0Pass',path='EXPERT', type='TProfile',
                                                 title="Selected Track d_{0} vs #phi; Track  #phi; Track d_{0}",
                                                 xbins=70, xmin=-3.5, xmax=3.5, opt='kLBN')

            # Beam profile plots: X-Y, X-Z, Y-Z
            # Total number of bins: 2700
            # 100 um bins in X and Y
            self.defineHistogram('VertexXPass, VertexYPass',path='EXPERT', type='TH2F',
                                                 title="Acc. Vertex Y vs X; Vertex X [mm]; Vertex Y [mm]",
                                                 xbins=50, xmin=-2.5, xmax=2.5, ybins=50, ymin=-2.5, ymax=2.5, opt='kLBN')
            self.defineHistogram('VertexZPass, VertexXPass',path='EXPERT', type='TProfile',
                                                 title="Acc. Vertex X vs Z; Vertex Z [mm]; Vertex X [mm]",
                                                 xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN')
            self.defineHistogram('VertexZPass, VertexYPass',path='EXPERT', type='TProfile',
                                                 title="Acc. Vertex Y vs Z; Vertex Z [mm]; Vertex Y [mm]",
                                                 xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN')

            # Beam profile w.r.t. the nominal position: X - X_nominal , etc
            # Total number of bins: 2700
            #  50 um bins in X and Y
            self.defineHistogram('VertexXZoomPass, VertexYZoomPass',path='EXPERT', type='TH2F',
                                                 title="Acc. Vertex Y-Y_{nominal} vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Y-Y_{nominal} [mm]",
                                                 xbins=50, xmin=-1.25, xmax=1.25, ybins=50, ymin=-1.25, ymax=1.25, opt='kLBN')
            self.defineHistogram('VertexZZoomPass, VertexXZoomPass',path='EXPERT', type='TProfile',
                                                 title="Acc. Vertex X-X_{nominal} vs Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Vertex X-X_{nominal} [mm]",
                                                 xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN')
            self.defineHistogram('VertexZZoomPass, VertexYZoomPass',path='EXPERT', type='TProfile',
                                                 title="Acc. Vertex Y-Y_{nominal} vs Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Vertex Y-Y_{nominal} [mm]",
                                                 xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN')

            # Beam position in X, Y, Z vs. Ntrk in vertex: allows to correct width for each vertex multiplicity
            # Total number of bins: 85,000 -> 105,000
            # 10 um bins in X and Y
            #  2 mm bins in Z
            self.defineHistogram('VertexNTrksPass, VertexXPass', path='EXPERT', type='TH2F',
                                                 title="Acc. Vertex X vs. NTrks; N trk per vertex; Vertex x [mm]",
                                                 xbins=100, xmin=0.0, xmax=100.0, ybins=400, ymin=-2.00, ymax=2.00, opt='kLBN')
            self.defineHistogram('VertexNTrksPass, VertexYPass', path='EXPERT', type='TH2F',
                                                 title="Acc. Vertex Y vs. NTrks; N trk per vertex; Vertex y [mm]",
                                                 xbins=100, xmin=0.0, xmax=100.0, ybins=400, ymin=-2.00, ymax=2.00, opt='kLBN')
            self.defineHistogram('VertexNTrksPass, VertexZPass', path='EXPERT', type='TH2F',
                                                 title="Acc. Vertex Z vs. NTrks; N trk per vertex; Vertex z [mm]",
                                                 xbins=100, xmin=0.0, xmax=100.0, ybins=250, ymin=-250.0, ymax=250.0, opt='kLBN')

            # Delta between the Split Vertex positions in X, Y, Z vs. Ntrk in vertex:
            # allows to correct width for each vertex multiplicity
            # Total number of bins: 150,000 -> 75,000
            # 10 um bins in X and Y
            # 2 mm bins in Z
            self.defineHistogram('SplitVertexDNTrksPass, SplitVertexDXPass', path='EXPERT', type='TH2F',
                                                 title="Split Split Vertex DX vs. NTrks; N trk per split vertex; #Deltax between split vertices [mm]",
                                                 xbins=100, xmin=0.0, xmax=50.0, ybins=250, ymin=-1.25, ymax=1.25, opt='kLBN')
            self.defineHistogram('SplitVertexDNTrksPass, SplitVertexDYPass', path='EXPERT', type='TH2F',
                                                 title="Split Vertex DY vs. NTrks; N trk per split vertex; #Deltay between split vertices [mm]",
                                                 xbins=100, xmin=0.0, xmax=50.0, ybins=250, ymin=-1.25, ymax=1.25, opt='kLBN')
            self.defineHistogram('SplitVertexDNTrksPass, SplitVertexDZPass', path='EXPERT', type='TH2F',
                                                 title="Split Vertex DZ vs. NTrks; N trk per split vertex; #Deltaz between split vertices [mm]",
                                                 xbins=100, xmin=0.0, xmax=50.0, ybins=250, ymin=-2.5, ymax=2.5, opt='kLBN')

            #---------------------------------------
            # Per BCID, per luminosity block histograms
            #---------------------------------------
            # Total number of per-BCID, per-LBN bins:
            #                  552,420
            #---------------------------------------

            # Bunch-to-bunch luminosity and bunch-profile information
            # Total number of bins: 17,820 (5*3,564)
            self.defineHistogram('BCID, TotalTracksPass',path='EXPERT', type='TProfile',
                                                 title="Acc. N Tracks vs BCID; BCID; N tracks vertices",
                                                 xbins=3564, xmin=0, xmax=3564, opt='kLBN')

            self.defineHistogram('BCID, NvtxPassBCID',path='EXPERT', type='TProfile',
                                                 title="Acc. N Vertices vs BCID; BCID; N accepted vertices",
                                                 xbins=3564, xmin=0, xmax=3564, opt='kLBN')

            self.defineHistogram('BCID, VertexXPassBCID',path='EXPERT', type='TProfile',
                                                 title="Acc. Vertex X vs BCID (profile); BCID; Vertex X [mm]",
                                                 xbins=3564, xmin=0, xmax=3564, opt='kLBN')
            self.defineHistogram('BCID, VertexYPassBCID',path='EXPERT', type='TProfile',
                                                 title="Acc. Vertex Y vs BCID (profile); BCID; Vertex Y [mm]",
                                                 xbins=3564, xmin=0, xmax=3564, opt='kLBN')
            self.defineHistogram('BCID, VertexZPassBCID',path='EXPERT', type='TProfile',
                                                 title="Acc. Vertex Z vs BCID (profile); BCID; Vertex Z [mm]",
                                                 xbins=3564, xmin=0, xmax=3564, opt='kLBN')

        if detail >= 2:

            # Bunch-to-bunch 2D histograms
            # Total number of bins: 534,600 ((50+50+50) * 3,564)
            # 20 um bins in X and Y (compared to 10 um for 1D version)
            # 10 mm bins in Z (compared to 5 mm bins for 1D version)

            self.defineHistogram('BCID, VertexXZoomPassBCID', path='EXPERT', type='TH2F',
                                                 title="Acc. Vertex X-X_{nominal} vs. BCID; BCID; Vertex X-X_{nominal} [mm]",
                                                 xbins=3564, xmin=0, xmax=3564, ybins=50, ymin=-0.5, ymax=0.5, opt='kLBN')
            self.defineHistogram('BCID, VertexYZoomPassBCID', path='EXPERT', type='TH2F',
                                                 title="Acc. Vertex Y-Y_{nominal} vs. BCID; BCID; Vertex Y-Y_{nominal} [mm]",
                                                 xbins=3564, xmin=0, xmax=3564, ybins=50, ymin=-0.5, ymax=0.5, opt='kLBN')
            self.defineHistogram('BCID, VertexZZoomPassBCID', path='EXPERT', type='TH2F',
                                                 title="Acc. Vertex Z-Z_{nominal} vs. BCID; BCID; Vertex Z-Z_{nominal} [mm]",
                                                 xbins=3564, xmin=0, xmax=3564, ybins=50, ymin=-250.0, ymax=250.0, opt='kLBN')

        if detail >= 2:

            # Track Z cluster properties
            # Total number of bins: 425
            # FIXME: addt'l number of bins below still need to be added to tally

            self.defineHistogram('NClusters', path='EXPERT', type='TH1F',
                                                 title="NClusters; Number of clusters per event; Number of events",
                                                 xbins=30, xmin=0.0, xmax=30.0)
            self.defineHistogram('ClusterNTracks', path='EXPERT', type='TH1F',
                                                 title="ClusterNTracks; Number tracks per cluster; Number of events",
                                                 xbins=100, xmin=0.0, xmax=100.0)
            self.defineHistogram('ClusterNTracks, ClusterNTracksUnused', path='EXPERT', type='TH2F',
                                                 title="Number of tracks in cluster unused vs used; Number of tracks per cluster; Number of unused tracks per cluster",
                                                 xbins=50, xmin=0.0, xmax=50.0, ybins=50, ymin=0.0, ymax=50.0)
            self.defineHistogram('ClusterZ', path='EXPERT', type='TH1F',
                                                 title="ClusterZ; Cluster Z position [mm]; Number of clusters",
                                                 xbins=200, xmin=-250.0, xmax=250.0)
            self.defineHistogram('ClusterDeltaZ0', path='EXPERT', type='TH1F',
                                                 title="ClusterDeltaZ0; Delta of track Z0 and cluster Z position [mm]; Number of tracks",
                                                 xbins=200, xmin=-10.0, xmax=10.0)
            self.defineHistogram('ClusterZ0Pull', path='EXPERT', type='TH1F',
                                                 title="ClusterZ0Pull; Pull of track Z0 with respect to cluster Z position; Number of tracks",
                                                 xbins=200, xmin=-10.0, xmax=10.0)
            self.defineHistogram('ClusterDeltaVertexZ', path='EXPERT', type='TH1F',
                                                 title="ClusterDeltaVertexZ; Delta of cluster Z position and Vertex Z position [mm]; Number of vertices",
                                                 xbins=200, xmin=-10.0, xmax=10.0)

            # Vertex properties
            # Total number of bins: 200
            self.defineHistogram('VertexSumPtPass', path='EXPERT', type='TH1F',
                                                 title="Acc. Vertex Sum p_{T}; Vertex #sum p_{T} [GeV]; Number of vertices",
                                                 xbins=100, xmin=0., xmax=100.)

            self.defineHistogram('VertexSumPt2Pass', path='EXPERT', type='TH1F',
                                                 title="Acc. Vertex #sqrt{Sum p_{T}^{2}}; Vertex #sqrt{#sum p_{T}^{2}} [GeV]; Number of vertices",
                                                 xbins=100, xmin=0., xmax=100.)

            # Split vertex properties
            self.defineHistogram('SplitVertex2NTrksPass, SplitVertex1NTrksPass',path='EXPERT', type='TH2F',
                                                 title="Split Vertex 2 NTrks vs Split Vertex 1 NTrks",
                                                 xbins=50, xmin=0, xmax=50, ybins=50, ymin=0, ymax=50)

            self.defineHistogram('VertexNTrksPass, VertexNTrksInVtxPass', path='EXPERT', type='TH2F',
                                                 title="Acc. Vertex NTrks vs NTrks in vertex; Vertex number of tracks; Number of tracks in vertex track list",
                                                 xbins=50, xmin=0, xmax=50, ybins=50, ymin=0., ymax=50, opt='kLBN')
                       
#================================================================================
# Cosmic
#================================================================================
#class T2VertexBeamSpotCosmicMonitoring(TrigGenericMonitoringToolConfig):
class T2VertexBeamSpotCosmicMonitoring(GenericMonitoringTool):
    def __init__ (self, name="T2VertexBeamSpotCosmicMonitoring"):
        super(T2VertexBeamSpotCosmicMonitoring, self).__init__(name)
        #self.defineTarget("Cosmic")


#TODO rename the class to something more suitable! 
class T2VertexBeamSpotAlgMonitoring(GenericMonitoringTool):
    def __init__ (self, name="T2VertexBeamSpotAlgMonitoring"):
        super(T2VertexBeamSpotAlgMonitoring, self).__init__(name)


        # Event survival: tells you what selections are cutting out events
        # Total number of bins: 9
        self.defineHistogram('EventStatistics', path='EXPERT', type='TH1F',
                                             title="EventStatistics",
                                             xbins=9, xmin=-0.5, xmax=8.5, opt='kLBN',
                                             #labels='All input : Has input TE : Has ROI : Has tracks : Has seed tracks : Has  enough tracks : Has track Z cluster : Has vertex : Has good vertex' )
                                             labels= ['All input', 'Has input TE', 'Has ROI',  'Has tracks', 'Has seed tracks',  'Has  enough tracks', 'Has track Z cluster', 'Has vertex', 'Has good vertex'] )

        self.defineHistogram('nTotalTracks', path='EXPERT', type='TH1F',
                                             title="nTotalTracks; N all tracks per event; Number of events",
                                             xbins=700, xmin=-0.5, xmax=699.0)


        self.defineHistogram('nTotalPassedTracks', path='EXPERT', type='TH1F',
                                             title="nTotalPassedTracks; N selected tracks per event; Number of events",
                                             xbins=700, xmin=-0.5, xmax=699.0)


        self.defineHistogram('nTotalHighPTTracks', path='EXPERT', type='TH1F',
                                             title="nTotalHighPTTracks; N all tracks per event; Number of events",
                                             xbins=500, xmin=-0.5, xmax=499.0)


        self.defineHistogram('nPerVecTETracks', path='EXPERT', type='TH1F',
                                             title="nPerVecTETracks; N all tracks per event; Number of events",
                                             xbins=700, xmin=-0.5, xmax=699.0)


        self.defineHistogram('nPerVecTEPassedTracks', path='EXPERT', type='TH1F',
                                             title="nPerVecTEPassedTracks; N selected tracks per event; Number of events",
                                             xbins=700, xmin=-0.5, xmax=699.0)


        self.defineHistogram('nPerVecTEHighPTTracks', path='EXPERT', type='TH1F',
                                             title="nPerVecTEHighPTTracks; N all tracks per event; Number of events",
                                             xbins=500, xmin=-0.5, xmax=499.0)


        self.defineHistogram('nPerTETracks', path='EXPERT', type='TH1F',
                                             title="nPerTETracks; N all tracks per event; Number of events",
                                             xbins=700, xmin=-0.5, xmax=699.0)


        self.defineHistogram('nPerTEPassedTracks', path='EXPERT', type='TH1F',
                                             title="nPerTEPassedTracks; N selected tracks per event; Number of events",
                                             xbins=700, xmin=-0.5, xmax=699.0)


        self.defineHistogram('nPerTEHighPTTracks', path='EXPERT', type='TH1F',
                                             title="nPerTEHighPTTracks; N all tracks per event; Number of events",
                                             xbins=500, xmin=-0.5, xmax=499.0)

        self.defineHistogram('TIME_TotalTime', path='EXPERT', type='TH1I',
              title="Timing track selection; time [#mus];",
              xbins=1000, xmin=0, xmax=100000)



#Different name for Run2/Run3?
        self.defineHistogram('TIME_LoopAllTEs', path='EXPERT', type='TH1I',
              title="Time to select tracks from all Trigger elements; time [#mus];",
              xbins=1000, xmin=0, xmax=100000)


#Different name for Run2/Run3?
        self.defineHistogram('TIME_EachTE', path='EXPERT', type='TH1I',
              title="Time to select tracks from all Trigger elements; time [#mus];",
              xbins=1000, xmin=0, xmax=100000)


        self.defineHistogram('TIME_toCreateOutput', path='EXPERT', type='TH1I',
                      title="Time to create output; time [#mus];",
                  xbins=1000, xmin=0, xmax=100000)

#TODO rename the class to something more suitable! 
class T2VertexBeamSpotToolMonitoring(GenericMonitoringTool):
    def __init__ (self, name="T2VertexBeamSpotToolMonitoring"):
        super(T2VertexBeamSpotToolMonitoring, self).__init__(name)

        self.defineHistogram('TIME_TrackSelection', path='EXPERT', type='TH1I',
              title="Timing track selection; time [#mus];",
              xbins=1000, xmin=0, xmax=100000)

        self.defineHistogram('TIME_VertexReconstruction', path='EXPERT', type='TH1I',
                      title="Time to reconstruct the vertex ; time [#mus];",
                  xbins=1000, xmin=0, xmax=100000)

        self.defineHistogram('TIME_SplitVertexReconstruction', path='EXPERT', type='TH1I',
                      title="Timing to reconstruct split vertex; time [#mus];",
                  xbins=1000, xmin=0, xmax=100000)

        self.defineHistogram('TIME_toSortTracks', path='EXPERT', type='TH1I',
                      title="Time to sort tracks; time [#mus];",
                  xbins=1000, xmin=0, xmax=100000)

        self.defineHistogram('TIME_toSortSplitTracks', path='EXPERT', type='TH1I',
                      title="Time to sort tracks; time [#mus];",
                  xbins=1000, xmin=0, xmax=100000)

        self.defineHistogram('TIME_toVertexFitSplit', path='EXPERT', type='TH1I',
                      title="Time ; time [#mus];",
                  xbins=1000, xmin=0, xmax=100000)

        self.defineHistogram('TIME_toZClusterSplit', path='EXPERT', type='TH1I',
                      title="Time to cluster (in z) tracks around the (split) seed; time [#mus];",
                  xbins=1000, xmin=0, xmax=100000)
        self.defineHistogram('TIME_toZCluster', path='EXPERT', type='TH1I',
                      title="Time to cluster (in z) tracks around the seed; time [#mus];",
                  xbins=1000, xmin=0, xmax=100000)


        #self.defineHistogram('TIME_toVertexFitSplit', path='EXPERT', type='TH1I',
        #              title="Time ; time [#mus];",
        #          xbins=1000, xmin=0, xmax=100000)



#       #Track counts  (Per ROI)
        self.defineHistogram('TracksPerROI', path='EXPERT', type='TH1F',
                                             title="TracksPerROI; N tracks per ROI; Number of ROIs",
                                             xbins=500, xmin=-0.5, xmax=499.5)
        self.defineHistogram('SelectedTracksPerROI', path='EXPERT', type='TH1F',
                                             title="TracksPerROIPass; N accepted tracks per ROI; Number of ROIs",
                                             #xbins=25, xmin=0, xmax=50)
                                             xbins=400, xmin=-0.5, xmax=399.5)

        self.defineHistogram('SelectedHiPTTracksPerROI', path='EXPERT', type='TH1F',
                                             title="HiPtTracksPerROIPass; N accepted High pT tracks per ROI; Number of ROIs",
                                             #xbins=25, xmin=0, xmax=50)
                                             xbins=200, xmin=-0.5, xmax=199.5)


        # Number of vertices
        # Total number of bins: 25
        self.defineHistogram('NvtxPass', path='EXPERT', type='TH1F',
                                             title="NvtxPass; N accepted vertices per event; Number of events",
                                             xbins=30, xmin=0.0, xmax=30.0, opt='kLBN')

        # Number of tracks per vertex
        # Total number of bins: 100
        self.defineHistogram('VertexNTrksPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex NTrks; N trk per vertex; Number of vertices",
                                             xbins=100, xmin=0, xmax=100, opt='kLBN')

        # Beam position in X, Y, Z: provides mean position and can extract width (convoluted with vertexing precision)
        # Total number of bins: 2500
        # 10 um bins in X and Y
        #  5 mm bins in Z
        self.defineHistogram('VertexXPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex X; Vertex x [mm]; Number of vertices",
                                             xbins=1000, xmin=-5.0, xmax=5.0,   opt='kLBN')
        self.defineHistogram('VertexYPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex Y; Vertex y [mm]; Number of vertices",
                                             xbins=1000, xmin=-5.0, xmax=5.0,   opt='kLBN')
        self.defineHistogram('VertexZPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex Z; Vertex z [mm]; Number of vertices",
                                             xbins=500, xmin=-500.0, xmax=500.0, opt='kLBN')

        # Beam position in X, Y, Z with respect to the nominal position
        # Total number of bins: 300
        # 10 um bins in X and Y
        #  5 mm bins in Z
        self.defineHistogram('VertexXZoomPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex X-X_{nominal}; Vertex X-X_{nominal}  [mm]; Number of vertices",
                                             xbins=100, xmin=-0.500, xmax=0.500, opt='kLBN')
        self.defineHistogram('VertexYZoomPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex Y-Y_{nominal}; Vertex Y-Y_{nominal}  [mm]; Number of vertices",
                                             xbins=100, xmin=-0.500, xmax=0.500, opt='kLBN')
        self.defineHistogram('VertexZZoomPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex Z-Z_{nominal}; Vertex Z-Z_{nominal}  [mm]; Number of vertices",
                                             xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN')

        # Track d0 vs phi: another method for extracting the beam position on average
        # Total number of bins: 70
        self.defineHistogram('TrackPhiPass, TrackD0Pass',path='EXPERT', type='TProfile',
                                             title="Selected Track d_{0} vs #phi; Track  #phi; Track d_{0}",
                                             xbins=70, xmin=-3.5, xmax=3.5, opt='kLBN')

        # Beam profile plots: X-Y, X-Z, Y-Z
        # Total number of bins: 2700
        # 100 um bins in X and Y
        self.defineHistogram('VertexXPass, VertexYPass',path='EXPERT', type='TH2F',
                                             title="Acc. Vertex Y vs X; Vertex X [mm]; Vertex Y [mm]",
                                             xbins=50, xmin=-2.5, xmax=2.5, ybins=50, ymin=-2.5, ymax=2.5, opt='kLBN')
        self.defineHistogram('VertexZPass, VertexXPass',path='EXPERT', type='TProfile',
                                             title="Acc. Vertex X vs Z; Vertex Z [mm]; Vertex X [mm]",
                                             xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN')
        self.defineHistogram('VertexZPass, VertexYPass',path='EXPERT', type='TProfile',
                                             title="Acc. Vertex Y vs Z; Vertex Z [mm]; Vertex Y [mm]",
                                             xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN')

        # Beam profile w.r.t. the nominal position: X - X_nominal , etc
        # Total number of bins: 2700
        #  50 um bins in X and Y
        self.defineHistogram('VertexXZoomPass, VertexYZoomPass',path='EXPERT', type='TH2F',
                                             title="Acc. Vertex Y-Y_{nominal} vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Y-Y_{nominal} [mm]",
                                             xbins=50, xmin=-1.25, xmax=1.25, ybins=50, ymin=-1.25, ymax=1.25, opt='kLBN')
        self.defineHistogram('VertexZZoomPass, VertexXZoomPass',path='EXPERT', type='TProfile',
                                             title="Acc. Vertex X-X_{nominal} vs Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Vertex X-X_{nominal} [mm]",
                                             xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN')
        self.defineHistogram('VertexZZoomPass, VertexYZoomPass',path='EXPERT', type='TProfile',
                                             title="Acc. Vertex Y-Y_{nominal} vs Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Vertex Y-Y_{nominal} [mm]",
                                             xbins=100, xmin=-250.0, xmax=250.0, opt='kLBN')

        # Beam position in X, Y, Z vs. Ntrk in vertex: allows to correct width for each vertex multiplicity
        # Total number of bins: 85,000 -> 105,000
        # 10 um bins in X and Y
        #  2 mm bins in Z
        self.defineHistogram('VertexNTrksPass, VertexXPass', path='EXPERT', type='TH2F',
                                             title="Acc. Vertex X vs. NTrks; N trk per vertex; Vertex x [mm]",
                                             xbins=100, xmin=0.0, xmax=100.0, ybins=400, ymin=-2.00, ymax=2.00, opt='kLBN')
        self.defineHistogram('VertexNTrksPass, VertexYPass', path='EXPERT', type='TH2F',
                                             title="Acc. Vertex Y vs. NTrks; N trk per vertex; Vertex y [mm]",
                                             xbins=100, xmin=0.0, xmax=100.0, ybins=400, ymin=-2.00, ymax=2.00, opt='kLBN')
        self.defineHistogram('VertexNTrksPass, VertexZPass', path='EXPERT', type='TH2F',
                                             title="Acc. Vertex Z vs. NTrks; N trk per vertex; Vertex z [mm]",
                                             xbins=100, xmin=0.0, xmax=100.0, ybins=250, ymin=-250.0, ymax=250.0, opt='kLBN')

        # Delta between the Split Vertex positions in X, Y, Z vs. Ntrk in vertex:
        # allows to correct width for each vertex multiplicity
        # Total number of bins: 150,000 -> 75,000
        # 10 um bins in X and Y
        # 2 mm bins in Z
        #TODO: OBsolete?
        #self.defineHistogram('SplitVertexDNTrksPass, SplitVertexDXPass', path='EXPERT', type='TH2F',
        #                                     title="Split Split Vertex DX vs. NTrks; N trk per split vertex; #Deltax between split vertices [mm]",
        #                                     xbins=100, xmin=0.0, xmax=50.0, ybins=250, ymin=-1.25, ymax=1.25, opt='kLBN')
        #self.defineHistogram('SplitVertexDNTrksPass, SplitVertexDYPass', path='EXPERT', type='TH2F',
        #                                     title="Split Vertex DY vs. NTrks; N trk per split vertex; #Deltay between split vertices [mm]",
        #                                     xbins=100, xmin=0.0, xmax=50.0, ybins=250, ymin=-1.25, ymax=1.25, opt='kLBN')
        #self.defineHistogram('SplitVertexDNTrksPass, SplitVertexDZPass', path='EXPERT', type='TH2F',
        #                                     title="Split Vertex DZ vs. NTrks; N trk per split vertex; #Deltaz between split vertices [mm]",
        #                                     xbins=100, xmin=0.0, xmax=50.0, ybins=250, ymin=-2.5, ymax=2.5, opt='kLBN')

        #---------------------------------------
        # Per BCID, per luminosity block histograms
        #---------------------------------------
        # Total number of per-BCID, per-LBN bins:
        #                  552,420
        #---------------------------------------

        # Bunch-to-bunch luminosity and bunch-profile information
        # Total number of bins: 17,820 (5*3,564)
        self.defineHistogram('BCID, TotalTracksPass',path='EXPERT', type='TProfile',
                                             title="Acc. N Tracks vs BCID; BCID; N tracks vertices",
                                             xbins=3564, xmin=0, xmax=3564, opt='kLBN')

        self.defineHistogram('BCID, NvtxPassBCID',path='EXPERT', type='TProfile',
                                             title="Acc. N Vertices vs BCID; BCID; N accepted vertices",
                                             xbins=3564, xmin=0, xmax=3564, opt='kLBN')

        self.defineHistogram('BCID, VertexXPassBCID',path='EXPERT', type='TProfile',
                                             title="Acc. Vertex X vs BCID (profile); BCID; Vertex X [mm]",
                                             xbins=3564, xmin=0, xmax=3564, opt='kLBN')
        self.defineHistogram('BCID, VertexYPassBCID',path='EXPERT', type='TProfile',
                                             title="Acc. Vertex Y vs BCID (profile); BCID; Vertex Y [mm]",
                                             xbins=3564, xmin=0, xmax=3564, opt='kLBN')
        self.defineHistogram('BCID, VertexZPassBCID',path='EXPERT', type='TProfile',
                                             title="Acc. Vertex Z vs BCID (profile); BCID; Vertex Z [mm]",
                                             xbins=3564, xmin=0, xmax=3564, opt='kLBN')

        # Bunch-to-bunch 2D histograms
        # Total number of bins: 534,600 ((50+50+50) * 3,564)
        # 20 um bins in X and Y (compared to 10 um for 1D version)
        # 10 mm bins in Z (compared to 5 mm bins for 1D version)

        self.defineHistogram('BCID, VertexXZoomPassBCID', path='EXPERT', type='TH2F',
                                             title="Acc. Vertex X-X_{nominal} vs. BCID; BCID; Vertex X-X_{nominal} [mm]",
                                             xbins=3564, xmin=0, xmax=3564, ybins=50, ymin=-0.5, ymax=0.5, opt='kLBN')
        self.defineHistogram('BCID, VertexYZoomPassBCID', path='EXPERT', type='TH2F',
                                             title="Acc. Vertex Y-Y_{nominal} vs. BCID; BCID; Vertex Y-Y_{nominal} [mm]",
                                             xbins=3564, xmin=0, xmax=3564, ybins=50, ymin=-0.5, ymax=0.5, opt='kLBN')
        self.defineHistogram('BCID, VertexZZoomPassBCID', path='EXPERT', type='TH2F',
                                             title="Acc. Vertex Z-Z_{nominal} vs. BCID; BCID; Vertex Z-Z_{nominal} [mm]",
                                             xbins=3564, xmin=0, xmax=3564, ybins=50, ymin=-250.0, ymax=250.0, opt='kLBN')

        # Track Z cluster properties
        # Total number of bins: 425
        # FIXME: addt'l number of bins below still need to be added to tally

        self.defineHistogram('NClusters', path='EXPERT', type='TH1F',
                                             title="NClusters; Number of clusters per event; Number of events",
                                             xbins=30, xmin=0.0, xmax=30.0)
        self.defineHistogram('ClusterNTracks', path='EXPERT', type='TH1F',
                                             title="ClusterNTracks; Number tracks per cluster; Number of events",
                                             xbins=100, xmin=0.0, xmax=100.0)
        self.defineHistogram('ClusterNTracks, ClusterNTracksUnused', path='EXPERT', type='TH2F',
                                             title="Number of tracks in cluster unused vs used; Number of tracks per cluster; Number of unused tracks per cluster",
                                             xbins=50, xmin=0.0, xmax=50.0, ybins=50, ymin=0.0, ymax=50.0)
        self.defineHistogram('ClusterZ', path='EXPERT', type='TH1F',
                                             title="ClusterZ; Cluster Z position [mm]; Number of clusters",
                                             xbins=200, xmin=-250.0, xmax=250.0)
        self.defineHistogram('ClusterDeltaZ0', path='EXPERT', type='TH1F',
                                             title="ClusterDeltaZ0; Delta of track Z0 and cluster Z position [mm]; Number of tracks",
                                             xbins=200, xmin=-10.0, xmax=10.0)
        self.defineHistogram('ClusterZ0Pull', path='EXPERT', type='TH1F',
                                             title="ClusterZ0Pull; Pull of track Z0 with respect to cluster Z position; Number of tracks",
                                             xbins=200, xmin=-10.0, xmax=10.0)
        self.defineHistogram('ClusterDeltaVertexZ', path='EXPERT', type='TH1F',
                                             title="ClusterDeltaVertexZ; Delta of cluster Z position and Vertex Z position [mm]; Number of vertices",
                                             xbins=200, xmin=-10.0, xmax=10.0)

        # Vertex properties
        # Total number of bins: 200
        self.defineHistogram('VertexSumPtPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex Sum p_{T}; Vertex #sum p_{T} [GeV]; Number of vertices",
                                             xbins=100, xmin=0., xmax=100.)

        self.defineHistogram('VertexSumPt2Pass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex #sqrt{Sum p_{T}^{2}}; Vertex #sqrt{#sum p_{T}^{2}} [GeV]; Number of vertices",
                                             xbins=100, xmin=0., xmax=100.)

        # Split vertex properties
        self.defineHistogram('SplitVertex2NTrksPass, SplitVertex1NTrksPass',path='EXPERT', type='TH2F',
                                             title="Split Vertex 2 NTrks vs Split Vertex 1 NTrks",
                                             xbins=50, xmin=0, xmax=50, ybins=50, ymin=0, ymax=50)

        self.defineHistogram('VertexNTrksPass, VertexNTrksInVtxPass', path='EXPERT', type='TH2F',
                                                 title="Acc. Vertex NTrks vs NTrks in vertex; Vertex number of tracks; Number of tracks in vertex track list",
                                                 xbins=50, xmin=0, xmax=50, ybins=50, ymin=0., ymax=50, opt='kLBN')
                       

#
#        # Per TE
#        self.defineHistogram('ROIperTE', path='EXPERT', type='TH1F',
#                                             title="ROIperTE; N ROIs per TE; Number of TEs",
#                                             xbins=20, xmin=0, xmax=20)
#        self.defineHistogram('TracksPerTE', path='EXPERT', type='TH1F',
#                                             title="TracksPerTE; N tracks per TE; Number of TEs",
#                                             xbins=75, xmin=0, xmax=150)
#        self.defineHistogram('TracksPerTEPass', path='EXPERT', type='TH1F',
#                                             title="TracksPerTEPass; N accepted tracks per TE; Number of TEs",
#                                             xbins=75, xmin=0, xmax=150)
#

      # ACCEPTED Tracks parameters
      #-------------------------------------------------------------------------------------------------
        self.defineHistogram('TrackPtPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Pt; p_{t} [GeV]; Number of tracks",
                                             xbins=50, xmin=0.0, xmax=50.0)
        self.defineHistogram('TrackEtaPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Eta; #eta; Number of tracks",
                                             xbins=60, xmin=-3.0, xmax=3.0)
        self.defineHistogram('TrackPhiPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Phi; #phi; Number of tracks",
                                             xbins=70, xmin=-3.5, xmax=3.5)
        self.defineHistogram('TrackZ0Pass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Z0; Track z0 [mm]; Number of tracks",
                                             xbins=100, xmin=-10.0, xmax=10.0)
        self.defineHistogram('TrackD0Pass', path='EXPERT', type='TH1F',
                                             title="Acc. Track D0; Track d0 [mm]; Number of tracks",
                                             xbins=100, xmin=-10.0, xmax=10.0)
        self.defineHistogram('TrackZ0errPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Z0err; Track z0 error [mm]; Number of tracks",
                                             xbins=100, xmin=0., xmax=5.)
        self.defineHistogram('TrackD0errPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track D0err; Track d0 error [mm]; Number of tracks",
                                             xbins=100, xmin=0., xmax=5.)
        self.defineHistogram('TrackNDFPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track NDF; Track NDF; Number of tracks",
                                             xbins=10, xmin=-0.5, xmax=9.5)
        self.defineHistogram('TrackQualPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Qual; Track #chi^{2}/ndf; Number of tracks",
                                             xbins=50, xmin=0., xmax=10.)
        self.defineHistogram('TrackChi2ProbPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track #chi^{2} probability; Track #chi^{2} probability; Number of tracks",
                                             xbins=70, xmin=-0.2, xmax=1.2)
        #Accepted Track hits in ID
        self.defineHistogram('TrackSiHitsPass', path='EXPERT', type='TH1I',
                                             title="Acc. Track Silicon hits; N Si hits; Number of tracks",
                                             xbins=12, xmin=0, xmax=12)
        self.defineHistogram('TrackTRTHitsPass', path='EXPERT', type='TH1I',
                                             title="Acc. Track TRT hits; N TRT hits; Number of tracks",
                                             xbins=50, xmin=0, xmax=50)
        self.defineHistogram('TrackPIXHitsPass', path='EXPERT', type='TH1I',
                                             title="Acc. Track PIX hits; N PIX hits; Number of tracks",
                                             xbins=7, xmin=0, xmax=7)
        self.defineHistogram('TrackSCTHitsPass', path='EXPERT', type='TH1I',
                                             title="Acc. Track SCT hits; N SCT hits; Number of tracks",
                                             xbins=9, xmin=0, xmax=9)

      # All Vertices parameters
      #-------------------------------------------------------------------------------------------------
      #Number of all vertices in an events
        self.defineHistogram('Nvtx', path='EXPERT', type='TH1F',
                                             title="Nvtx; N vertices per event; Number of events",
                                             xbins=10, xmin=0.0, xmax=10.0)
        self.defineHistogram('VertexNTrks', path='EXPERT', type='TH1F',
                                             title="Vertex NTrks; N trk per vertex; Number of vertices",
                                             xbins=100, xmin=0, xmax=100)
        self.defineHistogram('VertexSumPt', path='EXPERT', type='TH1F',
                                             title="Vertex SumPt; Vertex sum p_{T} [GeV]; Number of vertices",
                                             xbins=100, xmin=0., xmax=100000.)
        self.defineHistogram('VertexSumPt2', path='EXPERT', type='TH1F',
                                             title="Vertex SumPt^2; Vertex sum p_{T}^{2} [GeV^{2}]; Number of vertices",
                                             xbins=100, xmin=0., xmax=100000.)
        self.defineHistogram('VertexQual', path='EXPERT', type='TH1F',
                                             title="Vertex Qual; Vertex #chi^{2}/ndf; Number of vertices",
                                             xbins=50, xmin=0., xmax=50.)
        self.defineHistogram('VertexMass', path='EXPERT', type='TH1F',
                                             title="Vertex mass; Vertex Mass; Number of vertices",
                                             xbins=300, xmin=0., xmax=3000.)
        self.defineHistogram('VertexChi2Prob', path='EXPERT', type='TH1F',
                                             title="Vertex Probability; Vertex #chi^{2} probability; Number of vertices",
                                             xbins=70, xmin=-0.2, xmax=1.2)

        # Beam position in X, Y, Z: before vertex cleaning cuts
        # Total number of bins: 1500
        self.defineHistogram('VertexX', path='EXPERT', type='TH1F',
                                             title="Vertex X; Vertex X [mm]; Number of vertices",
                                             xbins=500, xmin=-5.0, xmax=5.0)
        self.defineHistogram('VertexXerr', path='EXPERT', type='TH1F',
                                             title="Vertex Xerr; Vertex x error [mm]; Number of vertices",
                                             xbins=50, xmin=0., xmax=0.5)
        self.defineHistogram('VertexY', path='EXPERT', type='TH1F',
                                             title="Vertex Y; Vertex Y [mm]; Number of vertices",
                                             xbins=500, xmin=-5.0, xmax=5.0)
        self.defineHistogram('VertexYerr', path='EXPERT', type='TH1F',
                                             title="Vertex Yerr; Vertex y error [mm]; Number of vertices",
                                             xbins=50, xmin=0., xmax=0.5)
        self.defineHistogram('VertexZ', path='EXPERT', type='TH1F',
                                             title="Vertex Z; Vertex Z [mm]; Number of vertices",
                                             xbins=500, xmin=-250.0, xmax=250.0)
        self.defineHistogram('VertexZerr', path='EXPERT', type='TH1F',
                                             title="Vertex Zerr; Vertex z error [mm]; Number of vertices",
                                             xbins=100, xmin=0., xmax=1.)

        self.defineHistogram('VertexXZoom', path='EXPERT', type='TH1F',
                                             title="Vertex X-X_{nominal}; Vertex X-X_{nominal} [mm]; Number of vertices",
                                             xbins=100, xmin=-0.250, xmax=0.250)
        self.defineHistogram('VertexYZoom', path='EXPERT', type='TH1F',
                                             title="Vertex Y-Y_{nominal}; Vertex Y-Y_{nominal} [mm]; Number of vertices",
                                             xbins=100, xmin=-0.250, xmax=0.250)
        self.defineHistogram('VertexZZoom', path='EXPERT', type='TH1F',
                                             title="Vertex Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Number of vertices",
                                             xbins=100, xmin=-250., xmax=250.)

        self.defineHistogram('VertexPull', path='EXPERT', type='TH1F',
                                             title="Vertex 'pull'; Vertex Z_{PV} - Z_{seed} [mm]; Number of vertices",
                                             xbins=100, xmin=-5.0, xmax=5.0)

        self.defineHistogram('VertexNTrksInVtx', path='EXPERT', type='TH1F',
                                                 title="Acc. NTrks in vertex; Vertex number of tracks; Number of tracks in vertex track list",
                                                 xbins=50, xmin=0, xmax=50, opt='kLBN')




      # Passed Vertices parameters
      #-------------------------------------------------------------------------------------------------

        self.defineHistogram('VertexNTrksPass', path='EXPERT', type='TH1F',
                                             title="Vertex NTrks; N trk per vertex; Number of vertices",
                                             xbins=100, xmin=0, xmax=100)
        self.defineHistogram('VertexSumPtPass', path='EXPERT', type='TH1F',
                                             title="Vertex SumPt; Vertex sum p_{T} [GeV]; Number of vertices",
                                             xbins=100, xmin=0., xmax=100000.)
        self.defineHistogram('VertexSumPt2Pass', path='EXPERT', type='TH1F',
                                             title="Vertex SumPt^2; Vertex sum p_{T}^{2} [GeV^{2}]; Number of vertices",
                                             xbins=100, xmin=0., xmax=100000.)
        self.defineHistogram('VertexQualPass', path='EXPERT', type='TH1F',
                                             title="Vertex Qual; Vertex #chi^{2}/ndf; Number of vertices",
                                             xbins=50, xmin=0., xmax=50.)
        self.defineHistogram('VertexMassPass', path='EXPERT', type='TH1F',
                                             title="Vertex mass; Vertex Mass; Number of vertices",
                                             xbins=300, xmin=0., xmax=3000.)
        self.defineHistogram('VertexChi2ProbPass', path='EXPERT', type='TH1F',
                                             title="Vertex Probability; Vertex #chi^{2} probability; Number of vertices",
                                             xbins=70, xmin=-0.2, xmax=1.2)

        # Beam position in X, Y, Z: before vertex cleaning cuts
        # Total number of bins: 1500
        self.defineHistogram('VertexXPass', path='EXPERT', type='TH1F',
                                             title="Vertex X; Vertex X [mm]; Number of vertices",
                                             xbins=500, xmin=-5.0, xmax=5.0)
        self.defineHistogram('VertexXerrPass', path='EXPERT', type='TH1F',
                                             title="Vertex Xerr; Vertex x error [mm]; Number of vertices",
                                             xbins=50, xmin=0., xmax=0.5)
        self.defineHistogram('VertexYPass', path='EXPERT', type='TH1F',
                                             title="Vertex Y; Vertex Y [mm]; Number of vertices",
                                             xbins=500, xmin=-5.0, xmax=5.0)
        self.defineHistogram('VertexYerrPass', path='EXPERT', type='TH1F',
                                             title="Vertex Yerr; Vertex y error [mm]; Number of vertices",
                                             xbins=50, xmin=0., xmax=0.5)
        self.defineHistogram('VertexZPass', path='EXPERT', type='TH1F',
                                             title="Vertex Z; Vertex Z [mm]; Number of vertices",
                                             xbins=500, xmin=-250.0, xmax=250.0)
        self.defineHistogram('VertexZerrPass', path='EXPERT', type='TH1F',
                                             title="Vertex Zerr; Vertex z error [mm]; Number of vertices",
                                             xbins=100, xmin=0., xmax=1.)

        self.defineHistogram('VertexXZoomPass', path='EXPERT', type='TH1F',
                                             title="Vertex X-X_{nominal}; Vertex X-X_{nominal} [mm]; Number of vertices",
                                             xbins=100, xmin=-0.250, xmax=0.250)
        self.defineHistogram('VertexYZoomPass', path='EXPERT', type='TH1F',
                                             title="Vertex Y-Y_{nominal}; Vertex Y-Y_{nominal} [mm]; Number of vertices",
                                             xbins=100, xmin=-0.250, xmax=0.250)
        self.defineHistogram('VertexZZoomPass', path='EXPERT', type='TH1F',
                                             title="Vertex Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Number of vertices",
                                             xbins=100, xmin=-250., xmax=250.)

        self.defineHistogram('VertexPullPass', path='EXPERT', type='TH1F',
                                             title="Vertex 'pull'; Vertex Z_{PV} - Z_{seed} [mm]; Number of vertices",
                                             xbins=100, xmin=-5.0, xmax=5.0)

        self.defineHistogram('VertexNTrksInVtxPass', path='EXPERT', type='TH1F',
                                                 title="Acc. NTrks in vertex; Vertex number of tracks; Number of tracks in vertex track list",
                                                 xbins=50, xmin=0, xmax=50, opt='kLBN')

      #-------------------------------------------------------------------------------------------------

        # 2D Histos
        self.defineHistogram('VertexX, VertexY',path='EXPERT', type='TH2F',
                                             title="Vertex Y vs X; Vertex X [mm]; Vertex Y [mm]",
                                             xbins=50, xmin=-5.0, xmax=5.0, ybins=50, ymin=-5.0, ymax=5.0)
        self.defineHistogram('VertexZ, VertexX',path='EXPERT', type='TH2F',
                                             title="Vertex X vs Z; Vertex X [mm]; Vertex Z [mm]",
                                             xbins=500, xmin=-250.0, xmax=250.0, ybins=50, ymin=-5.0, ymax=5.0)
        self.defineHistogram('VertexZ, VertexY',path='EXPERT', type='TH2F',
                                             title="Vertex Y vs Z; Vertex Y [mm]; Vertex Z [mm]",
                                             xbins=500, xmin=-250.0, xmax=250.0, ybins=50, ymin=-5.0, ymax=5.0)
        self.defineHistogram('VertexXZoom, VertexYZoom',path='EXPERT', type='TH2F',
                                             title="Vertex Y-Y_{nominal} vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Y-Y_{nominal} [mm]",
                                             xbins=100, xmin=-0.250, xmax=0.250, ybins=100, ymin=-0.250, ymax=0.250)

        self.defineHistogram('VertexZ, VertexXerr',path='EXPERT', type='TH2F',
                                             title="Vertex Xerr vs Z; Vertex Z [mm]; Vertex Xerr [mm]",
                                             xbins=500, xmin=-250.0, xmax=250.0, ybins=100, ymin=0.0, ymax=1.0)
        self.defineHistogram('VertexZ, VertexYerr',path='EXPERT', type='TH2F',
                                             title="Vertex Yerr vs Z; Vertex Z [mm]; Vertex Yerr [mm]",
                                             xbins=500, xmin=-250.0, xmax=250.0, ybins=100, ymin=0.0, ymax=1.0)

        # X(Y) vs Xerr(Yerr) for Bayesian Inference method
        self.defineHistogram('VertexXZoomPass, VertexXerrPass',path='EXPERT', type='TH2F',
                                             title="Vertex Xerr vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Xerr [mm]",
                                             xbins=400, xmin=-0.200, xmax=0.200, ybins=100, ymin=0.0, ymax=0.100)
        self.defineHistogram('VertexYZoomPass, VertexYerrPass',path='EXPERT', type='TH2F',
                                             title="Vertex Yerr vs Y-Y_{nominal}; Vertex Y-Y_{nominal} [mm]; Vertex Yerr [mm]",
                                             xbins=400, xmin=-0.200, xmax=0.200, ybins=100, ymin=0.0, ymax=0.100)

        self.defineHistogram('VertexNTrks, VertexXerr',path='EXPERT', type='TH2F',
                                             title="Vertex Xerr vs NTrks; Vertex N trks; Vertex Xerr [mm]",
                                             xbins=25, xmin=5.0, xmax=30.0, ybins=100, ymin=0.0, ymax=1.0)
        self.defineHistogram('VertexNTrks, VertexYerr',path='EXPERT', type='TH2F',
                                             title="Vertex Yerr vs NTrks; Vertex N trks; Vertex Yerr [mm]",
                                             xbins=25, xmin=5.0, xmax=30.0, ybins=100, ymin=0.0, ymax=1.0)
        self.defineHistogram('VertexNTrks, VertexZerr',path='EXPERT', type='TH2F',
                                             title="Vertex Zerr vs NTrks; Vertex N trks; Vertex Zerr [mm]",
                                             xbins=50, xmin=0.0, xmax=50.0, ybins=100, ymin=0.0, ymax=1.0)
#================================================================================
# Validation
#================================================================================
########## ##############################################################
#  add validation specific histograms.
#  If you ever remove histograms from Online - move them into Validation
#
#########################################################################
#class T2VertexBeamSpotValidationMonitoring( TrigGenericMonitoringToolConfig ):
class T2VertexBeamSpotValidationMonitoring(GenericMonitoringTool):
    def __init__ (self, name="T2VertexBeamSpotValidationMonitoring"):
        super(T2VertexBeamSpotValidationMonitoring, self).__init__(name)
        #self.defineTarget("Validation")

        # ACCEPTED Tracks
        self.defineHistogram('TrackSiHitsPass', path='EXPERT', type='TH1I',
                                             title="Acc. Track Silicon hits; N Si hits; Number of tracks",
                                             xbins=12, xmin=0, xmax=12)
        self.defineHistogram('TrackTRTHitsPass', path='EXPERT', type='TH1I',
                                             title="Acc. Track TRT hits; N TRT hits; Number of tracks",
                                             xbins=50, xmin=0, xmax=50)
        self.defineHistogram('TrackPIXHitsPass', path='EXPERT', type='TH1I',
                                             title="Acc. Track PIX hits; N PIX hits; Number of tracks",
                                             xbins=7, xmin=0, xmax=7)
        self.defineHistogram('TrackSCTHitsPass', path='EXPERT', type='TH1I',
                                             title="Acc. Track SCT hits; N SCT hits; Number of tracks",
                                             xbins=9, xmin=0, xmax=9)
#
#        # Per TE
#        self.defineHistogram('ROIperTE', path='EXPERT', type='TH1F',
#                                             title="ROIperTE; N ROIs per TE; Number of TEs",
#                                             xbins=20, xmin=0, xmax=20)
#        self.defineHistogram('TracksPerTE', path='EXPERT', type='TH1F',
#                                             title="TracksPerTE; N tracks per TE; Number of TEs",
#                                             xbins=75, xmin=0, xmax=150)
#        self.defineHistogram('TracksPerTEPass', path='EXPERT', type='TH1F',
#                                             title="TracksPerTEPass; N accepted tracks per TE; Number of TEs",
#                                             xbins=75, xmin=0, xmax=150)
#

        # ACCEPTED Tracks
        self.defineHistogram('TrackPtPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Pt; p_{t} [GeV]; Number of tracks",
                                             xbins=50, xmin=0.0, xmax=50.0)
        self.defineHistogram('TrackEtaPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Eta; #eta; Number of tracks",
                                             xbins=60, xmin=-3.0, xmax=3.0)
        self.defineHistogram('TrackPhiPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Phi; #phi; Number of tracks",
                                             xbins=70, xmin=-3.5, xmax=3.5)
        self.defineHistogram('TrackD0Pass', path='EXPERT', type='TH1F',
                                             title="Acc. Track D0; Track d0 [mm]; Number of tracks",
                                             xbins=100, xmin=-10.0, xmax=10.0)
        self.defineHistogram('TrackZ0errPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Z0err; Track z0 error [mm]; Number of tracks",
                                             xbins=100, xmin=0., xmax=5.)
        self.defineHistogram('TrackD0errPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track D0err; Track d0 error [mm]; Number of tracks",
                                             xbins=100, xmin=0., xmax=5.)
        self.defineHistogram('TrackQualPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track Qual; Track #chi^{2}/ndf; Number of tracks",
                                             xbins=50, xmin=0., xmax=10.)
        self.defineHistogram('TrackChi2ProbPass', path='EXPERT', type='TH1F',
                                             title="Acc. Track #chi^{2} probability; Track #chi^{2} probability; Number of tracks",
                                             xbins=70, xmin=-0.2, xmax=1.2)

#        # ALL Vertices and clusters
        self.defineHistogram('Nvtx', path='EXPERT', type='TH1F',
                                             title="Nvtx; N vertices per event; Number of events",
                                             xbins=10, xmin=0.0, xmax=10.0)
        self.defineHistogram('VertexNTrks', path='EXPERT', type='TH1F',
                                             title="Vertex NTrks; N trk per vertex; Number of vertices",
                                             xbins=100, xmin=0, xmax=100)
        self.defineHistogram('VertexSumPt', path='EXPERT', type='TH1F',
                                             title="Vertex SumPt; Vertex sum p_{T} [GeV]; Number of vertices",
                                             xbins=100, xmin=0., xmax=100000.)
        self.defineHistogram('VertexQual', path='EXPERT', type='TH1F',
                                             title="Vertex Qual; Vertex #chi^{2}/ndf; Number of vertices",
                                             xbins=50, xmin=0., xmax=50.)
        #self.defineHistogram('VertexProb', path='EXPERT', type='TH1F',
        #                                     title="Vertex Probability; Vertex #chi^{2} probability; Number of vertices",
        #                                     xbins=70, xmin=-0.2, xmax=1.2)
        self.defineHistogram('VertexXerr', path='EXPERT', type='TH1F',
                                             title="Vertex Xerr; Vertex x error [mm]; Number of vertices",
                                             xbins=50, xmin=0., xmax=0.5)
        self.defineHistogram('VertexYerr', path='EXPERT', type='TH1F',
                                             title="Vertex Yerr; Vertex y error [mm]; Number of vertices",
                                             xbins=50, xmin=0., xmax=0.5)
        self.defineHistogram('VertexZerr', path='EXPERT', type='TH1F',
                                             title="Vertex Zerr; Vertex z error [mm]; Number of vertices",
                                             xbins=100, xmin=0., xmax=1.)
        self.defineHistogram('VertexXZoom', path='EXPERT', type='TH1F',
                                             title="Vertex X-X_{nominal}; Vertex X-X_{nominal} [mm]; Number of vertices",
                                             xbins=100, xmin=-0.250, xmax=0.250)
        self.defineHistogram('VertexYZoom', path='EXPERT', type='TH1F',
                                             title="Vertex Y-Y_{nominal}; Vertex Y-Y_{nominal} [mm]; Number of vertices",
                                             xbins=100, xmin=-0.250, xmax=0.250)
        self.defineHistogram('VertexZZoom', path='EXPERT', type='TH1F',
                                             title="Vertex Z-Z_{nominal}; Vertex Z-Z_{nominal} [mm]; Number of vertices",
                                             xbins=100, xmin=-250., xmax=250.)
        self.defineHistogram('VertexPull', path='EXPERT', type='TH1F',
                                             title="Vertex 'pull'; Vertex Z_{PV} - Z_{seed} [mm]; Number of vertices",
                                             xbins=100, xmin=-5.0, xmax=5.0)

        # Beam position in X, Y, Z: before vertex cleaning cuts
        # Total number of bins: 1500
        self.defineHistogram('VertexX', path='EXPERT', type='TH1F',
                                             title="Vertex X; Vertex X [mm]; Number of vertices",
                                             xbins=500, xmin=-5.0, xmax=5.0)
        self.defineHistogram('VertexY', path='EXPERT', type='TH1F',
                                             title="Vertex Y; Vertex Y [mm]; Number of vertices",
                                             xbins=500, xmin=-5.0, xmax=5.0)
        self.defineHistogram('VertexZ', path='EXPERT', type='TH1F',
                                             title="Vertex Z; Vertex Z [mm]; Number of vertices",
                                             xbins=500, xmin=-250.0, xmax=250.0)

        # ACCEPTED Vertices
        self.defineHistogram('VertexMassPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex Mass; Vertex mass [GeV]; Number of vertices",
                                             xbins=100, xmin=0., xmax=100000.)
        self.defineHistogram('VertexQualPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex Qual; Vertex #chi^{2}/ndf; Number of vertices",
                                             xbins=50, xmin=0., xmax=50.)
        #self.defineHistogram('VertexProbPass', path='EXPERT', type='TH1F',
        #                                     title="Acc. Vertex Probability; Vertex #chi^{2} probability; Number of vertices",
        #                                     xbins=70, xmin=-0.2, xmax=1.2)
        self.defineHistogram('VertexXerrPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex Xerr; Vertex X error [mm]; Number of vertices",
                                             xbins=50, xmin=0., xmax=0.5)
        self.defineHistogram('VertexYerrPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex Yerr; Vertex Y error [mm]; Number of vertices",
                                             xbins=50, xmin=0., xmax=0.5)
        self.defineHistogram('VertexZerrPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex Zerr; Vertex Z error [mm]; Number of vertices",
                                             xbins=100, xmin=0., xmax=1.)
        self.defineHistogram('VertexPullPass', path='EXPERT', type='TH1F',
                                             title="Acc. Vertex 'pull'; Vertex Z_{PV} - Z_{seed} [mm]; Number of vertices",
                                             xbins=100, xmin=-5.0, xmax=5.0)

        # 2D Histos
        self.defineHistogram('VertexX, VertexY',path='EXPERT', type='TH2F',
                                             title="Vertex Y vs X; Vertex X [mm]; Vertex Y [mm]",
                                             xbins=50, xmin=-5.0, xmax=5.0, ybins=50, ymin=-5.0, ymax=5.0)
        self.defineHistogram('VertexZ, VertexX',path='EXPERT', type='TH2F',
                                             title="Vertex X vs Z; Vertex X [mm]; Vertex Z [mm]",
                                             xbins=500, xmin=-250.0, xmax=250.0, ybins=50, ymin=-5.0, ymax=5.0)
        self.defineHistogram('VertexZ, VertexY',path='EXPERT', type='TH2F',
                                             title="Vertex Y vs Z; Vertex Y [mm]; Vertex Z [mm]",
                                             xbins=500, xmin=-250.0, xmax=250.0, ybins=50, ymin=-5.0, ymax=5.0)
        self.defineHistogram('VertexXZoom, VertexYZoom',path='EXPERT', type='TH2F',
                                             title="Vertex Y-Y_{nominal} vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Y-Y_{nominal} [mm]",
                                             xbins=100, xmin=-0.250, xmax=0.250, ybins=100, ymin=-0.250, ymax=0.250)

        self.defineHistogram('VertexZ, VertexXerr',path='EXPERT', type='TH2F',
                                             title="Vertex Xerr vs Z; Vertex Z [mm]; Vertex Xerr [mm]",
                                             xbins=500, xmin=-250.0, xmax=250.0, ybins=100, ymin=0.0, ymax=1.0)
        self.defineHistogram('VertexZ, VertexYerr',path='EXPERT', type='TH2F',
                                             title="Vertex Yerr vs Z; Vertex Z [mm]; Vertex Yerr [mm]",
                                             xbins=500, xmin=-250.0, xmax=250.0, ybins=100, ymin=0.0, ymax=1.0)

        # X(Y) vs Xerr(Yerr) for Bayesian Inference method
        self.defineHistogram('VertexXZoomPass, VertexXerrPass',path='EXPERT', type='TH2F',
                                             title="Vertex Xerr vs X-X_{nominal}; Vertex X-X_{nominal} [mm]; Vertex Xerr [mm]",
                                             xbins=400, xmin=-0.200, xmax=0.200, ybins=100, ymin=0.0, ymax=0.100)
        self.defineHistogram('VertexYZoomPass, VertexYerrPass',path='EXPERT', type='TH2F',
                                             title="Vertex Yerr vs Y-Y_{nominal}; Vertex Y-Y_{nominal} [mm]; Vertex Yerr [mm]",
                                             xbins=400, xmin=-0.200, xmax=0.200, ybins=100, ymin=0.0, ymax=0.100)

        self.defineHistogram('VertexNTrks, VertexXerr',path='EXPERT', type='TH2F',
                                             title="Vertex Xerr vs NTrks; Vertex N trks; Vertex Xerr [mm]",
                                             xbins=25, xmin=5.0, xmax=30.0, ybins=100, ymin=0.0, ymax=1.0)
        self.defineHistogram('VertexNTrks, VertexYerr',path='EXPERT', type='TH2F',
                                             title="Vertex Yerr vs NTrks; Vertex N trks; Vertex Yerr [mm]",
                                             xbins=25, xmin=5.0, xmax=30.0, ybins=100, ymin=0.0, ymax=1.0)
        self.defineHistogram('VertexNTrks, VertexZerr',path='EXPERT', type='TH2F',
                                             title="Vertex Zerr vs NTrks; Vertex N trks; Vertex Zerr [mm]",
                                             xbins=50, xmin=0.0, xmax=50.0, ybins=100, ymin=0.0, ymax=1.0)


    def defineAlgoSpecificHistograms( self, prefix='' ):
        self.defineHistogram(prefix+'Pt', path='EXPERT', type='TH1F',
                                             title="Track Pt; p_{t} [GeV]; Number of tracks",
                                             xbins=50, xmin=0.0, xmax=50.0)
        self.defineHistogram(prefix+'Eta', path='EXPERT', type='TH1F',
                                             title="Track Eta; #eta; Number of tracks",
                                             xbins=60, xmin=-3.0, xmax=3.0)
        self.defineHistogram(prefix+'Phi', path='EXPERT', type='TH1F',
                                             title="Track Phi; #phi; Number of tracks",
                                             xbins=70, xmin=-3.5, xmax=3.5)
        self.defineHistogram(prefix+'Z0', path='EXPERT', type='TH1F',
                                             title="Track Z0; Track z0 [mm]; Number of tracks",
                                             xbins=100, xmin=-200., xmax=200.)
        self.defineHistogram(prefix+'D0', path='EXPERT', type='TH1F',
                                             title="Track D0; Track d0 [mm]; Number of tracks",
                                             xbins=100, xmin=-10.0, xmax=10.0)
        self.defineHistogram(prefix+'Z0err', path='EXPERT', type='TH1F',
                                             title="Track Z0err; Track z0 error [mm]; Number of tracks",
                                             xbins=100, xmin=0., xmax=5.)
        self.defineHistogram(prefix+'D0err', path='EXPERT', type='TH1F',
                                             title="Track D0err; Track d0 error [mm]; Number of tracks",
                                             xbins=100, xmin=0., xmax=5.)
        self.defineHistogram(prefix+'Qual', path='EXPERT', type='TH1F',
                                             title="Track Qual; Track #chi^{2}/ndf; Number of tracks",
                                             xbins=50, xmin=0., xmax=10.)
        self.defineHistogram(prefix+'Chi2Prob', path='EXPERT', type='TH1F',
                                             title="Track #chi^{2} probability; Track #chi^{2} probability; Number of tracks",
                                             xbins=70, xmin=-0.2, xmax=1.2)
        self.defineHistogram(prefix+'SiHits', path='EXPERT', type='TH1I',
                                             title="Track Silicon hits; N Si hits; Number of tracks",
                                             xbins=12, xmin=0, xmax=12)
        self.defineHistogram(prefix+'TRTHits', path='EXPERT', type='TH1I',
                                             title="Track TRT hits; N TRT hits; Number of tracks",
                                             xbins=50, xmin=0, xmax=50)
