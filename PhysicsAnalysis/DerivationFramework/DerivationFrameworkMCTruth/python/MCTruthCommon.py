# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Add translator from EVGEN input to xAOD-like truth here
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
from RecExConfig.ObjKeyStore import objKeyStore
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
import DerivationFrameworkMCTruth.TruthDerivationTools as dfTruth

from AthenaCommon import CfgMgr
from AthenaCommon import Logging
dfcommontruthlog = Logging.logging.getLogger('DFCommonTruth')

# Execute this only for MC
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo
if DerivationFrameworkIsMonteCarlo:
    dfInputIsEVNT = False # Flag to distinguish EVNT from AOD input
    # Build truth collection if input is HepMC. Must be scheduled first to allow slimming.
    # Input file is EVNT
    if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
        if not hasattr(DerivationFrameworkJob,'GEN_EVNT2xAOD'):
            DerivationFrameworkJob.insert(0,xAODMaker__xAODTruthCnvAlg("GEN_EVNT2xAOD",AODContainerName="GEN_EVENT"))
        dfInputIsEVNT = True
    # Input file is HITS and translation hasn't been scheduled - careful with the name difference!
    elif objKeyStore.isInInput( "McEventCollection", "TruthEvent"):
        if not hasattr(DerivationFrameworkJob,'GEN_AOD2xAOD'):
            DerivationFrameworkJob.insert(0,xAODMaker__xAODTruthCnvAlg("GEN_EVNT2xAOD",AODContainerName="TruthEvent"))
        dfInputIsEVNT = True
    # If it isn't available, make a truth meta data object (will hold MC Event Weights)
    if not objKeyStore.isInInput( "xAOD::TruthMetaDataContainer", "TruthMetaData" ) and not dfInputIsEVNT:
        # If we are going to be making the truth collection (dfInputIsEVNT) then this will be made elsewhere
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += CfgMgr.DerivationFramework__TruthMetaDataWriter(name='DFCommonTruthMetaDataWriter')
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonMetaDataWriterKernel",
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

# Helper for adding truth jet collections via new jet config
def addTruthJets(kernel=None, decorationDressing=None):
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob

    from JetRecConfig.StandardSmallRJets import AntiKt4Truth,AntiKt4TruthWZ,AntiKt4TruthDressedWZ,AntiKtVRTruthCharged
    from JetRecConfig.StandardLargeRJets import AntiKt10TruthTrimmed,AntiKt10TruthSoftDrop

    jetList = [AntiKt4Truth,AntiKt4TruthWZ,AntiKt4TruthDressedWZ,AntiKtVRTruthCharged,
               AntiKt10TruthTrimmed,AntiKt10TruthSoftDrop]

    from DerivationFrameworkJetEtMiss.JetCommon import addDAODJets
    addDAODJets(jetList, kernel)


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
        import METReconstruction.METConfig_Truth  # noqa: F401
        from METReconstruction.METRecoConfig import getMETRecoAlg
        metAlg = getMETRecoAlg('METReconstruction')
        kernel += metAlg

def schedulePreJetMCTruthAugmentations(kernel=None, decorationDressing=None):
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    if hasattr(kernel,'MCTruthCommonPreJetKernel'):
        # Already there!  Carry on...
        return
    # These augmentations do *not* require truth jets at all
    # If requested, add a decoration to photons that were used in the dressing
    if decorationDressing is not None:
        dfTruth.DFCommonTruthElectronDressingTool.decorationName = decorationDressing
        dfTruth.DFCommonTruthMuonDressingTool.decorationName = decorationDressing

    # schedule the special truth building tools and add them to a common augmentation; note taus are handled separately below
    augmentationToolsList = [ dfTruth.DFCommonTruthClassificationTool,
                              dfTruth.DFCommonTruthMuonTool,dfTruth.DFCommonTruthElectronTool,
                              dfTruth.DFCommonTruthPhotonToolSim,
                              dfTruth.DFCommonTruthNeutrinoTool,
                              dfTruth.DFCommonTruthBottomTool,
                              dfTruth.DFCommonTruthTopTool,
                              dfTruth.DFCommonTruthBosonTool,
                              dfTruth.DFCommonTruthBSMTool,
                              dfTruth.DFCommonTruthForwardProtonTool,
                              dfTruth.DFCommonTruthElectronDressingTool, dfTruth.DFCommonTruthMuonDressingTool,
                              dfTruth.DFCommonTruthElectronIsolationTool1, dfTruth.DFCommonTruthElectronIsolationTool2,
                              dfTruth.DFCommonTruthMuonIsolationTool1, dfTruth.DFCommonTruthMuonIsolationTool2,
                              dfTruth.DFCommonTruthPhotonIsolationTool1, dfTruth.DFCommonTruthPhotonIsolationTool2, dfTruth.DFCommonTruthPhotonIsolationTool3]

    kernel += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonPreJetKernel",
                                                             AugmentationTools = augmentationToolsList
                                                             )





def schedulePostJetMCTruthAugmentations(kernel=None, decorationDressing=None):
    # These augmentations *require* truth jets in order to behave properly
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    if hasattr(kernel,'MCTruthCommonPostJetKernel'):
        # Already there!  Carry on...
        return

    # Tau collections are built separately
    # truth tau matching needs truth jets, truth electrons and truth muons
    from DerivationFrameworkTau.TauTruthCommon import scheduleTauTruthTools
    scheduleTauTruthTools(kernel)
    augmentationToolsList = [ dfTruth.DFCommonTruthTauDressingTool ]

    #Save the post-shower HT and MET filter values that will make combining filtered samples easier (adds to the EventInfo)
    if dfInputIsEVNT:
        from DerivationFrameworkMCTruth.GenFilterToolSetup import DFCommonTruthGenFilter

        # schedule the special truth building tools and add them to a common augmentation; note taus are handled separately below
        from DerivationFrameworkMCTruth.TruthDerivationTools import DFCommonTruthDressedWZQGLabelTool
        augmentationToolsList += [ DFCommonTruthGenFilter ,
                                   DFCommonTruthDressedWZQGLabelTool]
    else:
        augmentationToolsList = []
    # SUSY signal decorations
    from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
    if IsSUSYSignal():
        from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
        augmentationToolsList += DecorateSUSYProcess('MCTruthCommon')

    kernel += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonPostJetKernel",
                                                             AugmentationTools = augmentationToolsList
                                                             )

    # add SoW of individual SUSY final states, relies on augmentation from DecorateSUSYProcess()
    if IsSUSYSignal():
        from DerivationFrameworkSUSY.SUSYWeightMetadata import addSUSYWeights
        addSUSYWeights(kernel)

# This adds the entirety of TRUTH3
def addStandardTruthContents(kernel=None,
                             decorationDressing='dressedPhoton',
                             includeTausInDressingPhotonRemoval=False,
                             prefix=''):
    # Tools that must come before jets
    schedulePreJetMCTruthAugmentations(kernel, decorationDressing)
    # Should photons that are dressed onto taus also be removed from truth jets?
    if includeTausInDressingPhotonRemoval:
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc.DFCommonTruthTauDressingTool.decorationName=decorationDressing
    # Jets and MET
    addTruthJets(kernel, decorationDressing)
    addTruthMET(kernel)
    # Tools that must come after jets
    schedulePostJetMCTruthAugmentations(kernel, decorationDressing)
    # Add back the navigation contect for the collections we want
    addTruthCollectionNavigationDecorations(kernel, ["TruthElectrons", "TruthMuons", "TruthPhotons", "TruthTaus", "TruthNeutrinos", "TruthBSM", "TruthBottom", "TruthTop", "TruthBoson"], prefix=prefix)
    # Some more additions for standard TRUTH3
    addBosonsAndDownstreamParticles(kernel)
    if dfInputIsEVNT:
        addLargeRJetD2(kernel)
    # Special collection for BSM particles
    addBSMAndDownstreamParticles(kernel)
    # Special collection for Born leptons
    addBornLeptonCollection(kernel)
    # Special collection for hard scatter (matrix element) - save TWO extra generations of particles
    addHardScatterCollection(kernel,2)
    # Energy density for isolation corrections
    if dfInputIsEVNT:
        addTruthEnergyDensity(kernel)


def addParentAndDownstreamParticles(kernel=None,
                                    generations=1,
                                    parents=[6],
                                    prefix='TopQuark',
                                    collection_prefix=None,
                                    rejectHadronChildren=False):
  # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    kernel_name = 'MCTruthCommon'+prefix+'AndDecaysKernel'
    if hasattr(kernel,kernel_name):
        # Already there!  Carry on...
        dfcommontruthlog.warning("Attempt to add a duplicate "+kernel_name+". Failing.")
        return
    collection_name=collection_prefix+'WithDecay' if collection_prefix is not None else 'Truth'+prefix+'WithDecay'
    # Set up a tool to keep the W/Z/H bosons and all downstream particles
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthDecayCollectionMaker
    collection_maker = DerivationFramework__TruthDecayCollectionMaker( name='DFCommon'+prefix+'AndDecaysTool',
                                                                       NewCollectionName=collection_name,
                                                                       PDGIDsToKeep=parents,
                                                                       Generations=generations,
                                                                       RejectHadronChildren=rejectHadronChildren)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += collection_maker
    kernel += CfgMgr.DerivationFramework__CommonAugmentation(kernel_name,
                                                             AugmentationTools = [collection_maker] )

# Add taus and their downstream particles (immediate and further decay products) in a special collection
def addTausAndDownstreamParticles(kernel=None, generations=1):
    return addParentAndDownstreamParticles(kernel=kernel,
                                    generations=generations,
                                    parents=[15],
                                    prefix='Tau')

# Add W bosons and their downstream particles
def addWbosonsAndDownstreamParticles(kernel=None, generations=1,
                                     rejectHadronChildren=False):
    return addParentAndDownstreamParticles(kernel=kernel,
                                           generations=generations,
                                           parents=[24],
                                           prefix='Wboson',
                                           rejectHadronChildren=rejectHadronChildren)

# Add W/Z/H bosons and their downstream particles (notice "boson" here does not include photons and gluons)
def addBosonsAndDownstreamParticles(kernel=None, generations=1,
                                    rejectHadronChildren=False):
    return addParentAndDownstreamParticles(kernel=kernel,
                                           generations=generations,
                                           parents=[23,24,25],
                                           prefix='Bosons',
                                           rejectHadronChildren=rejectHadronChildren)


def addBottomQuarkAndDownstreamParticles(kernel=None, generations=1, rejectHadronChildren=False):
   return addParentAndDownstreamParticles(kernel=kernel,
                                          generations=generations,
                                          parents=[5],
                                          prefix='BottomQuark',
                                          rejectHadronChildren=rejectHadronChildren)

def addTopQuarkAndDownstreamParticles(kernel=None, generations=1,
                                      rejectHadronChildren=False):
   return addParentAndDownstreamParticles(kernel=kernel,
                                          generations=generations,
                                          parents=[6],
                                          prefix='TopQuark',
                                          rejectHadronChildren=rejectHadronChildren)

# Add electrons, photons, and their downstream particles in a special collection
def addEgammaAndDownstreamParticles(kernel=None, generations=1):
    return addParentAndDownstreamParticles(kernel=kernel,
                                           generations=generations,
                                           parents=[11,22],
                                           prefix='Egamma')

# Add b/c-hadrons and their downstream particles (immediate and further decay products) in a special collection
def addHFAndDownstreamParticles(kernel=None, addB=True, addC=True, generations=-1, prefix=''):
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    if hasattr(kernel,prefix+'MCTruthCommonHFAndDecaysKernel'):
        # Already there!  Carry on...
        dfcommontruthlog.warning("Attempt to add a duplicate "+prefix+"MCTruthCommonHFAndDecaysKernel. Failing.")
        return
    # Set up a tool to keep b- and c-quarks and all downstream particles
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthDecayCollectionMaker
    DFCommonHFAndDecaysTool = DerivationFramework__TruthDecayCollectionMaker( name=prefix+"DFCommonHFAndDecaysTool",
                                                                              NewCollectionName=prefix+"TruthHFWithDecay",
                                                                              KeepBHadrons=addB,
                                                                              KeepCHadrons=addC,
                                                                              Generations=generations)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += DFCommonHFAndDecaysTool
    kernel += CfgMgr.DerivationFramework__CommonAugmentation(prefix+"MCTruthCommonHFAndDecaysKernel",
                                                             AugmentationTools = [DFCommonHFAndDecaysTool] )

# Add a one-vertex-per event "primary vertex" container
def addPVCollection(kernel=None):
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    if hasattr(kernel,'MCTruthCommonTruthPVCollKernel'):
        # Already there!  Carry on...
        dfcommontruthlog.warning("Attempt to add a duplicate MCTruthCommonTruthPVCollKernel. Failing.")
        return
    # Set up a tool to keep the primary vertices
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthPVCollectionMaker
    DFCommonTruthPVCollTool = DerivationFramework__TruthPVCollectionMaker( name="DFCommonTruthPVCollTool",
                                                                      NewCollectionName="TruthPrimaryVertices")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += DFCommonTruthPVCollTool
    kernel += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonTruthPVCollKernel",
                                                             AugmentationTools = [DFCommonTruthPVCollTool] )

# Add a mini-collection for the hard scatter and N subsequent generations
def addHardScatterCollection(kernel=None, generations=1):
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    if hasattr(kernel,'MCTruthCommonHSCollectionKernel'):
        # Already there!  Carry on...
        dfcommontruthlog.warning("Attempt to add a duplicate MCTruthCommonHSCollectionKernel. Failing.")
        return
    # Set up a tool to keep the taus and all downstream particles
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__HardScatterCollectionMaker
    DFCommonHSCollectionTool = DerivationFramework__HardScatterCollectionMaker( name="DFCommonHSCollectionTool",
                                                                   NewCollectionName="HardScatter",
                                                                         Generations=generations)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += DFCommonHSCollectionTool
    kernel += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonHSCollectionKernel",
                                                             AugmentationTools = [DFCommonHSCollectionTool] )

# Add navigation decorations on the truth collections
def addTruthCollectionNavigationDecorations(kernel=None,TruthCollections=[], prefix=''):
    if len(TruthCollections)==0: return
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    if hasattr(kernel,prefix+'MCTruthNavigationDecoratorKernel'):
        # Already there, no need for duplication
        dfcommontruthlog.warning("Attempt to add a duplicate "+prefix+"MCTruthNavigationDecoratorKernel. Failing.")
        return
    # Set up a tool to add the navigation decorations
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthNavigationDecorator
    DFCommonTruthNavigationDecorator = DerivationFramework__TruthNavigationDecorator( name=prefix+'DFCommonTruthNavigationDecorator',
                                                                                      InputCollections=TruthCollections)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += DFCommonTruthNavigationDecorator
    kernel += CfgMgr.DerivationFramework__CommonAugmentation(prefix+"MCTruthNavigationDecoratorKernel",
                                                             AugmentationTools = [DFCommonTruthNavigationDecorator] )

# Add BSM particles and their downstream particles (immediate and further decay products) in a special collection
def addBSMAndDownstreamParticles(kernel=None, generations=-1):
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    if hasattr(kernel,'MCTruthCommonBSMAndDecaysKernel'):
        # Already there!  Carry on...
        dfcommontruthlog.warning("Attempt to add a duplicate MCTruthCommonBSMAndDecaysKernel. Failing.")
        return
    # Set up a tool to keep the taus and all downstream particles
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthDecayCollectionMaker
    DFCommonBSMAndDecaysTool = DerivationFramework__TruthDecayCollectionMaker( name="DFCommonBSMAndDecaysTool",
                                                                   NewCollectionName="TruthBSMWithDecay",
                                                                             KeepBSM=True,
                                                                         Generations=generations)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += DFCommonBSMAndDecaysTool
    kernel += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonBSMAndDecaysKernel",
                                                             AugmentationTools = [DFCommonBSMAndDecaysTool] )

# Add a mini-collection for the born leptons
def addBornLeptonCollection(kernel=None):
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    if hasattr(kernel,'MCTruthCommonBornLeptonsKernel'):
        # Already there!  Carry on...
        dfcommontruthlog.warning("Attempt to add a duplicate MCTruthCommonBornLeptonsKernel. Failing.")
        return
    # Set up a tool to keep the taus and all downstream particles
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthBornLeptonCollectionMaker
    DFCommonBornLeptonCollTool = DerivationFramework__TruthBornLeptonCollectionMaker( name="DFCommonBornLeptonCollTool",
                                                                         NewCollectionName="BornLeptons")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += DFCommonBornLeptonCollTool
    kernel += CfgMgr.DerivationFramework__CommonAugmentation("MCTruthCommonBornLeptonsKernel",
                                                             AugmentationTools = [DFCommonBornLeptonCollTool] )

def addLargeRJetD2(kernel=None):
    #Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    if hasattr(kernel,'TRUTHD2Kernel'):
        # Already there!  Carry on...
        dfcommontruthlog.warning("Attempt to add a duplicate TRUTHD2Kernel. Failing.")
        return

    #Extra classifier for D2 variable
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthD2Decorator
    TruthD2Decorator= DerivationFramework__TruthD2Decorator("TruthD2Decorator",
                                                            JetContainerKey = "AntiKt10TruthSoftDropBeta100Zcut10Jets",
                                                            DecorationName = "D2")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += TruthD2Decorator
    kernel +=CfgMgr.DerivationFramework__DerivationKernel("TRUTHD2Kernel",
                                                          AugmentationTools = [TruthD2Decorator] )


def addTruthEnergyDensity(kernel=None):
    #Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    if hasattr(kernel,'DFCommonTruthEDKernel'):
        # Already there!  Carry on...
        dfcommontruthlog.warning("Attempt to add a duplicate DFCommonTruthEDKernel. Failing.")
        return
    # Truth energy density tools
    from EventShapeTools.EventDensityConfig import configEventDensityTool,EventDensityAthAlg
    from AthenaCommon.AppMgr import ToolSvc
    from JetRecConfig.StandardJetConstits import stdConstitDic as cst
    # Algorithms for the energy density - needed only if e/gamma hasn't set things up already
    if not hasattr(ToolSvc,'EDTruthCentralTool'):
        DFCommonTruthCentralEDTool = configEventDensityTool("DFCommonTruthCentralEDTool",
                                                            cst.Truth,
                                                            0.5,
                                                            AbsRapidityMax      = 1.5,
                                                            OutputContainer     = "TruthIsoCentralEventShape",
                                                           )
        # Note the helper function mangles the naming in a specific way that is not sufficiently general
        #DFCommonTruthCentralEDTool.InputContainer = "PseudoJet"+cst.Truth.label
        ToolSvc += DFCommonTruthCentralEDTool
        kernel += EventDensityAthAlg("DFCommonTruthCentralEDAlg", EventDensityTool = DFCommonTruthCentralEDTool )
    if not hasattr(ToolSvc,'EDTruthForwardTool'):
        DFCommonTruthForwardEDTool = configEventDensityTool("DFCommonTruthForwardEDTool",
                                                            cst.Truth,
                                                            0.5,
                                                            AbsRapidityMin      = 1.5,
                                                            AbsRapidityMax      = 3.0,
                                                            OutputContainer     = "TruthIsoForwardEventShape",
                                                           )
        # Note the helper function mangles the naming in a specific way that is not sufficiently general
        #DFCommonTruthForwardEDTool.InputContainer = "PseudoJet"+cst.Truth.label
        ToolSvc += DFCommonTruthForwardEDTool
        kernel += EventDensityAthAlg("DFCommonTruthForwardEDAlg", EventDensityTool = DFCommonTruthForwardEDTool )

    # Now add the tool to do the decoration
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthEDDecorator
    DFCommonTruthEDDecorator = DerivationFramework__TruthEDDecorator("DFCommonTruthEDDecorator",
                                                                     EventInfoName="EventInfo",
                                                                     EnergyDensityKeys=["TruthIsoCentralEventShape","TruthIsoForwardEventShape"],
                                                                     DecorationSuffix="_rho"
                                                                    )
    ToolSvc += DFCommonTruthEDDecorator
    kernel +=CfgMgr.DerivationFramework__DerivationKernel("DFCommonTruthEDKernel",
                                                          AugmentationTools = [DFCommonTruthEDDecorator] )



def addMiniTruthCollectionLinks(kernel=None, doElectrons=True, doPhotons=True, doMuons=True):
    # Sets up modifiers to move pointers to old truth collections to new mini truth collections
    # Ensure that we are adding it to something
    if kernel is None:
        from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
        kernel = DerivationFrameworkJob
    if hasattr(kernel,'MiniCollectionTruthLinkKernel'):
        # Already there!  Carry on...
        dfcommontruthlog.warning("Attempt to add a duplicate MiniCollectionTruthLinkKernel. Failing.")
        return
    # Truth link setup for electrons, photons, and muons
    from AthenaCommon.AppMgr import ToolSvc
    aug_tools = []
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthLinkRepointTool
    if doElectrons:
        electron_relink = DerivationFramework__TruthLinkRepointTool("ElMiniCollectionTruthLinkTool",
                                                                    RecoCollection="Electrons", TargetCollections=["TruthMuons","TruthPhotons","TruthElectrons"])
        ToolSvc += electron_relink
        aug_tools += [ electron_relink ]
    if doPhotons:
        photon_relink = DerivationFramework__TruthLinkRepointTool("PhMiniCollectionTruthLinkTool",
                                                                    RecoCollection="Photons", TargetCollections=["TruthMuons","TruthPhotons","TruthElectrons"])
        ToolSvc += photon_relink
        aug_tools += [ photon_relink ]
    if doMuons:
        muon_relink = DerivationFramework__TruthLinkRepointTool("MuMiniCollectionTruthLinkTool",
                                                                    RecoCollection="Muons", TargetCollections=["TruthMuons","TruthPhotons","TruthElectrons"])
        ToolSvc += muon_relink
        aug_tools += [ muon_relink ]
    kernel +=CfgMgr.DerivationFramework__DerivationKernel("MiniCollectionTruthLinkKernel",
                                                          AugmentationTools = aug_tools )


def addTruth3ContentToSlimmerTool(slimmer):
    slimmer.AllVariables += [
        "MET_Truth",
        "TruthElectrons",
        "TruthMuons",
        "TruthPhotons",
        "TruthTaus",
        "TruthNeutrinos",
        "TruthBSM",
        "TruthBottom",
        "TruthTop",
        "TruthBoson",
        "TruthForwardProtons",
        "BornLeptons",
        "TruthBosonsWithDecayParticles",
        "TruthBosonsWithDecayVertices",
        "TruthBSMWithDecayParticles",
        "TruthBSMWithDecayVertices",
        "HardScatterParticles",
        "HardScatterVertices",
    ]
    slimmer.ExtraVariables += [
        "AntiKt4TruthDressedWZJets.GhostCHadronsFinalCount.GhostBHadronsFinalCount.pt.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TrueFlavor",
        "AntiKt10TruthSoftDropBeta100Zcut10Jets.pt.Tau1_wta.Tau2_wta.Tau3_wta.D2",
        "TruthEvents.Q.XF1.XF2.PDGID1.PDGID2.PDFID1.PDFID2.X1.X2.crossSection"]
