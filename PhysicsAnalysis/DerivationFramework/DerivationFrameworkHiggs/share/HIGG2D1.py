#====================================================================
# HIGG2D1.py for 2L filter
# This requires the reductionConf flag HIGG2D1 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkHiggs.TruthCategories import *
import AthenaCommon.SystemOfUnits as Units

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG2D1Stream.StreamName
fileName = buildFileName(derivationFlags.WriteDAOD_HIGG2D1Stream)
HIGG2D1Stream = MSMgr.NewPoolRootStream( streamName, fileName)
HIGG2D1Stream.AcceptAlgs(["HIGG2D1Kernel"])

#================
# THINNING
#================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG2D1ThinningHelper = ThinningHelper("HIGG2D1ThinningHelper")
#trigger navigation content
HIGG2D1ThinningHelper.TriggerChains = "^(?!.*_[0-9]*(j|xe|tau|ht|xs|te))HLT_(([2-4]*e.*)|([2-4]*mu.*))"
# Triggers starting with HLT_e, HLT_2e, HLT_3e, HLT_4e, HLT_mu, HLT_2mu, HLT_3mu, HLT_4mu
# and not containing any of _j, _xe, _tau, _ht, _xs and _te.
# https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DaodRecommendations#Trigger
HIGG2D1ThinningHelper.AppendToStream(HIGG2D1Stream)

# ID tracks (https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DaodRecommendations#Tracking)
thinning_expression = "(InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV) < 1.5*mm && InDetTrackParticles.pt > 10*GeV)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG2D1TPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG2D1TPThinningTool",
                                                                   ThinningService        = HIGG2D1ThinningHelper.ThinningSvc(),
                                                                   SelectionString        = thinning_expression,
                                                                   InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D1TPThinningTool
thinningTools.append(HIGG2D1TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG2D1EMJetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "HIGG2D1EMJetTPThinningTool",
                                                                           ThinningService        = HIGG2D1ThinningHelper.ThinningSvc(),
                                                                           JetKey                 = "AntiKt4EMTopoJets",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D1EMJetTPThinningTool
thinningTools.append(HIGG2D1EMJetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG2D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "HIGG2D1MuonTPThinningTool",
                                                                           ThinningService        = HIGG2D1ThinningHelper.ThinningSvc(),
                                                                           MuonKey                = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D1MuonTPThinningTool
thinningTools.append(HIGG2D1MuonTPThinningTool)

# Tracks around (dR<0.4) Muons with pT>4 GeV
HIGG2D1MuonTPThinningTool2 = DerivationFramework__MuonTrackParticleThinning(name                   = "HIGG2D1MuonTPThinningTool2",
                                                                            ThinningService        = HIGG2D1ThinningHelper.ThinningSvc(),
                                                                            MuonKey                = "Muons",
                                                                            SelectionString        = "Muons.pt>4.*GeV",
                                                                            ConeSize               = 0.4,
                                                                            InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D1MuonTPThinningTool2
thinningTools.append(HIGG2D1MuonTPThinningTool2)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "HIGG2D1ElectronTPThinningTool",
                                                                                 ThinningService        = HIGG2D1ThinningHelper.ThinningSvc(),
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                 BestMatchOnly          = False)
ToolSvc += HIGG2D1ElectronTPThinningTool
thinningTools.append(HIGG2D1ElectronTPThinningTool)

# Tracks around (dR<0.4) Electrons with pT>4 GeV
HIGG2D1ElectronTPThinningTool2 = DerivationFramework__EgammaTrackParticleThinning(name                   = "HIGG2D1ElectronTPThinningTool2",
                                                                                  ThinningService        = HIGG2D1ThinningHelper.ThinningSvc(),
                                                                                  SGKey                  = "Electrons",
                                                                                  SelectionString        = "Electrons.pt>4.*GeV",
                                                                                  ConeSize               = 0.4,
                                                                                  InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                  BestMatchOnly          = False)
ToolSvc += HIGG2D1ElectronTPThinningTool2
thinningTools.append(HIGG2D1ElectronTPThinningTool2)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D1PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG2D1PhotonTPThinningTool",
                                                                                      ThinningService         = HIGG2D1ThinningHelper.ThinningSvc(),
                                                                                      SGKey                   = "Photons",
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      BestMatchOnly           = True)
ToolSvc += HIGG2D1PhotonTPThinningTool
thinningTools.append(HIGG2D1PhotonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG2D1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "HIGG2D1TauTPThinningTool",
                                                                         ThinningService        = HIGG2D1ThinningHelper.ThinningSvc(),
                                                                         TauKey                 = "TauJets",
                                                                         ConeSize               = 0.6,
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D1TauTPThinningTool
thinningTools.append(HIGG2D1TauTPThinningTool)

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
HIGG2D1ElectronCCThinningTool = DerivationFramework__CaloClusterThinning(name                  = "HIGG2D1ElectronCCThinningTool",
                                                                         ThinningService       = HIGG2D1ThinningHelper.ThinningSvc(),
                                                                         SGKey                 = "Electrons",
                                                                         TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                         SelectionString       = "Electrons.pt>0.*GeV",
                                                                         ConeSize              = 0.6)
ToolSvc += HIGG2D1ElectronCCThinningTool
thinningTools.append(HIGG2D1ElectronCCThinningTool)
HIGG2D1MuonCCThinningTool = DerivationFramework__CaloClusterThinning(name                  = "HIGG2D1MuonCCThinningTool",
                                                                     ThinningService       = HIGG2D1ThinningHelper.ThinningSvc(),
                                                                     SGKey                 = "Muons",
                                                                     TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                     SelectionString       = "Muons.pt>0.*GeV",
                                                                     ConeSize              = 0.6)
ToolSvc += HIGG2D1MuonCCThinningTool
thinningTools.append(HIGG2D1MuonCCThinningTool)
HIGG2D1TauCCThinningTool = DerivationFramework__CaloClusterThinning(name                  = "HIGG2D1TauCCThinningTool",
                                                                    ThinningService       = HIGG2D1ThinningHelper.ThinningSvc(),
                                                                    SGKey                 = "TauJets",
                                                                    TopoClCollectionSGKey = "CaloCalTopoClusters")
ToolSvc += HIGG2D1TauCCThinningTool
thinningTools.append(HIGG2D1TauCCThinningTool)

# Truth particles
useGenericTruthThinning = True
if useGenericTruthThinning:
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6))"                                      # Top quark
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon

    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    HIGG2D1TruthThinningTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG2D1TruthThinningTool", 
                                                                         ThinningService              = HIGG2D1ThinningHelper.ThinningSvc(),
                                                                         ParticleSelectionString      = truth_expression,
                                                                         PreserveDescendants          = False,
                                                                         PreserveGeneratorDescendants = True,
                                                                         PreserveAncestors            = True)
else:
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    HIGG2D1TruthThinningTool = DerivationFramework__MenuTruthThinning(name                         = "HIGG2D1TruthThinningTool",
                                                                      ThinningService              = "HIGG2D1ThinningSvc",
                                                                      WritePartons                 = False,
                                                                      WriteHadrons                 = False,
                                                                      WriteBHadrons                = True,
                                                                      WriteGeant                   = False,
                                                                      GeantPhotonPtThresh          = -1.0,
                                                                      WriteTauHad                  = True,
                                                                      PartonPtThresh               = -1.0,
                                                                      WriteBSM                     = True,
                                                                      WriteBosons                  = True,
                                                                      WriteBSMProducts             = True,
                                                                      WriteBosonProducts           = True,
                                                                      WriteTopAndDecays            = True,
                                                                      WriteEverything              = False,
                                                                      WriteAllLeptons              = True,
                                                                      WriteStatus3                 = False,
                                                                      PreserveGeneratorDescendants = True,
                                                                      WriteFirstN                  = -1)

if DerivationFrameworkIsMonteCarlo:
    ToolSvc += HIGG2D1TruthThinningTool
    thinningTools.append(HIGG2D1TruthThinningTool)
print "HIGG2D1.py thinningTools", thinningTools

#====================================================================
# SKIMMING TOOLS 
#====================================================================

## Trigger requirement 
from AthenaCommon.BeamFlags import jobproperties
print "HIGG2D1.py jobproperties.Beam.energy()", jobproperties.Beam.energy()
triggerRequirement=[HIGG2D1ThinningHelper.TriggerChains]
print "HIGG2D1.py triggerRequirement", triggerRequirement

Do4LVertexing = True

if Do4LVertexing:
    include( 'DerivationFrameworkHiggs/configureVertexFitter.py' )
else:
    TrkVKalVrtFitter = None    
print "HIGG2D1.py Do4LVertexing", Do4LVertexing

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG2
SkimmingToolHIGG2D1 = DerivationFramework__SkimmingToolHIGG2(name                     = "SkimmingToolHIGG2D1",
                                                             FilterType               = "2L", 
                                                             NumberOfLeptons          = 2,
                                                             NumberOfElectrons        = 0,
                                                             NumberOfMuons            = 0,
                                                             NumberOfJets             = 0,
                                                             NumberOfPhotons          = 0,
                                                             ElectronQuality          = "any",
                                                             MuonQuality              = "DFCommonGoodMuon",
                                                             ElectronEtCut            =  5.*Units.GeV,
                                                             MuonPtCut                =  5.*Units.GeV,
                                                             RequireTightLeptons      = True, 
                                                             TightElectronQuality     = "DFCommonElectronsLHLoose",
                                                             TightMuonQuality         = "DFCommonMuonsPreselection", 
                                                             TightElectronEtCut       = 15.*Units.GeV,
                                                             TightMuonPtCut           = 15.*Units.GeV,
                                                             InvariantMassCut         =  5.*Units.GeV,
                                                             Trigger2L                = triggerRequirement,
                                                             DoVertexing              = Do4LVertexing,
                                                             DoVertexing_el_withOrigInDetTrack = False,
                                                             VertexFitter             = TrkVKalVrtFitter)
ToolSvc += SkimmingToolHIGG2D1
print SkimmingToolHIGG2D1

augmentationTools = []
if Do4LVertexing:
    ToolSvc += SkimmingToolHIGG2D1
    augmentationTools.append(SkimmingToolHIGG2D1)

# Custom muon isolation requested on 08/03/2018
include("DerivationFrameworkHiggs/HIGG2D1CustomMuonIsolation.py")

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
higg2d1Seq = CfgMgr.AthSequencer("HIGG2D1Sequence")

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
higg2d1Seq += CfgMgr.DerivationFramework__DerivationKernel("HIGG2D1Kernel",
                                                           SkimmingTools = [SkimmingToolHIGG2D1],
                                                           ThinningTools = thinningTools, 
                                                           AugmentationTools = augmentationTools)

DerivationFrameworkJob += higg2d1Seq

#===================================================================
# Tag custom or pre-built jet collections
#===================================================================

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = higg2d1Seq)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkHiggs.HIGG2D1ExtraContent import *
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG2D1SlimmingHelper = SlimmingHelper("HIGG2D1SlimmingHelper")

HIGG2D1SlimmingHelper.SmartCollections = ["Electrons",
                                          "Photons",
                                          "Muons",
                                          "TauJets",
                                          "MET_Reference_AntiKt4EMTopo",
                                          "MET_Reference_AntiKt4EMPFlow",
                                          "AntiKt4EMTopoJets",
                                          "AntiKt4EMPFlowJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "BTagging_AntiKt4EMPFlow",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]
if DerivationFrameworkIsMonteCarlo:
        # https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DaodRecommendations#Jets_MET
        HIGG2D1SlimmingHelper.SmartCollections += ["AntiKt4TruthJets",
                                                   "AntiKt4TruthWZJets",
                                                   "AntiKt4TruthDressedWZJets"]

HIGG2D1SlimmingHelper.ExtraVariables = HIGG2D1ExtraContent
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
HIGG2D1SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
HIGG2D1SlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent
HIGG2D1SlimmingHelper.AllVariables = HIGG2D1ExtraContainers
HIGG2D1SlimmingHelper.AppendToDictionary = {'BTagging_AntiKt4EMPFlow':'xAOD::BTaggingContainer',
                                            'BTagging_AntiKt4EMPFlowAux':'xAOD::BTaggingAuxContainer'}
if DerivationFrameworkIsMonteCarlo:
    HIGG2D1SlimmingHelper.ExtraVariables += HIGG2D1ExtraContentTruth
    HIGG2D1SlimmingHelper.AllVariables += HIGG2D1ExtraContainersTruth
    HIGG2D1SlimmingHelper.AppendToDictionary.update({'TruthTop':'xAOD::TruthParticleContainer',
                                                     'TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                                     'TruthBSM':'xAOD::TruthParticleContainer',
                                                     'TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                                     'TruthBoson':'xAOD::TruthParticleContainer',
                                                     'TruthBosonAux':'xAOD::TruthParticleAuxContainer'})

HIGG2D1SlimmingHelper.IncludeMuonTriggerContent = True
HIGG2D1SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG2D1SlimmingHelper.AppendContentToStream(HIGG2D1Stream)
if Do4LVertexing:
    HIGG2D1Stream.AddItem("xAOD::VertexContainer#FourLeptonVertices")
    HIGG2D1Stream.AddItem("xAOD::VertexAuxContainer#FourLeptonVerticesAux.-vxTrackAtVertex")
