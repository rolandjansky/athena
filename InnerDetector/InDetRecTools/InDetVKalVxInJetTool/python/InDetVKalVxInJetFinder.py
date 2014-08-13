# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *
from AthenaCommon.Include  import Include, IncludeError, include
from AthenaCommon.Logging  import logging
from AthenaCommon.AppMgr   import ToolSvc
from AthenaCommon          import CfgMgr

from InDetVKalVxInJetTool.InDetVKalVxInJetToolConf import InDet__InDetVKalVxInJetTool

# define the class
class InDetVKalVxInJetFinder( InDet__InDetVKalVxInJetTool ):

    def __init__(self, name = 'InDetVKalVxInJetFinder'  ):        

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'InDetVKalVxInJetFinder::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        # 
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        SVertexFitterTool = Trk__TrkVKalVrtFitter(name="SVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator",
                                                  AtlasMagFieldSvc = "AtlasFieldSvc"
                                                  #AtlasMagFieldSvc = "Default",
                                                  #Extrapolator = "DefaultVKalPropagator"
                                                 )
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
        #----------------- ATLAS magnetic field
        from AthenaCommon.AppMgr import ServiceMgr
        from MagFieldServices.MagFieldServicesConf import MagField__AtlasFieldSvc
        ServiceMgr += MagField__AtlasFieldSvc("myAtlasFieldSvc");
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
                                                  Extrapolator=SVAtlasExtrapolator,
                                                  AtlasMagFieldSvc = "myAtlasFieldSvc"
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
