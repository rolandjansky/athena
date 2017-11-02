#********************************************************************
# EXOT5.py
# reductionConf flag EXOT5 in Reco_tf.py
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauTruthCommon import *
from DerivationFrameworkCore.WeightMetadata import *

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

#====================================================================
# SET UP STREAM
#====================================================================
stream_name = derivationFlags.WriteDAOD_EXOT5Stream.StreamName
file_name = buildFileName(derivationFlags.WriteDAOD_EXOT5Stream)
EXOT5Stream = MSMgr.NewPoolRootStream(stream_name, file_name)
EXOT5Stream.AcceptAlgs(['EXOT5Kernel'])

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT5ThinningHelper = ThinningHelper('EXOT5ThinningHelper')
thinningTools = []
skimmingTools = []
augmentationTools = []

# stream-specific sequence
exot5Seq = CfgMgr.AthSequencer('EXOT5Sequence')
DerivationFrameworkJob += exot5Seq

#====================================================================
# Trigger navigation thinning
#====================================================================
triggerRegEx = [
  ".?j.*",
  "xe.*",
  "e.*",
  "mu.*",
  "tau.*",
  "g.*"
]
EXOT5ThinningHelper.TriggerChains = 'HLT_(' + ' | '.join(triggerRegEx) + ')'
EXOT5ThinningHelper.AppendToStream(EXOT5Stream)

#====================================================================
# THINNING TOOLS
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import (
    DerivationFramework__MuonTrackParticleThinning,
    DerivationFramework__EgammaTrackParticleThinning,
    DerivationFramework__TauTrackParticleThinning,
    DerivationFramework__JetTrackParticleThinning,
    DerivationFramework__TrackParticleThinning)

# Keep tracks associated with muons
EXOT5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
    name                    = 'EXOT5MuonTPThinningTool',
    ThinningService         = EXOT5ThinningHelper.ThinningSvc(),
    MuonKey                 = 'Muons',
    InDetTrackParticlesKey  = 'InDetTrackParticles')
ToolSvc += EXOT5MuonTPThinningTool
thinningTools.append(EXOT5MuonTPThinningTool)

# Keep tracks associated with electrons
EXOT5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
    name                    = 'EXOT5ElectronTPThinningTool',
    ThinningService         = EXOT5ThinningHelper.ThinningSvc(),
    SGKey                   = 'Electrons',
    GSFTrackParticlesKey    = 'GSFTrackParticles',
    InDetTrackParticlesKey  = 'InDetTrackParticles')
ToolSvc += EXOT5ElectronTPThinningTool
thinningTools.append(EXOT5ElectronTPThinningTool)

# Keep tracks associated with photons
EXOT5PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
    name                    = 'EXOT5PhotonTPThinningTool',
    ThinningService         =  EXOT5ThinningHelper.ThinningSvc(),
    SGKey                   = 'Photons',
    InDetTrackParticlesKey  = 'InDetTrackParticles')
ToolSvc += EXOT5PhotonTPThinningTool
thinningTools.append(EXOT5PhotonTPThinningTool)

# Keep tracks associated with taus
EXOT5TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(
    name                    = 'EXOT5TauTPThinningTool',
    ThinningService         = EXOT5ThinningHelper.ThinningSvc(),
    TauKey                  = 'TauJets',
    InDetTrackParticlesKey  = 'InDetTrackParticles')
ToolSvc += EXOT5TauTPThinningTool
thinningTools.append(EXOT5TauTPThinningTool)

# Keep tracks associated with jets
#EXOT5JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(
    #name                    = 'EXOT5JetTPThinningTool',
    #ThinningService         = EXOT5ThinningHelper.ThinningSvc(),
    #JetKey                  = 'AntiKt4EMTopoJets',
    #InDetTrackParticlesKey  = 'InDetTrackParticles',
    #SelectionString         = 'AntiKt4EMTopoJets.pt > 20*GeV')
#ToolSvc += EXOT5JetTPThinningTool
#thinningTools.append(EXOT5JetTPThinningTool)

# Keep tracks with pT > 10 GeV for isolated track veto
EXOT5TPThinningTool = DerivationFramework__TrackParticleThinning(
    name                    = 'EXOT5TPThinningTool',
    ThinningService         = EXOT5ThinningHelper.ThinningSvc(),
    SelectionString         = 'InDetTrackParticles.pt > 10*GeV',
    InDetTrackParticlesKey  = 'InDetTrackParticles')
ToolSvc += EXOT5TPThinningTool
thinningTools.append(EXOT5TPThinningTool)

# Cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning

# Keep clusters associated to electrons
EXOT5ElectronCCThinningTool = DerivationFramework__CaloClusterThinning(
    name                    = 'EXOT5ElectronCCThinningTool',
    ThinningService         = EXOT5ThinningHelper.ThinningSvc(),
    SGKey                   = 'Electrons',
    CaloClCollectionSGKey   = 'egammaClusters',
    TopoClCollectionSGKey   = 'CaloCalTopoClusters',
    ConeSize                = 0.4)
ToolSvc += EXOT5ElectronCCThinningTool
thinningTools.append(EXOT5ElectronCCThinningTool)

# Keep clusters associated to photons
EXOT5PhotonCCThinningTool = DerivationFramework__CaloClusterThinning(
    name                    = 'EXOT5PhotonCCThinningTool',
    ThinningService         = EXOT5ThinningHelper.ThinningSvc(),
    SGKey                   = 'Photons',
    CaloClCollectionSGKey   = 'egammaClusters',
    TopoClCollectionSGKey   = 'CaloCalTopoClusters',
    ConeSize                = 0.4)
ToolSvc += EXOT5PhotonCCThinningTool
thinningTools.append(EXOT5PhotonCCThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
    from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier

    EXOT5Classifier = MCTruthClassifier(
        name                      = 'EXOT5Classifier',
        ParticleCaloExtensionTool = '')
    ToolSvc += EXOT5Classifier

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthClassificationDecorator

    EXOT5ClassificationDecorator = DerivationFramework__TruthClassificationDecorator(
        name              = 'EXOT5ClassificationDecorator',
        ParticlesKey      = 'TruthParticles',
        MCTruthClassifier = EXOT5Classifier)
    ToolSvc += EXOT5ClassificationDecorator
    augmentationTools.append(EXOT5ClassificationDecorator)

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning

    EXOT5MCThinningTool = DerivationFramework__MenuTruthThinning(
        name                       = 'EXOT5MCThinningTool',
        ThinningService            = 'EXOT5ThinningSvc',
        WriteEverything            = False,
        WritePartons               = False,
        PartonPtThresh             = -1.0,
        WriteHadrons               = False,
        WriteBHadrons              = False,
        WriteGeant                 = False,
        GeantPhotonPtThresh        = 10000,
        WriteTauHad                = True,
        WriteBSM                   = True,
        WriteBosons                = True,
        WriteBSMProducts           = True,
        WriteTopAndDecays          = True,
        WriteAllLeptons            = True,
        #WriteLeptonsNotFromHadrons = True,
        WriteStatus3               = False,
        WriteFirstN                = -1,
        PreserveAncestors          = True,
        PreserveGeneratorDescendants = False,
        SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset,
        )
    ToolSvc += EXOT5MCThinningTool
    thinningTools.append(EXOT5MCThinningTool)

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker

    EXOT5TruthElectronTool = DerivationFramework__TruthCollectionMaker(
        name                    = 'EXOT5TruthElectronTool',
        NewCollectionName       = 'EXOT5TruthElectrons',
        ParticleSelectionString = '(abs(TruthParticles.pdgId) == 11) && '
                                  '(TruthParticles.status == 1) && '
                                  '(TruthParticles.barcode < %d)' % DerivationFrameworkSimBarcodeOffset)
    ToolSvc += EXOT5TruthElectronTool
    augmentationTools.append(EXOT5TruthElectronTool)

    EXOT5TruthMuonTool = DerivationFramework__TruthCollectionMaker(
        name                    = 'EXOT5TruthMuonTool',
        NewCollectionName       = 'EXOT5TruthMuons',
        ParticleSelectionString = '(abs(TruthParticles.pdgId) == 13) && '
                                  '(TruthParticles.status == 1) && '
                                  '(TruthParticles.barcode < %d)' % DerivationFrameworkSimBarcodeOffset)
    ToolSvc += EXOT5TruthMuonTool
    augmentationTools.append(EXOT5TruthMuonTool)

    EXOT5TruthNeutrinoTool = DerivationFramework__TruthCollectionMaker(
        name                    = 'EXOT5TruthNeutrinoTool',
        NewCollectionName       = 'EXOT5TruthNeutrinos',
        ParticleSelectionString = '(abs(TruthParticles.pdgId) == 12 || '
                                  'abs(TruthParticles.pdgId) == 14 || '
                                  'abs(TruthParticles.pdgId) == 16) && '
                                  '(TruthParticles.status == 1) && '
                                  '(TruthParticles.barcode < %d)' % DerivationFrameworkSimBarcodeOffset)
    ToolSvc += EXOT5TruthNeutrinoTool
    augmentationTools.append(EXOT5TruthNeutrinoTool)

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthDressingTool

    EXOT5TruthElectronDressingTool = DerivationFramework__TruthDressingTool(
        name                  = 'EXOT5TruthElectronDressingTool',
        dressParticlesKey     = 'EXOT5TruthElectrons',
        usePhotonsFromHadrons = False,
        dressingConeSize      = 0.1,
        particleIDsToDress    = [11])
    ToolSvc += EXOT5TruthElectronDressingTool
    augmentationTools.append(EXOT5TruthElectronDressingTool)

    EXOT5TruthMuonDressingTool = DerivationFramework__TruthDressingTool(
        name                  = 'EXOT5TruthMuonDressingTool',
        dressParticlesKey     = 'EXOT5TruthMuons',
        usePhotonsFromHadrons = False,
        dressingConeSize      = 0.1,
        particleIDsToDress    = [13])
    ToolSvc += EXOT5TruthMuonDressingTool
    augmentationTools.append(EXOT5TruthMuonDressingTool)

#====================================================================
# SKIMMING TOOLS
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkExotics.DerivationFrameworkExoticsConf import DerivationFramework__SkimmingToolEXOT5

# if DerivationFrameworkIsMonteCarlo:
#     ToolSvc += CfgMgr.JetCalibrationTool('EXOT5JESTool',
#         IsData        = False,
#         ConfigFile    = 'JES_MC15cRecommendation_May2016.config',
#         CalibSequence = 'JetArea_Residual_Origin_EtaJES_GSC',
#         JetCollection = 'AntiKt4EMTopo')
# else:
#     ToolSvc += CfgMgr.JetCalibrationTool('EXOT5JESTool',
#         IsData        = True,
#         ConfigFile    = 'JES_MC15cRecommendation_May2016.config',
#         CalibSequence = 'JetArea_Residual_Origin_EtaJES_GSC_Insitu',
#         JetCollection = 'AntiKt4EMTopo')

triggers = [
    # MET
    'HLT_xe35',
    'HLT_xe50',
    'HLT_xe60',
    'HLT_xe70',
    'HLT_xe80',
    'HLT_xe100',
    # Single electron
    'HLT_e24_lhmedium_L1EM18VH',
    'HLT_e24_lhmedium_L1EM20VH',
    'HLT_e60_lhmedium',
    'HLT_e120_lhloose',
    # Single muon
    'HLT_mu20_iloose_L1MU15',
    'HLT_mu26_imedium',
    'HLT_mu50',
    # Photons
    'HLT_g140_loose',
    # Triggers successfully running in Run 2 throughout 50 ns
    'HLT_j30_xe10_razor100',
    'HLT_j30_xe10_razor170',
    'HLT_j30_xe10_razor185',
    'HLT_j30_xe10_razor195',
    # Additional triggers added for 25 ns / higher rate
    'HLT_j30_xe60_razor100',
    'HLT_j30_xe60_razor170',
    'HLT_j30_xe60_razor185',
    'HLT_j30_xe60_razor195',
    # VBF
    'HLT_2j40_0eta490_invm250_L1XE55',
    # new triggers for 2016
    'HLT_xe80_tc_lcw_L1XE50',
    'HLT_xe90_tc_lcw_L1XE50',
    'HLT_xe90_mht_L1XE50',
    'HLT_xe100_mht_L1XE50',
    'HLT_xe100_mht_L1XE55',
    'HLT_xe100_mht_L1XE60',
    'HLT_xe100_tc_lcw_L1XE50',
    'HLT_xe110_mht_L1XE50',
    'HLT_xe120_mht_L1XE50',
    'HLT_xe130_mht_L1XE50',
    'HLT_xe140_mht_L1XE50',
    'HLT_xe100_L1XE50',
    'HLT_xe110_L1XE50',
    'HLT_xe120_L1XE50',
    'HLT_mu24_ivarmedium',
    'HLT_mu26_ivarmedium',
    'HLT_e24_lhtight_nod0_ivarloose',
    'HLT_e26_lhtight_ivarloose',
    'HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e60_lhmedium_nod0',
    'HLT_e60_medium',
    'HLT_e120_lhloose_nod0',
    'HLT_e140_lhloose_nod0',
    'HLT_g140_loose',
    'HLT_g160_loose',
    'HLT_g120_tight',
    'HLT_noalg_L1J400'
    ]
expression = ' || '.join(triggers)

if not DerivationFrameworkIsMonteCarlo:
    EXOT5StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(
        name='EXOT5StringSkimmingTool',
        expression=expression)
    ToolSvc += EXOT5StringSkimmingTool
    skimmingTools.append(EXOT5StringSkimmingTool)

EXOT5SkimmingTool = DerivationFramework__SkimmingToolEXOT5(
    name                = 'EXOT5SkimmingTool',
    JetContainer        = 'AntiKt4EMTopoJets',
    UncalibMonoJetPtCut = 100000.,
    MonoJetPtCut        = 100000.,
    LeadingJetPtCut     = 40000.,
    SubleadingJetPtCut  = 40000.,
    DiJetMassCut        = 150000.)
ToolSvc += EXOT5SkimmingTool
skimmingTools.append(EXOT5SkimmingTool)

#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT5"] = []
reducedJetList = [
    "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,exot5Seq,"EXOT5")

#jet calibration
applyJetCalibration_xAODColl("AntiKt4EMTopo", exot5Seq)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

exot5Seq += CfgMgr.DerivationFramework__DerivationKernel(
    'EXOT5Kernel_skim', SkimmingTools=skimmingTools)
exot5Seq += CfgMgr.DerivationFramework__DerivationKernel(
    'EXOT5Kernel', AugmentationTools=augmentationTools, ThinningTools=thinningTools)

#========================================
# Add the containers to the output stream
#========================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

EXOT5SlimmingHelper = SlimmingHelper('EXOT5SlimmingHelper')
EXOT5SlimmingHelper.SmartCollections = [
    'Electrons',
    'Muons',
    'Photons',
    'TauJets',
    'InDetTrackParticles',
    'PrimaryVertices',
    'MET_Reference_AntiKt4EMTopo',
    'BTagging_AntiKt4EMTopo',
    'AntiKt4EMTopoJets'
    ]
EXOT5SlimmingHelper.ExtraVariables = [
    'Photons.author.Loose.Tight',
    'Electrons.author.Medium.Tight.Loose.charge',
    'Muons.ptcone20.ptcone30.ptcone40.etcone20.etcone30.etcone40',
    'CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix', # SUSYTools
    'ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix', # SUSYTools
    'TauJets.TruthCharge.TruthProng.IsTruthMatched.TruthPtVis.truthOrigin.truthType.truthParticleLink.truthJetLink'
    ]
EXOT5SlimmingHelper.AllVariables = [
    'AntiKt4EMTopoJets',
    'AntiKt4TruthJets',
    'MET_Truth',
    'MET_Track',
    'MET_LocHadTopo',
    'TruthEvents',
    'TruthParticles',
    'MuonSegments',
    ]
if DerivationFrameworkIsMonteCarlo:
    EXOT5SlimmingHelper.StaticContent = [
        'xAOD::TruthParticleContainer#EXOT5TruthMuons',
        'xAOD::TruthParticleAuxContainer#EXOT5TruthMuonsAux.',
        'xAOD::TruthParticleContainer#EXOT5TruthElectrons',
        'xAOD::TruthParticleAuxContainer#EXOT5TruthElectronsAux.',
        'xAOD::TruthParticleContainer#EXOT5TruthNeutrinos',
        'xAOD::TruthParticleAuxContainer#EXOT5TruthNeutrinosAux.',
        'xAOD::TruthParticleContainer#TruthTaus',
        'xAOD::TruthParticleAuxContainer#TruthTausAux.',
        ]
EXOT5SlimmingHelper.UserContent = []
EXOT5SlimmingHelper.IncludeMuonTriggerContent = True
EXOT5SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT5SlimmingHelper.IncludeJetTriggerContent = True
EXOT5SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT5SlimmingHelper.IncludeEtMissTriggerContent = True
EXOT5SlimmingHelper.AppendContentToStream(EXOT5Stream)
