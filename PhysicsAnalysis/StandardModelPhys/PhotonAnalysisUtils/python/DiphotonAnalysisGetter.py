# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from RecExConfig.RecFlags  import rec

import traceback
import EventKernel.ParticleDataType

from RecExConfig.Configured import Configured
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.Include import include

from AthenaCommon.Constants import *
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.GlobalFlags import globalflags

class DiphotonAnalysisGetter ( Configured ) :
    
    """ This class configures DiphotonAnalysis related tools and algorithms
    """

    def configure(self):

        from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
        from AthenaCommon.AlgSequence import AlgSequence,AthSequencer,iter_algseq
        mainJob = AlgSequence()
        if(PAUflags.AlgSequence.statusOn):
            theJob = 0
            for c in iter_algseq(mainJob):
                if c.getName() == PAUflags.AlgSequence.StoredValue:
                    theJob = c
            if theJob == 0:
                theJob = mainJob
        else:
            theJob = mainJob

        from AthenaCommon.AppMgr import ToolSvc

        from PhotonAnalysisUtils.PAU_AtlasExtrapolator import PAU_AtlasExtrapolator
        theAtlasExtrapolator = PAU_AtlasExtrapolator()

        # Helper Tool
        #
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionFinderUtils
        InDetConversionHelper = InDet__ConversionFinderUtils(name = "ConversionFinderUtils")
        ToolSvc += InDetConversionHelper

        # Distance of minimum approach utility
        #
        from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__SeedNewtonTrkDistanceFinder
        InDetConversionTrkDistanceFinder = Trk__SeedNewtonTrkDistanceFinder(name = 'InDetConversionTrkDistanceFinder')
        ToolSvc += InDetConversionTrkDistanceFinder 
        
        
        from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
        theAtlasTrackSummaryTool = AtlasTrackSummaryTool("theAtlasTrackSummaryTool")
        ToolSvc += theAtlasTrackSummaryTool
        
        # Single track conversion tool
        #
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__SingleTrackConversionTool
        InDetSingleTrackConversion = InDet__SingleTrackConversionTool(name                       = "SingleTrackConversionTool",
                                                                      ConversionFinderHelperTool = InDetConversionHelper,
                                                                      #TrackSummaryTool           = InDetTrackSummaryTool,
                                                                      TrackSummaryTool           = theAtlasTrackSummaryTool,
                                                                      MinInitialHitRadius        = 70.,
                                                                      MinRatioOfHLhits           = 0.95)#0.5)
        ToolSvc += InDetSingleTrackConversion

        from PhotonAnalysisUtils.VertexFitterTool import VertexFitterTool
        InDetConversionVxFitterTool = VertexFitterTool(theAtlasExtrapolator)

        from PhotonAnalysisUtils.PAU_ExtrapolateToCaloTool import PAU_ExtrapolateToCaloTool
        exToCalo = PAU_ExtrapolateToCaloTool(theAtlasExtrapolator = theAtlasExtrapolator)
        exToCalo.OutputLevel = INFO

        from PhotonAnalysisUtils.PAU_InDetConversionFinderTools import PAU_InDetConversionFinderTools
        InDetConversionFinderTools = PAU_InDetConversionFinderTools(InDetConversionHelper            = InDetConversionHelper,
                                                                    InDetSingleTrackConversion       = InDetSingleTrackConversion,
                                                                    theAtlasExtrapolator             = theAtlasExtrapolator,
                                                                    theAtlasTrackSummaryTool         = theAtlasTrackSummaryTool,
                                                                    InDetConversionTrkDistanceFinder = InDetConversionTrkDistanceFinder,
                                                                    InDetConversionVxFitterTool      = InDetConversionVxFitterTool)

        # Hybrid Primary Vertex Tool
        from PhotonAnalysisUtils.HybridPrimaryVertex import HybridPrimaryVertex
        PAUhybridPrimaryVertexTool = HybridPrimaryVertex(theAtlasExtrapolator        = theAtlasExtrapolator,
                                                         InDetSingleTrackConversion  = InDetSingleTrackConversion,
                                                         InDetConversionVxFitterTool = InDetConversionVxFitterTool,
                                                         exToCalo                    = exToCalo,
                                                         InDetConversionFinderUtils  = InDetConversionHelper)
        PAUhybridPrimaryVertexTool.OutputLevel = WARNING


        # H->gamma gamma fitter tool
        from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUhggFitter
        ToolSvc += PAUhggFitter()
        ToolSvc.PAUhggFitter.ExtrapolationTool=theAtlasExtrapolator

        # Conversion Flag tool
        from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUconversionFlagTool
        ConversionFlagTool = PAUconversionFlagTool("PAUconversionFlagTool",
                                                   ExtrapolateToCaloTool=exToCalo,
                                                   VertexFinderTool = InDetConversionFinderTools,
                                                   SingleTrkConvTool = InDetSingleTrackConversion)
        ToolSvc += ConversionFlagTool
        

        # photon correction tool
        from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import PAUphotonCorrectionTool
        PhotonCorrectionTool = PAUphotonCorrectionTool("PAUphotonCorrectionTool",
                                                       ConvTool = ConversionFlagTool,
                                                       HggFitter = ToolSvc.PAUhggFitter)
        ToolSvc += PhotonCorrectionTool


        from PhotonAnalysisUtils.PhotonAnalysisUtilsConf import DiphotonAnalysis
        diphotonana = DiphotonAnalysis("DiphotonAnalysis")
        diphotonana.HggFitterTool           = ToolSvc.PAUhggFitter
        diphotonana.HybridPrimaryVertexTool = PAUhybridPrimaryVertexTool
        diphotonana.PhotonCorrectionTool    = PhotonCorrectionTool
        diphotonana.PhotonContainer         = PAUflags.PhotonContainer()
        diphotonana.isData                  = (globalflags.DataSource()=='data')
        diphotonana.DiPhotonPtCut           = 25000
        diphotonana.OutputLevel             = WARNING

        theJob+= diphotonana

        return True
