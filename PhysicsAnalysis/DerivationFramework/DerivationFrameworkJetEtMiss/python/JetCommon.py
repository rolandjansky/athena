# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# Schedules all tools needed for jet/MET analyses
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
from AthenaCommon import CfgMgr
from AthenaCommon import Logging
dfjetlog = Logging.logging.getLogger('JetCommon')

##################################################################
# Schedule the augmentation of a flag to label events with large
# EMEC-IW Noise based on the presence of many bad quality clusters
##################################################################

def addBadBatmanFlag(sequence=DerivationFrameworkJob):

    from RecExConfig.ObjKeyStore import objKeyStore

    if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
        dfjetlog.warning('Running over EVNT files, BadBatmanAugmentation will not be scheduled')
        return

    if not objKeyStore.isInInput( "xAOD::CaloClusterContainer", "CaloCalTopoClusters" ):
        dfjetlog.warning('CaloCalTopoClusters not present. Could not schedule BadBatmanAugmentation!!!')

    if hasattr(sequence,"BadBatmanAugmentation"):
        dfjetlog.warning( "BadBatmanAugmentation: BadBatmanAugmentation already scheduled on sequence "+sequence.name )
        return

    batmanaugtool = CfgMgr.DerivationFramework__BadBatmanAugmentationTool("BadBatmanAugmentationTool")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += batmanaugtool

    batmanaug = CfgMgr.DerivationFramework__CommonAugmentation("BadBatmanAugmentation",
                                                               AugmentationTools = [batmanaugtool])
    sequence += batmanaug

################################################################## 
# Schedule adding the BCID info
################################################################## 

def addDistanceInTrain(sequence=DerivationFrameworkJob):

    if hasattr(sequence,"DistanceInTrainAugmentation"):
        dfjetlog.warning( "DistanceInTrainAugmentation: DistanceInTrainAugmentation already scheduled on sequence"+sequence.name )
        return

    from LumiBlockComps.BunchCrossingCondAlgDefault import BunchCrossingCondAlgDefault
    BunchCrossingCondAlgDefault()

    distanceintrainaugtool = CfgMgr.DerivationFramework__DistanceInTrainAugmentationTool("DistanceInTrainAugmentationTool")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += distanceintrainaugtool
    distanceintrainaug = CfgMgr.DerivationFramework__CommonAugmentation("DistanceInTrainAugmentation",
                                                                        AugmentationTools = [distanceintrainaugtool])
    sequence += distanceintrainaug

##################################################################
# Overlap removal tool (needed for cleaning flags)
##################################################################

def applyOverlapRemoval(sequence=DerivationFrameworkJob):

    from  DerivationFrameworkTau.TauCommon import AddTauAugmentation
    AddTauAugmentation(sequence,doLoose=True)

    from AssociationUtils.config import recommended_tools
    from AssociationUtils.AssociationUtilsConf import OverlapRemovalGenUseAlg
    outputLabel = 'DFCommonJets_passOR'
    bJetLabel = '' #default
    tauLabel = 'DFTauLoose'
    orTool = recommended_tools(outputLabel=outputLabel,bJetLabel=bJetLabel)
    algOR = OverlapRemovalGenUseAlg('OverlapRemovalGenUseAlg',
                                    OverlapLabel=outputLabel,
                                    OverlapRemovalTool=orTool,
                                    TauLabel=tauLabel,
                                    BJetLabel=bJetLabel)
    sequence += algOR

    MuonJetDrTool = CfgMgr.DerivationFramework__MuonJetDrTool("MuonJetDrTool")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += MuonJetDrTool
    sequence += CfgMgr.DerivationFramework__CommonAugmentation("DFCommonMuonsKernel2",
                                                          AugmentationTools = [MuonJetDrTool])

##################################################################
# Jet cleaning tool
##################################################################

def getJetCleaningTool(cleaningLevel,jetdef):
    jetcleaningtoolname = 'JetCleaningTool_'+cleaningLevel
    jetcleaningtool = None
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,jetcleaningtoolname):
        jetcleaningtool = getattr(ToolSvc,jetcleaningtoolname)
    else:
        jetcleaningtool = CfgMgr.JetCleaningTool(jetcleaningtoolname,CutLevel=cleaningLevel,JetContainer=jetdef)
        jetcleaningtool.UseDecorations = False
        ToolSvc += jetcleaningtool

    return jetcleaningtool

##################################################################
# Cleaning flags
##################################################################

def addEventCleanFlags(sequence, workingPoints = ['Loose', 'Tight', 'LooseLLP']):

    # Prereqs
    addPassJvtForCleaning(sequence)
    applyOverlapRemoval(sequence)

    from JetSelectorTools.JetSelectorToolsConf import ECUtils__EventCleaningTool as EventCleaningTool
    from JetSelectorTools.JetSelectorToolsConf import EventCleaningTestAlg
    supportedWPs = ['Loose', 'Tight', 'LooseLLP', 'VeryLooseLLP', 'SuperLooseLLP']
    prefix = "DFCommonJets_"

    for wp in workingPoints:
        if wp not in supportedWPs:
            dfjetlog.warning('*** Unsupported event cleaning WP {} requested! Skipping it.***'.format(wp))
            continue
        algName = 'EventCleaningTestAlg_' + wp
        if hasattr(sequence, algName):
            continue

        cleaningLevel = wp + 'Bad'
        # LLP WPs have a slightly different name format
        if 'LLP' in wp:
            cleaningLevel = wp.replace('LLP', 'BadLLP')

        ecTool = EventCleaningTool('EventCleaningTool_' + wp, CleaningLevel=cleaningLevel)
        ecTool.JetCleanPrefix = prefix
        ecTool.JetCleaningTool = getJetCleaningTool(cleaningLevel,"AntiKt4EMTopoJets")
        ecTool.JetContainer = "AntiKt4EMTopoJets"
        algClean = EventCleaningTestAlg(algName,
                                        EventCleaningTool=ecTool,
                                        JetCollectionName="AntiKt4EMTopoJets",
                                        EventCleanPrefix=prefix,
                                        CleaningLevel=cleaningLevel,
                                        doEvent = ('Loose' in wp)) # Only store event-level flags for Loose and LooseLLP
        sequence += algClean


###################################################################
# Function to add jet collections via new config
################################################################### 

def addDAODJets(jetlist,sequence):

    from JetRecConfig.JetRecConfig import getJetAlgs, reOrderAlgs
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    for jd in jetlist:
        algs, jetdef_i = getJetAlgs(ConfigFlags, jd, True)
        algs = reOrderAlgs( [a for a in algs if a is not None])
        for a in algs:
            if hasattr(sequence,a.getName()):
                continue
            sequence += conf2toConfigurable(a)


###################################################################
# Jet decoration tools
################################################################### 

def addJetPtAssociation(jetalg, truthjetalg, sequence):

    truthAssocAlgName = 'DFJetPtAssociationAlg_'+ truthjetalg + '_' + jetalg
    if hasattr(sequence, truthAssocAlgName):
        return
    jetTruthAssocTool = CfgMgr.JetPtAssociationTool('DFJetPtAssociation_' + truthjetalg + '_' + jetalg,
                                                    JetContainer = jetalg + 'Jets',
                                                    MatchingJetContainer = truthjetalg,
                                                    AssociationName = "GhostTruth")

    dfjetlog.info('JetCommon: Adding JetPtAssociationTool for jet collection: '+jetalg+'Jets')
    sequence += CfgMgr.JetDecorationAlg(truthAssocAlgName, JetContainer=jetalg+'Jets', Decorators=[jetTruthAssocTool])

##################################################################

def addJetTruthLabel(jetalg,labelname,sequence):
    supportedLabelNames = ['R10TruthLabel_R21Consolidated','R10TruthLabel_R21Precision']
    supportedTruthJets = ['AntiKt10Truth','AntiKt10TruthTrimmedPtFrac5SmallR20']
    supportedRecoJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20','AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20','AntiKt10UFOCSSKTrimmedPtFrac5SmallR20','AntiKt10UFOCSSKSoftDropBeta100Zcut10','AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5','AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5Ninf','AntiKt10UFOCHSTrimmedPtFrac5SmallR20']
    supportedJets = supportedRecoJets + supportedTruthJets
    if jetalg not in supportedJets:
        dfjetlog.warning('*** JetTruthLabeling augmentation requested for unsupported jet collection {}! ***'.format(jetalg))
        return
    elif labelname not in supportedLabelNames:
        dfjetlog.warning('*** JetTruthLabeling augmentation requested for unsupported label definition {}! ***'.format(labelname))
        return

    truthLabelAlgName = 'DFJetTruthLabelAlg_' + jetalg + '_' + labelname
    if hasattr(sequence, truthLabelAlgName):
        return

    isTruthJet = False
    if jetalg in supportedTruthJets:
        isTruthJet = True

    jetTruthLabelTool = CfgMgr.JetTruthLabelingTool('DFJetTruthLabel_'+jetalg+'_'+labelname,
                                                    IsTruthJetCollection = isTruthJet,
                                                    TruthLabelName = labelname)
    if not isTruthJet:
        jetTruthLabelTool.RecoJetContainer = jetalg + 'Jets'
    dfjetlog.info('JetCommon: Applying JetTruthLabel augmentation to jet collection: ' + jetalg + 'Jets' + ' using ' + labelname +' definition')
    sequence += CfgMgr.JetDecorationAlg(truthLabelAlgName, JetContainer=jetalg+'Jets', Decorators=[jetTruthLabelTool])

##################################################################  

def getPFlowfJVT(jetalg,sequence,primaryVertexCont="PrimaryVertices",overlapLabel="",outLabel="fJvt",includePV=False):
    supportedJets = ['AntiKt4EMPFlow']
    if jetalg not in supportedJets:
        dfjetlog.error('*** PFlow fJvt augmentation requested for unsupported jet collection {}! ***'.format(jetalg))
        return
    
    fJVTAlgName = "DFJetFJVTAlg_" + jetalg
    if hasattr(sequence, fJVTAlgName):
        return

    # Calibration tool specific for pFlow fJVT: without GSC and smearing
    jetCalibrationTool = CfgMgr.JetCalibrationTool('DFJetFJVT_' + jetalg + '_CalibTool',
                                                    JetCollection = 'AntiKt4EMPFlow',
                                                    ConfigFile = "JES_MC16Recommendation_Consolidated_PFlow_Apr2019_Rel21.config",
                                                    CalibSequence = "JetArea_Residual_EtaJES",
                                                    CalibArea = "00-04-82",
                                                    RhoKey = 'Kt4EMPFlowEventShape',
                                                    IsData = False)

    wPFOTool = CfgMgr.CP__WeightPFOTool("DFJetFJVT_" + jetalg + "_wPFO")

    from JetRecConfig.StandardSmallRJets import AntiKt4EMPFlow
    from JetRecConfig.StandardJetContext import jetContextDic

    fJVTTool = CfgMgr.JetForwardPFlowJvtTool('DFJetFJVT_' + jetalg,
                                                verticesName = primaryVertexCont,
                                                JetContainer = jetalg+"Jets",
                                                ### CHRIS - FIX ####
                                                TrackVertexAssociation = jetContextDic[AntiKt4EMPFlow.context]["TVA"],
                                                WeightPFOTool = wPFOTool,
                                                JetCalibrationTool = jetCalibrationTool,
                                                FEName = 'CHSGParticleFlowObjects',
                                                ORName = overlapLabel,
                                                FjvtRawName = 'DFCommonJets_' + outLabel,
                                                includePV = includePV)

    dfjetlog.info('JetCommon: Applying PFlow fJvt augmentation to jet collection: ' + jetalg + 'Jets')
    sequence += CfgMgr.JetDecorationAlg(fJVTAlgName, JetContainer=jetalg+'Jets', Decorators=[fJVTTool])

#################################################################
### Schedule Q/G-tagging decorations ### QGTaggerTool ##### 
#################################################################
def addQGTaggerTool(jetalg, sequence, truthjetalg=None):

    qgAlgName = 'DFQGTaggerAlg_' + jetalg
    if hasattr(sequence, qgAlgName):
        return

    if truthjetalg is not None:
        addJetPtAssociation(jetalg, truthjetalg, sequence)

    trackselectiontool = CfgMgr.InDet__InDetTrackSelectionTool('DFQGTaggerTool_InDetTrackSelectionTool_' + jetalg, CutLevel = "Loose" )
    trackvertexassoctool = CfgMgr.CP__TrackVertexAssociationTool('DFQGTaggerTool_InDetTrackVertexAssosciationTool_' + jetalg, WorkingPoint = "Loose")
    qgTool = CfgMgr.JetQGTaggerVariableTool('DFQGTaggerTool_' + jetalg,
                                            JetContainer=jetalg + 'Jets',
                                            VertexContainer='PrimaryVertices',
                                            TVATool=trackvertexassoctool,
                                            TrkSelTool=trackselectiontool)

    dfjetlog.info('JetCommon: Adding QGTaggerTool for jet collection: '+jetalg)
    sequence += CfgMgr.JetDecorationAlg(qgAlgName, JetContainer=jetalg+'Jets', Decorators=[qgTool])

##################################################################
def addVRTrackJetMoments(jetalg, sequence=DerivationFrameworkJob):

    VRvarAlgName = 'DFVRJetOverlapDecorator_' + jetalg

    if hasattr(sequence, VRvarAlgName):
        return

    vrODT = CfgMgr.FlavorTagDiscriminants__VRJetOverlapDecoratorTool("VRJetOverlapDecoratorTool")

    dfjetlog.info('JetCommon: Adding VRJetOverlapDecoratorTool for jet collection: '+jetalg)
    sequence += CfgMgr.JetDecorationAlg(VRvarAlgName, JetContainer=jetalg+'Jets', Decorators=[vrODT])


##################################################################

def addPassJvtForCleaning(sequence=DerivationFrameworkJob):
    from JetJvtEfficiency.JetJvtEfficiencyToolConfig import getJvtEffTool
    algName = "DFJet_EventCleaning_passJvtAlg"
    if hasattr(sequence, algName):
        return

    passJvtTool = getJvtEffTool('AntiKt4EMTopo')
    passJvtTool.PassJVTKey = "AntiKt4EMTopoJets.DFCommonJets_passJvt"

    dfjetlog.info('JetCommon: Adding passJvt decoration to AntiKt4EMTopoJets for event cleaning')
    sequence += CfgMgr.JetDecorationAlg(algName, JetContainer='AntiKt4EMTopoJets', Decorators=[passJvtTool])


##################################################################
# Add specific PFlow event shape from sidebands
##################################################################

def addSidebandEventShape(sequence=DerivationFrameworkJob):

    from JetRecConfig.JetRecConfig import getInputAlgs,getConstitPJGAlg,reOrderAlgs
    from JetRecConfig.StandardJetConstits import stdConstitDic as cst
    from JetRecConfig.JetInputConfig import buildEventShapeAlg
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    constit_algs = getInputAlgs(cst.GPFlow, configFlags=ConfigFlags)
    constit_algs = reOrderAlgs( [a for a in constit_algs if a is not None])

    for a in constit_algs:
        if not hasattr(sequence,a.getName()):
            sequence += conf2toConfigurable(a)

    constitPJAlg = getConstitPJGAlg(cst.GPFlow, suffix='EMPFlowPUSB')
    if not hasattr(sequence,constitPJAlg.getName()):
        sequence += conf2toConfigurable(constitPJAlg)

    eventshapealg = buildEventShapeAlg(cst.GPFlow, '', suffix = 'EMPFlowPUSB' )
    if not hasattr(sequence, eventshapealg.getName()):
        sequence += conf2toConfigurable(eventshapealg)


##################################################################
# Set up helpers for adding jets to the output streams
##################################################################
OutputJets = {}
OutputJets["SmallR"] = ["AntiKt4EMPFlowJets",
                        "AntiKt4EMTopoJets",
                        "AntiKt4TruthJets"]

OutputJets["LargeR"] = ["AntiKt10UFOCSSKJets",
                        "AntiKt10TruthJets"]

def addJetOutputs(slimhelper,contentlist,smartlist=[],vetolist=[]):
    outputlist = []
    for content in contentlist:
        if content in OutputJets.keys():
            for item in OutputJets[content]:
                if item in vetolist: continue
                outputlist.append(item)
        else:
            outputlist.append(content)

    for item in outputlist:
        if item not in slimhelper.AppendToDictionary:
            slimhelper.AppendToDictionary[item]='xAOD::JetContainer'
            slimhelper.AppendToDictionary[item+"Aux"]='xAOD::JetAuxContainer'
        if item in smartlist:
            dfjetlog.info( "Add smart jet collection "+item )
            slimhelper.SmartCollections.append(item)
        else:
            dfjetlog.info( "Add full jet collection "+item )
            slimhelper.AllVariables.append(item)

##################################################################
# Helper to add origin corrected clusters to output
##################################################################
def addOriginCorrectedClusters(slimhelper,writeLC=False,writeEM=False):

    slimhelper.ExtraVariables.append('CaloCalTopoClusters.calE.calEta.calPhi.calM')

    if writeLC:
        if "LCOriginTopoClusters" not in slimhelper.AppendToDictionary:
            slimhelper.AppendToDictionary["LCOriginTopoClusters"]='xAOD::CaloClusterContainer'
            slimhelper.AppendToDictionary["LCOriginTopoClustersAux"]='xAOD::ShallowAuxContainer'
            slimhelper.ExtraVariables.append('LCOriginTopoClusters.calEta.calPhi')

    if writeEM:
        if "EMOriginTopoClusters" not in slimhelper.AppendToDictionary:
            slimhelper.AppendToDictionary["EMOriginTopoClusters"]='xAOD::CaloClusterContainer'
            slimhelper.AppendToDictionary["EMOriginTopoClustersAux"]='xAOD::ShallowAuxContainer'
            slimhelper.ExtraVariables.append('EMOriginTopoClusters.calE.calEta.calPhi')
