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
modkeys = {"EMTopo":"emtopo_constpu","EMPFlow":"pflow_constpu"}
# Remove the truth jet association tool from the moment tool list
jtm.modifiersMap["emtopo_constpu"] = [ tool for tool in jtm.modifiersMap["emtopo_ungroomed"] if tool != "truthassoc" ]
jtm.modifiersMap["pflow_constpu"] = [ tool for tool in jtm.modifiersMap["pflow_ungroomed"] if tool != "truthassoc" ]

# Loop over input types: cluster & PFlow, different modifier chains
# and fill the lists of builder tools
for inputtype in pjbase.keys():
    for modseq in modsequences:
        # This generates the modifier sequence
        cms = ConstModHelpers.getConstModSeq(modseq,inputtype)
        constmodseqs.append(cms)
        # Strip out underscores for jet naming
        modseqshort = "".join(modseq)
        if not "LC" in inputtype: # LC clusters needed just for MET (CST)
            label = inputtype+modseqshort
            pjname = pjbase[inputtype]+modseqshort.lower()
            jf = ConstModHelpers.getJetFinder(inputtype,label,pjname,modkeys[inputtype])
            jetfinders.append(jf)
            pjg = ConstModHelpers.getPseudoJetGetter(label,pjname)
            ed = ConstModHelpers.getEventDensityTool(pjg)
            edtools.append(ed)

############################################################################
# Put all of the tools defined above into algorithms

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

preptools = [jtm.tracksel, jtm.tvassoc, jtm.trackselloose_trackjets]
               
jtm += CfgMgr.JetToolRunner("jetrunskconstit",
                            Tools=preptools+constmodseqs)
topSequence += CfgMgr.JetAlgorithm("jetalgskconstit",Tools=[jtm.jetrunskconstit])

jtm += CfgMgr.JetToolRunner("jetruneventshape",
                            EventShapeTools=edtools)
topSequence += CfgMgr.JetAlgorithm("jetalgeventshape",Tools=[jtm.jetruneventshape])

for finder in jetfinders:
    topSequence += CfgMgr.JetAlgorithm("jetalg"+finder.name(),Tools=[finder])

############################################################################
# Define the output stream content

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.PFSKJetMET.pool.root" )

xaodStream.AddItem('xAOD::EventInfo#EventInfo')
xaodStream.AddItem('xAOD::EventAuxInfo#EventInfoAux.')

xaodStream.AddItem('xAOD::VertexContainer#PrimaryVertices')
xaodStream.AddItem('xAOD::VertexAuxContainer#PrimaryVerticesAux.')

xaodStream.AddItem('xAOD::JetContainer#AntiKt4EMTopoJets')
xaodStream.AddItem('xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.')

xaodStream.AddItem('xAOD::JetContainer#AntiKt4EMPFlowJets')
xaodStream.AddItem('xAOD::JetAuxContainer#AntiKt4EMPFlowJetsAux.')

xaodStream.AddItem('xAOD::JetContainer#AntiKt4TruthJets')
xaodStream.AddItem('xAOD::JetAuxContainer#AntiKt4TruthJetsAux.')

xaodStream.AddItem('xAOD::EventShape#Kt4*EventShape')
xaodStream.AddItem('xAOD::EventShapeAuxInfo#Kt4*EventShapeAux.')

for jf in jetfinders:
    xaodStream.AddItem('xAOD::JetContainer#'+jf.name())
    xaodStream.AddItem('xAOD::JetAuxContainer#'+jf.name()+'Aux.')

#ServiceMgr.StoreGateSvc.Dump=True

svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=1000)
