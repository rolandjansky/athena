# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
# Author: Vadim Kostyukhin vadim.kostyukhin@cern.ch

from AthenaCommon.Logging  import logging
from NewVrtSecInclusiveTool.NewVrtSecInclusiveToolConf import Rec__NewVrtSecInclusiveTool
from NewVrtSecInclusiveTool.NewVrtSecInclusiveToolConf import Rec__NewVrtSecInclusiveAlg

# define the class
# Search for low-pt (soft) B-hadron vertices. 
#------------------------------------
class SoftBFinderTool( Rec__NewVrtSecInclusiveTool ):

    def __init__(self, name = 'SoftBFinderTool'  ):

        from AthenaCommon.AppMgr import ToolSvc
        mlog = logging.getLogger( 'SoftBFinderTool::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        # 
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        SVertexFitterTool = Trk__TrkVKalVrtFitter(name="SoftBVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator")
        ToolSvc += SVertexFitterTool
        #----------------------
        # Soft B-hadron vertex finder itself
        #
        Rec__NewVrtSecInclusiveTool.__init__( self, name = name,
                                             VertexFitterTool   = SVertexFitterTool,
                                             CutPt        = 500,
                                             CutBLayHits  = 1,
                                             CutPixelHits = 3,
                                             CutSiHits    = 8,
                                             CutTRTHits   = 10,
                                             useVertexCleaning  = True,
                                             MultiWithOneTrkVrt = True,
                                             removeTrkMatSignif = -1.,    # No additional material rejection
                                             AntiPileupSigRCut = 2.0,
                                             TrkSigCut         = 2.0,
                                             SelVrtSigCut      = 3.0,
                                             v2tIniBDTCut      =-0.7,
                                             v2tFinBDTCut      = 0.0,
                                             cosSVPVCut        = 0.4,
                                             FastZSVCut        = 5.,
                                             VertexMergeCut    = 4.,   # 3D vertex-vertex dist significance to try merging
                                             MaxSVRadiusCut    = 50    # Inside Pixel BL volume
                                             )
        mlog = logging.getLogger( 'SoftBFinderTool::__configured__ ' )

##########################################################################################################
# define the class
# Configuration for B-hadron search in the ttbar phase space
class InclusiveBFinderTool( Rec__NewVrtSecInclusiveTool ):

    def __init__(self, name = 'InclusiveBFinderTool'  ):

        from AthenaCommon.AppMgr import ToolSvc
        mlog = logging.getLogger( 'InclusiveBFinderTool::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        # 
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        SVertexFitterTool = Trk__TrkVKalVrtFitter(name="InclsusiveBVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator")
        ToolSvc += SVertexFitterTool
        #----------------------
        # Soft B-hadron vertex finder itself
        #
        Rec__NewVrtSecInclusiveTool.__init__( self, name = name,
                                             VertexFitterTool   = SVertexFitterTool,
                                             CutPt        = 500,
                                             CutBLayHits  = 0,
                                             CutPixelHits = 2,
                                             CutSiHits    = 8,
                                             CutTRTHits   = 10,
                                             useVertexCleaning  = True,
                                             MultiWithOneTrkVrt = True,
                                             removeTrkMatSignif = -1.,    # No additional material rejection
                                             AntiPileupSigRCut = 2.0,
                                             TrkSigCut         = 2.0,
                                             SelVrtSigCut      = 3.0,
                                             v2tIniBDTCut      =-0.7,
                                             v2tFinBDTCut      =-0.2,
                                             cosSVPVCut        = 0.5,
                                             FastZSVCut        = 8.,
                                             VertexMergeCut    = 4.,   # 3D vertex-vertex dist significance to try merging
                                             MaxSVRadiusCut    = 140   # Inside Pixel volume
                                             )
##########################################################################################################
# define the class
class HighPtBFinderTool( Rec__NewVrtSecInclusiveTool ):

    def __init__(self, name = 'HighPtBFinderTool'  ):

        from AthenaCommon.AppMgr import ToolSvc
        mlog = logging.getLogger( 'HighPtBFinderTool::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        # 
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        SVertexFitterTool = Trk__TrkVKalVrtFitter(name="HighPtBVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator")
        ToolSvc += SVertexFitterTool
        #----------------------
        # Soft B-hadron vertex finder itself
        #
        Rec__NewVrtSecInclusiveTool.__init__( self, name = name,
                                             VertexFitterTool   = SVertexFitterTool,
                                             CutPt        = 1000,
                                             CutBLayHits  = 0,
                                             CutPixelHits = 2,
                                             CutSiHits    = 8,
                                             useVertexCleaning  = True,
                                             MultiWithOneTrkVrt = True,
                                             removeTrkMatSignif = -1.,    # No additional material rejection
                                             AntiPileupSigRCut = 2.0,
                                             TrkSigCut         = 2.0,
                                             SelVrtSigCut      = 3.0,
                                             v2tIniBDTCut      =-0.6,
                                             v2tFinBDTCut      = 0.2
                                             )
##########################################################################################################
# define the class
# Configuration for hadronic interactions in ID material studies
class MaterialSVFinderTool( Rec__NewVrtSecInclusiveTool ):

    def __init__(self, name = 'MaterialSVFinderTool'  ):

        from AthenaCommon.AppMgr import ToolSvc
        mlog = logging.getLogger( 'MaterialSVFinderTool::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        # 
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        SVertexFitterTool = Trk__TrkVKalVrtFitter(name="MaterialSVVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator")
        ToolSvc += SVertexFitterTool
        #----------------------
        # Soft B-hadron vertex finder itself
        #
        Rec__NewVrtSecInclusiveTool.__init__( self, name = name,
                                             VertexFitterTool   = SVertexFitterTool,
                                             CutBLayHits  = 0,
                                             CutPixelHits = 1,
                                             CutSiHits    = 8,
                                             CutTRTHits   = 10,
                                             useVertexCleaning  = False,
                                             MultiWithOneTrkVrt = False,
                                             removeTrkMatSignif = -1.,    # No additional material rejection
                                             AntiPileupSigRCut = 5.0,
                                             cosSVPVCut        = 0.,    
                                             TrkSigCut         = 5.0,   # Minimal track 3D impact significance
                                             SelVrtSigCut      = 10.0,
                                             v2tIniBDTCut      =-1.01,   #Effectively remove MVA selection
                                             v2tFinBDTCut      =-1.01,   #Effectively remove MVA selection
                                             VrtMassLimit      = 8000.,
                                             Vrt2TrMassLimit   = 8000.,
                                             CutPt             = 500.
                                             )
##########################################################################################################
# define the class
# Configuration for LLP search using LRT 
class DVFinderTool( Rec__NewVrtSecInclusiveTool ):

    def __init__(self, name = 'DVFinderTool'  ):

        from AthenaCommon.AppMgr import ToolSvc
        mlog = logging.getLogger( 'DVFinderTool::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        # 
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        DVertexFitterTool = Trk__TrkVKalVrtFitter(name="DVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator")
        ToolSvc += DVertexFitterTool
        #----------------------
        # Soft B-hadron vertex finder itself
        #
        Rec__NewVrtSecInclusiveTool.__init__( self, name = name,
                                             VertexFitterTool   = DVertexFitterTool,
                                             CutPt        = 1000.,
                                             CutBLayHits  = 0,
                                             CutPixelHits = 0,
                                             CutSiHits    = 7,
                                             CutTRTHits   = 15,
                                             CutD0Max     = 1000.,  # Maximal track impact parameter
                                             CutD0Min     = 0.,     # Minimal track impact parameter
                                             AntiPileupSigRCut = 6.0,    # Remove tracks aroung beamline
                                             TrkSigCut         = 10.0,   # Minimal track 3D impact significance
                                             VrtMassLimit      = 1000000.,
                                             Vrt2TrMassLimit   = 1000000.,
                                             useVertexCleaning  = False,
                                             MultiWithOneTrkVrt = False,
                                             removeTrkMatSignif = -1.,   # No explicit material interation rejection
                                             SelVrtSigCut      = 8.0,
                                             v2tIniBDTCut      =-1.1,  # Disable b-hadron trained BDT here 
                                             v2tFinBDTCut      =-1.1,  # Disable b-hadron trained BDT here
                                             cosSVPVCut        = 0.0,
                                             CutZVrt           = 100.,
                                             VertexMergeCut    = 10.,   # 3D vertex-vertex dist significance to try merging
                                             FastZSVCut        = 30.,   # Pre-selection track-track distance cut
                                             MaxSVRadiusCut    = 350.
                                             )

##########################################################################################################
# define the class
# Configuration for creation of calibration ntuples for 2-track vertex classification BDT
# Version for B-hadrons
#
class V2TCalibrationTool( Rec__NewVrtSecInclusiveTool ):

    def __init__(self, name = 'V2TCalibrationTool'  ):

        from AthenaCommon.AppMgr import ToolSvc
        mlog = logging.getLogger( 'DVFinderTool::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        # 
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        SVertexFitterTool = Trk__TrkVKalVrtFitter(name="SVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator")
        ToolSvc += SVertexFitterTool
        #----------------------
        # Soft B-hadron vertex finder itself
        #
        Rec__NewVrtSecInclusiveTool.__init__( self, name = name,
                                             VertexFitterTool   = SVertexFitterTool,
                                             FillHist     =True,
                                             CutPt        = 400.,   # Lowest track Pt
                                             CutBLayHits  = 0,
                                             CutPixelHits = 1,      # Only vertices inside Pixel are considered
                                             CutSiHits    = 8,
                                             CutTRTHits   = 15,
                                             VrtMassLimit      = 5500.,
                                             Vrt2TrMassLimit   = 4000.,
                                             useVertexCleaning  = False,
                                             MultiWithOneTrkVrt = False,
                                             removeTrkMatSignif = -1.,   # No explicit material interation rejection
                                             AntiPileupSigRCut = 2.0,
                                             TrkSigCut         = 2.0,   # Minimal track 3D impact significance
                                             v2tIniBDTCut      =-1.1,   # Disable BDT usage
                                             v2tFinBDTCut      =-1.1,   # Disable BDT usage
                                             cosSVPVCut        = 0.0,   # Maximal angle between SV-PV and SV momentum
                                             CutD0Max          = 300.,  # Maximal track impact parameter
                                             CutD0Min          = 0.,    # Minimal track impact parameter
                                             CutZVrt           = 100.,
                                             FastZSVCut        = 15.,   # Pre-selection track-track distance cut
                                             MaxSVRadiusCut    = 140.
                                             )

##########################################################################################################
# define the Test algorithms
class AllBVertexFinderAlg( Rec__NewVrtSecInclusiveAlg ):

    def __init__(self, name = 'AllBVertexFinderAlg'  ):

        from AthenaCommon.AppMgr import ToolSvc
        mlog = logging.getLogger( 'AllBVertexFinderAlg::__init__ ' )
        mlog.info("entering")
        
        BFinderTool = InclusiveBFinderTool()
        ToolSvc += BFinderTool
        #----------------------
        # All B-hadron vertex finder itself
        #
        Rec__NewVrtSecInclusiveAlg.__init__( self, name = name,
                                             BVertexTool = BFinderTool
                                            )
