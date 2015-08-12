# test_RunJetRec.py
#
# David Adams
# February 2014
#
# Top-level job options to test jet reconstruction.
#
# Run with
# > athena.py test_RunJetRec.py
#

# Message prefix
myname = "test_RunJetRec"

# Dumping prefix
dmpprefix = 2

# Control what gets dumped.
dumpAnything = True
dumpClusters   = dumpAnything and False
dumpPseudojets = dumpAnything and True
dumpUngroomed  = dumpAnything and True
dumpGroomed    = dumpAnything and True

# import the jet flags.
from JetRec.JetRecFlags import jetFlags
jetFlags.containerNamePrefix = "Run2"

UseTriggerStore = False
jetFlags.skipTools = []

# Tag with track jets. This is now done by default.
isTrackJetTagged = True

# Specify input file.
infile = "/afs/cern.ch/user/d/dadams/pubdata/xaod_clusters.root"
clname = "Clusters"
vertexCollectionName = ""
doEventShape = False
if 0:
  # 16sep2014 run 1 data
  jetFlags.useTruth = False
  doEventShape = True
  #infile = "/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/DATA/AOD/data12_8TeV.CurrentRef.AOD.pool.root"
  infile = "/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/DATA/AOD/data12_8TeV.00209109.physics_JetTauEtmiss.merge.AOD.19_0_2_1._lb0186._SFO-1._0001.pool.root.1"
else:
  # rdotoesd result from Dec05 devval rel_6
  infile = "/afs/cern.ch/user/d/dadams/pubdata/r20test_AOD.pool.root"

# Flag to show messges while running.
#   0 - no messages
#   1 - Indicates which jetrec tool is running
#   2 - Details about jetrec execution including which modfier
#   3 - Plus messages from the finder
#   4 - Plus messages from the jet builder
jetFlags.debug = 0

# Timing flags for JetToolRunner and JetRecTool.
# 0 for none, 1 for some, 2 for detailed
timelev = 0
jetFlags.timeJetToolRunner = timelev
jetFlags.timeJetRecTool = timelev

# Event shape tools (calculate rho).
if doEventShape:
  jetFlags.eventShapeTools = ["emtopo", "lctopo"]

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
theApp.EvtMax = 2

#--------------------------------------------------------------
# Import the jet tool manager.
#--------------------------------------------------------------

from JetRec.JetRecStandardToolManager import jtm
jtm.debug = 0    # Nonzero to log more messages during configuration

#--------------------------------------------------------------
# Configure tools and algorithm.
#--------------------------------------------------------------

if len(vertexCollectionName):
  print myname + "Overriding vertex collection name to " + vertexCollectionName
  jtm.vertexContainer = vertexCollectionName

# Read job options and import the jet algorithm.
from RunJetRec import runJetGrooming, useLArHVCorr, useDRTruthFlavor
from JetRec.JetAlgorithm import jetalg

if not runJetGrooming:
  dumpGroomed = False

if len(vertexCollectionName):
  print myname + "Overriding vertex collection name to " + vertexCollectionName
  jtm.tvassoc.unlock()
  jtm.tvassoc.VertexContainer = vertexCollectionName
  jtm.tvassoc.lock()

if 0:
  print myname + "Setting output level to VERBOSE for all jetrecs"
  for jetrec in jtm.jetrecs:
    jtm.setOutputLevel(jetrec, VERBOSE)
elif 0:
  print myname + "Setting output level to DEBUG for all jetrecs"
  for jetrec in jtm.jetrecs:
    jtm.setOutputLevel(jetrec, DEBUG)

if 0:
  print myname + "Setting output level to VERBOSE for jet builder"
  jtm.setOutputLevel("jbld", VERBOSE)

if 0:
  print myname + "Setting output level to VERBOSE for muon segment builder"
  jtm.setOutputLevel("gmusegget", VERBOSE)

if 0:
  print myname + "Setting output level to VERBOSE for jet isolation"
  jtm.setOutputLevel("jetisol", VERBOSE)

verbosetools = []
if 0:
  verbosetools += ["jvf"]
  verbosetools += ["jvfloose"]
#verbosetools += ["calib_AntiKt4TopoEM_reco_arj"]
#verbosetools += ["LCTopoEventShape"]
#verbosetools = ["truthsel"]
#verbosetools = ["showerdec"]
#verbosetools = ["EMTopoEventShape"]
#verbosetools = ["gmusegget"]
#verbosetools = ["Run2Split040CamKt12LCTopoJetsGroomer"]
for toolname in verbosetools:
  print myname + "Setting output level to VERBOSE for " + toolname
  jtm.setOutputLevel(toolname, VERBOSE)

#--------------------------------------------------------------
# Configure output streams.
#--------------------------------------------------------------

# Allow message service more than default 500 lines.
ServiceMgr.MessageSvc.infoLimit = 1000000
ServiceMgr.MessageSvc.verboseLimit = 1000000
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"

#jtm.lcget.OutputLevel = DEBUG
#jtm.Run2AntiKt4LCTopoJets.OutputLevel = DEBUG
#jtm.Run2AntiKt10LCTopoJets.OutputLevel = DEBUG

#--------------------------------------------------------------
# Configure tools.
#--------------------------------------------------------------

# Dump before reco.
if dumpClusters:
  from JetRec.JetRecConf import JetDumper
  ToolSvc += JetDumper("inpdmp")
  inpdmp = ToolSvc.inpdmp
  inpdmp.ContainerName = clname
  inpdmp.Detail = 1
  inpdmp.MaxObject = 20
  inpdmp.OutputLevel = INFO
  jetalg.Tools += [inpdmp]

# Dump pseudojets after reco.
names = []
if dumpPseudojets:
  if jetFlags.useTruth():
    names += ["PseudoJetTruth"]
    names += ["PseudoJetTruthWZ"]
  names += ["PseudoJetEMTopo"]
  names += ["PseudoJetLCTopo"]
  if jetFlags.useTracks():
    names += ["PseudoJetTracks"]
  if jetFlags.useTruth():
    names += ["PseudoJetGhostTruth"]
    for ptype in jetFlags.truthFlavorTags():
      names += ["PseudoJetGhost" + ptype]
  if jetFlags.useMuonSegments():
    names += ["PseudoJetGhostMuonSegment"]
icount = 0
for name in names:
  tname = "psjdmp" + str(icount)
  icount += 1
  from JetRec.JetRecConf import JetDumper
  psjdmp = JetDumper(tname)
  ToolSvc += psjdmp
  psjdmp.ContainerName = name
  psjdmp.Detail = 1
  psjdmp.LineDetail = 2
  psjdmp.LineDetail = 4
  psjdmp.Prefix = dmpprefix
  psjdmp.MaxObject = 20
  psjdmp.OutputLevel = INFO
  jetalg.Tools += [psjdmp]

# Dump ungroomed jets.
names = []
if dumpUngroomed:
  if jetFlags.useTruth():
    names += ["Run2AntiKt4TruthJets"]
    names += ["Run2AntiKt4TruthWZJets"]
  if jetFlags.useTracks():
    names += ["Run2AntiKt4TrackJets"]
    names += ["Run2AntiKt4ZTrackJets"]
    names += ["Run2AntiKt2PV0TrackJets"]
    names += ["Run2AntiKt3PV0TrackJets"]
    names += ["Run2AntiKt4PV0TrackJets"]
  names += ["Run2AntiKt4EMTopoJets"]
  names += ["Run2AntiKt4LCTopoJets"]
  names += ["Run2AntiKt10LCTopoJets"]
  names += ["Run2CamKt12LCTopoJets"]
icount = 0
for name in names:
  isTruth = name.find("Truth") >= 0
  isTrack = name.find("Track") >= 0
  isVTrack = name.find("ZTrack") + name.find("PV0Track")>= 0
  isTopo = name.find("Topo") >= 0
  isAntiKt4 = name.find("AntiKt4") >= 0
  hasActiveArea = isTopo
  icount += 1
  tname = "jdmp" + str(icount)
  from JetRec.JetRecConf import JetDumper
  jdmp = JetDumper(tname)
  ToolSvc += jdmp
  jdmp.ContainerName = name
  jdmp.Detail = 2
  jdmp.LineDetail = 3
  jdmp.Prefix = dmpprefix
  jdmp.MaxObject = 20
  jdmp.IntMoments = ["AlgorithmType", "InputType"]
  jdmp.IntMoments += ["ConstituentScale"]
  if isTopo:
    jdmp.IntMoments += ["OriginCorrected"]
    jdmp.IntMoments += ["PileupCorrected"]
  if jetFlags.useMuonSegments() and isTopo:
    jdmp.IntMoments += ["GhostMuonSegmentCount"]
  if jetFlags.useTruth():
    for ptype in jetFlags.truthFlavorTags():
      jdmp.IntMoments += ["Ghost" + ptype + "Count"]
    jdmp.IntMoments += ["PartonTruthLabelID"]
  jdmp.FloatMoments = ["SizeParameter"]
  if hasActiveArea:
    jdmp.FloatMoments += ["JetGhostArea", "ActiveArea"]
  jdmp.FloatMoments += ["pt"]
  jdmp.FloatMoments += ["m"]
  jdmp.FloatMoments += ["eta"]
  jdmp.FloatMoments += ["phi"]
  if useDRTruthFlavor:
    jdmp.IntMoments += ["ConeTruthLabelID"]
    jdmp.FloatMoments += ["TruthLabelDeltaR_B"]
    jdmp.FloatMoments += ["TruthLabelDeltaR_C"]
    jdmp.FloatMoments += ["TruthLabelDeltaR_T"]
  if jetFlags.useTracks() and isTopo:
    jdmp.IntMoments += ["GhostTrackCount"]
    jdmp.FloatMoments += ["GhostTrackPt"]
    jdmp.FloatMoments += ["Charge"]
    jdmp.FloatMoments += ["ShowerDeconstructionW"]
    jdmp.FloatMoments += ["ShowerDeconstructionTop"]
    if isTrackJetTagged:
      jdmp.IntMoments += ["GhostAntiKt2TrackJetCount"]
      jdmp.IntMoments += ["GhostAntiKt3TrackJetCount"]
      jdmp.IntMoments += ["GhostAntiKt4TrackJetCount"]
      jdmp.FloatMoments += ["GhostAntiKt2TrackJetPt"]
      jdmp.FloatMoments += ["GhostAntiKt3TrackJetPt"]
      jdmp.FloatMoments += ["GhostAntiKt4TrackJetPt"]
    if isTopo and isAntiKt4:
      jdmp.FloatMoments += ["GhostTrackAssociationFraction"]
      jdmp.ElementLinkMoments += ["GhostTrackAssociationLink"]
  if jetFlags.useTruth():
    if isTopo or isTrack:
      jdmp.IntMoments += ["GhostTruthCount"]
      jdmp.FloatMoments += ["GhostTruthPt"]
    if isTopo:
      jdmp.FloatMoments += ["GhostTruthAssociationFraction"]
      jdmp.ElementLinkMoments += ["GhostTruthAssociationLink"]
  if isTopo:
    jdmp.FloatMoments += ["EMFrac", "HECFrac"]
    jdmp.FloatMoments += ["ECPSFraction"]
    if jetFlags.useCaloQualityTool():
      #jdmp.FloatMoments += ["LArQuality", "Timing", "HECQuality", "NegativeE", "AverageLArQF", "CentroidR"]
      jdmp.FloatMoments += ["Timing", "LArQuality", "HECQuality", "NegativeE", "AverageLArQF", "CentroidR"]
      jdmp.FloatMoments += ["OotFracClusters5", "OotFracClusters10", "FracSamplingMax"]
    jdmp.FloatMoments += ["BchCorrCell"]
    #jdmp.FloatMoments += ["BchCorrDotxc", "BchCorrJet", "BchCorrJetForCell"]
    jdmp.FloatMoments += ["PullMag", "PullPhi"]
    jdmp.FloatMoments += ["Pull_C00", "Pull_C01", "Pull_C10", "Pull_C11"]
  jdmp.FloatMoments += ["IsoDelta2SumPt"]
  jdmp.FloatMoments += ["IsoDelta3SumPt"]
  jdmp.FloatMoments += ["Width"]
  jdmp.FloatMoments += ["KtDR"]
  if useLArHVCorr:
    jdmp.FloatMoments += ["LArBadHVEnergyFrac", "LArBadHVNCellFrac"]
  jdmp.FloatMoments += ["Tau1", "Tau2", "Tau3"]
  jdmp.FloatMoments += ["Split12", "Split23", "Split34", "ZCut12", "ZCut23", "ZCut34"]
  jdmp.FloatMoments += ["Angularity"]
  jdmp.FloatMoments += ["Dip12", "Dip13", "Dip23", "DipExcl12"]
  jdmp.FloatMoments += ["PlanarFlow"]
  jdmp.FloatMoments += ["Mu12"]
  jdmp.FloatMoments += ["ECF1", "ECF2", "ECF3"]
  if isTopo and jetFlags.useTracks():
    jdmp.FloatMoments += ["Jvt"]
    jdmp.FloatMoments += ["JvtRpt"]
    jdmp.FloatMoments += ["JvtJvfcorr"]
  if not "comshapes" in jetFlags.skipTools():
    jdmp.FloatMoments += ["ThrustMin", "ThrustMaj"]
    jdmp.FloatMoments += ["FoxWolfram0","FoxWolfram1", "FoxWolfram2", "FoxWolfram3", "FoxWolfram4"]
    jdmp.FloatMoments += ["Sphericity", "Aplanarity"]
  jdmp.FourVectorMoments = []
  jdmp.FourVectorMoments += ["jetP4()"]
  jdmp.FourVectorMoments += ["JetConstitScaleMomentum"]
  if isTopo:
    jdmp.FourVectorMoments += ["JetOriginConstitScaleMomentum"]
    jdmp.FourVectorMoments += ["JetEMScaleMomentum"]
  if hasActiveArea:
    jdmp.FourVectorMoments += ["ActiveArea4vec"]
  if isTopo and jetFlags.useTracks():
    jdmp.IntVectorMoments += ["NumTrkPt500"]
    jdmp.IntVectorMoments += ["NumTrkPt1000"]
    jdmp.FloatVectorMoments += ["SumPtTrkPt500"]
    jdmp.FloatVectorMoments += ["SumPtTrkPt1000"]
    jdmp.FloatVectorMoments += ["TrackWidthPt500"]
    jdmp.FloatVectorMoments += ["TrackWidthPt1000"]
  if isTopo:  # Jet cluster moments
    jdmp.FloatMoments += ["LeadingClusterPt"]
    jdmp.FloatMoments += ["LeadingClusterSecondLambda"]
    jdmp.FloatMoments += ["LeadingClusterCenterLambda"]
    jdmp.FloatMoments += ["LeadingClusterSecondR"]
  if isTopo:  # Fix this when energy sampling is working again
    jdmp.FloatVectorMoments += ["EnergyPerSampling"]
  if isTopo and jetFlags.useTracks():
    jdmp.FloatVectorMoments += ["JVF"]
    #jdmp.FloatVectorMoments += ["JVFLoose"]
  if isVTrack:
    jdmp.ElementLinkMoments += ["OriginVertex"]
  if isTopo and jetFlags.useTracks():
    jdmp.ElementLinkMoments += ["HighestJVFVtx"]
    #jdmp.ElementLinkMoments += ["HighestJVFLooseVtx"]
  if isTopo or isTrack:
    jdmp.AssociatedParticleVectors += ["GhostTruth"]
  if isTopo:
    jdmp.AssociatedParticleVectors += ["GhostTrack"]
    if isTrackJetTagged:
      jdmp.AssociatedParticleVectors += ["GhostAntiKt2TrackJet"]
      jdmp.AssociatedParticleVectors += ["GhostAntiKt3TrackJet"]
      jdmp.AssociatedParticleVectors += ["GhostAntiKt4TrackJet"]
  if isTopo:
    jdmp.AssociatedParticleVectors += ["GhostMuonSegment"]
  if isTruth:
    for ptype in jetFlags.truthFlavorTags():
      jdmp.AssociatedParticleVectors += ["Ghost" + ptype]
  if jetFlags.useTruth():
    jdmp.AssociatedParticleVectors += ["ConeExclBHadronsFinal"]
    jdmp.AssociatedParticleVectors += ["ConeExclCHadronsFinal"]
    jdmp.AssociatedParticleVectors += ["ConeExclTausFinal"]
    jdmp.IntMoments += ["HadronConeExclTruthLabelID"]
  jdmp.OutputLevel = INFO
  jetalg.Tools += [jdmp]

# Dump jets after grooming.
if dumpGroomed:
  from JetRec.JetRecConf import JetDumper
  ToolSvc += JetDumper("gdmp1")
  gdmp1 = ToolSvc.gdmp1
  gdmp1.ContainerName = "Run2Split040CamKt12LCTopoJets"
  gdmp1.Detail = 2
  gdmp1.LineDetail = 3
  gdmp1.Prefix = dmpprefix
  gdmp1.MaxObject = 20
  gdmp1.IntMoments += ["AlgorithmType", "InputType"]
  gdmp1.IntMoments += ["TransformType", "NSubjetMax"]
  gdmp1.IntMoments += ["NSubjet"]
  gdmp1.CBoolMoments = ["BDRS"]
  gdmp1.FloatMoments = ["MuMax", "YMin", "RClus", 
                        "SizeParameter",
                        "DRFilt", "MuFilt", "YFilt"]
  hasActiveArea = True
  if hasActiveArea:
    gdmp1.FloatMoments += ["JetGhostArea", "ActiveArea"]
    gdmp1.FourVectorMoments = ["ActiveArea4vec"]
  gdmp1.AssociatedParticleVectors += ["GhostTruth"]
  gdmp1.ElementLinkMoments = ["Parent"]
  gdmp1.OutputLevel = INFO
  jetalg.Tools += [gdmp1]

if dumpGroomed:
  from JetRec.JetRecConf import JetDumper
  ToolSvc += JetDumper("gdmp2")
  gdmp2 = ToolSvc.gdmp2
  gdmp2.ContainerName = "Run2TrimR3AntiKt10LCTopoJets"
  gdmp2.Detail = 2
  gdmp2.LineDetail = 3
  gdmp2.Prefix = dmpprefix
  gdmp2.MaxObject = 20
  gdmp2.IntMoments += ["AlgorithmType", "InputType"]
  gdmp2.IntMoments += ["TransformType"]
  gdmp2.FloatMoments = ["SizeParameter", "JetGhostArea", "ActiveArea",
                        "RClus", "PtFrac"]
  gdmp2.FourVectorMoments = ["ActiveArea4vec"]
  gdmp2.AssociatedParticleVectors += ["GhostTruth"]
  gdmp2.ElementLinkMoments = ["Parent"]
  gdmp2.OutputLevel = INFO
  jetalg.Tools += [gdmp2]

if dumpGroomed:
  from JetRec.JetRecConf import JetDumper
  ToolSvc += JetDumper("gdmp3")
  gdmp3 = ToolSvc.gdmp3
  gdmp3.ContainerName = "Run2PruneR30Z10AntiKt10LCTopoJets"
  gdmp3.Detail = 2
  gdmp3.LineDetail = 3
  gdmp3.Prefix = dmpprefix
  gdmp3.MaxObject = 20
  gdmp3.IntMoments += ["AlgorithmType", "InputType"]
  gdmp3.IntMoments += ["TransformType"]
  gdmp3.FloatMoments = ["SizeParameter", "JetGhostArea", "ActiveArea",
                        "RCut", "ZCut"]
  gdmp3.FourVectorMoments = ["ActiveArea4vec"]
  gdmp3.AssociatedParticleVectors += ["GhostTruth"]
  gdmp3.ElementLinkMoments = ["Parent"]
  gdmp3.OutputLevel = INFO
  jetalg.Tools += [gdmp3]

