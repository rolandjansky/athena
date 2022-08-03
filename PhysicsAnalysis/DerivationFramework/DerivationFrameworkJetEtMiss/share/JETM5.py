#====================================================================
# JETM5.py 
# reductionConf flag JETM5 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob, buildFileName
from DerivationFrameworkJetEtMiss.JetCommon import addJetOutputs
from DerivationFrameworkJetEtMiss.METCommon import addMETOutputs
from DerivationFrameworkPhys import PhysCommon

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM5Stream )
JETM5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM5Stream.AcceptAlgs(["JETM5Kernel"])
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

#====================================================================
# ADD PFLOW AUG INFORMATION 
#====================================================================
from DerivationFrameworkJetEtMiss.PFlowCommon import applyPFOAugmentation
applyPFOAugmentation(DerivationFrameworkJob)

#====================================================================
# SKIMMING TOOL 
#====================================================================

skimmingTools = []

if not DerivationFrameworkIsMonteCarlo:
  expression = '( HLT_noalg_zb_L1ZB )'
  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
  JETM5SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM5SkimmingTool1",
                                                                   expression = expression)
  ToolSvc += JETM5SkimmingTool
  skimmingTools.append(JETM5SkimmingTool)

#====================================================================
# THINNING TOOLS 
#====================================================================
thinningTools = []

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM5MuonTPThinningTool",
                                                                    StreamName              = streamName,
                                                                    MuonKey                 = "Muons",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM5MuonTPThinningTool
thinningTools.append(JETM5MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM5ElectronTPThinningTool",
                                                                               StreamName              = streamName,
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM5ElectronTPThinningTool
thinningTools.append(JETM5ElectronTPThinningTool)

# TrackParticles associated with photons
JETM5PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM5PhotonTPThinningTool",
                                                                             StreamName              = streamName,
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM5PhotonTPThinningTool
thinningTools.append(JETM5PhotonTPThinningTool)

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
                                                                        StreamName              = streamName,
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)
    
    ToolSvc += JETM5TruthThinningTool
    thinningTools.append(JETM5TruthThinningTool)    

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm5Seq = CfgMgr.AthSequencer("JETM5Sequence")
DerivationFrameworkJob += jetm5Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm5Seq += CfgMgr.DerivationFramework__DerivationKernel(	name = "JETM5Kernel",
                                                                SkimmingTools = skimmingTools,
                                                                ThinningTools = thinningTools)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM5SlimmingHelper = SlimmingHelper("JETM5SlimmingHelper")
JETM5SlimmingHelper.SmartCollections = ["EventInfo",
                                        "Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices",
                                        "MET_Baseline_AntiKt4EMTopo",
                                        "MET_Baseline_AntiKt4EMPFlow",
                                        "AntiKt4EMTopoJets","AntiKt4EMPFlowJets",
                                        "BTagging_AntiKt4EMPFlow"]

JETM5SlimmingHelper.AllVariables = ["CaloCalTopoClusters",
                                    "MuonTruthParticles", "egammaTruthParticles",
                                    "TruthParticles", "TruthEvents", "TruthVertices",
                                    "MuonSegments",
                                    "Kt4EMTopoOriginEventShape","Kt4EMPFlowEventShape",
                                    "GlobalNeutralParticleFlowObjects", "GlobalChargedParticleFlowObjects"
                                    ]

# Add QG tagger variables
JETM5SlimmingHelper.ExtraVariables  += ["AntiKt4EMTopoJets.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1",
                                        "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1"]

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
addMETOutputs(JETM5SlimmingHelper,["Diagnostic","AntiKt4EMTopo","AntiKt4EMPFlow","Track"])

JETM5SlimmingHelper.AppendContentToStream(JETM5Stream)
