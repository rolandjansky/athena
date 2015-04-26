#====================================================================
# HIGG2D5.py for JPSI filter
# This requires the reductionConf flag HIGG2D5 in Reco_tf.py   
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
HIGG2D5TPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG2D5TPThinningTool",
                                                                   ThinningService        = "HIGG2D5ThinningSvc",
                                                                   SelectionString        = thinning_expression,
                                                                   InDetTrackParticlesKey = "InDetTrackParticles",
                                                                   ApplyAnd               = True)
ToolSvc += HIGG2D5TPThinningTool
thinningTools.append(HIGG2D5TPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG2D5JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "HIGG2D5JetTPThinningTool",
                                                                         ThinningService        = "HIGG2D5ThinningSvc",
                                                                         JetKey                 = "AntiKt4LCTopoJets",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles",
                                                                         ApplyAnd               = True)
ToolSvc += HIGG2D5JetTPThinningTool
thinningTools.append(HIGG2D5JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG2D5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "HIGG2D5MuonTPThinningTool",
                                                                           ThinningService        = "HIGG2D5ThinningSvc",
                                                                           MuonKey                = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D5MuonTPThinningTool
thinningTools.append(HIGG2D5MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG2D5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "HIGG2D5ElectronTPThinningTool",
                                                                                 ThinningService        = "HIGG2D5ThinningSvc",
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                 BestMatchOnly          = False)
ToolSvc += HIGG2D5ElectronTPThinningTool
thinningTools.append(HIGG2D5ElectronTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG2D5TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "HIGG2D5TauTPThinningTool",
                                                                         ThinningService        = "HIGG2D5ThinningSvc",
                                                                         TauKey                 = "TauJets",
                                                                         ConeSize               = 0.6,
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += HIGG2D5TauTPThinningTool
thinningTools.append(HIGG2D5TauTPThinningTool)

# Truth particles
truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6))"                                      # Top quark
truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon

truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG2D5TruthThinningTool = DerivationFramework__GenericTruthThinning(name                         = "HIGG2D5TruthThinningTool", 
                                                                     ThinningService              = "HIGG2D5ThinningSvc",
                                                                     ParticleSelectionString      = truth_expression,
                                                                     PreserveDescendants          = False,
                                                                     PreserveGeneratorDescendants = True,
                                                                     PreserveAncestors            = True)

from AthenaCommon.GlobalFlags import globalflags
print "HIGG2D5.py globalflags.DataSource()", globalflags.DataSource()

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG2D5TruthThinningTool
    thinningTools.append(HIGG2D5TruthThinningTool)
print "HIGG2D5.py thinningTools", thinningTools

#====================================================================
# SKIMMING TOOLS 
#====================================================================

from AthenaCommon.BeamFlags import jobproperties
print "HIGG2D5.py jobproperties.Beam.energy()", jobproperties.Beam.energy()
SkipTriggerRequirement=((globalflags.DataSource()=='geant4') and (jobproperties.Beam.energy()==4000000.0))
# 8 TeV MC does not have trigger information
print "HIGG2D5.py SkipTriggerRequirement", SkipTriggerRequirement
TriggerJPSI = []
if not SkipTriggerRequirement:
    if jobproperties.Beam.energy()==4000000.0:
        #  8 TeV data
        TriggerJPSI               = ["EF_mu24i_tight", "EF_mu36_tight", "EF_2mu13", "EF_mu18_tight_mu8_EFFS", "EF_mu18_tight_Jpsi_EFFS"]
    if jobproperties.Beam.energy()==6500000.0:
        # 13 TeV MC
        # TriggerJPSI               = ["HLT_mu26_imedium", "HLT_mu50", "HLT_2mu14", "HLT_mu24_mu8noL1"]
        TriggerJPSI               = ["HLT_mu24_imedium", "HLT_mu26_imedium", "HLT_mu50", "HLT_2mu10", "HLT_2mu14", "HLT_mu24_mu8noL1"]
print "HIGG2D5.py TriggerJPSI", TriggerJPSI

from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG2
SkimmingToolHIGG2D5 = DerivationFramework__SkimmingToolHIGG2(name                       = "SkimmingToolHIGG2D5",
                                                             SkipTriggerRequirement     = SkipTriggerRequirement,
                                                             FilterType                 = "JPSI", 
                                                             NumberOfLeptons            = 0,
                                                             NumberOfElectrons          = 0,
                                                             NumberOfMuons              = 2,
                                                             NumberOfJets               = 0,
                                                             NumberOfPhotons            = 1,
                                                             MuonQuality                = "inMS",
                                                             TriggerJPSI                = TriggerJPSI,
                                                             MuonPtCut                  =  2.5*Units.GeV,
                                                             PhotonPtCut                = 15.0*Units.GeV,
                                                             LeadingMuonPtCut           = 15.0*Units.GeV,
                                                             InvariantMassJpsiLowCut    =  2.0*Units.GeV,
                                                             InvariantMassJpsiUpCut     =  4.3*Units.GeV,
                                                             InvariantMassUpsilonLowCut =  8.0*Units.GeV,
                                                             InvariantMassUpsilonUpCut  = 12.0*Units.GeV)
ToolSvc += SkimmingToolHIGG2D5
print SkimmingToolHIGG2D5

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG2D5Kernel",
                                                                       SkimmingTools = [SkimmingToolHIGG2D5],
                                                                       ThinningTools = thinningTools)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG2D5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG2D5Stream )
HIGG2D5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG2D5Stream.AcceptAlgs(["HIGG2D5Kernel"])
# Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="HIGG2D5ThinningSvc", outStreams=[evtStream] )
 
#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkHiggs.HIGG2D5ExtraContent import *
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG2D5SlimmingHelper = SlimmingHelper("HIGG2D5SlimmingHelper")

HIGG2D5SlimmingHelper.SmartCollections = ["Electrons",
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

HIGG2D5SlimmingHelper.ExtraVariables = HIGG2D5ExtraContent
HIGG2D5SlimmingHelper.AllVariables = HIGG2D5ExtraContainers
if globalflags.DataSource()=='geant4':
    HIGG2D5SlimmingHelper.ExtraVariables += HIGG2D5ExtraContentTruth
    HIGG2D5SlimmingHelper.AllVariables += HIGG2D5ExtraContainersTruth

# Add MET_RefFinalFix
addMETOutputs(HIGG2D5SlimmingHelper,["AntiKt4LCTopo","Track"])

HIGG2D5SlimmingHelper.IncludeMuonTriggerContent = True
HIGG2D5SlimmingHelper.IncludeEGammaTriggerContent = True

HIGG2D5SlimmingHelper.AppendContentToStream(HIGG2D5Stream)
