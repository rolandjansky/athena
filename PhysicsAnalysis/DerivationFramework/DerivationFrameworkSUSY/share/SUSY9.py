#********************************************************************
# SUSY9.py 
# reductionConf flag SUSY9 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
#if DerivationFrameworkIsMonteCarlo:
#    from DerivationFrameworkMCTruth.MCTruthCommon import *
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY9Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY9Stream )
SUSY9Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY9Stream.AcceptAlgs(["SUSY9KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY9ThinningHelper = ThinningHelper( "SUSY9ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY9 = CfgMgr.AthSequencer("SeqSUSY9")
DerivationFrameworkJob += SeqSUSY9


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY9TriggerList import triggersNavThin
SUSY9ThinningHelper.TriggerChains = '|'.join(triggersNavThin)

SUSY9ThinningHelper.AppendToStream( SUSY9Stream )
     
#====================================================================
# THINNING TOOLS 
#====================================================================

# Jet tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
SUSY9JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "SUSY9JetTPThinningTool",
                                                                         ThinningService         = SUSY9ThinningHelper.ThinningSvc(),
                                                                         JetKey                  = "AntiKt4EMTopoJets",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                       )
ToolSvc += SUSY9JetTPThinningTool
thinningTools.append(SUSY9JetTPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY9MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY9MuonTPThinningTool",
                                                                          ThinningService         = SUSY9ThinningHelper.ThinningSvc(),
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY9MuonTPThinningTool
thinningTools.append(SUSY9MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY9ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY9ElectronTPThinningTool",
                                                                                ThinningService        = SUSY9ThinningHelper.ThinningSvc(),
                                                                                SGKey             	= "Electrons",
                                                                                InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY9ElectronTPThinningTool
thinningTools.append(SUSY9ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY9PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY9PhotonTPThinningTool",
                                                                              ThinningService	 = SUSY9ThinningHelper.ThinningSvc(),
                                                                              SGKey			 = "Photons",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY9PhotonTPThinningTool
thinningTools.append(SUSY9PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY9TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY9TauTPThinningTool",
                                                                         ThinningService = SUSY9ThinningHelper.ThinningSvc(),
                                                                         TauKey          = "TauJets",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY9TauTPThinningTool
thinningTools.append(SUSY9TauTPThinningTool)

# Cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning

# Keep clusters associated to electrons
SUSY9ElectronCCThinningTool = DerivationFramework__CaloClusterThinning(
    name                    = 'SUSY9ElectronCCThinningTool',
    ThinningService         = SUSY9ThinningHelper.ThinningSvc(),
    SGKey                   = 'Electrons',
    CaloClCollectionSGKey   = 'egammaClusters',
    TopoClCollectionSGKey   = 'CaloCalTopoClusters',
    ConeSize                = 0.2)
ToolSvc += SUSY9ElectronCCThinningTool
thinningTools.append(SUSY9ElectronCCThinningTool)

# Keep clusters associated to photons
SUSY9PhotonCCThinningTool = DerivationFramework__CaloClusterThinning(
    name                    = 'SUSY9PhotonCCThinningTool',
    ThinningService         = SUSY9ThinningHelper.ThinningSvc(),
    SGKey                   = 'Photons',
    CaloClCollectionSGKey   = 'egammaClusters',
    TopoClCollectionSGKey   = 'CaloCalTopoClusters',
    ConeSize                = 0.2)
ToolSvc += SUSY9PhotonCCThinningTool
thinningTools.append(SUSY9PhotonCCThinningTool)


# Calo Clusters associated with Muons
SUSY9MuonCCThinningTool = DerivationFramework__CaloClusterThinning( 
    name = "SUSYxMuonCCThinningTool",
    ThinningService         = SUSY9ThinningHelper.ThinningSvc(),
    SGKey                   = "Muons",
    TopoClCollectionSGKey   = "CaloCalTopoClusters",
    SelectionString         = "Muons.pt > 3*GeV",
    ConeSize                = 0.2)
ToolSvc += SUSY9MuonCCThinningTool
thinningTools.append(SUSY9MuonCCThinningTool)


#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    SUSY9TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY9TruthThinningTool",
                                                                     ThinningService              = SUSY9ThinningHelper.ThinningSvc(),
                                                                     WritePartons                 = False,
                                                                     WriteHadrons                 = False,
                                                                     WriteBHadrons                = True,
                                                                     WriteGeant                   = False,
                                                                     GeantPhotonPtThresh          = 20000,
                                                                     WriteTauHad                  = True,
                                                                     PartonPtThresh               = -1.0,
                                                                     WriteBSM                     = True,
                                                                     WriteBosons                  = True,
                                                                     WriteBosonProducts           = True,
                                                                     WriteBSMProducts             = True,
                                                                     WriteTopAndDecays            = True,
                                                                     WriteEverything              = False,
                                                                     WriteAllLeptons              = False,
                                                                     WriteLeptonsNotFromHadrons   = False,
                                                                     WriteStatus3                 = False,
                                                                     WriteFirstN                  = -1,
                                                                     PreserveAncestors            = True,
                                                                     PreserveGeneratorDescendants = False,
                                                                     SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)

   
    # Decorate Electron with bkg electron type/origin 
    from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
    from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification 
    BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool",MCTruthClassifierTool = BkgElectronMCTruthClassifier)
    ToolSvc += BkgElectronClassificationTool
    AugmentationTools.append(BkgElectronClassificationTool)
    
    ToolSvc += SUSY9TruthThinningTool
    thinningTools.append(SUSY9TruthThinningTool)

	
#====================================================================
# SKIMMING TOOL 
#====================================================================

# jet_prefab = 'count(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > XPT*GeV && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < XETA))'
# count_jet_hard = jet_prefab.replace("XPT", "150.").replace("XETA", "2.8")
# count_jet_soft = jet_prefab.replace("XPT", "20.").replace("XETA", "2.8")
# jetSelection= '('+count_jet_soft+'>=1)'

# ------------------------------------------------------------
# Lepton selection
muonsRequirements = '(Muons.pt > 4*GeV) && (abs(Muons.eta) < 2.5) && (Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 4*GeV) && (abs(Electrons.eta) < 2.5) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHVeryLoose))'
leptonSelection = '(count('+electronsRequirements+') >=2 || count('+muonsRequirements+') >= 2)'
#leptonSelection = '(count('+muonsRequirements+') >= 2)'


expression='('+leptonSelection+')'
#expression='('+leptonSelection+'&&'+jetSelection+')'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY9LeptonSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY9LeptonSkimmingTool",
                                                                        expression = expression)
ToolSvc += SUSY9LeptonSkimmingTool
# ------------------------------------------------------------

# ------------------------------------------------------------
# Trigger selection
from DerivationFrameworkSUSY.SUSY9TriggerList import triggersBPhys
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool,DerivationFramework__FilterCombinationOR,DerivationFramework__FilterCombinationAND


# JetMET trigger name contained ' - ' cause crash when using xAODStringSkimmingTool
SUSY9InclusiveTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY9InclusiveTriggerSkimmingTool",
                                                                               TriggerListOR = triggersBPhys)
ToolSvc += SUSY9InclusiveTriggerSkimmingTool

    
# ------------------------------------------------------------

# ------------------------------------------------------------
# Final skim selection, with trigger selection and lepton selection
SUSY9SkimmingTool = DerivationFramework__FilterCombinationAND(name = "SUSY9SkimmingTool", 
                                                               FilterList = [SUSY9LeptonSkimmingTool, 
                                                                             SUSY9InclusiveTriggerSkimmingTool])
ToolSvc += SUSY9SkimmingTool
# ------------------------------------------------------------

#====================================================================
# Max Cell sum decoration tool
#====================================================================

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__MaxCellDecorator
SUSY9_MaxCellDecoratorTool = DerivationFramework__MaxCellDecorator( name                    = "SUSY9_MaxCellDecoratorTool",
                                                                     SGKey_electrons         = "Electrons",
                                                                     SGKey_photons           = "Photons",
                                                                     )
ToolSvc += SUSY9_MaxCellDecoratorTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *


#==============================================================================
# SUSY signal augmentation (before skimming!)
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
if IsSUSYSignal():
    
    from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
    SeqSUSY9 += CfgMgr.DerivationFramework__DerivationKernel("SUSY9KernelSigAug",
                                                              AugmentationTools = DecorateSUSYProcess("SUSY9")
                                                              )

    from DerivationFrameworkSUSY.SUSYWeightMetadata import *

#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY9 += CfgMgr.DerivationFramework__DerivationKernel(
    "SUSY9KernelSkim",
    SkimmingTools = [SUSY9SkimmingTool]
    )


#==============================================================================
# Jet building
#==============================================================================
OutputJets["SUSY9"] = []
reducedJetList = [ "AntiKt2PV0TrackJets" ]
#if DerivationFrameworkIsMonteCarlo:
#  reducedJetList += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]

# AntiKt2PV0TrackJets is flavour-tagged automatically
replaceAODReducedJets(reducedJetList, SeqSUSY9, "SUSY9")


#==============================================================================
# Tau truth building/matching
#==============================================================================
#if DerivationFrameworkIsMonteCarlo:
#  from DerivationFrameworkSUSY.SUSYTruthCommon import addTruthTaus
#  addTruthTaus(AugmentationTools)
    

#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY9 += CfgMgr.DerivationFramework__DerivationKernel(
    "SUSY9KernelAug",
    AugmentationTools = AugmentationTools,
    ThinningTools = thinningTools,
    )


#====================================================================
# CONTENT LIST  
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY9SlimmingHelper = SlimmingHelper("SUSY9SlimmingHelper")
SUSY9SlimmingHelper.SmartCollections = ["Electrons",
                                         "Photons",
                                         "Muons",
                                         "TauJets",
                                         "AntiKt4EMTopoJets",
                                         "MET_Reference_AntiKt4EMTopo",
                                         "BTagging_AntiKt4EMTopo",
                                         "InDetTrackParticles",
                                         "PrimaryVertices"]
SUSY9SlimmingHelper.AllVariables = ["TruthParticles", 
                                     "TruthEvents", 
                                     "TruthVertices", 
                                     #"AntiKt4TruthJets",
                                     "MET_Track",
                                     "MET_LocHadTopo",
                                     "MuonSegments",
                                     "MET_Truth"]					
SUSY9SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
                                      "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag.eta_sampl.phi_sampl",
                                      "Photons.author.Loose.Tight",
                                      "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi",
                                      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix","CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink"
                                      + ".PanTau_isPanTauCandidate.ptPanTauCellBased.etaPanTauCellBased.phiPanTauCellBased.mPanTauCellBased",
                                      "TauNeutralParticleFlowObjects.pt.eta.phi.m",
                                      "TauChargedParticleFlowObjects.pt.eta.phi.m",
                                      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                                      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                                      "Electrons.bkgTruthType.bkgTruthOrigin.firstEgMotherPdgId.truthPdgId.bkgTruthParticleLink.bkgMotherPdgId.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherTruthParticleLink",
                                      "AntiKt2PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
                                      "BTagging_AntiKt2Track.MV2c10_discriminant",
                                      "CaloCalTopoClusters.rawE.rawEta.rawPhi.rawM.calE.calEta.calPhi.calM.e_sampl",
                                      "MuonClusterCollection.eta_sampl.phi_sampl"
]

SUSY9SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY9SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY9SlimmingHelper.IncludeBPhysTriggerContent  = True
SUSY9SlimmingHelper.IncludeJetTriggerContent    = False
SUSY9SlimmingHelper.IncludeTauTriggerContent    = False
SUSY9SlimmingHelper.IncludeEtMissTriggerContent = False
SUSY9SlimmingHelper.IncludeBJetTriggerContent   = False

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY9SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}
  
  SUSY9SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]   


SUSY9SlimmingHelper.AppendContentToStream(SUSY9Stream)


