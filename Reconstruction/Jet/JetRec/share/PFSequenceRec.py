####################################################################################################
#Use the standard PseudoJetGetter (rather than PFlowPseudoJetGetter) and
#and JetConstituentModSequence to reconstruct PFlow Jets
#
#Authors: John Stupak and Jennifer Roloff
####################################################################################################

theApp.EvtMax = 10

infile = "/afs/cern.ch/work/j/jstupak/mc16_13TeV.410007.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_allhad.merge.AOD.e4135_s2997_r8957_r8996/AOD.10340813._000004.pool.root.1"

from AthenaCommon.AppMgr import ServiceMgr
#ServiceMgr.MessageSvc.OutputLevel = VERBOSE

import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [infile]
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

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

############################################################################################
#Reconstruct PF jets with standard tools (PFlowPseudoJetGetter, etc) for comparison

jtm.addJetFinder("MyAntiKt4EMPFlowJets",  "AntiKt", 0.4,  "empflow_reduced", "pflow_ungroomed", ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")

############################################################################################
#Use a sequence and PseudoJetGetter to reconstruct PF jets

from JetRec.JetRecConf import PseudoJetGetter
jtm += PseudoJetGetter(
  "PFGetter",
  Label = "EMPFlow",
  InputContainer = "MyParticleFlowObjects",
  OutputContainer = "PFPseudoJet",
  SkipNegativeEnergy = True,
  )

import cppyy
try: cppyy.loadDictionary('xAODBaseDict')
except: pass
from ROOT import xAOD

#this tool does much of the PFO manipulations in PFlowPseudoJetGetter
from JetRecTools.JetRecToolsConf import CorrectPFOTool
correctPFOTool = CorrectPFOTool("correctPFOTool",
                                WeightPFOTool = jtm.pflowweighter,
                                InputIsEM = True,
                                CalibratePFO = False,
                                UseChargedWeights = True,
                                UseVertices = True,
                                UseTrackToVertexTool = False,
                                InputType = xAOD.Type.ParticleFlow
                                )
ToolSvc += correctPFOTool

#This tool removes charged hadrons not matched to the primary vertex
from JetRecTools.JetRecToolsConf import ChargedHadronSubtractionTool
CHSTool = ChargedHadronSubtractionTool("CHSTool", InputType = xAOD.Type.ParticleFlow)
ToolSvc += CHSTool

#run the above tools to modify PFO
from JetRecTools.JetRecToolsConf import JetConstituentModSequence
PFSequence = JetConstituentModSequence("PFSequence",
                                       InputContainer = "JetETMiss",
                                       OutputContainer = "My",   #"ParticleFlowObjects" will be appended later
                                       InputType = xAOD.Type.ParticleFlow,
                                       Modifiers = [correctPFOTool,CHSTool],
                                       SaveAsShallow = False,
                                       )
ToolSvc += PFSequence

myGetters=listReplace(jtm.gettersMap["empflow_reduced"],jtm.empflowget,jtm.PFGetter)  #use PFJetter instead of default PFlowPseudoJetGetter
from JetRec.JetRecStandardToolManager import filterout
#pflow_ungroomed_modifiers=filterout(['width'],jtm.modifiersMap["pflow_ungroomed"]) #width tool causes a problem for some reason
jtm.addJetFinder("MyAntiKt4EMPFlowJets2", "AntiKt", 0.4, myGetters, "pflow_ungroomed", ghostArea=0.01 , ptmin=5000, ptminFilter=10000, calibOpt="arj:pflow")

############################################################################################

from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence(job=topSequence, separateJetAlgs=True)

#insert the JetConstituentModSequence before sequence-based PF jet reconstruction
if hasattr(topSequence,"jetalgMyAntiKt4EMPFlowJets2"):
  topSequence.jetalgMyAntiKt4EMPFlowJets2.Tools.insert(0,PFSequence)

############################################################################################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xAODStream = MSMgr.NewPoolRootStream( "StreamXAOD", "test.Out.pool.root" )

xAODStream.AddItem(   "xAOD::JetContainer#*AntiKt4EMPFlowJets*")
xAODStream.AddItem("xAOD::JetAuxContainer#*AntiKt4EMPFlowJets*")

xAODStream.AddItem(       "xAOD::PFOContainer#*ParticleFlowObjects*")
xAODStream.AddItem(    "xAOD::PFOAuxContainer#*ParticleFlowObjects*")
xAODStream.AddItem("xAOD::ShallowAuxContainer#*ParticleFlowObjects*")
