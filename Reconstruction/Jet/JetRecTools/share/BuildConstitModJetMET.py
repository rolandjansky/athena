import AthenaPoolCnvSvc.ReadAthenaPool

############################################################################
# Set up detector description and magnetic field service
# This is needed for track extrapolation when computing
# isolation variables in MET track cleaning

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'

from RecExConfig.InputFilePeeker import inputFileSummary
#print inputFileSummary
if inputFileSummary['evt_type'][0] == 'IS_DATA':
    globalflags.DataSource = 'data'
else:
    globalflags.DataSource = 'geant4'

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()
if hasattr(DetFlags,'BField_on'): DetFlags.BField_setOn()
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
AtlasTrackingGeometrySvc  = svcMgr.AtlasTrackingGeometrySvc

from AtlasGeoModel import SetGeometryVersion, GeoModelInit
GeoModelSvc = ServiceMgr.GeoModelSvc
GeoModelSvc.AtlasVersion = inputFileSummary['metadata']['/TagInfo']['GeoAtlas']

include('RecExCond/AllDet_detDescr.py')


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
metalgs = []
metoutputs = []
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
            metalg,metout = ConstModHelpers.getConstModMETAlg(modseqshort, "PFlow" in inputtype)
            metalgs.append(metalg)
            metoutputs.append(metout)

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

for metalg in metalgs:
    topSequence += metalg

############################################################################
# Define the output stream content

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.PFSKJetMET.pool.root" )

xaodStream.AddItem('xAOD::EventInfo#EventInfo')
xaodStream.AddItem('xAOD::EventAuxInfo#EventInfoAux.')

xaodStream.AddItem('xAOD::VertexContainer#PrimaryVertices')
xaodStream.AddItem('xAOD::VertexAuxContainer#PrimaryVerticesAux.')

xaodStream.AddItem('xAOD::ElectronContainer#Electrons')
xaodStream.AddItem('xAOD::ElectronAuxContainer#ElectronsAux.')

xaodStream.AddItem('xAOD::PhotonContainer#Photons')
xaodStream.AddItem('xAOD::PhotonAuxContainer#PhotonsAux.')

xaodStream.AddItem('xAOD::MuonContainer#Muons')
xaodStream.AddItem('xAOD::MuonAuxContainer#MuonsAux.')

xaodStream.AddItem('xAOD::TauJetContainer#TauJets')
xaodStream.AddItem('xAOD::TauJetAuxContainer#TauJetsAux.')

xaodStream.AddItem('xAOD::JetContainer#AntiKt4EMTopoJets')
xaodStream.AddItem('xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.')

xaodStream.AddItem('xAOD::JetContainer#AntiKt4EMPFlowJets')
xaodStream.AddItem('xAOD::JetAuxContainer#AntiKt4EMPFlowJetsAux.')

xaodStream.AddItem('xAOD::JetContainer#AntiKt4TruthJets')
xaodStream.AddItem('xAOD::JetAuxContainer#AntiKt4TruthJetsAux.')

xaodStream.AddItem('xAOD::MissingETContainer#MET_Core_AntiKt4EMTopo')
xaodStream.AddItem('xAOD::MissingETAuxContainer#MET_Core_AntiKt4EMTopoAux.')

xaodStream.AddItem('xAOD::MissingETAssociationMap#METAssoc_AntiKt4EMTopo')
xaodStream.AddItem('xAOD::MissingETAuxAssociationMap#METAssoc_AntiKt4EMTopoAux.')

xaodStream.AddItem('xAOD::MissingETContainer#MET_Core_AntiKt4EMPFlow')
xaodStream.AddItem('xAOD::MissingETAuxContainer#MET_Core_AntiKt4EMPFlowAux.')

xaodStream.AddItem('xAOD::MissingETAssociationMap#METAssoc_AntiKt4EMPFlow')
xaodStream.AddItem('xAOD::MissingETAuxAssociationMap#METAssoc_AntiKt4EMPFlowAux.')

xaodStream.AddItem('xAOD::MissingETContainer#MET_Reference_AntiKt4EMTopo')
xaodStream.AddItem('xAOD::MissingETAuxContainer#MET_Reference_AntiKt4EMTopoAux.')

xaodStream.AddItem('xAOD::MissingETContainer#MET_Truth')
xaodStream.AddItem('xAOD::MissingETAuxContainer#MET_TruthAux.')

xaodStream.AddItem('xAOD::EventShape#Kt4*EventShape')
xaodStream.AddItem('xAOD::EventShapeAuxInfo#Kt4*EventShapeAux.')

for jf in jetfinders:
    xaodStream.AddItem('xAOD::JetContainer#'+jf.name())
    xaodStream.AddItem('xAOD::JetAuxContainer#'+jf.name()+'Aux.')

for outputdict in metoutputs:
    for cont,name in outputdict.iteritems():
        xaodStream.AddItem('{0}#{1}'.format(cont,name))

#ServiceMgr.StoreGateSvc.Dump=True

svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=1000)
