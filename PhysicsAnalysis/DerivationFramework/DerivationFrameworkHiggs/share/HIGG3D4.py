#********************************************************************
# HIGG3D4.py 
# reductionConf flag HIGG3D4 in Reco_tf.py   
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
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG3D4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG3D4Stream )
HIGG3D4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG3D4Stream.AcceptAlgs(["HIGG3D4Kernel_skimming"])

## Prepare thinning service and add trigger chains for TrigNavigation thinning
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG3D4ThinningHelper = ThinningHelper( "HIGG3D4ThinningHelper" )

## Use the trigger lists in order to thin away all other trigEger information
from DerivationFrameworkHiggs.HIGG3D1TriggerList import triggersNavThin
HIGG3D4ThinningHelper.TriggerChains = ' | '.join(triggersNavThin)

HIGG3D4ThinningHelper.AppendToStream( HIGG3D4Stream )

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools=[]

# InDetTrackParticle Thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

#MET Track Thinning
MET_track_thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
HIGG3D4MetTPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG3D4MetTPThinningTool",
                                                                      ThinningService        = HIGG3D4ThinningHelper.ThinningSvc(),
                                                                      SelectionString        = MET_track_thinning_expression,
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      ApplyAnd               = True)
ToolSvc += HIGG3D4MetTPThinningTool
thinningTools.append(HIGG3D4MetTPThinningTool)

#Jet Track Thinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG3D4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG3D4JetTPThinningTool",
                                                                ThinningService         = HIGG3D4ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += HIGG3D4JetTPThinningTool
thinningTools.append(HIGG3D4JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG3D4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG3D4MuonTPThinningTool",
                                                                           ThinningService         = HIGG3D4ThinningHelper.ThinningSvc(),
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += HIGG3D4MuonTPThinningTool
thinningTools.append(HIGG3D4MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG3D4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "HIGG3D4ElectronTPThinningTool",
                                                                                 ThinningService         = HIGG3D4ThinningHelper.ThinningSvc(),
                                                                                 SGKey                   = "Electrons",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG3D4ElectronTPThinningTool
thinningTools.append(HIGG3D4ElectronTPThinningTool)

# Tracks themselves
inclusive_track_thinning_expression = "(InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV) < 1.5*mm && InDetTrackParticles.pt > 10.*GeV)"
HIGG3D4TPThinningTool = DerivationFramework__TrackParticleThinning(name                    = "HIGG3D4TPThinningTool",
                                                                   ThinningService         = HIGG3D4ThinningHelper.ThinningSvc(),
                                                                   SelectionString         = inclusive_track_thinning_expression,
                                                                   InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG3D4TPThinningTool
thinningTools.append(HIGG3D4TPThinningTool)

# Calo cluster thinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning

# Calo Clusters associated with Electrons
HIGG3D4ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "HIGG3D4ElectronCCThinningTool",
                                                                          ThinningService       = HIGG3D4ThinningHelper.ThinningSvc(),
                                                                          SGKey                 = "Electrons",
                                                                          TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                          SelectionString       = "Electrons.pt > 7*GeV",
                                                                          ConeSize              = 0.2)
ToolSvc += HIGG3D4ElectronCCThinningTool
thinningTools.append(HIGG3D4ElectronCCThinningTool)

# Calo Clusters associated with Muons
HIGG3D4MuonCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "HIGG3D4MuonCCThinningTool",
                                                                      ThinningService       = HIGG3D4ThinningHelper.ThinningSvc(),
                                                                      SGKey                 = "Muons",
                                                                      TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                      SelectionString       = "Muons.pt > 6*GeV",
                                                                      ConeSize              = 0.2)
ToolSvc += HIGG3D4MuonCCThinningTool
thinningTools.append(HIGG3D4MuonCCThinningTool)

# Seems to no longer be needed/causes errors

# Calo Clusters associated with AntiKt4EM Jets
#HIGG3D4AntiKt4EMCCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "HIGG3D4AntiKt4EMCCThinningTool",
#                                                                             ThinningService         = HIGG3D4ThinningHelper.ThinningSvc(),
#                                                                             SGKey                   = "AntiKt4EMTopoJets",
#                                                                             TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                             SelectionString         = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV")
#ToolSvc += HIGG3D4AntiKt4EMCCThinningTool
#thinningTools.append(HIGG3D4AntiKt4EMCCThinningTool)

###############################################################


# Truth particles
truth_cond1 = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond3 = "(abs(TruthParticles.pdgId) == 6)"  # Top quark
truth_cond4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 5*GeV))" # Photon
truth_cond5 = "((abs(TruthParticles.pdgId) >= 35) && (abs(TruthParticles.pdgId) <= 37))" # BSM Higgs

truth_expression = truth_cond1+' || '+truth_cond2 +' || '+truth_cond3 +' || '+truth_cond4 +' || '+truth_cond5

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG3D4TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HIGG3D4TruthThinningTool",
                                                                     ThinningService         = HIGG3D4ThinningHelper.ThinningSvc(),
                                                                     ParticleSelectionString = truth_expression,
                                                                     PreserveDescendants     = True,
                                                                     PreserveAncestors      = True,
                                                                     SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset)

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG3D4TruthThinningTool
    thinningTools.append(HIGG3D4TruthThinningTool)

#====================================================================
# SKIMMING TOOLS
#====================================================================

skimmingTools=[]

# preselection
electronIDRequirements = '(Electrons.DFCommonElectronsLHVeryLoose)'
electronRequirements = '(Electrons.pt > 7*GeV) && (abs(Electrons.eta) < 2.6) && '+electronIDRequirements
leadElectron = electronRequirements + ' && (Electrons.pt > 17*GeV)'
muonsRequirements = '(Muons.pt > 7*GeV) && (Muons.DFCommonGoodMuon)'
leadMuon = muonsRequirements + ' && (Muons.pt > 17*GeV)'

eeSelection = '((count('+electronRequirements+') >= 1) && (count('+leadElectron+') >= 1))'
mmSelection = '((count('+muonsRequirements+') >= 1) && (count('+leadMuon+') >= 1))'
emSelection = '(((count('+electronRequirements+') >= 1) && (count('+muonsRequirements+') >= 1)) && ((count('+leadElectron+') >= 1) || (count('+leadMuon+') >= 1)))'
expression = eeSelection+' || '+mmSelection #+' || '+emSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG3D4PreselectionSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG3D4PreselectionSkimmingTool",
                                                                               expression = expression)
ToolSvc += HIGG3D4PreselectionSkimmingTool
skimmingTools.append(HIGG3D4PreselectionSkimmingTool)

# trigger skimming
from DerivationFrameworkHiggs.HIGG3D1TriggerList import SingleLepton_Combined,Dilepton_Combined
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
triglist = SingleLepton_Combined+Dilepton_Combined
HIGG3D4InclusiveTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "HIGG3D4InclusiveTriggerSkimmingTool",TriggerListOR = triglist)

ToolSvc += HIGG3D4InclusiveTriggerSkimmingTool
skimmingTools.append(HIGG3D4InclusiveTriggerSkimmingTool)

HIGG3D4Seq = CfgMgr.AthSequencer("HIGG3D4Sequence")
HIGG3D4PreSeq = CfgMgr.AthSequencer("HIGG3D4PreSelectionSequence")

#====================================================================
# RESTORE JET COLLECTIONS REMOVED BETWEEN r20 AND r21
#====================================================================
OutputJets["HIGG3D4"] = ["AntiKt4EMPFlowJets",
                         "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810"]

reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt10LCTopoJets" # Needed for Ghost association
                 ]
if jetFlags.useTruth:
   reducedJetList += ["AntiKt4TruthJets", "AntiKt4TruthWZJets"]
replaceAODReducedJets(reducedJetList, HIGG3D4Seq,"HIGG3D4")

addDefaultTrimmedJets(HIGG3D4Seq,"HIGG3D4")

#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet
#====================================================================
addVRJets(HIGG3D4Seq)

#addVRJets(HIGG3D4Seq, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet",
#          VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track",
#          ghostArea = 0 , ptmin = 2000, ptminFilter = 7000,
#          variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")

#===================================================================
# B-TAGGING
#===================================================================
from BTagging.BTaggingFlags import BTaggingFlags

# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

#===================================================================
# Hbb tagger
#===================================================================
addHbbTagger(HIGG3D4Seq, ToolSvc)

# Add flavor tagging to the PFlow Jet collections
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = HIGG3D4Seq)

#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
HIGG3D4Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()


#========================================
# CREATE THE DERIVATION KERNEL ALGORITHMS
#========================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
HIGG3D4PreSeq += CfgMgr.DerivationFramework__DerivationKernel("HIGG3D4Kernel_skimming",
                                                           SkimmingTools = skimmingTools
                                                           )


DerivationFrameworkJob += HIGG3D4PreSeq
HIGG3D4PreSeq += HIGG3D4Seq

HIGG3D4Seq += CfgMgr.DerivationFramework__DerivationKernel("HIGG3D4Kernel_thinning",
                                                           ThinningTools = thinningTools
                                                           )


applyJetCalibration_xAODColl("AntiKt4EMTopo", HIGG3D4Seq)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG3D4SlimmingHelper = SlimmingHelper("HIGG3D4SlimmingHelper")
from DerivationFrameworkHiggs.HIGG3D4ExtraContent import *

HIGG3D4SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "TauJets",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "AntiKt4EMTopoJets",
                                          "AntiKt4EMPFlowJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "BTagging_AntiKt4EMPFlow",
                                          "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

HIGG3D4SlimmingHelper.ExtraVariables = list(HIGG3D4ExtraVariables)
HIGG3D4SlimmingHelper.AllVariables = list(HIGG3D4ExtraContainers)
HIGG3D4SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
HIGG3D4SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
HIGG3D4SlimmingHelper.AppendToDictionary = {'BTagging_AntiKt4EMPFlow':'xAOD::BTaggingContainer',
                                            'BTagging_AntiKt4EMPFlowAux':'xAOD::BTaggingAuxContainer',
                                            'AntiKtVR30Rmax4Rmin02Track':'xAOD::JetContainer',
                                            'AntiKtVR30Rmax4Rmin02TrackAux':'xAOD::JetAuxContainer',
                                            'BTagging_AntiKtVR30Rmax4Rmin02Track_201810':'xAOD::BTaggingContainer',
                                            'BTagging_AntiKtVR30Rmax4Rmin02Track_201810Aux':'xAOD::BTaggingAuxContainer'
                                           }

if globalflags.DataSource()=='geant4':
    HIGG3D4SlimmingHelper.SmartCollections += ["AntiKt4TruthJets",
                                               "AntiKt4TruthWZJets"]

    # Add special truth containers
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addBSMAndDownstreamParticles
    addStandardTruthContents()
    addBSMAndDownstreamParticles()

    HIGG3D4SlimmingHelper.AppendToDictionary = {
                                                'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                                #'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                               }
    HIGG3D4SlimmingHelper.AllVariables += list(HIGG3D4ExtraTruthContainers)
    HIGG3D4SlimmingHelper.ExtraVariables += list(HIGG3D4ExtraTruthVariables)

# Add Trigger content
HIGG3D4SlimmingHelper.IncludeMuonTriggerContent = True
HIGG3D4SlimmingHelper.IncludeEGammaTriggerContent = True

# Add MET to output stream
addMETOutputs(HIGG3D4SlimmingHelper, ["AntiKt4EMPFlow", "Track"],
                                     ["AntiKt4EMPFlow",
                                      "AntiKt4EMTopo"] # smart collections list
                                     )
addJetOutputs(HIGG3D4SlimmingHelper, ["HIGG3D4"],
                                     [], # smart collections list
                                     ["AntiKt2PV0TrackJets",
                                      "AntiKt4TruthJets",
                                      "AntiKt4TruthWZJets",
                                      "AntiKt4EMPFlowJets",
                                      "AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                                      "AntiKt10LCTopoJets",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"] #veto list
                                     )

HIGG3D4SlimmingHelper.AppendContentToStream(HIGG3D4Stream)



