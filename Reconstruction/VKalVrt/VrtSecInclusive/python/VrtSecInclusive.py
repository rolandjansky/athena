# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from VrtSecInclusive.VrtSecInclusiveConf import VKalVrtAthena__VrtSecInclusive

class VrtSecInclusive ( VKalVrtAthena__VrtSecInclusive )  :

    def __init__(self, name="VrtSecInclusive"):        

        from AthenaCommon.AppMgr import ToolSvc
        mlog = logging.getLogger( 'VrtSecInclusive::__init__ ' )
        mlog.info("entering")


        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        TrkVKalVrtFitter = Trk__TrkVKalVrtFitter()
#        print TrkVKalVrtFitter
        ToolSvc += TrkVKalVrtFitter

        from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
        AtlasTrackSummaryTool = AtlasTrackSummaryTool()
#        print AtlasTrackSummaryTool
        mlog.info("AtlasTrackSummaryTool for VrtSecInclusive created")
        ToolSvc += AtlasTrackSummaryTool


        VKalVrtAthena__VrtSecInclusive.__init__( self,name="VrtSecInclusive")

