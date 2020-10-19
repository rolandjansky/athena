#********************************************************************
# SUSY5.py
# reductionConf flag SUSY5 in Reco_tf.py
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauCommon import *
if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addBSMAndDownstreamParticles,addHFAndDownstreamParticles,addPVCollection
  addStandardTruthContents()
  addMiniTruthCollectionLinks()
  addBSMAndDownstreamParticles()
  addHFAndDownstreamParticles()
  addPVCollection()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import *

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY5Stream )
SUSY5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY5Stream.AcceptAlgs(["SUSY5KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY5ThinningHelper = ThinningHelper( "SUSY5ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY5 = CfgMgr.AthSequencer("SeqSUSY5")
DerivationFrameworkJob += SeqSUSY5
applyJetCalibration_xAODColl('AntiKt4EMPFlow',SeqSUSY5)

# make Pixel and SCT conditions available
include ("InDetRecExample/PixelConditionsAccess.py") # include all pixel condtions avaliable in AOD /DT
include ("InDetRecExample/SCTConditionsAccess.py")

#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY5TriggerList import METorPhoton_triggers, Lepton_triggers
triggersNavThin = METorPhoton_triggers + Lepton_triggers
SUSY5ThinningHelper.TriggerChains = '|'.join(triggersNavThin)

SUSY5ThinningHelper.AppendToStream( SUSY5Stream )

#====================================================================
# THINNING TOOLS
#====================================================================

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY5MuonTPThinningTool",
                                                                         ThinningService         = SUSY5ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY5MuonTPThinningTool
thinningTools.append(SUSY5MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY5ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY5ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	= "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY5ElectronTPThinningTool
thinningTools.append(SUSY5ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY5PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY5PhotonTPThinningTool",
										 ThinningService	 = SUSY5ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY5PhotonTPThinningTool
thinningTools.append(SUSY5PhotonTPThinningTool)

# TrackParticles associated with taus, only keep charged tau tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY5TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "SUSY5TauTPThinningTool",
                                                                       ThinningService        = SUSY5ThinningHelper.ThinningSvc(),
                                                                       TauKey                 = "TauJets",
                                                                       InDetTrackParticlesKey = "InDetTrackParticles",
                                                                       SelectionString        = "(TauJets.ptFinalCalib >= 15.*GeV) && (TauJets.nTracks==1 || TauJets.nTracks==3)",
                                                                       ApplyAnd               = False,
                                                                       DoTauTracksThinning    = True,
                                                                       TauTracksKey           = "TauTracks")
ToolSvc += SUSY5TauTPThinningTool
thinningTools.append(SUSY5TauTPThinningTool)

# TauJets thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
SUSY5TauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "SUSY5TauJetsThinningTool",
                                                                      ThinningService = SUSY5ThinningHelper.ThinningSvc(),
                                                                      ContainerName   = "TauJets",
                                                                      SelectionString = "(TauJets.ptFinalCalib >= 15.*GeV) && (TauJets.nTracks==1 || TauJets.nTracks==3)")
ToolSvc += SUSY5TauJetsThinningTool
thinningTools.append(SUSY5TauJetsThinningTool)

# TrackParticles associated with SV
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__VertexParticleThinning
SUSY5VertexTPThinningTool = DerivationFramework__VertexParticleThinning( name                = "SUSY5VertexTPThinningTool",
                                                                      ThinningService        = SUSY5ThinningHelper.ThinningSvc(),
                                                                      VertexKey              = "VrtSecInclusive_SoftBtagCandidateVertices",
                                                                      InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY5VertexTPThinningTool
thinningTools.append(SUSY5VertexTPThinningTool)

# TrackParticles associated with LC jets: useful when the global track thinning has a pT threshold ~1-2 GeV
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
#SUSY5LCJetsTrackThinningTool = DerivationFramework__JetTrackParticleThinning( name            = "SUSY5LCJetsTrackThinningTool",
#                                                                              ThinningService = SUSY5ThinningHelper.ThinningSvc(),
#                                                                              JetKey          = "AntiKt4LCTopoJets",
#                                                                              SelectionString         = "AntiKt4LCTopoJets.pt > 18*GeV",
#                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY5LCJetsTrackThinningTool
#thinningTools.append(SUSY5LCJetsTrackThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkHasTruth:

  # Decorate Electron with bkg electron type/origin
  from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier
  from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
  BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool",MCTruthClassifierTool = BkgElectronMCTruthClassifier)
  ToolSvc += BkgElectronClassificationTool
  AugmentationTools.append(BkgElectronClassificationTool)


#====================================================================
# SKIMMING TOOL
#====================================================================

muonsRequirementsSL = '(Muons.pt >= 2.5*GeV) && (Muons.pt <= 25.*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonMuonsPreselection)'
electronsRequirementsSL = '(Electrons.pt > 4.*GeV) && (Electrons.pt <= 25.*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
objectSelectionSL = '(count('+electronsRequirementsSL+') + count('+muonsRequirementsSL+') >= 1)'

muonsRequirementsHL = '(Muons.pt >= 25.*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonMuonsPreselection)'
electronsRequirementsHL = '(Electrons.pt > 25.*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
objectSelectionHL = '(count('+electronsRequirementsHL+') + count('+muonsRequirementsHL+') >= 1)'

muonsRequirements = '(Muons.pt >= 2.5*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 4.*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
objectSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+') >= 1)'


trig_expression = '(' + ' || '.join(METorPhoton_triggers+Lepton_triggers) + ')'
MEttrig_expression ='(' + ' || '.join(METorPhoton_triggers) + ')'
Leptrig_expression = '(' + ' || '.join(Lepton_triggers) + ')'

JetEleExpression4j15 = '(count(AntiKt4EMPFlowJets.DFCommonJets_Calib_pt>15*GeV && abs(AntiKt4EMPFlowJets.DFCommonJets_Calib_eta)<2.8)>=4)'
JetEleExpression2j25 = '(count(AntiKt4EMPFlowJets.DFCommonJets_Calib_pt>25*GeV && abs(AntiKt4EMPFlowJets.DFCommonJets_Calib_eta)<2.8)>=2)'
JetEleHighExpression = '(count(AntiKt4EMPFlowJets.DFCommonJets_Calib_pt>200*GeV && abs(AntiKt4EMPFlowJets.DFCommonJets_Calib_eta)<2.8)>=1)'
LepTrigexpression = '('+'('+Leptrig_expression+'&&'+objectSelectionHL+'&&'+JetEleExpression4j15+')'+'||'+'('+trig_expression+'&&'+objectSelectionHL+'&&'+JetEleExpression2j25+')'+'||'+'('+MEttrig_expression +'&&'+ objectSelectionSL+'&&'+JetEleExpression2j25+')'+'||'+'('+MEttrig_expression +'&&'+ objectSelection+'&&'+JetEleHighExpression+')'+' )'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY5SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY5SkimmingTool",
                                                                 expression = LepTrigexpression)

ToolSvc += SUSY5SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY5 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY5KernelSkim",
  SkimmingTools = [SUSY5SkimmingTool]
)


#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY5)

## Adding decorations for fJVT PFlow jets
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqSUSY5, algname='JetForwardPFlowJvtToolAlg')
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=SeqSUSY5, algname='JetForwardJvtToolBDTAlg')

#==============================================================================
OutputJets["SUSY5"] = []
reducedJetList = [ "AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets" ]

replaceAODReducedJets(reducedJetList, SeqSUSY5, "SUSY5")

# AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets
addDefaultTrimmedJets(SeqSUSY5, "SUSY5")

#==============================================================================
# Tau truth building/matching
#==============================================================================
# now part of MCTruthCommon
if DerivationFrameworkHasTruth:
  ToolSvc.DFCommonTauTruthMatchingTool.WriteInvisibleFourMomentum = True

#==============================================================================
# Soft b-tag
#==============================================================================
from DerivationFrameworkFlavourTag.SoftBtagCommon import *
applySoftBtagging("softBtag", SeqSUSY5 )

#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY5 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY5KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)

# Add VR jets
addVRJets(SeqSUSY5)
addRecommendedXbbTaggers(SeqSUSY5, ToolSvc)

#====================================================================
# Prompt Lepton Tagger
#====================================================================
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
import LeptonTaggers.LeptonTaggersConfig as LepTagConfig


# simple call to replaceAODReducedJets(["AntiKt4PV0TrackJets"], SeqSUSY5, "SUSY5")
JetTagConfig.ConfigureAntiKt4PV0TrackJets(SeqSUSY5, "SUSY5")

# add decoration
SeqSUSY5 += JetTagConfig.GetDecoratePromptLeptonAlgs(addSpectators=True)
SeqSUSY5 += JetTagConfig.GetDecoratePromptTauAlgs()
SeqSUSY5 += LepTagConfig.GetDecorateImprovedPromptLeptonAlgs()

#====================================================================
# CONTENT LIST
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY5SlimmingHelper = SlimmingHelper("SUSY5SlimmingHelper")
SUSY5SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4EMPFlowJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKt4EMTopo_201810",
                                        "InDetTrackParticles",
                                        "PrimaryVertices",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"
                                        ]

SUSY5SlimmingHelper.AllVariables = ["MET_Truth", "MET_Track"]
SUSY5SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo_201810.MV1_discriminant.MV1c_discriminant",
                                      "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag.TruthLink",
                                      "Photons.author.Loose.Tight.TruthLink",
                                      "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi.N90Constituents.Timing.Width.DFCommonJets_jetClean_VeryLooseBadLLP",
                                      "AntiKt4EMPFlowJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi.N90Constituents.Timing.Width.DFCommonJets_jetClean_VeryLooseBadLLP",
                                      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix","CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink.seedTrackWidthPt500.seedTrackWidthPt1000",
                                      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                                      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID",
                                      "Electrons.bkgTruthType.bkgTruthOrigin.bkgMotherPdgId.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherPdgId.TruthLink",
                                      "AntiKt2PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
                                      #"BTagging_AntiKt2Track.MV2c10_discriminant",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.KtDR.ZCut12.Angularity.Aplanarity.PlanarFlow.FoxWolfram2.FoxWolfram0.Dip12.Sphericity.ThrustMin.ThrustMaj",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.XbbScoreHiggs.XbbScoreTop.XbbScoreQCD",
                                      "AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.eta.phi.m.GhostBHadronsFinalCount",
                                      "PrimaryVertices.covariance",
                                      "InDetTrackParticles.is_selected.is_associated.is_svtrk_final.pt_wrtSV.eta_wrtSV.phi_wrtSV.d0_wrtSV.z0_wrtSV.errP_wrtSV.errd0_wrtSV.errz0_wrtSV.chi2_toSV",
                                      "HLT_xAOD__JetContainer_SplitJet.pt.eta.phi.m",
                                      "HLT_xAOD__BTaggingContainer_HLTBjetFex.MV2c20_discriminant.MV2c10_discriminant",
                                      ]


# Saves BDT and input variables for light lepton algorithms.
# Can specify just electrons or just muons by adding 'name="Electrons"' or 'name="Muons"' as the argument.
SUSY5SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD(addSpectators=True)
# Saves BDT and input variables tau algorithm
SUSY5SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptTauVariablesForDxAOD()
SUSY5SlimmingHelper.ExtraVariables += LepTagConfig.GetExtraImprovedPromptVariablesForDxAOD()

SUSY5SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY5SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY5SlimmingHelper.IncludeBPhysTriggerContent  = False
SUSY5SlimmingHelper.IncludeJetTriggerContent    = False
SUSY5SlimmingHelper.IncludeTauTriggerContent    = False
SUSY5SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY5SlimmingHelper.IncludeBJetTriggerContent   = False

StaticContent = []
StaticContent += ["xAOD::VertexContainer#VrtSecInclusive_SoftBtagCandidateVertices"]
StaticContent += ["xAOD::VertexAuxContainer#VrtSecInclusive_SoftBtagCandidateVerticesAux."]

SUSY5SlimmingHelper.StaticContent = StaticContent

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkHasTruth:

  SUSY5SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSMWithDecayParticles':'xAOD::TruthParticleContainer','TruthBSMWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthHFWithDecayParticles':'xAOD::TruthParticleContainer','TruthHFWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer' ,
                                            'TruthBSMWithDecayVertices':'xAOD::TruthVertexContainer','TruthBSMWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthHFWithDecayVertices':'xAOD::TruthVertexContainer','TruthHFWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthPrimaryVertices':'xAOD::TruthVertexContainer','TruthPrimaryVerticesAux':'xAOD::TruthVertexAuxContainer'}

  SUSY5SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBoson", 
                                       "TruthBSMWithDecayParticles", "TruthHFWithDecayParticles", "TruthBSMWithDecayVertices", "TruthHFWithDecayVertices",
                                       "TruthPrimaryVertices"]

SUSY5SlimmingHelper.AppendContentToStream(SUSY5Stream)

