#********************************************************************
# HIGG3D3.py 
# reductionConf flag HIGG3D3 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from AthenaCommon.GlobalFlags import globalflags

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG3D3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG3D3Stream )
HIGG3D3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG3D3Stream.AcceptAlgs(["HIGG3D3Kernel"])

## Prepare thinning service and add trigger chains for TrigNavigation thinning
TriggerChains = ["HLT_mu24_L1MU15",
                 "HLT_g10_etcut",
                 "HLT_g10_loose",
                 "HLT_g15_loose_L1EM7",
                 "HLT_g20_loose_L1EM12",
                 "HLT_g20_loose_L1EM15",
                 "HLT_g20_etcut_L1EM12",
                 "HLT_e17_lhloose_L1EM15",
                 "HLT_e17_loose_L1EM15",
                 "HLT_g25_loose_L1EM15",
                 "HLT_g35_loose_L1EM15",
                 "HLT_g40_loose_L1EM15",
                 "HLT_g45_loose_L1EM15",
                 "HLT_g50_loose_L1EM15",
                 "HLT_e60_medium",
                 "HLT_e60_lhmedium",
                 "HLT_g35_loose_L1EM15_g25_loose_L1EM15",
                 "HLT_e24_lhmedium_L1EM18VH",
                 "HLT_e24_lhmedium_L1EM20VH",
                 "HLT_e20_lhmedium_g35_loose",
                 "HLT_2e12_loose_L12EM10VH",
                 "HLT_2e12_lhloose_L12EM10VH",
                 "HLT_2e15_vloose_L12EM13VH",
                 "HLT_2g20_loose_g15_loose",
                 "HLT_mu4",
                 "HLT_mu6",
                 "HLT_mu10",
                 "HLT_mu14",
                 "HLT_mu18",
                 "HLT_mu20_L1MU15",
                 "HLT_mu24",
                 "HLT_mu40",
                 "HLT_mu20_iloose_L1MU15",
                 "HLT_e5_etcut",
                 "HLT_e10_etcut_L1EM7",
                 "HLT_e13_etcut_trkcut_L1EM10",
                 "HLT_e15_etcut_L1EM7",
                 "HLT_e18_etcut_trkcut_L1EM15",
                 "HLT_e20_etcut_L1EM12",
                 "HLT_e25_etcut_L1EM15",
                 "HLT_e30_etcut_L1EM15",
                 "HLT_e40_etcut_L1EM15",
                 "HLT_e50_etcut_L1EM15",
                 "HLT_e60_etcut",
                 "HLT_e5_lhvloose",
                 "HLT_e5_lhvloose_nod0",
                 "HLT_e10_lhvloose_L1EM7",
                 "HLT_e12_lhvloose_L1EM10VH",
                 "HLT_e12_lhvloose_nod0_L1EM10VH",
                 "HLT_e15_lhvloose_L1EM7",
                 "HLT_e15_lhvloose_L1EM13VH",
                 "HLT_e15_lhvloose_nod0_L1EM13VH",
                 "HLT_e17_lhvloose",
                 "HLT_e17_lhvloose_nod0",
                 "HLT_e20_lhvloose_L1EM12",
                 "HLT_e20_lhvloose",
                 "HLT_e20_lhvloose_nod0",
                 "HLT_e24_lhvloose_nod0_L1EM18VH",
                 "HLT_e24_lhvloose_nod0_L1EM20VH"]

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
HIGG3D3TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool( name                = "HIGG3D3TrigSkimmingTool",
                                                                    TriggerListOR       = TriggerChains )
ToolSvc += HIGG3D3TrigSkimmingTool

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG3D3ThinningHelper = ThinningHelper( "HIGG3D3ThinningHelper" )
HIGG3D3ThinningHelper.TriggerChains = " | ".join(TriggerChains)
HIGG3D3ThinningHelper.AppendToStream( HIGG3D3Stream )

#================
# THINNING
#================
thinningTools=[]

# MET/Jet tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG3D3JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG3D3JetTPThinningTool",
                                                                ThinningService         = HIGG3D3ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += HIGG3D3JetTPThinningTool
thinningTools.append(HIGG3D3JetTPThinningTool)


# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG3D3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG3D3MuonTPThinningTool",
                                                                           ThinningService         = HIGG3D3ThinningHelper.ThinningSvc(),
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG3D3MuonTPThinningTool
thinningTools.append(HIGG3D3MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG3D3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "HIGG3D3ElectronTPThinningTool",
                                                                                 ThinningService         = HIGG3D3ThinningHelper.ThinningSvc(),
                                                                                 SGKey                   = "Electrons",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG3D3ElectronTPThinningTool
thinningTools.append(HIGG3D3ElectronTPThinningTool)

# Truth particles
truth_cond1 = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond2 = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond3 = "(abs(TruthParticles.pdgId) == 5 || abs(TruthParticles.pdgId) == 6)"  # b and Top quark
truth_cond4 = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 5*GeV))" # Photon

truth_expression = truth_cond1+' || '+truth_cond2 +' || '+truth_cond3 +' || '+truth_cond4

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG3D3TruthThinningTool = DerivationFramework__GenericTruthThinning(name                    = "HIGG3D3TruthThinningTool", 
                                                                     ThinningService         = HIGG3D3ThinningHelper.ThinningSvc(),
                                                                     ParticleSelectionString = truth_expression,
                                                                     PreserveDescendants     = True,
                                                                     PreserveAncestors      = True,
                                                                     SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset )

if globalflags.DataSource()=='geant4':
    ToolSvc += HIGG3D3TruthThinningTool                                                                      
    thinningTools.append(HIGG3D3TruthThinningTool)

#====================================================================
# SKIMMING TOOL
#====================================================================
electronRequirements = '(Electrons.pt > 7*GeV) && (abs(Electrons.eta) < 2.6)'
muonsRequirements = '(Muons.pt > 7*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonGoodMuon)'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__DeltaRTool
HIGG3D3ElJDeltaRTool = DerivationFramework__DeltaRTool( name = "HIGG3D3ElJDeltaRTool",
                                                        StoreGateEntryName = "dRElecJet",
                                                        ObjectRequirements = electronRequirements,
                                                        SecondObjectRequirements = "(AntiKt4EMTopoJets.pt > 15*GeV)",
                                                        ContainerName = "Electrons",
                                                        SecondContainerName = "AntiKt4EMTopoJets" )
ToolSvc += HIGG3D3ElJDeltaRTool
HIGG3D3MuJDeltaRTool = DerivationFramework__DeltaRTool( name = "HIGG3D3MuJDeltaRTool",
                                                        StoreGateEntryName = "dRMuJet",
                                                        ObjectRequirements = muonsRequirements,
                                                        SecondObjectRequirements = "(AntiKt4EMTopoJets.pt > 15*GeV)",
                                                        ContainerName = "Muons",
                                                        SecondContainerName = "AntiKt4EMTopoJets" )
ToolSvc += HIGG3D3MuJDeltaRTool

eSelection = '(count('+electronRequirements+')>=1)'
mSelection = '(count('+muonsRequirements+')>=1)'
expression = '(' + eSelection+' || '+mSelection+')' + '&& ( (count(dRMuJet > 2.5) >= 1) || (count(dRElecJet > 2.5) >= 1))'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG3D3SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "HIGG3D3SkimmingTool1",
                                                                   expression = expression)
ToolSvc += HIGG3D3SkimmingTool

skimmingTools = []

# Keep only ~5% of MC to reduce the total derivation size, adding this to the skimming tool list FIRST
if globalflags.DataSource()=='geant4':
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__PrescaleTool
    mcPrescaleTool = DerivationFramework__PrescaleTool(name = "HIGG3D3MCPrescaleTool",
                                                       Prescale = 19)
    ToolSvc += mcPrescaleTool
    skimmingTools.append(mcPrescaleTool)

skimmingTools += [HIGG3D3TrigSkimmingTool,HIGG3D3SkimmingTool]

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("HIGG3D3Kernel",
                                                                       AugmentationTools = [HIGG3D3ElJDeltaRTool,HIGG3D3MuJDeltaRTool],
                                                                       SkimmingTools = skimmingTools,
                                                                       ThinningTools = thinningTools )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG3D3SlimmingHelper = SlimmingHelper("HIGG3D3SlimmingHelper")
from DerivationFrameworkHiggs.HIGG3D3ExtraContent import *
#HIGG3D3SlimmingHelper.UserContent = HIGG3D3Content
HIGG3D3SlimmingHelper.SmartCollections = [ "Electrons",
                                           "Muons",
                                           "AntiKt4EMTopoJets",
                                           "InDetTrackParticles",
                                           "BTagging_AntiKt4EMTopo",
                                           "PrimaryVertices",
                                           "MET_Reference_AntiKt4EMTopo" ]

HIGG3D3SlimmingHelper.ExtraVariables = HIGG3D3ExtraVariables
HIGG3D3SlimmingHelper.AllVariables = HIGG3D3ExtraContainers

if globalflags.DataSource()=='geant4':
    HIGG3D3SlimmingHelper.AllVariables += HIGG3D3ExtraTruthContainers
    HIGG3D3SlimmingHelper.ExtraVariables += HIGG3D3ExtraTruthVariables

# Add Trigger content
HIGG3D3SlimmingHelper.IncludeMuonTriggerContent = True
HIGG3D3SlimmingHelper.IncludeEGammaTriggerContent = True

# Add MET to output stream
#addMETOutputs(HIGG3D3SlimmingHelper)

HIGG3D3SlimmingHelper.AppendContentToStream(HIGG3D3Stream)
