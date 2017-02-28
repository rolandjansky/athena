theApp.EvtMax = 10

infile = "/afs/cern.ch/work/j/jstupak/mc16_13TeV.410007.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad.merge.AOD.e4135_s2997_r8957_r8996/AOD.10340813._000004.pool.root.1"
#infile = "root://eosatlas//eos/atlas/user/m/mdobre/forRTTmc16/MC16_210.AOD.pool.root"
#infile = "/afs/cern.ch/work/j/jstupak/mc15_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.merge.AOD.e3569_s2576_s2132_r7725_r7676/AOD.07916735._000053.pool.root.1"

from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [infile]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from JetRec.JetRecFlags import jetFlags

#from JetRec.JetRecStandardToolManager import jtm
from JetRec.JetRecStandard import jtm

# Flag to show messges while running.
#   0 - no messages
#   1 - Indicates which jetrec tool is running
#   2 - Details about jetrec execution including which modfier
#   3 - Plus messages from the finder
#   4 - Plus messages from the jet builder
jetFlags.debug = 4
ServiceMgr.MessageSvc.verboseLimit = 100000

# Add tools to find or groom jets.
# Each call to addJetFinder adds one JetRecTool so that one jet
# container will be added to the event.
# The first argument is the name of that container.
# The next two are the jet algorithm (Kt, AntiKt, CamKt) and
# size parameter.
# The next two are the names of the input and modifier lists.
# The following optional, named arguments may also be provided:
#   ghostArea: Size in eta-phi for area ghosts
#   ptmin: pT threshold in MeV applied in jet finding
#   ptminFilter: pT threshold applied by the jet modifier "jetfilter"

#--------------------------------------------------------------

jtm.addJetFinder("MyAntiKt4TruthJets",  "AntiKt", 0.4,  "truth",   "truth",
                   ghostArea=0.01 , ptmin=2000, ptminFilter=3000)

jtm.addJetFinder("MyAntiKt4EMPFlowJets",  "AntiKt", 0.4,  "empflow", "pflow_ungroomed", ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")

from JetRec.JetRecStandardToolManager import empfgetters,pflow_ungroomed_modifiers
jtm.addJetFinder("MyAntiKt4EMPFlowJets2",  "AntiKt", 0.4,  empfgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")

############################################################################################3
from copy import copy
def listReplace(l,old,new):
  result=copy(l)
  i=result.index(old)
  result.pop(i)
  result.insert(i,new)
  return result

from JetRecTools.JetRecToolsConf import PFlowPseudoJetGetter
jtm += PFlowPseudoJetGetter(
  "myempflowget",
  Label = "EMPFlow",
  OutputContainer = "MyPseudoJetEMPFlow",
  RetrievePFOTool = jtm.pflowretriever,
  WeightPFOTool = jtm.pflowweighter,
  InputIsEM = True,
  CalibratePFO = False,
  SkipNegativeEnergy = True,
  UseChargedWeights = True,
  UseVertices = True,
  UseTrackToVertexTool = False,
  )

myempfgetters=listReplace(empfgetters,jtm.empflowget,jtm.myempflowget)
print 'JS3:', myempfgetters
jtm.addJetFinder("MyAntiKt4EMPFlowJets3",  "AntiKt", 0.4,  myempfgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")

from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence()

#--------------------------------------------------------------
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xAODStream = MSMgr.NewPoolRootStream( "StreamXAOD", "test.Out.pool.root" )

xAODStream.AddItem(   "xAOD::JetContainer#*AntiKt4EM*PFlow*Jets*")
xAODStream.AddItem("xAOD::JetAuxContainer#*AntiKt4EM*PFlow*Jets*")

xAODStream.AddItem(   "xAOD::PFOContainer#PF*Candidates*")
xAODStream.AddItem("xAOD::PFOAuxContainer#PF*Candidates*")

xAODStream.AddItem(   "xAOD::PFOContainer#JetETMiss*ParticleFlowObjects*")
xAODStream.AddItem("xAOD::PFOAuxContainer#JetETMiss*ParticleFlowObjects*")
