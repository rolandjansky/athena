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
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

#====================================================================
# SKIMMING TOOL 
#====================================================================
# None for the moment

#====================================================================
# THINNING TOOLS 
#====================================================================
# None for the moment -- just use TRUTH3 truth containers
# We want to keep all constituents

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
# Here we run custom reconstruction

jetm12Seq = CfgMgr.AthSequencer("JETM13Sequence")
DerivationFrameworkJob += jetm12Seq

from DerivationFrameworkJetEtMiss.TCCReconstruction import runTCCReconstruction
runTCCReconstruction(jetm12Seq,ToolSvc)

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
reducedJetList = ["AntiKt4TruthJets","AntiKt10TruthJets",]
replaceAODReducedJets(reducedJetList,jetm12Seq,"JETM13")

jetm12Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM13MainKernel", 
                                                          SkimmingTools = [],
                                                          ThinningTools = [])

#====================================================================
# Special jets
#====================================================================

OutputJets["JETM13"] = []

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
                                     "JetETMissChargedParticleFlowObjects", "JetETMissNeutralParticleFlowObjects",
                                     "Kt4EMTopoOriginEventShape","Kt4EMPFlowEventShape",
                                     ]

for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthTaus",
    "TruthNeutrinos"
    ]:
    JETM13SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM13SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

# Add the jet containers to the stream
addJetOutputs(JETM13SlimmingHelper,["JETM13"])
# Add the MET containers to the stream
addMETOutputs(JETM13SlimmingHelper,["Track"])

JETM13SlimmingHelper.AppendContentToStream(JETM13Stream)
JETM13Stream.RemoveItem("xAOD::TrigNavigation#*")
JETM13Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")
