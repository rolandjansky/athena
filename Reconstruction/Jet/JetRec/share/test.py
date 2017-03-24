theApp.EvtMax = 10

#infile="/afs/cern.ch/work/j/jstupak/mc16_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.recon.AOD.e3668_s2997_r9064/AOD.10618464._001695.pool.root.1"
#outputName="JZ4.Out.pool.root"
infile="/afs/cern.ch/work/j/jstupak/mc16_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.merge.AOD.e3569_s2997_r9064_r8996/AOD.10618338._000230.pool.root.1"
outputName="JZ1.Out.pool.root"

from AthenaCommon.AppMgr import ServiceMgr
#ServiceMgr.MessageSvc.OutputLevel = VERBOSE

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

############################################################################################

from copy import copy
def listReplace(l,old,new):
  result=copy(l)
  i=result.index(old)
  result.pop(i)
  result.insert(i,new)
  return result

############################################################################################


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

jtm.addJetFinder("MyAntiKt4TruthJets",  "AntiKt", 0.4,  "truth",   "truth",
                   ghostArea=0.01 , ptmin=2000, ptminFilter=3000)

#--------------------------------------------------------------
"""
jtm.addJetFinder("MyAntiKt4EMPFlowJets",  "AntiKt", 0.4,  "empflow", "pflow_ungroomed", ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")

from JetRec.JetRecStandardToolManager import empfgetters,pflow_ungroomed_modifiers
jtm.addJetFinder("MyAntiKt4EMPFlowJets2",  "AntiKt", 0.4,  empfgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")

############################################################################################3

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
jtm.addJetFinder("MyAntiKt4EMPFlowJets3",  "AntiKt", 0.4,  myempfgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")
"""
############################################################################################
#Use a sequence to correct PFO and apply CHS
"""
from PFlowUtils.PFlowUtilsConf import CP__RetrievePFOTool as RetrievePFOTool
jtm += RetrievePFOTool("mypflowretriever",
    NeutralInputContainer="correctedJetETMissNeutralParticleFlowObjects",
    ChargedInputContainer="correctedJetETMissChargedParticleFlowObjects",
    )

from JetRecTools.JetRecToolsConf import PFlowPseudoJetGetter
jtm += PFlowPseudoJetGetter(
  "myempflowget2",
  Label = "EMPFlow",
  OutputContainer = "MyPseudoJetEMPFlow2",
  RetrievePFOTool = jtm.mypflowretriever,
  WeightPFOTool = jtm.pflowweighter,
  InputIsEM = True,
  CalibratePFO = True, #False
  SkipNegativeEnergy = True,
  UseChargedWeights = False, #True
  UseVertices = False, #True
  UseTrackToVertexTool = False,
  ApplyChargedHadronSubtraction = False, #True
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

from JetRecTools.JetRecToolsConf import ChargedHadronSubtractionTool
CHSTool = ChargedHadronSubtractionTool("CHSTool")
ToolSvc += CHSTool

from JetRecTools.JetRecToolsConf import  JetConstituentModSequence
nominalSequence = JetConstituentModSequence("nominalSequence",
                                            InputContainer = "JetETMiss",
                                            OutputContainer = "correctedJetETMiss",
                                            InputType = "ParticleFlow",
                                            Modifiers = [correctPFOTool,CHSTool],
                                            SaveAsShallow = False,
                                            )
ToolSvc += nominalSequence

from JetRec.JetRecStandardToolManager import empfgetters,pflow_ungroomed_modifiers
myempfgetters=listReplace(empfgetters,jtm.empflowget,jtm.myempflowget2)
jtm.addJetFinder("MyAntiKt4EMPFlowJets4",  "AntiKt", 0.4,  myempfgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")
"""

############################################################################################
#Make standard PFCHS jets as reference for dR(jet) matching

from JetRec.JetRecConf import PseudoJetGetter
jtm += PseudoJetGetter(
  "PFCHSRefGetter",
  Label = "EMPFlow",
  InputContainer = "PFCHSRefParticleFlowObjects",
  OutputContainer = "PFCHSRefPseudoJet",
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

from JetRecTools.JetRecToolsConf import ChargedHadronSubtractionTool
CHSTool = ChargedHadronSubtractionTool("CHSTool")
ToolSvc += CHSTool

from JetRecTools.JetRecToolsConf import  JetConstituentModSequence
PFCHSRefSequence = JetConstituentModSequence("PFCHSRefSequence",
                                          InputContainer = "JetETMiss",
                                          OutputContainer = "PFCHSRef",
                                          InputType = "ParticleFlow",
                                          Modifiers = [correctPFOTool,CHSTool],
                                          SaveAsShallow = False,
                                          )
ToolSvc += PFCHSRefSequence

from JetRec.JetRecStandardToolManager import empfgetters,pflow_ungroomed_modifiers
myPFCHSRefgetters=listReplace(empfgetters,jtm.empflowget,jtm.PFCHSRefGetter)
from JetRec.JetRecStandardToolManager import filterout
pflow_ungroomed_modifiers=filterout(['width'],pflow_ungroomed_modifiers)
jtm.addJetFinder("MyPFCHSRefAntiKt4EMPFlowJets",  "AntiKt", 0.4,  myPFCHSRefgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")

############################################################################################
#Use a sequence and PseudoJetGetter to correct PFO and apply CHS

from JetRecTools.JetRecToolsConf import PFOAnalyzer
PFOana=PFOAnalyzer('PFOana',JetCollection='MyPFCHSRefAntiKt4EMPFlowJets')
ToolSvc += PFOana

from JetRec.JetRecConf import PseudoJetGetter
jtm += PseudoJetGetter(
  "PFCHSGetter",
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
                                      ApplyWeight=False)  #Just calculate the weights
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
from JetRec.JetRecStandardToolManager import filterout
pflow_ungroomed_modifiers=filterout(['width'],pflow_ungroomed_modifiers)
jtm.addJetFinder("MyPFCHSAntiKt4EMPFlowJets",  "AntiKt", 0.4,  myPFCHSgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")

############################################################################################
#Run PUPPI

from JetRec.JetRecConf import PseudoJetGetter
jtm += PseudoJetGetter(
  "PFPUPPICHSGetter",
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

#PFOanaWithMatching=PFOAnalyzer('PFOanaWithMatching',JetCollection='MyPFCHSAntiKt4EMPFlowJets')
#ToolSvc += PFOanaWithMatching

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
from JetRec.JetRecStandardToolManager import filterout
pflow_ungroomed_modifiers=filterout(['width'],pflow_ungroomed_modifiers)
jtm.addJetFinder("MyPFPUPPICHSAntiKt4EMPFlowJets",  "AntiKt", 0.4,  myPFPUPPICHSgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")

############################################################################################
# CorrectPFO only - For comparison

from JetRec.JetRecConf import PseudoJetGetter
jtm += PseudoJetGetter(
  "PFGetter",
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

from JetRecTools.JetRecToolsConf import ChargedHadronSubtractionTool
CHSTool = ChargedHadronSubtractionTool("CHSTool")
ToolSvc += CHSTool

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
from JetRec.JetRecStandardToolManager import filterout
pflow_ungroomed_modifiers=filterout(['width'],pflow_ungroomed_modifiers)
jtm.addJetFinder("MyPFAntiKt4EMPFlowJets",  "AntiKt", 0.4,  myPFgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")

############################################################################################
#CorrectPFO and PUPPI only - for comparison

from JetRec.JetRecConf import PseudoJetGetter
jtm += PseudoJetGetter(
  "PFPUPPIGetter",
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

from JetRecTools.JetRecToolsConf import ChargedHadronSubtractionTool
CHSTool = ChargedHadronSubtractionTool("CHSTool")
ToolSvc += CHSTool

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
from JetRec.JetRecStandardToolManager import filterout
pflow_ungroomed_modifiers=filterout(['width'],pflow_ungroomed_modifiers)
jtm.addJetFinder("MyPFPUPPIAntiKt4EMPFlowJets",  "AntiKt", 0.4,  myPFPUPPIgetters, pflow_ungroomed_modifiers, ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")

############################################################################################

from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence(job=topSequence, separateJetAlgs=True)
"""
if hasattr(topSequence,"jetalgMyAntiKt4EMPFlowJets4"):
  topSequence.jetalgMyAntiKt4EMPFlowJets4.Tools.insert(0,PFCHSSequence)
"""

if hasattr(topSequence,"jetalgMyPFCHSRefAntiKt4EMPFlowJets"):
  topSequence.jetalgMyPFCHSRefAntiKt4EMPFlowJets.Tools.insert(0,PFCHSRefSequence)

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
