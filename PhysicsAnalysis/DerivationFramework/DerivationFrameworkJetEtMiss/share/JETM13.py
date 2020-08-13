#====================================================================
# JETM13.py
# reductionConf flag JETM13 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

#
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth import MCTruthCommon
  MCTruthCommon.addStandardTruthContents()
  MCTruthCommon.addBosonsAndDownstreamParticles(generations=4,rejectHadronChildren=True)
  MCTruthCommon.addTopQuarkAndDownstreamParticles(generations=4,rejectHadronChildren=True)

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM13Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM13Stream )
JETM13Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM13Stream.AcceptAlgs(["JETM13MainKernel"])
# for thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="JETM13ThinningSvc", outStreams=[evtStream] )

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
                                                             ThinningService    = JETM13ThinningHelper.ThinningSvc(),
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

from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction
runTCCReconstruction(jetm13Seq,ToolSvc, "LCOriginTopoClusters", outputTCCName="TrackCaloClustersCombinedAndNeutral")

# Add the necessary constituents for UFOs
addUFOs(jetm13Seq,"JETM13",doCHS=True)


#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
OutputJets["JETM13"] = []
reducedJetList = ["AntiKt4TruthJets","AntiKt10TruthJets",]
replaceAODReducedJets(reducedJetList,jetm13Seq,"JETM13")

jetm13Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM13MainKernel",
                                                          SkimmingTools = [],
                                                          ThinningTools = jetm13thin)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM13SlimmingHelper = SlimmingHelper("JETM13SlimmingHelper")
JETM13SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMTopoJets","AntiKt4EMPFlowJets",
                                        ]
JETM13SlimmingHelper.AllVariables = ["CaloCalTopoClusters",
                                     "TrackCaloClustersCombinedAndNeutral",
                                     "JetETMissChargedParticleFlowObjects",
                                     "JetETMissNeutralParticleFlowObjects",
                                     "Kt4EMTopoOriginEventShape","Kt4EMPFlowEventShape",
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
  "MuonSegments.x.y.z.px.py.pz",
  "AntiKt4LCTopoJets.pt.eta.phi.m",
  ]


JETM13SlimmingHelper.AppendToDictionary["CHSUFO"] = 'xAOD::TrackCaloClusterContainer'
JETM13SlimmingHelper.AppendToDictionary['CHSUFOAux'] = 'xAOD::TrackCaloClusterAuxContainer'
JETM13SlimmingHelper.ExtraVariables +=[ 'CHSUFO.pt.eta.phi.m.taste.iparticleLinks.trackParticleLink' ]

JETM13SlimmingHelper.AppendToDictionary["CSSKUFO"] = 'xAOD::TrackCaloClusterContainer'
JETM13SlimmingHelper.AppendToDictionary['CSSKUFOAux'] = 'xAOD::TrackCaloClusterAuxContainer'
JETM13SlimmingHelper.ExtraVariables +=[ 'CSSKUFO.pt.eta.phi.m.taste.iparticleLinks.trackParticleLink' ]


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
      

# Add the jet containers to the stream
addJetOutputs(JETM13SlimmingHelper,["JETM13"])
# Add the MET containers to the stream
addMETOutputs(JETM13SlimmingHelper,["Track"])

JETM13SlimmingHelper.AppendContentToStream(JETM13Stream)
JETM13Stream.RemoveItem("xAOD::TrigNavigation#*")
JETM13Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")
