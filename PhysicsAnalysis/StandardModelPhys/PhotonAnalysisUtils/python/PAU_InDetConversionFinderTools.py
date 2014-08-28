# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
import traceback
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer

def PAU_InDetConversionFinderTools(InDetConversionHelper,
                                   InDetSingleTrackConversion,
                                   theAtlasExtrapolator,
                                   theAtlasTrackSummaryTool,
                                   InDetConversionTrkDistanceFinder,
                                   InDetConversionVxFitterTool):

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr

    # Track selector tool
    #
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetConversionTrackSelectorTool
    InDetConversionTrackSelector = InDet__InDetConversionTrackSelectorTool(name             = "ConversionTrackSelector",
                                                                           # TrackSummaryTool = InDetTrackSummaryTool,
                                                                           # Extrapolator     = InDetExtrapolator,
                                                                           TrackSummaryTool = theAtlasTrackSummaryTool,
                                                                           Extrapolator     = theAtlasExtrapolator, 
                                                                           maxSiD0          = 10000.,    #50.,      #50.0,   # 10000.,
                                                                           maxTrtD0         = 10000.,    #100.,     #100.,   # 10000.,
                                                                           maxSiZ0          = 10000.,   #350.0,  # 10000.,
                                                                           maxTrtZ0         = 10000.,   #1400.,  # 10000.,
                                                                           minPt            = 500.0,
                                                                           RatioCut1        = 0.0,       #0.5,      #0.5,    # 0.,
                                                                           RatioCut2        = 0.1,      #0.1,    # 0.,
                                                                           RatioCut3        = 0.1      #0.05)   # 0.,
                                                                           #  maxSiD0          = 50.0,
                                                                           #  maxTrtD0         = 100.,
                                                                           #  maxSiZ0          = 10000., 
                                                                           #  maxTrtZ0         = 10000., 
                                                                           #  minPt            = 500.0,
                                                                           #  RatioCut1        = 0.,  #0.5,
                                                                           #  RatioCut2        = 0.,  #0.1,
                                                                           #  RatioCut3        = 0.)  #0.05)
                                                                           )
    
    ToolSvc += InDetConversionTrackSelector
    print      InDetConversionTrackSelector

    # Track pairs selector
    #
    from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__TrackPairsSelector
    InDetConversionTrackPairsSelector = InDet__TrackPairsSelector(name                       = "ConversionTrackPairsSelector",
                                                                  ConversionFinderHelperTool = InDetConversionHelper,
                                                                  DistanceTool               = InDetConversionTrkDistanceFinder,
                                                                  MaxFirstHitRadius          = 500.,
                                                                  MaxDistBetweenTracks       = [10.,50.,50.],#[6.,90.,30.],
                                                                  MaxEta                     = [.3,.5,.5] #[0.5,0.9,0.4],
                                                                  #  MaxInitDistance            = [350.,640.,80.])
                                                                  )
    
    ToolSvc += InDetConversionTrackPairsSelector
    print      InDetConversionTrackPairsSelector


    # Vertex point estimator
    #
    from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
    InDetConversionVtxPointEstimator = InDet__VertexPointEstimator(name                   = "ConversionVtxPointEstimator",
                                                                   # MaxTrkXYDiffAtVtx      = [6.,8.,2.],
                                                                   # MaxTrkZDiffAtVtx       = [80.,380.,80.],
                                                                   # MaxTrkXYValue          = [450.,650.,400.],
                                                                   # MinArcLength           = [30.,80.,350.],
                                                                   # MaxArcLength           = [600.,650.,680.],
                                                                   # MaxChi2OfVtxEstimation = 20.)
                                                                   MinDeltaR              = [-5.,-25.,-50.],
                                                                   MaxDeltaR              = [5.,10.,10.],
                                                                   MaxPhi                 = [0.05, 0.1, 0.1])
    ToolSvc += InDetConversionVtxPointEstimator
    print      InDetConversionVtxPointEstimator


    # Conversion post selector
    #
    from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionPostSelector
    InDetConversionPostSelector = InDet__ConversionPostSelector(name             = "ConversionPostSelector",
                                                                # MaxChi2PerTrack  = [400., 100., 80.],     # [10000.,10000.,10000.],  #[40.,50.,25.],
                                                                MaxChi2Vtx  = [50., 50., 50.],
                                                                MaxInvariantMass = [10000., 10000., 10000.], #[60., 60., 30.],       # [10000.,10000.,10000.],  #[60.,50.,25.],
                                                                MinFitMomentum   = [0., 0., 0.], #[2000., 2000., 2000.], # [0.,0.,0.],              #[2000.,2000.,2000.],
                                                                MinRadius        = [-10000., -10000., -10000.], #[30., 35., 250.],      # [0.,0.,0.],              #[30.,35.,350.],
                                                                MinPt            = 0., # 500.,
                                                                MaxdR            = -10000., #-250.)                 # -10000.)                 #-100.
                                                                MaxPhiVtxTrk     = 10000.)
    ToolSvc += InDetConversionPostSelector
    print      InDetConversionPostSelector
    


    from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__InDetConversionFinderTools
    InDetConversionFinderTools = InDet__InDetConversionFinderTools(name                       = "InDetConversionFinderTools",
                                                                   VertexFitterTool           = InDetConversionVxFitterTool,
                                                                   TrackSelectorTool          = InDetConversionTrackSelector,
                                                                   TrackPairsSelector         = InDetConversionTrackPairsSelector,
                                                                   ConversionFinderHelperTool = InDetConversionHelper,
                                                                   VertexPointEstimator       = InDetConversionVtxPointEstimator,
                                                                   PostSelector               = InDetConversionPostSelector,
                                                                   SingleTrackConversionTool  = InDetSingleTrackConversion,
                                                                   Extrapolator               = theAtlasExtrapolator,
                                                                   TrackParticleCollection    = "TrackParticleCandidate",
                                                                   RemoveTrtTracks            = False,
                                                                   IsConversion               = True)
    ToolSvc += InDetConversionFinderTools
    print      InDetConversionFinderTools

    return InDetConversionFinderTools
