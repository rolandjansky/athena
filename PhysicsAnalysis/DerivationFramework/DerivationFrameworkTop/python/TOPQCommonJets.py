# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#===================================================================================
# Common file used for TOPQ jet setup
# Call with:
#   import DerivationFrameworkTop.TOPQCommonJets
#   DerivationFrameworkTop.TOPQCommonJets.buildTOPQCA15jets(TOPQXSequence)
#   DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("JetCollection")
#   DerivationFrameworkTop.TOPQCommonJets.TOPQupdateJVT("JetCollection")
#===================================================================================

#========
# IMPORTS
#========
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.ObjKeyStore import cfgKeyStore

#==================
# CamKt15LCTopoJets
#==================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkJetEtMiss/trunk/python/ExtendedJetCommon.py
def buildTOPQCA15jets(algseq):
    if not cfgKeyStore.isInInput("xAOD::JetContainer","CamKt15LCTopoJets"):
        from JetRec.JetRecConf import JetAlgorithm
        jtm.modifiersMap["lctopoCA15"] = list(jtm.modifiersMap["lctopo"])
        if globalflags.DataSource()=='geant4':
            jtm.modifiersMap["lctopoCA15"].remove('truthassoc')
        if hasattr(jtm,"CamKt15LCTopoJets"):
            TOPQCA15LC = getattr(jtm,"CamKt15LCTopoJets")
        else:
            TOPQCA15LC = jtm.addJetFinder("CamKt15LCTopoJets", "CamKt", 1.5, "lctopo","lctopoCA15", ghostArea=0.01, ptmin= 2000, ptminFilter=100000, calibOpt="aro")
        if not hasattr(algseq,"jetalgCamKt15LCTopo"):
            TOPQCA15LCalg = JetAlgorithm("jetalgCamKt15LCTopo", Tools = [TOPQCA15LC] )
            algseq += TOPQCA15LCalg
            print "Running jet finding algorithm for CamKt15LCTopoJets"
# end buildTOPQCA15jets(algseq)

#================
# Soft drop jets
#================
def addSoftDropJetsForTop(algseq, outputGroup):
    from DerivationFrameworkJetEtMiss.JetCommon import addSoftDropJets
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addCSSKSoftDropJets
    if (globalflags.DataSource()=='geant4'):
        addSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.1, mods="truth_groomed", algseq=algseq, outputGroup=outputGroup, writeUngroomed=True)

    addCSSKSoftDropJets(algseq, outputGroup)

#================
# TTC jets
#================
def addTCCTrimmedJetsForTop(algseq, outputGroup):
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addTCCTrimmedJets
    addTCCTrimmedJets(algseq, outputGroup)

#================
# VR jets
#================
def addVRJetsForTop(algseq):
    from DerivationFrameworkFlavourTag.HbbCommon import addVRJets
    addVRJets(algseq)

#================
# xbb tagging information
#================
def addXbbTaggerInformation(algseq, ToolSvc):
    from DerivationFrameworkFlavourTag.HbbCommon import addRecommendedXbbTaggers
    addRecommendedXbbTaggers(algseq, ToolSvc)

#================
# JET CALIBRATION
#================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkJetEtMiss/trunk/python/ExtendedJetCommon.py
def applyTOPQJetCalibration(jetcollection, algseq=None):
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import \
        applyJetCalibration_xAODColl, applyJetCalibration_CustomColl

    supportedJets = ['AntiKt4EMTopo', 'AntiKt4EMPFlow', 'AntiKt10LCTopoTrimmedPtFrac5SmallR20']
    if not jetcollection in supportedJets:
        print "TOPQCommonJets:",jetcollection, "is an unsupported collection for calibration!"
        return
    elif jetcollection == 'AntiKt4EMTopo':
        applyJetCalibration_xAODColl(jetcollection,algseq)
    elif jetcollection == 'AntiKt4EMPFlow':
        applyJetCalibration_xAODColl(jetcollection,algseq)
    elif jetcollection == 'AntiKt10LCTopoTrimmedPtFrac5SmallR20':
        applyJetCalibration_CustomColl(jetcollection, algseq)
# end applyTOPQJetCalibration(jetcollection, algseq=None)

#=======================
# JVT AUGMENTATION
#=======================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkJetEtMiss/trunk/python/ExtendedJetCommon.py
def TOPQupdateJVT(jetcollection, algseq=None):
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import updateJVT_xAODColl

    supportedJets = ['AntiKt4EMTopo']
    if not jetcollection in supportedJets:
        print "TOPQCommonJets:", jetcollection, "is an unsupported collection for JVT augmentation!"
        return
    else:
        updateJVT_xAODColl(jetcollection,algseq)
# end TOPQupdateJVT(jetcollection, algseq=None)

#=======================
# MSV variables
#=======================
def addMSVVariables(jetcollection, algseq, ToolSvc):
    from DerivationFrameworkTop.DerivationFrameworkTopConf import DerivationFramework__JetMSVAugmentation
    TopQJetMSVAugmentation = DerivationFramework__JetMSVAugmentation("TopQJetMSVAugmentation")
    TopQJetMSVAugmentation.JetCollectionName=jetcollection
    ToolSvc+=TopQJetMSVAugmentation
    augmentationTools = [TopQJetMSVAugmentation]
    TOPQJetMSVKernel = CfgMgr.DerivationFramework__CommonAugmentation("TOPQJetMSVKernel", AugmentationTools = augmentationTools)
    algseq+=TOPQJetMSVKernel

#=======================
# ExKtDoubleTagVariables
#=======================
def addExKtDoubleTagVariables(algseq, ToolSvc):
    from DerivationFrameworkJetEtMiss.JetCommon import DFJetAlgs
    from DerivationFrameworkFlavourTag.HbbCommon import addExKtCoM
    jetToolName = "DFReclustertingTool"
    algoName = "DFJetReclusteringAlgo"
    if jetToolName not in DFJetAlgs:
        ToolSvc += CfgMgr.JetReclusteringTool(jetToolName,InputJetContainer="AntiKt4EMPFlowJets", OutputJetContainer="AntiKt8EMPFlowJets")
        getattr(ToolSvc,jetToolName).ReclusterRadius = 0.8
        getattr(ToolSvc,jetToolName).InputJetPtMin = 0
        getattr(ToolSvc,jetToolName).RCJetPtMin = 1
        getattr(ToolSvc,jetToolName).TrimPtFrac = 0
        getattr(ToolSvc,jetToolName).DoArea = False
        getattr(ToolSvc,jetToolName).GhostTracksInputContainer = "InDetTrackParticles"
        getattr(ToolSvc,jetToolName).GhostTracksVertexAssociationName  = "JetTrackVtxAssoc"
        DFisMC = (globalflags.DataSource()=='geant4')
        if(DFisMC):
            getattr(ToolSvc,jetToolName).GhostTruthInputBContainer = "BHadronsFinal"
            getattr(ToolSvc,jetToolName).GhostTruthInputCContainer = "CHadronsFinal"

        algseq += CfgMgr.AthJetReclusteringAlgo(algoName, JetReclusteringTool = getattr(ToolSvc,jetToolName))
        DFJetAlgs[jetToolName] = getattr(ToolSvc,jetToolName)
    ExKtTopJetCollection__FatJetConfigs = {
        "AntiKt8EMPFlowJets"         : {"doTrackSubJet": True},
    }

    # build subjets
    ExKtTopJetCollection__FatJet = ExKtTopJetCollection__FatJetConfigs.keys()
    ExKtTopJetCollection__SubJet = []

    for key, config in ExKtTopJetCollection__FatJetConfigs.items():
        # N=2 subjets
        ExKtTopJetCollection__SubJet += addExKtCoM(algseq, ToolSvc, key, nSubjets=2, **config)

        # N=3 subjets
        if "RNNCone" not in key:
            ExKtTopJetCollection__SubJet += addExKtCoM(algseq, ToolSvc, key, nSubjets=3, **config)

    algseq += CfgMgr.xAODMaker__ElementLinkResetAlg("ELReset_AfterSubjetBuild", SGKeys=[name+"Aux." for name in ExKtTopJetCollection__SubJet])
    from BTagging.BTaggingFlags import BTaggingFlags
    BTaggingFlags.CalibrationChannelAliases += [ jetname[:-4].replace("PV0", "")+"->AntiKt4EMTopo" for jetname in ExKtTopJetCollection__FatJet ]
    BTaggingFlags.CalibrationChannelAliases += [ jetname[:-4].replace("PV0", "")+"->AntiKt4EMTopo" for jetname in ExKtTopJetCollection__SubJet ]

    from DerivationFrameworkTop.TOPQAugTools import TOPQExKtCommonTruthKernel
    algseq += TOPQExKtCommonTruthKernel
