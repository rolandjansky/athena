# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file TauAlgorithmsHolder.py
#
#@brief All tau algorithms needed for tau reconstruction are configured here.
#
#@author Felix Friedrich <felix.friedrich@cern.ch>
################################################################################

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from tauRec.tauRecFlags import tauFlags

cached_instances = {}

sPrefix = 'tauRec_'
bAODmode = False

# standard container names
_DefaultVertexContainer = "PrimaryVertices"
_DefaultTrackContainer ="InDetTrackParticles"

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

########################################################################
# JetSeedBuilder
def getJetSeedBuilder(seed_collection_name):
    _name = sPrefix + 'JetSeedBuilder'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import JetSeedBuilder
    JetSeedBuilder = JetSeedBuilder(name = _name,
            JetCollection = seed_collection_name,
            maxDist = 0.2,
            minPt = 10.*GeV,
            SwitchJetsEmScale = False)
            
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
                                    CellCorrection = True)
                                    
    cached_instances[_name] = TauAxisSetter                
    return TauAxisSetter

########################################################################
# Tau energy calibration
def getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix=''):
 
    _name = sPrefix +'EnergyCalibrationLC' + postfix
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauCalibrateLC
    TauCalibrateLC = TauCalibrateLC(name = _name,
                                    calibrationFile = "TES_MC16a_prelim.root",
                                    doEnergyCorrection = correctEnergy,
                                    doAxisCorrection = correctAxis,
                                    doPtResponse = True,
                                    countOnlyPileupVertices = True,
                                    )
            
    cached_instances[_name] = TauCalibrateLC                
    return TauCalibrateLC

########################################################################
# Tau cell variables calculation
def getCellVariables(cellConeSize=0.2, prefix=''):
    #if prefix is not given, take global one 
    if not prefix: 
        prefix=sPrefix 
 
    _name = prefix + 'CellVariables'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauCellVariables
    TauCellVariables = TauCellVariables(name = _name,
            CellEthreshold = 0.2*GeV,
            StripEthreshold = 0.2*GeV,
            EMSumThreshold = 0.5*GeV,
            EMSumRadius = 0.2,
            CellCone = cellConeSize,
            CellCorrection = True)
            
    cached_instances[_name] = TauCellVariables   
    return TauCellVariables

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
                                                        LinearizedTrackFactory=getTauFullLinearizedTrackFactory(),
                                                        XAODConverter="Trk::VxCandidateXAODVertex/VertexInternalEdmFactory")
    
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
# Tau Test
def getTauTestDump():
    _name = sPrefix + 'TauTestDump'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauTestDump
    TauTestDump = TauTestDump(name = _name)
    
    cached_instances[_name] = TauTestDump
    return TauTestDump

#########################################################################
# Tau Vertex Variables
def getTauVertexVariables():
    _name = sPrefix + 'TauVertexVariables'
    
    if _name in cached_instances:
        return cached_instances[_name]

    from tauRec.tauRecFlags import jobproperties
    useOldSeedFinderAPI = jobproperties.tauRecFlags.useOldVertexFitterAPI()

    from tauRecTools.tauRecToolsConf import TauVertexVariables
    TauVertexVariables = TauVertexVariables(  name = _name,
                                            PrimaryVertexKey  = _DefaultVertexContainer,                                            
                                            TrackToVertexIPEstimator = getTauTrackToVertexIPEstimator(),
                                            VertexFitter = getTauAdaptiveVertexFitter(),
                                            #VertexFitter = "Trk::AdaptiveVertexFitter/InDetAdaptiveVxFitterTool",
                                            SeedFinder = getTauCrossDistancesSeedFinder(),
                                            XAODConverter = "Trk::VxCandidateXAODVertex/VertexInternalEdmFactory", # ATM only needed in case old API is used
                                            TrackParticleContainer = _DefaultTrackContainer, # ATM only needed in case old API is used
                                            useOldSeedFinderAPI = useOldSeedFinderAPI,
                                            runOnAOD = bAODmode,
                                            #OutputLevel = 2                                            
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
                                                          # parameters for CaloIsoCorrected variable
                                                          maxPileUpCorrection = 4000., #MeV
                                                          pileUpAlpha = 1.0,
                                                          VertexCorrection = True,
                                                          inAODmode = bAODmode)
    
    cached_instances[_name] = TauSubstructureVariables
    return TauSubstructureVariables

#########################################################################
# ele veto variables
def getElectronVetoVars():
    _name = sPrefix + 'TauElectronVetoVars'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauElectronVetoVariables
    TauElectronVetoVariables = TauElectronVetoVariables(name = _name,
                                                        CellCorrection = True,
                                                        ParticleCaloExtensionTool = getParticleCaloExtensionTool())
    
    cached_instances[_name] = TauElectronVetoVariables
    return TauElectronVetoVariables


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
# Pi0 algo
# Cluster finder for Pi0 algo
def getPi0ClusterFinder():    
    _name = sPrefix + 'Pi0ClusterFinder'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
    TauCellContainerFinalizer = CaloCellContainerFinalizerTool(name=sPrefix+'tauPi0CellContainerFinalizer')
    
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += TauCellContainerFinalizer
    
    from tauRecTools.tauRecToolsConf import TauPi0CreateROI
    TauPi0CreateROI = TauPi0CreateROI(name = _name,
#        CaloWeightTool = getCellWeightTool(),
#        ExtrapolateToCaloTool = getExtrapolateToCaloTool(),
        CellMakerTool = TauCellContainerFinalizer,
        )
    
    cached_instances[_name] = TauPi0CreateROI
    return TauPi0CreateROI

#####################
# create Pi0 clusters
def getPi0ClusterCreator():
    _name = sPrefix + 'Pi0ClusterCreator'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauPi0ClusterCreator
    TauPi0ClusterCreator = TauPi0ClusterCreator(name = _name,
        InputPi0ClusterContainerName = 'TauPi0SubtractedClusters',
        OutputPi0ClusterContainerName = 'TauPi0Clusters',
        NeutralPFOContainerName= 'TauNeutralParticleFlowObjects',
        HadronicClusterPFOContainerName = 'TauHadronicParticleFlowObjects',
        )
    
    cached_instances[_name] = TauPi0ClusterCreator
    return TauPi0ClusterCreator

#####################
# Set energy of cluster to take care of charged pion energy deposited in the ECAL
def getPi0ClusterScaler(): 
    _name = sPrefix + 'Pi0ClusterScaler'

    if _name in cached_instances:
        return cached_instances[_name]

    from tauRecTools.tauRecToolsConf import TauPi0ClusterScaler
    TauPi0ClusterScaler = TauPi0ClusterScaler(name = _name,
        ChargedPFOContainerName = 'TauChargedParticleFlowObjects',
        runOnAOD = bAODmode,
        )

    cached_instances[_name] = TauPi0ClusterScaler
    return TauPi0ClusterScaler

#####################
# calculate MVA scores of pi0 clusters
def getPi0ScoreCalculator():
    _name = sPrefix + 'Pi0ScoreCalculator'

    if _name in cached_instances:
        return cached_instances[_name]

    from tauRecTools.tauRecToolsConf import TauPi0ScoreCalculator
    TauPi0ScoreCalculator = TauPi0ScoreCalculator(name = _name,
        ReaderOption = 'Silent:!Color',
        BDTWeightFile = 'TauPi0BDTWeights.root',
        )

    cached_instances[_name] = TauPi0ScoreCalculator
    return TauPi0ScoreCalculator

#####################
# select pi0 clusters
def getPi0Selector():
    _name = sPrefix + 'Pi0Selector'

    if _name in cached_instances:
        return cached_instances[_name]

    from tauRec.tauRecFlags import jobproperties
    pi0EtCuts = jobproperties.tauRecFlags.pi0EtCuts()
    pi0MVACuts_1prong = jobproperties.tauRecFlags.pi0MVACuts_1prong()
    pi0MVACuts_mprong = jobproperties.tauRecFlags.pi0MVACuts_mprong()
     

    from tauRecTools.tauRecToolsConf import TauPi0Selector
    TauPi0Selector = TauPi0Selector(name = _name,
        ClusterEtCut         = pi0EtCuts,
        ClusterBDTCut_1prong = pi0MVACuts_1prong,
        ClusterBDTCut_mprong = pi0MVACuts_mprong,
        )

    cached_instances[_name] = TauPi0Selector
    return TauPi0Selector




#########################################################################
# Photon Shot Finder algo
def getTauShotFinder():    
    _name = sPrefix + 'TauShotFinder'
    
    if _name in cached_instances:
        return cached_instances[_name]

    from tauRec.tauRecFlags import jobproperties
    shotPtCut_1Photon = jobproperties.tauRecFlags.shotPtCut_1Photon()
    shotPtCut_2Photons = jobproperties.tauRecFlags.shotPtCut_2Photons()

    #from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
    #TauCellContainerFinalizer = CaloCellContainerFinalizerTool(name=sPrefix+'tauShotCellContainerFinalizer')
    #from AthenaCommon.AppMgr import ToolSvc
    #ToolSvc += TauCellContainerFinalizer
    
    from tauRecTools.tauRecToolsConf import TauShotFinder
    TauShotFinder = TauShotFinder(name = _name,
        CaloWeightTool = getCellWeightTool(),
        #ReaderOption = "Silent:!Color",
        #BDTWeightFile_barrel =  "TauShotsBDTWeights.xml",
        #BDTWeightFile_endcap1 = "TauShotsBDTWeights.xml",
        #BDTWeightFile_endcap2 = "TauShotsBDTWeights.xml",
        NCellsInEta           = 5,
        MinPtCut              = shotPtCut_1Photon,
        AutoDoubleShotCut     = shotPtCut_2Photons,
        MergedBDTScoreCut     = (-9999999.,-9999999.,-9999999.,-9999999.,-9999999.),
        )
    cached_instances[_name] = TauShotFinder
    return TauShotFinder






#########################################################################
def getInDetTrackSelectorTool():
    _name = sPrefix + 'InDetTrackSelectorTool'  
    
    if _name in cached_instances:
        return cached_instances[_name]
    
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
    
    cached_instances[_name] = InDetTrackSelectorTool
    return InDetTrackSelectorTool


#########################################################################
def getInDetTrackSelectionToolForTJVA():
    _name = sPrefix + 'InDetTrackSelectionToolForTJVA'  
    
    if _name in cached_instances:
        return cached_instances[_name]
    
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
    
    cached_instances[_name] = InDetTrackSelectionToolForTJVA
    return InDetTrackSelectionToolForTJVA


############################################################################
def getInDetTrackSelectorToolxAOD():
    _name = sPrefix + 'IInDetTrackSelectionTool'  
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    #Configures tau track selection tool (should eventually check whether an existing one is available)
    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    myInDetTrackSelectionTool = InDet__InDetTrackSelectionTool(name = _name,
                                                               CutLevel="TightPrimary",
                                                               )
                                                                
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += myInDetTrackSelectionTool
    
    cached_instances[_name] = myInDetTrackSelectionTool
    return myInDetTrackSelectionTool


############################################################################
# setup up JVA tools
def setupTauJVFTool():
    from AthenaCommon.AppMgr import ToolSvc

    #Configures tau track selection tool for TJVA
    """
    #Configures tau track selector tool for TJVA
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
    InDetTrackSelectorToolForTJVA = InDet__InDetDetailedTrackSelectorTool(name = sPrefix + 'InDetTrackSelectorToolForTJVA',
                                                                pTMin                = 1000.,
                                                                IPd0Max              = 9999.*mm,
                                                                IPz0Max              = 9999.*mm,                                                                 
                                                                nHitPix              = 2,  # PixelHits + PixelDeadSensors
                                                                nHitSct              = 0,  # SCTHits + SCTDeadSensors
                                                                nHitSi               = 7,  # PixelHits + SCTHits + PixelDeadSensors + SCTDeadSensors
                                                                fitChi2OnNdfMax      = 99999,
                                                                TrackSummaryTool     = None,
                                                                Extrapolator         = getAtlasExtrapolator())        
        
    ToolSvc += InDetTrackSelectorToolForTJVA
    """
    from JetRec.JetRecConf import JetAlgorithm
    jetTrackAlg = JetAlgorithm("JetTrackAlg_forTaus")

    """
    from JetRecTools.JetRecToolsConf import JetTrackSelectionTool
    ToolSvc += JetTrackSelectionTool(InputContainer = _DefaultTrackContainer, 
                                     OutputContainer="JetSelectedTracks_forTaus", 
                                     Selector=InDetTrackSelectorToolForTJVA, 
                                     OutputLevel=2
                                     # what is about ptmin, eta min/max???
                                     )
    """

    from JetRecTools.JetRecToolsConf import TrackVertexAssociationTool
    ToolSvc += TrackVertexAssociationTool(TrackParticleContainer = _DefaultTrackContainer ,
                                          TrackVertexAssociation="JetTrackVtxAssoc_forTaus", 
                                          VertexContainer= _DefaultVertexContainer,
                                          MaxTransverseDistance = 2.5 *mm,
                                          #MaxLongitudinalDistance = 2 *mm, 
                                          MaxZ0SinTheta = 3.0 *mm,
                                          #OutputLevel=2
                                          )
    #jetTrackAlg.Tools = [ToolSvc.JetTrackSelectionTool , ToolSvc.TrackVertexAssociationTool ]                                          
    jetTrackAlg.Tools = [ToolSvc.TrackVertexAssociationTool ]                                          
    
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()    
    topSequence+=jetTrackAlg
    
#########################################################################
def getTauVertexFinder(doUseTJVA=False):
    _name = sPrefix + 'TauVertexFinder'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    if doUseTJVA:
        setupTauJVFTool()
    
    # Algorithm that overwrites numTrack() and charge() of all tauJets in the container
    from tauRecTools.tauRecToolsConf import TauVertexFinder
    TauVertexFinder = TauVertexFinder(name = _name,
                                      UseTJVA                 = doUseTJVA,
                                      PrimaryVertexContainer  = _DefaultVertexContainer,
                                      AssociatedTracks="GhostTrack", # OK??
                                      TrackVertexAssociation="JetTrackVtxAssoc_forTaus",
                                      InDetTrackSelectionToolForTJVA = getInDetTrackSelectionToolForTJVA()
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
def getTauTrackFinder(removeDuplicateTracks=True):
    _name = sPrefix + 'TauTrackFinder'
    
    if _name in cached_instances:
        return cached_instances[_name] 
    
    from tauRecTools.tauRecToolsConf import TauTrackFinder
    TauTrackFinder = TauTrackFinder(name = _name,
                                    MaxJetDrTau = 0.2,
                                    MaxJetDrWide          = 0.4,
                                    TrackSelectorToolTau  = getInDetTrackSelectorTool(),
                                    #TrackSelectorToolTauxAOD  = getInDetTrackSelectorToolxAOD(),
                                    TrackParticleContainer    = _DefaultTrackContainer,
                                    TrackToVertexTool         = getTrackToVertexTool(),
                                    ParticleCaloExtensionTool = getParticleCaloExtensionTool(),
                                    removeDuplicateCoreTracks = removeDuplicateTracks,
                                    #maxDeltaZ0wrtLeadTrk = 2, #in mm
                                    #removeTracksOutsideZ0wrtLeadTrk = True
                                    )
    
    cached_instances[_name] = TauTrackFinder      
    return TauTrackFinder

########################################################################
# TauTrackFilter
def getTauTrackFilter():
    _name = sPrefix + 'TauTrackFilter'
    from tauRecTools.tauRecToolsConf import TauTrackFilter
    TauTrackFilter = TauTrackFilter(name = _name, TrackContainerName=_DefaultTrackContainer)
    cached_instances[_name] = TauTrackFilter
    return TauTrackFilter

########################################################################
# TauGenericPi0Cone
def getTauGenericPi0Cone():
    _name = sPrefix + 'TauGenericPi0Cone'
    from tauRecTools.tauRecToolsConf import TauGenericPi0Cone
    TauGenericPi0Cone = TauGenericPi0Cone(name = _name)
    cached_instances[_name] = TauGenericPi0Cone
    return TauGenericPi0Cone

#end

########################################################################
# MvaTESVariableDecorator
def getMvaTESVariableDecorator():
    _name = sPrefix + 'MvaTESVariableDecorator'
    from tauRecTools.tauRecToolsConf import MvaTESVariableDecorator
    MvaTESVariableDecorator = MvaTESVariableDecorator(name = _name)
    cached_instances[_name] = MvaTESVariableDecorator
    return MvaTESVariableDecorator

########################################################################
# MvaTESEvaluator
def getMvaTESEvaluator():
    _name = sPrefix + 'MvaTESEvaluator'
    from tauRecTools.tauRecToolsConf import MvaTESEvaluator
    MvaTESEvaluator = MvaTESEvaluator(name = _name,
                                      WeightFileName = 'MvaTES_20170207_v2_BDTG.weights.root') #update config?
    cached_instances[_name] = MvaTESEvaluator
    return MvaTESEvaluator

########################################################################
# CombinedP4FromRecoTaus
def getCombinedP4FromRecoTaus():
    _name = sPrefix + 'CombinedP4FromRecoTaus'
    from tauRecTools.tauRecToolsConf import CombinedP4FromRecoTaus
    CombinedP4FromRecoTaus = CombinedP4FromRecoTaus(name = _name,
                                                    WeightFileName = 'CalibLoopResult_v04-04.root') #update config?
    cached_instances[_name] = CombinedP4FromRecoTaus
    return CombinedP4FromRecoTaus
    
#########################################################################
def getTauTrackClassifier():
    _name = sPrefix + 'TauTrackClassifier'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from AthenaCommon.AppMgr import ToolSvc
    from tauRecTools.tauRecToolsConf import tauRecTools__TauTrackClassifier as TauTrackClassifier
    from tauRecTools.tauRecToolsConf import tauRecTools__TrackMVABDT as TrackMVABDT

    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.loadDictionary('xAODTau_cDict')

    # =========================================================================
    _BDT_TTCT_ITFT_0 = TrackMVABDT(name = _name + "_0",
                                   #InputWeightsPath = "TMVAClassification_BDT.weights.root",
                                   #Threshold      = -0.005,
                                   InputWeightsPath = tauFlags.tauRecMVATrackClassificationConfig()[0][0],
                                   Threshold = tauFlags.tauRecMVATrackClassificationConfig()[0][1],
                                   ExpectedFlag   = ROOT.xAOD.TauJetParameters.unclassified, 
                                   SignalType     = ROOT.xAOD.TauJetParameters.classifiedCharged, 
                                   BackgroundType = ROOT.xAOD.TauJetParameters.classifiedIsolation,
                                   calibFolder = tauFlags.tauRecToolsCVMFSPath(), 
                                   )
    ToolSvc += _BDT_TTCT_ITFT_0
    cached_instances[_BDT_TTCT_ITFT_0.name] = _BDT_TTCT_ITFT_0
    
    _BDT_TTCT_ITFT_0_0 = TrackMVABDT(name = _name + "_0_0",
                                     #InputWeightsPath = "TMVAClassification_BDT_0.weights.root",
                                     #Threshold      = -0.0074,
                                     InputWeightsPath = tauFlags.tauRecMVATrackClassificationConfig()[1][0],
                                     Threshold = tauFlags.tauRecMVATrackClassificationConfig()[1][1],
                                     ExpectedFlag   = ROOT.xAOD.TauJetParameters.classifiedCharged,
                                     SignalType     = ROOT.xAOD.TauJetParameters.classifiedCharged,
                                     BackgroundType = ROOT.xAOD.TauJetParameters.classifiedConversion,
                                     calibFolder = tauFlags.tauRecToolsCVMFSPath(),
                                     )
    ToolSvc += _BDT_TTCT_ITFT_0_0
    cached_instances[_BDT_TTCT_ITFT_0_0.name] = _BDT_TTCT_ITFT_0_0
    
    _BDT_TTCT_ITFT_0_1 = TrackMVABDT(name = _name + "_0_1",
                                     #InputWeightsPath = "TMVAClassification_BDT_1.weights.root",
                                     #Threshold      = 0.0005,
                                     InputWeightsPath = tauFlags.tauRecMVATrackClassificationConfig()[2][0],
                                     Threshold = tauFlags.tauRecMVATrackClassificationConfig()[2][1],
                                     ExpectedFlag   = ROOT.xAOD.TauJetParameters.classifiedIsolation, 
                                     SignalType     = ROOT.xAOD.TauJetParameters.classifiedIsolation, 
                                     BackgroundType = ROOT.xAOD.TauJetParameters.classifiedFake,
                                     calibFolder = tauFlags.tauRecToolsCVMFSPath(),
                                     )
    ToolSvc += _BDT_TTCT_ITFT_0_1
    cached_instances[_BDT_TTCT_ITFT_0_1.name] = _BDT_TTCT_ITFT_0_1

    # create tool alg
    myTauTrackClassifier = TauTrackClassifier( name = _name,
                                               Classifiers = [_BDT_TTCT_ITFT_0, _BDT_TTCT_ITFT_0_0, _BDT_TTCT_ITFT_0_1] )
    #ToolSvc += TauTrackClassifier #only add to tool service sub tools to your tool, the main tool will be added via TauRecConfigured
    cached_instances[_name] = myTauTrackClassifier 

    return myTauTrackClassifier

########################################################################                                                                                                             
#                                                                                                                                                                                  
def getTauWPDecoratorJetBDT():
    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.loadDictionary('xAODTau_cDict')

    _name = sPrefix + 'TauWPDecoratorJetBDT'
    from tauRecTools.tauRecToolsConf import TauWPDecorator
    myTauWPDecorator = TauWPDecorator( name=_name,
                                       flatteningFile1Prong = "FlatJetBDT1Pv2.root", #update
                                       flatteningFile3Prong = "FlatJetBDT3Pv2.root", #update
                                       CutEnumVals = 
                                       [ ROOT.xAOD.TauJetParameters.JetBDTSigVeryLoose, ROOT.xAOD.TauJetParameters.JetBDTSigLoose,
                                         ROOT.xAOD.TauJetParameters.JetBDTSigMedium, ROOT.xAOD.TauJetParameters.JetBDTSigTight ],
                                       SigEff1P = [0.95, 0.85, 0.75, 0.60],
                                       SigEff3P = [0.95, 0.75, 0.60, 0.45],
                                       ScoreName = "BDTJetScore",
                                       NewScoreName = "BDTJetScoreSigTrans",
                                       DefineWPs = True,
                                       )
    cached_instances[_name] = myTauWPDecorator
    return myTauWPDecorator


# 
def getTauWPDecoratorEleBDT(flatteningFile1Prong="EleBDTFlat1P.root",
                            flatteningFile3Prong="EleBDTFlat3P.root",
                            retuned=False):
    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.loadDictionary('xAODTau_cDict')

    _name = sPrefix + 'TauWPDecoratorEleBDT'
    from tauRecTools.tauRecToolsConf import TauWPDecorator
    if retuned:
        # Do NOT overwrite the original WPs in isTauFlags
        # (i.e. do NOT set CutEnumVals and SigEff/1P/3P)
        # Decorate separately in the aux instead
        # (i.e. set "DecorWPNames" and "DecorWPCutEff/1P/3P")
        TauScoreFlatteningTool = TauWPDecorator( name=_name,
                                                 flatteningFile1Prong = flatteningFile1Prong,
                                                 flatteningFile3Prong = flatteningFile3Prong,
                                                 UseEleBDT = True ,
                                                 ScoreName = "BDTEleScore_retuned",
                                                 NewScoreName = "BDTEleScoreSigTrans_retuned", #dynamic
                                                 DefineWPs = True,
                                                 DecorWPNames =
                                                 ["BDTEleLoose_retuned",
                                                  "BDTEleMedium_retuned",
                                                  "BDTEleTight_retuned"],
                                                 DecorWPCutEffs1P = [0.95, 0.85, 0.75],
                                                 DecorWPCutEffs3P = [0.95, 0.85, 0.75],
                                                 ) 
    else:
        TauScoreFlatteningTool = TauWPDecorator( name=_name,
                                                 flatteningFile1Prong = flatteningFile1Prong,
                                                 flatteningFile3Prong = flatteningFile3Prong,
                                                 UseEleBDT = True ,
                                                 ScoreName = "BDTEleScore",
                                                 NewScoreName = "BDTEleScoreSigTrans", #dynamic
                                                 DefineWPs = True,
                                                 CutEnumVals = 
                                                 [ROOT.xAOD.TauJetParameters.EleBDTLoose, 
                                                  ROOT.xAOD.TauJetParameters.EleBDTMedium, 
                                                  ROOT.xAOD.TauJetParameters.EleBDTTight],
                                                 SigEff1P = [0.95, 0.85, 0.75],
                                                 SigEff3P = [0.95, 0.85, 0.75],
                                                 ) 
    cached_instances[_name] = TauScoreFlatteningTool
    return TauScoreFlatteningTool


#                                                                                                                                                                                  
def getTauWPDecoratorJetRNN(flatteningFile1Prong="rnnid_prelim_flat_deep_1p_binfix.root",
                            flatteningFile3Prong="rnnid_prelim_flat_deep_3p_binfix.root"):
    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.loadDictionary('xAODTau_cDict')

    _name = sPrefix + 'TauWPDecoratorJetRNN'
    from tauRecTools.tauRecToolsConf import TauWPDecorator
    myTauWPDecorator = TauWPDecorator( name=_name,
                                       flatteningFile1Prong = flatteningFile1Prong,
                                       flatteningFile3Prong = flatteningFile3Prong,
                                       CutEnumVals =
                                       [ ROOT.xAOD.TauJetParameters.JetRNNSigVeryLoose, ROOT.xAOD.TauJetParameters.JetRNNSigLoose,
                                         ROOT.xAOD.TauJetParameters.JetRNNSigMedium, ROOT.xAOD.TauJetParameters.JetRNNSigTight ],
                                       SigEff1P = [0.95, 0.85, 0.75, 0.60],
                                       SigEff3P = [0.95, 0.75, 0.60, 0.45],
                                       ScoreName = "RNNJetScore",
                                       NewScoreName = "RNNJetScoreSigTrans",
                                       DefineWPs = True,
                                       )
    cached_instances[_name] = myTauWPDecorator
    return myTauWPDecorator



def getTauJetBDTEvaluator(suffix="TauJetBDT", weightsFile="", minNTracks=0, maxNTracks=10000, outputVarName="BDTJetScore", GradiantBoost=True, minAbsTrackEta=-1, maxAbsTrackEta=-1):
    _name = sPrefix + suffix
    from tauRecTools.tauRecToolsConf import TauJetBDTEvaluator
    myTauJetBDTEvaluator = TauJetBDTEvaluator(name=_name,
                                              weightsFile=weightsFile, #update config?
                                              minNTracks=minNTracks,
                                              maxNTracks=maxNTracks,
                                              minAbsTrackEta=minAbsTrackEta,
                                              maxAbsTrackEta=maxAbsTrackEta,
                                              outputVarName=outputVarName,
                                              GradiantBoost=GradiantBoost)
    cached_instances[_name] = myTauJetBDTEvaluator
    return myTauJetBDTEvaluator

def getTauJetRNNEvaluator(suffix="TauJetRNN", NetworkFile1P="", NetworkFile3P="", OutputVarname="RNNJetScore", MinChargedTracks=1, MaxTracks=10, MaxClusters=6, MaxClusterDR=1.0, InputLayerScalar="scalar", InputLayerTracks="tracks", InputLayerClusters="clusters", OutputLayer="rnnid_output", OutputNode="sig_prob"):
    _name = sPrefix + suffix
    from tauRecTools.tauRecToolsConf import TauJetRNNEvaluator
    myTauJetRNNEvaluator = TauJetRNNEvaluator(name=_name,
                                              NetworkFile1P=NetworkFile1P,
                                              NetworkFile3P=NetworkFile3P,
                                              OutputVarname=OutputVarname,
                                              MinChargedTracks=MinChargedTracks,
                                              MaxTracks=MaxTracks,
                                              MaxClusters=MaxClusters,
                                              MaxClusterDR=MaxClusterDR,
                                              InputLayerScalar=InputLayerScalar,
                                              InputLayerTracks=InputLayerTracks,
                                              InputLayerClusters=InputLayerClusters,
                                              OutputLayer=OutputLayer,
                                              OutputNode=OutputNode)
    cached_instances[_name] = myTauJetRNNEvaluator
    return myTauJetRNNEvaluator

def getTauIDVarCalculator():
    _name = sPrefix + 'TauIDVarCalculator'
    from tauRecTools.tauRecToolsConf import TauIDVarCalculator            
    myTauIDVarCalculator = TauIDVarCalculator(name=_name,
                                              )
    cached_instances[_name] = myTauIDVarCalculator
    return myTauIDVarCalculator

def getTauEleOLRDecorator():
    _name = sPrefix + 'TauEleOLRDecorator'
    from tauRecTools.tauRecToolsConf import TauEleOLRDecorator
    myTauEleOLRDecorator = TauEleOLRDecorator(name=_name,
                                              ElectronContainerName="Electrons",
                                              EleOLRFile="eVetoAODfix.root")
    cached_instances[_name] = myTauEleOLRDecorator
    return myTauEleOLRDecorator

########################################################################
# FTauDecorator
def getFTauDecorator():
    _name = sPrefix + 'FTauDecorator'
    from tauRecTools.tauRecToolsConf import FTauDecorator
    FTauDecorator = FTauDecorator(name = _name)
    cached_instances[_name] = FTauDecorator
    return FTauDecorator
########################################################################



