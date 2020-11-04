# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging  import logging
from InDetSVWithMuonTool.InDetSVWithMuonToolConf import InDet__InDetSVWithMuonTool

# define the class
class SecVrtWithMuonFinder( InDet__InDetSVWithMuonTool ):

    def __init__(self, name = 'SecVrtWithMuonFinder'  ):        

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'SecVrtWithMuonFinder::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        # 
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        MuonVertexFitterTool = Trk__TrkVKalVrtFitter(name="MuonVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator"
                                                 )
        ToolSvc += MuonVertexFitterTool
        #----------------------
        # Secondary vertex finder itself
        #
        InDet__InDetSVWithMuonTool.__init__( self, name = name,
                                              VertexFitterTool     = MuonVertexFitterTool
                                            )

