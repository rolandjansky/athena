# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Add translator from EVGEN input to xAOD-like truth here
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from RecExConfig.ObjKeyStore import objKeyStore
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
from DerivationFrameworkMCTruth.TruthDerivationTools import *

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
    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
    kernel += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonMetaDataWriterKernel",
                                                             AugmentationTools = [ToolSvc.DFCommonTruthMetaDataWriter]
                                                             )
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

def addTruthJetsEVNT(kernel=None, decorationDressing=None):
    # Ensure that we are running on something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    # Add jet algorithms if they aren't there
    from JetRec.JetRecStandard import jtm
    from JetRec.JetRecConf import JetAlgorithm
    truth_modifiers = [jtm.truthpartondr, jtm.partontruthlabel, jtm.jetdrlabeler, jtm.trackjetdrlabeler]
    if not objKeyStore.isInInput( "xAOD::JetContainer","AntiKt4TruthJets"):
        # Standard truth jets
        # To remove jet constituents add the modifier jtm.removeconstit
        from DerivationFrameworkJetEtMiss.JetCommon import addStandardJets
        addStandardJets("AntiKt", 0.4, "Truth", 15000, mods=truth_modifiers, algseq=kernel, outputGroup="DFCommonMCTruthJets")
    if not objKeyStore.isInInput( "xAOD::JetContainer","AntiKt4TruthWZJets"):
        # WZ Truth Jets - handle dressed and non-dressed cases
        from DerivationFrameworkJetEtMiss.JetCommon import addStandardJets
        if decorationDressing is None:
            addStandardJets("AntiKt", 0.4, "TruthWZ", 15000, mods=truth_modifiers, algseq=kernel, outputGroup="DFCommonMCTruthJets")
        else:
            addStandardJets("AntiKt", 0.4, "TruthDressedWZ", ptmin=15000, mods="truth_ungroomed", algseq=kernel, outputGroup="DFCommonMCTruthJets")
    if not objKeyStore.isInInput( "xAOD::JetContainer","TrimmedAntiKt10TruthJets"):
        #Large R jets
        from DerivationFrameworkJetEtMiss.JetCommon import addTrimmedJets
        addTrimmedJets('AntiKt', 1.0, 'Truth', rclus=0.2, ptfrac=0.05, mods="truth_groomed",
                       algseq=kernel, outputGroup="Trimmed", writeUngroomed=False)

def addTruthJetsAOD(kernel=None, decorationDressing=None):
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    # In this case, we simply use the helpers from ExtendedJetCommon
    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
    addAntiKt4TruthJets(kernel,"TRUTH") # Ignore the output list
    if decorationDressing is None:
        addAntiKt4TruthWZJets(kernel,"TRUTH")
    else:
        addAntiKt4TruthDressedWZJets(kernel,'TRUTH')
    addAntiKt10TruthJets(kernel,"TRUTH")

# Helper for adding truth jet collections
def addTruthJets(kernel=None, decorationDressing=None):
    # In case it's requested, set up the use of photon decorations from dressing code
    if decorationDressing is not None:
        # Ensure that we are adding it to something
        if kernel is None:
            from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
            kernel = DerivationFrameworkJob
        # make sure if we are using EVNT that we don't try to check sim metadata 
        barCodeFromMetadata=2
        if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
            barCodeFromMetadata=0
        from JetRec.JetRecStandardToolManager import jtm
        from ParticleJetTools.ParticleJetToolsConf import CopyTruthJetParticles
        jtm += CopyTruthJetParticles("truthpartdressedwz", OutputName="JetInputTruthParticlesDressedWZ",
                                     MCTruthClassifier=jtm.JetMCTruthClassifier,
                                     IncludePromptLeptons=False,IncludePromptPhotons=False,
                                     IncludeMuons=True,IncludeNeutrinos=True,BarCodeFromMetadata=barCodeFromMetadata,
                                     FSRPhotonCone=-1., DressingDecorationName=decorationDressing
                                    )
        # Add a jet tool runner for this thing
        from JetRec.JetRecConf import JetToolRunner,JetAlgorithm,PseudoJetGetter
        jtm += JetToolRunner("jetdressedwzrun", EventShapeTools=[], Tools=[jtm.truthpartdressedwz], Timer=jetFlags.timeJetToolRunner() )
        # And an algorithm to run in
        kernel += JetAlgorithm("jetdressedwzalg")
        jetdressedwzalg = kernel.jetdressedwzalg
        jetdressedwzalg.Tools = [ jtm.jetdressedwzrun ]
        jtm += PseudoJetGetter(
          "truthdressedwzget",
          Label = "TruthDressedWZ",
          InputContainer = jtm.truthpartdressedwz.OutputName,
          OutputContainer = "PseudoJetTruthDressedWZ",
          GhostScale = 0.0,
          SkipNegativeEnergy = True
        )
        jtm.gettersMap['truthdressedwz'] = [jtm.truthdressedwzget]
    # Propagate that downward
    if dfInputIsEVNT:
        addTruthJetsEVNT(kernel,decorationDressing)
    else:
        addTruthJetsAOD(kernel,decorationDressing)

# Helper for scheduling the truth MET collection
def addTruthMET(kernel=None):
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    # Only do this if the truth MET is not present
    # This should handle EVNT correctly without an explicit check
    from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
    if not objKeyStore.isInInput( "xAOD::MissingETContainer","MET_Truth") and not hasattr(DerivationFrameworkJob,"METReconstruction"):
        import METReconstruction.METConfig_Truth
        from METReconstruction.METRecoConfig import getMETRecoAlg
        metAlg = getMETRecoAlg('METReconstruction')
        kernel += metAlg

def schedulePreJetMCTruthAugmentations(kernel=None, decorationDressing=None):
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    # These augmentations do *not* require truth jets at all
    # If requested, add a decoration to photons that were used in the dressing
    if decorationDressing is not None:
        DFCommonTruthElectronDressingTool.decorationName = decorationDressing
        DFCommonTruthMuonDressingTool.decorationName = decorationDressing

    # Tau collections are built separately
    import DerivationFrameworkTau.TauTruthCommon

    # schedule the special truth building tools and add them to a common augmentation; note taus are handled separately below
    augmentationToolsList = [ DFCommonTruthClassificationTool,
                              DFCommonTruthMuonTool,DFCommonTruthElectronTool,
                              DFCommonTruthPhotonToolSim,
                              DFCommonTruthNeutrinoTool,
                              DFCommonTruthTopTool,
                              DFCommonTruthBosonTool,
                              DFCommonTruthBSMTool,
                              DFCommonTruthElectronDressingTool, DFCommonTruthMuonDressingTool,
                              DFCommonTruthTauDressingTool,
                              DFCommonTruthElectronIsolationTool1, DFCommonTruthElectronIsolationTool2,
                              DFCommonTruthMuonIsolationTool1, DFCommonTruthMuonIsolationTool2,
                              DFCommonTruthPhotonIsolationTool1, DFCommonTruthPhotonIsolationTool2]
    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
    kernel += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonPreJetKernel",
                                                             AugmentationTools = augmentationToolsList
                                                             )

def schedulePostJetMCTruthAugmentations(kernel=None):
    # These augmentations *require* truth jets in order to behave properly
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob

    #Save the post-shower HT and MET filter values that will make combining filtered samples easier (adds to the EventInfo)
    from DerivationFrameworkMCTruth.GenFilterToolSetup import DFCommonTruthGenFilter

    # schedule the special truth building tools and add them to a common augmentation; note taus are handled separately below
    augmentationToolsList = [ DFCommonTruthGenFilter,
                              DFCommonTruthQGLabelTool]
    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
    kernel += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonPostJetKernel",
                                                             AugmentationTools = augmentationToolsList
                                                             )

# This adds the entirety of TRUTH3
def addStandardTruthContents(kernel=None,
                             decorationDressing='dressedPhoton',
                             includeTausInDressingPhotonRemoval=False):
    # Tools that must come before jets
    schedulePreJetMCTruthAugmentations(kernel, decorationDressing)
    # Should photons that are dressed onto taus also be removed from truth jets?
    if includeTausInDressingPhotonRemoval:
        ToolSvc.DFCommonTruthTauDressingTool.decorationName=decorationDressing
    # Jets and MET
    addTruthJets(kernel, decorationDressing)
    addTruthMET(kernel)
    # Tools that must come after jets
    schedulePostJetMCTruthAugmentations(kernel)

# Add taus and their downstream particles (immediate and further decay products) in a special collection
def addTausAndDownstreamParticles(kernel):
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    # Set up a tool to keep the taus and all downstream particles
    DFCommonTausAndDecaysTool = DerivationFramework__TruthDecayCollectionMaker( name="DFCommonTausAndDecaysTool",
                                                                   NewCollectionName="TauWithDecay",
                                                                   PDGIDsToKeep = [15])
    ToolSvc += DFCommonTausAndDecaysTool
    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
    kernel += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonTausAndDecaysKernel",
                                                             AugmentationTools = [DFCommonTausAndDecaysTool] )

# Add electrons, photons, and their downstream particles in a special collection
def addEgammaAndDownstreamParticles(kernel):
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    # Set up a tool to keep the e/gammas and all downstream particles
    DFCommonEgammasAndDecaysTool = DerivationFramework__TruthDecayCollectionMaker( name="DFCommonEgammasAndDecaysTool",
                                                                   NewCollectionName="EgammaWithDecay",
                                                                   PDGIDsToKeep = [11,22])
    ToolSvc += DFCommonEgammasAndDecaysTool
    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
    kernel += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonEgammasAndDecaysKernel",
                                                             AugmentationTools = [DFCommonEgammasAndDecaysTool] )
