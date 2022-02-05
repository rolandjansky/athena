# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file TrigTauAlgorithmsHolder.py
#
#@brief All tau algorithms needed for tau reconstruction are configured here.
#
################################################################################

from AthenaCommon.SystemOfUnits import mm, GeV

cached_instances = {}

sPrefix = 'TrigTau_'
bAODmode = False
doVertexCorrection = False

######################################################################## 
def setPrefix(prefix): 
    global sPrefix 
    sPrefix = prefix 
    
######################################################################## 
def setAODmode(mode): 
    global bAODmode 
    bAODmode = mode

########################################################################
# Atlas Extrapolator
def getAtlasExtrapolator():
    _name = sPrefix + 'theAtlasExtrapolator'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    #Configure the extrapolator
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    theAtlasExtrapolator=AtlasExtrapolator(name = _name)
    theAtlasExtrapolator.DoCaloDynamic = False # this turns off dynamic
    
    ToolSvc += theAtlasExtrapolator
    cached_instances[_name] = theAtlasExtrapolator
    return theAtlasExtrapolator


# JetSeedBuilder
def getJetSeedBuilder():
    _name = sPrefix + 'JetSeedBuilder'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import JetSeedBuilder
    JetSeedBuilder = JetSeedBuilder(name = _name)
            
    cached_instances[_name] = JetSeedBuilder
    return JetSeedBuilder

########################################################################
# Tau energy calibration and tau axis direction
def getTauAxis():
    _name = sPrefix + 'TauAxis'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauAxisSetter
    TauAxisSetter = TauAxisSetter(  name = _name, 
                                    ClusterCone = 0.2,
                                    VertexCorrection = doVertexCorrection )
    # No Axis correction at trigger level
                                    
    cached_instances[_name] = TauAxisSetter                
    return TauAxisSetter

########################################################################
# Tau energy calibration
def getEnergyCalibrationLC(caloOnly=False):
 
    _name = sPrefix +'EnergyCalibrationLC'
    
    if _name in cached_instances:
        return cached_instances[_name]

    calibFileName = "TES2016_LC_online.root"
    if caloOnly:
        calibFileName = "TES2016_LC_online_inc.root"
    
    from tauRecTools.tauRecToolsConf import TauCalibrateLC
    TauCalibrateLC = TauCalibrateLC(name = _name,
                                    calibrationFile = calibFileName,
                                    doPtResponse = False,
                                    VertexCorrection = doVertexCorrection)

    TauCalibrateLC.isCaloOnly = caloOnly
    #Need to empty the vertex key collection in the trigger case
    TauCalibrateLC.Key_vertexInputContainer = ""

    cached_instances[_name] = TauCalibrateLC                
    return TauCalibrateLC

########################################################################
# MvaTESVariableDecorator
def getMvaTESVariableDecorator():

    _name = sPrefix + 'MvaTESVariableDecorator'

    if _name in cached_instances:
        return cached_instances[_name]

    from AthenaCommon.AppMgr import ToolSvc
    from tauRecTools.tauRecToolsConf import MvaTESVariableDecorator
    MvaTESVariableDecorator = MvaTESVariableDecorator(name = _name,
                                                      VertexCorrection = doVertexCorrection)

    MvaTESVariableDecorator.Key_vertexInputContainer = ""
    MvaTESVariableDecorator.EventShapeKey = ""

    ToolSvc += MvaTESVariableDecorator
    cached_instances[_name] = MvaTESVariableDecorator
    return MvaTESVariableDecorator

########################################################################
# MvaTESEvaluator
def getMvaTESEvaluator():

    _name = sPrefix + 'MvaTESEvaluator'

    if _name in cached_instances:
        return cached_instances[_name]

    from AthenaCommon.AppMgr import ToolSvc
    from tauRecTools.tauRecToolsConf import MvaTESEvaluator
    MvaTESEvaluator = MvaTESEvaluator(name = _name,
                                      WeightFileName = 'OnlineMvaTES_BRT_v1.weights.root')

    ToolSvc += MvaTESEvaluator
    cached_instances[_name] = MvaTESEvaluator
    return MvaTESEvaluator

########################################################################
# Tau cell variables calculation
def getCellVariables(cellConeSize=0.2):
 
    _name = sPrefix + 'CellVariables'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauCellVariables
    TauCellVariables = TauCellVariables(name = _name,
            StripEthreshold = 0.2*GeV,
            CellCone = cellConeSize,
            VertexCorrection = doVertexCorrection)
            
    cached_instances[_name] = TauCellVariables   
    return TauCellVariables


########################################################################
########################################################################
# Tracking Tools
# TODO: rearrange
########################################################################


########################################################################
# TauFullLinearizedTrackFactory
def getTauFullLinearizedTrackFactory():
    _name = sPrefix + 'TauFullLinearizedTrackFactory'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
    TauFullLinearizedTrackFactory=Trk__FullLinearizedTrackFactory(name = _name, Extrapolator = getAtlasExtrapolator())
    
    ToolSvc += TauFullLinearizedTrackFactory
    cached_instances[_name] = TauFullLinearizedTrackFactory
    return TauFullLinearizedTrackFactory

########################################################################
# TauCrossDistancesSeedFinder
def getTauCrossDistancesSeedFinder():
    _name = 'TauCrossDistancesSeedFinder'

    from AthenaCommon.AppMgr import ToolSvc

    if _name in cached_instances:
        return cached_instances[_name]

    #first the seed finder utils
    from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__SeedNewtonTrkDistanceFinder
    TauNewtonTrkDistanceFinder = Trk__SeedNewtonTrkDistanceFinder( name = sPrefix+'TauSeedNewtonTrkDistanceFinder')
    ToolSvc += TauNewtonTrkDistanceFinder
    
    #then the seed finder tools
    from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
    TauCrossDistancesSeedFinder = Trk__CrossDistancesSeedFinder( name = _name, TrkDistanceFinder=TauNewtonTrkDistanceFinder)

    cached_instances[_name] = TauCrossDistancesSeedFinder
    ToolSvc +=TauCrossDistancesSeedFinder
    return TauCrossDistancesSeedFinder

########################################################################
# TauAdaptiveVertexFitter
def getTauAdaptiveVertexFitter():
    _name = sPrefix + 'TauAdaptiveVertexFitter'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    #then the fitter utils
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator
    TauInDetImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name = sPrefix+'TauTrkImpactPoint3dEstimator', Extrapolator = getAtlasExtrapolator())
    ToolSvc += TauInDetImpactPoint3dEstimator
    
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__DetAnnealingMaker
    TauDetAnnealingMaker = Trk__DetAnnealingMaker(name = sPrefix+'TauDetAnnealingMaker', SetOfTemperatures = [ 64, 32, 16, 8, 4, 2, 1 ] )
    ToolSvc += TauDetAnnealingMaker
    
    #then the fitters (smoother + adaptive with smoothing + fast billoir)
    from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
    TauSequentialVertexSmoother = Trk__SequentialVertexSmoother(name = sPrefix+'TauSequentialVertexSmoother')
    ToolSvc += TauSequentialVertexSmoother
     
    from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveVertexFitter
    TauAdaptiveVertexFitter = Trk__AdaptiveVertexFitter(name = _name, 
                                                        SeedFinder=getTauCrossDistancesSeedFinder(), 
                                                        ImpactPoint3dEstimator=TauInDetImpactPoint3dEstimator, 
                                                        VertexSmoother=TauSequentialVertexSmoother, 
                                                        AnnealingMaker=TauDetAnnealingMaker,
                                                        LinearizedTrackFactory=getTauFullLinearizedTrackFactory())
    
    cached_instances[_name] = TauAdaptiveVertexFitter
    ToolSvc +=TauAdaptiveVertexFitter
    return TauAdaptiveVertexFitter

########################################################################
# TauTrackToVertexIPEstimator
def getTauTrackToVertexIPEstimator():
    _name = sPrefix + 'TauTrackToVertexIPEstimator'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
    TauTrackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(name = _name,
                                                                Extrapolator=getAtlasExtrapolator(),
                                                                LinearizedTrackFactory=getTauFullLinearizedTrackFactory())
    cached_instances[_name] = TauTrackToVertexIPEstimator
    ToolSvc += TauTrackToVertexIPEstimator
    return TauTrackToVertexIPEstimator                                     

#########################################################################
# Tau Variables
# TODO: rename + rearrange
def getTauCommonCalcVars():
    _name = sPrefix + 'TauCommonCalcVars'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauCommonCalcVars
    TauCommonCalcVars = TauCommonCalcVars(name = _name)
    
    cached_instances[_name] = TauCommonCalcVars    
    return TauCommonCalcVars

#########################################################################
# Tau Vertex Variables
def getTauVertexVariables():
    _name = sPrefix + 'TauVertexVariables'
    
    if _name in cached_instances:
        return cached_instances[_name]

    from tauRecTools.tauRecToolsConf import TauVertexVariables
    TauVertexVariables = TauVertexVariables(  name = _name,
                                              VertexFitter = getTauAdaptiveVertexFitter(),
                                              SeedFinder = getTauCrossDistancesSeedFinder(),
                                              )
    
    cached_instances[_name] = TauVertexVariables    
    return TauVertexVariables

#########################################################################
# Tau Variables
# TODO: rename + rerrange
def getTauSubstructure():
    _name = sPrefix + 'TauSubstructure'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauSubstructureVariables
    TauSubstructureVariables = TauSubstructureVariables(  name = _name,
                                                          VertexCorrection = doVertexCorrection)
    
    cached_instances[_name] = TauSubstructureVariables
    return TauSubstructureVariables

#########################################################################
# cell weight tool
def getCellWeightTool():
    _name = sPrefix + 'CellWeightTool'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    #from CaloClusterCorrection.CaloClusterCorrectionConf import H1WeightToolCSC12Generic    
    from CaloRec.CaloTopoClusterFlags import jobproperties
    # -- auto configure weight tool
    finder = jobproperties.CaloTopoClusterFlags.cellWeightRefFinder.get_Value()
    size   = jobproperties.CaloTopoClusterFlags.cellWeightRefSize.get_Value()
    signal = jobproperties.CaloTopoClusterFlags.cellWeightRefSignal.get_Value()
    
    from CaloClusterCorrection.StandardCellWeightCalib import getCellWeightTool
    CaloWeightTool = getCellWeightTool(finder,size,signal)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += CaloWeightTool
    
    cached_instances[_name] = CaloWeightTool
    return CaloWeightTool

#########################################################################
def getInDetTrackSelectorTool():
    _name = sPrefix + 'InDetTrackSelectorTool'  
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    #Configures tau track selector tool (should eventually check whether an existing one is available)
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut
    InDetTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = _name,
                                                                pTMin                = 1000.,
                                                                IPd0Max              = 2.,
                                                                IPz0Max              = 9999., 
                                                                useTrackSummaryInfo  = True,
                                                                nHitBLayer           = 0, 
                                                                nHitBLayerPlusPix    = 0,
                                                                nHitPix              = 2,  # PixelHits + PixelDeadSensors
                                                                nHitSct              = 0,  # SCTHits + SCTDeadSensors
                                                                nHitSi               = 7,  # PixelHits + SCTHits + PixelDeadSensors + SCTDeadSensors
                                                                nHitTrt              = 0,  # nTRTHits
                                                                useSharedHitInfo     = False,
                                                                useTrackQualityInfo  = True,
                                                                fitChi2OnNdfMax      = 99999,
                                                                TrackSummaryTool     = None,
                                                                Extrapolator         = getAtlasExtrapolator(),
                                                                TrtDCCutTool         = InDetTrigTRTDriftCircleCut)
                                                                
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += InDetTrackSelectorTool
    
    cached_instances[_name] = InDetTrackSelectorTool
    return InDetTrackSelectorTool
    
#########################################################################
def getTauVertexFinder(doUseTJVA=False):
    _name = sPrefix + 'TauVertexFinder'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    # Algorithm that overwrites numTrack() and charge() of all tauJets in the container
    from tauRecTools.tauRecToolsConf import TauVertexFinder
    TauVertexFinder = TauVertexFinder(name = _name,
                                      UseTJVA                 = doUseTJVA,
                                      AssociatedTracks="GhostTrack", # OK??
                                      InDetTrackSelectionToolForTJVA = "", 
                                      Key_vertexInputContainer = "",
                                      Key_trackPartInputContainer= "",
                                      OnlineMaxTransverseDistance = 2.5*mm,   # ATR-15665
                                      # OnlineMaxLongitudinalDistance = 2 *mm,
                                      OnlineMaxZ0SinTheta = 3.0 *mm,
                                      TVATool = "", 
                                      )
    
    cached_instances[_name] = TauVertexFinder         
    return TauVertexFinder 

#########################################################################
def getTrackToVertexTool():
    _name = sPrefix + 'TrackToVertexTool'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
    TrackToVertexTool = Reco__TrackToVertex( name = _name,
                                             Extrapolator = getAtlasExtrapolator())
                                             
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += TrackToVertexTool
    
    cached_instances[_name] = TrackToVertexTool
    return TrackToVertexTool

########################################################################
# Tau-Track Association
def getTauTrackFinder(applyZ0cut=False, maxDeltaZ0=2, noSelector = False, prefix=''):

    #if prefix is not given, take global one 
    if not prefix: 
        prefix=sPrefix 
    _name = prefix + 'TauTrackFinder'
    
    if _name in cached_instances:
        return cached_instances[_name] 

 
    from tauRecTools.tauRecToolsConf import TauTrackFinder
    TauTrackFinder = TauTrackFinder(name = _name,
                                    MaxJetDrTau = 0.2,
                                    MaxJetDrWide          = 0.4,
                                    TrackSelectorToolTau  = getInDetTrackSelectorTool(),
                                    Key_trackPartInputContainer = "",
                                    TrackToVertexTool         = getTrackToVertexTool(),
                                    maxDeltaZ0wrtLeadTrk = maxDeltaZ0, #in mm
                                    removeTracksOutsideZ0wrtLeadTrk = applyZ0cut,
                                    ParticleCaloExtensionTool = getParticleCaloExtensionTool(),
                                    BypassSelector = noSelector,
                                    BypassExtrapolator = True,
                                    TrackToVertexIPEstimator = getTauTrackToVertexIPEstimator()
                                    )
    # Selector not needed for fast-tracks
    # Extrapolator never needed

    TauTrackFinder.tauParticleCache = ""

    cached_instances[_name] = TauTrackFinder      
    return TauTrackFinder


# Associate the cluster in jet constituents to the tau candidate
def getTauClusterFinder():
    _name = sPrefix + 'TauClusterFinder'

    if _name in cached_instances:
        return cached_instances[_name]
  
    from tauRecTools.tauRecToolsConf import TauClusterFinder
    TauClusterFinder = TauClusterFinder(name = _name,
                                        UseOriginalCluster = False)

    cached_instances[_name] = TauClusterFinder
    return TauClusterFinder


def getTauVertexedClusterDecorator():
    from tauRecTools.tauRecToolsConf import TauVertexedClusterDecorator

    _name = sPrefix + 'TauVertexedClusterDecorator'
    
    if _name in cached_instances:
        return cached_instances[_name]
  
    myTauVertexedClusterDecorator = TauVertexedClusterDecorator(name = _name,
                                                                SeedJet = "")
    
    cached_instances[_name] = myTauVertexedClusterDecorator
    return myTauVertexedClusterDecorator


########################################################################
# TauIDVarCalculator
def getTauIDVarCalculator():

    _name = sPrefix + 'TauIDVarCalculator'

    if _name in cached_instances:
        return cached_instances[_name]
    
    from AthenaCommon.AppMgr import ToolSvc
    from tauRecTools.tauRecToolsConf import TauIDVarCalculator            
    TauIDVarCalculator = TauIDVarCalculator(name=_name,
                                            VertexCorrection = doVertexCorrection)
    
    ToolSvc += TauIDVarCalculator                                 
    cached_instances[_name] = TauIDVarCalculator
    return TauIDVarCalculator

########################################################################
# TauJetRNNEvaluator
def getTauJetRNNEvaluator(NetworkFile0P="", NetworkFile1P="", NetworkFile3P="", OutputVarname="RNNJetScore", 
                          MaxTracks=10, MaxClusters=6, MaxClusterDR=1.0, TrackClassification=False,
                          InputLayerScalar="scalar", InputLayerTracks="tracks", InputLayerClusters="clusters", 
                          OutputLayer="rnnid_output", OutputNode="sig_prob", suffix=""):

    _name = sPrefix + 'TauJetRNNEvaluator' + suffix

    if _name in cached_instances:
        return cached_instances[_name]

    from AthenaCommon.AppMgr import ToolSvc
    from tauRecTools.tauRecToolsConf import TauJetRNNEvaluator
    TauJetRNNEvaluator = TauJetRNNEvaluator(name = _name,
                                            NetworkFile0P = NetworkFile0P,
                                            NetworkFile1P = NetworkFile1P,
                                            NetworkFile3P = NetworkFile3P,
                                            OutputVarname = OutputVarname,
                                            MaxTracks = MaxTracks,
                                            MaxClusters = MaxClusters,
                                            MaxClusterDR = MaxClusterDR,
                                            VertexCorrection = doVertexCorrection,
                                            TrackClassification = TrackClassification,
                                            InputLayerScalar = InputLayerScalar,
                                            InputLayerTracks = InputLayerTracks,
                                            InputLayerClusters = InputLayerClusters,
                                            OutputLayer = OutputLayer,
                                            OutputNode = OutputNode)

    ToolSvc += TauJetRNNEvaluator
    cached_instances[_name] = TauJetRNNEvaluator
    return TauJetRNNEvaluator

########################################################################
# TauWPDecoratorJetRNN
def getTauWPDecoratorJetRNN():

    _name = sPrefix + 'TauWPDecoratorJetRNN'

    if _name in cached_instances:
        return cached_instances[_name]

    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.load_library('libxAODTau_cDict')

    from AthenaCommon.AppMgr import ToolSvc
    from tauRecTools.tauRecToolsConf import TauWPDecorator
    TauWPDecorator = TauWPDecorator( name=_name,
                                     flatteningFile0Prong = "rnnid_flat_0p_v4.root",
                                     flatteningFile1Prong = "rnnid_flat_1p_v4.root",
                                     flatteningFile3Prong = "rnnid_flat_mp_v4.root",
                                     CutEnumVals =
                                     [ ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigVeryLoose, ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigLoose,
                                       ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigMedium, ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigTight ],
                                     SigEff0P = [0.98, 0.90, 0.65, 0.50],
                                     SigEff1P = [0.992, 0.99, 0.965, 0.94],
                                     SigEff3P = [0.99, 0.98, 0.865, 0.80],
                                     ScoreName = "RNNJetScore",
                                     NewScoreName = "RNNJetScoreSigTrans",
                                     DefineWPs = True )

    ToolSvc += TauWPDecorator
    cached_instances[_name] = TauWPDecorator
    return TauWPDecorator

########################################################################
# TauWPDecoratorJetRNN
def getTauWPDecoratorJetLLP():

    _name = sPrefix + 'TauWPDecoratorJetLLP'

    if _name in cached_instances:
        return cached_instances[_name]

    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.load_library('libxAODTau_cDict')

    from AthenaCommon.AppMgr import ToolSvc
    from tauRecTools.tauRecToolsConf import TauWPDecorator
    TauWPDecorator = TauWPDecorator( name=_name,
                                     flatteningFile0Prong = "llpdev/rnnid_flat_llp_llz0p_050621-v1.root",
                                     flatteningFile1Prong = "llpdev/rnnid_flat_llp_llz1p_050621-v1.root",
                                     flatteningFile3Prong = "llpdev/rnnid_flat_llp_llzmp_050621-v1.root",
                                     CutEnumVals =
                                     [ ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigVeryLoose, ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigLoose,
                                       ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigMedium, ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigTight ],
                                     SigEff0P = [0.98, 0.90, 0.65, 0.50],
                                     SigEff1P = [0.992, 0.99, 0.965, 0.94],
                                     SigEff3P = [0.99, 0.98, 0.865, 0.80],
                                     ScoreName = "RNNJetScore",
                                     NewScoreName = "RNNJetScoreSigTrans",
                                     DefineWPs = True )

    ToolSvc += TauWPDecorator
    cached_instances[_name] = TauWPDecorator
    return TauWPDecorator


########################################################################
# ParticleCaloExtensionTool
def getParticleCaloExtensionTool():
    _name = sPrefix + 'ParticleCaloExtensionTool'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
    tauParticleCaloExtensionTool=Trk__ParticleCaloExtensionTool(name = _name, Extrapolator = getAtlasExtrapolator())
    
    ToolSvc += tauParticleCaloExtensionTool  
    cached_instances[_name] = tauParticleCaloExtensionTool
    return tauParticleCaloExtensionTool   


