# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

################################################################################
#
#@file TauToolHolder.py
#
#@brief All tau tools needed for tau reconstruction are configured here.
#
################################################################################

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.AccumulatorCache import AccumulatorCache
from AthenaConfiguration.Enums import ProductionStep
from AthenaCommon.SystemOfUnits import GeV, MeV, mm, deg
from tauRec.tauRecFlags import tauFlags

cached_instances = {}

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
# JetSeedBuilder
@AccumulatorCache
def JetSeedBuilderCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'JetSeedBuilder'

    JetSeedBuilder = CompFactory.getComp("JetSeedBuilder")
    JetSeedBuilder = JetSeedBuilder(name = _name)
    
    result.setPrivateTools(JetSeedBuilder)
    return result

#########################################################################
def InDetTrackSelectionToolForTJVACfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'InDetTrackSelectionToolForTJVA'

    # Configures tau track selector tool (should eventually check whether an existing one is available)
    InDet__InDetTrackSelectionTool = CompFactory.InDet.InDetTrackSelectionTool
    InDetTrackSelectionToolForTJVA = InDet__InDetTrackSelectionTool(name = _name,
                                                                    minPt                = 1000.,
                                                                    maxD0                = 9999.*mm,
                                                                    maxZ0                = 9999.*mm,
                                                                    minNPixelHits        = 2,  # PixelHits + PixelDeadSensors
                                                                    minNSctHits          = 0,  # SCTHits + SCTDeadSensors
                                                                    minNSiHits           = 7,  # PixelHits + SCTHits + PixelDeadSensors + SCTDeadSensors
                                                                    minNTrtHits          = 0)

    result.setPrivateTools(InDetTrackSelectionToolForTJVA)
    return result

#########################################################################
def TVAToolCfg(flags):
    _name = sPrefix + "TVATool"

    from TrackVertexAssociationTool.TTVAToolConfig import TTVAToolCfg as _TTVAToolCfg
    # returns a component accumulator instance, also sets UsedInFitDecorator
    TVAToolCA = _TTVAToolCfg(flags, _name,
                           WorkingPoint = "Nonprompt_Hard_MaxWeight",
                           TrackContName = flags.Tau.TrackCollection,
                           VertexContName = flags.Tau.VertexCollection)
    return TVAToolCA

#########################################################################
def TauVertexFinderCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauVertexFinder'

    # Algorithm that overwrites numTrack() and charge() of tauJets in container
    # from tauRecTools.tauRecToolsConf import TauVertexFinder
    TauVertexFinder = CompFactory.getComp("TauVertexFinder")
    TauVertexFinder = TauVertexFinder(name = _name,
                                      UseTJVA                 = flags.Tau.doTJVA,
                                      UseTJVA_Tiebreak        = flags.Tau.doTJVATiebreak,
                                      AssociatedTracks="GhostTrack", # OK??
                                      InDetTrackSelectionToolForTJVA = result.popToolsAndMerge(InDetTrackSelectionToolForTJVACfg(flags)),
                                      Key_trackPartInputContainer= flags.Tau.TrackCollection,
                                      Key_vertexInputContainer = flags.Tau.VertexCollection,
                                      TVATool = result.popToolsAndMerge(TVAToolCfg(flags)) )

    result.setPrivateTools(TauVertexFinder)
    return result

########################################################################
# Tau energy calibration and tau axis direction
def TauAxisCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauAxis'

    TauAxisSetter = CompFactory.getComp("TauAxisSetter")
    TauAxisSetter = TauAxisSetter(  name = _name,
                                    ClusterCone = 0.2,
                                    VertexCorrection = True )

    result.setPrivateTools(TauAxisSetter)
    return result

#########################################################################
def InDetTrackSelectorToolCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'InDetTrackSelectorTool'

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg(flags))

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    result.merge(BeamSpotCondAlgCfg(flags))

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    #Configures tau track selector tool (should eventually check whether an existing one is available)
    InDet__InDetDetailedTrackSelectorTool = CompFactory.InDet.InDetDetailedTrackSelectorTool
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
                                                                   Extrapolator         = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags)) )

    result.setPrivateTools(InDetTrackSelectorTool)
    return result

#########################################################################
def TrackToVertexToolCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TrackToVertexTool'

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    result.merge(BeamSpotCondAlgCfg(flags))

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    Reco__TrackToVertex = CompFactory.Reco.TrackToVertex
    TrackToVertexTool = Reco__TrackToVertex( name = _name,
                                             Extrapolator = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags)) )

    result.setPrivateTools(TrackToVertexTool)
    return result


def getParticleCache(flags):
    #If reading from ESD we not create a cache of extrapolations to the calorimeter, so we should signify this by setting the cache key to a null string
    if flags.Common.ProductionStep is ProductionStep.Reconstruction:
        ParticleCache = "ParticleCaloExtension"
    else:
        ParticleCache = ""

    return ParticleCache

########################################################################
# TauFullLinearizedTrackFactory
def TauFullLinearizedTrackFactoryCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauFullLinearizedTrackFactory'

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg(flags))

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    Trk__FullLinearizedTrackFactory = CompFactory.Trk.FullLinearizedTrackFactory
    TauFullLinearizedTrackFactory = Trk__FullLinearizedTrackFactory(name = _name,
                                                                    Extrapolator = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags)) )

    result.setPrivateTools(TauFullLinearizedTrackFactory)
    return result

########################################################################
# TauTrackToVertexIPEstimator
def TauTrackToVertexIPEstimatorCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauTrackToVertexIPEstimator'

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    Trk__TrackToVertexIPEstimator = CompFactory.Trk.TrackToVertexIPEstimator
    TauTrackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(name = _name,
                                                                Extrapolator = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags)),
                                                                LinearizedTrackFactory=result.popToolsAndMerge(TauFullLinearizedTrackFactoryCfg(flags)) )

    result.setPrivateTools(TauTrackToVertexIPEstimator)
    return result

########################################################################
# Tau-Track Association
def TauTrackFinderCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauTrackFinder'

    # BeamSpot Conditions
    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    result.merge(BeamSpotCondAlgCfg(flags))

    from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg

    TauTrackFinder = CompFactory.getComp("TauTrackFinder")
    TauTrackFinder = TauTrackFinder(name = _name,
                                    MaxJetDrTau = 0.2,
                                    MaxJetDrWide = 0.4,
                                    TrackSelectorToolTau      = result.popToolsAndMerge(InDetTrackSelectorToolCfg(flags)),
                                    TrackToVertexTool         = result.popToolsAndMerge(TrackToVertexToolCfg(flags)),
                                    ParticleCaloExtensionTool = result.popToolsAndMerge(ParticleCaloExtensionToolCfg(flags)),
                                    tauParticleCache = getParticleCache(flags), # only returns a string
                                    removeDuplicateCoreTracks = flags.Tau.RemoveDupeCoreTracks,
                                    useGhostTracks = flags.Tau.useGhostTracks,
                                    ghostTrackDR = flags.Tau.ghostTrackDR,
                                    Key_jetContainer = (flags.Tau.SeedJetCollection if flags.Tau.useGhostTracks else ""),
                                    Key_trackPartInputContainer = flags.Tau.TrackCollection,
                                    Key_LargeD0TrackInputContainer = (flags.Tau.LargeD0TrackContainer if flags.Tau.associateLRT else ""),
                                    TrackToVertexIPEstimator = result.popToolsAndMerge(TauTrackToVertexIPEstimatorCfg(flags)) )
                                    #maxDeltaZ0wrtLeadTrk = 2, #in mm
                                    #removeTracksOutsideZ0wrtLeadTrk = True
    
    result.setPrivateTools(TauTrackFinder)
    return result

########################################################################
# Associate the cluster in jet constituents to the tau candidate
def TauClusterFinderCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauClusterFinder'


    TauClusterFinder = CompFactory.getComp("TauClusterFinder")
    TauClusterFinder = TauClusterFinder(name = _name)


    result.setPrivateTools(TauClusterFinder)
    return result

########################################################################
def TauVertexedClusterDecoratorCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauVertexedClusterDecorator'

    TauVertexedClusterDecorator = CompFactory.getComp("TauVertexedClusterDecorator")
    myTauVertexedClusterDecorator = TauVertexedClusterDecorator(name = _name,
                                                                SeedJet = flags.Tau.SeedJetCollection)

    result.setPrivateTools(myTauVertexedClusterDecorator)
    return result

########################################################################
def TauTrackRNNCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauTrackRNNClassifier'

    TrackRNN = CompFactory.tauRecTools.TrackRNN
    _RNN = TrackRNN(name = _name + "_TrackRNN",
                    InputWeightsPath = flags.Tau.tauRNNTrackClassConfig,
                    calibFolder = flags.Tau.tauRecToolsCVMFSPath )

    result.setPrivateTools(_RNN)
    return result

########################################################################
def TauTrackRNNClassifierCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauTrackRNNClassifier'

    # create tool alg
    TauTrackRNNClassifier = CompFactory.tauRecTools.TauTrackRNNClassifier

    import cppyy
    cppyy.load_library('libxAODTau_cDict')

    _classifyLRT = True
    if flags.Tau.associateLRT and not flags.Tau.classifyLRT:
        _classifyLRT = False

    myTauTrackClassifier = TauTrackRNNClassifier( name = _name,
                                                  Classifiers = [ result.popToolsAndMerge(TauTrackRNNCfg(flags)) ],
                                                  classifyLRT = _classifyLRT )

    result.setPrivateTools(myTauTrackClassifier)
    return result

########################################################################
# Tau energy calibration
def EnergyCalibrationLCCfg(flags, correctEnergy=True, correctAxis=False, postfix=''):
    result = ComponentAccumulator()
    _name = sPrefix +'EnergyCalibrationLC' + postfix

    TauCalibrateLC=CompFactory.getComp("TauCalibrateLC")
    TauCalibrateLC = TauCalibrateLC(name = _name,
                                    calibrationFile = tauFlags.tauRecCalibrateLCConfig(),
                                    Key_vertexInputContainer = flags.Tau.VertexCollection)
            
    result.setPrivateTools(TauCalibrateLC)
    return result

########################################################################
# Tau cell variables calculation
def CellVariablesCfg(flags, cellConeSize=0.2, prefix=''):
    result = ComponentAccumulator()
    #if prefix is not given, take global one 
    if not prefix: 
        prefix=sPrefix 
    _name = prefix + 'CellVariables'

    TauCellVariables = CompFactory.getComp("TauCellVariables")
    TauCellVariables = TauCellVariables(name = _name,
                                        StripEthreshold = 0.2*GeV,
                                        CellCone = cellConeSize,
                                        VertexCorrection = True)

    result.setPrivateTools(TauCellVariables)
    return result

#########################################################################
# ele veto variables
def ElectronVetoVarsCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauElectronVetoVars'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg

    TauElectronVetoVariables = CompFactory.getComp("TauElectronVetoVariables")
    TauElectronVetoVariables = TauElectronVetoVariables(name = _name,
                                                        VertexCorrection = True,
                                                        ParticleCaloExtensionTool = result.popToolsAndMerge(ParticleCaloExtensionToolCfg(flags)),
                                                        tauEVParticleCache = getParticleCache(flags))

    result.setPrivateTools(TauElectronVetoVariables)
    return result

#########################################################################
# cell weight tool
def CellWeightToolCfg(flags):
    result = ComponentAccumulator()

    # copied from CaloClusterCorrection/python/StandardCellWeightCalib
    H1WeightToolCSC12Generic = CompFactory.H1WeightToolCSC12Generic  # CaloClusterCorrection
    isMC = flags.Input.isMC 

    finder = "Cone"
    mainparam = 0.4
    inputn = "Topo"
    onlyCellWeight = False
    from CaloClusterCorrection.StandardCellWeightCalib import H1Calibration, editParm
    (key,folder,tag) = H1Calibration.getCalibDBParams(finder,mainparam,inputn, onlyCellWeight, isMC)
    # H1Calibration.loadCaloFolder(result, flags, folder, tag, isMC)
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    if isMC:
        dbString="CALO_OFL"
    else:
        dbString="CALO"
    if (folder,tag) not in H1Calibration.loaded_folder:
        if H1Calibration.overrideFolder():
            result.merge(addFolders(flags, folder+'<tag>'+tag+'</tag>', detDb=dbString, className='CaloRec::ToolConstants') )
        else:
            result.merge(addFolders(flags, folder, detDb=dbString, className='CaloRec::ToolConstants') )

        H1Calibration.loaded_folder.append( (folder,tag) )

    #-- configure tool
    toolName = finder + editParm(mainparam) + inputn
    cellcalibtool = H1WeightToolCSC12Generic("H1Weight"+toolName)
    cellcalibtool.DBHandleKey = key
    result.setPrivateTools(cellcalibtool)
    # --
    return result

    # _name = sPrefix + 'CellWeightTool'

    # once StandardCellWeightCalib is updated can use this
    # from CaloRec.CaloTopoClusterFlags import jobproperties
    # -- auto configure weight tool
    # finder = jobproperties.CaloTopoClusterFlags.cellWeightRefFinder.get_Value()
    # size   = jobproperties.CaloTopoClusterFlags.cellWeightRefSize.get_Value()
    # signal = jobproperties.CaloTopoClusterFlags.cellWeightRefSignal.get_Value()
    # Defaults already set in StandardCellWeightCalib:
    # finder=Cone, size=0.4, signal=Topo

    #from CaloClusterCorrection.StandardCellWeightCalib import getCellWeightTool
    # CaloWeightTool = getCellWeightTool(finder,size,signal)
    #CaloWeightTool = result.popToolsAndMerge(getCellWeightTool(flags))
    
    #result.setPrivateTools(CaloWeightTool)
    #return result

#########################################################################
# Photon Shot Finder
def TauShotFinderCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauShotFinder'

    shotPtCut_1Photon = flags.Tau.shotPtCut_1Photon
    shotPtCut_2Photons = flags.Tau.shotPtCut_2Photons

    
    from CaloClusterCorrection.StandardCellWeightCalib import getCellWeightTool
    CaloWeightTool = getCellWeightTool(flags=flags)


    TauShotFinder = CompFactory.getComp("TauShotFinder")
    TauShotFinder = TauShotFinder(name = _name,
                                  CaloWeightTool = result.popToolsAndMerge(CaloWeightTool),
                                  NCellsInEta           = 5,
                                  MinPtCut              = shotPtCut_1Photon,
                                  AutoDoubleShotCut     = shotPtCut_2Photons,
                                  Key_caloCellInputContainer="AllCalo"
                                  )

    result.setPrivateTools(TauShotFinder)
    return result

#########################################################################
# Pi0 algo
# Cluster finder for Pi0 algo
def Pi0ClusterFinderCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'Pi0ClusterFinder'

    TauPi0CreateROI = CompFactory.getComp("TauPi0CreateROI")
    
    TauPi0CreateROI = TauPi0CreateROI(name = _name,
                                      Key_caloCellInputContainer="AllCalo")

    result.setPrivateTools(TauPi0CreateROI)
    return result

#########################################################################
# Cell finalizer tool for BuildAlg
def TauCellFinalizerCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'tauPi0CellContainerFinalizer'

    CaloCellContainerFinalizerTool = CompFactory.getComp("CaloCellContainerFinalizerTool")
    TauCellContainerFinalizer = CaloCellContainerFinalizerTool(name=_name)

    result.setPrivateTools(TauCellContainerFinalizer)
    return result

#########################################################################
# Calo Tools
#########################################################################

def TauCaloLCClassificationCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'LCClassify'

    CaloLCClassificationTool = CompFactory.getComp("CaloLCClassificationTool")
    LCClassify = CaloLCClassificationTool(_name)
    LCClassify.ClassificationKey   = "EMFracClassify"
    LCClassify.UseSpread = False
    LCClassify.MaxProbability = 0.5
    # add the moments EM_PROBABILITY, HAD_WEIGHT, OOC_WEIGHT, DM_WEIGHT to the AOD:
    LCClassify.StoreClassificationProbabilityInAOD = True
    LCClassify.WeightingOfNegClusters = flags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute
    LCClassify.UseNormalizedEnergyDensity = True

    result.setPrivateTools(LCClassify)
    return result

def TauCaloLCWeightCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'CaloLCWeightTool'

    # Schedule total noise cond alg
    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge(CaloNoiseCondAlgCfg(flags,"totalNoise"))
    # Schedule electronic noise cond alg (needed for LC weights)
    result.merge(CaloNoiseCondAlgCfg(flags,"electronicNoise"))

    CaloLCWeightTool = CompFactory.getComp("CaloLCWeightTool")
    LCWeight = CaloLCWeightTool(_name)
    LCWeight.CorrectionKey       = "H1ClusterCellWeights"
    LCWeight.SignalOverNoiseCut  = 2.0
    LCWeight.UseHadProbability   = True

    result.setPrivateTools(LCWeight)
    return result

def TauCaloLCOutOfClusterCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'CaloLCOutOfClusterTool'

    CaloLCOutOfClusterTool = CompFactory.getComp("CaloLCOutOfClusterTool")
    LCOut = CaloLCOutOfClusterTool(_name)
    LCOut.CorrectionKey       = "OOCCorrection"
    LCOut.UseEmProbability    = False
    LCOut.UseHadProbability   = True

    result.setPrivateTools(LCOut)
    return result

def TauCaloLCOutPi0Cfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'LCOutPi0'

    CaloLCOutOfClusterTool = CompFactory.getComp("CaloLCOutOfClusterTool")
    LCOutPi0 = CaloLCOutOfClusterTool(_name)
    LCOutPi0.CorrectionKey    = "OOCPi0Correction"
    LCOutPi0.UseEmProbability  = True
    LCOutPi0.UseHadProbability = False

    result.setPrivateTools(LCOutPi0)
    return result

def TauCaloLCDeadMaterialCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'CaloLCDeadMaterialTool'

    CaloLCDeadMaterialTool = CompFactory.getComp("CaloLCDeadMaterialTool")
    LCDeadMaterial = CaloLCDeadMaterialTool(_name)
    LCDeadMaterial.HadDMCoeffKey       = "HadDMCoeff2"
    LCDeadMaterial.ClusterRecoStatus   = 0
    LCDeadMaterial.WeightModeDM        = 2
    LCDeadMaterial.UseHadProbability   = True
    LCDeadMaterial.WeightingOfNegClusters = flags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute

    result.setPrivateTools(LCDeadMaterial)
    return result

def TauCaloClusterLocalCalibCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'CaloLocalCalib'

    CaloClusterLocalCalib = CompFactory.getComp("CaloClusterLocalCalib")
    LocalCalib =  CaloClusterLocalCalib(_name)
    LocalCalib.ClusterClassificationTool     = [result.popToolsAndMerge(TauCaloLCClassificationCfg(flags))]
    LocalCalib.ClusterRecoStatus             = [1,2]
    LocalCalib.LocalCalibTools               = [result.popToolsAndMerge(TauCaloLCWeightCfg(flags))]
    LocalCalib.WeightingOfNegClusters = flags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute

    result.setPrivateTools(LocalCalib)
    return result

def TauCaloOOCCalibCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'OOCCalibTool'

    CaloClusterLocalCalib = CompFactory.getComp("CaloClusterLocalCalib")
    OOCCalib   = CaloClusterLocalCalib (_name)
    OOCCalib.ClusterRecoStatus   = [1,2]
    OOCCalib.LocalCalibTools     = [result.popToolsAndMerge(TauCaloLCOutOfClusterCfg(flags))]
    OOCCalib.WeightingOfNegClusters = flags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute

    result.setPrivateTools(OOCCalib)
    return result

def TauCaloDMCalibCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'DMCalib'

    CaloClusterLocalCalib = CompFactory.getComp("CaloClusterLocalCalib")
    DMCalib =  CaloClusterLocalCalib(_name)
    DMCalib.ClusterRecoStatus             = [1,2]
    DMCalib.LocalCalibTools = [result.popToolsAndMerge(TauCaloLCDeadMaterialCfg(flags))]
    DMCalib.WeightingOfNegClusters = flags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute

    result.setPrivateTools(DMCalib)
    return result

def TauCaloOOCPi0CalibCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'OOCPi0CalibTool'

    CaloClusterLocalCalib = CompFactory.getComp("CaloClusterLocalCalib")
    OOCPi0Calib   = CaloClusterLocalCalib (_name)
    OOCPi0Calib.ClusterRecoStatus   = [1,2]
    OOCPi0Calib.LocalCalibTools     = [result.popToolsAndMerge(TauCaloLCOutPi0Cfg(flags))]
    OOCPi0Calib.WeightingOfNegClusters = flags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute

    result.setPrivateTools(OOCPi0Calib)
    return result

def TauCaloClusterCellWeightCalibCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'CellWeights'

    CaloClusterCellWeightCalib = CompFactory.getComp("CaloClusterCellWeightCalib")
    CellWeights = CaloClusterCellWeightCalib(_name)
    CellWeights.CellSignalWeightTool = result.popToolsAndMerge(CellWeightToolCfg(flags))
    CellWeights.Direction = "AbsSignal" #-- use absolute cell energies for eta/phi calculation
    CellWeights.BelowThresholdLikeAll = True #-- treat clusters below thresholds the same as all others
    CellWeights.BelowThresholdDirection = "AbsSignal" #-- alternative direction calculation for below threshold clusters, ignored if BelowThresholdLikeAll = True
    CellWeights.EnergyThreshold = 0.0*MeV #-- threshold for possible change of direction calculation
    CellWeights.IgnoreGeoWeights = False #-- ignore geometrical cell signal weights if True

    result.setPrivateTools(CellWeights)
    return result

def TauCaloTopoClusterMakerCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'Pi0TopoClusterMaker'

    # Schedule total noise cond alg
    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge(CaloNoiseCondAlgCfg(flags,"totalNoise"))
    # Schedule electronic noise cond alg (needed for LC weights)
    result.merge(CaloNoiseCondAlgCfg(flags,"electronicNoise"))

    CaloTopoClusterMaker = CompFactory.getComp("CaloTopoClusterMaker")
    TopoClusterForTaus = CaloTopoClusterMaker(_name)

    TopoClusterForTaus.CellsName = "TauCommonPi0Cells"
    TopoClusterForTaus.CalorimeterNames=["LAREM"]
    TopoClusterForTaus.SeedSamplingNames = ["PreSamplerB", "EMB1", "EMB2", "PreSamplerE", "EME1", "EME2"]
    TopoClusterForTaus.NeighborOption                    = "super3D"
    TopoClusterForTaus.RestrictHECIWandFCalNeighbors     = False
    TopoClusterForTaus.RestrictPSNeighbors               = True
    TopoClusterForTaus.CellThresholdOnEorAbsEinSigma     = 0.0
    TopoClusterForTaus.NeighborThresholdOnEorAbsEinSigma = 2.0
    TopoClusterForTaus.SeedThresholdOnEorAbsEinSigma     = 4.0
    TopoClusterForTaus.SeedCutsInAbsE                    = True
    TopoClusterForTaus.ClusterEtorAbsEtCut               = 0.5*GeV # 0.0*MeV in standard CaloCalTopoCluster JobOptions!
    TopoClusterForTaus.TwoGaussianNoise                  = flags.Calo.TopoCluster.doTwoGaussianNoise

    result.setPrivateTools(TopoClusterForTaus)
    return result

def TauCaloTopoClusterSplitterCfg(flags):

    result = ComponentAccumulator()
    _name = sPrefix + 'Pi0TopoClusterSplitter'

    CaloTopoClusterSplitter = CompFactory.getComp("CaloTopoClusterSplitter")
    TopoSplitterForTaus = CaloTopoClusterSplitter(_name)

    # cells from the following samplings will be able to form local
    # maxima. The excluded samplings are PreSamplerB, EMB1,
    # PreSamplerE, EME1, all Tile samplings, all HEC samplings and the
    # two rear FCal samplings.
    TopoSplitterForTaus.SamplingNames = ["EMB2","EME2"] # Do we want to use EMB3 and EME3?
    # cells from the following samplings will also be able to form
    # local maxima but only if they are not overlapping in eta and phi
    # with local maxima in previous samplings from the primary list.
    TopoSplitterForTaus.SecondarySamplingNames = ["EMB1","EME1"]
    TopoSplitterForTaus.ShareBorderCells = True
    TopoSplitterForTaus.RestrictHECIWandFCalNeighbors  = False
    TopoSplitterForTaus.WeightingOfNegClusters = flags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute

    result.setPrivateTools(TopoSplitterForTaus)
    return result

def TauCaloClusterMomentsMakerCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'Pi0TopoMoments'

    # Schedule total noise cond alg
    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge(CaloNoiseCondAlgCfg(flags,"totalNoise"))
    # Schedule electronic noise cond alg (needed for LC weights)
    result.merge(CaloNoiseCondAlgCfg(flags,"electronicNoise"))

    CaloClusterMomentsMaker = CompFactory.getComp("CaloClusterMomentsMaker")
    TopoMomentsForTaus = CaloClusterMomentsMaker(_name)
    TopoMomentsForTaus.WeightingOfNegClusters = flags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute
    TopoMomentsForTaus.MaxAxisAngle = 20*deg
    TopoMomentsForTaus.TwoGaussianNoise = flags.Calo.TopoCluster.doTwoGaussianNoise
    TopoMomentsForTaus.MinBadLArQuality = 4000
    TopoMomentsForTaus.MomentsNames = ["FIRST_PHI"
                                       ,"FIRST_ETA"
                                       ,"SECOND_R"
                                       ,"SECOND_LAMBDA"
                                       ,"DELTA_PHI"
                                       ,"DELTA_THETA"
                                       ,"DELTA_ALPHA"
                                       ,"CENTER_X"
                                       ,"CENTER_Y"
                                       ,"CENTER_Z"
                                       ,"CENTER_MAG"
                                       ,"CENTER_LAMBDA"
                                       ,"LATERAL"
                                       ,"LONGITUDINAL"
                                       ,"ENG_FRAC_EM"
                                       ,"ENG_FRAC_MAX"
                                       ,"ENG_FRAC_CORE"
                                       ,"FIRST_ENG_DENS"
                                       ,"SECOND_ENG_DENS"
                                       ,"ISOLATION"]

    result.setPrivateTools(TopoMomentsForTaus)
    return result

def TauCaloClusterBadChannelCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'CaloClusterBadChannelList'

    CaloClusterBadChannelListCorr = CompFactory.getComp("CaloClusterBadChannelList")
    BadChannelListCorrForTaus = CaloClusterBadChannelListCorr(_name)

    result.setPrivateTools(BadChannelListCorrForTaus)
    return result

########################################################################
########################################################################
# Tracking Tools
########################################################################

########################################################################
# Tools for Adaptive Vertex Finder
def TauNewtonTrkDistanceFinderCfg(flags):
    result = ComponentAccumulator()

    Trk__SeedNewtonTrkDistanceFinder = CompFactory.Trk.SeedNewtonTrkDistanceFinder
    TauNewtonTrkDistanceFinder = Trk__SeedNewtonTrkDistanceFinder( name = sPrefix+'TauSeedNewtonTrkDistanceFinder')

    result.setPrivateTools(TauNewtonTrkDistanceFinder)
    return result

def CrossDistancesSeedFinderCfg(flags):
    result = ComponentAccumulator()
    _name = 'TauCrossDistancesSeedFinder'
    
    Trk__CrossDistancesSeedFinder = CompFactory.Trk.CrossDistancesSeedFinder
    TauCrossDistancesSeedFinder = Trk__CrossDistancesSeedFinder( name = _name, TrkDistanceFinder=result.popToolsAndMerge(TauNewtonTrkDistanceFinderCfg(flags)) )
    
    result.setPrivateTools(TauCrossDistancesSeedFinder)
    return result
    
def ImpactPoint3dEstimatorCfg(flags):
    result = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    Trk__ImpactPoint3dEstimator = CompFactory.Trk.ImpactPoint3dEstimator
    TauInDetImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name = sPrefix+'TauTrkImpactPoint3dEstimator', Extrapolator = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags)) )

    result.setPrivateTools(TauInDetImpactPoint3dEstimator)
    return result

def SequentialVertexSmootherCfg(flags):
    result = ComponentAccumulator()

    Trk__SequentialVertexSmoother = CompFactory.Trk.SequentialVertexSmoother
    TauSequentialVertexSmoother = Trk__SequentialVertexSmoother(name = sPrefix+'TauSequentialVertexSmoother')

    result.setPrivateTools(TauSequentialVertexSmoother)
    return result

def DetAnnealingMakerCfg(flags):
    result = ComponentAccumulator()

    Trk__DetAnnealingMaker = CompFactory.Trk.DetAnnealingMaker
    TauDetAnnealingMaker = Trk__DetAnnealingMaker(name = sPrefix+'TauDetAnnealingMaker', SetOfTemperatures = [ 64, 32, 16, 8, 4, 2, 1 ] )

    result.setPrivateTools(TauDetAnnealingMaker)
    return result

########################################################################
# TauAdaptiveVertexFitter
def TauAdaptiveVertexFitterCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauAdaptiveVertexFitter'

    Trk__AdaptiveVertexFitter = CompFactory.Trk.AdaptiveVertexFitter
    TauAdaptiveVertexFitter = Trk__AdaptiveVertexFitter(name = _name,
                                                        SeedFinder=result.popToolsAndMerge(CrossDistancesSeedFinderCfg(flags)),
                                                        ImpactPoint3dEstimator=result.popToolsAndMerge(ImpactPoint3dEstimatorCfg(flags)),
                                                        VertexSmoother=result.popToolsAndMerge(SequentialVertexSmootherCfg(flags)),
                                                        AnnealingMaker=result.popToolsAndMerge(DetAnnealingMakerCfg(flags)),
                                                        LinearizedTrackFactory=result.popToolsAndMerge(TauFullLinearizedTrackFactoryCfg(flags)) )

    result.setPrivateTools(TauAdaptiveVertexFitter)
    return result

#####################
# create Pi0 clusters
def Pi0ClusterCreatorCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'Pi0ClusterCreator'

    TauPi0ClusterCreator = CompFactory.getComp("TauPi0ClusterCreator")
    TauPi0ClusterCreator = TauPi0ClusterCreator(name = _name)

    result.setPrivateTools(TauPi0ClusterCreator)
    return result

#####################
# Set energy of cluster to take care of charged pion energy deposited in the ECAL
def Pi0ClusterScalerCfg(flags): 
    result = ComponentAccumulator()
    _name = sPrefix + 'Pi0ClusterScaler'

    TauPi0ClusterScaler = CompFactory.getComp("TauPi0ClusterScaler")
    TauPi0ClusterScaler = TauPi0ClusterScaler(name = _name)

    result.setPrivateTools(TauPi0ClusterScaler)
    return result

#####################
# calculate MVA scores of pi0 clusters
def Pi0ScoreCalculatorCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'Pi0ScoreCalculator'

    TauPi0ScoreCalculator = CompFactory.getComp("TauPi0ScoreCalculator")
    TauPi0ScoreCalculator = TauPi0ScoreCalculator(name = _name,
                                                  BDTWeightFile = flags.Tau.Pi0ScoreConfig)

    result.setPrivateTools(TauPi0ScoreCalculator)
    return result

#####################
# select pi0 clusters
def Pi0SelectorCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'Pi0Selector'

    TauPi0Selector = CompFactory.getComp("TauPi0Selector")
    TauPi0Selector = TauPi0Selector(name = _name,
                                    ClusterEtCut         = flags.Tau.pi0EtCuts,
                                    ClusterBDTCut_1prong = flags.Tau.pi0MVACuts_1prong,
                                    ClusterBDTCut_mprong = flags.Tau.pi0MVACuts_mprong)

    result.setPrivateTools(TauPi0Selector)
    return result

#########################################################################
# Tau Vertex Variables
def TauVertexVariablesCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauVertexVariables'

    TauVertexVariables = CompFactory.getComp("TauVertexVariables")
    TauVertexVariables = TauVertexVariables(  name = _name,
                                              VertexFitter = result.popToolsAndMerge(TauAdaptiveVertexFitterCfg(flags)),
                                              SeedFinder = result.popToolsAndMerge(CrossDistancesSeedFinderCfg(flags)) )

    result.setPrivateTools(TauVertexVariables)
    return result

#########################################################################
# Tau Variables
def TauCommonCalcVarsCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauCommonCalcVars'

    TauCommonCalcVars = CompFactory.getComp("TauCommonCalcVars")
    TauCommonCalcVars = TauCommonCalcVars(name = _name)
    
    result.setPrivateTools(TauCommonCalcVars)
    return result

#########################################################################
# Tau Variables
def TauSubstructureCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauSubstructure'

    TauSubstructureVariables = CompFactory.getComp("TauSubstructureVariables")
    TauSubstructureVariables = TauSubstructureVariables(  name = _name )

    result.setPrivateTools(TauSubstructureVariables)
    return result

########################################################################
# MvaTESVariableDecorator
def MvaTESVariableDecoratorCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'MvaTESVariableDecorator'

    MvaTESVariableDecorator = CompFactory.getComp("MvaTESVariableDecorator")
    MvaTESVariableDecorator = MvaTESVariableDecorator(name = _name,
                                                      Key_vertexInputContainer= flags.Tau.VertexCollection,
                                                      VertexCorrection = True)
    result.setPrivateTools(MvaTESVariableDecorator)
    return result

########################################################################
# MvaTESEvaluator
def MvaTESEvaluatorCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'MvaTESEvaluator'

    MvaTESEvaluator = CompFactory.getComp("MvaTESEvaluator")
    MvaTESEvaluator = MvaTESEvaluator(name = _name,
                                      WeightFileName = flags.Tau.MvaTESConfig,
                                      WeightFileName0p = flags.Tau.MvaTESConfig0p )

    result.setPrivateTools(MvaTESEvaluator)
    return result

def TauIDVarCalculatorCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauIDVarCalculator'

    TauIDVarCalculator = CompFactory.getComp("TauIDVarCalculator")    
    myTauIDVarCalculator = TauIDVarCalculator(name=_name,
                                              VertexCorrection = True)

    result.setPrivateTools(myTauIDVarCalculator)
    return result

def TauJetRNNEvaluatorCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauJetRNN'

    TauJetRNNEvaluator = CompFactory.getComp("TauJetRNNEvaluator")
    RNNConf = flags.Tau.TauJetRNNConfig
    myTauJetRNNEvaluator = TauJetRNNEvaluator(name = _name,
                                              NetworkFile0P = "",
                                              NetworkFile1P = RNNConf[0],
                                              NetworkFile2P = RNNConf[1],
                                              NetworkFile3P = RNNConf[2],
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

    result.setPrivateTools(myTauJetRNNEvaluator)
    return result

def TauWPDecoratorJetRNNCfg(flags):
    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.load_library('libxAODTau_cDict')

    result = ComponentAccumulator()
    _name = sPrefix + 'TauWPDecoratorJetRNN'

    TauWPDecorator = CompFactory.getComp("TauWPDecorator")
    WPConf = flags.Tau.TauJetRNNWPConfig
    myTauWPDecorator = TauWPDecorator( name=_name,
                                       flatteningFile1Prong = WPConf[0],
                                       flatteningFile2Prong = WPConf[1],
                                       flatteningFile3Prong = WPConf[2],
                                       CutEnumVals =
                                       [ ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigVeryLoose,
                                         ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigLoose,
                                         ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigMedium,
                                         ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigTight ],
                                       SigEff1P = [0.95, 0.85, 0.75, 0.60],
                                       SigEff2P = [0.95, 0.75, 0.60, 0.45],
                                       SigEff3P = [0.95, 0.75, 0.60, 0.45],
                                       ScoreName = "RNNJetScore",
                                       NewScoreName = "RNNJetScoreSigTrans",
                                       DefineWPs = True )

    result.setPrivateTools(myTauWPDecorator)
    return result

def TauEleRNNEvaluatorCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauEleRNN' 

    TauJetRNNEvaluator = CompFactory.getComp("TauJetRNNEvaluator")
    RNNConf = flags.Tau.TauEleRNNConfig    
    myTauEleRNNEvaluator = TauJetRNNEvaluator(name = _name,
                                              NetworkFile1P = RNNConf[0],
                                              NetworkFile3P = RNNConf[1],
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

    result.setPrivateTools(myTauEleRNNEvaluator)
    return result

def TauWPDecoratorEleRNNCfg(flags):
    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.load_library('libxAODTau_cDict')

    result = ComponentAccumulator()
    _name = sPrefix + 'TauWPDecoratorEleRNN'

    TauWPDecorator = CompFactory.getComp("TauWPDecorator")
    WPConf = flags.Tau.TauEleRNNWPConfig
    myTauEleWPDecorator = TauWPDecorator( name=_name,
                                       flatteningFile1Prong = WPConf[0],
                                       flatteningFile3Prong = WPConf[1],
                                       CutEnumVals =
                                       [ ROOT.xAOD.TauJetParameters.IsTauFlag.EleRNNLoose,
                                         ROOT.xAOD.TauJetParameters.IsTauFlag.EleRNNMedium,
                                         ROOT.xAOD.TauJetParameters.IsTauFlag.EleRNNTight ],
                                       SigEff1P = [0.95, 0.90, 0.85],
                                       SigEff3P = [0.98, 0.95, 0.90],
                                       UseEleBDT = True ,
                                       ScoreName = "RNNEleScore",
                                       NewScoreName = "RNNEleScoreSigTrans",
                                       DefineWPs = True)

    result.setPrivateTools(myTauEleWPDecorator)
    return result

def TauDecayModeNNClassifierCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauDecayModeNNClassifier'

    TauDecayModeNNClassifier = CompFactory.getComp("TauDecayModeNNClassifier")
    myTauDecayModeNNClassifier = TauDecayModeNNClassifier(name=_name,
                                                          WeightFile=flags.Tau.DecayModeNNClassifierConfig)

    result.setPrivateTools(myTauDecayModeNNClassifier)
    return result

def TauAODSelectorCfg(flags):
    result = ComponentAccumulator()
    _name = sPrefix + 'TauAODSelector'

    TauAODSelector = CompFactory.getComp("TauAODSelector")
    myTauAODSelector = TauAODSelector(name=_name,
                                      Min0pTauPt = flags.Tau.MinPt0p,
                                      MinTauPt = flags.Tau.MinPt)

    result.setPrivateTools(myTauAODSelector)
    return result

########################################################################
# TauCombinedTES
def TauCombinedTESCfg(flags):
    result = ComponentAccumulator()    
    _name = sPrefix + 'TauCombinedTES'

    TauCombinedTES = CompFactory.getComp("TauCombinedTES")
    myTauCombinedTES = TauCombinedTES(name = _name,
                                      WeightFileName = flags.Tau.CombinedTESConfig)

    result.setPrivateTools(myTauCombinedTES)
    return result

