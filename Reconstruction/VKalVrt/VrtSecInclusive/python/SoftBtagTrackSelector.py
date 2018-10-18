# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ToolSvc
#
from VrtSecInclusiveConf import VKalVrtAthena__SoftBtagTrackSelector

class SoftBtagTrackSelector ( VKalVrtAthena__SoftBtagTrackSelector )  :

    def __init__(self, name="SoftBtagTrackSelector"):

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'SoftBtagTrackSelector::__init__ ' )
        mlog.info("entering")


        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        TrkVKalVrtFitter = Trk__TrkVKalVrtFitter()
#        print TrkVKalVrtFitter
        ToolSvc += TrkVKalVrtFitter

        from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
        AtlasTrackSummaryTool = AtlasTrackSummaryTool()
#        print AtlasTrackSummaryTool
        mlog.info("AtlasTrackSummaryTool for SoftBtagTrackSelector created")
        ToolSvc += AtlasTrackSummaryTool


        VKalVrtAthena__SoftBtagTrackSelector.__init__( self,name="SoftBtagTrackSelector")

