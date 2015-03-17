# vrjets.py
#
# David Adams
# June 2014
#
# These top-level job options demonstrate how a user can configure
# finding of variable-R jets

#--------------------------------------------------------------
# Input stream
#--------------------------------------------------------------
infile = "/afs/cern.ch/user/d/dadams/pubdata/r20test_AOD.pool.root"
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

# Import the jet reconstruction control flags.
from JetRec.JetRecFlags import jetFlags

# Flag to show messges while running.
#   0 - no messages
#   1 - Indicates which jetrec tool is running
#   2 - Details about jetrec execution including which modfier
#   3 - Plus messages from the finder
#   4 - Plus messages from the jet builder
jetFlags.debug = 0

#--------------------------------------------------------------
# Configure jet reconstruction.
#--------------------------------------------------------------

# Import the jet tool manager.
from JetRec.JetRecStandardToolManager import jtm

# Special flag to create jet containers in trigger format.
#jtm.useTriggerStore = True

# Build a new list of jet inputs.
# See JetRec/python/JetRecStandardTools.py for standard definitions
# and syntax for adding more.
# Here we add a new list of inputs with the name "mygetters".
jtm.gettersMap["mygetters"] = [jtm.lcget]
print jtm.gettersMap.keys()

# Build a new list of jet modifiers.
# See JetRec/python/JetRecStandardTools.py for standard definitions
# and syntax for adding more.
# In addition to these tools, a few special strings may be used to 
# specify tools whose configuration depends on the input jet
# collection. These include
#   calib:OPT where OPT = a, r, j, ...
#   truthassoc
#   trackassoc
#   jetfilter
# Here we add a new list of inputs with the name "mymods".
jtm.modifiersMap["mymods"] = [
  "calib:ar", 
  "jetfilter",
  jtm.nsubjettiness,
  jtm.jetens,
  jtm.caloqual_cluster,
  jtm.width
]

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
jtm.addJetFinder("MyAntiKtVRLCTopoJets", "AntiKt", 0.4, "mygetters", "mymods",
                 ghostArea=0.01 , ptmin=2000, ptminFilter=7000,
                 variableRMinRadius=0.2, variableRMassScale=60000)

#--------------------------------------------------------------
# Add jet reco to the algorithm sequence.
# The current configuration of the jet tool manager is used.
#--------------------------------------------------------------
from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence()

#--------------------------------------------------------------
# Add tool to dump the new jet container to the log.
#--------------------------------------------------------------
from JetRec.JetRecConf import JetDumper
from JetRec.JetAlgorithm import jetalg

ToolSvc += JetDumper("jetdumper0")
jetdumper0 = ToolSvc.jetdumper0
jetdumper0.ContainerName = "MyAntiKtVRLCTopoJets"
jetdumper0.Detail = 2
jetdumper0.FloatMoments = ["SizeParameter"]
jetdumper0.FloatMoments += ["VariableRMinRadius", "VariableRMassScale"]
jetdumper0.MaxObject = 1
jetalg.Tools += [jetdumper0]

ToolSvc += JetDumper("jetdumper")
jetdumper = ToolSvc.jetdumper
jetdumper.ContainerName = "MyAntiKtVRLCTopoJets"
jetdumper.MaxObject = 500
jetalg.Tools += [jetdumper]
