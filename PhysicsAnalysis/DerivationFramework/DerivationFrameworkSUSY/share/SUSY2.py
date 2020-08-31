#********************************************************************
# SUSY2.py
# reductionConf flag SUSY2 in Reco_tf.py
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
streamName = derivationFlags.WriteDAOD_SUSY2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY2Stream )
SUSY2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY2Stream.AcceptAlgs(["SUSY2KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY2ThinningHelper = ThinningHelper( "SUSY2ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY2 = CfgMgr.AthSequencer("SeqSUSY2")
DerivationFrameworkJob += SeqSUSY2




#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY2TriggerList import LeptonTriggers, PhotonTriggers
triggersNavThin = LeptonTriggers + PhotonTriggers
SUSY2ThinningHelper.TriggerChains = '|'.join(triggersNavThin)


SUSY2ThinningHelper.AppendToStream( SUSY2Stream )


#====================================================================
# THINNING TOOLS
#====================================================================

# B.M.: likely not used
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# TrackParticles directly
#SUSY2TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY2TPThinningTool",
#                                                                 ThinningService         = SUSY2ThinningHelper.ThinningSvc(),
#                                                                 SelectionString         = "InDetTrackParticles.pt > 10*GeV",
#                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY2TPThinningTool
#thinningTools.append(SUSY2TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY2MuonTPThinningTool",
                                                                         ThinningService         = SUSY2ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY2MuonTPThinningTool
thinningTools.append(SUSY2MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY2ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY2ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	= "Electrons",
                                                                                 BestMatchOnly          = False,
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY2ElectronTPThinningTool
thinningTools.append(SUSY2ElectronTPThinningTool)
# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY2PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY2PhotonTPThinningTool",
										 ThinningService	 = SUSY2ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY2PhotonTPThinningTool
thinningTools.append(SUSY2PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY2TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY2TauTPThinningTool",
                                                                        ThinningService = SUSY2ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY2TauTPThinningTool
thinningTools.append(SUSY2TauTPThinningTool)

# TrackParticles associated with SV
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__VertexParticleThinning
SUSY2VertexTPThinningTool = DerivationFramework__VertexParticleThinning( name                = "SUSY2VertexTPThinningTool",
                                                                      ThinningService        = SUSY2ThinningHelper.ThinningSvc(),
                                                                      VertexKey              = "VrtSecInclusive_SoftBtagCandidateVertices",
                                                                      InDetTrackParticlesKey = "InDetTrackParticles")

ToolSvc += SUSY2VertexTPThinningTool
thinningTools.append(SUSY2VertexTPThinningTool)

# make Pixel and SCT conditions available
include ("InDetRecExample/PixelConditionsAccess.py") # include all pixel condtions avaliable in AOD /DT
include ("InDetRecExample/SCTConditionsAccess.py")


# Cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning

# Caloclusters associated to electrons
SUSY2ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "SUSY2ElectronCCThinningTool",
                                                                                     ThinningService         = SUSY2ThinningHelper.ThinningSvc(),
                                                                                     SGKey                   = "Electrons",
                                                                                     #CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     #SelectionString         = "Electrons.pt > 9.0*GeV",
                                                                                     ConeSize                = 0.4)

ToolSvc += SUSY2ElectronCCThinningTool
thinningTools.append(SUSY2ElectronCCThinningTool)

# Calo Clusters associated with Photons
SUSY2PhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "SUSY2PhotonCCThinningTool",
                                                                     ThinningService         = SUSY2ThinningHelper.ThinningSvc(),
                                                                     SGKey                   = "Photons",
                                                                     #CaloClCollectionSGKey   = "egammaClusters",
                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                     #SelectionString         = "Photons.pt > 10.0*GeV",
                                                                     ConeSize                = 0.4)

ToolSvc += SUSY2PhotonCCThinningTool
thinningTools.append(SUSY2PhotonCCThinningTool)

# Calo Clusters associated with Muons
SUSY2MuonCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "SUSY2MuonCCThinningTool",
                                                                   ThinningService         = SUSY2ThinningHelper.ThinningSvc(),
                                                                   SGKey                   = "Muons",
                                                                   #CaloClCollectionSGKey   = "MuonClusterCollection",
                                                                   TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                   #SelectionString         = "Muons.pt > 9.0*GeV",
                                                                   ConeSize                = 0.4)

ToolSvc += SUSY2MuonCCThinningTool
thinningTools.append(SUSY2MuonCCThinningTool)

# TrackParticles associated with LC jets: useful when the global track thinning has a pT threshold ~1-2 GeV
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
#SUSY2LCJetsTrackThinningTool = DerivationFramework__JetTrackParticleThinning( name            = "SUSY2LCJetsTrackThinningTool",
#                                                                              ThinningService = SUSY2ThinningHelper.ThinningSvc(),
#                                                                              JetKey          = "AntiKt4LCTopoJets",
#                                                                              SelectionString         = "AntiKt4LCTopoJets.pt > 18*GeV",
#                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY2LCJetsTrackThinningTool
#thinningTools.append(SUSY2LCJetsTrackThinningTool)



#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY2TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY2TruthThinningTool",
                                                       ThinningService              = SUSY2ThinningHelper.ThinningSvc(),
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
                                                       WriteAllLeptons              = True,
                                                       WriteLeptonsNotFromHadrons   = False,
                                                       WriteStatus3                 = False,
                                                       WriteFirstN                  = -1,
                                                       PreserveAncestors            = True,
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset,
                                                       LongLivedPDGIDs              = [1000011,2000011,1000013,2000013,1000015,2000015,15])

  ToolSvc += SUSY2TruthThinningTool
  thinningTools.append(SUSY2TruthThinningTool)

  # Decorate Electron with bkg electron type/origin
  from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier
  from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
  BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool", MCTruthClassifierTool = BkgElectronMCTruthClassifier)
  ToolSvc += BkgElectronClassificationTool
  AugmentationTools.append(BkgElectronClassificationTool)

  from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__MuonTruthClassifierFallback
  MuonTruthClassifierFallback = DerivationFramework__MuonTruthClassifierFallback( name = "MuonTruthClassifierFallback",MCTruthClassifierTool = BkgElectronMCTruthClassifier, ContainerKey="Muons")
  ElectronTruthClassifierFallback = DerivationFramework__MuonTruthClassifierFallback( name = "ElectronTruthClassifierFallback",MCTruthClassifierTool = BkgElectronMCTruthClassifier, ContainerKey="Electrons")
  ToolSvc += MuonTruthClassifierFallback
  ToolSvc += ElectronTruthClassifierFallback
  AugmentationTools.append(MuonTruthClassifierFallback)
  AugmentationTools.append(ElectronTruthClassifierFallback)

#====================================================================
# SKIMMING TOOL
#====================================================================
muonsRequirements = '(Muons.pt >= 9.0*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 9.0*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
photonRequirements = '(DFCommonPhotons_et > 40*GeV) && (abs(DFCommonPhotons_eta) < 2.6) && (Photons.DFCommonPhotonsIsEMTight)'
TightMuRequirements = '(Muons.pt >= 25.0*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonMuonsPreselection)'
TightEleRequirements = '(Electrons.pt >= 25.0*GeV) && (abs(Electrons.eta) < 2.6) &&((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'


objectSelection = 'count('+electronsRequirements+') + count('+muonsRequirements+') >= 2'
objectSelectionPHMu =  'count('+photonRequirements+') > 0 && count('+TightMuRequirements+') >0'
objectSelectionPHEle =  'count('+photonRequirements+') > 0 && count('+TightEleRequirements+') >0'
objectSelectionPH = '(%s) || (%s)' % (objectSelectionPHEle, objectSelectionPHMu)

#Defults SUSY2
#alltriggers = ' || '.join(triggersNavThin) 

#expression = '(' + alltriggers + ') && '+objectSelection 


trig_PHexpression = '(' + ' || '.join(PhotonTriggers)+')'
trig_Lepexpression = '(' + ' || '.join(LeptonTriggers)+')'
LepPh_expression = '('+'('+trig_Lepexpression+'&&'+objectSelection+')'+'||'+'('+trig_PHexpression+ '&&'+objectSelectionPH+')'+')'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY2SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY2SkimmingTool",
                                                                expression = LepPh_expression)

ToolSvc += SUSY2SkimmingTool


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
   SeqSUSY2 += CfgMgr.DerivationFramework__DerivationKernel("SUSY2KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY2")
                                                            )

   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY2 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY2KernelSkim",
  SkimmingTools = [SUSY2SkimmingTool]
  )

#==============================================================================
# Soft b-tag
#==============================================================================
from DerivationFrameworkFlavourTag.SoftBtagCommon import *
applySoftBtagging("softBtag", SeqSUSY2 )


#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY2)

## Adding decorations for fJVT PFlow jets                                                                                                                                                                   
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqSUSY2, algname='JetForwardPFlowJvtToolAlg')
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=SeqSUSY2, algname='JetForwardJvtToolBDTAlg')
#==============================================================================
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#
#  OutputJets["SUSY2"] = []
#  reducedJetList = [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]
#
#  replaceAODReducedJets(reducedJetList, SeqSUSY2, "SUSY2")


#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY2 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY2KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)

#====================================================================
# Prompt Lepton Tagger
#====================================================================

import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig

# simple call to replaceAODReducedJets(["AntiKt4PV0TrackJets"], SeqSUSY2, "SUSY2")
JetTagConfig.ConfigureAntiKt4PV0TrackJets(SeqSUSY2, "SUSY2")

# add decoration
SeqSUSY2 += JetTagConfig.GetDecoratePromptLeptonAlgs(addSpectators=True)
SeqSUSY2 += JetTagConfig.GetDecoratePromptTauAlgs()


#====================================================================
# CONTENT LIST
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY2SlimmingHelper = SlimmingHelper("SUSY2SlimmingHelper")
SUSY2SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4EMPFlowJets",
                                        #"AntiKt4LCTopoJets",
#                                        "BTagging_AntiKt4EMTopo",
#                                        "BTagging_AntiKt4EMPFlow",
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKt4EMTopo_201810",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"]
SUSY2SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "MET_Track"]
SUSY2SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo_201810.MV1_discriminant.MV1c_discriminant",
                                      "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
                                      "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_jetClean_VeryLooseBadLLP",
                                      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "InDetTrackParticles.truthOrigin.truthType",
                                      "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
                                      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                                      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                                      "Electrons.bkgTruthType.bkgTruthOrigin.bkgMotherPdgId.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherPdgId.deltaPhi1",
                                      "CaloCalTopoClusters.rawE.rawEta.rawPhi.rawM.calE.calEta.calPhi.calM.e_sampl",
                                      "MuonClusterCollection.eta_sampl.phi_sampl",
                                      "Muons.quality.etcone20.ptconecoreTrackPtrCorrection","Electrons.quality.etcone20.ptconecoreTrackPtrCorrection",
                                      "Muons.TruthClassifierFallback_truthType.TruthClassifierFallback_truthOrigin.TruthClassifierFallback_dR",
                                      "Electrons.TruthClassifierFallback_truthType.TruthClassifierFallback_truthOrigin.TruthClassifierFallback_dR",
                                      "PrimaryVertices.covariance"]

# Saves BDT and input variables for light lepton algorithms.
# Can specify just electrons or just muons by adding 'name="Electrons"' or 'name="Muons"' as the argument.
SUSY2SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD(addSpectators=True)
# Saves BDT and input variables tau algorithm
SUSY2SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptTauVariablesForDxAOD()
# add missing branches needed by Charge Flip Killer Tool
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
SUSY2SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent


SUSY2SlimmingHelper.IncludeMuonTriggerContent = True
SUSY2SlimmingHelper.IncludeEGammaTriggerContent = True
#SUSY2SlimmingHelper.IncludeBPhysTriggerContent = True
#SUSY2SlimmingHelper.IncludeJetTauEtMissTriggerContent = True

StaticContent = []
StaticContent += ["xAOD::VertexContainer#VrtSecInclusive_SoftBtagCandidateVertices"]
StaticContent += ["xAOD::VertexAuxContainer#VrtSecInclusive_SoftBtagCandidateVerticesAux."]

SUSY2SlimmingHelper.StaticContent = StaticContent

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY2SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}

  SUSY2SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]


SUSY2SlimmingHelper.AppendContentToStream(SUSY2Stream)
