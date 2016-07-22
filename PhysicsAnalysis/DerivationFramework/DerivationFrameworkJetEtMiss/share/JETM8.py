#====================================================================
# JETM8.py 
# reductionConf flag JETM8 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
#from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
#
if globalflags.DataSource()=='geant4':
    from DerivationFrameworkMCTruth.MCTruthCommon import *
    from DerivationFrameworkTau.TauTruthCommon import *

#====================================================================
# SKIMMING TOOL 
#====================================================================
from DerivationFrameworkJetEtMiss.TriggerLists import *
triggers = jetTriggers

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
JETM8TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name           = "JETM8TrigSkimmingTool",
                                                                    TriggerListOR  = triggers )
ToolSvc += JETM8TrigSkimmingTool

# For first data
jetSelection = '(count( AntiKt10LCTopoJets.pt > 100.*GeV ) >=1)'
#jetSelection = '(count( CamKt12LCTopoJets.pt > 150.*GeV ) >=1)'
expression = jetSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM8OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM8OfflineSkimmingTool",
                                                                        expression = expression)
ToolSvc += JETM8OfflineSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
JETM8SkimmingTool = DerivationFramework__FilterCombinationAND(name="JETM8SkimmingTool", FilterList=[JETM8TrigSkimmingTool,JETM8OfflineSkimmingTool] )
ToolSvc += JETM8SkimmingTool

#====================================================================
# THINNING TOOLS 
#====================================================================
thinningTools = []

# thinning_expression = "InDetTrackParticles.pt > 0.5*GeV"
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
# JETM8TPThinningTool = DerivationFramework__TrackParticleThinning( name                    = "JETM8ThinningTool",
#                                                                   ThinningService         = "JETM8ThinningSvc",
#                                                                   SelectionString         = thinning_expression,
#                                                                   InDetTrackParticlesKey  = "InDetTrackParticles")

# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
# JETM8JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM8Akt4JetTPThinningTool",
#                                                                         ThinningService         = "JETM8ThinningSvc",
#                                                                         JetKey                  = "AntiKt4EMTopoJets",
#                                                                         InDetTrackParticlesKey  = "InDetTrackParticles",
#                                                                         ApplyAnd                = True)
# ToolSvc += JETM8JetTPThinningTool
# thinningTools.append(JETM8JetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
JETM8Akt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM8Akt4JetTPThinningTool",
                                                                        ThinningService         = "JETM8ThinningSvc",
                                                                        JetKey                  = "AntiKt4EMTopoJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd                = False)
ToolSvc += JETM8Akt4JetTPThinningTool
thinningTools.append(JETM8Akt4JetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
JETM8Akt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "JETM8Akt10JetTPThinningTool",
                                                                        ThinningService         = "JETM8ThinningSvc",
                                                                        JetKey                  = "AntiKt10LCTopoJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd                = False)
ToolSvc += JETM8Akt10JetTPThinningTool
thinningTools.append(JETM8Akt10JetTPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM8MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM8MuonTPThinningTool",
                                                                         ThinningService         = "JETM8ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM8MuonTPThinningTool
thinningTools.append(JETM8MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM8ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM8ElectronTPThinningTool",
                                                                               ThinningService         = "JETM8ThinningSvc",
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM8ElectronTPThinningTool
thinningTools.append(JETM8ElectronTPThinningTool)

# TrackParticles associated with photons
JETM8PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM8PhotonTPThinningTool",
                                                                             ThinningService         = "JETM8ThinningSvc",
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM8PhotonTPThinningTool
thinningTools.append(JETM8PhotonTPThinningTool)

# # TrackParticles associated with taus
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
# JETM8TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "JETM8TauTPThinningTool",
#                                                                         ThinningService = "JETM8ThinningSvc",
#                                                                         TauKey          = "TauJets",
#                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
# ToolSvc += JETM8TauTPThinningTool
# thinningTools.append(JETM8TauTPThinningTool)

# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and globalflags.DataSource()=='geant4':
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))"            # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.barcode < 200000))"            # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) <=  5  && (TruthParticles.pt > 10000.)) || (abs(TruthParticles.pdgId) == 6))"                 # Quarks
    truth_cond_Gluon  = "((abs(TruthParticles.pdgId) == 21) && (TruthParticles.pt > 10000.))"                                                # Gluons
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 10000.) && (TruthParticles.barcode < 200000))"                 # Photon
    
    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+' || '+truth_cond_Gluon+' || '+truth_cond_Photon+')'
    
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    JETM8TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "JETM8TruthThinningTool",
                                                                        ThinningService        = "JETM8ThinningSvc",
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)
    
    ToolSvc += JETM8TruthThinningTool
    thinningTools.append(JETM8TruthThinningTool)    

#=======================================
# AUGMENT ANTIKT4EMTOPO JETS
#=======================================

addTrackSumMoments("AntiKt4EMTopo")

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm8Seq = CfgMgr.AthSequencer("JETM8Sequence")
DerivationFrameworkJob += jetm8Seq
#jetm8Seq = DerivationFrameworkJob

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm8Seq += CfgMgr.DerivationFramework__DerivationKernel(	name = "JETM8Kernel", 
									SkimmingTools = [JETM8SkimmingTool],
									ThinningTools = thinningTools)

#====================================================================
# Special jets
#====================================================================

OutputJets["JETM8"] = []

# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(jetm8Seq,"JETM8")

if jetFlags.useTruth:
    addPrunedJets("CamKt", 1.0, "Truth", rcut=0.5, zcut=0.15, algseq=jetm8Seq, outputGroup="JETM8")
    addFilteredJets("CamKt", 1.2, "Truth", mumax=1.0, ymin=0.15, algseq=jetm8Seq, outputGroup="JETM8")
    addFilteredJets("CamKt", 1.2, "Truth", mumax=1.0, ymin=0.04, algseq=jetm8Seq, outputGroup="JETM8")
    addStandardJets("CamKt", 1.5, "Truth", ptmin=40000, algseq=jetm8Seq, outputGroup="JETM8")

# CamKtLCTopo 10 and 12
addPrunedJets("CamKt", 1.0, "LCTopo", rcut=0.5, zcut=0.15, algseq=jetm8Seq, outputGroup="JETM8")
addFilteredJets("CamKt", 1.2, "LCTopo", mumax=1.0, ymin=0.15, algseq=jetm8Seq, outputGroup="JETM8")
addFilteredJets("CamKt", 1.2, "LCTopo", mumax=1.0, ymin=0.04, algseq=jetm8Seq, outputGroup="JETM8")
# CamKt15LCTopo
addStandardJets("CamKt", 1.5, "LCTopo", mods="calib", calibOpt="none", ghostArea=0.01, ptmin=2000, ptminFilter=50000,
                algseq=jetm8Seq, outputGroup="JETM8")
# AntiKt10Track
#addStandardJets("AntiKt", 1.0, "PV0Track", ptmin=40000, algseq=jetm8Seq, outputGroup="JETM8")
addTrimmedJets("AntiKt", 1.0, "PV0Track", rclus=0.2, ptfrac=0.05, algseq=jetm8Seq, outputGroup="JETM8")

# PFlow fat jets
addTrimmedJets("AntiKt", 1.0, "EMCPFlow", rclus=0.2, ptfrac=0.05, algseq=jetm8Seq, outputGroup="JETM8")
addTrimmedJets("AntiKt", 1.0, "EMPFlow", rclus=0.2, ptfrac=0.05, algseq=jetm8Seq, outputGroup="JETM8")

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM8Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM8Stream )
JETM8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM8Stream.AcceptAlgs(["JETM8Kernel"])
# for thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="JETM8ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM8SlimmingHelper = SlimmingHelper("JETM8SlimmingHelper")
JETM8SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4LCTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4EMPFlowJets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
JETM8SlimmingHelper.AllVariables = ["BTagging_AntiKt4LCTopo", "BTagging_AntiKt4EMTopo",
                                    "BTagging_AntiKt2Track", "BTagging_AntiKt3Track",
                                    "CaloCalTopoClusters",
                                    "MuonTruthParticles", "egammaTruthParticles",
                                    "TruthParticles", "TruthEvents", "TruthVertices",
                                    "JetETMissChargedParticleFlowObjects", "JetETMissNeutralParticleFlowObjects"]
#JETM8SlimmingHelper.ExtraVariables = []
for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthTaus",
#    "TruthNeutrinos"
    ]:
    JETM8SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM8SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

# Trigger content
from DerivationFrameworkCore.JetTriggerContent import JetTriggerContent
for trig in JetTriggerContent:
    if 'HLT' in trig and not 'Aux' in trig:
        JETM8SlimmingHelper.AllVariables.append(trig)
JETM8SlimmingHelper.IncludeJetTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(JETM8SlimmingHelper,["SmallR",
                                   "LargeR",
                                   "JETM8","AntiKt10EMCPFlowJets","AntiKt10EMPFlowJets"])
# Add the MET containers to the stream
addMETOutputs(JETM8SlimmingHelper,["Diagnostic","AntiKt4LCTopo","AntiKt4EMPFlow","Track"])

JETM8SlimmingHelper.AppendContentToStream(JETM8Stream)
JETM8Stream.RemoveItem("xAOD::TrigNavigation#*")
JETM8Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")
