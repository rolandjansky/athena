
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
tool1 = InDetPhysValMonitoringTool()
#Are we using a track selector...?
tool1.useTrackSelection = False
tool1.EnableLumi = False
ToolSvc += tool1
monMan.AthenaMonTools += [tool1]
'''
#if the Track selector is used , selector must be initialised somewhere
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
InDetTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name                                = "InDetDetailedTrackSelectorTool",
                                                               pTMin                               = InDetPrimaryVertexingCuts.minPT(),
                                                               IPd0Max                             = InDetPrimaryVertexingCuts.IPd0Max(),
                                                               IPz0Max                             = InDetPrimaryVertexingCuts.IPz0Max(),
                                                               z0Max                               = InDetPrimaryVertexingCuts.z0Max(),
                                                               sigIPd0Max                          = InDetPrimaryVertexingCuts.sigIPd0Max(),
                                                               sigIPz0Max                          = InDetPrimaryVertexingCuts.sigIPz0Max(),
                                                               d0significanceMax                   = InDetPrimaryVertexingCuts.d0significanceMax(),
                                                               z0significanceMax                   = InDetPrimaryVertexingCuts.z0significanceMax(),
                                                               etaMax                              = InDetPrimaryVertexingCuts.etaMax(),
                                                               useTrackSummaryInfo                 = InDetPrimaryVertexingCuts.useTrackSummaryInfo(),
                                                               nHitBLayer                          = InDetPrimaryVertexingCuts.nHitBLayer(),
                                                               nHitPix                             = InDetPrimaryVertexingCuts.nHitPix(),
                                                               nHolesPixel                         = InDetPrimaryVertexingCuts.nHolesPix(),
                                                               nHitBLayerPlusPix                   = InDetPrimaryVertexingCuts.nHitBLayerPlusPix(),
                                                               nHitSct                             = InDetPrimaryVertexingCuts.nHitSct(),
                                                               nHitSi                              = InDetPrimaryVertexingCuts.nHitSi(),
                                                               nHitTrt                             = InDetPrimaryVertexingCuts.nHitTrt(),
                                                               nHitTrtHighEFractionMax             = InDetPrimaryVertexingCuts.nHitTrtHighEFractionMax(),
                                                               nHitTrtHighEFractionWithOutliersMax = InDetPrimaryVertexingCuts.nHitTrtHighEFractionWithOutliersMax(),
                                                               useSharedHitInfo                    = InDetPrimaryVertexingCuts.useSharedHitInfo(),
                                                               useTrackQualityInfo                 = InDetPrimaryVertexingCuts.useTrackQualityInfo(),
                                                               fitChi2OnNdfMax                     = InDetPrimaryVertexingCuts.fitChi2OnNdfMax(),
                                                               TrtMaxEtaAcceptance                 = InDetPrimaryVertexingCuts.TrtMaxEtaAcceptance(),
                                                               # InDetTestBLayerTool                 = InDetRecTestBLayerTool,
                                                               TrackSummaryTool                    = InDetTrackSummaryTool,
                                                               Extrapolator                        = InDetExtrapolator)

        
ToolSvc += InDetTrackSelectorTool
'''
