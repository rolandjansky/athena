# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr as svcMgr, ToolSvc

from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__PixelClusterOnTrackTool
egTrigPixelClusterOnTrackTool = InDet__PixelClusterOnTrackTool("egTrigPixelClusterOnTrackTool",
                                                               DisableDistortions = False,
                                                               applyNNcorrection = False)
ToolSvc += egTrigPixelClusterOnTrackTool

from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
egTrigSCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool ("egTrigSCT_ClusterOnTrackTool",
                                                          CorrectionStrategy = 0,  # do correct position bias
                                                          ErrorStrategy      = 2)  # do use phi dependent errors
ToolSvc += egTrigSCT_ClusterOnTrackTool



from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
egTrigRotCreator = Trk__RIO_OnTrackCreator(name             = 'egTrigRotCreator',
                                       ToolPixelCluster = egTrigPixelClusterOnTrackTool,
                                       ToolSCT_Cluster  = egTrigSCT_ClusterOnTrackTool,
                                       Mode             = 'indet')
ToolSvc += egTrigRotCreator


from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
egTrigTrkPropagator = Propagator(name = 'egTrigTrkPropagator')
egTrigTrkPropagator.AccuracyParameter = 0.0001
ToolSvc += egTrigTrkPropagator

from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as StepPropagator
egTrigTrkStepPropagator = StepPropagator(name = 'egTrigTrkStepPropagator')
ToolSvc += egTrigTrkStepPropagator

from TrkExTools.TrkExToolsConf import Trk__Navigator
egTrigTrkNavigator = Trk__Navigator(name = 'egTrigTrkNavigator')
ToolSvc += egTrigTrkNavigator

from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
egTrigTrkMaterialUpdator = Trk__MaterialEffectsUpdator(name = "egTrigTrkMaterialEffectsUpdator")
ToolSvc += egTrigTrkMaterialUpdator

egTrigTrkSubPropagators = []
egTrigTrkSubUpdators = []
# -------------------- set it depending on the geometry ----------------------------------------------------
# default for ID is (Rk,Mat)
egTrigTrkSubPropagators += [ egTrigTrkPropagator.name() ]
egTrigTrkSubUpdators    += [ egTrigTrkMaterialUpdator.name() ]
# default for Calo is (Rk,MatLandau)
egTrigTrkSubPropagators += [ egTrigTrkPropagator.name() ]
egTrigTrkSubUpdators    += [ egTrigTrkMaterialUpdator.name() ]
# default for MS is (STEP,Mat)
egTrigTrkSubPropagators += [ egTrigTrkStepPropagator.name() ]
egTrigTrkSubUpdators    += [ egTrigTrkMaterialUpdator.name() ]
# ----------------------------------------------------------------------------------------------------------            

from TrkExTools.TrkExToolsConf import Trk__Extrapolator
egTrigTrkExtrapolator = Trk__Extrapolator(name                    = 'egTrigTrkExtrapolator',
                                      Propagators             = [ egTrigTrkPropagator, egTrigTrkStepPropagator ],
                                      MaterialEffectsUpdators = [ egTrigTrkMaterialUpdator ],
                                      Navigator               = egTrigTrkNavigator,
                                      SubPropagators          = egTrigTrkSubPropagators,
                                      SubMEUpdators           = egTrigTrkSubUpdators)
ToolSvc += egTrigTrkExtrapolator


# Set up the GSF
from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMaterialMixtureConvolution
GSFTrigMaterialUpdator = Trk__GsfMaterialMixtureConvolution (name = 'GSFTrigMaterialUpdator')
ToolSvc += GSFTrigMaterialUpdator
# component Reduction
from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__QuickCloseComponentsMultiStateMerger
GSFTrigComponentReduction = Trk__QuickCloseComponentsMultiStateMerger (name                      = 'GSFTrigComponentReduction',
                                                                       MaximumNumberOfComponents = 12)
ToolSvc += GSFTrigComponentReduction

from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdator as ConfiguredKalmanUpdator
egTrigTrkUpdator = ConfiguredKalmanUpdator('egTrigTrkUpdator')
ToolSvc += egTrigTrkUpdator

from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMeasurementUpdator
GSFTrigMeasurementUpdator = Trk__GsfMeasurementUpdator( name    = 'GSFTrigMeasurementUpdator',
                                                    Updator = egTrigTrkUpdator )
ToolSvc += GSFTrigMeasurementUpdator

from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfExtrapolator
GSFTrigExtrapolator = Trk__GsfExtrapolator(name                          = 'GSFTrigExtrapolator',
                                       Propagators                   = [ egTrigTrkPropagator ],
                                       SearchLevelClosestParameters  = 10,
                                       StickyConfiguration           = True,
                                       Navigator                     = egTrigTrkNavigator,
                                       GsfMaterialConvolution        = GSFTrigMaterialUpdator,
                                       ComponentMerger               = GSFTrigComponentReduction,
                                       SurfaceBasedMaterialEffects   = False )
ToolSvc += GSFTrigExtrapolator


from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GaussianSumFitter
GSFTrigTrackFitter = Trk__GaussianSumFitter(name                    = 'GSFTrigTrackFitter',
                                        ToolForExtrapolation    = GSFTrigExtrapolator,
                                        MeasurementUpdatorType  = GSFTrigMeasurementUpdator,
                                        ReintegrateOutliers     = True,
                                        MakePerigee             = True,
                                        RefitOnMeasurementBase  = True,
                                        DoHitSorting            = True,
                                        ValidationMode          = False,
                                        ToolForROTCreation      = egTrigRotCreator)
# --- end of fitter loading
ToolSvc += GSFTrigTrackFitter
