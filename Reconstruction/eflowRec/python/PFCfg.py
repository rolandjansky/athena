# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory

def PFTrackSelectorAlgCfg(inputFlags,algName,useCaching=True):
    PFTrackSelectorFactory=CompFactory.PFTrackSelector
    PFTrackSelector=PFTrackSelectorFactory(algName)

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg    
    Trk__ParticleCaloExtensionToolFactory=CompFactory.Trk.ParticleCaloExtensionTool
    result = AtlasExtrapolatorCfg(inputFlags)
    pcExtensionTool = Trk__ParticleCaloExtensionToolFactory(Extrapolator = result.popPrivateTools())

    eflowTrackCaloExtensionTool=CompFactory.eflowTrackCaloExtensionTool
    TrackCaloExtensionTool=eflowTrackCaloExtensionTool(TrackCaloExtensionTool=pcExtensionTool)
    if (not useCaching):
      TrackCaloExtensionTool.PFParticleCache = ""

    PFTrackSelector.trackExtrapolatorTool = TrackCaloExtensionTool

    InDet__InDetTrackSelectionToolFactory=CompFactory.InDet.InDetTrackSelectionTool
    TrackSelectionTool = InDet__InDetTrackSelectionToolFactory("PFTrackSelectionTool")

    TrackSelectionTool.CutLevel = "TightPrimary"
    TrackSelectionTool.minPt = 500.0 
    
    PFTrackSelector.trackSelectionTool = TrackSelectionTool

    result.addEventAlgo (PFTrackSelector)

    return result

def getPFClusterSelectorTool(clustersin,calclustersin,algName):

    PFClusterSelectorToolFactory = CompFactory.PFClusterSelectorTool
    PFClusterSelectorTool = PFClusterSelectorToolFactory(algName)
    if clustersin:
        PFClusterSelectorTool.clustersName = clustersin
    if calclustersin:
        PFClusterSelectorTool.calClustersName = calclustersin

    return PFClusterSelectorTool

def getPFTrackClusterMatchingTool(inputFlags,matchCut,distanceType,clusterPositionType,name):
    PFTrackClusterMatchingTool = CompFactory.PFTrackClusterMatchingTool
    MatchingTool = PFTrackClusterMatchingTool(name)
    MatchingTool.ClusterPositionType = clusterPositionType
    MatchingTool.DistanceType = distanceType
    MatchingTool.MatchCut = matchCut*matchCut
    return MatchingTool


def getPFCellLevelSubtractionTool(inputFlags,toolName):
    PFCellLevelSubtractionToolFactory = CompFactory.PFCellLevelSubtractionTool
    PFCellLevelSubtractionTool = PFCellLevelSubtractionToolFactory(toolName)
    
    eflowCellEOverPTool_mc12_JetETMiss = CompFactory.eflowCellEOverPTool_mc12_JetETMiss
    PFCellLevelSubtractionTool.eflowCellEOverPTool = eflowCellEOverPTool_mc12_JetETMiss()

    if(inputFlags.PF.EOverPMode):
        PFCellLevelSubtractionTool.CalcEOverP = True
        PFCellLevelSubtractionTool.nMatchesInCellLevelSubtraction = -1
    else:
        PFCellLevelSubtractionTool.nMatchesInCellLevelSubtraction = 1

    if(inputFlags.PF.EOverPMode):
        PFCellLevelSubtractionTool.PFTrackClusterMatchingTool = getPFTrackClusterMatchingTool(inputFlags,0.2,"EtaPhiSquareDistance","PlainEtaPhi","CalObjBldMatchingTool")
    else:
        PFCellLevelSubtractionTool.PFTrackClusterMatchingTool = getPFTrackClusterMatchingTool(inputFlags,1.64,"EtaPhiSquareSignificance","GeomCenterEtaPhi","CalObjBldMatchingTool")

    PFCellLevelSubtractionTool.PFTrackClusterMatchingTool_02 = getPFTrackClusterMatchingTool(inputFlags,0.2,"EtaPhiSquareDistance","PlainEtaPhi","MatchingTool_Pull_02")            
    
    return PFCellLevelSubtractionTool

def getPFRecoverSplitShowersTool(inputFlags,toolName):
    PFRecoverSplitShowersToolFactory = CompFactory.PFRecoverSplitShowersTool
    PFRecoverSplitShowersTool = PFRecoverSplitShowersToolFactory(toolName)

    eflowCellEOverPTool_mc12_JetETMiss = CompFactory.eflowCellEOverPTool_mc12_JetETMiss
    PFRecoverSplitShowersTool.eflowCellEOverPTool = eflowCellEOverPTool_mc12_JetETMiss("eflowCellEOverPTool_mc12_JetETMiss_Recover")

    PFRecoverSplitShowersTool.RecoverIsolatedTracks = inputFlags.PF.recoverIsolatedTracks

    PFRecoverSplitShowersTool.useUpdated2015ChargedShowerSubtraction = inputFlags.PF.useUpdated2015ChargedShowerSubtraction

    return PFRecoverSplitShowersTool

def getPFMomentCalculatorTool(inputFlags, momentsToCalculateList):
    PFMomentCalculatorToolFactory = CompFactory.PFMomentCalculatorTool
    PFMomentCalculatorTool = PFMomentCalculatorToolFactory("PFMomentCalculatorTool")

    from CaloRec.CaloTopoClusterConfig import getTopoMoments
    PFClusterMomentsMaker = getTopoMoments(inputFlags)
    if (len(momentsToCalculateList) > 0):
        PFClusterMomentsMaker.MomentsNames = momentsToCalculateList
    PFMomentCalculatorTool.CaloClusterMomentsMaker = PFClusterMomentsMaker

    PFClusterCollectionTool = CompFactory.PFClusterCollectionTool
    PFMomentCalculatorTool.PFClusterCollectionTool = PFClusterCollectionTool("PFClusterCollectionTool")

    if(inputFlags.PF.useCalibHitTruthClusterMoments):
        PFMomentCalculatorTool.UseCalibHitTruth=True
        from CaloRec.CaloTopoClusterConfig import getTopoCalibMoments
        PFMomentCalculatorTool.CaloCalibClusterMomentsMaker2 = getTopoCalibMoments(inputFlags)

    return PFMomentCalculatorTool

def getPFLCCalibTool(inputFlags):
    PFLCCalibTool = CompFactory.PFLCCalibTool
    PFLCCalibTool = PFLCCalibTool("PFLCCalibTool")

    PFClusterCollectionTool = CompFactory.PFClusterCollectionTool
    PFLCCalibTool.eflowRecClusterCollectionTool = PFClusterCollectionTool("PFClusterCollectionTool_LCCalib")
    PFLCCalibTool.UseLocalWeight = False

    from CaloRec.CaloTopoClusterConfig import getTopoClusterLocalCalibTools    
    lcCalibToolList = getTopoClusterLocalCalibTools(inputFlags)

    PFLCCalibTool.CaloClusterLocalCalib=lcCalibToolList[0]
    PFLCCalibTool.CaloClusterLocalCalibOOCC=lcCalibToolList[1]                                                            
    PFLCCalibTool.CaloClusterLocalCalibOOCCPi0=lcCalibToolList[2]
    PFLCCalibTool.CaloClusterLocalCalibDM=lcCalibToolList[3]

    return PFLCCalibTool

def getChargedPFOCreatorAlgorithm(inputFlags,chargedPFOOutputName):
    PFOChargedCreatorAlgorithmFactory = CompFactory.PFOChargedCreatorAlgorithm
    PFOChargedCreatorAlgorithm = PFOChargedCreatorAlgorithmFactory("PFOChargedCreatorAlgorithm")
    if chargedPFOOutputName:
        PFOChargedCreatorAlgorithm.PFOOutputName = chargedPFOOutputName
    if(inputFlags.PF.EOverPMode):
        PFOChargedCreatorAlgorithm.PFOOutputName="EOverPChargedParticleFlowObjects"

    return PFOChargedCreatorAlgorithm

def getNeutralPFOCreatorAlgorithm(inputFlags,neutralPFOOutputName):
    PFONeutralCreatorAlgorithmFactory = CompFactory.PFONeutralCreatorAlgorithm
    PFONeutralCreatorAlgorithm =  PFONeutralCreatorAlgorithmFactory("PFONeutralCreatorAlgorithm")
    if neutralPFOOutputName:
        PFONeutralCreatorAlgorithm.PFOOutputName = neutralPFOOutputName
    if(inputFlags.PF.EOverPMode):
        PFONeutralCreatorAlgorithm.PFOOutputName="EOverPNeutralParticleFlowObjects"
    if(inputFlags.PF.useCalibHitTruthClusterMoments and inputFlags.PF.addClusterMoments):
        PFONeutralCreatorAlgorithm.UseCalibHitTruth=True

    PFONeutralCreatorAlgorithm.DoClusterMoments=inputFlags.PF.addClusterMoments
        
    return PFONeutralCreatorAlgorithm

def getChargedFlowElementCreatorAlgorithm(inputFlags,chargedFlowElementOutputName):    
    FlowElementChargedCreatorAlgorithmFactory = CompFactory.PFChargedFlowElementCreatorAlgorithm
    FlowElementChargedCreatorAlgorithm = FlowElementChargedCreatorAlgorithmFactory("PFChargedFlowElementCreatorAlgorithm")
    if chargedFlowElementOutputName:
        FlowElementChargedCreatorAlgorithm.FlowElementOutputName=chargedFlowElementOutputName
    if(inputFlags.PF.EOverPMode):
        FlowElementChargedCreatorAlgorithm.FlowElementOutputName="EOverPChargedFlowElements"

    return FlowElementChargedCreatorAlgorithm

def getNeutralFlowElementCreatorAlgorithm(inputFlags,neutralFlowElementOutputName):
    FlowElementNeutralCreatorAlgorithmFactory = CompFactory.PFNeutralFlowElementCreatorAlgorithm
    FlowElementNeutralCreatorAlgorithm = FlowElementNeutralCreatorAlgorithmFactory("PFNeutralFlowElementCreatorAlgorithm")
    if neutralFlowElementOutputName:
        FlowElementNeutralCreatorAlgorithm.FlowElementOutputName=neutralFlowElementOutputName
    if(inputFlags.PF.EOverPMode):
        FlowElementNeutralCreatorAlgorithm.FlowElementOutputName="EOverPNeutralFlowElements"
    if(inputFlags.PF.useCalibHitTruthClusterMoments and inputFlags.PF.addClusterMoments):
        FlowElementNeutralCreatorAlgorithm.useCalibHitTruth=True


    return FlowElementNeutralCreatorAlgorithm
            
