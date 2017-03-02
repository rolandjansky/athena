# JetRec standalone reconstruction jobOptions

infile = "/atlas/data1/userdata/khoo/Data16/AOD_r21/mc16_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2997_r8903_r8906/AOD.10226638._000244.pool.root.1"
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [infile]

theApp.EvtMax = 50

from AthenaCommon import CfgMgr
# Convert EM clusters to pseudojets
emget = CfgMgr.PseudoJetGetter(
  "emget",
  InputContainer = "CaloCalTopoClusters",
  Label = "EMTopo",
  OutputContainer = "PseudoJetEMTopo",
  SkipNegativeEnergy = True,
  GhostScale = 0.0
)
ToolSvc += emget

# Set up the jet finder
JetFinder_AntiKt4 = CfgMgr.JetFinder("MTAntiKt4EMTopoJetsFinder",
                                     JetAlgorithm = "AntiKt",
                                     JetRadius = 0.4,
                                     GhostArea = 0.01,
                                     PtMin = 2000)

#Then we setup a jet builder to calculate the areas needed for the rho subtraction
# Actually, we don't really need the areas but we may as well use this one
from AthenaCommon.AppMgr import ToolSvc
JetBuilder_AntiKt4 = CfgMgr.JetFromPseudojet("jblda", Attributes = ["ActiveArea", "ActiveArea4vec"])
ToolSvc += JetBuilder_AntiKt4
JetFinder_AntiKt4.JetBuilder = JetBuilder_AntiKt4
ToolSvc += JetFinder_AntiKt4

#Now we setup a JetRecTool which will use the above JetFinder
JetRecTool = CfgMgr.JetRecTool("MTAntiKt4EMTopoJets")
JetRecTool.JetFinder = JetFinder_AntiKt4
ToolSvc += JetRecTool
JetRecTool.PseudoJetGetters = [emget]
JetRecTool.OutputContainer = "MTAntiKt4EMTopoJets"

jpjretriever = CfgMgr.JetPseudojetRetriever("jpjretriever")
ToolSvc += jpjretriever

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
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

