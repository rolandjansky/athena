# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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
        InDet__InDetVKalVxInJetTool.__init__( self, name = name,
                                                VertexFitterTool     = SVertexFitterTool
                                              )

# define the class
class InDetVKalVxInHiPtJetFinder( InDet__InDetVKalVxInJetTool ):

    def __init__(self, name = 'InDetVKalVxInJetFinder'  ):

        mlog = logging.getLogger( 'InDetVKalVxInHiPtJetFinder::__init__ ' )
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
        InDet__InDetVKalVxInJetTool.__init__( self, name = name,
                                                VertexFitterTool     = SVertexFitterTool,
                                                CutSharedHits    = 0,
                                                Sel2VrtChi2Cut   = 4.,
                                                CutBVrtScore = 0.002
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


        InDet__InDetVKalVxInJetTool.__init__( self, name = name,
                                             VertexFitterTool     = SVertexFitterTool
                                            )
