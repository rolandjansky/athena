#====================================================================
# JETM5.py 
# reductionConf flag JETM5 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
#from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
#
from DerivationFrameworkJetEtMiss.METCommon import *
#
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

#====================================================================
# SKIMMING TOOL 
#====================================================================
expression = '( (EventInfo.eventTypeBitmask==1) || HLT_noalg_zb_L1ZB )'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM5SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM5SkimmingTool1",
                                                                    expression = expression)
ToolSvc += JETM5SkimmingTool

#====================================================================
# THINNING TOOLS 
#====================================================================
thinningTools = []

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM5MuonTPThinningTool",
                                                                    ThinningService         = "JETM5ThinningSvc",
                                                                    MuonKey                 = "Muons",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM5MuonTPThinningTool
thinningTools.append(JETM5MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM5ElectronTPThinningTool",
                                                                               ThinningService         = "JETM5ThinningSvc",
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM5ElectronTPThinningTool
thinningTools.append(JETM5ElectronTPThinningTool)

# TrackParticles associated with photons
JETM5PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM5PhotonTPThinningTool",
                                                                             ThinningService         = "JETM5ThinningSvc",
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM5PhotonTPThinningTool
thinningTools.append(JETM5PhotonTPThinningTool)

# # TrackParticles associated with taus
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
# JETM5TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "JETM5TauTPThinningTool",
#                                                                         ThinningService = "JETM5ThinningSvc",
#                                                                         TauKey          = "TauJets",
#                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
# ToolSvc += JETM5TauTPThinningTool
# thinningTools.append(JETM5TauTPThinningTool)

# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and DerivationFrameworkIsMonteCarlo:
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))"            # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.barcode < 200000))"            # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) <=  5 && (TruthParticles.pt > 10000.)) || (abs(TruthParticles.pdgId) == 6))"                 # Quarks
    truth_cond_Gluon  = "((abs(TruthParticles.pdgId) == 21) && (TruthParticles.pt > 10000.))"                                                # Gluons
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 10000.))"                 # Photon
    
    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+'||'+truth_cond_Gluon+' || '+truth_cond_Photon+')'
    
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    JETM5TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "JETM5TruthThinningTool",
                                                                        ThinningService        = "JETM5ThinningSvc",
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)
    
    ToolSvc += JETM5TruthThinningTool
    thinningTools.append(JETM5TruthThinningTool)    

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(	name = "JETM5Kernel",
									SkimmingTools = [JETM5SkimmingTool],
									ThinningTools = thinningTools)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM5Stream )
JETM5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM5Stream.AcceptAlgs(["JETM5Kernel"])
# for thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="JETM5ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM5SlimmingHelper = SlimmingHelper("JETM5SlimmingHelper")
JETM5SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4LCTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4EMPFlowJets",
                                        "BTagging_AntiKt4EMTopo", ]
JETM5SlimmingHelper.AllVariables = ["CaloCalTopoClusters",
                                    "MuonTruthParticles", "egammaTruthParticles",
                                    "TruthParticles", "TruthEvents", "TruthVertices",
                                    "MuonSegments",
                                    "Kt4EMTopoOriginEventShape","Kt4LCTopoOriginEventShape","Kt4EMPFlowEventShape",
                                    ]
#JETM5SlimmingHelper.ExtraVariables = []
for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthTaus",
#    "TruthNeutrinos"
    ]:
    JETM5SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM5SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

# No trigger objects for now

# Add the jet containers to the stream
addJetOutputs(JETM5SlimmingHelper,["SmallR"])
# Add the MET containers to the stream
addMETOutputs(JETM5SlimmingHelper,["Diagnostic","AntiKt4LCTopo","AntiKt4EMPFlow","Track"])

JETM5SlimmingHelper.AppendContentToStream(JETM5Stream)
