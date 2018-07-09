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
# We want to keep all constituents
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
                                                           WriteFirstN        = -1)
ToolSvc += TruthThinningTool

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
# Here we run custom reconstruction

jetm13Seq = CfgMgr.AthSequencer("JETM13Sequence")
DerivationFrameworkJob += jetm13Seq

from DerivationFrameworkJetEtMiss.TCCReconstruction import runTCCReconstruction
runTCCReconstruction(jetm13Seq,ToolSvc)

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
reducedJetList = ["AntiKt4TruthJets","AntiKt10TruthJets",]
replaceAODReducedJets(reducedJetList,jetm13Seq,"JETM13")

jetm13Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM13MainKernel",
                                                          SkimmingTools = [],
                                                          ThinningTools = [TruthThinningTool])

#====================================================================
# Special jets
#====================================================================

OutputJets["JETM13"] = []

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
                                     "TruthEvents",
                                     ]

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
  "TruthBoson"
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
