theApp.EvtMax = 10

inputName="/eos/atlas/user/j/jstupak/data/AOD/mc15_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.merge.AOD.e3569_s2576_s2132_r7725_r7676/AOD.07916735._000053.pool.root.1"
outputName="JZ1.root"

from AthenaCommon.AppMgr import ServiceMgr
#ServiceMgr.MessageSvc.OutputLevel = VERBOSE

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [inputName]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from JetRec.JetRecFlags import jetFlags
from JetRec.JetRecStandard import jtm

# Flag to show messges while running.
#   0 - no messages
#   1 - Indicates which jetrec tool is running
#   2 - Details about jetrec execution including which modfier
#   3 - Plus messages from the finder
#   4 - Plus messages from the jet builder
jetFlags.debug = 4
ServiceMgr.MessageSvc.verboseLimit = 100000

############################################################################################

from copy import copy
def listReplace(l,old,new):
  result=copy(l)
  i=result.index(old)
  result.pop(i)
  result.insert(i,new)
  return result

############################################################################################
#Run nominal PUPPI algo (CorrectPFO+PUPPI+CHS)

from JetRec.JetRecConf import PseudoJetGetter
jtm += PseudoJetGetter("PUPPIGetter",
                       Label = "EMPFlow",
                       InputContainer = "PUPPIParticleFlowObjects",
                       OutputContainer = "PUPPIPseudoJets",
                       SkipNegativeEnergy = True,
                       )

from JetRecTools.JetRecToolsConf import CorrectPFOTool
correctPFOTool = CorrectPFOTool("correctPFOTool",
                                WeightPFOTool = jtm.pflowweighter,
                                InputIsEM = True,
                                CalibratePFO = False,
                                UseChargedWeights = True,
                                UseVertices = True,
                                UseTrackToVertexTool = False,
                                )
ToolSvc += correctPFOTool

from JetRecTools.JetRecToolsConf import PuppiWeightTool
puppiWeightTool = PuppiWeightTool("PUPPIWeightTool")
ToolSvc += puppiWeightTool

from JetRecTools.JetRecToolsConf import ChargedHadronSubtractionTool
CHSTool = ChargedHadronSubtractionTool("CHSTool")
ToolSvc += CHSTool

from JetRecTools.JetRecToolsConf import  JetConstituentModSequence
PUPPISequence = JetConstituentModSequence("PUPPISequence",
                                          InputContainer = "JetETMiss",
                                          OutputContainer = "PUPPI",
                                          InputType = "ParticleFlow",
                                          Modifiers = [correctPFOTool,puppiWeightTool,CHSTool],
                                          SaveAsShallow = False,
                                          )
ToolSvc += PUPPISequence

from JetRec.JetRecStandardToolManager import empfgetters,pflow_ungroomed_modifiers
myPUPPIgetters=listReplace(empfgetters,jtm.empflowget,jtm.PUPPIGetter)
pflow_ungroomed_modifiers.remove('calib')
pflow_ungroomed_modifiers.remove('truthassoc')
jtm.addJetFinder("AntiKt4EMPFlowPUPPIJets",  "AntiKt", 0.4,  myPUPPIgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="")

############################################################################################

from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence(job=topSequence, separateJetAlgs=True)

if hasattr(topSequence,"jetalgAntiKt4EMPFlowPUPPIJets"):
  topSequence.jetalgAntiKt4EMPFlowPUPPIJets.Tools.insert(0,PUPPISequence)

############################################################################################

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xAODStream = MSMgr.NewPoolRootStream( "StreamXAOD", outputName )

xAODStream.AddItem(   "xAOD::JetContainer#AntiKt4EMPFlowPUPPIJets")
xAODStream.AddItem("xAOD::JetAuxContainer#AntiKt4EMPFlowPUPPIJets")

xAODStream.AddItem(       "xAOD::PFOContainer#*ParticleFlowObjects*")
xAODStream.AddItem(    "xAOD::PFOAuxContainer#*ParticleFlowObjects*")
xAODStream.AddItem("xAOD::ShallowAuxContainer#*ParticleFlowObjects*")
