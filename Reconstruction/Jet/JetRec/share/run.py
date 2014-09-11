# run.py
#
# David Adams
# January 2014
#
# Example job options for running jet reconstruction.
#
# Run with
# > athena.py run.py
#

# Specify input file.
infile = "/afs/cern.ch/user/d/dadams/pubdata/xaod_clusters.root"
clname = "Clusters"
if 1:
  infile = "/afs/cern.ch/user/d/delsart/public/AOD.dev_rel3.pool.root"
  clname = "CaloCalTopoCluster"
if 0:
  infile = "/afs/cern.ch/user/d/dadams/pubdata/xaod_jetclusters.root"
  clname = "JetClusters"

UseTriggerStore = False
groomer = "massdrop"  # massdrop, trim or prune
groomer = ""

#--------------------------------------------------------------
# Input stream
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [infile]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed. Default is end [-1].
theApp.EvtMax =  1

#--------------------------------------------------------------
# Configure algorithm.
#--------------------------------------------------------------

# Allow messge service more than default 500 lines.
ServiceMgr.MessageSvc.infoLimit = 10000

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add the algorithm. It runs the demo tools.
from JetRec.JetRecConf import JetAlgorithm
job += JetAlgorithm("jetalg")
jetalg = job.jetalg
jetalg.OutputLevel = INFO

#--------------------------------------------------------------
# Configure tools.
#--------------------------------------------------------------

# Dump before reco.
if 1:
  from JetRec.JetRecConf import JetDumper
  ToolSvc += JetDumper("inpdmp")
  inpdmp = ToolSvc.inpdmp
  inpdmp.ContainerName = clname
  inpdmp.Detail = 1
  inpdmp.MaxObject = 20
  inpdmp.OutputLevel = INFO
  jetalg.Tools += [inpdmp]

# JetRec tool for finding.
if 1:
  from JetRec.JetRecConf import JetRecTool
  ToolSvc += JetRecTool("jetrec")
  jetrec = ToolSvc.jetrec
  jetrec.OutputContainer = "MyNewJets"
  jetrec.OutputLevel = DEBUG
  jetalg.Tools += [jetrec]

# Find jet inputs.
if 1:
  from JetRec.JetRecConf import PseudoJetGetter
  ToolSvc += PseudoJetGetter("psjget")
  psjget = ToolSvc.psjget
  psjget.InputContainer = clname
  psjget.Label = "Cluster"
  psjget.OutputContainer = "PseudoJetClusters"
  psjget.OutputLevel = INFO
  psjget.SkipNegativeEnergy = True
  psjget.GhostScale = 0.0
  jetrec.PseudoJetGetters += [psjget]

# Test ghosts
if 0:
  from JetRec.JetRecConf import PseudoJetGetter
  ToolSvc += PseudoJetGetter("psjgetg")
  psjgetg = ToolSvc.psjgetg
  psjgetg.InputContainer = clname
  psjgetg.Label = "GhostCluster"
  psjgetg.OutputContainer = "PseudoJetClusters"
  psjgetg.OutputLevel = INFO
  psjgetg.SkipNegativeEnergy = True
  psjgetg.GhostScale = 0.1
  jetrec.PseudoJetGetters += [psjgetg]

# Jet builder.
if 1:
  from JetRec.JetRecConf import JetFromPseudojet
  ToolSvc += JetFromPseudojet("jbld")
  jbld = ToolSvc.jbld
  jbld.Attributes = ["ActiveArea", "ActiveArea4vec"]
  jbld.OutputLevel = INFO

# Find jets.
if 1:
  from JetRec.JetRecConf import JetFinder
  ToolSvc += JetFinder("jfind")
  jfind = ToolSvc.jfind
  jfind.JetAlgorithm = "CamKt"    # Kt, AntiKt, CamKt
  jfind.JetRadius = 0.8
  jfind.GhostArea = 0.01;
  jfind.PtMin = 20000.0
  jfind.JetBuilder = jbld
  jfind.OutputLevel = DEBUG
  jetrec.JetFinder = jfind
  jetrec.Trigger = UseTriggerStore

# Dump pseudojets after reco.
if 1:
  from JetRec.JetRecConf import JetDumper
  ToolSvc += JetDumper("psjdmp")
  psjdmp = ToolSvc.psjdmp
  psjdmp.ContainerName = "PseudoJetClusters"
  psjdmp.Detail = 1
  psjdmp.LineDetail = 2
  psjdmp.MaxObject = 20
  psjdmp.OutputLevel = INFO
  jetalg.Tools += [psjdmp]

# Copy jets with a new JetRecTool.
if 1:
  from JetRec.JetRecConf import JetRecTool
  ToolSvc += JetRecTool("jetrec2")
  jetrec2 = ToolSvc.jetrec2
  jetrec2.OutputContainer = "MyCopiedJets"
  jetrec2.InputContainer = "MyNewJets"
  jetrec2.OutputLevel = INFO
  jetalg.Tools += [jetrec2]

# Add filter for copied jets.
if 1:
  from JetRec.JetRecConf import JetFilterTool
  ToolSvc += JetFilterTool("jetfil")
  jetfil = ToolSvc.jetfil
  jetfil.PtMin = 50000
  jetfil.OutputLevel = INFO
  jetrec2.JetModifiers += [jetfil]
  jetrec2.Trigger = UseTriggerStore

# Add jet width moment builder
if 1:
  from JetMomentTools.JetMomentToolsConf import JetWidthTool
  ToolSvc += JetWidthTool("jetwidth")
  jetwidth = ToolSvc.jetwidth
  jetwidth.OutputLevel = INFO
  jetrec2.JetModifiers += [jetwidth]

# Add KtDr moment builder
if 1:
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtDeltaRTool
  ToolSvc += KtDeltaRTool("ktdr")
  ktdr = ToolSvc.ktdr
  ktdr.OutputLevel = INFO
  jetrec2.JetModifiers += [ktdr]

# Dump copied jets.
if 1:
  from JetRec.JetRecConf import JetDumper
  ToolSvc += JetDumper("jdmp2")
  ToolSvc += JetDumper("jdmp2")
  jdmp2 = ToolSvc.jdmp2
  jdmp2.ContainerName = "MyCopiedJets"
  jdmp2.Detail = 2
  jdmp2.LineDetail = 3
  jdmp2.MaxObject = 20
  jdmp2.IntMoments = ["AlgorithmType"]
  jdmp2.FloatMoments = ["SizeParameter", "JetGhostArea", "ActiveArea", "Width", "KtDR"]
  jdmp2.FourVectorMoments = ["ActiveArea4vec"]
  jdmp2.OutputLevel = INFO
  jetalg.Tools += [jdmp2]

# Groom jets with a new JetRecTool.
groom = len(groomer)
if groom:
  from JetRec.JetRecConf import JetRecTool
  ToolSvc += JetRecTool("jetrec3")
  jetrec3 = ToolSvc.jetrec3
  jetrec3.OutputContainer = "MyGroomedJets"
  jetrec3.InputContainer = "MyCopiedJets"
  jetrec3.OutputLevel = DEBUG
  jetrec3.Trigger = UseTriggerStore
  jetalg.Tools += [jetrec3]
  grooming_params = []

# Mass-drop grooming
if groom and groomer == "massdrop":
  from JetRec.JetRecConf import MassDropFilter
  ToolSvc += MassDropFilter("jmdf")
  jmdf = ToolSvc.jmdf
  jmdf.MuMax = 1.0
  jmdf.YMin = 0.20
  jmdf.RClus = 0.30
  jmdf.JetBuilder = jbld
  jmdf.OutputLevel = VERBOSE
  jetrec3.JetGroomer = jmdf
  grooming_params = ["DRFilt", "MuFilt", "YFilt"]

# Trimming
elif groom and groomer == "trim":
  from JetRec.JetRecConf import JetTrimmer
  ToolSvc += JetTrimmer("trim")
  trim = ToolSvc.trim
  trim.RClus = 0.30
  trim.PtFrac = 0.05
  trim.JetBuilder = jbld
  trim.OutputLevel = VERBOSE
  jetrec3.JetGroomer = trim

# Pruning
elif groom and groomer == "prune":
  from JetRec.JetRecConf import JetPruner
  ToolSvc += JetPruner("prun")
  prun = ToolSvc.prun
  jfind.JetAlgorithm = "CamKt"    # Kt, AntiKt, CamKt
  prun.RCut = 0.30
  prun.ZCut = 0.10
  prun.JetBuilder = jbld
  prun.OutputLevel = VERBOSE
  jetrec3.JetGroomer = prun

# Dump jets after grooming.
if 1 and groom:
  from JetRec.JetRecConf import JetDumper
  ToolSvc += JetDumper("gdmp")
  gdmp = ToolSvc.gdmp
  gdmp.ContainerName = "MyGroomedJets"
  gdmp.Detail = 2
  gdmp.LineDetail = 3
  gdmp.MaxObject = 20
  gdmp.IntMoments = ["AlgorithmType"]
  gdmp.FloatMoments = ["SizeParameter", "JetGhostArea", "ActiveArea"] + grooming_params
  gdmp.FourVectorMoments = ["ActiveArea4vec"]
  gdmp.ElementLinkMoments = ["Parent"]
  gdmp.OutputLevel = INFO
  jetalg.Tools += [gdmp]

