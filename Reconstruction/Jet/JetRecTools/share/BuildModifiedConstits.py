import AthenaPoolCnvSvc.ReadAthenaPool

############################################################################
# Set up the jet & MET collections

from JetRecTools import ConstModHelpers
from JetRec.JetRecFlags import jetFlags
jetFlags.useTruth.set_Value_and_Lock(False)

# Define the chains of constituent modifiers here
modsequences = [
    ["SK"],
    ["CS","SK"],
    ["Vor","SK"]
    ]

# Set up lists of tools that need to be run
from JetRec.JetRecStandard import jtm
constmodseqs = [jtm.JetConstitSeq_EMOrigin,jtm.JetConstitSeq_LCOrigin]
edtools = []
jetfinders = []
# Define some dicts to help with generating collections in a loop
pjbase = {"EMTopo":"em",
          "LCTopo":"lc",
          "EMPFlow":"pf"}

# Loop over input types: cluster & PFlow, different modifier chains
# and fill the lists of builder tools
for inputtype in pjbase.keys():
    for modseq in modsequences:
        # This generates the modifier sequence
        cms = ConstModHelpers.getConstModSeq(modseq,inputtype)
        constmodseqs.append(cms)
        # Strip out underscores for jet naming
        modseqshort = "".join(modseq)

############################################################################
# Put all of the tools defined above into algorithms

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

preptools = [jtm.tracksel, jtm.tvassoc, jtm.trackselloose_trackjets]
               
jtm += CfgMgr.JetToolRunner("jetrunskconstit",
                            Tools=preptools+constmodseqs)
topSequence += CfgMgr.JetAlgorithm("jetalgskconstit",Tools=[jtm.jetrunskconstit])

############################################################################
# Define the output stream content

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.ConstitMods.pool.root" )

xaodStream.AddItem('xAOD::EventInfo#EventInfo')
xaodStream.AddItem('xAOD::EventAuxInfo#EventInfoAux.')

xaodStream.AddItem('xAOD::CaloClusterContainer#*TopoClusters')
xaodStream.AddItem('xAOD::CaloClusterAuxContainer#*TopoClustersAux.')

xaodStream.AddItem('xAOD::PFOContainer#*ParticleFlowObjects')
xaodStream.AddItem('xAOD::PFOAuxContainer#*ParticleFlowObjectsAux.')

svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=1000)
