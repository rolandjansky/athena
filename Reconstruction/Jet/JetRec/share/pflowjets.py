# myjets.py
#
# David Adams
# May 2014
#
# These top-level job options demonstrate how a user can configure
# jet finding

#--------------------------------------------------------------
# Input stream
#--------------------------------------------------------------
#infile = "/afs/cern.ch/user/d/dadams/pubdata/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.recon.AOD_5ev.root"
infile = "/afs/cern.ch/user/d/dadams/pubdata/r20.1.4.3test_AOD.pool.root"
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [infile]

#--------------------------------------------------------------
# Message logging.
#--------------------------------------------------------------

# Allow message service more than the default 500 lines.
ServiceMgr.MessageSvc.infoLimit = 100000

# Format the messge service lines.
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed. Default is end [-1].
theApp.EvtMax = 5

#--------------------------------------------------------------
# Jet control flags.
#--------------------------------------------------------------

# Exclude modifiers that cause problems.
# B/c they require cells?
from JetRec.JetRecFlags import jetFlags
jetFlags.skipTools += ["jetens", "caloqual_cluster"]

# Import the jet tool manager.
from JetRec.JetRecStandardToolManager import jtm

# Import the jet reconstruction control flags.
from JetRec.JetRecFlags import jetFlags

# Flag to show messges while running.
#   0 - no messages
#   1 - Indicates which jetrec tool is running
#   2 - Details about jetrec execution including which modfier
#   3 - Plus messages from the finder
#   4 - Plus messages from the jet builder
jetFlags.debug = 0
ServiceMgr.MessageSvc.verboseLimit = 100000

#--------------------------------------------------------------
# Configure jet reconstruction.
#--------------------------------------------------------------

doEMPFlowJets = True
doEMCPFlowJets = True
doLCPFlowJets = False
findJets = True

# Special flag to create jet containers in trigger format.
#jtm.useTriggerStore = True

# Build a new list of jet inputs.
# See JetRec/python/JetRecStandardTools.py for standard definitions
# and syntax for adding more.
# Here we add a new list of inputs with the name "mygetters".
print jtm.gettersMap.keys()

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
if findJets:
  jtm.addJetFinder("MyAntiKt4TruthJets",  "AntiKt", 0.4,  "truth",   "truth",
                   ghostArea=0.01 , ptmin=2000, ptminFilter=3000)
if doEMPFlowJets and findJets:
  jtm.addJetFinder("MyAntiKt4EMPFlowJets",  "AntiKt", 0.4,  "empflow", "pflow",
                   ghostArea=0.01 , ptmin=2000, ptminFilter=7000, calibOpt="a:pflow")
if doEMCPFlowJets and findJets:
  jtm.addJetFinder("MyAntiKt4EMCPFlowJets", "AntiKt", 0.4, "emcpflow", "pflow",
                   ghostArea=0.01 , ptmin=2000, ptminFilter=7000, calibOpt="a:pflow")
if doLCPFlowJets and findJets:
  jtm.addJetFinder("MyAntiKt4LCPFlowJets",  "AntiKt", 0.4,  "lcpflow", "pflow",
                   ghostArea=0.01 , ptmin=2000, ptminFilter=7000, calibOpt="a:pflow")

#--------------------------------------------------------------

# Add neutral-only pflow.

# Add neutral pflow builder.
# LC-scale neutral pflow.

from eflowRec.eflowRecFlags import jobproperties

if True == jobproperties.eflowRecFlags.useUpdated2015ChargedShowerSubtraction:
  useChargedWeights = True
else:
  useChargedWeights = False

from JetRecTools.JetRecToolsConf import PFlowPseudoJetGetter
jtm += PFlowPseudoJetGetter(
  "lcnpflowget",
  Label = "LCPFlow",
  OutputContainer = "PseudoJetLCNPFlow",
  RetrievePFOTool = jtm.pflowretriever,
  InputIsEM = False,
  CalibratePFO = False,
  UseChargedWeights = useChargedWeights,
  SkipNegativeEnergy = True,
  UseNeutral = True,
  UseCharged = False
)

# Add neutral-only pflow jet definition.
# Note here we provide an explicit list of getter instead of
# the name of such a list in the jtm map. Either is allowed.
if doLCPFlowJets and findJets:
  jtm.addJetFinder("MyAntiKt4LCNPFlowJets", "AntiKt", 0.4, [jtm.lcnpflowget],  [jtm.width],
                   ghostArea=0.01 , ptmin=5000, ptminFilter=7000)

#--------------------------------------------------------------
# Add jet reco to the algorithm sequence.
# The current configuration of the jet tool manager is used.
#--------------------------------------------------------------
from JetRec.JetAlgorithm import addJetRecoToAlgSequence
#addJetRecoToAlgSequence(eventShapeTools=["empflow", "emcpflow", "lcpflow"])
if doEMCPFlowJets:
  addJetRecoToAlgSequence(eventShapeTools=["emcpflow"])
else:
  addJetRecoToAlgSequence(eventShapeTools=[])
#jtm.setOutputLevel("EMPFlowEventShape", VERBOSE)

#--------------------------------------------------------------
# Add tool to dump the new jet container to the log.
#--------------------------------------------------------------
from JetRec.JetRecConf import JetDumper
from JetRec.JetAlgorithm import jetalg
dumps = []
if doEMPFlowJets:
  dumps += [jtm.empflowget]
if doEMCPFlowJets:
  dumps += [jtm.emcpflowget]
if doLCPFlowJets:
  dumps += [jtm.lcpflowget, jtm.lcnpflowget]
dumps += jtm.jetrecs
for jetrec in dumps:
  print 1
  print jetrec
  print 2
  name = jetrec.name()
  cname = name
  isPFlow = name.find("PFlow") >= 0
  if name == "empflowget": cname =  "PseudoJetEMPFlow"
  if name == "emcpflowget": cname = "PseudoJetEMCPFlow"
  if name == "lcpflowget": cname =  "PseudoJetLCPFlow"
  if name == "lcnpflowget": cname = "PseudoJetLCNPFlow"
  tname = name + "Dumper"
  ToolSvc += JetDumper(tname)
  dumper = getattr(ToolSvc, tname)
  dumper.ContainerName = cname
  dumper.MaxObject = 10000
  # Set Detail=2 and LineDetail=3 to show the moments including ECPSFraction.
  dumper.Detail = 1
  dumper.LineDetail = 2
  dumper.FloatMoments += ["pt"]
  dumper.FloatMoments += ["m"]
  dumper.FloatMoments += ["eta"]
  dumper.FloatMoments += ["phi"]
  if isPFlow:
    dumper.FloatMoments += ["ECPSFraction"]
  jetalg.Tools += [dumper]
