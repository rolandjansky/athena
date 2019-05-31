# ------------------------
# SET UP FITTER 
# ------------------------

# User's analysis requirements here:
# ----------------------------------

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
InDetExtrapolator = AtlasExtrapolator(name     = "AtlasExtrapolator")
ToolSvc += InDetExtrapolator
print      InDetExtrapolator

from TrkV0Fitter.TrkV0FitterConf import Trk__TrkV0VertexFitter
TrkV0Fitter = Trk__TrkV0VertexFitter(name              = "TrkV0FitterName",
                                         MaxIterations     = 10,
                                         Use_deltaR        = False,
                                         Extrapolator      = InDetExtrapolator
        )
ToolSvc += TrkV0Fitter
print TrkV0Fitter

from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
TrkVKalVrtFitter = Trk__TrkVKalVrtFitter(  name                = "VKalVrtFitter",
                                           Extrapolator        = InDetExtrapolator,
                                           FirstMeasuredPoint  = False,
                                           MakeExtendedVertex  = True)
ToolSvc += TrkVKalVrtFitter
print TrkVKalVrtFitter




from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator
VtxPointEstimator = InDet__VertexPointEstimator(name             = "VtxPointEstimator",
                                                    MinDeltaR              = [-10000.,-10000.,-10000.],
                                                    MaxDeltaR              = [10000.,10000.,10000.],
                                                    MaxPhi                 = [10000., 10000., 10000.],
                                                    MaxChi2OfVtxEstimation = 2000.)
ToolSvc += VtxPointEstimator
print VtxPointEstimator

from InDetRecExample.InDetKeys import InDetKeys

from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
InDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name  = "CascadeInDetPrdAssociationTool", 
 PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap())
ToolSvc += InDetPrdAssociationTool
print InDetPrdAssociationTool


from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
InDetTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name         = "CascInDetSummaryHelper",
                                      AssoTool     = InDetPrdAssociationTool,
                                     DoSharedHits = False,
                                     HoleSearch   = InDetHoleSearchTool,
                                     usePixel      = DetFlags.haveRIO.pixel_on(),
                                     useSCT        = DetFlags.haveRIO.SCT_on(),
                                     useTRT        = DetFlags.haveRIO.TRT_on())
ToolSvc += InDetTrackSummaryHelperTool
print InDetTrackSummaryHelperTool

from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
InDetTrackSummaryTool = Trk__TrackSummaryTool(name = "CascadeInDetTrackSummaryTool",
                                                  InDetSummaryHelperTool = InDetTrackSummaryHelperTool,
                                                  doSharedHits           = False,
                                                  doHolesInDet           = True)

ToolSvc += InDetTrackSummaryTool
print InDetTrackSummaryTool



from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
InDetTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "cascade_InDetDetailedTrackSelectorTool",
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
                                                                       TrackSummaryTool     = InDetTrackSummaryTool,
                                                                       Extrapolator         = InDetExtrapolator
                                                                      )
        
ToolSvc += InDetTrackSelectorTool
print InDetTrackSelectorTool



from JpsiUpsilonTools.JpsiUpsilonToolsConf import Analysis__JpsiFinder
myJpsiFinder = Analysis__JpsiFinder(name                         = "JpsiFinder",
                                        OutputLevel                 = INFO,
                                        muAndMu                     = True,
                                        muAndTrack                  = False,
                                        TrackAndTrack               = False,
                                        assumeDiMuons               = True, 
                                        invMassUpper                = 3600.0,
                                        invMassLower                = 2600.0,
                                        Chi2Cut                     = 30.,
                                        oppChargesOnly	            = True,
                                        combOnly		    = True,
                                        atLeastOneComb              = False,
                                        useCombinedMeasurement      = False, # Only takes effect if combOnly=True	
                                        muonCollectionKey           = "Muons",
                                        TrackParticleCollection     = "InDetTrackParticles",
                                        V0VertexFitterTool          = TrkV0Fitter,             # V0 vertex fitter
                                        useV0Fitter                 = False,                   # if False a TrkVertexFitterTool will be used
                                        TrkVertexFitterTool         = TrkVKalVrtFitter,        # VKalVrt vertex fitter
                                        TrackSelectorTool           = InDetTrackSelectorTool,
                                        VertexPointEstimator        = VtxPointEstimator,
                                        useMCPCuts                  = False)
ToolSvc += myJpsiFinder
print      myJpsiFinder



from VKalVrtValidation.VKalVrtValidationConf import TestCascadeAlg
Vertex = TestCascadeAlg ( name = "Vertex", 
                        TrkVertexFitterTool = TrkVKalVrtFitter,
                        JpsiFinder          = myJpsiFinder,
                        OutputLevel = INFO,
                        PerEventLimit = 100
                      )

#-------------------------------------------------------------
# User analysis steering algorithm
#-------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()
job += AthSequencer("ModSequence1")
job.ModSequence1 += Vertex

