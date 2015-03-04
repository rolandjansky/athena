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
infile = "/afs/cern.ch/user/d/dadams/pubdata/r20test_AOD.pool.root"
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [infile]

#--------------------------------------------------------------
# Message logging.
#--------------------------------------------------------------

# Allow message service more than the default 500 lines.
ServiceMgr.MessageSvc.infoLimit = 100000
ServiceMgr.MessageSvc.verboseLimit = 100000

# Format the messge service lines.
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed. Default is end [-1].
theApp.EvtMax = 2

#--------------------------------------------------------------
# Jet control flags.
#--------------------------------------------------------------

# Import the jet reconstruction control flags.
from JetRec.JetRecFlags import jetFlags

# Import the jet tool manager.
from JetRec.JetRecStandardToolManager import jtm

# Flag to show messges while running.
#   0 - no messages
#   1 - Indicates which jetrec tool is running
#   2 - Details about jetrec execution including which modfier
#   3 - Plus messages from the finder
#   4 - Plus messages from the jet builder
#jetFlags.debug = 0
ServiceMgr.MessageSvc.verboseLimit = 100000
#jtm.setOutputLevel("jblda", VERBOSE)
#jtm.setOutputLevel("jetens", VERBOSE)

#--------------------------------------------------------------
# Configure jet reconstruction.
#--------------------------------------------------------------

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
  "calib:arj", 
  "jetfilter",
  jtm.nsubjettiness,
  jtm.jetens,
  jtm.caloqual_cluster,
  jtm.width
]
trimmods = []
trimmods = [jtm.jetens]


# Add tools to find or groom jets.
# Each call to addJetFinder adds one JetRecTool so that one jet
# container will be added to the event.
# The first argument is the name of that container.
# The next two are the jet algorithm (Kt, AntiKt, CamKt) and
# size parameter.
# The next three are the names of the input lit, modifier lists
# and consumer list.
# The following optional, named arguments may also be provided:
#   ghostArea: Size in eta-phi for area ghosts
#   ptmin: pT threshold in MeV applied in jet finding
#   ptminFilter: pT threshold applied by the jet modifier "jetfilter"

trimexisting = True
debugpseudojets = False

# Re-find the ungroomed jets and copy their pseudojets to the original collection.
findcons = []
if trimexisting:
  from JetRec.JetRecConf import JetPseudojetCopier
  jtm += JetPseudojetCopier(
    "myjpjcopier",
    DestinationContainer ="AntiKt4LCTopoJets",
    JetPseudojetRetriever =jtm.jpjretriever
  );
  findcons = [jtm.myjpjcopier]
  if debugpseudojets:
    jtm.setOutputLevel("myjpjcopier", VERBOSE)
jtm.addJetFinder("MyAntiKt4LCTopoJets", "AntiKt", 0.4, "mygetters", "mymods", findcons,
                 ghostArea=0.01 , ptmin=2000, ptminFilter=7000)

# Trim the new ungroomed jets.
jtm.addJetTrimmer("MyAntiKt4LCTopoTrimmedJets", 0.05, 0.10, "MyAntiKt4LCTopoJets", trimmods)

# Trim the original ungroomed jets using the pseudojets from the new ungroomed jets.
if trimexisting:
  if debugpseudojets:
    pjrname = "jpjretriever"
    jtm.setOutputLevel(pjrname, VERBOSE)
    jtm.addJetTrimmer("AntiKt4LCTopoTrimmedJets", 0.05, 0.10, "AntiKt4LCTopoJets", trimmods,
                      pseudojetRetriever=pjrname)
  else:
    jtm.addJetTrimmer("AntiKt4LCTopoTrimmedJets", 0.05, 0.10, "AntiKt4LCTopoJets", trimmods)

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
show0 = True
if show0:
  ToolSvc += JetDumper("jetdumper0")
  jd0 = ToolSvc.jetdumper0
  jd0.ContainerName = "AntiKt4LCTopoJets"
  jd0.Detail = 1
  jd0.LineDetail = 3
  jd0.FourVectorMoments += ["JetEMScaleMomentum"]
  jd0.MaxObject = 500
  jetalg.Tools += [jd0]
ToolSvc += JetDumper("jetdumper")
jd1 = ToolSvc.jetdumper
jd1.ContainerName = "MyAntiKt4LCTopoJets"
jd1.Detail = 1
jd1.LineDetail = 3
jd1.FourVectorMoments += ["JetEMScaleMomentum"]
jd1.MaxObject = 500
jetalg.Tools += [jd1]
ToolSvc += JetDumper("jetdumper2")
jd2 = ToolSvc.jetdumper2
jd2.ContainerName = "MyAntiKt4LCTopoTrimmedJets"
jd2.Detail = 1
jd2.LineDetail = 3
jd2.FourVectorMoments += ["JetEMScaleMomentum"]
jd2.MaxObject = 500
jetalg.Tools += [jd2]
if trimexisting:
  ToolSvc += JetDumper("jetdumper3")
  jd3 = ToolSvc.jetdumper3
  jd3.ContainerName = "AntiKt4LCTopoTrimmedJets"
  jd3.Detail = 1
  jd3.LineDetail = 3
  jd3.FourVectorMoments += ["JetEMScaleMomentum"]
  jd3.MaxObject = 500
  jetalg.Tools += [jd3]
