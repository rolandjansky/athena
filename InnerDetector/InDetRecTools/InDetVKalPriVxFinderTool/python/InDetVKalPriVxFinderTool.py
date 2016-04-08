# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
#
from InDetVKalPriVxFinderToolConf import InDet__InDetVKalPriVxFinderTool
            
class InDetVKalPriVxFinder ( InDet__InDetVKalPriVxFinderTool )  :

    def __init__(self, name="InDetVKalPriVxFinder"):        

        from AthenaCommon.AppMgr import ToolSvc
        mlog = logging.getLogger( 'InDetVKalPriVxFinder::__init__ ' )
        mlog.info("entering")


        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        TrkVKalVrtPFitter = Trk__TrkVKalVrtFitter()
        ToolSvc += TrkVKalVrtPFitter

        from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
        AtlasTrackSummaryTool = AtlasTrackSummaryTool()
        mlog.info("AtlasTrackSummaryTool for InDetVKalPriVxFinder created")
        ToolSvc += AtlasTrackSummaryTool


        InDet__InDetVKalPriVxFinderTool.__init__( self,name="InDetVKalPriVxFinderTool", 
	                                          TrackSummaryTool=AtlasTrackSummaryTool,
						  FitterTool=TrkVKalVrtPFitter )
