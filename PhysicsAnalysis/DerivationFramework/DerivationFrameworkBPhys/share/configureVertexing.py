#====================================================================
# Collection of tools required by JpsiFinder
# Based on JpsiUpsilonTools/configureServices.py
#====================================================================

class BPHYVertexTools:
 
    def __init__(self, derivation = ""):

        if derivation == "":
            print '--------> FATAL: BPHYVertexTools, "derivation" string not set!'
            import sys
            sys.exit()

        from AthenaCommon.AppMgr import ToolSvc

        from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummarySvc
        InDetTRTStrawStatusSummarySvc = TRT_StrawStatusSummarySvc( name = "InDetTRTStrawStatusSummarySvc" )
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr += InDetTRTStrawStatusSummarySvc
        
        # set up extrapolator
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        self.InDetExtrapolator = AtlasExtrapolator(name             = derivation+"_AtlasExtrapolator")
        ToolSvc += self.InDetExtrapolator
        print      self.InDetExtrapolator

        # Vertex point estimator
        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
        self.VtxPointEstimator = InDet__VertexPointEstimator(name             = derivation+"_VtxPointEstimator",
                                                    MinDeltaR              = [-10000.,-10000.,-10000.],
                                                    MaxDeltaR              = [10000.,10000.,10000.],
                                                    MaxPhi                 = [10000., 10000., 10000.],
                                                    MaxChi2OfVtxEstimation = 2000.)
        ToolSvc += self.VtxPointEstimator
        print self.VtxPointEstimator

        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionFinderUtils
        self.InDetConversionHelper = InDet__ConversionFinderUtils(name = derivation+"_InDetConversionFinderUtils")
        ToolSvc += self.InDetConversionHelper
        print self.InDetConversionHelper

        from InDetRecExample.InDetKeys import InDetKeys

        from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
        self.InDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                     = derivation+"_InDetPrdAssociationTool",
                                                                          PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap())
        ToolSvc += self.InDetPrdAssociationTool
        print self.InDetPrdAssociationTool

        from RecExConfig.RecFlags import rec
        CountDeadModulesAfterLastHit=False
        #rec.Commissioning=False

        from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
        self.InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = derivation+"_InDetHoleSearchTool",
                                                          Extrapolator = self.InDetExtrapolator,
                                                          usePixel      = DetFlags.haveRIO.pixel_on(),
                                                          useSCT        = DetFlags.haveRIO.SCT_on(),
                                                          #Commissioning = rec.Commissioning())
		    				      CountDeadModulesAfterLastHit = CountDeadModulesAfterLastHit)	
        ToolSvc += self.InDetHoleSearchTool
        print self.InDetHoleSearchTool

        from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
        self.InDetTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name         = derivation+"_InDetSummaryHelper",
                                                                     AssoTool     = self.InDetPrdAssociationTool,
                                                                     DoSharedHits = False,
                                                                     HoleSearch   = self.InDetHoleSearchTool,
                                                                     usePixel      = DetFlags.haveRIO.pixel_on(),
                                                                     useSCT        = DetFlags.haveRIO.SCT_on(),
                                                                     useTRT        = DetFlags.haveRIO.TRT_on())
        ToolSvc += self.InDetTrackSummaryHelperTool
        print self.InDetTrackSummaryHelperTool

        from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
        self.InDetTrackSummaryTool = Trk__TrackSummaryTool(name = derivation+"_InDetTrackSummaryTool",
                                                  InDetSummaryHelperTool = self.InDetTrackSummaryHelperTool,
                                                  doSharedHits           = False,
                                                  InDetHoleSearchTool    = self.InDetHoleSearchTool)
        ToolSvc += self.InDetTrackSummaryTool
        print self.InDetTrackSummaryTool

        # =====================================================
        # THIS IS WHERE THE USER CONTROLS MAIN TRACK SELECTIONS
        # =====================================================
        from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
        self.InDetTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = derivation+"_InDetDetailedTrackSelectorTool",
                                                                       pTMin                = 400.0,
                                                                       IPd0Max              = 10000.0,
                                                                       IPz0Max              = 10000.0,
                                                                       z0Max                = 10000.0,
                                                                       sigIPd0Max           = 10000.0,
                                                                       sigIPz0Max           = 10000.0,
                                                                       d0significanceMax    = -1.,
                                                                       z0significanceMax    = -1.,
                                                                       etaMax               = 9999.,
                                                                       useTrackSummaryInfo  = True,
                                                                       nHitBLayer           = 0,
                                                                       nHitPix              = 1,
                                                                       nHitBLayerPlusPix    = 1,
                                                                       nHitSct              = 2,
                                                                       nHitSi               = 3,
                                                                       nHitTrt              = 0,
                                                                       nHitTrtHighEFractionMax = 10000.0,
                                                                       useSharedHitInfo     = False,
                                                                       useTrackQualityInfo  = True,
                                                                       fitChi2OnNdfMax      = 10000.0,
                                                                       TrtMaxEtaAcceptance  = 1.9,
                                                                       TrackSummaryTool     = self.InDetTrackSummaryTool,
                                                                       Extrapolator         = self.InDetExtrapolator
                                                                      )
        
        ToolSvc += self.InDetTrackSelectorTool
        print self.InDetTrackSelectorTool

        # configure vertex fitters
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        self.TrkVKalVrtFitter = Trk__TrkVKalVrtFitter(
                                                 name                = derivation+"_VKalVrtFitter",
                                                 Extrapolator        = self.InDetExtrapolator,
        #                                         MagFieldSvc         = InDetMagField,
                                                 FirstMeasuredPoint  = False,
                                                 #FirstMeasuredPointLimit = True,
                                                 MakeExtendedVertex  = True)
        ToolSvc += self.TrkVKalVrtFitter
        print self.TrkVKalVrtFitter

        from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
        self.InDetLinFactory = Trk__FullLinearizedTrackFactory(name              = derivation+"_Trk::InDetFullLinearizedTrackFactory",
                                                      Extrapolator      = self.InDetExtrapolator,
        #                                                  MagneticFieldTool = InDetMagField
                                                      )
        ToolSvc += self.InDetLinFactory
        print self.InDetLinFactory


        from TrkV0Fitter.TrkV0FitterConf import Trk__TrkV0VertexFitter
        self.TrkV0Fitter = Trk__TrkV0VertexFitter(name              = derivation+"_TrkV0FitterName",
                                         MaxIterations     = 10,
                                         Use_deltaR        = False,
                                         Extrapolator      = self.InDetExtrapolator,
        #                                     MagneticFieldTool = InDetMagField
                                         )
        ToolSvc += self.TrkV0Fitter
        print self.TrkV0Fitter

        # Primary vertex refitting
        from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__KalmanVertexUpdator
        self.VertexUpdator = Trk__KalmanVertexUpdator(name             = derivation+"_KalmanVertexUpdator")
        ToolSvc += self.VertexUpdator
        print      self.VertexUpdator
