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

    # P->T conversion extra dependencies
    if inputFlags.Detector.GeometryITk:
        PFTrackSelector.ExtraInputs = [
            ("InDetDD::SiDetectorElementCollection", "ConditionStore+ITkPixelDetectorElementCollection"),
            ("InDetDD::SiDetectorElementCollection", "ConditionStore+ITkStripDetectorElementCollection"),
        ]
    else:
        PFTrackSelector.ExtraInputs = [
            ("InDetDD::SiDetectorElementCollection", "ConditionStore+PixelDetectorElementCollection"),
            ("InDetDD::SiDetectorElementCollection", "ConditionStore+SCT_DetectorElementCollection"),
            ("InDetDD::TRT_DetElementContainer", "ConditionStore+TRT_DetElementContainer"),
        ]

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
    PFCellLevelSubtractionToolFactory = CompFactory.PFSubtractionTool
    PFCellLevelSubtractionTool = PFCellLevelSubtractionToolFactory(toolName)

    eflowCellEOverPTool_Run2_mc20_JetETMiss = CompFactory.eflowCellEOverPTool_Run2_mc20_JetETMiss
    PFCellLevelSubtractionTool.eflowCellEOverPTool = eflowCellEOverPTool_Run2_mc20_JetETMiss()

    if(inputFlags.PF.EOverPMode):
        PFCellLevelSubtractionTool.CalcEOverP = True
        PFCellLevelSubtractionTool.nClusterMatchesToUse = -1
    else:
        PFCellLevelSubtractionTool.nClusterMatchesToUse = 1

    if(inputFlags.PF.EOverPMode):
        PFCellLevelSubtractionTool.PFTrackClusterMatchingTool = getPFTrackClusterMatchingTool(inputFlags,0.2,"EtaPhiSquareDistance","PlainEtaPhi","CalObjBldMatchingTool")
    else:
        PFCellLevelSubtractionTool.PFTrackClusterMatchingTool = getPFTrackClusterMatchingTool(inputFlags,1.64,"EtaPhiSquareSignificance","GeomCenterEtaPhi","CalObjBldMatchingTool")

    PFCellLevelSubtractionTool.PFTrackClusterMatchingTool_015 = getPFTrackClusterMatchingTool(inputFlags,0.15,"EtaPhiSquareDistance","PlainEtaPhi","MatchingTool_Pull_015")
    PFCellLevelSubtractionTool.PFTrackClusterMatchingTool_02 = getPFTrackClusterMatchingTool(inputFlags,0.2,"EtaPhiSquareDistance","PlainEtaPhi","MatchingTool_Pull_02")

    return PFCellLevelSubtractionTool

def getPFRecoverSplitShowersTool(inputFlags,toolName):
    PFRecoverSplitShowersToolFactory = CompFactory.PFSubtractionTool
    PFRecoverSplitShowersTool = PFRecoverSplitShowersToolFactory(toolName)

    eflowCellEOverPTool_Run2_mc20_JetETMiss = CompFactory.eflowCellEOverPTool_Run2_mc20_JetETMiss
    PFRecoverSplitShowersTool.eflowCellEOverPTool = eflowCellEOverPTool_Run2_mc20_JetETMiss("eflowCellEOverPTool_Run2_mc20_JetETMiss_Recover")

    PFRecoverSplitShowersTool.RecoverSplitShowers = True

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

def getChargedFlowElementCreatorAlgorithm(inputFlags,chargedFlowElementOutputName,eflowCaloObjectContainerName="eflowCaloObjects"):
    FlowElementChargedCreatorAlgorithmFactory = CompFactory.PFChargedFlowElementCreatorAlgorithm
    FlowElementChargedCreatorAlgorithm = FlowElementChargedCreatorAlgorithmFactory("PFChargedFlowElementCreatorAlgorithm")
    FlowElementChargedCreatorAlgorithm.eflowCaloObjectContainerName = eflowCaloObjectContainerName
    if chargedFlowElementOutputName:
        FlowElementChargedCreatorAlgorithm.FlowElementOutputName=chargedFlowElementOutputName
    if(inputFlags.PF.EOverPMode):
        FlowElementChargedCreatorAlgorithm.FlowElementOutputName="EOverPChargedParticleFlowObjects"

    return FlowElementChargedCreatorAlgorithm

def getNeutralFlowElementCreatorAlgorithm(inputFlags,neutralFlowElementOutputName,eflowCaloObjectContainerName="eflowCaloObjects"):
    FlowElementNeutralCreatorAlgorithmFactory = CompFactory.PFNeutralFlowElementCreatorAlgorithm
    FlowElementNeutralCreatorAlgorithm = FlowElementNeutralCreatorAlgorithmFactory("PFNeutralFlowElementCreatorAlgorithm")
    FlowElementNeutralCreatorAlgorithm.eflowCaloObjectContainerName = eflowCaloObjectContainerName
    if neutralFlowElementOutputName:
        FlowElementNeutralCreatorAlgorithm.FlowElementOutputName=neutralFlowElementOutputName
    if(inputFlags.PF.EOverPMode):
        FlowElementNeutralCreatorAlgorithm.FEOutputName="EOverPNeutralParticleFlowObjects"
    if(inputFlags.PF.useCalibHitTruthClusterMoments and inputFlags.PF.addClusterMoments):
        FlowElementNeutralCreatorAlgorithm.useCalibHitTruth=True

    return FlowElementNeutralCreatorAlgorithm

def getLCNeutralFlowElementCreatorAlgorithm(inputFlags,neutralFlowElementOutputName):
    LCFlowElementNeutralCreatorAlgorithmFactory = CompFactory.PFLCNeutralFlowElementCreatorAlgorithm
    LCFlowElementNeutralCreatorAlgorithm = LCFlowElementNeutralCreatorAlgorithmFactory("PFLCNeutralFlowElementCreatorAlgorithm")
    if neutralFlowElementOutputName:
      LCFlowElementNeutralCreatorAlgorithm.FELCOutputName==neutralFlowElementOutputName
    if(inputFlags.PF.EOverPMode):
      LCFlowElementNeutralCreatorAlgorithm.FEInputContainerName="EOverPNeutralParticleFlowObjects"
      LCFlowElementNeutralCreatorAlgorithm.FELCOutputName="EOverPLCNeutralParticleFlowObjects"
    
    return LCFlowElementNeutralCreatorAlgorithm 

def getEGamFlowElementAssocAlgorithm(inputFlags,neutral_FE_cont_name="",charged_FE_cont_name="",AODTest=False,doTCC=False):

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

    if(doTCC):
        # ReadHandles to change
        PFEGamFlowElementLinkerAlgorithm.JetEtMissNeutralFlowElementContainer="TrackCaloClustersNeutral"
        PFEGamFlowElementLinkerAlgorithm.JetEtMissChargedFlowElementContainer="TrackCaloClustersCharged"
        
        #Now to change the writeHandles
        # first the Electron -> FE links
        EL_NFE_Link=str(PFEGamFlowElementLinkerAlgorithm.ElectronNeutralFEDecorKey)
        PFEGamFlowElementLinkerAlgorithm.ElectronNeutralFEDecorKey=EL_NFE_Link.replace("FELinks","TCCLinks")
        EL_CFE_Link=str(PFEGamFlowElementLinkerAlgorithm.ElectronChargedFEDecorKey)
        PFEGamFlowElementLinkerAlgorithm.ElectronChargedFEDecorKey=EL_CFE_Link.replace("FELinks","TCCLinks")
        #then the converse case (FE -> Electron)
        
        PFEGamFlowElementLinkerAlgorithm.ChargedFEElectronDecorKey="TrackCaloClustersCharged.TCC_ElectronLinks"
        PFEGamFlowElementLinkerAlgorithm.NeutralFEElectronDecorKey="TrackCaloClustersNeutral.TCC_ElectronLinks"
        

        # first the Photon -> FE links
        PH_NFE_Link=str(PFEGamFlowElementLinkerAlgorithm.PhotonNeutralFEDecorKey)
        PFEGamFlowElementLinkerAlgorithm.PhotonNeutralFEDecorKey=PH_NFE_Link.replace("FELinks","TCCLinks")
        PH_CFE_Link=str(PFEGamFlowElementLinkerAlgorithm.PhotonChargedFEDecorKey)
        PFEGamFlowElementLinkerAlgorithm.PhotonChargedFEDecorKey=PH_CFE_Link.replace("FELinks","TCCLinks")
        #then the converse case (FE -> Photons)
        
        PFEGamFlowElementLinkerAlgorithm.ChargedFEPhotonDecorKey="TrackCaloClustersCharged.TCC_PhotonLinks"
        PFEGamFlowElementLinkerAlgorithm.NeutralFEPhotonDecorKey="TrackCaloClustersNeutral.TCC_PhotonLinks"
        
        
        
        
        
    return PFEGamFlowElementLinkerAlgorithm

def getMuonFlowElementAssocAlgorithm(inputFlags,neutral_FE_cont_name="",charged_FE_cont_name="",LinkNeutralFEClusters=True,useMuonTopoClusters=False,AODTest=False,doTCC=False):
    

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

    #Track Calo cluster (TCC) specific configuration. Input is differently named FE container, and in the AOD step specifically
    if(doTCC):
        #input containers are TrackCaloClustersCharged and TrackCaloClustersNeutral, so rename them
        #service_key="StoreGateSvc+"
        service_key=""
        PFMuonFlowElementLinkerAlgorithm.JetEtMissChargedFlowElementContainer=service_key+"TrackCaloClustersCharged"
        PFMuonFlowElementLinkerAlgorithm.JetEtMissNeutralFlowElementContainer=service_key+"TrackCaloClustersNeutral"
        
        #Output
        #rename the FE_MuonLinks as TCC_MuonLinks
        #rename output containers
        PFMuonFlowElementLinkerAlgorithm.MuonContainer_chargedFELinks=service_key+"Muons.chargedTCCLinks"
        PFMuonFlowElementLinkerAlgorithm.MuonContainer_neutralFELinks=service_key+"Muons.neutralTCCLinks"
        PFMuonFlowElementLinkerAlgorithm.JetETMissNeutralFlowElementContainer_FE_MuonLinks=service_key+"TrackCaloClustersNeutral.TCC_MuonLinks"
        PFMuonFlowElementLinkerAlgorithm.JetETMissChargedFlowElements_FE_MuonLinks=service_key+"TrackCaloClustersCharged.TCC_MuonLinks"
        # several variables relating to Neutral Flow Elements/TCCs to Muons for debug. perhaps at some point these should be removed by default 
        PFMuonFlowElementLinkerAlgorithm.FlowElementContainer_nMatchedMuons="TrackCaloClustersNeutral.TCC_nMatchedMuons"
        PFMuonFlowElementLinkerAlgorithm.FlowElementContainer_FE_efrac_matched_muon="TrackCaloClustersNeutral.TCC_efrac_matched_muon"
        
        PFMuonFlowElementLinkerAlgorithm.MuonContainer_muon_efrac_matched_FE="Muons.muon_efrac_matched_TCC"
        # this is because the algorithm adds this debug container which we don't need 
        PFMuonFlowElementLinkerAlgorithm.MuonContainer_ClusterInfo_deltaR="Muons.deltaR_muon_clus_TCCalg"
    if(LinkNeutralFEClusters):
        if (doTCC or AODTest):
            # since the cells are deleted on AOD, if you try to run the link between NFE and Muon on AOD, it will crash. Terminate to catch this.
            # This is a known bug to rectify soon
            from AthenaCommon.Logging import logging
            msg=logging.getLogger("PFCfg.py::getMuonFlowElementAssocAlgorithm")
            msg.error("Neutral FE from AOD configured to be linked with Muon. This link will fail due to missing CaloCells in the AOD")
            msg.info("Terminating job")
            exit(0)
        
        

    return PFMuonFlowElementLinkerAlgorithm



def getTauFlowElementAssocAlgorithm(inputFlags,neutral_FE_cont_name="",charged_FE_cont_name="",AODTest=False,doTCC=False) :

    PFTauFlowElementLinkerAlgorithmFactory=CompFactory.PFTauFlowElementAssoc
    PFTauFlowElementLinkerAlgorithm=PFTauFlowElementLinkerAlgorithmFactory("PFTauFlowElementAssoc")

    #set an an alternate name if needed
    #this uses some gaudi core magic, namely that you can change the name of the handle as it is a callable attribute, despite the attribute not being explicitly listed in the header
    #for a key of type SG::WriteDecorHandle<xAOD::SomeCont>someKey{this,"SpecificContainerName","myContainerName","other-labels"}
    #setting algorithm.SpecificContainerName="myNewContainerName" changes parameter "myContainerName"
    #(also applies to ReadHandles)
    if(neutral_FE_cont_name!=""):
        PFTauFlowElementLinkerAlgorithm.JetETMissNeutralFlowElementContainer=neutral_FE_cont_name

    if(charged_FE_cont_name!=""):
        PFTauFlowElementLinkerAlgorithm.JetETMissChargedFlowElementContainer=charged_FE_cont_name

    if(AODTest):
        TAU_NFE_Link=str(PFTauFlowElementLinkerAlgorithm.TauNeutralFEDecorKey)
        PFTauFlowElementLinkerAlgorithm.TauNeutralFEDecorKey=TAU_NFE_Link.replace("FELinks","CustomFELinks")

        TAU_CFE_Link=str(PFTauFlowElementLinkerAlgorithm.TauChargedFEDecorKey)
        PFTauFlowElementLinkerAlgorithm.TauChargedFEDecorKey=TAU_CFE_Link.replace("FELinks","CustomFELinks")

        NFE_TAU_Link=str(PFTauFlowElementLinkerAlgorithm.NeutralFETauDecorKey)
        PFTauFlowElementLinkerAlgorithm.NeutralFETauDecorKey=NFE_TAU_Link.replace("FE","CustomFE")

        CFE_TAU_Link=str(PFTauFlowElementLinkerAlgorithm.ChargedFETauDecorKey)
        PFTauFlowElementLinkerAlgorithm.ChargedFETauDecorKey=CFE_TAU_Link.replace("FE","CustomFE")

    if(doTCC):
         PFTauFlowElementLinkerAlgorithm.JetETMissNeutralFlowElementContainer="TrackCaloClustersNeutral"
         PFTauFlowElementLinkerAlgorithm.JetETMissChargedFlowElementContainer="TrackCaloClustersCharged"

         PFTauFlowElementLinkerAlgorithm.TauNeutralFEDecorKey="TauJets.neutralTCCLinks"
         PFTauFlowElementLinkerAlgorithm.TauChargedFEDecorKey="TauJets.chargedTCCLinks"
         
         PFTauFlowElementLinkerAlgorithm.NeutralFETauDecorKey="TrackCaloClustersNeutral.TCC_TauLinks"
         PFTauFlowElementLinkerAlgorithm.ChargedFETauDecorKey="TrackCaloClustersCharged.TCC_TauLinks"

    return PFTauFlowElementLinkerAlgorithm

def getOfflinePFAlgorithm(inputFlags):
    result=ComponentAccumulator()

    PFAlgorithm=CompFactory.PFAlgorithm
    PFAlgorithm = PFAlgorithm("PFAlgorithm")
    
    topoClustersName="CaloTopoClusters"

    PFAlgorithm.PFClusterSelectorTool = getPFClusterSelectorTool(topoClustersName,"CaloCalTopoClusters","PFClusterSelectorTool")    
    
    PFAlgorithm.SubtractionToolList = [getPFCellLevelSubtractionTool(inputFlags,"PFCellLevelSubtractionTool")]

    if(False is inputFlags.PF.EOverPMode):
        PFAlgorithm.SubtractionToolList += [getPFRecoverSplitShowersTool(inputFlags,"PFRecoverSplitShowersTool")]

    PFMomentCalculatorTools=result.popToolsAndMerge(getPFMomentCalculatorTool(inputFlags,[]))
    PFAlgorithm.BaseToolList = [PFMomentCalculatorTools]
    PFAlgorithm.BaseToolList += [getPFLCCalibTool(inputFlags)]
    result.addEventAlgo(PFAlgorithm)
    return result

def PFTauFlowElementLinkingCfg(inputFlags,neutral_FE_cont_name="",charged_FE_cont_name="",AODTest=False):
    result=ComponentAccumulator()
    
    result.addEventAlgo(getTauFlowElementAssocAlgorithm(inputFlags,neutral_FE_cont_name,charged_FE_cont_name,AODTest))

    # the following is needed to reliably determine whether we're really being steered from an old-style job option
    # assume we're running CPython
    #Snippet provided by Carlo Varni
    import inspect
    stack = inspect.stack()
    if len(stack) >= 2 and stack[1].function == 'CAtoGlobalWrapper':
      for el in result._allSequences:
        el.name = "TopAlg"

    return result



