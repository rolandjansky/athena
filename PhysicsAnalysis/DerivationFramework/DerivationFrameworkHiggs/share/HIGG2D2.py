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
from DerivationFrameworkInDet.InDetCommon import *
import AthenaCommon.SystemOfUnits as Units

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

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG2D2JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "HIGG2D2JetTPThinningTool",
                                                                         StreamName              = streamName,
                                                                         JetKey                 = "AntiKt4LCTopoJets",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles",
                                                                         TrackSelectionString   = thinning_expression)
ToolSvc += HIGG2D2JetTPThinningTool
thinningTools.append(HIGG2D2JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG2D2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "HIGG2D2MuonTPThinningTool",
                                                                           StreamName              = streamName,
                                                                           MuonKey                = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D2MuonTPThinningTool
thinningTools.append(HIGG2D2MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "HIGG2D2ElectronTPThinningTool",
                                                                                 StreamName              = streamName,
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                 BestMatchOnly          = False)
ToolSvc += HIGG2D2ElectronTPThinningTool
thinningTools.append(HIGG2D2ElectronTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG2D2TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "HIGG2D2TauTPThinningTool",
                                                                         StreamName             = streamName,
                                                                         TauKey                 = "TauJets",
                                                                         ConeSize               = 0.6,
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D2TauTPThinningTool
thinningTools.append(HIGG2D2TauTPThinningTool)

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
HIGG2D2MuonCCThinningTool = DerivationFramework__CaloClusterThinning(name                  = "HIGG2D2MuonCCThinningTool",
                                                                     StreamName            = streamName,
                                                                     SGKey                 = "Muons",
                                                                     TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                     SelectionString       = "Muons.pt>0.*GeV",
                                                                     ConeSize              = 0.6)
ToolSvc += HIGG2D2MuonCCThinningTool
thinningTools.append(HIGG2D2MuonCCThinningTool)

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
                                                                         StreamName                   = streamName,
                                                                         ParticleSelectionString      = truth_expression,
                                                                         PreserveDescendants          = False,
                                                                         PreserveGeneratorDescendants = True,
                                                                         PreserveAncestors            = True)
else:
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    HIGG2D2TruthThinningTool = DerivationFramework__MenuTruthThinning(name                         = "HIGG2D2TruthThinningTool",
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
print (">> HIGG2D2.py globalflags.DataSource() %s" %globalflags.DataSource())

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG2D2TruthThinningTool
    thinningTools.append(HIGG2D2TruthThinningTool)
print (">> HIGG2D2.py thinningTools:") 
print (thinningTools)

#====================================================================
# SKIMMING TOOLS 
#====================================================================

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
                                                             InvariantMassCut         = 10.*Units.GeV)
ToolSvc += SkimmingToolHIGG2D2
print ('>> HIGG2D2.py SkimmingToolHIGG2D2')
print (SkimmingToolHIGG2D2)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG2D2Kernel",
                                                                       SkimmingTools = [SkimmingToolHIGG2D2],
                                                                       ThinningTools = thinningTools)

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
                                          "AntiKt4EMTopoJets",
                                          "AntiKt4LCTopoJets",
                                          "BTagging_AntiKt4EMTopo",
                                          "BTagging_AntiKt4LCTopo",
                                          "InDetTrackParticles",
                                          "PrimaryVertices"]

HIGG2D2SlimmingHelper.ExtraVariables = HIGG2D2ExtraContent
HIGG2D2SlimmingHelper.AllVariables = HIGG2D2ExtraContainers
if globalflags.DataSource()=='geant4':
    HIGG2D2SlimmingHelper.ExtraVariables += HIGG2D2ExtraContentTruth
    HIGG2D2SlimmingHelper.AllVariables += HIGG2D2ExtraContainersTruth

# Add MET_RefFinalFix
addMETOutputs(HIGG2D2SlimmingHelper,["AntiKt4LCTopo","Track"])

HIGG2D2SlimmingHelper.IncludeMuonTriggerContent = True
HIGG2D2SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG2D2SlimmingHelper.AppendContentToStream(HIGG2D2Stream)
