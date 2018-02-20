# **************************************************************************************************************************
# TAUP2.py 
# reductionConf flag TAUP2 in Reco_tf.py   
# **************************************************************************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()


# =============================================
# Private sequence here
# =============================================
TAUP2seq = CfgMgr.AthSequencer("TAUP2Sequence")
DerivationFrameworkJob += TAUP2seq

# =============================================
# Set up stream
# =============================================
streamName      = derivationFlags.WriteDAOD_TAUP2Stream.StreamName
fileName        = buildFileName( derivationFlags.WriteDAOD_TAUP2Stream )
TAUP2Stream     = MSMgr.NewPoolRootStream( streamName, fileName )
TAUP2Stream.AcceptAlgs(["TAUP2Kernel"])


# =============================================
# Thinning tool
# =============================================
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
TAUP2ThinningHelper                              = ThinningHelper( "TAUP2ThinningHelper" )
TAUP2ThinningHelper.TriggerChains                = "HLT_e24.*|HLT_e60.*"
TAUP2ThinningHelper.AppendToStream( TAUP2Stream )

thinningTools = []

# MET/Jet tracks
thinning_expression     = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
TAUP2MetTPThinningTool  = DerivationFramework__TrackParticleThinning(
  name                    = "TAUP2MetTPThinningTool",
  ThinningService         = TAUP2ThinningHelper.ThinningSvc(),
  SelectionString         = thinning_expression,
  InDetTrackParticlesKey  = "InDetTrackParticles",
  ApplyAnd                = True)

ToolSvc += TAUP2MetTPThinningTool
thinningTools.append(TAUP2MetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
TAUP2JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(
  name                    = "TAUP2JetTPThinningTool",
  ThinningService         = TAUP2ThinningHelper.ThinningSvc(),
  JetKey                  = "AntiKt4LCTopoJets",
  InDetTrackParticlesKey  = "InDetTrackParticles",
  ApplyAnd                = True)

ToolSvc += TAUP2JetTPThinningTool
thinningTools.append(TAUP2JetTPThinningTool)

# Tracks associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
TAUP2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(
  name                      = "TAUP2ElectronTPThinningTool",
  ThinningService           = TAUP2ThinningHelper.ThinningSvc(),
  SGKey                     = "Electrons",
  InDetTrackParticlesKey    = "InDetTrackParticles")

ToolSvc  += TAUP2ElectronTPThinningTool
thinningTools.append(TAUP2ElectronTPThinningTool)

# Tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
TAUP2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(
  name                    = "TAUP2MuonTPThinningTool",
  ThinningService         = TAUP2ThinningHelper.ThinningSvc(),
  MuonKey                 = "Muons",
  InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc  += TAUP2MuonTPThinningTool
thinningTools.append(TAUP2MuonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
TAUP2TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(
  name                    = "TAUP2TauTPThinningTool",
  ThinningService         = TAUP2ThinningHelper.ThinningSvc(),
  TauKey                  = "TauJets",
  # SelectionString         = "TauJets.pt > 12*GeV",
  InDetTrackParticlesKey  = "InDetTrackParticles",
  ConeSize                = 0.6)

ToolSvc  += TAUP2TauTPThinningTool
thinningTools.append(TAUP2TauTPThinningTool)


# =============================================
# Skimming tool
# =============================================
tauRequirement = "( count( (TauJets.pt > 12.0*GeV || TauJets.ptFinalCalib > 12.0*GeV ) && (abs(TauJets.eta) < 2.6) ) >= 1 )"
expression     = tauRequirement

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
TAUP2SkimmingTool   = DerivationFramework__xAODStringSkimmingTool(
  name                = "TAUP2SkimmingTool",
  expression          = expression)

ToolSvc += TAUP2SkimmingTool

# =============================================
# Standard jets
# =============================================
if globalflags.DataSource() == "geant4":
  print 'Adding AntiKt4TruthJets here'
  #addStandardJets("AntiKt", 0.4, "Truth", 5000, mods="truth_ungroomed", algseq=TAUP2seq, outputGroup="TAUP2")
  reducedJetList = ["AntiKt4TruthJets"]
  replaceAODReducedJets(reducedJetList,TAUP2seq, "TAUP2")  
  from DerivationFrameworkTau.TauTruthCommon import *

# =============================================
# Create derivation Kernel
# =============================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

TAUP2seq += CfgMgr.DerivationFramework__DerivationKernel(
  "TAUP2Kernel",
  SkimmingTools             = [TAUP2SkimmingTool],
  ThinningTools             = thinningTools
  )

# ===============================================
# Add the containers to the output stream (slimming done here)
# ===============================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkTau.TAUPExtraContent import *

TAUP2SlimmingHelper  = SlimmingHelper("TAUP2SlimmingHelper")
TAUP2SlimmingHelper.SmartCollections  = ["Electrons",
                                         # "Photons",
                                         "Muons"]
if DerivationFrameworkIsMonteCarlo:
  TAUP2SlimmingHelper.StaticContent   = ["xAOD::TruthParticleContainer#TruthElectrons",
                                         "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
                                         "xAOD::TruthParticleContainer#TruthMuons",
                                         "xAOD::TruthParticleAuxContainer#TruthMuonsAux."]

#TAUP2SlimmingHelper.ExtraVariables               = ExtraContentTAUP2
TAUP2SlimmingHelper.AllVariables                 = ExtraContainersTAUP2

if globalflags.DataSource() == "geant4":
  #TAUP2SlimmingHelper.ExtraVariables            += ExtraContentTruthTAUP2
  TAUP2SlimmingHelper.ExtraVariables            = ExtraContentTruthTAUP2
  TAUP2SlimmingHelper.AllVariables              += ExtraContainersTruthTAUP2

TAUP2SlimmingHelper.AppendContentToStream(TAUP2Stream)

TAUP2Stream.AddItem("xAOD::EventShape#*")
TAUP2Stream.AddItem("xAOD::EventShapeAuxInfo#*")

