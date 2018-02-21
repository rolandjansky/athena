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
from DerivationFrameworkTau.TauTruthCommon import scheduleTauTruthTools
scheduleTauTruthTools()
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
  "2e.*",
  "mu.*",
  "2mu.*",
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
    ConeSize                = 0.4)
ToolSvc += EXOT5ElectronCCThinningTool
thinningTools.append(EXOT5ElectronCCThinningTool)

# Keep clusters associated to photons
EXOT5PhotonCCThinningTool = DerivationFramework__CaloClusterThinning(
    name                    = 'EXOT5PhotonCCThinningTool',
    ThinningService         = EXOT5ThinningHelper.ThinningSvc(),
    SGKey                   = 'Photons',
    CaloClCollectionSGKey   = 'egammaClusters',
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
    'HLT_noalg_L1J400',
    # new triggers for 2017
    'HLT_xe90_pufit_L1XE50',
    'HLT_xe100_pufit_L1XE55',
    'HLT_xe100_pufit_L1XE50',
    'HLT_xe110_pufit_L1XE55',
    'HLT_xe110_pufit_L1XE50',
    'HLT_e300_etcut',
    'HLT_g300_etcut',
    'HLT_mu60_0eta105_msonly',
    ]
lepton_triggers = [
   # el - y2015
   "HLT_e24_lhmedium_L1EM20VH",
   "HLT_e60_lhmedium",
   "HLT_e120_lhloose",
   "HLT_2e12_lhloose_L12EM10VH",
   # el - y2016
   "HLT_e24_lhtight_nod0_ivarloose",
   "HLT_e24_lhmedium_nod0_L1EM20VH",
   "HLT_e26_lhtight_nod0_ivarloose",
   "HLT_e60_lhmedium_nod0",
   "HLT_e60_medium",
   "HLT_e140_lhloose_nod0",
   "HLT_e300_etcut",
   "HLT_2e15_lhvloose_nod0_L12EM13VH",
   "HLT_2e17_lhvloose_nod0",
   # el - y2017
   "HLT_e26_lhtight_nod0_ivarloose_L1EM22VHIM",
   "HLT_e28_lhtight_nod0_ivarloose",
   "HLT_e28_lhtight_nod0_ivarloose_L1EM24VHIM",
   "HLT_e60_lhmedium_nod0_L1EM24VHI",
   "HLT_e140_lhloose_nod0_L1EM24VHI",
   "HLT_e300_etcut_L1EM24VHI",

   # mu - y2015
   "HLT_mu20_iloose_L1MU15",
   "HLT_mu40",
   "HLT_mu60_0eta105_msonly",
   "HLT_2mu10",
   "HLT_mu18_mu8noL1",
   # mu - y2016
   "HLT_mu24_iloose",
   "HLT_mu24_iloose_L1MU15",
   "HLT_mu24_ivarloose",
   "HLT_mu24_ivarloose_L1MU15",
   "HLT_mu24_ivarmedium",
   "HLT_mu24_imedium",
   "HLT_mu26_ivarmedium",
   "HLT_mu26_imedium",
   "HLT_mu50",
   "HLT_2mu10_nomucomb",
   "HLT_2mu14",
   "HLT_2mu14_nomucomb",
   "HLT_mu20_mu8noL1",
   "HLT_mu20_nomucomb_mu6noL1_nscan03",
   "HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb",
   "HLT_mu22_mu8noL1",
   # mu - y2017
   "HLT_mu60",
   "HLT_mu22_mu8noL1_calotag_0eta010",
   "HLT_mu24_mu8noL1",
   "HLT_mu24_mu8noL1_calotag_0eta010",
   "HLT_mu26_mu10noL1"
    ]

expression = ' || '.join(triggers+lepton_triggers)

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
applyJetCalibration_xAODColl("AntiKt4EMPFlow", exot5Seq)

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
    'MET_Reference_AntiKt4EMPFlow',
    'BTagging_AntiKt4EMTopo',
    'BTagging_AntiKt4EMPFlow',
    'AntiKt4EMTopoJets',
    'AntiKt4EMPFlowJets',
    ]

EXOT5SlimmingHelper.ExtraVariables = [
    'AntiKt4EMTopoJets.ConstituentScale.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m.InputType.AlgorithmType.SizeParameter.btaggingLink.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt.JetGhostArea.JetLCScaleMomentum_eta.JetLCScaleMomentum_m.JetLCScaleMomentum_phi.JetLCScaleMomentum_pt.LArBadHVEnergyFrac.LArBadHVNCell.LeadingClusterCenterLambda.LeadingClusterPt.LeadingClusterSecondLambda.LeadingClusterSecondR.N90Constituents.NegativeE.NumTrkPt1000.NumTrkPt500.OotFracClusters10.OotFracClusters5.OriginCorrected.OriginVertex.PartonTruthLabelID.PileupCorrected.SumPtTrkPt1000.SumPtTrkPt500.Timing.TrackSumMass.TrackSumPt.TrackWidthPt1000.TrackWidthPt500.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.Width.WidthPhi.ActiveArea.AverageLArQF.BchCorrCell.CentroidR.Charge.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.ConeTruthLabelID.DFCommonJets_Calib_eta.DFCommonJets_Calib_m.DFCommonJets_Calib_phi.DFCommonJets_Calib_pt.DetectorEta.DetectorPhi.ECPSFraction.EMFrac.EnergyPerSampling.FracSamplingMax.FracSamplingMaxIndex.GhostAntiKt2TrackJet.GhostAntiKt2TrackJetCount.GhostAntiKt2TrackJetPt.GhostAntiKt4TrackJet.GhostAntiKt4TrackJetCount.GhostAntiKt4TrackJetPt.GhostBHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostBHadronsInitial.GhostBHadronsInitialCount.GhostBHadronsInitialPt.GhostBQuarksFinal.GhostBQuarksFinalCount.GhostBQuarksFinalPt.GhostCHadronsFinal.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostCHadronsInitial.GhostCHadronsInitialCount.GhostCHadronsInitialPt.GhostCQuarksFinal.GhostCQuarksFinalCount.GhostCQuarksFinalPt.GhostHBosons.GhostHBosonsCount.GhostHBosonsPt.GhostMuonSegment.GhostMuonSegmentCount.GhostPartons.GhostPartonsCount.GhostPartonsPt.GhostTQuarksFinal.GhostTQuarksFinalCount.GhostTQuarksFinalPt.GhostTausFinal.GhostTausFinalCount.GhostTausFinalPt.GhostTrack.GhostTrackAssociationFraction.GhostTrackAssociationLink.GhostTrackCount.GhostTrackPt.GhostTruth.GhostTruthAssociationFraction.GhostTruthAssociationLink.GhostTruthCount.GhostTruthPt.GhostWBosons.GhostWBosonsCount.GhostWBosonsPt.GhostZBosons.GhostZBosonsCount.GhostZBosonsPt.HECFrac.HECQuality.HadronConeExclExtendedTruthLabelID.HadronConeExclTruthLabelID.HighestJVFVtx',
    'Electrons.author.Medium.Tight.Loose.charge',
    'MET_Track.name.mpx.mpy.source.sumet',
    'Muons.ptcone20.ptcone30.ptcone40.etcone20.etcone30.etcone40',
    'Photons.author.Loose.Tight',
    'TauJets.TruthCharge.TruthProng.IsTruthMatched.TruthPtVis.truthOrigin.truthType.truthParticleLink.truthJetLink',
    'AntiKt4EMPFlowJets.btaggingLink.GhostTruthAssociationLink.HadronConeExclTruthLabelID.PartonTruthLabelID.Timing.JetEtaJESScaleMomentum_eta.JetEtaJESScaleMomentum_m.JetEtaJESScaleMomentum_phi.JetEtaJESScaleMomentum_pt',
    'LVL1EmTauRoIs.eta.phi.roiWord.etScale.emIsol.hadIsol',
    'LVL1EnergySumRoI.energyX.energyY',
    'LVL1JetRoIs.eta.phi.et8x8',
    'LVL1MuonRoIs.eta.phi.thrValue',
    'Muons.EnergyLoss.energyLossType',    
    'TriggerMenu.smk.l1psk.hltpsk.itemCtpIds.itemNames.itemPrescales.chainIds.chainNames.chainParentNames.chainPrescales.chainRerunPrescales.chainPassthroughPrescales.chainSignatureCounters.chainSignatureLogics.chainSignatureOutputTEs.sequenceInputTEs.sequenceOutputTEs.sequenceAlgorithms.bunchGroupBunches',
    'xTrigDecision.bgCode.tav.tap.tbp.lvl2PassedRaw.efPassedRaw.lvl2PassedThrough.efPassedThrough.lvl2Prescaled.efPrescaled.lvl2Resurrected.efResurrected'
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
    EXOT5SlimmingHelper.ExtraVariables += [
        'AntiKt4TruthJets.pt.eta.phi.m.ConstituentScale.JetConstitScaleMomentum_pt.JetConstitScaleMomentum_eta.JetConstitScaleMomentum_phi.JetConstitScaleMomentum_m.InputType.AlgorithmType.SizeParameter.JetGhostArea.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.Width.WidthPhi.ActiveArea.ActiveArea4vec_eta.ActiveArea4vec_m.ActiveArea4vec_phi.ActiveArea4vec_pt.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.ConeTruthLabelID.GhostBHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostBHadronsInitial.GhostBHadronsInitialCount.GhostBHadronsInitialPt.GhostBQuarksFinal.GhostBQuarksFinalCount.GhostBQuarksFinalPt.GhostCHadronsFinal.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostCHadronsInitial.GhostCHadronsInitialCount.GhostCHadronsInitialPt.GhostCQuarksFinal.GhostCQuarksFinalCount.GhostCQuarksFinalPt.GhostHBosons.GhostHBosonsCount.GhostHBosonsPt.GhostPartons.GhostPartonsCount.GhostPartonsPt.GhostTQuarksFinal.GhostTQuarksFinalCount.GhostTQuarksFinalPt.GhostTausFinal.GhostTausFinalCount.GhostTausFinalPt.GhostWBosons.GhostWBosonsCount.GhostWBosonsPt.GhostZBosons.GhostZBosonsCount.GhostZBosonsPt.HadronConeExclExtendedTruthLabelID.HadronConeExclTruthLabelID', 
        'MET_Truth.name.mpx.mpy.source.sumet', 
        'TruthEvents.truthParticleLinks.truthVertexLinks.signalProcessVertexLink.beamParticle1Link.beamParticle2Link.crossSectionError.weights.PDFID1.PDFID2.PDGID1.PDGID2.Q.X1.X2.XF1.XF2.crossSection',
        'TruthParticles.px.py.pz.pdgId.status.e.barcode.prodVtxLink'
        'AntiKt4EMPFlowJets.GhostTruthAssociationLink.HadronConeExclTruthLabelID.PartonTruthLabelID',
        'TruthVertices.incomingParticleLinks'
        ]

EXOT5SlimmingHelper.UserContent = []
EXOT5SlimmingHelper.IncludeMuonTriggerContent = True
EXOT5SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT5SlimmingHelper.IncludeJetTriggerContent = True
EXOT5SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT5SlimmingHelper.IncludeEtMissTriggerContent = True
EXOT5SlimmingHelper.AppendContentToStream(EXOT5Stream)
