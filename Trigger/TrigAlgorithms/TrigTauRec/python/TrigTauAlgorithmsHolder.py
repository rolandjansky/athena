# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file TrigTauAlgorithmsHolder.py
#
#@brief All tau algorithms needed for tau reconstruction are configured here.
#
#@author Felix Friedrich <felix.friedrich@cern.ch>
################################################################################

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *

cached_instances = {}

sPrefix = 'TrigTau_'
bAODmode = False
doCellCorrection = False

# standard container names
_DefaultVertexContainer = "PrimaryVertices" #????
_DefaultTrackContainer ="InDetTrackParticles"  #????
_DefaultTrigTauTrackContainer ="TrigTauTrackCandidate" #????

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
def getJetSeedBuilder():
    _name = sPrefix + 'JetSeedBuilder'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import JetSeedBuilder
    JetSeedBuilder = JetSeedBuilder(name = _name,
            #JetCollection = seed_collection_name,
            maxDist = 0.2,
            minPt = 10.*GeV,
            SwitchJetsEmScale = False)
            
    cached_instances[_name] = JetSeedBuilder
    return JetSeedBuilder

########################################################################
# Tau energy calibration and tau axis direction
def getPileUpCorrection():
    _name = sPrefix + 'IDPileUp'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauIDPileupCorrection
    TauPileUp = TauIDPileupCorrection( name = _name, 
                                       averageEstimator = 20.,
                                       calibrationFile1Prong = "fitted.pileup_1prong_hlt_2015.root",
                                       calibrationFile3Prong = "fitted.pileup_multiprongs_hlt_2015.root",
                                       useMu = True)

    cached_instances[_name] = TauPileUp
    return TauPileUp

########################################################################
# Tau energy calibration and tau axis direction
def getTauAxis():
    _name = sPrefix + 'TauAxis'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauAxisSetter
    TauAxisSetter = TauAxisSetter(  name = _name, 
                                    ClusterCone = 0.2,
                                    CellCorrection = doCellCorrection,
                                    AxisCorrection = False)
    # No Axis correction at trigger level
                                    
    cached_instances[_name] = TauAxisSetter                
    return TauAxisSetter

########################################################################
# Tau energy calibration
def getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='', caloOnly=False):
 
    _name = sPrefix +'EnergyCalibrationLC' + postfix
    
    if _name in cached_instances:
        return cached_instances[_name]


##    calibFileName = "TES2015_LC_online.root"
    calibFileName = "TES2016_LC_online.root"
    if caloOnly == True :
##        calibFileName = "TES2015_LC_online.root"
        calibFileName = "TES2016_LC_online_inc.root"
    
    from tauRecTools.tauRecToolsConf import TauCalibrateLC
    TauCalibrateLC = TauCalibrateLC(name = _name,
                                    #calibrationFile = "EnergyCalibrationLC2012_retuned.root",
                                    calibrationFile = calibFileName,
                                    doEnergyCorrection = correctEnergy,
                                    doAxisCorrection = correctAxis)

    TauCalibrateLC.isCaloOnly = caloOnly
            
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
            CellCorrection = doCellCorrection)
            
    cached_instances[_name] = TauCellVariables   
    return TauCellVariables


                
########################################################################
# calibrate tau at EM scale
def getEnergyCalibrationEM():
    _name = sPrefix + 'EnergyCalibrationEM'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauCalibrateEM
    TauCalibrateEM = TauCalibrateEM(name = _name, response_functions_file = "EMTES_Fits_Oct2010.root")
    
    cached_instances[_name] = TauCalibrateEM
    return TauCalibrateEM

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
    from TrigInDetConf.TrigInDetRecVtxTools import InDetTrigVxEdmCnv
    TauAdaptiveVertexFitter = Trk__AdaptiveVertexFitter(name = _name, 
                                                        SeedFinder=getTauCrossDistancesSeedFinder(), 
                                                        ImpactPoint3dEstimator=TauInDetImpactPoint3dEstimator, 
                                                        VertexSmoother=TauSequentialVertexSmoother, 
                                                        AnnealingMaker=TauDetAnnealingMaker,
                                                        LinearizedTrackFactory=getTauFullLinearizedTrackFactory(),
                                                        XAODConverter=InDetTrigVxEdmCnv)
    
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

########################################################################
# lock tau containers
def getContainerLock():
    _name = sPrefix + 'TauContainerLock'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import LockTauContainers
    LockTauContainers = LockTauContainers(name = _name)
    
    cached_instances[_name] = LockTauContainers 
    return LockTauContainers       

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
    from TrigInDetConf.TrigInDetRecVtxTools import InDetTrigVxEdmCnv
    TauVertexVariables = TauVertexVariables(  name = _name,
                                            PrimaryVertexKey  = _DefaultVertexContainer,                                            
                                            TrackToVertexIPEstimator = getTauTrackToVertexIPEstimator(),
                                            VertexFitter = getTauAdaptiveVertexFitter(),
                                            #VertexFitter = "Trk::AdaptiveVertexFitter/InDetAdaptiveVxFitterTool",
                                            SeedFinder = getTauCrossDistancesSeedFinder(),
                                            XAODConverter = InDetTrigVxEdmCnv, # ATM only needed in case old API is used
                                            TrackParticleContainer = _DefaultTrackContainer, # ATM only needed in case old API is used
                                            useOldSeedFinderAPI = useOldSeedFinderAPI,
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
                                                          VertexCorrection = doCellCorrection,
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
                                                        CellCorrection = doCellCorrection)
    
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
# Bonn Pi0 algo
# Cluster finder for Pi0 algo
def getBonnPi0ClusterFinder():    
    _name = sPrefix + 'BonnPi0ClusterFinder'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
    TauCellContainerFinalizer = CaloCellContainerFinalizerTool(name=sPrefix+'tauPi0CellContainerFinalizer')
    
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += TauCellContainerFinalizer
    
    from tauRecTools.tauRecToolsConf import TauPi0BonnCreateROI
    TauPi0BonnCreateROI = TauPi0BonnCreateROI(name = _name,
        CaloWeightTool = getCellWeightTool(),
        CellMakerTool = TauCellContainerFinalizer,
        #LonParFile = "longitudinal_para.dat",
        #LatParFile = "lateral_para.dat",
        LatParFile = "lateral_para.root",
        #OriginCorrectionTool = getTauCellCorrection(),
        ChargedPFOContainerName = 'TauPi0BonnChargedPFOContainer',
        )
    
    cached_instances[_name] = TauPi0BonnCreateROI
    return TauPi0BonnCreateROI

#####################
# create Pi0 clusters
def getBonnPi0ClusterCreator():
    _name = sPrefix + 'BonnPi0ClusterCreator'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauPi0BonnClusterCreator
    TauPi0BonnClusterCreator = TauPi0BonnClusterCreator(name = _name,
        InputPi0ClusterContainerName = 'TauPi0BonnSubtractedClusterContainer',
        OutputPi0ClusterContainerName = 'TauPi0BonnClusterContainer',
        NeutralPFOContainerName= 'TauPi0BonnNeutralPFOContainer',
        )
    
    cached_instances[_name] = TauPi0BonnClusterCreator
    return TauPi0BonnClusterCreator

#####################
# calculate MVA scores of pi0 clusters
def getPi0BonnScoreCalculator():
    _name = sPrefix + 'BonnPi0ScoreCalculator'

    if _name in cached_instances:
        return cached_instances[_name]

    from tauRecTools.tauRecToolsConf import TauPi0BonnScoreCalculator
    TauPi0BonnScoreCalculator = TauPi0BonnScoreCalculator(name = _name,
        ReaderOption = 'Silent:!Color',
        BDTWeightFile = 'TauPi0BonnBDTWeights.xml',
        )

    cached_instances[_name] = TauPi0BonnScoreCalculator
    return TauPi0BonnScoreCalculator

#####################
# select pi0 clusters
def getPi0BonnSelector():
    _name = sPrefix + 'BonnPi0Selector'

    if _name in cached_instances:
        return cached_instances[_name]

    from tauRecTools.tauRecToolsConf import TauPi0BonnSelector
    TauPi0BonnSelector = TauPi0BonnSelector(name = _name,
        ClusterEtCut         = (2100.*MeV,2500.*MeV,2600.*MeV,2400.*MeV,1900.*MeV),
        ClusterBDTCut_1prong = (0.46,0.39,0.51,0.47,0.54),
        ClusterBDTCut_mprong = (0.47,0.52,0.60,0.55,0.50),
        )

    cached_instances[_name] = TauPi0BonnSelector
    return TauPi0BonnSelector




#########################################################################
# Photon Shot Finder algo
def getTauShotFinder():    
    _name = sPrefix + 'TauShotFinder'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    #from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
    #TauCellContainerFinalizer = CaloCellContainerFinalizerTool(name=sPrefix+'tauShotCellContainerFinalizer')
    #from AthenaCommon.AppMgr import ToolSvc
    #ToolSvc += TauCellContainerFinalizer
    
    from tauRecTools.tauRecToolsConf import TauShotFinder
    TauShotFinder = TauShotFinder(name = _name,
        CaloWeightTool = getCellWeightTool(),
        ReaderOption = "Silent:!Color",
        BDTWeightFile_barrel =  "TauShotsBDTWeights.xml",
        BDTWeightFile_endcap1 = "TauShotsBDTWeights.xml",
        BDTWeightFile_endcap2 = "TauShotsBDTWeights.xml",
        NCellsInEta           = 5,
        MinPtCut              = (400.*MeV,320.*MeV,9999999.*MeV,350.*MeV,320.*MeV),
        AutoDoubleShotCut     = (10000.*MeV,10000.*MeV,9999999.*MeV,10000.*MeV,10000.*MeV),
        MergedBDTScoreCut     = (-9999999.,-9999999.,-9999999.,-9999999.,-9999999.),
        )
    cached_instances[_name] = TauShotFinder
    return TauShotFinder




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


############################################################################
# setup up JVA tools
def setupTauJVFTool():
    from AthenaCommon.AppMgr import ToolSvc

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
    jetTrackAlg = JetAlgorithm(sPrefix+"JetTrackAlg_forTaus")

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
                                          TrackVertexAssociation=sPrefix+"JetTrackVtxAssoc_forTaus", 
                                          VertexContainer= _DefaultVertexContainer,
                                          MaxTransverseDistance = 2.5 *mm,
                                          MaxLongitudinalDistance = 2 *mm, 
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

    # ATR-15665: commented out, track-vertex association done directly in TauVertexFinder    
    #if doUseTJVA:
    #    setupTauJVFTool()
    
    # Algorithm that overwrites numTrack() and charge() of all tauJets in the container
    from tauRecTools.tauRecToolsConf import TauVertexFinder
    TauVertexFinder = TauVertexFinder(name = _name,
                                      UseTJVA                 = doUseTJVA,
                                      PrimaryVertexContainer  = _DefaultVertexContainer,
                                      AssociatedTracks="GhostTrack", # OK??
                                      TrackVertexAssociation=sPrefix+"JetTrackVtxAssoc_forTaus",
                                      InDetTrackSelectionToolForTJVA = getInDetTrackSelectionToolForTJVA(),
                                      OnlineMaxTransverseDistance = 2.5*mm,   # ATR-15665
                                      #OnlineMaxLongitudinalDistance = 2 *mm,
                                      OnlineMaxZ0SinTheta = 3.0 *mm    
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
    _name = sPrefix + 'TauTrackFinder'
    
    if _name in cached_instances:
        return cached_instances[_name] 
    
    from tauRecTools.tauRecToolsConf import TauTrackFinder
    TauTrackFinder = TauTrackFinder(name = _name,
                                    MaxJetDrTau = 0.2,
                                    MaxJetDrWide          = 0.4,
                                    TrackSelectorToolTau  = getInDetTrackSelectorTool(),
                                    TrackParticleContainer    = _DefaultTrigTauTrackContainer,  #???
                                    TrackToVertexTool         = getTrackToVertexTool(),
                                    maxDeltaZ0wrtLeadTrk = maxDeltaZ0, #in mm
                                    removeTracksOutsideZ0wrtLeadTrk = applyZ0cut,
                                    BypassSelector = noSelector,
                                    BypassExtrapolator = True
                                    )
    # Selector not needed for fast-tracks
    # Extrapolator never needed

    cached_instances[_name] = TauTrackFinder      
    return TauTrackFinder

########################################################################
# TauTrackFilter
def getTauTrackFilter():
    _name = sPrefix + 'TauTrackFilter'
    from tauRecTools.tauRecToolsConf import TauTrackFilter
    TauTrackFilter = TauTrackFilter(name = _name, TrackContainerName=_DefaultTrigTauTrackContainer)  #???)
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

########################################################################
# TauGenericPi0Cone
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

    input_file_name = 'ftf_tracks_mva_classifier.root'
    threshold = 0.5
    # =========================================================================
    _ftf_tracks_mva_bdt = TrackMVABDT(
        name = _name + "_0",
        # calibFolder='data/TrigTauRec',
        calibFolder='TrigTauRec/00-11-01/',
        inTrigger=True,
        InputWeightsPath=input_file_name,
        Threshold=threshold,
        ExpectedFlag   = ROOT.xAOD.TauJetParameters.unclassified, 
        SignalType     = ROOT.xAOD.TauJetParameters.classifiedIsolation, 
        BackgroundType = ROOT.xAOD.TauJetParameters.classifiedCharged)
    ToolSvc += _ftf_tracks_mva_bdt

    classifier = TauTrackClassifier(
        name=_name, 
        Classifiers=[_ftf_tracks_mva_bdt])
        # TauTrackContainerName=_DefaultTrigTauTrackContainer)
    cached_instances[_name] = classifier
    return classifier

# end




""" obsolete methods

########################################################################
# Tau Origin Cell Correction Tool
def getTauCellCorrection():
    _name = sPrefix + 'TauCellCorrection'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    if _name in cached_instances:
        return cached_instances[_name]

    from tauRec.tauRecConf import TauOriginCorrectionTool
    TauCellCorrectionTool = TauOriginCorrectionTool(name = _name,
            UseJVA = False,  #not using JetVertexAssociation, b/c JetRec doesn't use it too
            UsePrimaryVertex = True,
            UseBeamSpot = True,
            VertexContainerKey = "PrimaryVertices")
    
    ToolSvc += TauCellCorrectionTool
    cached_instances[_name] = TauCellCorrectionTool         
    return TauCellCorrectionTool

#########################################################################
# CaloNoiseTool
def getCaloNoiseTool():
    _name = 'CaloNoiseTool'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    theCaloNoiseTool = CaloNoiseToolDefault()
    
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += theCaloNoiseTool
    
    cached_instances[_name] = theCaloNoiseTool
    return theCaloNoiseTool

#########################################################################
# tau1p3p track match cells
def getTauEflowTrackMatchCells():
    _name = sPrefix + 'EflowTrackMatchCells'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRec.tauRecConf import TauEflowTrackMatchCells
    TauEflowTrackMatchCells = TauEflowTrackMatchCells(name = _name,
                detRIsolCaloCut   = 0.4,
                useNoiseSigma     = 1,
                AbsNoiseSigma_cut = 2,
                CaloNoiseTool     = getCaloNoiseTool(),
                selectConeSize    = 0.45, #not used anymore
                CellCorrection = True,
                OriginCorrectionTool = getTauCellCorrection()) 
    
    cached_instances[_name] = TauEflowTrackMatchCells         
    return TauEflowTrackMatchCells

#########################################################################
# tau1p3p AddCaloInfo
def getTauEflowAddCaloInfo():
    _name = sPrefix + 'EflowAddCaloInfo'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRec.tauRecConf import TauEflowAddCaloInfo
    TauEflowAddCaloInfo = TauEflowAddCaloInfo(name = _name,
                detRCoreCaloCut     = 0.2,
                detRIsolCaloCut     = 0.4,
                ETCellMinCut        = 0.1*GeV,
                ETStripMinCut       = 0.2*GeV,
                detaStripCut        = 0.2,
                CellCorrection = True,
                OriginCorrectionTool = getTauCellCorrection())  
    
    cached_instances[_name] = TauEflowAddCaloInfo         
    return TauEflowAddCaloInfo

#########################################################################
# tau1p3p eflow info
def getTauEflowVariables():
    _name = sPrefix + 'EflowVariables'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRec.tauRecConf import TauEflowVariables
    TauEflowVariables = TauEflowVariables(name = _name,
                detRCoreCaloCut = 0.2,
                dphiEMCLCut     = 0.0375,
                detaEMCLCut     = 0.0375,
                dphiEMCLFACCut  = 2,
                detaEMCLFACCut  = 3,
                dphiChrgEMCut   = 0.0375,
                detaChrgEMCut   = 0.0375,
                CaloClusterContainerName = "EMTopoForTaus", #TODO: rec.scoping<3 case??
                RecoTopoClusterETCut     = 0.2*GeV,
                RecoEtaCut               = 2.5,
                TrackTopoClusPhi2Cut     = 0.0375,
                TrackTopoClusEta1Cut     = 0.01,
                MVisEflowCut             = 10.*GeV,
                MTrk3PCut                = 10.*GeV,
                ETeflow_ETcaloCut        = 10.,
                ETeflow_ETcaloCutMin     = 0.1,
                useEMTopoClusters        = True,
                CellCorrection = True,
                OriginCorrectionTool = getTauCellCorrection())
    
    cached_instances[_name] = TauEflowVariables         
    return TauEflowVariables  
    
#####################
# Pi0 Creator Chooser
def getPi0CreatorChooser():
    _name = sPrefix + 'Pi0CreatorChooser'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += getBonnPi0ClusterCreator()
    ToolSvc += getCrakowPi0ClusterCreator()
    
    from tauRec.tauRecConf import TauPi0CreatorChooser
    TauPi0CreatorChooser = TauPi0CreatorChooser(name = _name,
                Tau1p3pCreatePi0ClusTool = getCrakowPi0ClusterCreator(),
                TauCommonCreatePi0ClusTool = getBonnPi0ClusterCreator())
    
    cached_instances[_name] = TauPi0CreatorChooser
    return TauPi0CreatorChooser 

#########################################################################
# Crakow Pi0/eflow algorithm
# Cluster/Cellfinder for Pi0/Eflow algos
def getPi0EflowCreateROI():
    _name = sPrefix + 'TauPi0EflowCreateROI'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRec.tauRecConf import TauPi0EflowCreateROI
    TauPi0EflowCreateROI = TauPi0EflowCreateROI( name = _name,
                detRIsolCaloCut   = 0.4,
                detRCoreCaloCut   = 0.2,
                useNoiseSigma     = 0,
                AbsNoiseSigma_cut = 2,
                removeChrgEM01    = 1,
                removeChrgEM2     = 1,
                detRChrgEMCut     = 0.0375,
                # Added by SL
                fillCellContainer = TRUE,
                CellsOutputContainerName = "TauCells",
                CaloNoiseTool     = getCaloNoiseTool(),
                CaloCellMakerToolNames = ["CaloCellContainerFinalizerTool/cellfinalizerForTaus","CaloCellContainerCheckerTool/cellcheckForTaus"],
                CellCorrection = True,
                OriginCorrectionTool = getTauCellCorrection())   
    
    cached_instances[_name] = TauPi0EflowCreateROI
    return TauPi0EflowCreateROI

################
# Pi0 Clustering
def getCrakowPi0ClusterCreator():
    _name = sPrefix + 'CrakowPi0ClusterCreator'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRec.tauRecConf import TauPi0CrakowClusterCreator
    TauPi0CrakowClusterCreator = TauPi0CrakowClusterCreator( name = _name,
                detRCoreCaloCut = 0.2,
                CaloClusterContainerName = "EMTopoForTaus",  #TODO: rec.scoping<3 case??
                RecoTopoClusterETCut     = 1.0*GeV,
                RecoEtaCut               = 2.5,
                detTrkClusMin            = 0.0375,
                fracEM01verEM            = 0.1)
    
    cached_instances[_name] = TauPi0CrakowClusterCreator
    return TauPi0CrakowClusterCreator  

########################################################################
# set track infos (charge + global track collection)
def getTauSetTracksAndCharge():
    _name = sPrefix + 'TauSetTracksAndCharge'
    
    if _name in cached_instances:
        return cached_instances[_name] 
    
    from tauRec.tauRecConf import TauSetTracksAndCharge
    TauSetTracksAndCharge = TauSetTracksAndCharge(name = _name, TrackContainer    = "TrackParticleCandidate")
    
    cached_instances[_name] = TauSetTracksAndCharge        
    return TauSetTracksAndCharge

"""
