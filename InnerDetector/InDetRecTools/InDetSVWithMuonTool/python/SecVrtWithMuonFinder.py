# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *
from AthenaCommon.Include  import Include, IncludeError, include
from AthenaCommon.Logging  import logging
from AthenaCommon.AppMgr   import ToolSvc
from AthenaCommon          import CfgMgr

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

