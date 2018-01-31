#********************************************************************
# SUSY7.py 
# reductionConf flag SUSY7 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *


### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY7Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY7Stream )
SUSY7Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY7Stream.AcceptAlgs(["SUSY7KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY7ThinningHelper = ThinningHelper( "SUSY7ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY7 = CfgMgr.AthSequencer("SeqSUSY7")
DerivationFrameworkJob += SeqSUSY7


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY7TriggerList import * 
SUSY7ThinningHelper.TriggerChains = '|'.join(SUSY7ThinTriggers) #'HLT_e.*|HLT_g.*|HLT_mu.*|HLT_2e.*|HLT_2mu.*|HLT_3mu.*'
SUSY7ThinningHelper.AppendToStream( SUSY7Stream )


#====================================================================
# THINNING TOOL 
#====================================================================\

# B.M.: likely not used
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

#SUSY7TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY7TPThinningTool",
#								 ThinningService	 = SUSY7ThinningHelper.ThinningSvc(),
#								 SelectionString	 = "InDetTrackParticles.pt > 10*GeV",
#								 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY7TPThinningTool
#thinningTools.append(SUSY7TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY7MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name			 = "SUSY7MuonTPThinningTool",
									 ThinningService	 = SUSY7ThinningHelper.ThinningSvc(),
									 MuonKey		 = "Muons",
									 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY7MuonTPThinningTool
thinningTools.append(SUSY7MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY7ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY7ElectronTPThinningTool",
										 ThinningService	 = SUSY7ThinningHelper.ThinningSvc(),
										 SGKey			 = "Electrons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY7ElectronTPThinningTool
thinningTools.append(SUSY7ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY7PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY7PhotonTPThinningTool",
										 ThinningService	 = SUSY7ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY7PhotonTPThinningTool
thinningTools.append(SUSY7PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY7TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name		= "SUSY7TauTPThinningTool",
									ThinningService = SUSY7ThinningHelper.ThinningSvc(),
									TauKey  	= "TauJets",
									InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY7TauTPThinningTool
thinningTools.append(SUSY7TauTPThinningTool)

# Calo Clusters associated with Photons
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
SUSY7PhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "SUSY7PhotonCCThinningTool",
                                                                                     ThinningService         = SUSY7ThinningHelper.ThinningSvc(),
                                                                                     SGKey                   = "Photons",
                                                                                     #CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Photons.pt > 10*GeV",
                                                                                     #FrwdClCollectionSGKey   = "ForwardElectronClusters",
                                                                                     ConeSize                = 0.4)
ToolSvc += SUSY7PhotonCCThinningTool
thinningTools.append(SUSY7PhotonCCThinningTool)


#from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
# Calo Clusters associated with Electrons
SUSY7ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "SUSY7ElectronCCThinningTool",
                                                                                     ThinningService         = SUSY7ThinningHelper.ThinningSvc(),
                                                                                     SGKey                   = "Electrons",
                                                                                     #CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Electrons.pt > 7*GeV",
                                                                                     ConeSize                = 0.2)
ToolSvc += SUSY7ElectronCCThinningTool
thinningTools.append(SUSY7ElectronCCThinningTool)

# Calo Clusters associated with Muons
SUSY7MuonCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "SUSY7MuonCCThinningTool",
                                                                                     ThinningService         = SUSY7ThinningHelper.ThinningSvc(),
                                                                                     SGKey                   = "Muons",
                                                                                     #CaloClCollectionSGKey   = "MuonClusterCollection",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Muons.pt > 6*GeV",
                                                                                     ConeSize                = 0.2)
ToolSvc += SUSY7MuonCCThinningTool
thinningTools.append(SUSY7MuonCCThinningTool)

# Calo Clusters associated with jets
#from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__JetCaloClusterThinning
#SUSY7aKt4CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "SUSY7aKt4CCThinningTool",
#                                                                     ThinningService         = SUSY7ThinningHelper.ThinningSvc(),
#                                                                      SGKey                   = "AntiKt4LCTopoJets",
#                                                                      TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                      SelectionString         = "AntiKt4LCTopoJets.pt > 20*GeV")
#ToolSvc += SUSY7aKt4CCThinningTool
#thinningTools.append(SUSY7aKt4CCThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY7TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY7TruthThinningTool",
                                                       ThinningService              = SUSY7ThinningHelper.ThinningSvc(),
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
  ToolSvc += SUSY7TruthThinningTool
  thinningTools.append(SUSY7TruthThinningTool)


# now done in ExtendedJetCommon
#applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY7)
#updateJVT_xAODColl("AntiKt4EMTopo", SeqSUSY7)
#applyBTagging_xAODColl('AntiKt4EMTopo', SeqSUSY7)


#dilepton selection
muonsRequirements = '(Muons.pt >= 6.*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 7.*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.DFCommonElectronsLHLoose)'
diLepExpr = '(count(%s) + count (%s)) > 1' % (muonsRequirements, electronsRequirements)

#btagging selection 
# cut value from https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingBenchmarks#b_tagging_Benchmarks_for_tag_AN1
bfix77='AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77'
bfix85='AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_85'
bflat77='AntiKt4EMTopoJets.DFCommonJets_FlatBEff_77'
bflat85='AntiKt4EMTopoJets.DFCommonJets_FlatBEff_85'
jetpt='AntiKt4EMTopoJets.DFCommonJets_Calib_pt'

bjet77='(%s || %s)' % (bfix77, bflat77)
bjet85='(%s || %s)' % (bfix85, bflat85)

onebtagExpr = "count(%s && (%s>50.*GeV))>0" % (bjet77, jetpt)

multibExpr  = "count(%s && (%s>20.*GeV))>1" % (bjet85, jetpt)

#ISR-selection (non-btagged high-pt + btagged low-pt)
isrBFixExpr  = "(count(!%s && (%s>150.*GeV))>0 && count(%s && (%s>20.*GeV))>0)" % (bfix77,jetpt,bfix85,jetpt)
isrBFlatExpr = "(count(!%s && (%s>150.*GeV))>0 && count(%s && (%s>20.*GeV))>0)" % (bflat77,jetpt,bflat85,jetpt)
isrExpr = "(%s || %s)" % (isrBFixExpr, isrBFlatExpr)

### skimming 
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY7diLepSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY7diLepSkimmingTool",
                                                                      expression = diLepExpr)
ToolSvc += SUSY7diLepSkimmingTool

SUSY7btagSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY7btagSkimmingTool",
                                                                     expression = onebtagExpr)
ToolSvc += SUSY7btagSkimmingTool

SUSY7multibSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY7multibSkimmingTool",
                                                                       expression = multibExpr)
ToolSvc += SUSY7multibSkimmingTool

SUSY7isrSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY7isrSkimmingTool",
                                                                    expression = isrExpr)
ToolSvc += SUSY7isrSkimmingTool

#SUSY7isrBFixSkimmingTool  = DerivationFramework__xAODStringSkimmingTool( name = "SUSY7isrBFixSkimmingTool", expression = isrBFixExpr)
#SUSY7isrBFlatSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY7isrBFlatSkimmingTool", expression = isrBFlatExpr)
#ToolSvc += SUSY7isrBFixSkimmingTool
#ToolSvc += SUSY7isrBFlatSkimmingTool


#make selections OR
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
SUSY7SkimmingORTool = DerivationFramework__FilterCombinationOR(name = "SUSY7SkimmingORTool",
                                                               FilterList = [SUSY7diLepSkimmingTool, SUSY7btagSkimmingTool, SUSY7multibSkimmingTool, SUSY7isrSkimmingTool])
ToolSvc += SUSY7SkimmingORTool
   

#add AND with Trigger skimming criteria
from DerivationFrameworkSUSY.SUSY7TriggerList import *
trigExpr = '('+' || '.join(SUSY7AllTriggers)+')'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
SUSY7trigSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY7trigSkimmingTool",
                                                                  TriggerListOR = SUSY7AllTriggers)
   
ToolSvc += SUSY7trigSkimmingTool
   
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
SUSY7SkimmingTool = DerivationFramework__FilterCombinationAND(name = "SUSY7SkimmingTool",
                                                              FilterList = [SUSY7SkimmingORTool, SUSY7trigSkimmingTool])
ToolSvc += SUSY7SkimmingTool


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
   SeqSUSY7 += CfgMgr.DerivationFramework__DerivationKernel("SUSY7KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY7")
                                                            )
   
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY7 += CfgMgr.DerivationFramework__DerivationKernel(
   "SUSY7KernelSkim",
   SkimmingTools = [SUSY7SkimmingTool]
   )


#==============================================================================
# Jet building
#==============================================================================
OutputJets["SUSY7"] = []
reducedJetList = [ "AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets" ]

# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#  reducedJetList += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]

# AntiKt2PV0TrackJets is flavour-tagged automatically (AntiKt4PV0TrackJets is not supported in R21)
replaceAODReducedJets(reducedJetList, SeqSUSY7, "SUSY7")

# re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY7)


#==============================================================================
# Tau truth building/matching
#==============================================================================
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#  from DerivationFrameworkSUSY.SUSYTruthCommon import addTruthTaus
#  addTruthTaus(AugmentationTools)


#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY7 += CfgMgr.DerivationFramework__DerivationKernel(
   "SUSY7KernelAug",
   AugmentationTools = AugmentationTools,
   ThinningTools = thinningTools,
   )


#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY7SlimmingHelper = SlimmingHelper("SUSY7SlimmingHelper")
SUSY7SlimmingHelper.SmartCollections = ["Electrons","Photons","MET_Reference_AntiKt4EMTopo","Muons","TauJets","AntiKt4EMTopoJets", "BTagging_AntiKt4EMTopo", "InDetTrackParticles", "PrimaryVertices","AntiKt4EMPFlowJets", "MET_Reference_AntiKt4EMPFlow"]
SUSY7SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "MET_Track","METAssoc_AntiKt4EMPFlow","MET_Core_AntiKt4EMPFlow"]
SUSY7SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
				      "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
				      "Photons.author.Loose.Tight",
				      "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_FixedCutBEff_30.DFCommonJets_FixedCutBEff_30.DFCommonJets_FixedCutBEff_50.DFCommonJets_FixedCutBEff_60.DFCommonJets_FixedCutBEff_70.DFCommonJets_FixedCutBEff_77.DFCommonJets_FixedCutBEff_80.DFCommonJets_FixedCutBEff_85.DFCommonJets_FixedCutBEff_90.DFCommonJets_FlatBEff_30.DFCommonJets_FlatBEff_40.DFCommonJets_FlatBEff_50.DFCommonJets_FlatBEff_60.DFCommonJets_FlatBEff_70.DFCommonJets_FlatBEff_77.DFCommonJets_FlatBEff_85'",
				      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
				      "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
				      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
				      "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
				      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
				      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID",
                                      "AntiKt4PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
#BM: no longer supported in R21                                "BTagging_AntiKt4Track.MV2c20_discriminant.MV2c10_discriminant",
#P. Pani removed 20/06/16                                      "AntiKt3PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
#P. Pani removed 20/06/16                                      "BTagging_AntiKt3Track.MV2c20_discriminant","BTagging_AntiKt3Track.MV2c10_discriminant",
                                      "AntiKt2PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
                                      "BTagging_AntiKt2Track.MV2c20_discriminant.MV2c10_discriminant",
				      "Muons.quality.etcone20.ptconecoreTrackPtrCorrection","Electrons.quality.etcone20.ptconecoreTrackPtrCorrection",
				      "CaloCalTopoClusters.rawE.rawEta.rawPhi.rawM.calE.calEta.calPhi.calM.e_sampl",
				      "MuonClusterCollection.eta_sampl.phi_sampl",
				      "AntiKt4EMPFlowJets.EMFrac.HECFrac.LArQuality.HECQuality.FracSamplingMax.NegativeE.AverageLArQF.FracSamplingMaxIndex.SumPtTrkPt500",
                                      "AntiKt4EMPFlowJets.pt.eta.btagging.btaggingLink.TruthLabelID.constituentLinks.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostAntiKt3TrackJet.GhostAntiKt4TrackJet.GhostMuonSegment.GhostTrack.GhostTruthAssociationLink.HighestJVFVtx.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.HighestJVFLooseVtx.ConeTruthLabelID.GhostAntiKt2TrackJet.PartonTruthLabelID.Jvt.HadronConeExclTruthLabelID.JvtJvfcorr.JvtRpt.SumPtTrkPt500.SumPtTrkPt1000.TrackWidthPt1000.TrackWidthPt500",
				      "BTagging_AntiKt4EMPFlow.MV1_discriminant.MV1c_discriminant.SV1_pb.SV1_pu.IP3D_pb.IP3D_pu.MV2c00_discriminant.MV2c10_discriminant.MV2c20_discriminant.MVb_discriminant.MSV_vertices.SV0_badTracksIP.SV0_vertices.SV1_badTracksIP.SV1_vertices.BTagTrackToJetAssociator.BTagTrackToJetAssociatorBB.JetFitter_JFvertices.JetFitter_tracksAtPVlinks.MSV_badTracksIP.MV2c100_discriminant.MV2m_pu.MV2m_pc.MV2m_pb",
				      #"AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.n_constituents.n_subjets.tau_1.tau_2.tau_3.ecf_1.ecf_2.ecf_3"
                                      # this jet collection is not scheduled
                                  ]

SUSY7SlimmingHelper.IncludeMuonTriggerContent = True
SUSY7SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY7SlimmingHelper.IncludeBPhysTriggerContent = False
SUSY7SlimmingHelper.IncludeJetTriggerContent = True
SUSY7SlimmingHelper.IncludeTauTriggerContent = False
SUSY7SlimmingHelper.IncludeEtMissTriggerContent = True

#addJetOutputs(SUSY7SlimmingHelper,["LargeR", "SUSY7"], [], ["CamKt12LCTopoJets","AntiKt10LCTopoJets","AntiKt10TruthJets","CamKt12TruthWZJets","CamKt12TruthJets","AntiKt10TruthWZJets",
#                                                            "AntiKt4TruthWZJets","AntiKt4TruthJets","AntiKt2PV0TrackJets","AntiKt4PV0TrackJets"])

# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
SUSY7SlimmingHelper.AppendToDictionary = {'BTagging_AntiKt4EMPFlow':'xAOD::BTaggingContainer','BTagging_AntiKt4EMPFlowAux':'xAOD::BTaggingAuxContainer',
                                          'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                          'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                          'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py) 
if DerivationFrameworkIsMonteCarlo:   
  SUSY7SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]   


SUSY7SlimmingHelper.AppendContentToStream(SUSY7Stream)

