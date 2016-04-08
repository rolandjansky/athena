#********************************************************************
# EXOT11.py 
# reductionConf flag EXOT11 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

from DerivationFrameworkCore.WeightMetadata import *

from JetRec.JetRecStandard import jtm

from AthenaCommon.GlobalFlags import globalflags
isMC = False
if globalflags.DataSource()=='geant4':
  isMC = True

exot11Seq = CfgMgr.AthSequencer("EXOT11Sequence")

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools = []

#Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT11ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                   = "EXOT11ElectronTPThinningTool",
                                                                                 ThinningService        = "EXOT11ThinningSvc",
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT11ElectronTPThinningTool
thinningTools.append(EXOT11ElectronTPThinningTool)

#Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT11MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                  = "EXOT11MuonTPThinningTool",
                                                                           ThinningService       = "EXOT11ThinningSvc",
                                                                           MuonKey               = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT11MuonTPThinningTool
thinningTools.append(EXOT11MuonTPThinningTool)

#Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT11AKt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EXOT11AKt4JetTPThinningTool",
                                                                             ThinningService         = "EXOT11ThinningSvc",
                                                                             JetKey                  = "AntiKt4LCTopoJets",
                                                                             SelectionString         = "AntiKt4LCTopoJets.pt > 15*GeV && abs(AntiKt4LCTopoJets.eta) < 2.8",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT11AKt4JetTPThinningTool
thinningTools.append(EXOT11AKt4JetTPThinningTool)

EXOT11AKt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EXOT11AKt10JetTPThinningTool",
                                                                              ThinningService         = "EXOT11ThinningSvc",
                                                                              JetKey                  = "AntiKt10LCTopoJets",
                                                                              SelectionString         = "AntiKt10LCTopoJets.pt > 200*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT11AKt10JetTPThinningTool
thinningTools.append(EXOT11AKt10JetTPThinningTool)

#Truth Thinning (!!! NOT WORKING FOR athena 19)
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT11MenuHadThinningTool = DerivationFramework__MenuTruthThinning( name = "EXOT11MenuHadThinningTool",
                                                                 ThinningService = "EXOT11ThinningSvc",
                                                                 WritePartons               = False,#?
                                                                 WriteHadrons               = True,
                                                                 WriteBHadrons              = True,
                                                                 WriteGeant                 = False,
                                                                 GeantPhotonPtThresh        = -1.0,
                                                                 WriteTauHad                = False,#?
                                                                 PartonPtThresh             = -1.0,
                                                                 WriteBSM                   = False,
                                                                 WriteBosons                = False,
                                                                 WriteBSMProducts           = False,
                                                                 WriteTopAndDecays          = False,
                                                                 WriteEverything            = False,
                                                                 WriteAllLeptons            = False,
                                                                 WriteLeptonsNotFromHadrons = True,
                                                                 WriteStatus3               = False,
                                                                 WriteFirstN                = -1,
                                                                 PreserveDescendants        = False)

EXOT11MenuThinningTool = DerivationFramework__MenuTruthThinning( name = "EXOT11MenuThinningTool",
                                                                 ThinningService = "EXOT11ThinningSvc",
                                                                 WritePartons               = False,#?
                                                                 WriteHadrons               = False,
                                                                 WriteBHadrons              = False,
                                                                 WriteGeant                 = False,
                                                                 GeantPhotonPtThresh        = -1.0,
                                                                 WriteTauHad                = False,
                                                                 PartonPtThresh             = -1.0,
                                                                 WriteBSM                   = True,
                                                                 WriteBosons                = True,
                                                                 WriteBSMProducts           = True,
                                                                 WriteTopAndDecays          = True,
                                                                 WriteEverything            = False,
                                                                 WriteAllLeptons            = False,
                                                                 WriteLeptonsNotFromHadrons = True,
                                                                 WriteStatus3               = False,
                                                                 WriteFirstN                = -1,
                                                                 PreserveDescendants        = True)

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT11GenThinningTool = DerivationFramework__GenericTruthThinning( name = "EXOT11GenThinningTool",
                                                                   ThinningService = "EXOT11ThinningSvc",
                                                                   ParticleSelectionString = "abs(TruthParticles.pdgId) ==34 || abs(TruthParticles.pdgId)==5100039", 
                                                                   PreserveDescendants = False)

if isMC:
  ToolSvc += EXOT11MenuHadThinningTool
  thinningTools.append(EXOT11MenuHadThinningTool)
  ToolSvc += EXOT11MenuThinningTool
  thinningTools.append(EXOT11MenuThinningTool)
  ToolSvc += EXOT11GenThinningTool
  thinningTools.append(EXOT11GenThinningTool)

#=======================================
# SKIMMING TOOL   
#=======================================

#trig = " (HLT_e28_tight_iloose || HLT_e60_medium1)|| (HLT_mu26_imedium || HLT_mu50) "
selection_e = "(count((Electrons.Medium) && (Electrons.pt > 20*GeV)) > 0)"
selection_mu = "(count(Muons.pt > 20*GeV) > 0)"
selection_j = "( (count(CamKt12LCTopoJets.pt > 200*GeV) > 0) || (count(AntiKt10LCTopoJets.pt > 200*GeV) > 0) || (count(AntiKt4LCTopoJets.pt > 200*GeV) > 0) )"
#selection_j = "( (count(CamKt12LCTopoJets.pt > 200*GeV) > 0) || (count(AntiKt10LCTopoJets.pt > 200*GeV) > 0) || (count(AntiKt4LCTopoJets.pt > 200*GeV) > 0) )"

beamEnergy = jobproperties.Beam.energy()
expression_lep = ''
if (beamEnergy < 4.1e+06):
    expression_lep = "(" + selection_e + "||" + selection_mu + ")"
    expression_jet = selection_j
if (beamEnergy > 6.0e+06):
    expression_lep = "(" + selection_e + "||" + selection_mu + ")"
    expression_jet = selection_j

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT11SkimmingTool_lep = DerivationFramework__xAODStringSkimmingTool( name = "EXOT11SkimmingTool_lep", 
                                                                  expression = expression_lep)
ToolSvc += EXOT11SkimmingTool_lep
print EXOT11SkimmingTool_lep

EXOT11SkimmingTool_jet = DerivationFramework__xAODStringSkimmingTool( name = "EXOT11SkimmingTool_jet", 
                                                                  expression = expression_jet)
ToolSvc += EXOT11SkimmingTool_jet
print EXOT11SkimmingTool_jet

#=======================================
# THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot11Seq
exot11Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT11Kernel_lep",
                                                          SkimmingTools = [EXOT11SkimmingTool_lep])


#=======================================
# Augmentation: Jet Collections
#=======================================

from DerivationFrameworkExotics.JetDefinitions import *


#addDefaultTrimmedJets(exot11Seq, "EXOT11")
if isMC:
    addTrimmedJets('AntiKt', 1.0, 'Truth', rclus=0.2, ptfrac=0.05, algseq=exot11Seq, outputGroup="EXOT11", mods="exotJetMods")
addTrimmedJets('AntiKt', 1.0, 'LCTopo', rclus=0.2, ptfrac=0.05, algseq=exot11Seq, outputGroup="EXOT11", mods="exotJetMods")

#addStandardJets("CamKt", 0.8, "LCTopo", mods = "calib_notruth", algseq = exot11Seq, outputGroup = "EXOT11")
#addStandardJets("CamKt", 1.0, "LCTopo", mods = "calib_notruth", algseq = exot11Seq, outputGroup = "EXOT11")
addFilteredJets("CamKt", 1.2, "LCTopo", mumax=1.0, ymin=0.15, algseq = exot11Seq, outputGroup="EXOT11")

augTools = []
if globalflags.DataSource() == 'geant4':
  replaceBuggyAntiKt4TruthWZJets(exot11Seq)
  ToolSvc += CfgMgr.DerivationFramework__SUSYGenFilterTool(
    "EXOT11GenFilt",
    SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset
  )
  augTools.append(ToolSvc.EXOT11GenFilt)

exot11Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT11Kernel_jet",
                                                          SkimmingTools = [EXOT11SkimmingTool_jet])
exot11Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT11Kernel",
                                                          ThinningTools = thinningTools,
                                                          AugmentationTools = augTools)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName   = derivationFlags.WriteDAOD_EXOT11Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_EXOT11Stream )
EXOT11Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT11Stream.AcceptAlgs(["EXOT11Kernel"])

# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr   += createThinningSvc( svcName="EXOT11ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT11ContentList import *
EXOT11SlimmingHelper = SlimmingHelper("EXOT11SlimmingHelper")
EXOT11SlimmingHelper.SmartCollections = EXOT11SmartContent
EXOT11SlimmingHelper.ExtraVariables = EXOT11ExtraVariablesContent
EXOT11SlimmingHelper.AllVariables = EXOT11AllVariablesContent
EXOT11SlimmingHelper.StaticContent = EXOT11Content
if globalflags.DataSource()=='geant4':
    EXOT11SlimmingHelper.ExtraVariables += EXOT11ExtraVariablesContentTruth
    EXOT11SlimmingHelper.AllVariables += EXOT11AllVariablesContentTruth
    EXOT11SlimmingHelper.StaticContent += EXOT11ContentTruth

EXOT11SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT11SlimmingHelper.IncludeMuonTriggerContent = True

EXOT11SlimmingHelper.AppendContentToStream(EXOT11Stream)

