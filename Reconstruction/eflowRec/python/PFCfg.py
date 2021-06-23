# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

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

    result.addEventAlgo (PFTrackSelector, primary=True)

    return result

def getPFClusterSelectorTool(clustersin,calclustersin,algName):

    PFClusterSelectorToolFactory = CompFactory.PFClusterSelectorTool
    PFClusterSelectorTool = PFClusterSelectorToolFactory(algName)
    if clustersin is not None:
        PFClusterSelectorTool.clustersName = clustersin
    if calclustersin is not None:
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
    result=ComponentAccumulator()
    PFMomentCalculatorToolFactory = CompFactory.PFMomentCalculatorTool
    PFMomentCalculatorTool = PFMomentCalculatorToolFactory("PFMomentCalculatorTool")

    from CaloRec.CaloTopoClusterConfig import getTopoMoments
    PFClusterMomentsMaker = result.popToolsAndMerge(getTopoMoments(inputFlags))
    if (len(momentsToCalculateList) > 0):
        PFClusterMomentsMaker.MomentsNames = momentsToCalculateList
    PFMomentCalculatorTool.CaloClusterMomentsMaker = PFClusterMomentsMaker

    PFClusterCollectionTool = CompFactory.PFClusterCollectionTool
    PFMomentCalculatorTool.PFClusterCollectionTool = PFClusterCollectionTool("PFClusterCollectionTool")

    if(inputFlags.PF.useCalibHitTruthClusterMoments):
        PFMomentCalculatorTool.UseCalibHitTruth=True
        from CaloRec.CaloTopoClusterConfig import getTopoCalibMoments
        PFMomentCalculatorTool.CaloCalibClusterMomentsMaker2 = getTopoCalibMoments(inputFlags)

    result.setPrivateTools(PFMomentCalculatorTool)
    return result

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

def getChargedPFOCreatorAlgorithm(inputFlags,chargedPFOOutputName,eflowObjectsInputName=None):
    PFOChargedCreatorAlgorithmFactory = CompFactory.PFOChargedCreatorAlgorithm
    PFOChargedCreatorAlgorithm = PFOChargedCreatorAlgorithmFactory("PFOChargedCreatorAlgorithm")
    if chargedPFOOutputName:
        PFOChargedCreatorAlgorithm.PFOOutputName = chargedPFOOutputName
    if(inputFlags.PF.EOverPMode):
        PFOChargedCreatorAlgorithm.PFOOutputName="EOverPChargedParticleFlowObjects"
    if eflowObjectsInputName is not None:
        PFOChargedCreatorAlgorithm.eflowCaloObjectContainerName = eflowObjectsInputName

    return PFOChargedCreatorAlgorithm

def getNeutralPFOCreatorAlgorithm(inputFlags,neutralPFOOutputName,eflowObjectsInputName=None):
    PFONeutralCreatorAlgorithmFactory = CompFactory.PFONeutralCreatorAlgorithm
    PFONeutralCreatorAlgorithm =  PFONeutralCreatorAlgorithmFactory("PFONeutralCreatorAlgorithm")
    if neutralPFOOutputName:
        PFONeutralCreatorAlgorithm.PFOOutputName = neutralPFOOutputName
    if(inputFlags.PF.EOverPMode):
        PFONeutralCreatorAlgorithm.PFOOutputName="EOverPNeutralParticleFlowObjects"
    if(inputFlags.PF.useCalibHitTruthClusterMoments and inputFlags.PF.addClusterMoments):
        PFONeutralCreatorAlgorithm.UseCalibHitTruth=True
    if eflowObjectsInputName is not None:
        PFONeutralCreatorAlgorithm.eflowCaloObjectContainerName = eflowObjectsInputName    

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

def getEGamFlowElementAssocAlgorithm(inputFlags,neutral_FE_cont_name="",charged_FE_cont_name="",AODTest=False):
    
    PFEGamFlowElementLinkerAlgorithmFactory=CompFactory.PFEGamFlowElementAssoc
    PFEGamFlowElementLinkerAlgorithm=PFEGamFlowElementLinkerAlgorithmFactory("PFEGamFlowElementAssoc")

    #set an an alternate name if needed
    #this uses some gaudi core magic, namely that you can change the name of the handle as it is a callable attribute, despite the attribute not being explicitly listed in the header
    #for a key of type SG::WriteDecorHandle<xAOD::SomeCont>someKey{this,"SpecificContainerName","myContainerName","other-labels"}
    #setting algorithm.SpecificContainerName="myNewContainerName" changes parameter "myContainerName"
    #(also applies to ReadHandles)
    if(neutral_FE_cont_name!=""):
        PFEGamFlowElementLinkerAlgorithm.JetEtMissNeutralFlowElementContainer=neutral_FE_cont_name
        
    if(charged_FE_cont_name!=""):
        PFEGamFlowElementLinkerAlgorithm.JetEtMissChargedFlowElementContainer=charged_FE_cont_name

    # as per muon block, if run on AOD as a test (where the links are already defined), need to rename the output containers to be something new
    if(AODTest):
        # do electron container renaming first
        EL_NFE_Link=str(PFEGamFlowElementLinkerAlgorithm.ElectronNeutralFEDecorKey)

        PFEGamFlowElementLinkerAlgorithm.ElectronNeutralFEDecorKey=EL_NFE_Link.replace("FELinks","CustomFELinks")

        EL_CFE_Link=str(PFEGamFlowElementLinkerAlgorithm.ElectronChargedFEDecorKey)
        PFEGamFlowElementLinkerAlgorithm.ElectronChargedFEDecorKey=EL_CFE_Link.replace("FELinks","CustomFELinks")

        
        NFE_EL_Link=str(PFEGamFlowElementLinkerAlgorithm.NeutralFEElectronDecorKey)
        PFEGamFlowElementLinkerAlgorithm.NeutralFEElectronDecorKey=NFE_EL_Link.replace("FE","CustomFE")

        CFE_EL_Link=str(PFEGamFlowElementLinkerAlgorithm.ChargedFEElectronDecorKey)
        PFEGamFlowElementLinkerAlgorithm.ChargedFEElectronDecorKey=CFE_EL_Link.replace("FE","CustomFE")


        # now do same with photons
        PH_NFE_Link=str(PFEGamFlowElementLinkerAlgorithm.PhotonNeutralFEDecorKey)
        PFEGamFlowElementLinkerAlgorithm.PhotonNeutralFEDecorKey=PH_NFE_Link.replace("FELinks","CustomFELinks")
        PH_CFE_Link=str(PFEGamFlowElementLinkerAlgorithm.PhotonChargedFEDecorKey)
        PFEGamFlowElementLinkerAlgorithm.PhotonChargedFEDecorKey=PH_CFE_Link.replace("FELinks","CustomFELinks")

        NFE_PH_Link=str(PFEGamFlowElementLinkerAlgorithm.NeutralFEPhotonDecorKey)
        PFEGamFlowElementLinkerAlgorithm.NeutralFEPhotonDecorKey=NFE_PH_Link.replace("FE","CustomFE")

        CFE_PH_Link=str(PFEGamFlowElementLinkerAlgorithm.ChargedFEPhotonDecorKey)
        PFEGamFlowElementLinkerAlgorithm.ChargedFEPhotonDecorKey=CFE_PH_Link.replace("FE","CustomFE")


        
    return PFEGamFlowElementLinkerAlgorithm

def getMuonFlowElementAssocAlgorithm(inputFlags,neutral_FE_cont_name="",charged_FE_cont_name="",LinkNeutralFEClusters=True,useMuonTopoClusters=False,AODTest=False):
    
    PFMuonFlowElementLinkerAlgorithmFactory=CompFactory.PFMuonFlowElementAssoc
    PFMuonFlowElementLinkerAlgorithm=PFMuonFlowElementLinkerAlgorithmFactory("PFMuonFlowElementAssoc")

    #set an an alternate name if needed
    #this uses some gaudi core magic, namely that you can change the name of the handle as it is a callable attribute, despite the attribute not being explicitly listed in the header as such
    #for a key of type SG::WriteDecorHandle<xAOD::SomeCont>someKey{this,"SpecificContainerName","myContainerName","other-labels"}
    #setting algorithm.SpecificContainerName="myNewContainerName" changes parameter "myContainerName" to "myNewContainerName"
    if(neutral_FE_cont_name!=""):
        #update the readhandle
        PFMuonFlowElementLinkerAlgorithm.JetEtMissNeutralFlowElementContainer=neutral_FE_cont_name
        #update the write handle for the link
        
    if(charged_FE_cont_name!=""):
        PFMuonFlowElementLinkerAlgorithm.JetEtMissChargedFlowElementContainer=charged_FE_cont_name

    
    PFMuonFlowElementLinkerAlgorithm.m_LinkNeutralFEClusters=LinkNeutralFEClusters
    PFMuonFlowElementLinkerAlgorithm.m_UseMuonTopoClusters=useMuonTopoClusters

    #prototype on AOD with the linkers already defined - so need to rename the output links to something besides their default name.

    if(AODTest):
        # use same Gaudi trick to rename the container input name.         
        MuonFELink=str(PFMuonFlowElementLinkerAlgorithm.MuonContainer_chargedFELinks)
        PFMuonFlowElementLinkerAlgorithm.MuonContainer_chargedFELinks=MuonFELink.replace("FELinks","CustomFELinks")
        
        PFMuonFlowElementLinkerAlgorithm.MuonContainer_neutralFELinks=str(PFMuonFlowElementLinkerAlgorithm.MuonContainer_neutralFELinks).replace("FELinks","CustomFELinks")
                                                                                                                        
        PFMuonFlowElementLinkerAlgorithm.JetETMissNeutralFlowElementContainer_FE_MuonLinks=str(PFMuonFlowElementLinkerAlgorithm.JetETMissNeutralFlowElementContainer_FE_MuonLinks).replace("MuonLinks","CustomMuonLinks")
        PFMuonFlowElementLinkerAlgorithm.JetETMissChargedFlowElements_FE_MuonLinks=str(PFMuonFlowElementLinkerAlgorithm.JetETMissChargedFlowElements_FE_MuonLinks).replace("MuonLinks","CustomMuonLinks")
        
        # and because debug info is also written, need to rename each handle for this too.... - might want to change this in future/re-work it so that a rename -> no debug info saved by default.
        PFMuonFlowElementLinkerAlgorithm.FlowElementContainer_FE_efrac_matched_muon=str(PFMuonFlowElementLinkerAlgorithm.FlowElementContainer_FE_efrac_matched_muon).replace("FE","CustomFE")
        PFMuonFlowElementLinkerAlgorithm.MuonContainer_muon_efrac_matched_FE=str(PFMuonFlowElementLinkerAlgorithm.MuonContainer_muon_efrac_matched_FE).replace("FE","CustomFE")
        PFMuonFlowElementLinkerAlgorithm.FlowElementContainer_nMatchedMuons=str(PFMuonFlowElementLinkerAlgorithm.FlowElementContainer_nMatchedMuons).replace("FE","CustomFE")
        
        
        
        
    return PFMuonFlowElementLinkerAlgorithm
