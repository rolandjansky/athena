#********************************************************************
# HIGG3D1.py
# reductionConf flag HIGG3D1 in Reco_tf.py
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import *
from DerivationFrameworkCore.WeightMetadata import *

from JetRec.JetRecStandard import jtm

from DerivationFrameworkHiggs.TruthCategories import *

from AthenaCommon.GlobalFlags import globalflags

#====================================================================
# Setup Soft B-vertex search tool
#====================================================================
#from NewVrtSecInclusiveTool.NewVrtSecInclusive import SoftBFinderTool
from VrtSecInclusive.MultiSecVertexTool import SoftBFinderTool
SVBFinder  = SoftBFinderTool("SVBFinder")
ToolSvc += SVBFinder

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SoftBVertexTool
HIGG3D1_SoftBVertexTool = DerivationFramework__SoftBVertexTool( name = "HIGG3D1_SoftBVertexTool",
                                    BVertexContainerName="SoftBVertices",                                    
                                    TrackContainerName="InDetTrackParticles",
                                    PVContainerName="PrimaryVertices",
				    BVertexTool=SVBFinder)
ToolSvc += HIGG3D1_SoftBVertexTool
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("SoftBVertexKernel", AugmentationTools = [HIGG3D1_SoftBVertexTool])

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG3D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG3D1Stream )
HIGG3D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG3D1Stream.AcceptAlgs(["HIGG3D1Kernel_skimming"])

## Prepare thinning service and add trigger chains for TrigNavigation thinning
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG3D1ThinningHelper = ThinningHelper( "HIGG3D1ThinningHelper" )

## Use the trigger lists in order to thin away all other trigger information
from DerivationFrameworkHiggs.HIGG3D1TriggerList import triggersNavThin
HIGG3D1ThinningHelper.TriggerChains = ' | '.join(triggersNavThin)

HIGG3D1ThinningHelper.AppendToStream( HIGG3D1Stream )

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools=[]

# InDetTrackParticle Thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

#MET Track Thinning
MET_track_thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
HIGG3D1MetTPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG3D1MetTPThinningTool",
                                                                      ThinningService        = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                      SelectionString        = MET_track_thinning_expression,
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      ApplyAnd               = True)
ToolSvc += HIGG3D1MetTPThinningTool
thinningTools.append(HIGG3D1MetTPThinningTool)

#Jet Track Thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG3D1JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG3D1JetTPThinningTool",
                                                                ThinningService         = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += HIGG3D1JetTPThinningTool
thinningTools.append(HIGG3D1JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG3D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG3D1MuonTPThinningTool",
                                                                           ThinningService         = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += HIGG3D1MuonTPThinningTool
thinningTools.append(HIGG3D1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG3D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "HIGG3D1ElectronTPThinningTool",
                                                                                 ThinningService         = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                                 SGKey                   = "Electrons",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG3D1ElectronTPThinningTool
thinningTools.append(HIGG3D1ElectronTPThinningTool)

# Tracks themselves
inclusive_track_thinning_expression = "(InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV) < 1.5*mm && InDetTrackParticles.pt > 10.*GeV)"
HIGG3D1TPThinningTool = DerivationFramework__TrackParticleThinning(name                    = "HIGG3D1TPThinningTool",
                                                                   ThinningService         = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                   SelectionString         = inclusive_track_thinning_expression,
                                                                   InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG3D1TPThinningTool
thinningTools.append(HIGG3D1TPThinningTool)

# Calo cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning

# Calo Clusters associated with Electrons
HIGG3D1ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "HIGG3D1ElectronCCThinningTool",
                                                                          ThinningService       = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                          SGKey                 = "Electrons",
                                                                          TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                          SelectionString       = "Electrons.pt > 7*GeV",
                                                                          ConeSize              = 0.2)
ToolSvc += HIGG3D1ElectronCCThinningTool
thinningTools.append(HIGG3D1ElectronCCThinningTool)

# Calo Clusters associated with Muons
HIGG3D1MuonCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "HIGG3D1MuonCCThinningTool",
                                                                      ThinningService       = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                      SGKey                 = "Muons",
                                                                      TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                      SelectionString       = "Muons.pt > 6*GeV",
                                                                      ConeSize              = 0.2)
ToolSvc += HIGG3D1MuonCCThinningTool
thinningTools.append(HIGG3D1MuonCCThinningTool)

# Calo Clusters associated with AntiKt4EMTopo Jets
# In Rel21, origin-corrected topoclusters are now used
HIGG3D1AntiKt4EMTopoCCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "HIGG3D1AntiKt4EMTopoCCThinningTool",
                                                                                 ThinningService         = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                                 SGKey                   = "AntiKt4EMTopoJets",
                                                                                 TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                 SelectionString         = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV",
                                                                                 AdditionalClustersKey   = ["EMOriginTopoClusters"])
ToolSvc += HIGG3D1AntiKt4EMTopoCCThinningTool
thinningTools.append(HIGG3D1AntiKt4EMTopoCCThinningTool)

# Particle flow objects associated with AntiKt4EMPFlow Jets
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetPFlowThinning

HIGG3D1AntiKt4EMPFlowPFOThinningTool = DerivationFramework__JetPFlowThinning(name                 = "HIGG3D1AntiKt4EMPFlowPFOThinningTool",
                                                                             ThinningService      = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                             SGKey                = "AntiKt4EMPFlowJets",
                                                                             SelectionString      = "AntiKt4EMPFlowJets.DFCommonJets_Calib_pt > 15*GeV")
ToolSvc += HIGG3D1AntiKt4EMPFlowPFOThinningTool
thinningTools.append(HIGG3D1AntiKt4EMPFlowPFOThinningTool)

###############################################################


# Truth particles
truth_cond1 = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond3 = "(abs(TruthParticles.pdgId) == 6)"  # Top quark
truth_cond4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 5*GeV))" # Photon
truth_cond5 = "((abs(TruthParticles.pdgId) >= 35) && (abs(TruthParticles.pdgId) <= 37))" # BSM Higgs

truth_expression = truth_cond1+' || '+truth_cond2 +' || '+truth_cond3 +' || '+truth_cond4 +' || '+truth_cond5

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG3D1TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HIGG3D1TruthThinningTool",
                                                                     ThinningService         = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                     ParticleSelectionString = truth_expression,
                                                                     PreserveDescendants     = False,
                                                                     PreserveAncestors      = True,
                                                                     SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset)

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG3D1TruthThinningTool
    thinningTools.append(HIGG3D1TruthThinningTool)

#====================================================================
# SKIMMING TOOLS
#====================================================================

skimmingTools=[]

# preselection
electronIDRequirements = '(Electrons.DFCommonElectronsLHVeryLoose)'
electronRequirements = '(Electrons.pt > 4*GeV) && (abs(Electrons.eta) < 2.6) && '+electronIDRequirements
leadElectron = electronRequirements + ' && (Electrons.pt > 17*GeV)'

muonsRequirements = '(Muons.pt > 2*GeV) && (Muons.DFCommonGoodMuon) && (Muons.DFCommonMuonsPreselection)'
leadMuon = muonsRequirements + ' && (Muons.pt > 17*GeV)'

# Set the pT threshold lower for the symmetric dilepton case
leadMuon_symdilepton = muonsRequirements + ' && (Muons.pt > 14*GeV)'

eeSelection = '((count('+electronRequirements+') >= 2) && (count('+leadElectron+') >= 1))'
mmSelection = '((count('+muonsRequirements+') >= 2) && (count('+leadMuon+') >= 1))'

#Adding in a symmetric dilepton kinematic + trigger requirment
mmSelection_symdilepton = '((count('+muonsRequirements+') >= 2) && (count('+leadMuon_symdilepton+') >= 2) && (HLT_2mu14 || HLT_2mu10))'

emSelection = '(((count('+electronRequirements+') >= 1) && (count('+muonsRequirements+') >= 1)) && ((count('+leadElectron+') >= 1) || (count('+leadMuon+') >= 1)))'

preselection_expression = eeSelection+' || '+mmSelection+' || '+emSelection+' || '+mmSelection_symdilepton

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG3D1PreselectionSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG3D1PreselectionSkimmingTool",
                                                                               expression = preselection_expression)
ToolSvc += HIGG3D1PreselectionSkimmingTool
skimmingTools.append(HIGG3D1PreselectionSkimmingTool)

# trigger skimming
from DerivationFrameworkHiggs.HIGG3D1TriggerList import SingleLepton_Combined,Dilepton_Combined
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
triglist = SingleLepton_Combined+Dilepton_Combined
HIGG3D1InclusiveTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "HIGG3D1InclusiveTriggerSkimmingTool",TriggerListOR = triglist)

ToolSvc += HIGG3D1InclusiveTriggerSkimmingTool
skimmingTools.append(HIGG3D1InclusiveTriggerSkimmingTool)

higg3d1Seq = CfgMgr.AthSequencer("HIGG3d1Sequence")
higg3d1PreSeq = CfgMgr.AthSequencer("HIGG3d1PreSelectionSequence")

#====================================================================
# RESTORE JET COLLECTIONS REMOVED BETWEEN r20 AND r21
#====================================================================
OutputJets["HIGG3D1"] = ["AntiKt4EMPFlowJets",
                         "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                         "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903"]

reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                  "AntiKt10LCTopoJets" # Needed for Ghost association
                 ]
if jetFlags.useTruth:
    reducedJetList += ["AntiKt4TruthJets", "AntiKt4TruthWZJets", "AntiKt4TruthDressedWZJets", "AntiKt10TruthTrimmedPtFrac5SmallR20Jets"]
replaceAODReducedJets(reducedJetList, higg3d1Seq,"HIGG3D1")

addDefaultTrimmedJets(higg3d1Seq,"HIGG3D1")

# Also add DFCommonJets_Calib_pt to jet collections
applyJetCalibration_xAODColl("AntiKt4EMTopo", higg3d1Seq)
applyJetCalibration_xAODColl("AntiKt4EMPFlow", higg3d1Seq)

#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet
#====================================================================
addVRJets(higg3d1Seq)
addVRJets(higg3d1Seq, training='201903')

#addVRJets(higg3d1Seq, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet",
#          VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track",
#          ghostArea = 0 , ptmin = 2000, ptminFilter = 7000,
#          variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")

#===================================================================
# B-TAGGING
#===================================================================
from BTagging.BTaggingFlags import BTaggingFlags

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

# Add flavor tagging to the PFlow Jet collections
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = higg3d1Seq)

# Hbb tagger
addHbbTagger(higg3d1Seq, ToolSvc)

#====================================================================
# fJVT for PFlow jets
#====================================================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import getPFlowfJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=higg3d1Seq, algname='JetForwardPFlowJvtToolAlg')

#====================================================================
# QG tagging
#====================================================================
addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=higg3d1Seq,algname="QGTaggerToolAlg",truthjetalg='AntiKt4TruthJets')
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=higg3d1Seq,algname="QGTaggerToolPFAlg",truthjetalg='AntiKt4TruthJets')

#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the LeptonTaggers config and add to the private sequence
import LeptonTaggers.LeptonTaggersConfig as LepTagConfig
higg3d1Seq += LepTagConfig.GetDecoratePromptLeptonAlgs()
higg3d1Seq += LepTagConfig.GetDecorateImprovedPromptLeptonAlgs()

#====================================================================
# Truth decoration tool
#====================================================================
augmentationTools=[]
if globalflags.DataSource()=='geant4':
    from DerivationFrameworkHiggs.HIGG3DxAugmentation import getHIGG3TruthDecoratorTool
    HIGG3D1TruthDecoratorTool = getHIGG3TruthDecoratorTool(outputStream = HIGG3D1Stream,
                                                           toolNamePrefix = "HIGG3D1"
                                                           )
    ToolSvc += HIGG3D1TruthDecoratorTool
    augmentationTools.append(HIGG3D1TruthDecoratorTool)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG3D1SlimmingHelper = SlimmingHelper("HIGG3D1SlimmingHelper")
from DerivationFrameworkHiggs.HIGG3D1ExtraContent import *

HIGG3D1SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "TauJets",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "AntiKt4EMTopoJets_BTagging201810",
                                          "AntiKt4EMPFlowJets",
                                          "AntiKt4EMPFlowJets_BTagging201810",
                                          "AntiKt4EMPFlowJets_BTagging201903",
                                          "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                                          "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
                                          "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                          "BTagging_AntiKt4EMTopo_201810",
                                          "BTagging_AntiKt4EMPFlow_201810",
                                          "BTagging_AntiKt4EMPFlow_201903",
                                          "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
                                          "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

HIGG3D1SlimmingHelper.ExtraVariables = list(HIGG3D1ExtraVariables)
HIGG3D1SlimmingHelper.AllVariables = list(HIGG3D1ExtraContainers)
HIGG3D1SlimmingHelper.ExtraVariables += LepTagConfig.GetExtraPromptVariablesForDxAOD(onlyBDT=False)
HIGG3D1SlimmingHelper.ExtraVariables += LepTagConfig.GetExtraImprovedPromptVariablesForDxAOD()

# needed to calculate electron LH downstream
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import ElectronsCPDetailedContent, GSFTracksCPDetailedContent
HIGG3D1SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
HIGG3D1SlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent

addOriginCorrectedClusters(HIGG3D1SlimmingHelper,writeEM=True)

if globalflags.DataSource()=='geant4':
    HIGG3D1SlimmingHelper.SmartCollections += ["AntiKt4TruthJets", "AntiKt4TruthDressedWZJets"]

    # Add special truth containers
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addWbosonsAndDownstreamParticles,addBSMAndDownstreamParticles,addLargeRJetD2
    addStandardTruthContents()
    addWbosonsAndDownstreamParticles()
    addBSMAndDownstreamParticles()

    HIGG3D1SlimmingHelper.AppendToDictionary.update({
                                                     'SoftBVertices':'xAOD::VertexContainer','SoftBVerticesAux': 'xAOD::VertexAuxContainer',
                                                     'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                                     'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                                     'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                                     'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                                     'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                                     'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                                     'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                                     'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                                     'TruthWbosonWithDecayParticles':'xAOD::TruthParticleContainer','TruthWbosonWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                                     'TruthWbosonWithDecayVertices':'xAOD::TruthVertexContainer','TruthWbosonWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                                     'TruthBSMWithDecayParticles':'xAOD::TruthParticleContainer','TruthBSMWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                                     'TruthBSMWithDecayVertices':'xAOD::TruthVertexContainer','TruthBSMWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                                     'AntiKt4TruthDressedWZJets':'xAOD::JetContainer','AntiKt4TruthDressedWZJetsAux':'xAOD::JetAuxContainer',
                                                     'AntiKt10TruthTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer','AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer'
                                                    })
    HIGG3D1SlimmingHelper.AllVariables += list(HIGG3D1ExtraTruthContainers)
    HIGG3D1SlimmingHelper.ExtraVariables += list(HIGG3D1ExtraTruthVariables)
    HIGG3D1SlimmingHelper.ExtraVariables += list(HIGG3D1TruthDecoratorVariables)

#========================================
# CREATE THE DERIVATION KERNEL ALGORITHMS
#========================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
higg3d1PreSeq += CfgMgr.DerivationFramework__DerivationKernel("HIGG3D1Kernel_skimming",
                                                           SkimmingTools = skimmingTools
                                                           )
higg3d1PreSeq += higg3d1Seq

higg3d1Seq += CfgMgr.DerivationFramework__DerivationKernel("HIGG3D1Kernel_thinning",
                                                           ThinningTools = thinningTools
                                                           )
higg3d1Seq += CfgMgr.DerivationFramework__DerivationKernel("HIGG3D1Kernel_augmentation",
                                                           AugmentationTools = augmentationTools
                                                           )
DerivationFrameworkJob += higg3d1PreSeq

# Add Trigger content
HIGG3D1SlimmingHelper.IncludeMuonTriggerContent = True
HIGG3D1SlimmingHelper.IncludeEGammaTriggerContent = True

# Add MET to output stream
addMETOutputs(HIGG3D1SlimmingHelper, ["AntiKt4EMPFlow", "Track"],
                                     ["AntiKt4EMPFlow",
                                      "AntiKt4EMTopo"] # smart collections list
                                     )
addJetOutputs(HIGG3D1SlimmingHelper, ["HIGG3D1"],
                                     [], # smart collections list
                                     ["AntiKt2PV0TrackJets",
                                      "AntiKt4PV0TrackJets",
                                      "AntiKt4TruthJets",
                                      "AntiKt4TruthWZJets",
                                      "AntiKt4TruthDressedWZJets",
                                      "AntiKt4EMPFlowJets",
                                      "AntiKt10LCTopoJets",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                      "AntiKt10TruthTrimmedPtFrac5SmallR20Jets"] #veto list
                                     )

HIGG3D1SlimmingHelper.AppendContentToStream(HIGG3D1Stream)
HIGG3D1Stream.AddItem("xAOD::VertexContainer#SoftBVertices")
HIGG3D1Stream.AddItem("xAOD::VertexAuxContainer#SoftBVerticesAux.")
