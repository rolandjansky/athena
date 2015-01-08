# reclustejets.po
#
# David Adams
# November 2014
#
# These top-level job options demonstrate how a user can configure
# reclustering of jets (i.e. building jets from the constituents in
# each of the jets in a container).
#
# If the reclustering algorithm is the same and the radius is the same
# or larger as those for the parent, then the new jets should have the
# same constituents and # cluster sequences as the original ones.
# How true is this?
#
# If reclustering is done with a smaller radius, then subjets are found.

#--------------------------------------------------------------
# Helper functions.
#--------------------------------------------------------------

# Fn to convert name (e.g. AntiKt4) to radius:
def name2rad(name):
  for alg in ["AntiKt", "Kt", "CamKt"]:
    if name[0:len(alg)] == alg:
      return 0.1*int(name[len(alg):])
  raise KeyError

# Fn to convert name (e.g. AntiKt4) to algorithm
def name2alg(name):
  for alg in ["AntiKt", "Kt", "CamKt"]:
    if name[0:len(alg)] == alg:
      return alg
  raise KeyError

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
#jetFlags.debug = 2
ServiceMgr.MessageSvc.debugLimit = 100000
ServiceMgr.MessageSvc.verboseLimit = 100000
#jtm.setOutputLevel("jblda", VERBOSE)
#jtm.setOutputLevel("jbldna", VERBOSE)
#jtm.setOutputLevel("jetens", VERBOSE)
#jtm.setOutputLevel("jconretriever", VERBOSE)

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
groommods = []
# This causes a crash (25nov2014).
#groommods = [jtm.jetens]


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

groomexisting = False
debugpseudojets = False

# Recluster the old ungroomed jets.
parent="AntiKt4LCTopo"
algrads = ["AntiKt4", "AntiKt3", "CamKt6"]
for ar in algrads:
  jtm.addJetReclusterer(parent+ar+"Jets", name2alg(ar), name2rad(ar), parent+"Jets", groommods)

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
# Jets read from file.
if show0:
  ToolSvc += JetDumper("jetdumper0")
  jd0 = ToolSvc.jetdumper0
  jd0.ContainerName = "AntiKt4LCTopoJets"
  jd0.Detail = 1
  jd0.LineDetail = 4
  jd0.FourVectorMoments += ["JetEMScaleMomentum"]
  jd0.MaxObject = 500
  jetalg.Tools += [jd0]
# Reclustered jets.
for ar in algrads:
  jd = JetDumper("jetdumper2" + ar)
  ToolSvc += jd
  jd.ContainerName = parent+ar+"Jets"
  jd.Detail = 1
  jd.LineDetail = 4
  jd.FourVectorMoments += ["JetEMScaleMomentum"]
  jd.MaxObject = 500
  jetalg.Tools += [jd]
