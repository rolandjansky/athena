#********************************************************************
# HIGG3D1.py 
# reductionConf flag HIGG3D1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

from DerivationFrameworkCore.WeightMetadata import *

from AthenaCommon.GlobalFlags import globalflags

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG3D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG3D1Stream )
HIGG3D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG3D1Stream.AcceptAlgs(["HIGG3D1Kernel"])

## Prepare thinning service and add trigger chains for TrigNavigation thinning
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG3D1ThinningHelper = ThinningHelper( "HIGG3D1ThinningHelper" )
HIGG3D1ThinningHelper.TriggerChains = "HLT_mu.* | HLT_e.* | HLT_2e.* | HLT_2mu.*"
HIGG3D1ThinningHelper.AppendToStream( HIGG3D1Stream )

#================
# THINNING
#================
thinningTools=[]

# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
HIGG3D1MetTPThinningTool = DerivationFramework__TrackParticleThinning(name                   = "HIGG3D1MetTPThinningTool",
                                                                      ThinningService        = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                      SelectionString        = thinning_expression,
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      ApplyAnd               = True)
ToolSvc += HIGG3D1MetTPThinningTool
thinningTools.append(HIGG3D1MetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG3D1JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG3D1JetTPThinningTool",
                                                                ThinningService         = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += HIGG3D1JetTPThinningTool
thinningTools.append(HIGG3D1JetTPThinningTool)


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG3D1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG3D1MuonTPThinningTool",
                                                                           ThinningService         = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG3D1MuonTPThinningTool
thinningTools.append(HIGG3D1MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG3D1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "HIGG3D1ElectronTPThinningTool",
                                                                                 ThinningService         = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                                 SGKey                   = "Electrons",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG3D1ElectronTPThinningTool
thinningTools.append(HIGG3D1ElectronTPThinningTool)

# Tracks themselves
HIGG3D1TPThinningTool = DerivationFramework__TrackParticleThinning(name                    = "HIGG3D1TPThinningTool",
                                                                   ThinningService         = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                   SelectionString         = "abs( DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta)) < 3.0",
                                                                   InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG3D1TPThinningTool
thinningTools.append(HIGG3D1TPThinningTool)

# Truth particles
truth_cond1 = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond3 = "(abs(TruthParticles.pdgId) == 6)"  # Top quark
truth_cond4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 5*GeV))" # Photon

truth_expression = truth_cond1+' || '+truth_cond2 +' || '+truth_cond3 +' || '+truth_cond4

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG3D1TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HIGG3D1TruthThinningTool", 
                                                                     ThinningService         = HIGG3D1ThinningHelper.ThinningSvc(),
                                                                     ParticleSelectionString = truth_expression,
                                                                     PreserveDescendants     = True,
                                                                     PreserveAncestors      = True,
                                                                     SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset)

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG3D1TruthThinningTool                                                                      
    thinningTools.append(HIGG3D1TruthThinningTool)

#====================================================================
# SKIMMING TOOL 
#====================================================================
electronIDRequirements = '(Electrons.DFCommonElectronsLHLoose)'
electronRequirements = '(Electrons.pt > 7*GeV) && (abs(Electrons.eta) < 2.6)'
leadElectron = electronRequirements + ' && (Electrons.pt > 17*GeV)'
muonsRequirements = '(Muons.pt > 7*GeV) && (Muons.DFCommonGoodMuon)'
leadMuon = muonsRequirements + ' && (Muons.pt > 17*GeV)'

eeSelection = '((count('+electronRequirements+') >= 2) && (count('+leadElectron+') >= 1) && (count('+electronRequirements+' && '+electronIDRequirements+') >= 1))'
mmSelection = '((count('+muonsRequirements+') >= 2) && (count('+leadMuon+') >= 1))'
emSelection = '(((count('+electronRequirements+') >= 1) && (count('+muonsRequirements+') >= 1)) && ((count('+leadElectron+') >= 1) || (count('+leadMuon+') >= 1)))'
expression = eeSelection+' || '+mmSelection+' || '+emSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG3D1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG3D1SkimmingTool1",
                                                                    expression = expression)
ToolSvc += HIGG3D1SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG3D1Kernel",
									SkimmingTools = [HIGG3D1SkimmingTool],
									ThinningTools = thinningTools
                                                                      )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG3D1SlimmingHelper = SlimmingHelper("HIGG3D1SlimmingHelper")
from DerivationFrameworkHiggs.HIGG3D1ExtraContent import *

HIGG3D1SlimmingHelper.SmartCollections = [ "Electrons",
                                           "Muons",
                                           "Photons",
                                           "MET_Reference_AntiKt4LCTopo",
                                           "MET_Reference_AntiKt4EMTopo",
                                           "AntiKt4EMTopoJets",
                                           "AntiKt4LCTopoJets",
                                           "BTagging_AntiKt4LCTopo",
                                           "BTagging_AntiKt4EMTopo",
                                           "BTagging_AntiKt4Track",
                                           "BTagging_AntiKt2Track",
                                           "InDetTrackParticles",
                                           "PrimaryVertices" ]

HIGG3D1SlimmingHelper.ExtraVariables = HIGG3D1ExtraVariables
HIGG3D1SlimmingHelper.AllVariables = HIGG3D1ExtraContainers

if globalflags.DataSource()=='geant4':
    HIGG3D1SlimmingHelper.AllVariables += HIGG3D1ExtraTruthContainers
    HIGG3D1SlimmingHelper.ExtraVariables += HIGG3D1ExtraTruthVariables

# Add Trigger content
HIGG3D1SlimmingHelper.IncludeMuonTriggerContent = True
HIGG3D1SlimmingHelper.IncludeEGammaTriggerContent = True

# Add MET to output stream
#addMETOutputs(HIGG3D1SlimmingHelper)

HIGG3D1SlimmingHelper.AppendContentToStream(HIGG3D1Stream)
