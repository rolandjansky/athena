# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ToolSvc
#
from VrtSecFuzzyConf import VKalVrtAthena__VrtSecFuzzy

class VrtSecFuzzy ( VKalVrtAthena__VrtSecFuzzy )  :

    def __init__(self, name="VrtSecFuzzy"):        

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'VrtSecFuzzy::__init__ ' )
        mlog.info("entering")


        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        TrkVKalVrtFitter = Trk__TrkVKalVrtFitter()
        ToolSvc += TrkVKalVrtFitter

        from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
        AtlasTrackSummaryTool = AtlasTrackSummaryTool()
        mlog.info("AtlasTrackSummaryTool for VrtSecFuzzy created")
        ToolSvc += AtlasTrackSummaryTool


        VKalVrtAthena__VrtSecFuzzy.__init__( self, name
                                             )
