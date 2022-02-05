# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

################################################################################
#
#@file TauAlgorithmsHolder.py
#
#@brief All tau algorithms needed for tau reconstruction are configured here.
#
################################################################################

from AthenaCommon.SystemOfUnits import GeV, mm
from tauRec.tauRecFlags import tauFlags

sPrefix = 'tauRec_'
bAODmode = False

# standard container names
_DefaultVertexContainer = "PrimaryVertices"
_DefaultTrackContainer = "InDetTrackParticles"
_DefaultLargeD0TrackContainer = "InDetLargeD0TrackParticles"

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
    
    #Configure the extrapolator
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    theAtlasExtrapolator=AtlasExtrapolator(name = _name)
    theAtlasExtrapolator.DoCaloDynamic = False # this turns off dynamic
    
    ToolSvc += theAtlasExtrapolator
    return theAtlasExtrapolator

########################################################################
# JetSeedBuilder
def getJetSeedBuilder():
    _name = sPrefix + 'JetSeedBuilder'
    
    from tauRecTools.tauRecToolsConf import JetSeedBuilder
    JetSeedBuilder = JetSeedBuilder(name = _name)
            
    return JetSeedBuilder

########################################################################
# Tau energy calibration and tau axis direction
def getTauAxis():
    _name = sPrefix + 'TauAxis'
    
    from tauRecTools.tauRecToolsConf import TauAxisSetter
    TauAxisSetter = TauAxisSetter(  name = _name, 
                                    ClusterCone = 0.2,
                                    VertexCorrection = True )
                                    
    return TauAxisSetter

########################################################################
# Tau energy calibration
def getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix=''):
 
    _name = sPrefix +'EnergyCalibrationLC' + postfix
    
    from tauRecTools.tauRecToolsConf import TauCalibrateLC
    TauCalibrateLC = TauCalibrateLC(name = _name,
                                    calibrationFile = tauFlags.tauRecCalibrateLCConfig(),
                                    Key_vertexInputContainer = _DefaultVertexContainer)
            
    return TauCalibrateLC

########################################################################
# Tau cell variables calculation
def getCellVariables(cellConeSize=0.2, prefix=''):
    #if prefix is not given, take global one 
    if not prefix: 
        prefix=sPrefix 
 
    _name = prefix + 'CellVariables'
    
    from tauRecTools.tauRecToolsConf import TauCellVariables
    TauCellVariables = TauCellVariables(name = _name,
            StripEthreshold = 0.2*GeV,
            CellCone = cellConeSize,
            VertexCorrection = True)
            
    return TauCellVariables

########################################################################
# ParticleCaloExtensionTool
def getParticleCaloExtensionTool():
    _name = sPrefix + 'ParticleCaloExtensionTool'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
    
    tauParticleCaloExtensionTool = Trk__ParticleCaloExtensionTool(name = _name, Extrapolator = getAtlasExtrapolator())
    
    ToolSvc += tauParticleCaloExtensionTool  
    return tauParticleCaloExtensionTool   

                
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
    
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
    TauFullLinearizedTrackFactory=Trk__FullLinearizedTrackFactory(name = _name, Extrapolator = getAtlasExtrapolator())
    
    ToolSvc += TauFullLinearizedTrackFactory
    return TauFullLinearizedTrackFactory

########################################################################
# TauCrossDistancesSeedFinder
def getTauCrossDistancesSeedFinder():
    _name = 'TauCrossDistancesSeedFinder'

    from AthenaCommon.AppMgr import ToolSvc

    #first the seed finder utils
    from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__SeedNewtonTrkDistanceFinder
    TauNewtonTrkDistanceFinder = Trk__SeedNewtonTrkDistanceFinder( name = sPrefix+'TauSeedNewtonTrkDistanceFinder')
    ToolSvc += TauNewtonTrkDistanceFinder
    
    #then the seed finder tools
    from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
    TauCrossDistancesSeedFinder = Trk__CrossDistancesSeedFinder( name = _name, TrkDistanceFinder=TauNewtonTrkDistanceFinder)

    ToolSvc +=TauCrossDistancesSeedFinder
    return TauCrossDistancesSeedFinder

########################################################################
# TauAdaptiveVertexFitter
def getTauAdaptiveVertexFitter():
    _name = sPrefix + 'TauAdaptiveVertexFitter'
    
    from AthenaCommon.AppMgr import ToolSvc
    
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
    
    ToolSvc +=TauAdaptiveVertexFitter
    return TauAdaptiveVertexFitter

########################################################################
# TauTrackToVertexIPEstimator
def getTauTrackToVertexIPEstimator():
    _name = sPrefix + 'TauTrackToVertexIPEstimator'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
    TauTrackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(name = _name,
                                                                Extrapolator=getAtlasExtrapolator(),
                                                                LinearizedTrackFactory=getTauFullLinearizedTrackFactory())
    ToolSvc += TauTrackToVertexIPEstimator
    return TauTrackToVertexIPEstimator                                     

#########################################################################
# Tau Variables
# TODO: rename + rearrange
def getTauCommonCalcVars():
    _name = sPrefix + 'TauCommonCalcVars'
    
    from tauRecTools.tauRecToolsConf import TauCommonCalcVars
    TauCommonCalcVars = TauCommonCalcVars(name = _name)
    
    return TauCommonCalcVars

#########################################################################
# Tau Vertex Variables
def getTauVertexVariables():
    _name = sPrefix + 'TauVertexVariables'
    
    from tauRecTools.tauRecToolsConf import TauVertexVariables
    TauVertexVariables = TauVertexVariables(  name = _name,
                                              VertexFitter = getTauAdaptiveVertexFitter(),
                                              SeedFinder = getTauCrossDistancesSeedFinder(),
                                              )
    
    return TauVertexVariables

#########################################################################
# Tau Variables
# TODO: rename + rerrange
def getTauSubstructure():
    _name = sPrefix + 'TauSubstructure'
    
    from tauRecTools.tauRecToolsConf import TauSubstructureVariables
    TauSubstructureVariables = TauSubstructureVariables(name = _name)
    
    return TauSubstructureVariables

#########################################################################
# ele veto variables
def getElectronVetoVars():
    _name = sPrefix + 'TauElectronVetoVars'
    
    from tauRecTools.tauRecToolsConf import TauElectronVetoVariables
    TauElectronVetoVariables = TauElectronVetoVariables(name = _name,
                                                        VertexCorrection = True,
                                                        ParticleCaloExtensionTool = getParticleCaloExtensionTool(),
                                                        tauEVParticleCache = getParticleCache())
    
    return TauElectronVetoVariables


#########################################################################
# cell weight tool
def getCellWeightTool():
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
    
    return CaloWeightTool

#########################################################################
# Pi0 algo
# Cluster finder for Pi0 algo
def getPi0ClusterFinder():    
    _name = sPrefix + 'Pi0ClusterFinder'
    
    from tauRecTools.tauRecToolsConf import TauPi0CreateROI
    TauPi0CreateROI = TauPi0CreateROI(name = _name,
                                      #        CaloWeightTool = getCellWeightTool(),
                                      #        ExtrapolateToCaloTool = getExtrapolateToCaloTool(),
                                      Key_caloCellInputContainer="AllCalo"
                                      )
    
    return TauPi0CreateROI

#####################
# create Pi0 clusters
def getPi0ClusterCreator():
    _name = sPrefix + 'Pi0ClusterCreator'
    
    from tauRecTools.tauRecToolsConf import TauPi0ClusterCreator
    TauPi0ClusterCreator = TauPi0ClusterCreator(name = _name)
    
    return TauPi0ClusterCreator

#####################
# Set energy of cluster to take care of charged pion energy deposited in the ECAL
def getPi0ClusterScaler(): 
    _name = sPrefix + 'Pi0ClusterScaler'

    from tauRecTools.tauRecToolsConf import TauPi0ClusterScaler
    TauPi0ClusterScaler = TauPi0ClusterScaler(name = _name)

    return TauPi0ClusterScaler

#####################
# calculate MVA scores of pi0 clusters
def getPi0ScoreCalculator():
    _name = sPrefix + 'Pi0ScoreCalculator'

    from tauRecTools.tauRecToolsConf import TauPi0ScoreCalculator
    TauPi0ScoreCalculator = TauPi0ScoreCalculator(name = _name,
        BDTWeightFile = tauFlags.tauRecPi0ScoreConfig(),
        )

    return TauPi0ScoreCalculator

#####################
# select pi0 clusters
def getPi0Selector():
    _name = sPrefix + 'Pi0Selector'

    pi0EtCuts = tauFlags.pi0EtCuts()
    pi0MVACuts_1prong = tauFlags.pi0MVACuts_1prong()
    pi0MVACuts_mprong = tauFlags.pi0MVACuts_mprong()
     

    from tauRecTools.tauRecToolsConf import TauPi0Selector
    TauPi0Selector = TauPi0Selector(name = _name,
        ClusterEtCut         = pi0EtCuts,
        ClusterBDTCut_1prong = pi0MVACuts_1prong,
        ClusterBDTCut_mprong = pi0MVACuts_mprong,
        )

    return TauPi0Selector

#########################################################################
# Photon Shot Finder algo
def getTauShotFinder():    
    _name = sPrefix + 'TauShotFinder'
    
    shotPtCut_1Photon = tauFlags.shotPtCut_1Photon()
    shotPtCut_2Photons = tauFlags.shotPtCut_2Photons()

    from tauRecTools.tauRecToolsConf import TauShotFinder
    TauShotFinder = TauShotFinder(name = _name,
                                  CaloWeightTool = getCellWeightTool(),
                                  NCellsInEta           = 5,
                                  MinPtCut              = shotPtCut_1Photon,
                                  AutoDoubleShotCut     = shotPtCut_2Photons,
                                  Key_caloCellInputContainer="AllCalo"
                                  )
    return TauShotFinder


#########################################################################
def getInDetTrackSelectorTool():
    _name = sPrefix + 'InDetTrackSelectorTool'  
    
    #Configures tau track selector tool (should eventually check whether an existing one is available)
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
    InDetTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = _name,
                                                                pTMin                = 1000.,
                                                                IPd0Max              = 1.,
                                                                IPz0Max              = 1.5, 
                                                                useTrackSummaryInfo  = True,
                                                                nHitBLayer           = 0, 
                                                                nHitPix              = 2,  # PixelHits + PixelDeadSensors
                                                                nHitSct              = 0,  # SCTHits + SCTDeadSensors
                                                                nHitSi               = 7,  # PixelHits + SCTHits + PixelDeadSensors + SCTDeadSensors
                                                                nHitTrt              = 0,  # nTRTHits
                                                                useSharedHitInfo     = False,
                                                                nSharedBLayer        = 99999,
                                                                nSharedPix           = 99999,
                                                                nSharedSct           = 99999,
                                                                nSharedSi            = 99999,
                                                                useTrackQualityInfo  = True,
                                                                fitChi2OnNdfMax      = 99999,
                                                                TrackSummaryTool     = None,
                                                                Extrapolator         = getAtlasExtrapolator())
                                                                
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += InDetTrackSelectorTool
    return InDetTrackSelectorTool


#########################################################################
def getInDetTrackSelectionToolForTJVA():
    _name = sPrefix + 'InDetTrackSelectionToolForTJVA'  
    
    #Configures tau track selector tool (should eventually check whether an existing one is available)
    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    InDetTrackSelectionToolForTJVA = InDet__InDetTrackSelectionTool(name = _name,
                                                                    minPt                = 1000.,
                                                                    maxD0                = 9999.*mm,
                                                                    maxZ0                = 9999.*mm,                                                                 
                                                                    minNPixelHits        = 2,  # PixelHits + PixelDeadSensors
                                                                    minNSctHits          = 0,  # SCTHits + SCTDeadSensors
                                                                    minNSiHits           = 7,  # PixelHits + SCTHits + PixelDeadSensors + SCTDeadSensors
                                                                    minNTrtHits          = 0)
                                                                    #fitChi2OnNdfMax      = 99999,
                                                                    #TrackSummaryTool     = None,
                                                                    #Extrapolator         = getAtlasExtrapolator())
    from AthenaCommon.AppMgr import ToolSvc

    ToolSvc += InDetTrackSelectionToolForTJVA    
    return InDetTrackSelectionToolForTJVA


############################################################################
def getInDetTrackSelectorToolxAOD():
    _name = sPrefix + 'IInDetTrackSelectionTool'  
    
    #Configures tau track selection tool (should eventually check whether an existing one is available)
    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    myInDetTrackSelectionTool = InDet__InDetTrackSelectionTool(name = _name,
                                                               CutLevel="TightPrimary",
                                                               )
                                                                
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += myInDetTrackSelectionTool
    
    return myInDetTrackSelectionTool
    
#########################################################################
def getTauVertexFinder(doUseTJVA=False):
    _name = sPrefix + 'TauVertexFinder'
    
    # Algorithm that overwrites numTrack() and charge() of all tauJets in the container
    from tauRecTools.tauRecToolsConf import TauVertexFinder
    TauVertexFinder = TauVertexFinder(name = _name,
                                      UseTJVA                 = doUseTJVA,
                                      UseTJVA_Tiebreak        = tauFlags.doTJVA_Tiebreak(),
                                      AssociatedTracks="GhostTrack", # OK??
                                      InDetTrackSelectionToolForTJVA = getInDetTrackSelectionToolForTJVA(),
                                      Key_trackPartInputContainer=_DefaultTrackContainer,
                                      Key_vertexInputContainer = _DefaultVertexContainer,
                                      TVATool = getTVATool()
                                      )
    
    return TauVertexFinder 

#########################################################################
def getTrackToVertexTool():
    _name = sPrefix + 'TrackToVertexTool'
    
    from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
    TrackToVertexTool = Reco__TrackToVertex( name = _name,
                                             Extrapolator = getAtlasExtrapolator())
                                             
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += TrackToVertexTool
    
    return TrackToVertexTool

########################################################################
# Tau-Track Association
def getTauTrackFinder(removeDuplicateTracks=True):
    _name = sPrefix + 'TauTrackFinder'
    
    from tauRecTools.tauRecToolsConf import TauTrackFinder
    TauTrackFinder = TauTrackFinder(name = _name,
                                    MaxJetDrTau = 0.2,
                                    MaxJetDrWide          = 0.4,
                                    TrackSelectorToolTau  = getInDetTrackSelectorTool(),
                                    TrackToVertexTool         = getTrackToVertexTool(),
                                    ParticleCaloExtensionTool = getParticleCaloExtensionTool(),
                                    tauParticleCache = getParticleCache(),
                                    removeDuplicateCoreTracks = removeDuplicateTracks,
                                    useGhostTracks = tauFlags.useGhostTracks(),
                                    ghostTrackDR = tauFlags.ghostTrackDR(),
                                    Key_jetContainer = tauFlags.tauRecSeedJetCollection() if tauFlags.useGhostTracks() else "",
                                    Key_trackPartInputContainer = _DefaultTrackContainer,
                                    Key_LargeD0TrackInputContainer = _DefaultLargeD0TrackContainer if tauFlags.associateLRT() else "",
                                    TrackToVertexIPEstimator = getTauTrackToVertexIPEstimator(),
                                    #maxDeltaZ0wrtLeadTrk = 2, #in mm
                                    #removeTracksOutsideZ0wrtLeadTrk = True
                                    )
    
    return TauTrackFinder


# Associate the cluster in jet constituents to the tau candidate
def getTauClusterFinder():
    _name = sPrefix + 'TauClusterFinder'

    from tauRecTools.tauRecToolsConf import TauClusterFinder
    TauClusterFinder = TauClusterFinder(name = _name,)

    return TauClusterFinder


########################################################################
# MvaTESVariableDecorator
def getMvaTESVariableDecorator():
    _name = sPrefix + 'MvaTESVariableDecorator'
    from tauRecTools.tauRecToolsConf import MvaTESVariableDecorator
    MvaTESVariableDecorator = MvaTESVariableDecorator(name = _name,
                                                      Key_vertexInputContainer=_DefaultVertexContainer,
                                                      VertexCorrection = True)

    return MvaTESVariableDecorator

########################################################################
# MvaTESEvaluator
def getMvaTESEvaluator():
    _name = sPrefix + 'MvaTESEvaluator'
    from tauRecTools.tauRecToolsConf import MvaTESEvaluator
    MvaTESEvaluator = MvaTESEvaluator(name = _name,
                                      WeightFileName = tauFlags.tauRecMvaTESConfig(),
                                      WeightFileName0p = tauFlags.tauRec0pMvaTESConfig())

    return MvaTESEvaluator

########################################################################
# TauCombinedTES
def getTauCombinedTES():
    _name = sPrefix + 'TauCombinedTES'
    from tauRecTools.tauRecToolsConf import TauCombinedTES
    TauCombinedTES = TauCombinedTES(name = _name,
                                    WeightFileName = tauFlags.tauRecCombinedTESConfig())

    return TauCombinedTES
    

########################################################################
def getTauTrackRNNClassifier():
    _name = sPrefix + 'TauTrackRNNClassifier'
    
    from AthenaCommon.AppMgr import ToolSvc
    from tauRecTools.tauRecToolsConf import tauRecTools__TauTrackRNNClassifier as TauTrackRNNClassifier
    from tauRecTools.tauRecToolsConf import tauRecTools__TrackRNN as TrackRNN

    import cppyy
    cppyy.load_library('libxAODTau_cDict')

    _RNN = TrackRNN(name = _name + "_TrackRNN",
                    InputWeightsPath = tauFlags.tauRecRNNTrackClassificationConfig(),
                    calibFolder = tauFlags.tauRecToolsCVMFSPath(),
                   )
    ToolSvc += _RNN

    _classifyLRT = True
    if tauFlags.associateLRT() and not tauFlags.classifyLRT():
        _classifyLRT = False

    # create tool alg
    myTauTrackClassifier = TauTrackRNNClassifier( name = _name,
                                                  Classifiers = [_RNN],
                                                  classifyLRT = _classifyLRT )

    return myTauTrackClassifier


def getTauJetRNNEvaluator():
    _name = sPrefix + 'TauJetRNN'
    from tauRecTools.tauRecToolsConf import TauJetRNNEvaluator
    myTauJetRNNEvaluator = TauJetRNNEvaluator(name = _name,
                                              NetworkFile0P = "",
                                              NetworkFile1P = tauFlags.tauRecTauJetRNNConfig()[0],
                                              NetworkFile2P = tauFlags.tauRecTauJetRNNConfig()[1],
                                              NetworkFile3P = tauFlags.tauRecTauJetRNNConfig()[2],
                                              OutputVarname = "RNNJetScore",
                                              MaxTracks = 10,
                                              MaxClusters = 6,
                                              MaxClusterDR = 1.0,
                                              VertexCorrection = True,
                                              InputLayerScalar = "scalar",
                                              InputLayerTracks = "tracks",
                                              InputLayerClusters = "clusters",
                                              OutputLayer = "rnnid_output",
                                              OutputNode = "sig_prob")

    return myTauJetRNNEvaluator


def getTauWPDecoratorJetRNN():
    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.load_library('libxAODTau_cDict')

    _name = sPrefix + 'TauWPDecoratorJetRNN'
    from tauRecTools.tauRecToolsConf import TauWPDecorator
    myTauWPDecorator = TauWPDecorator( name=_name,
                                       flatteningFile1Prong = tauFlags.tauRecTauJetRNNWPConfig()[0],
                                       flatteningFile2Prong = tauFlags.tauRecTauJetRNNWPConfig()[1],
                                       flatteningFile3Prong = tauFlags.tauRecTauJetRNNWPConfig()[2],
                                       CutEnumVals =
                                       [ ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigVeryLoose, ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigLoose,
                                         ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigMedium, ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigTight ],
                                       SigEff1P = [0.95, 0.85, 0.75, 0.60],
                                       SigEff2P = [0.95, 0.75, 0.60, 0.45],
                                       SigEff3P = [0.95, 0.75, 0.60, 0.45],
                                       ScoreName = "RNNJetScore",
                                       NewScoreName = "RNNJetScoreSigTrans",
                                       DefineWPs = True,
                                       )
    return myTauWPDecorator


def getTauIDVarCalculator():
    _name = sPrefix + 'TauIDVarCalculator'
    from tauRecTools.tauRecToolsConf import TauIDVarCalculator   

    myTauIDVarCalculator = TauIDVarCalculator(name=_name,
                                              VertexCorrection = True)

    return myTauIDVarCalculator

def getTauEleRNNEvaluator():
    _name = sPrefix + 'TauEleRNN' 
    from tauRecTools.tauRecToolsConf import TauJetRNNEvaluator
    tool = TauJetRNNEvaluator(name = _name,
                              NetworkFile1P = tauFlags.tauRecTauEleRNNConfig()[0],
                              NetworkFile3P = tauFlags.tauRecTauEleRNNConfig()[1],
                              OutputVarname = "RNNEleScore",
                              MaxTracks = 10,
                              MaxClusters = 6,
                              MaxClusterDR = 1.0,
                              VertexCorrection = True,
                              InputLayerScalar = "scalar",
                              InputLayerTracks = "tracks",
                              InputLayerClusters = "clusters",
                              OutputLayer = "rnneveto_output",
                              OutputNode = "sig_prob")

    return tool

def getTauWPDecoratorEleRNN():
    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.load_library('libxAODTau_cDict')

    _name = sPrefix + 'TauWPDecoratorEleRNN'
    from tauRecTools.tauRecToolsConf import TauWPDecorator
    myTauWPDecorator = TauWPDecorator( name=_name,
                                       flatteningFile1Prong = tauFlags.tauRecTauEleRNNWPConfig()[0],
                                       flatteningFile3Prong = tauFlags.tauRecTauEleRNNWPConfig()[1],
                                       CutEnumVals =
                                       [ ROOT.xAOD.TauJetParameters.IsTauFlag.EleRNNLoose,
                                         ROOT.xAOD.TauJetParameters.IsTauFlag.EleRNNMedium,
                                         ROOT.xAOD.TauJetParameters.IsTauFlag.EleRNNTight ],
                                       SigEff1P = [0.95, 0.90, 0.85],
                                       SigEff3P = [0.98, 0.95, 0.90],
                                       UseEleBDT = True ,
                                       ScoreName = "RNNEleScore",
                                       NewScoreName = "RNNEleScoreSigTrans",
                                       DefineWPs = True,
                                       )
    return myTauWPDecorator
              
def getTauDecayModeNNClassifier():
    _name = sPrefix + 'TauDecayModeNNClassifier'

    from tauRecTools.tauRecToolsConf import TauDecayModeNNClassifier
    TauDecayModeNNClassifier = TauDecayModeNNClassifier(name=_name, WeightFile=tauFlags.tauRecDecayModeNNClassifierConfig())

    return TauDecayModeNNClassifier

def getTauVertexedClusterDecorator():
    from tauRec.tauRecFlags import tauFlags
    from tauRecTools.tauRecToolsConf import TauVertexedClusterDecorator

    _name = sPrefix + 'TauVertexedClusterDecorator'
    
    myTauVertexedClusterDecorator = TauVertexedClusterDecorator(name = _name,
                                                                SeedJet = tauFlags.tauRecSeedJetCollection())
    
    return myTauVertexedClusterDecorator

def getParticleCache():
    #If reading from ESD we not create a cache of extrapolations to the calorimeter, so we should signify this by setting the cache key to a null string
    from RecExConfig.RecFlags import rec
    if rec.doESD():
        ParticleCache = "ParticleCaloExtension"
    else: 
        ParticleCache = ""
    
    return ParticleCache

def getTVATool():
    _name = sPrefix + "TVATool"

    from TrackVertexAssociationTool.getTTVAToolForReco import getTTVAToolForReco
    TVATool = getTTVAToolForReco(name = _name,
                                 WorkingPoint = "Nonprompt_Hard_MaxWeight",
                                 TrackContName = _DefaultTrackContainer,
                                 VertexContName= _DefaultVertexContainer)

    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += TVATool

    return TVATool

def getTauAODSelector():
    _name = sPrefix + 'TauAODSelector'
    from tauRecTools.tauRecToolsConf import TauAODSelector

    myTauAODSelector = TauAODSelector(name=_name,
                                      Min0pTauPt = tauFlags.tauRec0pMinPt(),
                                      MinTauPt = tauFlags.tauRecMinPt())
    return myTauAODSelector
