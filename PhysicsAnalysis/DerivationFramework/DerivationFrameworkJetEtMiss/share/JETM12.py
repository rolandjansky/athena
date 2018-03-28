#====================================================================
# jETM12.py 
# reductionConf flag jETM12 in Reco_tf.py   
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

jetm12Seq = CfgMgr.AthSequencer("JETM12Sequence")
DerivationFrameworkJob += jetm12Seq

from DerivationFrameworkJetEtMiss.TCCReconstruction import runTCCReconstruction
runTCCReconstruction(jetm12Seq,ToolSvc)

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
reducedJetList = ["AntiKt4TruthJets",]
replaceAODReducedJets(reducedJetList,jetm12Seq,"JETM12")

jetm12Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM12MainKernel", 
                                                          SkimmingTools = [],
                                                          ThinningTools = [])

#====================================================================
# Special jets
#====================================================================

OutputJets["JETM12"] = []

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM12Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM12Stream )
JETM12Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM12Stream.AcceptAlgs(["JETM12MainKernel"])
# for thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="JETM12ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM12SlimmingHelper = SlimmingHelper("JETM12SlimmingHelper")
JETM12SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMTopoJets","AntiKt4EMPFlowJets",
                                        ]
JETM12SlimmingHelper.AllVariables = ["CaloCalTopoClusters",
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
    JETM12SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM12SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

# Add the jet containers to the stream
addJetOutputs(JETM12SlimmingHelper,["JETM12"])
# Add the MET containers to the stream
addMETOutputs(JETM12SlimmingHelper,["Track"])

JETM12SlimmingHelper.AppendContentToStream(JETM12Stream)
JETM12Stream.RemoveItem("xAOD::TrigNavigation#*")
JETM12Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")
