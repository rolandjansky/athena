#====================================================================
# HIGG2D2.py for 4L filter
# This requires the reductionConf flag HIGG2D2 in Reco_tf.py   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkHasTruth:
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
streamName = derivationFlags.WriteDAOD_HIGG2D2Stream.StreamName
fileName = buildFileName(derivationFlags.WriteDAOD_HIGG2D2Stream)
HIGG2D2Stream = MSMgr.NewPoolRootStream( streamName, fileName)
HIGG2D2Stream.AcceptAlgs(["HIGG2D2Kernel"])

#================
# THINNING
#================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG2D2ThinningHelper = ThinningHelper("HIGG2D2ThinningHelper")
#trigger navigation content
HIGG2D2ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_3e.*|HLT_mu.*|HLT_2mu.*|HLT_3mu.*'
HIGG2D2ThinningHelper.AppendToStream(HIGG2D2Stream)

# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG2D2TPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG2D2TPThinningTool",
                                                                   ThinningService        = HIGG2D2ThinningHelper.ThinningSvc(),
                                                                   SelectionString        = thinning_expression,
                                                                   InDetTrackParticlesKey = "InDetTrackParticles",
                                                                   ApplyAnd               = True)
ToolSvc += HIGG2D2TPThinningTool
thinningTools.append(HIGG2D2TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
# HIGG2D2LCJetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "HIGG2D2LCJetTPThinningTool",
#                                                                            ThinningService        = HIGG2D2ThinningHelper.ThinningSvc(),
#                                                                            JetKey                 = "AntiKt4LCTopoJets",
#                                                                            InDetTrackParticlesKey = "InDetTrackParticles",
#                                                                            ApplyAnd               = True)
# ToolSvc += HIGG2D2LCJetTPThinningTool
# thinningTools.append(HIGG2D2LCJetTPThinningTool)
HIGG2D2EMJetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "HIGG2D2EMJetTPThinningTool",
                                                                           ThinningService        = HIGG2D2ThinningHelper.ThinningSvc(),
                                                                           JetKey                 = "AntiKt4EMTopoJets",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles",
                                                                           ApplyAnd               = True)
ToolSvc += HIGG2D2EMJetTPThinningTool
thinningTools.append(HIGG2D2EMJetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG2D2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "HIGG2D2MuonTPThinningTool",
                                                                           ThinningService        = HIGG2D2ThinningHelper.ThinningSvc(),
                                                                           MuonKey                = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D2MuonTPThinningTool
thinningTools.append(HIGG2D2MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "HIGG2D2ElectronTPThinningTool",
                                                                                 ThinningService        = HIGG2D2ThinningHelper.ThinningSvc(),
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                 BestMatchOnly          = False)
ToolSvc += HIGG2D2ElectronTPThinningTool
thinningTools.append(HIGG2D2ElectronTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG2D2TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "HIGG2D2TauTPThinningTool",
                                                                         ThinningService        = HIGG2D2ThinningHelper.ThinningSvc(),
                                                                         TauKey                 = "TauJets",
                                                                         ConeSize               = 0.6,
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D2TauTPThinningTool
thinningTools.append(HIGG2D2TauTPThinningTool)

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
HIGG2D2ElectronCCThinningTool = DerivationFramework__CaloClusterThinning(name                  = "HIGG2D2ElectronCCThinningTool",
                                                                         ThinningService       = HIGG2D2ThinningHelper.ThinningSvc(),
                                                                         SGKey                 = "Electrons",
                                                                         TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                         SelectionString       = "Electrons.pt>0.*GeV",
                                                                         ConeSize              = 0.6)
ToolSvc += HIGG2D2ElectronCCThinningTool
thinningTools.append(HIGG2D2ElectronCCThinningTool)
HIGG2D2MuonCCThinningTool = DerivationFramework__CaloClusterThinning(name                  = "HIGG2D2MuonCCThinningTool",
                                                                     ThinningService       = HIGG2D2ThinningHelper.ThinningSvc(),
                                                                     SGKey                 = "Muons",
                                                                     TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                     SelectionString       = "Muons.pt>0.*GeV",
                                                                     ConeSize              = 0.6)
ToolSvc += HIGG2D2MuonCCThinningTool
thinningTools.append(HIGG2D2MuonCCThinningTool)
HIGG2D2TauCCThinningTool = DerivationFramework__CaloClusterThinning(name                  = "HIGG2D2TauCCThinningTool",
                                                                    ThinningService       = HIGG2D2ThinningHelper.ThinningSvc(),
                                                                    SGKey                 = "TauJets",
                                                                    TopoClCollectionSGKey = "CaloCalTopoClusters")
ToolSvc += HIGG2D2TauCCThinningTool
thinningTools.append(HIGG2D2TauCCThinningTool)

# Truth particles
useGenericTruthThinning = True
if useGenericTruthThinning:
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6))"                                      # Top quark
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon

    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'

    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    HIGG2D2TruthThinningTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG2D2TruthThinningTool", 
                                                                         ThinningService              = HIGG2D2ThinningHelper.ThinningSvc(),
                                                                         ParticleSelectionString      = truth_expression,
                                                                         PreserveDescendants          = False,
                                                                         PreserveGeneratorDescendants = True,
                                                                         PreserveAncestors            = True)
else:
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    HIGG2D2TruthThinningTool = DerivationFramework__MenuTruthThinning(name                         = "HIGG2D2TruthThinningTool",
                                                                      ThinningService              = "HIGG2D2ThinningSvc",
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

if DerivationFrameworkHasTruth:
    ToolSvc += HIGG2D2TruthThinningTool
    thinningTools.append(HIGG2D2TruthThinningTool)
print "HIGG2D2.py thinningTools", thinningTools

#====================================================================
# SKIMMING TOOLS 
#====================================================================

## Trigger requirement 
from AthenaCommon.BeamFlags import jobproperties
print "HIGG2D2.py jobproperties.Beam.energy()", jobproperties.Beam.energy()
# 13 TeV
singleElectronTriggerRequirement=["HLT_e.*"]
diElectronTriggerRequirement=["HLT_2e.*", "HLT_3e.*"]
singleMuonTriggerRequirement=["HLT_mu.*"]
diMuonTriggerRequirement=["HLT_2mu.*", "HLT_3mu.*"]
electronMuonTriggerRequirement=[]
if jobproperties.Beam.energy()==4000000.0:
    # 8 TeV
    singleElectronTriggerRequirement=["EF_e24vhi_medium1", "EF_e60_medium1"]
    diElectronTriggerRequirement=["EF_2e12Tvh_loose1", "EF_2e12Tvh_loose1_L2StarB"]
    singleMuonTriggerRequirement=["EF_mu24i_tight", "EF_mu36_tight"]
    diMuonTriggerRequirement=["EF_2mu13", "EF_mu18_tight_mu8_EFFS"]
    electronMuonTriggerRequirement=["EF_e12Tvh_medium1_mu8", "EF_e24vhi_loose1_mu8"]
triggerRequirement=singleElectronTriggerRequirement+diElectronTriggerRequirement+singleMuonTriggerRequirement+diMuonTriggerRequirement+electronMuonTriggerRequirement
# 8 TeV MC does not have trigger information
# Note presence of MC truth being used as a proxy for MC - should be ok for 8 TeV MC
SkipTriggerRequirement=(DerivationFrameworkHasTruth and (jobproperties.Beam.energy()==4000000.0))
print "HIGG2D2.py SkipTriggerRequirement", SkipTriggerRequirement
if SkipTriggerRequirement:
    triggerRequirement=[]
print "HIGG2D2.py triggerRequirement", triggerRequirement

Do4LVertexing = True

if Do4LVertexing:
    include('DerivationFrameworkHiggs/configureVertexFitter.py')
else:
    TrkVKalVrtFitter = None
print "HIGG2D2.py Do4LVertexing", Do4LVertexing

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG2
SkimmingToolHIGG2D2 = DerivationFramework__SkimmingToolHIGG2(name                     = "SkimmingToolHIGG2D2",
                                                             FilterType               = "4L", 
                                                             NumberOfLeptons          = 4,
                                                             NumberOfElectrons        = 0,
                                                             NumberOfMuons            = 0,
                                                             NumberOfJets             = 0,
                                                             NumberOfPhotons          = 0,
                                                             ElectronQuality          = "any",
                                                             PrimaryElectronQuality4L = "DFCommonElectronsLHVeryLoose", 
                                                             ElectronEtCut            =  5.*Units.GeV,
                                                             MuonPtCut                =  4.*Units.GeV,
                                                             JetPtCut                 = 15.*Units.GeV,
                                                             InvariantMassCut         = 10.*Units.GeV,
                                                             Trigger4L                = triggerRequirement,
                                                             DoVertexing              = Do4LVertexing,
                                                             DoVertexing_el_withOrigInDetTrack = False,
                                                             VertexFitter             = TrkVKalVrtFitter)
ToolSvc += SkimmingToolHIGG2D2
print SkimmingToolHIGG2D2

augmentationTools = []
if Do4LVertexing:
    ToolSvc += SkimmingToolHIGG2D2
    augmentationTools.append(SkimmingToolHIGG2D2)

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
higg2d2Seq = CfgMgr.AthSequencer("HIGG2D2Sequence")

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
higg2d2Seq += CfgMgr.DerivationFramework__DerivationKernel("HIGG2D2Kernel",
                                                           SkimmingTools = [SkimmingToolHIGG2D2],
                                                           ThinningTools = thinningTools,
                                                           AugmentationTools = augmentationTools)

DerivationFrameworkJob += higg2d2Seq

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkHiggs.HIGG2D2ExtraContent import *
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG2D2SlimmingHelper = SlimmingHelper("HIGG2D2SlimmingHelper")

HIGG2D2SlimmingHelper.SmartCollections = ["Electrons",
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
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

HIGG2D2SlimmingHelper.ExtraVariables = HIGG2D2ExtraContent
HIGG2D2SlimmingHelper.AllVariables = HIGG2D2ExtraContainers
if DerivationFrameworkHasTruth:
    HIGG2D2SlimmingHelper.ExtraVariables += HIGG2D2ExtraContentTruth
    HIGG2D2SlimmingHelper.AllVariables += HIGG2D2ExtraContainersTruth

# Add MET_RefFinalFix
addMETOutputs(HIGG2D2SlimmingHelper,["AntiKt4LCTopo","Track"])

HIGG2D2SlimmingHelper.IncludeMuonTriggerContent = True
HIGG2D2SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG2D2SlimmingHelper.AppendContentToStream(HIGG2D2Stream)

if Do4LVertexing:
    HIGG2D2Stream.AddItem("xAOD::VertexContainer#FourLeptonVertices")
    HIGG2D2Stream.AddItem("xAOD::VertexAuxContainer#FourLeptonVerticesAux.-vxTrackAtVertex")
