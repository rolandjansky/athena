# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory

def getPFTrackSelectorAlgorithm(inputFlags,algName,useCaching=True):
    PFTrackSelector=CompFactory.PFTrackSelector
    PFTrackSelector=PFTrackSelector(algName)

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg    
    Trk__ParticleCaloExtensionTool=CompFactory.Trk.ParticleCaloExtensionTool
    extrapCfg = AtlasExtrapolatorCfg(inputFlags)
    pcExtensionTool = Trk__ParticleCaloExtensionTool(Extrapolator = extrapCfg.popPrivateTools())

    eflowTrackCaloExtensionTool=CompFactory.eflowTrackCaloExtensionTool
    TrackCaloExtensionTool=eflowTrackCaloExtensionTool(TrackCaloExtensionTool=pcExtensionTool)
    if False is useCaching:
      TrackCaloExtensionTool.PFParticleCache = ""

    PFTrackSelector.trackExtrapolatorTool = TrackCaloExtensionTool

    InDet__InDetTrackSelectionTool=CompFactory.InDet.InDetTrackSelectionTool
    TrackSelectionTool = InDet__InDetTrackSelectionTool("PFTrackSelectionTool")

    TrackSelectionTool.CutLevel = "TightPrimary"
    TrackSelectionTool.minPt = 500.0 
    
    PFTrackSelector.trackSelectionTool = TrackSelectionTool

    return PFTrackSelector

def getPFClusterSelectorTool(clustersin,calclustersin,algName):

    PFClusterSelectorTool = CompFactory.PFClusterSelectorTool
    PFClusterSelectorTool = PFClusterSelectorTool(algName)
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
    PFCellLevelSubtractionTool = CompFactory.PFCellLevelSubtractionTool
    PFCellLevelSubtractionTool = PFCellLevelSubtractionTool(toolName)
    
    eflowCellEOverPTool_mc12_JetETMiss = CompFactory.eflowCellEOverPTool_mc12_JetETMiss
    PFCellLevelSubtractionTool.eflowCellEOverPTool = eflowCellEOverPTool_mc12_JetETMiss()

    if(True is inputFlags.PF.EOverPMode):
        PFCellLevelSubtractionTool.CalcEOverP = True
        PFCellLevelSubtractionTool.nMatchesInCellLevelSubtraction = -1
    else:
        PFCellLevelSubtractionTool.nMatchesInCellLevelSubtraction = 1

    if(True is inputFlags.PF.EOverPMode):
        PFCellLevelSubtractionTool.PFTrackClusterMatchingTool = getPFTrackClusterMatchingTool(inputFlags,0.2,"EtaPhiSquareDistance","PlainEtaPhi","CalObjBldMatchingTool")
    else:
        PFCellLevelSubtractionTool.PFTrackClusterMatchingTool = getPFTrackClusterMatchingTool(inputFlags,1.64,"EtaPhiSquareSignificance","GeomCenterEtaPhi","CalObjBldMatchingTool")

    PFCellLevelSubtractionTool.PFTrackClusterMatchingTool_02 = getPFTrackClusterMatchingTool(inputFlags,0.2,"EtaPhiSquareDistance","PlainEtaPhi","MatchingTool_Pull_02")            
    
    return PFCellLevelSubtractionTool

def getPFRecoverSplitShowersTool(inputFlags,toolName):
    PFRecoverSplitShowersTool = CompFactory.PFRecoverSplitShowersTool
    PFRecoverSplitShowersTool = PFRecoverSplitShowersTool(toolName)

    eflowCellEOverPTool_mc12_JetETMiss = CompFactory.eflowCellEOverPTool_mc12_JetETMiss
    PFRecoverSplitShowersTool.eflowCellEOverPTool = eflowCellEOverPTool_mc12_JetETMiss("eflowCellEOverPTool_mc12_JetETMiss_Recover")

    PFRecoverSplitShowersTool.RecoverIsolatedTracks = inputFlags.PF.recoverIsolatedTracks

    PFRecoverSplitShowersTool.useUpdated2015ChargedShowerSubtraction = inputFlags.PF.useUpdated2015ChargedShowerSubtraction

    return PFRecoverSplitShowersTool

def getPFMomentCalculatorTool(inputFlags, momentsToCalculateList):
    PFMomentCalculatorTool = CompFactory.PFMomentCalculatorTool
    PFMomentCalculatorTool = PFMomentCalculatorTool("PFMomentCalculatorTool")

    from CaloRec.CaloTopoClusterConfig import getTopoMoments
    PFClusterMomentsMaker = getTopoMoments(inputFlags)
    if (len(momentsToCalculateList) > 0):
        PFClusterMomentsMaker.MomentsNames = momentsToCalculateList
    PFMomentCalculatorTool.CaloClusterMomentsMaker = PFClusterMomentsMaker

    PFClusterCollectionTool = CompFactory.PFClusterCollectionTool
    PFMomentCalculatorTool.PFClusterCollectionTool = PFClusterCollectionTool("PFClusterCollectionTool")

    if(True is inputFlags.PF.useCalibHitTruthClusterMoments):
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
    PFOChargedCreatorAlgorithm = CompFactory.PFOChargedCreatorAlgorithm
    PFOChargedCreatorAlgorithm = PFOChargedCreatorAlgorithm("PFOChargedCreatorAlgorithm")
    if chargedPFOOutputName:
        PFOChargedCreatorAlgorithm.PFOOutputName = chargedPFOOutputName
    if(True is inputFlags.PF.EOverPMode):
        PFOChargedCreatorAlgorithm.PFOOutputName="EOverPChargedParticleFlowObjects"

    return PFOChargedCreatorAlgorithm

def getNeutralPFOCreatorAlgorithm(inputFlags,neutralPFOOutputName):

    PFONeutralCreatorAlgorithm = CompFactory.PFONeutralCreatorAlgorithm
    PFONeutralCreatorAlgorithm =  PFONeutralCreatorAlgorithm("PFONeutralCreatorAlgorithm")
    if neutralPFOOutputName:
        PFONeutralCreatorAlgorithm.PFOOutputName = neutralPFOOutputName
    if(True is inputFlags.PF.EOverPMode):
        PFONeutralCreatorAlgorithm.PFOOutputName="EOverPNeutralParticleFlowObjects"
    if(True is inputFlags.PF.useCalibHitTruthClusterMoments and True is inputFlags.PF.addClusterMoments):
        PFONeutralCreatorAlgorithm.UseCalibHitTruth=True

    PFONeutralCreatorAlgorithm.DoClusterMoments=inputFlags.PF.addClusterMoments
        
    return PFONeutralCreatorAlgorithm
