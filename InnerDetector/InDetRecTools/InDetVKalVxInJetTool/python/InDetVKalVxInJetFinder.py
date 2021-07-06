# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging  import logging
from InDetVKalVxInJetTool.InDetVKalVxInJetToolConf import InDet__InDetVKalVxInJetTool

# define the class
class InDetVKalVxInJetFinder( InDet__InDetVKalVxInJetTool ):

    def __init__(self, name = 'InDetVKalVxInJetFinder'  ):

        mlog = logging.getLogger( 'InDetVKalVxInJetFinder::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        #
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        SVertexFitterTool = Trk__TrkVKalVrtFitter(name="SVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator"
                                                 )
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += SVertexFitterTool
        #----------------------
        # Secondary vertex finder itself
        #
        if hasattr(ToolSvc,'InDetParticleCreatorTool'):
          InDet__InDetVKalVxInJetTool.__init__( self, name = name,
                                                VertexFitterTool     = SVertexFitterTool
                                              )
        else:         #--------------------------------
                      # TrackParticle creator is needed
          from TrkParticleCreator.TrkParticleCreatorConf     import Trk__TrackParticleCreatorTool
          if hasattr(ToolSvc,'InDetTrackSummaryTool'):
            SVParticleCreatorTool = Trk__TrackParticleCreatorTool(name="SVParticleCreatorTool",
                                                                  TrackSummaryTool="InDetTrackSummaryTool",
                                                                  UseTrackSummaryTool=False )
          else:
            from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
            SVTrackSummaryTool = AtlasTrackSummaryTool(name="SVTrackSummaryTool")
            ToolSvc += SVTrackSummaryTool
            SVParticleCreatorTool = Trk__TrackParticleCreatorTool(name="SVParticleCreatorTool",
                                                                  TrackSummaryTool=SVTrackSummaryTool,
                                                                  UseTrackSummaryTool=False )
          ToolSvc += SVParticleCreatorTool
          InDet__InDetVKalVxInJetTool.__init__( self, name = name,
                                                VertexFitterTool     = SVertexFitterTool,
                                                TrackParticleCreator = SVParticleCreatorTool
                                              )

##########################################################################################################
# define the class
class AtlasVKalVxInJetFinder( InDet__InDetVKalVxInJetTool ):

    def __init__(self, name = 'AtlasVKalVxInJetFinder'  ):

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'AtlasVKalVxInJetFinder::__init__ ' )
        mlog.info("entering")
        #----------------- ATLAS extrapolator
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        SVAtlasExtrapolator=AtlasExtrapolator()
        ToolSvc+=SVAtlasExtrapolator

        #--------------------------
        from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
        SVAtlasTrackSummaryTool = AtlasTrackSummaryTool()
        ToolSvc += SVAtlasTrackSummaryTool

        #-------------------------
        # TrackParticle creator
        #
        from TrkParticleCreator.TrkParticleCreatorConf     import Trk__TrackParticleCreatorTool
        SVParticleCreatorTool = Trk__TrackParticleCreatorTool(name="SVParticleCreatorTool",
                                                              TrackSummaryTool=SVAtlasTrackSummaryTool,
                                                              UseTrackSummaryTool=False
                                                             )
        ToolSvc += SVParticleCreatorTool
        #----------------------
        # VKalVrt vertex fitter
        #
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        SVertexFitterTool = Trk__TrkVKalVrtFitter(name="SVertexFitterTool",
                                                  Extrapolator=SVAtlasExtrapolator
                                                 )
        ToolSvc += SVertexFitterTool
        #----------------------
        # Secondary vertex finder itself
        #
        from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
        SVLinearizedTrackFactory = Trk__FullLinearizedTrackFactory(name="SVLinearizedTrackFactory",Extrapolator = SVAtlasExtrapolator)
        ToolSvc += SVLinearizedTrackFactory

        from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
        SVTrackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(name="SVTrackToVertexIPEstimator",
                                                                   Extrapolator = SVAtlasExtrapolator,
                                                                   LinearizedTrackFactory=SVLinearizedTrackFactory)
        ToolSvc += SVTrackToVertexIPEstimator

        InDet__InDetVKalVxInJetTool.__init__( self, name = name,
                                             VertexFitterTool     = SVertexFitterTool,
                                             TrackParticleCreator = SVParticleCreatorTool,
                                             TrackSummaryTool     = SVAtlasTrackSummaryTool,
                                             TrackToVertexTool    = SVTrackToVertexIPEstimator
                                            )
