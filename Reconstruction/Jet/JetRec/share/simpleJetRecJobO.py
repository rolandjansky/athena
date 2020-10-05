# JetRec standalone reconstruction jobOptions
import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()

from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.OutputLevel( INFO )
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )

from AthenaCommon import CfgMgr
# Convert EM clusters to pseudojets
# In the midst of this migration, we can only cope with one pseudojet type
# otherwise we end up trying to update an object in SG, which is not allowed
# Needs JE's PseudojetContainer developments
emget = CfgMgr.PseudoJetAlgorithm(
  "emget",
  InputContainer = "CaloCalTopoClusters",
  Label = "EMTopo",
  OutputContainer = "PseudoJetEMTopo",
  SkipNegativeEnergy = True,
  GhostScale = 0.0,
#  OutputLevel=VERBOSE,
)

topSequence += emget

# Set up the jet finder
JetFinder_AntiKt4 = CfgMgr.JetFinder("MTAntiKt4EMTopoJetsFinder",
                                     JetAlgorithm = "AntiKt",
                                     JetRadius = 0.4,
                                     GhostArea = 0.01,
                                     PtMin = 2000,
                                     OutputLevel=VERBOSE,
                                     )

#Then we setup a jet builder to calculate the areas needed for the rho subtraction
# Actually, we don't really need the areas but we may as well use this one
JetBuilder_AntiKt4 = CfgMgr.JetFromPseudojet("jblda", Attributes = ["ActiveArea", "ActiveArea4vec"],
                                             OutputLevel=VERBOSE)
JetFinder_AntiKt4.JetBuilder = JetBuilder_AntiKt4

#Now we setup a JetRecTool which will use the above JetFinder
JetRecTool = CfgMgr.JetRecTool("MTAntiKt4EMTopoJets",
                               OutputLevel=VERBOSE)
JetRecTool.JetFinder = JetFinder_AntiKt4
JetRecTool.InputPseudoJets = [emget.OutputContainer]
JetRecTool.OutputContainer = "MTAntiKt4EMTopoJets"

topSequence += CfgMgr.JetAlgorithm("MTJetAlg",Tools = [JetRecTool])

write_xAOD = True
if write_xAOD:
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.pool.root" )
    xaodStream.AddItem("xAOD::EventInfo#EventInfo")
    xaodStream.AddItem("xAOD::EventAuxInfo#EventInfoAux.")
    xaodStream.AddItem("xAOD::JetContainer#AntiKt4EMTopoJets")
    xaodStream.AddItem("xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.")
    xaodStream.AddItem("xAOD::JetContainer#MTAntiKt4EMTopoJets")
    xaodStream.AddItem("xAOD::JetAuxContainer#MTAntiKt4EMTopoJetsAux.")
    xaodStream.AddItem("xAOD::CaloClusterContainer#CaloCalTopoClusters")
    xaodStream.AddItem("xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.")

