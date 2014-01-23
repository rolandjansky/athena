#
# Magnetic field tool
#
from TrkMagFieldTools.TrkMagFieldToolsConf import Trk__MagneticFieldTool
InDetMagField = Trk__MagneticFieldTool('InDetMagField')
ToolSvc += InDetMagField
print      InDetMagField

#
# set up extrapolator
#
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
InDetExtrapolator = AtlasExtrapolator()
ToolSvc += InDetExtrapolator
print      InDetExtrapolator

# Vertex point estimator
#
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
VtxPointEstimator = InDet__VertexPointEstimator(name                   = "VtxPointEstimator",
                                                MinDeltaR              = [-10000.,-10000.,-10000.],
                                                MaxDeltaR              = [10000.,10000.,10000.],
                                                MaxPhi                 = [10000., 10000., 10000.],
                                                MaxChi2OfVtxEstimation = 2000.)
ToolSvc += VtxPointEstimator

# Helper Tool
#
from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__ConversionFinderUtils
InDetConversionHelper = InDet__ConversionFinderUtils(name = "InDetConversionFinderUtils")
ToolSvc += InDetConversionHelper


from InDetRecExample.InDetKeys import InDetKeys

from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
InDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool",
                                                                      PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap())
ToolSvc += InDetPrdAssociationTool

from RecExConfig.RecFlags import rec
rec.Commissioning=False
#rec.CountDeadModulesAfterLastHit=False

from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool",
                                                      Extrapolator = InDetExtrapolator,
                                                      usePixel      = DetFlags.haveRIO.pixel_on(),
                                                      useSCT        = DetFlags.haveRIO.SCT_on())
                                                      #Commissioning = rec.Commissioning())
						      #CountDeadModulesAfterLastHit = rec.CountDeadModulesAfterLastHit())
ToolSvc += InDetHoleSearchTool

from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
InDetTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name         = "InDetSummaryHelper",
                                                                 AssoTool     = InDetPrdAssociationTool,
                                                                 DoSharedHits = False,
                                                                 HoleSearch   = InDetHoleSearchTool,
                                                                 usePixel      = DetFlags.haveRIO.pixel_on(),
                                                                 useSCT        = DetFlags.haveRIO.SCT_on(),
                                                                 useTRT        = DetFlags.haveRIO.TRT_on())
ToolSvc += InDetTrackSummaryHelperTool

from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
InDetTrackSummaryTool = Trk__TrackSummaryTool(name = "InDetTrackSummaryTool",
                                              InDetSummaryHelperTool = InDetTrackSummaryHelperTool,
                                              doSharedHits           = False,
                                              InDetHoleSearchTool    = InDetHoleSearchTool)
ToolSvc += InDetTrackSummaryTool

# =====================================================
# THIS IS WHERE THE USER CONTROLS MAIN TRACK SELECTIONS
# =====================================================
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
InDetTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectorTool",
                                                               pTMin                = 100,
                                                               IPd0Max              = 4,
                                                               IPz0Max              = 1000,
                                                               z0Max                = 1000,
                                                               sigIPd0Max           = 0.35,
                                                               sigIPz0Max           = 2.5,
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
                                                               nHitTrtHighEFractionMax = 1,
                                                               useSharedHitInfo     = False,
                                                               useTrackQualityInfo  = True,
                                                               fitChi2OnNdfMax      = 3.5,
                                                               TrtMaxEtaAcceptance  = 1.9,
                                                               TrackSummaryTool     = InDetTrackSummaryTool,
                                                               Extrapolator         = InDetExtrapolator
                                                              )

ToolSvc+=InDetTrackSelectorTool
print      InDetTrackSelectorTool

# configure vertex fitters

from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
TrkVKalVrtFitter = Trk__TrkVKalVrtFitter(
                                         name                = "VKalVrtFitterName",
                                         Extrapolator        = InDetExtrapolator,
                                         MagFieldSvc         = InDetMagField,
                                         FirstMeasuredPoint  = True,
                                         #FirstMeasuredPointLimit = True,
                                         MakeExtendedVertex  = True)
ToolSvc += TrkVKalVrtFitter
print      TrkVKalVrtFitter

from TrkV0Fitter.TrkV0FitterConf import Trk__TrkV0VertexFitter
TrkV0Fitter = Trk__TrkV0VertexFitter(name              = 'TrkV0FitterName',
                                     MaxIterations     = 10,
                                     Use_deltaR        = False,
                                     Extrapolator      = InDetExtrapolator,
                                     MagneticFieldTool = InDetMagField)
ToolSvc += TrkV0Fitter
print      TrkV0Fitter

#
# --- load linearized track factory
#
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
InDetLinFactory = Trk__FullLinearizedTrackFactory(name              = "InDetFullLinearizedTrackFactory",
                                                  Extrapolator      = InDetExtrapolator,
                                                  MagneticFieldTool = InDetMagField)
ToolSvc += InDetLinFactory
print InDetLinFactory

from TrkVertexBilloirTools.TrkVertexBilloirToolsConf import Trk__FastVertexFitter
InDetFastVxFitterTool = Trk__FastVertexFitter(name                   = "InDetFastVertexFitterTool",
                                              LinearizedTrackFactory = InDetLinFactory,
                                              Extrapolator           = InDetExtrapolator)
ToolSvc += InDetFastVxFitterTool
print      InDetFastVxFitterTool

from TrkVertexBilloirTools.TrkVertexBilloirToolsConf import Trk__FullVertexFitter
InDetFullVxFitterTool = Trk__FullVertexFitter(name                   = "InDetFullVertexFitterTool",
                                              LinearizedTrackFactory = InDetLinFactory,
                                              Extrapolator           = InDetExtrapolator)
ToolSvc += InDetFullVxFitterTool
print      InDetFullVxFitterTool


