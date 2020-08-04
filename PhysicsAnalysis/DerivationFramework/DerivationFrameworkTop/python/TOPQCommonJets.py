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
    if (globalflags.DataSource()=='geant4'):
        addSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.1, mods="truth_groomed", algseq=algseq, outputGroup=outputGroup, writeUngroomed=True)


#================
# TTC jets
#================
def addTCCTrimmedJetsForTop(algseq, outputGroup):
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addTCCTrimmedJets
    addTCCTrimmedJets(algseq, outputGroup)

#================
# VR jets
#================
def addVRJetsForTop(algseq, train='201810'):
    from DerivationFrameworkFlavourTag.HbbCommon import addVRJets
    addVRJets(algseq,training=train)

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

    from DerivationFrameworkFlavourTag.HbbCommon import addExKtDoubleTaggerRCJets
    addExKtDoubleTaggerRCJets(algseq, ToolSvc)

    from DerivationFrameworkFlavourTag.DerivationFrameworkFlavourTagConf import BTagVertexAugmenter
    from InDetVKalVxInJetTool.InDetVKalVxInJetFinder import InDetVKalVxInJetFinder

    algseq += BTagVertexAugmenter("BTagVertexAugmenter")

    SoftBJetMSVFinderTool = InDetVKalVxInJetFinder("SoftBJetMSVFinder")

    SoftBJetMSVFinderTool.ConeForTag = 0.8
    SoftBJetMSVFinderTool.MultiVertex = True
    SoftBJetMSVFinderTool.MultiWithOneTrkVrt = True    

    ToolSvc += SoftBJetMSVFinderTool

    softTagAlgMSVTight = CfgMgr.SoftBVrt__SoftBVrtClusterTool("SoftBVrtClusterToolMSVTight")
    softTagAlgMSVTight.SVFinderName = 'SoftBJetMSVFinder'
    softTagAlgMSVTight.TrackJetCollectionName = 'AntiKt4PV0TrackJets'
    softTagAlgMSVTight.TrackSelectionTool.CutLevel = "LoosePrimary"
    softTagAlgMSVTight.OperatingPoint = 'MSVTight'

    algseq += softTagAlgMSVTight

    from DerivationFrameworkTop.TOPQAugTools import TOPQExKtCommonTruthKernel
    algseq += TOPQExKtCommonTruthKernel

    
