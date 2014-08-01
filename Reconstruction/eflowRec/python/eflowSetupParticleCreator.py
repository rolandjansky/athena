# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
import traceback

class eflowSetupParticleCreator:

    def setup(self):

         mlog = logging.getLogger( 'eflowSetupParticleCreator::setup' )

         try:
             from AthenaCommon.AppMgr import ToolSvc
         except:
             mlog.error("could not import ToolSvc")
             print traceback.format_exc()
             return False

         try:
             from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
             eflowExtrapolator = AtlasExtrapolator(name = "eflowExtrapolator")
         except:
             mlog.error("could not import AtlasExtrapolator from TrkExTools.AtlasExtrapolator")
             print traceback.format_exc()
             return False

         ToolSvc+=eflowExtrapolator

         try:
             from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
             eflowTrackSummaryTool = AtlasTrackSummaryTool(name = "eflowTrackSummaryTool")
         except:
             mlog.error("could not import AtlasTrackSummaryTool from TrkTrackSummaryTool.AtlasTrackSummaryTool")
             print traceback.format_exc()
             return False

         ToolSvc+=eflowTrackSummaryTool

         try:
             from TrkParticleCreator.TrkParticleCreatorConf import  Trk__TrackParticleCreatorTool
             eflowParticleCreatorTool = Trk__TrackParticleCreatorTool(name = "eflowParticleCreatorTool",Extrapolator = eflowExtrapolator, TrackSummaryTool = eflowTrackSummaryTool)
         except:
             mlog.error("could not import Trk__TrackParticleCreatorTool from TrkParticleCreator.TrkParticleCreatorConf")
             print traceback.format_exc()
             return False

        
         
         ToolSvc += eflowParticleCreatorTool

         return eflowParticleCreatorTool
