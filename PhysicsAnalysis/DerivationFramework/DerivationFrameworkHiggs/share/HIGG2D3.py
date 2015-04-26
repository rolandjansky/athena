#====================================================================
# HIGG2D3.py for TP filter
# This requires the reductionConf flag HIGG2D3 in Reco_tf.py   
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
HIGG2D3TPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG2D3TPThinningTool",
                                                                   ThinningService        = "HIGG2D3ThinningSvc",
                                                                   SelectionString        = thinning_expression,
                                                                   InDetTrackParticlesKey = "InDetTrackParticles",
                                                                   ApplyAnd               = True)
ToolSvc += HIGG2D3TPThinningTool
thinningTools.append(HIGG2D3TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG2D3JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "HIGG2D3JetTPThinningTool",
                                                                         ThinningService        = "HIGG2D3ThinningSvc",
                                                                         JetKey                 = "AntiKt4LCTopoJets",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles",
                                                                         ApplyAnd               = True)
ToolSvc += HIGG2D3JetTPThinningTool
thinningTools.append(HIGG2D3JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG2D3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "HIGG2D3MuonTPThinningTool",
                                                                           ThinningService        = "HIGG2D3ThinningSvc",
                                                                           MuonKey                = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D3MuonTPThinningTool
thinningTools.append(HIGG2D3MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "HIGG2D3ElectronTPThinningTool",
                                                                                 ThinningService        = "HIGG2D3ThinningSvc",
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                 BestMatchOnly          = False)
ToolSvc += HIGG2D3ElectronTPThinningTool
thinningTools.append(HIGG2D3ElectronTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG2D3TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "HIGG2D3TauTPThinningTool",
                                                                         ThinningService        = "HIGG2D3ThinningSvc",
                                                                         TauKey                 = "TauJets",
                                                                         ConeSize               = 0.6,
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D3TauTPThinningTool
thinningTools.append(HIGG2D3TauTPThinningTool)

# Truth particles
truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6))"                                      # Top quark
truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon

truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG2D3TruthThinningTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG2D3TruthThinningTool", 
                                                                     ThinningService              = "HIGG2D3ThinningSvc",
                                                                     ParticleSelectionString      = truth_expression,
                                                                     PreserveDescendants          = False,
                                                                     PreserveGeneratorDescendants = True,
                                                                     PreserveAncestors            = True)

from AthenaCommon.GlobalFlags import globalflags
print "HIGG2D3.py globalflags.DataSource()", globalflags.DataSource()

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG2D3TruthThinningTool
    thinningTools.append(HIGG2D3TruthThinningTool)
print "HIGG2D3.py thinningTools", thinningTools

#====================================================================
# SKIMMING TOOLS 
#====================================================================

from AthenaCommon.BeamFlags import jobproperties
print "HIGG2D3.py jobproperties.Beam.energy()", jobproperties.Beam.energy()
SkipTriggerRequirement=((globalflags.DataSource()=='geant4') and (jobproperties.Beam.energy()==4000000.0))
# 8 TeV MC does not have trigger information
print "HIGG2D3.py SkipTriggerRequirement", SkipTriggerRequirement
TriggerTP = []
if not SkipTriggerRequirement:
    if jobproperties.Beam.energy()==4000000.0:
        #  8 TeV data
        TriggerTP               = ["EF_e24vhi_medium1", "EF_e60_medium1", "EF_mu24i_tight", "EF_mu36_tight"]
    if jobproperties.Beam.energy()==6500000.0:
        # 13 TeV MC
        # TriggerTP               = ["HLT_e28_tight_iloose", "HLT_e60_medium", "HLT_mu26_imedium", "HLT_mu50"]
        TriggerTP               = ["HLT_e24_medium_iloose", "HLT_e24_tight_L1EM20V", "HLT_e28_tight_iloose", "HLT_e60_medium", "HLT_mu24_imedium", "HLT_mu26_imedium", "HLT_mu50"]
print "HIGG2D3.py TriggerTP", TriggerTP

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG2
SkimmingToolHIGG2D3 = DerivationFramework__SkimmingToolHIGG2(name                   = "SkimmingToolHIGG2D3",
                                                             SkipTriggerRequirement = SkipTriggerRequirement, 
                                                             FilterType             = "TP", 
                                                             NumberOfLeptons        = 2,
                                                             NumberOfElectrons      = 0,
                                                             NumberOfMuons          = 0,
                                                             NumberOfJets           = 0,
                                                             NumberOfPhotons        = 0,
                                                             ElectronQuality        = "any",
                                                             TriggerTP              = TriggerTP, 
                                                             ElectronEtCut          =  5.*Units.GeV,
                                                             MuonPtCut              =  4.*Units.GeV,
                                                             LeadingElectronEtCut   = 15.*Units.GeV,
                                                             LeadingMuonPtCut       = 15.*Units.GeV,
                                                             InvariantMassCut       = 50.*Units.GeV)
ToolSvc += SkimmingToolHIGG2D3
print SkimmingToolHIGG2D3

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG2D3Kernel",
                                                                       SkimmingTools = [SkimmingToolHIGG2D3],
                                                                       ThinningTools = thinningTools)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG2D3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG2D3Stream )
HIGG2D3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG2D3Stream.AcceptAlgs(["HIGG2D3Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG2D3ThinningSvc", outStreams=[evtStream] )
 
#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkHiggs.HIGG2D3ExtraContent import *
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG2D3SlimmingHelper = SlimmingHelper("HIGG2D3SlimmingHelper")

HIGG2D3SlimmingHelper.SmartCollections = ["Electrons",
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

HIGG2D3SlimmingHelper.ExtraVariables = HIGG2D3ExtraContent
HIGG2D3SlimmingHelper.AllVariables = HIGG2D3ExtraContainers
if globalflags.DataSource()=='geant4':
    HIGG2D3SlimmingHelper.ExtraVariables += HIGG2D3ExtraContentTruth
    HIGG2D3SlimmingHelper.AllVariables += HIGG2D3ExtraContainersTruth

# Add MET_RefFinalFix
addMETOutputs(HIGG2D3SlimmingHelper,["AntiKt4LCTopo","Track"])

HIGG2D3SlimmingHelper.IncludeMuonTriggerContent = True
HIGG2D3SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG2D3SlimmingHelper.AppendContentToStream(HIGG2D3Stream)
