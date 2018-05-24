# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ToolSvc
#
from VrtSecInclusiveConf import VKalVrtAthena__IPAugmentor

class IPAugmentor ( VKalVrtAthena__IPAugmentor )  :

    def __init__(self, name="IPAugmentor"):        

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'IPAugmentor::__init__ ' )
        mlog.info("entering")


        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        TrkVKalVrtFitter = Trk__TrkVKalVrtFitter()
#        print TrkVKalVrtFitter
        ToolSvc += TrkVKalVrtFitter

        from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
        AtlasTrackSummaryTool = AtlasTrackSummaryTool()
#        print AtlasTrackSummaryTool
        mlog.info("AtlasTrackSummaryTool for IPAugmentor created")
        ToolSvc += AtlasTrackSummaryTool


        VKalVrtAthena__IPAugmentor.__init__( self, name )

