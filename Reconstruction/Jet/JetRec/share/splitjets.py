# splitjets.py
#
# David Adams
# June 2015
#
# These top-level job options demonstrate how a user can configure
# jet splitting.

#--------------------------------------------------------------
# Input stream
#--------------------------------------------------------------
infile = "/afs/cern.ch/user/d/dadams/pubdata/r20test_AOD.pool.root"
infile = "/afs/cern.ch/work/c/cdelitzs/public/mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.merge.AOD.e2928_s1982_s2008_r5787_r5853_tid01597981_00/AOD.01597981._000608.pool.root.1"
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [infile]

r19data = True

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
ServiceMgr.EventSelector.SkipEvents = 2704

#--------------------------------------------------------------
# Jet control flags.
#--------------------------------------------------------------

# Import the jet reconstruction control flags.
from JetRec.JetRecFlags import jetFlags
jetFlags.useTruth = False

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

if r19data:
  from JetRec.JetRecConf import PseudoJetGetter
  jtm += PseudoJetGetter(
    "lcgetr19",
    InputContainer = "CaloCalTopoCluster",
    Label = "LCTopo",
    OutputContainer = "PseudoJetLCTopo",
    SkipNegativeEnergy = True,
    GhostScale = 0.0
  )

# Build a new list of jet inputs.
# See JetRec/python/JetRecStandardTools.py for standard definitions
# and syntax for adding more.
# Here we add a new list of inputs with the name "mygetters".
jtm.gettersMap["mygetters"] = [jtm.lcgetr19]
print jtm.gettersMap.keys()

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

jtm.addJetFinder("CamKt15LCTopoJets", "CamKt", 1.5, "mygetters", [],
                 ghostArea=0.01 , ptmin=2000)


jtm.addJetSplitter("CamKt15LCTopoSplitJets", 1.0, 0.15, "CamKt15LCTopoJets", [], doArea=True)


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
  jd0.ContainerName = "CamKt15LCTopoJets"
  jd0.Detail = 1
  jd0.LineDetail = 3
  jd0.FourVectorMoments += ["JetEMScaleMomentum"]
  jd0.MaxObject = 500
  jetalg.Tools += [jd0]
ToolSvc += JetDumper("jetdumper")
jd1 = ToolSvc.jetdumper
jd1.ContainerName = "CamKt15LCTopoSplitJets"
jd1.Detail = 1
jd1.LineDetail = 3
jd1.FourVectorMoments += ["JetEMScaleMomentum"]
jd1.MaxObject = 500
jetalg.Tools += [jd1]
