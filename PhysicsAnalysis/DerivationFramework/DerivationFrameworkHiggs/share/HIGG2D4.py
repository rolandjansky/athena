#====================================================================
# HIGG2D4.py for 2L2Q filter
# This requires the reductionConf flag HIGG2D4 in Reco_tf.py   
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

#================
# THINNING
#================
thinningTools=[]
# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG2D4TPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG2D4TPThinningTool",
                                                                   ThinningService        = "HIGG2D4ThinningSvc",
                                                                   SelectionString        = thinning_expression,
                                                                   InDetTrackParticlesKey = "InDetTrackParticles",
                                                                   ApplyAnd               = True)
ToolSvc += HIGG2D4TPThinningTool
thinningTools.append(HIGG2D4TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG2D4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "HIGG2D4JetTPThinningTool",
                                                                         ThinningService        = "HIGG2D4ThinningSvc",
                                                                         JetKey                 = "AntiKt4LCTopoJets",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles",
                                                                         ApplyAnd               = True)
ToolSvc += HIGG2D4JetTPThinningTool
thinningTools.append(HIGG2D4JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG2D4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "HIGG2D4MuonTPThinningTool",
                                                                           ThinningService        = "HIGG2D4ThinningSvc",
                                                                           MuonKey                = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D4MuonTPThinningTool
thinningTools.append(HIGG2D4MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "HIGG2D4ElectronTPThinningTool",
                                                                                 ThinningService        = "HIGG2D4ThinningSvc",
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                 BestMatchOnly          = False)
ToolSvc += HIGG2D4ElectronTPThinningTool
thinningTools.append(HIGG2D4ElectronTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG2D4TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "HIGG2D4TauTPThinningTool",
                                                                         ThinningService        = "HIGG2D4ThinningSvc",
                                                                         TauKey                 = "TauJets",
                                                                         ConeSize               = 0.6,
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D4TauTPThinningTool
thinningTools.append(HIGG2D4TauTPThinningTool)

# Truth particles
truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6))"                                      # Top quark
truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon

truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG2D4TruthThinningTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG2D4TruthThinningTool", 
                                                                     ThinningService              = "HIGG2D4ThinningSvc",
                                                                     ParticleSelectionString      = truth_expression,
                                                                     PreserveDescendants          = False,
                                                                     PreserveGeneratorDescendants = True,
                                                                     PreserveAncestors            = True)

from AthenaCommon.GlobalFlags import globalflags
print "HIGG2D4.py globalflags.DataSource()", globalflags.DataSource()

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG2D4TruthThinningTool
    thinningTools.append(HIGG2D4TruthThinningTool)
print "HIGG2D4.py thinningTools", thinningTools

#====================================================================
# SKIMMING TOOLS 
#====================================================================

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG2
SkimmingToolHIGG2D4 = DerivationFramework__SkimmingToolHIGG2(name                    = "SkimmingToolHIGG2D4",
                                                             FilterType              = "2L2Q", 
                                                             NumberOfLeptons         = 2,
                                                             NumberOfElectrons       = 0,
                                                             NumberOfMuons           = 0,
                                                             JetContainerKey         = "AntiKt4EMTopoJets",
                                                             NumberOfJets            = 2,
                                                             JetPtCut                = 0.*Units.GeV,
                                                             JetEtaCut               = 2.6, 
                                                             MergedJetContainerKey0  = "AntiKt4EMTopoJets",
                                                             NumberOfMergedJets0     = 1,
                                                             MergedJetPtCut0         = 100.*Units.GeV,
                                                             MergedJetEtaCut0        = 2.6, 
                                                             MergedJetContainerKey1  = "AntiKt10LCTopoJets",
                                                             NumberOfMergedJets1     = 1,
                                                             MergedJetPtCut1         = 150.*Units.GeV,
                                                             MergedJetEtaCut1        = 2.6, 
                                                             MergedJetContainerKey2  = "CamKt12LCTopoJets",
                                                             NumberOfMergedJets2     = 1,
                                                             MergedJetPtCut2         = 150.*Units.GeV,
                                                             MergedJetEtaCut2        = 2.6, 
                                                             NumberOfPhotons         = 0,
                                                             ElectronQuality         = "any",
                                                             ElectronEtCut           = 6.*Units.GeV,
                                                             MuonPtCut               = 6.*Units.GeV,
                                                             InvariantMassCut        = 5.*Units.GeV,
                                                             DRElectronJetCut        = 0.05)
ToolSvc += SkimmingToolHIGG2D4
print SkimmingToolHIGG2D4

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG2D4Kernel",
                                                                       SkimmingTools = [SkimmingToolHIGG2D4],
                                                                       ThinningTools = thinningTools)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG2D4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG2D4Stream )
HIGG2D4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG2D4Stream.AcceptAlgs(["HIGG2D4Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG2D4ThinningSvc", outStreams=[evtStream] )
 
#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkHiggs.HIGG2D4ExtraContent import *
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG2D4SlimmingHelper = SlimmingHelper("HIGG2D4SlimmingHelper")

HIGG2D4SlimmingHelper.SmartCollections = ["Electrons",
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

HIGG2D4SlimmingHelper.ExtraVariables = HIGG2D4ExtraContent
HIGG2D4SlimmingHelper.AllVariables = HIGG2D4ExtraContainers
if globalflags.DataSource()=='geant4':
    HIGG2D4SlimmingHelper.ExtraVariables += HIGG2D4ExtraContentTruth
    HIGG2D4SlimmingHelper.AllVariables += HIGG2D4ExtraContainersTruth

# Add MET_RefFinalFix
addMETOutputs(HIGG2D4SlimmingHelper,["AntiKt4LCTopo","Track"])

HIGG2D4SlimmingHelper.IncludeMuonTriggerContent = True
HIGG2D4SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG2D4SlimmingHelper.AppendContentToStream(HIGG2D4Stream)

