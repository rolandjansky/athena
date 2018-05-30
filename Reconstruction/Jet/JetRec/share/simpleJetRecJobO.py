# JetRec standalone reconstruction jobOptions

# infile = "/atlas/data1/userdata/khoo/Data16/AOD_r21/mc16_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2997_r8903_r8906/AOD.10226638._000244.pool.root.1"

infile = "root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r7572_tid07644622_00/RDO.07644622._000001.pool.root.1"
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [infile]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()

if nThreads>0:
    from SGComps.SGCompsConf import SGInputLoader
    topSequence += SGInputLoader(OutputLevel=INFO,ShowEventDump=True)
    topSequence.SGInputLoader.Load = [ ('xAOD::EventInfo','EventInfo'),
                                       ('xAOD::CaloClusterContainer','CaloCalTopoClusters')]

    from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
    svcMgr += ForwardSchedulerSvc()
    # Check that dependencies are OK
    # Default to extract any missing dependencies from file
#    ServiceMgr.ForwardSchedulerSvc.CheckDependencies = True
#    ServiceMgr.ForwardSchedulerSvc.DataLoaderAlg = "SGInputLoader"

theApp.EvtMax = 5

from AthenaCommon import CfgMgr
# Convert EM clusters to pseudojets
# In the midst of this migration, we can only cope with one pseudojet type
# otherwise we end up trying to update an object in SG, which is not allowed
# Needs JE's PseudojetContainer developments
emget = CfgMgr.PseudoJetGetter(
  "emget",
  InputContainer = "CaloCalTopoClusters",
  Label = "EMTopo",
  OutputContainer = "PseudoJetEMTopo",
  SkipNegativeEnergy = True,
  GhostScale = 0.0,
  OutputLevel=DEBUG,
)
ToolSvc += emget

topSequence += CfgMgr.PseudoJetAlgorithm("pjalg_EMTopo",PJGetter=emget)

# Set up the jet finder
JetFinder_AntiKt4 = CfgMgr.JetFinder("MTAntiKt4EMTopoJetsFinder",
                                     JetAlgorithm = "AntiKt",
                                     JetRadius = 0.4,
                                     GhostArea = 0.01,
                                     PtMin = 2000,
                                     )

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
JetRecTool.InputPseudoJets = [emget.OutputContainer]
JetRecTool.OutputContainer = "MTAntiKt4EMTopoJets"

#jpjretriever = CfgMgr.JetPseudojetRetriever("jpjretriever")
#ToolSvc += jpjretriever

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

