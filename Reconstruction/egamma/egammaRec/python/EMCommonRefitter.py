# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

print "EMCommonRefitter.py"
from InDetRecExample.InDetJobProperties import InDetFlags
from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags


#Deal with the Rot creator

if  hasattr(ToolSvc, 'InDetRotCreator') :
    egRotCreator=ToolSvc.InDetRotCreator
else :
    #Setup special RotCreator if one is not present
  
    if InDetFlags.doPixelClusterSplitting():
        #
        # --- Neutral Network version ?
        #
        if InDetFlags.pixelClusterSplittingType() == 'NeuralNet':
        
            # --- temp: read calib file 
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            if not hasattr(svcMgr, 'THistSvc'):
                from GaudiSvc.GaudiSvcConf import THistSvc
                svcMgr += THistSvc()
                import sys,os    

            # --- neutral network tools
            from TrkNeuralNetworkUtils.TrkNeuralNetworkUtilsConf import Trk__NeuralNetworkToHistoTool
            egNeuralNetworkToHistoTool=Trk__NeuralNetworkToHistoTool(name = "egNeuralNetworkToHistoTool")
            print egNeuralNetworkToHistoTool
            ToolSvc += egNeuralNetworkToHistoTool
      

            # --- new NN factor   
            # COOL binding
            from IOVDbSvc.CondDB import conddb
            conddb.addFolder("PIXEL_OFL","/PIXEL/PixelClustering/PixelClusNNCalib")
               
            # --- put in a temporary hack here for 19.1.0, to select the necessary settings when running on run 1 data/MC
            # --- since a correction is needed to fix biases when running on new run 2 compatible calibation
            # --- a better solution is needed...
       
            from SiClusterizationTool.SiClusterizationToolConf import InDet__NnClusterizationFactory    
            if not "R2" in globalflags.DetDescrVersion() and not "IBL3D25" in globalflags.DetDescrVersion():
                egNnClusterizationFactory = InDet__NnClusterizationFactory( name                 = "egNnClusterizationFactory",
                                                                            NetworkToHistoTool   = egNeuralNetworkToHistoTool,
                                                                            doRunI = True,
                                                                            useToT = False,
                                                                            useRecenteringNNWithoutTracks = True,
                                                                            useRecenteringNNWithTracks = False,
                                                                            correctLorShiftBarrelWithoutTracks = 0,
                                                                            correctLorShiftBarrelWithTracks = 0.030,
                                                                            LoadNoTrackNetwork   = True,
                                                                            LoadWithTrackNetwork = True)
        
            else:
                egNnClusterizationFactory = InDet__NnClusterizationFactory( name                 = "egNnClusterizationFactory",
                                                                            NetworkToHistoTool   = egNeuralNetworkToHistoTool,
                                                                            LoadNoTrackNetwork   = True,
                                                                            LoadWithTrackNetwork = True)               
            ToolSvc += egNnClusterizationFactory
      

        
    # ----------- control loading of ROT_creator
    #

    #
    # --- configure default ROT creator
    #
    if DetFlags.haveRIO.pixel_on():
        #
        # load Pixel ROT creator, we overwrite the defaults for the
        # tool to always make conservative pixel cluster errors
        from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__PixelClusterOnTrackTool
        egPixelClusterOnTrackTool = InDet__PixelClusterOnTrackTool("egPixelClusterOnTrackTool",
                                                                   DisableDistortions = InDetFlags.doFatras(),
                                                                   applyNNcorrection = ( InDetFlags.doPixelClusterSplitting() 
                                                                                             and InDetFlags.pixelClusterSplittingType() == 'NeuralNet'))
            
        if InDetFlags.doPixelClusterSplitting() and InDetFlags.pixelClusterSplittingType() == 'NeuralNet':
            egPixelClusterOnTrackTool.NnClusterizationFactory  = egNnClusterizationFactory
            ToolSvc += egPixelClusterOnTrackTool
    else:
        egPixelClusterOnTrackTool = None

            
    if DetFlags.haveRIO.SCT_on():
        from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
        egSCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool ("egSCT_ClusterOnTrackTool",
                                                                  CorrectionStrategy = 0,  # do correct position bias
                                                                  ErrorStrategy      = 2)  # do use phi dependent errors
        ToolSvc += egSCT_ClusterOnTrackTool
    else:
        egSCT_ClusterOnTrackTool = None

    #
    # default ROT creator, not smart !
    #
    from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
    egRotCreator = Trk__RIO_OnTrackCreator(name             = 'egRotCreator',
                                           ToolPixelCluster = egPixelClusterOnTrackTool,
                                           ToolSCT_Cluster  = egSCT_ClusterOnTrackTool,
                                           Mode             = 'indet')
    ToolSvc += egRotCreator

    #
    # load error scaling
    #
    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested('Indet/TrkErrorScaling'):
        conddb.addFolderSplitOnline('INDET','/Indet/Onl/TrkErrorScaling','/Indet/TrkErrorScaling') 


# The main part , set up 
###############################################################################
#######          Trk     Extrapolator Related Packages                 ########
###############################################################################
###############################################################################   
#
# declare the extrapolator
# set up geometry

if not hasattr(ToolSvc,'AtlasExtrapolator'):
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    ToolSvc += AtlasExtrapolator()

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
AtlasTrackingGeometrySvc = svcMgr.AtlasTrackingGeometrySvc
#
# get propagator
#
from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
#
egTrkPropagator = Propagator(name = 'egTrkPropagator')
egTrkPropagator.AccuracyParameter = 0.0001
ToolSvc += egTrkPropagator
#print      egTrkPropagator

from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as StepPropagator
egTrkStepPropagator = StepPropagator(name = 'egTrkStepPropagator')
ToolSvc += egTrkStepPropagator
#
# Setup the Navigator (default, could be removed)
#
from TrkExTools.TrkExToolsConf import Trk__Navigator
egTrkNavigator = Trk__Navigator(name                 = 'egTrkNavigator',
                                #TrackingGeometrySvc = AtlasTrackingGeometrySvc
                                )
ToolSvc += egTrkNavigator
#print      egTrkNavigator
#
# Setup the MaterialEffectsUpdator
#
from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
egTrkMaterialUpdator = Trk__MaterialEffectsUpdator(name = "egTrkMaterialEffectsUpdator")
ToolSvc += egTrkMaterialUpdator
#print      egTrkMaterialUpdator

# CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE

egTrkSubPropagators = []
egTrkSubUpdators = []

# -------------------- set it depending on the geometry ----------------------------------------------------
# default for ID is (Rk,Mat)
egTrkSubPropagators += [ egTrkPropagator.name() ]
egTrkSubUpdators    += [ egTrkMaterialUpdator.name() ]

# default for Calo is (Rk,MatLandau)
egTrkSubPropagators += [ egTrkPropagator.name() ]
egTrkSubUpdators    += [ egTrkMaterialUpdator.name() ]

# default for MS is (STEP,Mat)
egTrkSubPropagators += [ egTrkStepPropagator.name() ]
egTrkSubUpdators    += [ egTrkMaterialUpdator.name() ]
# ----------------------------------------------------------------------------------------------------------            

#
# set up extrapolator
#
from TrkExTools.TrkExToolsConf import Trk__Extrapolator
egTrkExtrapolator = Trk__Extrapolator(name                    = 'egTrkExtrapolator',
                                      Propagators             = [ egTrkPropagator, egTrkStepPropagator ],
                                      MaterialEffectsUpdators = [ egTrkMaterialUpdator ],
                                      Navigator               = egTrkNavigator,
                                      SubPropagators          = egTrkSubPropagators,
                                      SubMEUpdators           = egTrkSubUpdators)
ToolSvc += egTrkExtrapolator

###############################################################################
#######                     GSF Related Packaages                      ########
###############################################################################
###############################################################################

from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMaterialMixtureConvolution
GsfMaterialUpdator = Trk__GsfMaterialMixtureConvolution (name = 'GsfMaterialUpdator')
ToolSvc += GsfMaterialUpdator
#
# component Reduction
#
from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__QuickCloseComponentsMultiStateMerger
GsfComponentReduction = Trk__QuickCloseComponentsMultiStateMerger (name                      = 'GsfComponentReduction',
                                                                   MaximumNumberOfComponents = 12)
ToolSvc += GsfComponentReduction

from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdator as ConfiguredKalmanUpdator
egTrkUpdator = ConfiguredKalmanUpdator('egTrkUpdator')
ToolSvc += egTrkUpdator

from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMeasurementUpdator
GsfMeasurementUpdator = Trk__GsfMeasurementUpdator( name    = 'GsfMeasurementUpdator',
                                                    Updator = egTrkUpdator )
ToolSvc += GsfMeasurementUpdator

from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfExtrapolator
GsfExtrapolator = Trk__GsfExtrapolator(name                          = 'GsfExtrapolator',
                                       Propagators                   = [ egTrkPropagator ],
                                       SearchLevelClosestParameters  = 10,
                                       StickyConfiguration           = True,
                                       Navigator                     = egTrkNavigator,
                                       GsfMaterialConvolution        = GsfMaterialUpdator,
                                       ComponentMerger               = GsfComponentReduction,
                                       SurfaceBasedMaterialEffects   = False )
ToolSvc += GsfExtrapolator

from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GaussianSumFitter
GSFTrackFitter = Trk__GaussianSumFitter(name                    = 'GSFTrackFitter',
                                        ToolForExtrapolation    = GsfExtrapolator,
                                        MeasurementUpdatorType  = GsfMeasurementUpdator,
                                        ReintegrateOutliers     = True,
                                        MakePerigee             = True,
                                        RefitOnMeasurementBase  = True,
                                        DoHitSorting            = True,
                                        ValidationMode          = False,
                                        ToolForROTCreation      = egRotCreator,
                                        OutputLevel =5)
# --- end of fitter loading
ToolSvc += GSFTrackFitter

###############################################################################
#######                     GX2 Related Packaages                      ########
###############################################################################
###############################################################################

from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
GX2TrackFitter = Trk__GlobalChi2Fitter(name                  = 'GX2TrackFitter',
                                         OutputLevel = 4, 
                                         ExtrapolationTool     = egTrkExtrapolator,
                                         NavigatorTool         = egTrkNavigator,
                                         PropagatorTool        = egTrkPropagator,
                                         RotCreatorTool        = egRotCreator,
                                         TrackingGeometrySvc   = AtlasTrackingGeometrySvc,
                                         MaterialUpdateTool    = egTrkMaterialUpdator,
                                         BroadRotCreatorTool   = None,
                                         MeasurementUpdateTool = egTrkUpdator,
                                         StraightLine          = not InDetFlags.solenoidOn(),
                                         OutlierCut            = 4,
                                         SignedDriftRadius     = True,
                                         ReintegrateOutliers   = True,
                                         RecalibrateSilicon    = False,
                                         RecalibrateTRT        = False,
                                         TRTTubeHitCut         = 2.5,
                                         MaxIterations         = 40,
                                         Acceleration          = True,
                                         RecalculateDerivatives= InDetFlags.doCosmics() or InDetFlags.doBeamHalo(),
                                         TRTExtensionCuts      = True,
                                         TrackChi2PerNDFCut    = 10)

ToolSvc += GX2TrackFitter
