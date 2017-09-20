# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Add translator from EVGEN input to xAOD-like truth here
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from RecExConfig.ObjKeyStore import objKeyStore
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
from DerivationFrameworkMCTruth.TruthDerivationTools import *

augmentationToolsList = []

dfInputIsEVNT = False # Flag to distinguish EVNT from AOD input
# Build truth collection if input is HepMC. Must be scheduled first to allow slimming.
# Input file is EVNT
if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
    DerivationFrameworkJob.insert(0,xAODMaker__xAODTruthCnvAlg("GEN_EVNT2xAOD",AODContainerName="GEN_EVENT"))
    dfInputIsEVNT = True
# Input file is HITS
elif objKeyStore.isInInput( "McEventCollection", "TruthEvent"):
    DerivationFrameworkJob.insert(0,xAODMaker__xAODTruthCnvAlg("GEN_EVNT2xAOD",AODContainerName="TruthEvent"))
    dfInputIsEVNT = True
# If it isn't available, make a truth meta data object (will hold MC Event Weights)
if not objKeyStore.isInInput( "xAOD::TruthMetaDataContainer", "TruthMetaData" ) and not dfInputIsEVNT:
    # If we are going to be making the truth collection (dfInputIsEVNT) then this will be made elsewhere
    ToolSvc += CfgMgr.DerivationFramework__TruthMetaDataWriter(name='DFCommonTruthMetaDataWriter')
    augmentationToolsList.append(ToolSvc.DFCommonTruthMetaDataWriter)
# Add in some jets - global config if we are running on EVNT
if dfInputIsEVNT:
    from JetRec.JetRecFlags import jetFlags
    jetFlags.useTruth = True
    jetFlags.useTracks = False
    jetFlags.truthFlavorTags = ["BHadronsInitial", "BHadronsFinal", "BQuarksFinal",
                                "CHadronsInitial", "CHadronsFinal", "CQuarksFinal",
                                "TausFinal",
                                "Partons",
                                ]

# Add jet algorithms if they aren't there
from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm
truth_modifiers = [jtm.truthpartondr, jtm.partontruthlabel, jtm.jetdrlabeler, jtm.trackjetdrlabeler]
if not objKeyStore.isInInput( "xAOD::JetContainer","AntiKt4TruthJets"):
    # Standard truth jets
    # To remove jet constituents add the modifier jtm.removeconstit
    from DerivationFrameworkJetEtMiss.JetCommon import addStandardJets
    addStandardJets("AntiKt", 0.4, "Truth", 15000, mods=truth_modifiers, algseq=DerivationFrameworkJob, outputGroup="DFCommonMCTruthJets")
if not objKeyStore.isInInput( "xAOD::JetContainer","AntiKt4TruthWZJets"):
    # WZ Truth Jets
    from DerivationFrameworkJetEtMiss.JetCommon import addStandardJets
    addStandardJets("AntiKt", 0.4, "TruthWZ", 15000, mods=truth_modifiers, algseq=DerivationFrameworkJob, outputGroup="DFCommonMCTruthJets")

# Some examples of other truth jet collections
#akt6wz    = jtm.addJetFinder("AntiKt6TruthWZJets",  "AntiKt", 0.6,  "truthwz", ptmin= 5000)
#akt6      = jtm.addJetFinder("AntiKt6TruthJets", "AntiKt", 0.6, "truth", ptmin= 5000)

if not objKeyStore.isInInput( "xAOD::JetContainer","TrimmedAntiKt10TruthJets"):
    #Large R jets
    from DerivationFrameworkJetEtMiss.JetCommon import addTrimmedJets
    addTrimmedJets('AntiKt', 1.0, 'Truth', rclus=0.2, ptfrac=0.05, mods="truth_groomed",
                   algseq=DerivationFrameworkJob, outputGroup="Trimmed", writeUngroomed=False)

# If we are running on EVNT, we also need some MET
if dfInputIsEVNT:
    # Add truth-based MET algorithm here
    import METReconstruction.METConfig_Truth
    from METReconstruction.METRecoFlags import metFlags # not sure if you even need this line
    from METReconstruction.METRecoConfig import getMETRecoAlg
    metAlg = getMETRecoAlg('METReconstruction')
    DerivationFrameworkJob += metAlg

#Save the post-shower HT and MET filter values that will make combining filtered samples easier (adds to the EventInfo)
from DerivationFrameworkMCTruth.GenFilterToolSetup import *

# schedule the special truth building tools and add them to a common augmentation; note taus are handled separately below
augmentationToolsList += [ DFCommonTruthClassificationTool,
                           DFCommonTruthGenFilter,
                           DFCommonTruthMuonTool,DFCommonTruthElectronTool,
                           DFCommonTruthPhotonToolSim,
                           DFCommonTruthNeutrinoTool,
                           DFCommonTruthTopTool,
                           DFCommonTruthBosonTool,
                           DFCommonTruthBSMTool,
                           DFCommonTruthElectronDressingTool, DFCommonTruthMuonDressingTool,
                           DFCommonTruthElectronIsolationTool1, DFCommonTruthElectronIsolationTool2,
                           DFCommonTruthMuonIsolationTool1, DFCommonTruthMuonIsolationTool2,
                           DFCommonTruthPhotonIsolationTool1, DFCommonTruthPhotonIsolationTool2,
                           DFCommonTruthQGLabelTool]
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonKernel",
                                                                         AugmentationTools = augmentationToolsList
                                                                         )

# Tau collections are built separately
from DerivationFrameworkTau.TauTruthCommon import *
