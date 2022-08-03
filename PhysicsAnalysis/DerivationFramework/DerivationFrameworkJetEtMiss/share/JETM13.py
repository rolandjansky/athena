#====================================================================
# JETM13.py
# reductionConf flag JETM13 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob, buildFileName
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets, addJetOutputs
from DerivationFrameworkJetEtMiss.METCommon import addMETOutputs
from DerivationFrameworkPhys import PhysCommon

if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth import MCTruthCommon
  MCTruthCommon.addBosonsAndDownstreamParticles(generations=4,rejectHadronChildren=True)
  MCTruthCommon.addTopQuarkAndDownstreamParticles(generations=4,rejectHadronChildren=True)

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM13Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM13Stream )
JETM13Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM13Stream.AcceptAlgs(["JETM13MainKernel"])

#====================================================================
# SKIMMING TOOL
#====================================================================
# None for the moment

#====================================================================
# THINNING TOOLS
#====================================================================
# Retain only stable truth particles, remove G4
# We want to keep all truth jet constituents
# Also keep the first 10 particles mainly for the HS truth vertex
jetm13thin = []
if DerivationFrameworkIsMonteCarlo:

  from DerivationFrameworkCore.ThinningHelper import ThinningHelper
  JETM13ThinningHelper = ThinningHelper( "JETM13ThinningHelper" )
  JETM13ThinningHelper.AppendToStream( JETM13Stream )
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  TruthThinningTool = DerivationFramework__MenuTruthThinning(name               = "JETM13TruthThinning",
                                                             StreamName         = streamName,
                                                             WriteAllStable     = True,
                                                             # Disable the flags that have been annoyingly
                                                             # defaulted to True
                                                             WritePartons       = False,
                                                             WriteHadrons       = False,
                                                             WriteBHadrons      = True,
                                                             WriteCHadrons      = False,
                                                             WriteGeant         = False,
                                                             WriteFirstN        = 10)
  ToolSvc += TruthThinningTool
  jetm13thin.append(TruthThinningTool)

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
# Here we run custom reconstruction

jetm13Seq = CfgMgr.AthSequencer("JETM13Sequence")
DerivationFrameworkJob += jetm13Seq

#=======================================
# UFO CHS
#=======================================
from JetRecConfig.JetRecConfig import getInputAlgs,reOrderAlgs
from JetRecConfig.StandardJetConstits import stdConstitDic as cst
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags

constit_algs = getInputAlgs(cst.UFO, configFlags=ConfigFlags)
constit_algs = reOrderAlgs( [a for a in constit_algs if a is not None])

for a in constit_algs:
  if not hasattr(DerivationFrameworkJob,a.getName()):
    DerivationFrameworkJob += conf2toConfigurable(a)

#=======================================
# Kernel
#=======================================

jetm13Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM13MainKernel",
                                                          SkimmingTools = [],
                                                          ThinningTools = jetm13thin)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM13SlimmingHelper = SlimmingHelper("JETM13SlimmingHelper")
JETM13SlimmingHelper.SmartCollections = ["EventInfo",
                                         "Electrons", "Photons", "Muons", "TauJets",
                                         "InDetTrackParticles", "PrimaryVertices",
                                         "MET_Baseline_AntiKt4EMTopo",
                                         "MET_Baseline_AntiKt4EMPFlow",
                                         "AntiKt4EMTopoJets","AntiKt4EMPFlowJets",
                                         "AntiKt4TruthJets","AntiKt10TruthJets",
                                        ]

JETM13SlimmingHelper.AllVariables = ["CaloCalTopoClusters",
                                     "GlobalChargedParticleFlowObjects", "GlobalNeutralParticleFlowObjects",
                                     "CHSGChargedParticleFlowObjects","CHSGNeutralParticleFlowObjects",
                                     "Kt4EMTopoOriginEventShape","Kt4EMPFlowEventShape","Kt4EMPFlowPUSBEventShape","Kt4EMPFlowNeutEventShape",
                                     "TruthParticles",
                                     "TruthVertices",
                                     "TruthEvents",
                                    ]

JETM13SlimmingHelper.ExtraVariables = [
  "InDetTrackParticles.particleHypothesis.vx.vy.vz",
  "GSFTrackParticles.particleHypothesis.vx.vy.vz",
  "PrimaryVertices.x.y.z",
  "TauJets.clusterLinks",
  "Muons.energyLossType.EnergyLoss.ParamEnergyLoss.MeasEnergyLoss.EnergyLossSigma.MeasEnergyLossSigma.ParamEnergyLossSigmaPlus.ParamEnergyLossSigmaMinus.clusterLinks.FSR_CandidateEnergy",
  "MuonSegments.x.y.z.px.py.pz"]

JETM13SlimmingHelper.AppendToDictionary = {'GlobalChargedParticleFlowObjects':'xAOD::FlowElementContainer','GlobalChargedParticleFlowObjectsAux':'xAOD::FlowElementAuxContainer',
                                           'GlobalNeutralParticleFlowObjects':'xAOD::FlowElementContainer', 'GlobalNeutralParticleFlowObjectsAux':'xAOD::FlowElementAuxContainer',
                                           'CHSGChargedParticleFlowObjects':'xAOD::FlowElementContainer','CHSGChargedParticleFlowObjectsAux':'xAOD::ShallowAuxContainer',
                                           'CHSGNeutralParticleFlowObjects':'xAOD::FlowElementContainer','CHSGNeutralParticleFlowObjectsAux':'xAOD::ShallowAuxContainer',
                                           'UFOCSSK':'xAOD::FlowElementContainer','UFOCSSKAux':'xAOD::FlowElementAuxContainer',
                                           'UFO':'xAOD::FlowElementContainer','UFOAux':'xAOD::FlowElementAuxContainer',
                                           'Kt4EMPFlowNeutEventShape':'xAOD::EventShape','Kt4EMPFlowNeutEventShapeAux':'xAOD::EventShapeAuxInfo'}

JETM13SlimmingHelper.ExtraVariables +=[ 'UFOCSSK.pt.eta.phi.m.signalType']
JETM13SlimmingHelper.ExtraVariables +=[ 'UFO.pt.eta.phi.m.signalType' ]

for truthc in [
  "TruthMuons",
  "TruthElectrons",
  "TruthPhotons",
  "TruthTaus",
  "TruthNeutrinos",
  ]:
  JETM13SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
  JETM13SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

for truthc in [
  "TruthTopQuark",
  "TruthBosons"
  ]:
  JETM13SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc+"WithDecayParticles")
  JETM13SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"WithDecayParticlesAux.")
  JETM13SlimmingHelper.StaticContent.append("xAOD::TruthVertexContainer#"+truthc+"WithDecayVertices")
  JETM13SlimmingHelper.StaticContent.append("xAOD::TruthVertexAuxContainer#"+truthc+"WithDecayVerticesAux.")
      

OutputJets["JETM13"] = ["AntiKt10TruthJets"]

# Add the jet containers to the stream
addJetOutputs(JETM13SlimmingHelper,["JETM13"])
# Add the MET containers to the stream
addMETOutputs(JETM13SlimmingHelper,["Track"])

JETM13SlimmingHelper.AppendContentToStream(JETM13Stream)
JETM13Stream.RemoveItem("xAOD::TrigNavigation#*")
JETM13Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")
