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
##from DerivationFrameworkFlavourTag.FlavourTagCommon import *

from DerivationFrameworkCore.WeightMetadata import *

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
HIGG3D4ThinningHelper.TriggerChains = "HLT_mu.* | HLT_e.* | HLT_2e.* | HLT_2mu.*"
HIGG3D4ThinningHelper.AppendToStream( HIGG3D4Stream )

#================
# THINNING
#================
thinningTools=[]

# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG3D4MetTPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG3D4MetTPThinningTool",
                                                                      ThinningService        = HIGG3D4ThinningHelper.ThinningSvc(),
                                                                      SelectionString        = thinning_expression,
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      ApplyAnd               = True)
ToolSvc += HIGG3D4MetTPThinningTool
thinningTools.append(HIGG3D4MetTPThinningTool)

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
HIGG3D4TPThinningTool = DerivationFramework__TrackParticleThinning(name                    = "HIGG3D4TPThinningTool",
                                                                   ThinningService         = HIGG3D4ThinningHelper.ThinningSvc(),
                                                                   SelectionString         = "abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
                                                                   InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG3D4TPThinningTool
thinningTools.append(HIGG3D4TPThinningTool)

# Truth particles
truth_cond1 = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond3 = "(abs(TruthParticles.pdgId) == 6)"  # Top quark
truth_cond4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 5*GeV))" # Photon

truth_expression = truth_cond1+' || '+truth_cond2 +' || '+truth_cond3 +' || '+truth_cond4

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

# Calo Clusters associated with AntiKt4EM Jets
HIGG3D4AntiKt4EMCCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "HIGG3D4AntiKt4EMCCThinningTool", 
                                                                             ThinningService         = HIGG3D4ThinningHelper.ThinningSvc(), 
                                                                             SGKey                   = "AntiKt4EMTopoJets", 
                                                                             TopoClCollectionSGKey   = "CaloCalTopoClusters", 
                                                                             SelectionString         = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV")
ToolSvc += HIGG3D4AntiKt4EMCCThinningTool
thinningTools.append(HIGG3D4AntiKt4EMCCThinningTool)
###############################################################

#====================================================================
# SKIMMING TOOL 
#====================================================================
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
HIGG3D4SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG3D4SkimmingTool1",
                                                                    expression = expression)
ToolSvc += HIGG3D4SkimmingTool

higg3d4Seq = CfgMgr.AthSequencer("HIGG3d4Sequence")
higg3d4PreSeq = CfgMgr.AthSequencer("HIGG3d4PreSelectionSequence")

#====================================================================
# RESTORE JET COLLECTIONS REMOVED BETWEEN r20 AND r21
#====================================================================

reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets"]
if jetFlags.useTruth:
   reducedJetList += ["AntiKt4TruthJets", "AntiKt4TruthWZJets"]
replaceAODReducedJets(reducedJetList, higg3d4Seq,"HIGG3D4Jets")

#===================================================================
# Run b-tagging
#===================================================================
from BTagging.BTaggingFlags import BTaggingFlags

# alias for VR
# BTaggingFlags.CalibrationChannelAliases += ["AntiKt4EMTopo"]

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
# must re-tag AntiKt4LCTopoJets and AntiKt4PV0TrackJets to make JetFitterNN work with corresponding VR jets (nikola: why?)
# also, re-tag R=0.2 track jets
#FlavorTagInit( JetCollections = ["AntiKt4PV0TrackJets", "AntiKt2PV0TrackJets"], Sequencer = higg3d4Seq )
FlavorTagInit( JetCollections = ["AntiKt4PV0TrackJets"], Sequencer = higg3d4Seq )
### add back VR jets !!!!

#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
higg3d4Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
higg3d4PreSeq += CfgMgr.DerivationFramework__DerivationKernel("HIGG3D4Kernel_skimming",
                                                           SkimmingTools = [HIGG3D4SkimmingTool],
                                                           )


DerivationFrameworkJob += higg3d4PreSeq
higg3d4PreSeq += higg3d4Seq

higg3d4Seq += CfgMgr.DerivationFramework__DerivationKernel("HIGG3D4Kernel_thinning",
                                                           ThinningTools = thinningTools
                                                           )


applyJetCalibration_xAODColl("AntiKt4EMTopo", higg3d4Seq)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG3D4SlimmingHelper = SlimmingHelper("HIGG3D4SlimmingHelper")
from DerivationFrameworkHiggs.HIGG3D4ExtraContent import *

#HIGG3D4SlimmingHelper.AppendToDictionary = {
 # "AntiKtVR30Rmax4Rmin02TrackJets"                :   "xAOD::JetContainer"        ,
 # "AntiKtVR30Rmax4Rmin02TrackJetsAux"             :   "xAOD::JetAuxContainer"     ,
 # "BTagging_AntiKtVR30Rmax4Rmin02Track"           :   "xAOD::BTaggingContainer"   ,
 # "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"        :   "xAOD::BTaggingAuxContainer",
#  }

HIGG3D4SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "TauJets",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "MET_Reference_AntiKt4LCTopo",
                                          "MET_Reference_AntiKt4EMPFlow",
                                          "AntiKt4EMTopoJets",
                                          "AntiKt4LCTopoJets",
                                          "AntiKt4EMPFlowJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "BTagging_AntiKt4EMPFlow",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

HIGG3D4SlimmingHelper.ExtraVariables = list(HIGG3D4ExtraVariables)
HIGG3D4SlimmingHelper.AllVariables = list(HIGG3D4ExtraContainers)
#HIGG3D4SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()

if globalflags.DataSource()=='geant4':
    HIGG3D4SlimmingHelper.AllVariables += list(HIGG3D4ExtraTruthContainers)
    HIGG3D4SlimmingHelper.ExtraVariables += list(HIGG3D4ExtraTruthVariables)

    # Add special truth containers
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()
    HIGG3D4SlimmingHelper.StaticContent = [ "xAOD::TruthParticleContainer#TruthMuons",
                                            "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
                                            "xAOD::TruthParticleContainer#TruthElectrons",
                                            "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
                                            "xAOD::TruthParticleContainer#TruthPhotons",
                                            "xAOD::TruthParticleAuxContainer#TruthPhotonsAux.",
                                            "xAOD::TruthParticleContainer#TruthNeutrinos",
                                            "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux." ]

# Add Trigger content
HIGG3D4SlimmingHelper.IncludeMuonTriggerContent = True
HIGG3D4SlimmingHelper.IncludeEGammaTriggerContent = True

# Add MET to output stream
addMETOutputs(HIGG3D4SlimmingHelper, ["AntiKt4EMPFlow", "Track"])

HIGG3D4SlimmingHelper.AppendContentToStream(HIGG3D4Stream)














##
##
##


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

# Seems to no longer be needed/causes errors

# Calo Clusters associated with AntiKt4EM Jets
#HIGG3D1AntiKt4EMCCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "HIGG3D1AntiKt4EMCCThinningTool",
#                                                                             ThinningService         = HIGG3D1ThinningHelper.ThinningSvc(),
#                                                                             SGKey                   = "AntiKt4EMTopoJets",
#                                                                             TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                             SelectionString         = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV")
#ToolSvc += HIGG3D1AntiKt4EMCCThinningTool
#thinningTools.append(HIGG3D1AntiKt4EMCCThinningTool)

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
                                                                     PreserveDescendants     = True,
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
electronRequirements = '(Electrons.pt > 7*GeV) && (abs(Electrons.eta) < 2.6) && '+electronIDRequirements
leadElectron = electronRequirements + ' && (Electrons.pt > 17*GeV)'
muonsRequirements = '(Muons.pt > 7*GeV) && (Muons.DFCommonGoodMuon)'
leadMuon = muonsRequirements + ' && (Muons.pt > 17*GeV)'

eeSelection = '((count('+electronRequirements+') >= 2) && (count('+leadElectron+') >= 1))'
mmSelection = '((count('+muonsRequirements+') >= 2) && (count('+leadMuon+') >= 1))'

emSelection = '(((count('+electronRequirements+') >= 1) && (count('+muonsRequirements+') >= 1)) && ((count('+leadElectron+') >= 1) || (count('+leadMuon+') >= 1)))'

preselection_expression = eeSelection+' || '+mmSelection+' || '+emSelection

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
                         "AntiKtVR30Rmax4Rmin02TrackJets"]

reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt10LCTopoJets" # Needed for Ghost association
                 ]
if jetFlags.useTruth:
   reducedJetList += ["AntiKt4TruthJets", "AntiKt4TruthWZJets"]
replaceAODReducedJets(reducedJetList, higg3d1Seq,"HIGG3D1")

addDefaultTrimmedJets(higg3d1Seq,"HIGG3D1")

#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet
#====================================================================
addVRJets(higg3d1Seq)

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

#===================================================================
# Hbb tagger
#===================================================================
addHbbTagger(higg3d1Seq, ToolSvc)

# Add flavor tagging to the PFlow Jet collections
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = higg3d1Seq)

#====================================================================
# Add non-prompt lepton tagging
#====================================================================
# import the JetTagNonPromptLepton config and add to the private sequence
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
higg3d1Seq += JetTagConfig.GetDecoratePromptLeptonAlgs()


#========================================
# CREATE THE DERIVATION KERNEL ALGORITHMS
#========================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
higg3d1PreSeq += CfgMgr.DerivationFramework__DerivationKernel("HIGG3D1Kernel_skimming",
                                                           SkimmingTools = skimmingTools
                                                           )


DerivationFrameworkJob += higg3d1PreSeq
higg3d1PreSeq += higg3d1Seq

higg3d1Seq += CfgMgr.DerivationFramework__DerivationKernel("HIGG3D1Kernel_thinning",
                                                           ThinningTools = thinningTools
                                                           )


applyJetCalibration_xAODColl("AntiKt4EMTopo", higg3d1Seq)

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
                                          "AntiKt4EMPFlowJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "BTagging_AntiKt4EMPFlow",
                                          "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

HIGG3D1SlimmingHelper.ExtraVariables = list(HIGG3D1ExtraVariables)
HIGG3D1SlimmingHelper.AllVariables = list(HIGG3D1ExtraContainers)
HIGG3D1SlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
HIGG3D1SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
HIGG3D1SlimmingHelper.AppendToDictionary = {'BTagging_AntiKt4EMPFlow':'xAOD::BTaggingContainer',
                                            'BTagging_AntiKt4EMPFlowAux':'xAOD::BTaggingAuxContainer',
                                            'AntiKtVR30Rmax4Rmin02Track':'xAOD::JetContainer',
                                            'AntiKtVR30Rmax4Rmin02TrackAux':'xAOD::JetAuxContainer',
                                            'BTagging_AntiKtVR30Rmax4Rmin02Track':'xAOD::BTaggingContainer',
                                            'BTagging_AntiKtVR30Rmax4Rmin02TrackAux':'xAOD::BTaggingAuxContainer'
                                           }

if globalflags.DataSource()=='geant4':
    HIGG3D1SlimmingHelper.SmartCollections += ["AntiKt4TruthJets",
                                               "AntiKt4TruthWZJets"]

    # Add special truth containers
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addBSMAndDownstreamParticles
    addStandardTruthContents()
    addBSMAndDownstreamParticles()

    HIGG3D1SlimmingHelper.AppendToDictionary = {
                                                'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                                'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                                #'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                               }
    HIGG3D1SlimmingHelper.AllVariables += list(HIGG3D1ExtraTruthContainers)
    HIGG3D1SlimmingHelper.ExtraVariables += list(HIGG3D1ExtraTruthVariables)

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
                                      "AntiKt4TruthJets",
                                      "AntiKt4TruthWZJets",
                                      "AntiKt4EMPFlowJets",
                                      "AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                                      "AntiKt10LCTopoJets",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"] #veto list
                                     )

HIGG3D1SlimmingHelper.AppendContentToStream(HIGG3D1Stream)
