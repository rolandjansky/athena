#********************************************************************
# EXOT12.py 
# reductionConf flag EXOT12 in Reco_tf.py   
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

exot12Seq = CfgMgr.AthSequencer("EXOT12Sequence")
#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools = []

#Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT12ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                   = "EXOT12ElectronTPThinningTool",
                                                                                 ThinningService        = "EXOT12ThinningSvc",
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT12ElectronTPThinningTool
thinningTools.append(EXOT12ElectronTPThinningTool)

#Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT12MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                  = "EXOT12MuonTPThinningTool",
                                                                           ThinningService       = "EXOT12ThinningSvc",
                                                                           MuonKey               = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT12MuonTPThinningTool
thinningTools.append(EXOT12MuonTPThinningTool)

#Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT12AKt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EXOT12AKt4JetTPThinningTool",
                                                                             ThinningService         = "EXOT12ThinningSvc",
                                                                             JetKey                  = "AntiKt4LCTopoJets",
                                                                             SelectionString         = "AntiKt4LCTopoJets.pt > 15*GeV && abs(AntiKt4LCTopoJets.eta) < 2.8",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT12AKt4JetTPThinningTool
thinningTools.append(EXOT12AKt4JetTPThinningTool)

EXOT12AKt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EXOT12AKt10JetTPThinningTool",
                                                                              ThinningService         = "EXOT12ThinningSvc",
                                                                              JetKey                  = "AntiKt10LCTopoJets",
                                                                              SelectionString         = "AntiKt10LCTopoJets.pt > 200*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT12AKt10JetTPThinningTool
thinningTools.append(EXOT12AKt10JetTPThinningTool)

#Truth Thinning (!!! NOT WORKING FOR athena 19)
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT12MenuHadThinningTool = DerivationFramework__MenuTruthThinning( name = "EXOT12MenuHadThinningTool",
                                                                 ThinningService = "EXOT12ThinningSvc",
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

EXOT12MenuThinningTool = DerivationFramework__MenuTruthThinning( name = "EXOT12MenuThinningTool",
                                                                 ThinningService = "EXOT12ThinningSvc",
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

EXOT12MenuStatus3ThinningTool = DerivationFramework__MenuTruthThinning( name = "EXOT12MenuStatus3ThinningTool",
                                                                 ThinningService = "EXOT12ThinningSvc",
                                                                 WritePartons               = False,#?
                                                                 WriteHadrons               = False,
                                                                 WriteBHadrons              = False,
                                                                 WriteGeant                 = False,
                                                                 GeantPhotonPtThresh        = -1.0,
                                                                 WriteTauHad                = False,
                                                                 PartonPtThresh             = -1.0,
                                                                 WriteBSM                   = False,
                                                                 WriteBosons                = False,
                                                                 WriteBSMProducts           = False,
                                                                 WriteTopAndDecays          = False,
                                                                 WriteEverything            = False,
                                                                 WriteAllLeptons            = False,
                                                                 WriteLeptonsNotFromHadrons = False,
                                                                 WriteStatus3               = True,
                                                                 WriteFirstN                = -1,
                                                                 PreserveDescendants        = False)

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT12GenThinningTool = DerivationFramework__GenericTruthThinning( name = "EXOT12GenThinningTool",
                                                                   ThinningService = "EXOT12ThinningSvc",
                                                                   ParticleSelectionString = "abs(TruthParticles.pdgId) ==34 || abs(TruthParticles.pdgId)==5100039", 
                                                                   PreserveDescendants = False)

if isMC:
  ToolSvc += EXOT12MenuHadThinningTool
  thinningTools.append(EXOT12MenuHadThinningTool)
  ToolSvc += EXOT12MenuThinningTool
  thinningTools.append(EXOT12MenuThinningTool)
  ToolSvc += EXOT12GenThinningTool
  thinningTools.append(EXOT12GenThinningTool)
  ToolSvc += EXOT12MenuStatus3ThinningTool
  thinningTools.append(EXOT12MenuStatus3ThinningTool)

#=======================================
# SKIMMING TOOL   
#=======================================

#trig = " (HLT_e28_tight_iloose || HLT_e60_medium1)|| (HLT_mu26_imedium || HLT_mu50) "
selection_e = "((count((Electrons.Medium) && (Electrons.pt > 15*GeV)) > 0) && (count((Electrons.Loose) && (Electrons.pt > 15*GeV)) > 1))"
selection_mu = "(count(Muons.pt > 15*GeV) > 1)"
selection_e_mu = "((count(Muons.pt > 15*GeV) > 0) && (count((Electrons.Loose) && (Electrons.pt > 15*GeV)) > 0))"
#selection_j = "( (count(CamKt12LCTopoJets.pt > 200*GeV) > 0) || (count(AntiKt10LCTopoJets.pt > 200*GeV) > 0))" #|| (count(AntiKt4LCTopoJets.pt > 200*GeV) > 0) )"
#selection_j = "( (count(CamKt12LCTopoJets.pt > 200*GeV) > 0) || (count(AntiKt10LCTopoJets.pt > 0*GeV) > 0))" #|| (count(AntiKt4LCTopoJets.pt > 200*GeV) > 0) )"


beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression_lep = "(" + selection_e + "||" + selection_mu + "||" + selection_e_mu + ")"
    expression_jet = "" # selection_j
if (beamEnergy > 6.0e+06):
    expression_lep = "(" + selection_e + "||" + selection_mu + "||" + selection_e_mu + ")"
    expression_jet = "" # selection_j

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT12SkimmingTool_lep = DerivationFramework__xAODStringSkimmingTool( name = "EXOT12SkimmingTool_lep", 
                                                                      expression = expression_lep)
ToolSvc += EXOT12SkimmingTool_lep
print EXOT12SkimmingTool_lep

#EXOT12SkimmingTool_jet = DerivationFramework__xAODStringSkimmingTool( name = "EXOT12SkimmingTool_jet", 
#                                                                      expression = expression_jet)
#ToolSvc += EXOT12SkimmingTool_jet
#print EXOT12SkimmingTool_jet

#=======================================
# THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot12Seq
exot12Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT12Kernel_lep",
                                                          SkimmingTools = [EXOT12SkimmingTool_lep])

#=======================================
# Augmentation: Jet Collections
#=======================================

from DerivationFrameworkExotics.JetDefinitions import *

#addDefaultTrimmedJets(exot12Seq, "EXOT12")
if isMC:
    addTrimmedJets('AntiKt', 1.0, 'Truth', rclus=0.2, ptfrac=0.05, algseq=exot12Seq, outputGroup="EXOT12", mods="exotJetMods")
addTrimmedJets('AntiKt', 1.0, 'LCTopo', rclus=0.2, ptfrac=0.05, algseq=exot12Seq, outputGroup="EXOT12", mods="exotJetMods")

#addStandardJets("CamKt", 0.8, "LCTopo", mods = "calib_notruth", algseq = exot12Seq, outputGroup = "EXOT12")
#addStandardJets("CamKt", 1.0, "LCTopo", mods = "calib_notruth", algseq = exot12Seq, outputGroup = "EXOT12")

addFilteredJets("CamKt", 1.2, "LCTopo", mumax=1.0, ymin=0.15, algseq=exot12Seq, outputGroup="EXOT12")

if globalflags.DataSource() == 'geant4':
  replaceBuggyAntiKt4TruthWZJets(exot12Seq)


#exot12Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT12Kernel_jet",
#                                                          SkimmingTools = [EXOT12SkimmingTool_jet])

exot12Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT12Kernel",
                                                          ThinningTools = thinningTools)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName   = derivationFlags.WriteDAOD_EXOT12Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_EXOT12Stream )
EXOT12Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT12Stream.AcceptAlgs(["EXOT12Kernel"])

# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr   += createThinningSvc( svcName="EXOT12ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT12ContentList import *
EXOT12SlimmingHelper = SlimmingHelper("EXOT12SlimmingHelper")
EXOT12SlimmingHelper.SmartCollections = EXOT12SmartContent
EXOT12SlimmingHelper.ExtraVariables = EXOT12ExtraVariablesContent
EXOT12SlimmingHelper.AllVariables = EXOT12AllVariablesContent
EXOT12SlimmingHelper.StaticContent = EXOT12Content
if globalflags.DataSource()=='geant4':
    EXOT12SlimmingHelper.ExtraVariables += EXOT12ExtraVariablesContentTruth
    EXOT12SlimmingHelper.AllVariables += EXOT12AllVariablesContentTruth
    EXOT12SlimmingHelper.StaticContent += EXOT12ContentTruth

EXOT12SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT12SlimmingHelper.IncludeMuonTriggerContent = True

EXOT12SlimmingHelper.AppendContentToStream(EXOT12Stream)

