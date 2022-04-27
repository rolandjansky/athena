#  Load Tracking Tools
from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'geant4'
globalflags.InputFormat = 'pool'

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc,theApp, ServiceMgr
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
from AthenaCommon.BeamFlags import jobproperties
from InDetRecExample import TrackingCommon

topSequence = AlgSequence()


###############################################################################
###############################################################################
#######                  Tracking Realted Packaages                    ########
###############################################################################
###############################################################################
#
# SiLorentzAngleTool
#
from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()
#
# load SCT ROT creator, we overwrite the defaults for it
#
from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
SCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool ("SCT_ClusterOnTrackTool",
                                                        CorrectionStrategy = 0,  # do correct position bias
                                                        ErrorStrategy      = 2,  # do use phi dependent errors
                                                        LorentzAngleTool   = sctLorentzAngleToolSetup.SCTLorentzAngleTool)
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
from TrkMagFieldTools.TrkMagFieldToolsConf import Trk__MagneticFieldTool
ElectronTrkMagField = Trk__MagneticFieldTool('ElectronTrkMagField')
ToolSvc += ElectronTrkMagField
#
# set up geometry
#
from InDetRecExample.TrackingCommon import use_tracking_geometry_cond_alg
geom_svc=None
geom_cond_key=''
if not use_tracking_geometry_cond_alg :
	from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
	acc = TrackingGeometrySvcCfg(flags)
	geom_svc = acc.getPrimary()
else :
  cond_alg = TrackingCommon.createAndAddCondAlg(TrackingCommon.getTrackingGeometryCondAlg, "AtlasTrackingGeometryCondAlg", name="AtlasTrackingGeometryCondAlg")
  geom_cond_key = cond_alg.TrackingGeometryWriteKey
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
			TrackingGeometrySvc = geom_svc,
			TrackingGeometryKey = geom_cond_key)
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
                                      DoCaloDynamic           = False)
ToolSvc += ElectronTrkExtrapolator

 

###############################################################################
###############################################################################
#######                     GSF config                                 ########
###############################################################################
###############################################################################
import egammaRec.EMCommonRefitter
GSFTrackFitter = egammaRec.EMCommonRefitter.getGSFTrackFitter(name = 'GSFTrackFitter')
# --- end of fitter loading
ToolSvc += GSFTrackFitter

###############################################################################
###############################################################################
#######                     GX2 Realted Packaages                      ########
###############################################################################
###############################################################################

from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
GX2TrackFitter = Trk__GlobalChi2Fitter(name                  = 'GX2TrackFitter',
                                         OutputLevel = 4, 
                                         ExtrapolationTool     = ElectronTrkExtrapolator,
                                         NavigatorTool         = ElectronTrkNavigator,
                                         PropagatorTool        = ElectronTrkPropagator,
                                         RotCreatorTool        = ElectronRotCreator,
                                         BroadRotCreatorTool   = None,
                                         MeasurementUpdateTool = ElectronUpdator,
                                         MagneticFieldTool     = ElectronTrkMagField,
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
## ElectronRefitterTool = egammaTrkRefitterTool(name = 'ElectronRefitterTool',
## #                                            FitterTool = GSFTrackFitter,
##                                             FitterTool = GX2TrackFitter,
## #                                            RemoveTRTHits = True,
##                                             matEffects = 1,
##                                             minNoSiHits = -1,
##                                             useBeamSpot = False,
##                                             OutputLevel =4)
## ToolSvc += ElectronRefitterTool
ElectronRefitterTool = egammaTrkRefitterTool(name = 'ElectronRefitterTool',
                                             FitterTool = GX2TrackFitter,
                                             matEffects = 3,
                                            RemoveTRTHits = True,
                                             minNoSiHits = -1,
                                              useBeamSpot = False,
                                              OutputLevel =4)
ToolSvc += ElectronRefitterTool

ElectronRefitterTool2 = egammaTrkRefitterTool(name = 'ElectronRefitterTool2',
                                             FitterTool = GX2TrackFitter,
                                             matEffects = 3,
 #                                            RemoveTRTHits = True,
                                             minNoSiHits = -1,
                                              useBeamSpot = False,
                                              OutputLevel =4)
ToolSvc += ElectronRefitterTool2


GSFTrackCollection = "GSFTracks"
DNATrackCollection = "DNATracks"

