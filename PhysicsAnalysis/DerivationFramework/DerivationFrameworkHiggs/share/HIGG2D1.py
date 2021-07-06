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
from DerivationFrameworkInDet.InDetCommon import *
import AthenaCommon.SystemOfUnits as Units

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
HIGG2D1ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*|HLT_3e.*|HLT_mu.*|HLT_2mu.*|HLT_3mu.*'
HIGG2D1ThinningHelper.AppendToStream(HIGG2D1Stream)

# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG2D1JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "HIGG2D1JetTPThinningTool",
                                                                         StreamName              = streamName,
                                                                         JetKey                 = "AntiKt4LCTopoJets",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles",
                                                                         TrackSelectionString   = thinning_expression)
ToolSvc += HIGG2D1JetTPThinningTool
thinningTools.append(HIGG2D1JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG2D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "HIGG2D1MuonTPThinningTool",
                                                                           StreamName              = streamName,
                                                                           MuonKey                = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D1MuonTPThinningTool
thinningTools.append(HIGG2D1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "HIGG2D1ElectronTPThinningTool",
                                                                                 StreamName              = streamName,
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                 BestMatchOnly          = False)
ToolSvc += HIGG2D1ElectronTPThinningTool
thinningTools.append(HIGG2D1ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D1PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG2D1PhotonTPThinningTool",
                                                                                      StreamName              = streamName,
                                                                                      SGKey                   = "Photons",
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      BestMatchOnly           = True)
ToolSvc += HIGG2D1PhotonTPThinningTool
thinningTools.append(HIGG2D1PhotonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG2D1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "HIGG2D1TauTPThinningTool",
                                                                         StreamName              = streamName,
                                                                         TauKey                 = "TauJets",
                                                                         ConeSize               = 0.6,
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D1TauTPThinningTool
thinningTools.append(HIGG2D1TauTPThinningTool)

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
HIGG2D1MuonCCThinningTool = DerivationFramework__CaloClusterThinning(name                  = "HIGG2D1MuonCCThinningTool",
                                                                     StreamName            = streamName,
                                                                     SGKey                 = "Muons",
                                                                     TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                     SelectionString       = "Muons.pt>0.*GeV",
                                                                     ConeSize              = 0.6)
ToolSvc += HIGG2D1MuonCCThinningTool
thinningTools.append(HIGG2D1MuonCCThinningTool)

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
                                                                         StreamName                   = streamName,
                                                                         ParticleSelectionString      = truth_expression,
                                                                         PreserveDescendants          = False,
                                                                         PreserveGeneratorDescendants = True,
                                                                         PreserveAncestors            = True)
else:
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    HIGG2D1TruthThinningTool = DerivationFramework__MenuTruthThinning(name                         = "HIGG2D1TruthThinningTool",
                                                                      StreamName                   = streamName,
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

from AthenaCommon.GlobalFlags import globalflags
print ("HIGG2D1.py globalflags.DataSource(): %s" %globalflags.DataSource())

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG2D1TruthThinningTool
    thinningTools.append(HIGG2D1TruthThinningTool)
print ("HIGG2D1.py thinningTools: %s" %thinningTools)

#====================================================================
# SKIMMING TOOLS 
#====================================================================

## Trigger requirement 
from AthenaCommon.BeamFlags import jobproperties
print ("HIGG2D1.py jobproperties.Beam.energy(): %f GeV" %jobproperties.Beam.energy())
# 13 TeV
singleElectronTriggerRequirement=["L1_EM.*"]
diElectronTriggerRequirement=["L1_2EM.*", "L1_3EM.*"]
singleMuonTriggerRequirement=["L1_MU.*"]
diMuonTriggerRequirement=["L1_2MU.*", "L1_3MU.*"]
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
SkipTriggerRequirement=((globalflags.DataSource()=='geant4') and (jobproperties.Beam.energy()==4000000.0))
print ("HIGG2D1.py SkipTriggerRequirement: %r" %SkipTriggerRequirement)
if SkipTriggerRequirement:
    triggerRequirement=[]
print ("HIGG2D1.py triggerRequirement: %s" %triggerRequirement)

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG2
SkimmingToolHIGG2D1 = DerivationFramework__SkimmingToolHIGG2(name                     = "SkimmingToolHIGG2D1",
                                                             FilterType               = "2L", 
                                                             NumberOfLeptons          = 2,
                                                             NumberOfElectrons        = 0,
                                                             NumberOfMuons            = 0,
                                                             NumberOfJets             = 0,
                                                             NumberOfPhotons          = 0,
                                                             ElectronQuality          = "any", #"DFCommonElectronsLHVeryLoose",
                                                             MuonQuality              = "DFCommonGoodMuon",
                                                             ElectronEtCut            =  5.*Units.GeV,
                                                             MuonPtCut                =  5.*Units.GeV,
                                                             RequireTightLeptons      = True, 
                                                             TightElectronQuality     = "DFCommonElectronsLHVeryLoose", 
                                                             TightMuonQuality         = "DFCommonMuonsLoose", 
                                                             TightElectronEtCut       = 15.*Units.GeV,
                                                             TightMuonPtCut           = 15.*Units.GeV,
                                                             InvariantMassCut         =  5.*Units.GeV,
                                                             Trigger2L                = triggerRequirement)
ToolSvc += SkimmingToolHIGG2D1
print (SkimmingToolHIGG2D1)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG2D1Kernel",
                                                                       SkimmingTools = [SkimmingToolHIGG2D1],
                                                                       ThinningTools = thinningTools)

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
                                          "MET_Reference_AntiKt4LCTopo",
                                          "AntiKt4EMTopoJets",
                                          "AntiKt4LCTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "BTagging_AntiKt4LCTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

HIGG2D1SlimmingHelper.ExtraVariables = HIGG2D1ExtraContent
HIGG2D1SlimmingHelper.AllVariables = HIGG2D1ExtraContainers
if globalflags.DataSource()=='geant4':
    HIGG2D1SlimmingHelper.ExtraVariables += HIGG2D1ExtraContentTruth
    HIGG2D1SlimmingHelper.AllVariables += HIGG2D1ExtraContainersTruth

# Add MET_RefFinalFix
addMETOutputs(HIGG2D1SlimmingHelper,["AntiKt4LCTopo","Track"])

HIGG2D1SlimmingHelper.IncludeMuonTriggerContent = True
HIGG2D1SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG2D1SlimmingHelper.AppendContentToStream(HIGG2D1Stream)

