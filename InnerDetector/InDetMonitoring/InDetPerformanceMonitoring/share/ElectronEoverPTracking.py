####  Load Tracking Tools
from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'geant4'
globalflags.InputFormat = 'pool'

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc,theApp,ServiceMgr
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
from AthenaCommon.BeamFlags import jobproperties

topSequence = AlgSequence()


###############################################################################
###############################################################################
#######                  Tracking Realted Packaages                    ########
###############################################################################
###############################################################################
#
# load SCT ROT creator, we overwrite the defaults for it
#
from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
SCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool ("SCT_ClusterOnTrackTool",
                                                          CorrectionStrategy = 0,  # do correct position bias
                                                          ErrorStrategy      = 2)  # do use phi dependent errors
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += SCT_ClusterOnTrackTool
#
# default ROT creator, not smart !
#
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
ElectronRotCreator = Trk__RIO_OnTrackCreator(name            = 'ElectronRotCreator',
                                       ToolSCT_Cluster = SCT_ClusterOnTrackTool,
                                       Mode            = 'indet')
ToolSvc += ElectronRotCreator
#
# load error scaling
#
from IOVDbSvc.CondDB import conddb
if not conddb.folderRequested('Indet/TrkErrorScaling'):
    conddb.addFolder("INDET","/Indet/TrkErrorScaling")

###############################################################################
###############################################################################
#######          Trk     Extraplotor Related Packaages                 ########
###############################################################################
###############################################################################
 #
# declare the extrapolator
#

import MagFieldServices.SetupField
#from TrkMagFieldTools.TrkMagFieldToolsConf import Trk__MagneticFieldTool
#ElectronTrkMagField = Trk__MagneticFieldTool('ElectronTrkMagField')
#ToolSvc += ElectronTrkMagField
#
# set up geometry
#


#include('/afs/cern.ch/user/s/sthenkel/work/ProjectArea/Tracking/TrkDetDescr/TrkDetDescrSvc/python/AtlasTrackingGeometrySvc.py')
#from __main__ import AtlasTrackingGeometrySvc
#
# get propagator
#
from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
#
ElectronTrkPropagator = Propagator(name = 'ElectronTrkPropagator')
ElectronTrkPropagator.AccuracyParameter = 0.0001
ToolSvc += ElectronTrkPropagator

from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as StepPropagator
ElectronTrkStepPropagator = StepPropagator(name = 'ElectronTrkStepPropagator')
ToolSvc += ElectronTrkStepPropagator
#
# Setup the Navigator (default, could be removed)
#
from TrkExTools.TrkExToolsConf import Trk__Navigator
ElectronTrkNavigator = Trk__Navigator(name = 'ElectronTrkNavigator',
                                      #TrackingGeometrySvc = AtlasTrackingGeometrySvc
                                )
ToolSvc += ElectronTrkNavigator
#
# Setup the MaterialEffectsUpdator
#
from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
ElectronTrkMaterialUpdator = Trk__MaterialEffectsUpdator(name = "ElectronTrkMaterialEffectsUpdator")
ToolSvc += ElectronTrkMaterialUpdator

# CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE

ElectronTrkSubPropagators = []
ElectronTrkSubUpdators = []

# -------------------- set it depending on the geometry ----------------------------------------------------
# default for ID is (Rk,Mat)
ElectronTrkSubPropagators += [ ElectronTrkPropagator.name() ]
ElectronTrkSubUpdators    += [ ElectronTrkMaterialUpdator.name() ]

# default for Calo is (Rk,MatLandau)
ElectronTrkSubPropagators += [ ElectronTrkPropagator.name() ]
ElectronTrkSubUpdators    += [ ElectronTrkMaterialUpdator.name() ]

# default for MS is (STEP,Mat)
ElectronTrkSubPropagators += [ ElectronTrkStepPropagator.name() ]
ElectronTrkSubUpdators    += [ ElectronTrkMaterialUpdator.name() ]
# ----------------------------------------------------------------------------------------------------------

#
# set up extrapolator
#
from TrkExTools.TrkExToolsConf import Trk__Extrapolator
ElectronTrkExtrapolator = Trk__Extrapolator(name                    = 'ElectronTrkExtrapolator',
                                      Propagators             = [ ElectronTrkPropagator, ElectronTrkStepPropagator ],
                                      MaterialEffectsUpdators = [ ElectronTrkMaterialUpdator ],
                                      Navigator               = ElectronTrkNavigator,
                                      SubPropagators          = ElectronTrkSubPropagators,
                                      SubMEUpdators           = ElectronTrkSubUpdators,
                                      )
ToolSvc += ElectronTrkExtrapolator



###############################################################################
###############################################################################
#######                     GSF Realted Packaages                      ########
###############################################################################
###############################################################################

from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMaterialMixtureConvolution
GsfMaterialUpdator = Trk__GsfMaterialMixtureConvolution (name = 'GsfMaterialUpdator')
ToolSvc += GsfMaterialUpdator
print      GsfMaterialUpdator
#
# component Reduction
#
from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__QuickCloseComponentsMultiStateMerger
GsfComponentReduction = Trk__QuickCloseComponentsMultiStateMerger (name = 'GsfComponentReduction',
                                                              MaximumNumberOfComponents = 12)
ToolSvc += GsfComponentReduction
print      GsfComponentReduction


from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdator as ConfiguredKalmanUpdator
ElectronUpdator = ConfiguredKalmanUpdator('ElectronUpdator')
ToolSvc += ElectronUpdator

from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMeasurementUpdator
GsfMeasurementUpdator = Trk__GsfMeasurementUpdator( name    = 'GsfMeasurementUpdator',
                                                  Updator = ElectronUpdator )
ToolSvc += GsfMeasurementUpdator



from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfExtrapolator
GsfExtrapolator = Trk__GsfExtrapolator(name                          = 'GsfExtrapolator',
                                       Propagators                   = [ ElectronTrkPropagator ],
                                       SearchLevelClosestParameters  = 10,
                                       StickyConfiguration           = True,
                                       Navigator                     = ElectronTrkNavigator,
                                       GsfMaterialConvolution        = GsfMaterialUpdator,
                                       ComponentMerger               = GsfComponentReduction,
                                       SurfaceBasedMaterialEffects   = False )
ToolSvc += GsfExtrapolator


from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__BremFind
BremFind = Trk__BremFind(name = 'BremFind',
                         UseCalibration = True,
                         ValidationMode = True )


ToolSvc += BremFind

from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GaussianSumFitter
GSFTrackFitter = Trk__GaussianSumFitter(name                    = 'GSFTrackFitter',
                                          ToolForExtrapolation    = GsfExtrapolator,
                                          MeasurementUpdatorType  = GsfMeasurementUpdator,
                                          ToolForROTCreation      = ElectronRotCreator,
                                          ReintegrateOutliers     = True,
                                          MakePerigee             = True,
                                          RefitOnMeasurementBase  = True,
                                          DoHitSorting            = True,
                                          ValidationMode          = False,
                                          BremFind                = BremFind,
                                          runBremFinder           = False,
                                          OutputLevel = 3)
# --- end of fitter loading
ToolSvc += GSFTrackFitter



###############################################################################
###############################################################################
#######                     DNA Realted Packaages                      ########
###############################################################################
###############################################################################

from InDetCompetingRIOsOnTrackTool.InDetCompetingRIOsOnTrackToolConf import InDet__CompetingPixelClustersOnTrackTool as IDCPCOTT
ElectronCompetingPixelTool = IDCPCOTT(name='ElectronKalmanCompetingPixelClustersTool',
                                WeightCutValueBarrel = 5.5,
                                WeightCutValueEndCap = 5.5)
ToolSvc+=ElectronCompetingPixelTool

from InDetCompetingRIOsOnTrackTool.InDetCompetingRIOsOnTrackToolConf import InDet__CompetingSCT_ClustersOnTrackTool as IDCSCOTT
ElectronCompetingSctTool = IDCSCOTT(name='ElectronKalmanCompetingSCT_ClustersTool',
                              WeightCutValueBarrel = 5.5,
                              WeightCutValueEndCap = 5.5)
ToolSvc+=ElectronCompetingSctTool

from TrkCompetingRIOsOnTrackTool.TrkCompetingRIOsOnTrackToolConf import Trk__CompetingRIOsOnTrackTool as CompRotTool
ElectronKalmanCompetingROT_Tool = CompRotTool(name='ElectronKalmanCompetingRIOsTool',
                                        ToolForCompPixelClusters = ElectronCompetingPixelTool,
                                        ToolForCompSCT_Clusters = ElectronCompetingSctTool)
ToolSvc += ElectronKalmanCompetingROT_Tool

from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanPiecewiseAnnealingFilter as KPAF
ElectronKalmanInternalDAF = KPAF(name = 'ElectronKalmanInternalDAF',
                           CompetingRIOsOnTrackCreator = ElectronKalmanCompetingROT_Tool)
ToolSvc += ElectronKalmanInternalDAF

from TrkKalmanFitter.TrkKalmanFitterConf import Trk__MeasRecalibSteeringTool
ElectronMeasRecalibST = Trk__MeasRecalibSteeringTool(name='ElectronMeasRecalibST')
ToolSvc += ElectronMeasRecalibST

from TrkDynamicNoiseAdjustor.TrkDynamicNoiseAdjustorConf import Trk__InDetDynamicNoiseAdjustment
ElectronDNAdjustor = Trk__InDetDynamicNoiseAdjustment(name       = 'ElectronDNAdjustor')
                                                #signifmin  = 0,
                                                #lambdaxmin = 0)
ToolSvc += ElectronDNAdjustor

# Load Kalman Filter tools
from TrkKalmanFitter.TrkKalmanFitterConf import Trk__ForwardKalmanFitter as PublicFKF
ElectronFKF = PublicFKF(name                  = 'ElectronFKF',
                  StateChi2PerNDFPreCut = 30.0)
ToolSvc += ElectronFKF

from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanSmoother as PublicBKS
ElectronBKS = PublicBKS(name                        = 'ElectronBKS',
                  InitialCovarianceSeedFactor = 200.)
ToolSvc += ElectronBKS

from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanOutlierLogic as PublicKOL
ElectronKOL = PublicKOL(name               = 'ElectronKOL',
                  TrackChi2PerNDFCut = 17.0,
                  StateChi2PerNDFCut = 12.5,
                  #BroadPixelClusterHandle = BroadPixelClusterOnTrackTool
                  )
ToolSvc += ElectronKOL

from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanFitter as ConfiguredKalmanFitter
DNATrackFitter = ConfiguredKalmanFitter(name                             = 'DNATrackFitter',
                                        ExtrapolatorHandle             = ElectronTrkExtrapolator,
                                        MeasurementUpdatorHandle       = ElectronUpdator,
                                        ForwardKalmanFitterHandle      = ElectronFKF,
                                        KalmanSmootherHandle           = ElectronBKS,
                                        KalmanOutlierLogicHandle       = ElectronKOL,
                                        DynamicNoiseAdjustorHandle     = ElectronDNAdjustor,
                                        BrempointAnalyserHandle        = None,
                                        AlignableSurfaceProviderHandle = None,
                                        RIO_OnTrackCreatorHandle       = ElectronRotCreator,
                                        #RecalibratorHandle             = ElectronMeasRecalibST,
                                        RecalibratorHandle             = None,
                                        InternalDAFHandle              = None#ElectronKalmanInternalDAF
                                        )

ToolSvc += DNATrackFitter



###############################################################################
###############################################################################
#######                     GX2 Realted Packaages                      ########
###############################################################################
###############################################################################

from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
GX2TrackFitter = Trk__GlobalChi2Fitter(name                  = 'GX2TrackFitter',
                                         OutputLevel = 4,
                                       #TrackingGeometrySvc     = AtlasTrackingGeometrySvc,
					 ExtrapolationTool     = ElectronTrkExtrapolator,
                                         NavigatorTool         = ElectronTrkNavigator,
                                         PropagatorTool        = ElectronTrkPropagator,
                                         RotCreatorTool        = ElectronRotCreator,
                                         BroadRotCreatorTool   = None,
                                         MeasurementUpdateTool = ElectronUpdator,
                                         StraightLine          = not InDetFlags.solenoidOn(),
                                         OutlierCut            = 4,
                                         SignedDriftRadius     = True,
                                         ReintegrateOutliers   = False,
                                         RecalibrateSilicon    = False,
                                         RecalibrateTRT        = False,
                                         TRTTubeHitCut         = 2.5,
                                         MaxIterations         = 40,
                                         Acceleration          = True,
                                         RecalculateDerivatives= InDetFlags.doCosmics() or InDetFlags.doBeamHalo(),
                                         TRTExtensionCuts      = True,
                                         TrackChi2PerNDFCut    = 10)

ToolSvc += GX2TrackFitter

###############################################################################
###############################################################################
#######                    Refitting the Electrons                     ########
###############################################################################
###############################################################################


job = AlgSequence()


#######                 Loading egammaTrkRefitterTool                  ########


from egammaTrackTools.egammaTrackToolsConf import egammaTrkRefitterTool
ElectronRefitterTool = egammaTrkRefitterTool(name = 'ElectronRefitterTool',
#                                            FitterTool = GSFTrackFitter,
                                            FitterTool = GX2TrackFitter,
                                            matEffects = 3,
                                            minNoSiHits = -1,
                                            useBeamSpot = False,
                                            OutputLevel =4)
ToolSvc += ElectronRefitterTool


ElectronRefitterTool2 = egammaTrkRefitterTool(name = 'ElectronRefitterTool2',
                                            FitterTool = GX2TrackFitter,
                                            matEffects = 3,
                                            minNoSiHits = -1,
                                            RemoveTRTHits = True,
                                            useBeamSpot = False,
                                            OutputLevel =4)
ToolSvc += ElectronRefitterTool2

MuonRefitterTool = egammaTrkRefitterTool(name = 'MuonRefitterTool',
                                            FitterTool = GX2TrackFitter,
                                            matEffects = 2,
                                            minNoSiHits = -1,
         				    useBeamSpot = False,
                                            OutputLevel =4)
ToolSvc += MuonRefitterTool


MuonRefitterTool2 = egammaTrkRefitterTool(name = 'MuonRefitterTool2',
                                            FitterTool = GX2TrackFitter,
                                            matEffects = 2,
                                            minNoSiHits = -1,
					    RemoveTRTHits = True,
					    useBeamSpot = False,
                                            OutputLevel =4)
ToolSvc += MuonRefitterTool2

### (SH) Commented out for now since not offically supported in
#MuonRefitterTool3 = egammaTrkRefitterTool(name = 'MuonRefitterTool3',
#                                          FitterTool = GX2TrackFitter,
#                                          matEffects = 2,
#                                          minNoSiHits = -1,
#                                          RemoveTRTHits = True,
#                                          RemoveIBLHits = True,
#                                          useBeamSpot = False,
#                                          OutputLevel =4)
#ToolSvc += MuonRefitterTool3


GSFTrackCollection = "RefittedElectronTracks"
DNATrackCollection = "RefittedElectronTracks2"
