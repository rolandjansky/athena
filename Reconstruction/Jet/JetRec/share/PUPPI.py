theApp.EvtMax = 100

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

jtm.addJetFinder("MyAntiKt4TruthJets",  "AntiKt", 0.4,  "truth",   "truth",
                   ghostArea=0.01 , ptmin=2000, ptminFilter=3000)

############################################################################################
#Use a sequence and PseudoJetGetter to correct PFO and apply CHS - This reproduces standard EMPflow jets

from JetRecTools.JetRecToolsConf import PFOAnalyzer
PFOana=PFOAnalyzer('PFOana',JetCollection='AntiKt4EMPFlowJets')
ToolSvc += PFOana

from JetRec.JetRecConf import PseudoJetGetter
jtm += PseudoJetGetter("PFCHSGetter",
                       Label = "EMPFlow",
                       InputContainer = "PFCHSParticleFlowObjects",
                       OutputContainer = "PFCHSPseudoJet",
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
fakePuppiWeightTool = PuppiWeightTool("fakePUPPIWeightTool",
                                      ApplyWeight=False)  #Just calculate the would-be PUPPI weights, don't apply them
ToolSvc += fakePuppiWeightTool

from JetRecTools.JetRecToolsConf import ChargedHadronSubtractionTool
CHSTool = ChargedHadronSubtractionTool("CHSTool")
ToolSvc += CHSTool

from JetRecTools.JetRecToolsConf import  JetConstituentModSequence
PFCHSSequence = JetConstituentModSequence("PFCHSSequence",
                                          InputContainer = "JetETMiss",
                                          OutputContainer = "PFCHS",
                                          InputType = "ParticleFlow",
                                          Modifiers = [correctPFOTool,PFOana,fakePuppiWeightTool,CHSTool],
                                          SaveAsShallow = False,
                                          )
ToolSvc += PFCHSSequence

from JetRec.JetRecStandardToolManager import empfgetters,pflow_ungroomed_modifiers
myPFCHSgetters=listReplace(empfgetters,jtm.empflowget,jtm.PFCHSGetter)
#from JetRec.JetRecStandardToolManager import filterout
#pflow_ungroomed_modifiers=filterout(['width'],pflow_ungroomed_modifiers)
jtm.addJetFinder("MyPFCHSAntiKt4EMPFlowJets",  "AntiKt", 0.4,  myPFCHSgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="j:pflow")

############################################################################################
#Run default PUPPI

from JetRec.JetRecConf import PseudoJetGetter
jtm += PseudoJetGetter("PFPUPPICHSGetter",
                       Label = "EMPFlow",
                       InputContainer = "PFPUPPICHSParticleFlowObjects",
                       OutputContainer = "PFPUPPICHSPseudoJet",
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
PFPUPPICHSSequence = JetConstituentModSequence("PFPUPPICHSSequence",
                                               InputContainer = "JetETMiss",
                                               OutputContainer = "PFPUPPICHS",
                                               InputType = "ParticleFlow",
                                               Modifiers = [correctPFOTool,PFOana,puppiWeightTool,CHSTool],
                                               SaveAsShallow = False,
                                               )
ToolSvc += PFPUPPICHSSequence

from JetRec.JetRecStandardToolManager import empfgetters,pflow_ungroomed_modifiers
myPFPUPPICHSgetters=listReplace(empfgetters,jtm.empflowget,jtm.PFPUPPICHSGetter)
#from JetRec.JetRecStandardToolManager import filterout
#pflow_ungroomed_modifiers=filterout(['width'],pflow_ungroomed_modifiers)
jtm.addJetFinder("MyPFPUPPICHSAntiKt4EMPFlowJets",  "AntiKt", 0.4,  myPFPUPPICHSgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="j:pflow")

############################################################################################
# CorrectPFO only - For comparison only

from JetRec.JetRecConf import PseudoJetGetter
jtm += PseudoJetGetter("PFGetter",
                       Label = "EMPFlow",
                       InputContainer = "PFParticleFlowObjects",
                       OutputContainer = "PFPseudoJet",
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
fakePuppiWeightTool = PuppiWeightTool("fakePUPPIWeightTool")
ToolSvc += fakePuppiWeightTool

from JetRecTools.JetRecToolsConf import  JetConstituentModSequence
PFSequence = JetConstituentModSequence("PFSequence",
                                       InputContainer = "JetETMiss",
                                       OutputContainer = "PF",
                                       InputType = "ParticleFlow",
                                       Modifiers = [correctPFOTool,PFOana,fakePuppiWeightTool],
                                       SaveAsShallow = False,
                                       )
ToolSvc += PFSequence

from JetRec.JetRecStandardToolManager import empfgetters,pflow_ungroomed_modifiers
myPFgetters=listReplace(empfgetters,jtm.empflowget,jtm.PFGetter)
#from JetRec.JetRecStandardToolManager import filterout
#pflow_ungroomed_modifiers=filterout(['width'],pflow_ungroomed_modifiers)
jtm.addJetFinder("MyPFAntiKt4EMPFlowJets",  "AntiKt", 0.4,  myPFgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="j:pflow")

############################################################################################
#CorrectPFO and PUPPI only - for comparison only

from JetRec.JetRecConf import PseudoJetGetter
jtm += PseudoJetGetter("PFPUPPIGetter",
                       Label = "EMPFlow",
                       InputContainer = "PFPUPPIParticleFlowObjects",
                       OutputContainer = "PFPUPPIPseudoJet",
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

from JetRecTools.JetRecToolsConf import  JetConstituentModSequence
PFPUPPISequence = JetConstituentModSequence("PFPUPPISequence",
                                            InputContainer = "JetETMiss",
                                            OutputContainer = "PFPUPPI",
                                            InputType = "ParticleFlow",
                                            Modifiers = [correctPFOTool,PFOana,puppiWeightTool],
                                            SaveAsShallow = False,
                                            )
ToolSvc += PFPUPPISequence

from JetRec.JetRecStandardToolManager import empfgetters,pflow_ungroomed_modifiers
myPFPUPPIgetters=listReplace(empfgetters,jtm.empflowget,jtm.PFPUPPIGetter)
#from JetRec.JetRecStandardToolManager import filterout
#pflow_ungroomed_modifiers=filterout(['width'],pflow_ungroomed_modifiers)
jtm.addJetFinder("MyPFPUPPIAntiKt4EMPFlowJets",  "AntiKt", 0.4,  myPFPUPPIgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="j:pflow")

############################################################################################

from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence(job=topSequence, separateJetAlgs=True)
"""
if hasattr(topSequence,"jetalgMyAntiKt4EMPFlowJets4"):
  topSequence.jetalgMyAntiKt4EMPFlowJets4.Tools.insert(0,PFCHSSequence)
"""

if hasattr(topSequence,"jetalgMyPFCHSAntiKt4EMPFlowJets"):
  topSequence.jetalgMyPFCHSAntiKt4EMPFlowJets.Tools.insert(0,PFCHSSequence)

if hasattr(topSequence,"jetalgMyPFPUPPICHSAntiKt4EMPFlowJets"):
  topSequence.jetalgMyPFPUPPICHSAntiKt4EMPFlowJets.Tools.insert(0,PFPUPPICHSSequence)

if hasattr(topSequence,"jetalgMyPFAntiKt4EMPFlowJets"):
  topSequence.jetalgMyPFAntiKt4EMPFlowJets.Tools.insert(0,PFSequence)

if hasattr(topSequence,"jetalgMyPFPUPPIAntiKt4EMPFlowJets"):
  topSequence.jetalgMyPFPUPPIAntiKt4EMPFlowJets.Tools.insert(0,PFPUPPISequence)

#For validation only
from PFlowUtils.PFlowUtilsConf import PFlowMerger
topSequence+=PFlowMerger('PFCHSMerger',InputContainerNames = [
  'PFCHSChargedParticleFlowObjects',
  'PFCHSNeutralParticleFlowObjects'
  ],OutputContainerName='PFCHSParticleFlowObjects')

topSequence+=PFlowMerger('PFPUPPICHSMerger',InputContainerNames = [
  'PFPUPPICHSChargedParticleFlowObjects',
  'PFPUPPICHSNeutralParticleFlowObjects'
  ],OutputContainerName='PFPUPPICHSParticleFlowObjects')

topSequence+=PFlowMerger('PFMerger',InputContainerNames = [
  'PFChargedParticleFlowObjects',
  'PFNeutralParticleFlowObjects'
  ],OutputContainerName='PFParticleFlowObjects')

topSequence+=PFlowMerger('PFPUPPIMerger',InputContainerNames = [
  'PFPUPPIChargedParticleFlowObjects',
  'PFPUPPINeutralParticleFlowObjects'
  ],OutputContainerName='PFPUPPIParticleFlowObjects')

############################################################################################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xAODStream = MSMgr.NewPoolRootStream( "StreamXAOD", outputName )

xAODStream.AddItem(   "xAOD::JetContainer#*AntiKt4TruthJets*")
xAODStream.AddItem("xAOD::JetAuxContainer#*AntiKt4TruthJets*")

xAODStream.AddItem(   "xAOD::JetContainer#*AntiKt4EM*PFlow*Jets*")
xAODStream.AddItem("xAOD::JetAuxContainer#*AntiKt4EM*PFlow*Jets*")

xAODStream.AddItem(       "xAOD::PFOContainer#*ParticleFlowObjects*")
xAODStream.AddItem(    "xAOD::PFOAuxContainer#*ParticleFlowObjects*")
xAODStream.AddItem("xAOD::ShallowAuxContainer#*ParticleFlowObjects*")
