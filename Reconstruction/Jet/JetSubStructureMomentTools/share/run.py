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
infile = "/home/mileswu/xaod_jets.root"
clname = "AntiKt4LCTopoJets"

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
  inpdmp.ContainerName = "JetClusters"
  inpdmp.Detail = 1
  inpdmp.MaxObject = 20
  inpdmp.OutputLevel = INFO
  jetalg.Tools += [inpdmp]

# JetRec tool for finding.
if 0:
  from JetRec.JetRecConf import JetRecTool
  ToolSvc += JetRecTool("jetrec")
  jetrec = ToolSvc.jetrec
  jetrec.OutputContainer = "MyNewJets"
  jetrec.OutputLevel = INFO
  jetalg.Tools += [jetrec]

# Find jet inputs.
if 0:
  from JetRec.JetRecConf import PseudoJetGetter
  ToolSvc += PseudoJetGetter("psjget")
  psjget = ToolSvc.psjget
  psjget.InputContainer = clname
  psjget.Label = "Cluster"
  psjget.OutputContainer = "PseudoJetClusters"
  psjget.OutputLevel = INFO
  psjget.SkipNegativeEnergy = True
  jetrec.PseudoJetGetters += [psjget]

# Find jets.
if 0:
  from JetRec.JetRecConf import JetFinder
  ToolSvc += JetFinder("jfind")
  jfind = ToolSvc.jfind
  jfind.JetAlgorithm = "AntiKt"
  jfind.JetRadius = 0.8
  jfind.GhostArea = 0.01;
  jfind.PtMin = 20000.0
  jfind.OutputLevel = VERBOSE
  jetrec.JetFinder = jfind

# Dump pseudojets after reco.
if 0:
  from JetRec.JetRecConf import JetDumper
  ToolSvc += JetDumper("psjdmp")
  psjdmp = ToolSvc.psjdmp
  psjdmp.ContainerName = "PseudoJetClusters"
  psjdmp.Detail = 1
  psjdmp.MaxObject = 20
  psjdmp.OutputLevel = INFO
  jetalg.Tools += [psjdmp]

# Dump jets after reco.
if 0:
  from JetRec.JetRecConf import JetDumper
  ToolSvc += JetDumper("jdmp")
  jdmp = ToolSvc.jdmp
  jdmp.ContainerName = "MyNewJets"
  jdmp.Detail = 1
  jdmp.LineDetail = 3
  jdmp.MaxObject = 20
  jdmp.OutputLevel = INFO
  jetalg.Tools += [jdmp]

# Copy jets with a new JetRecTool.
if 1:
  from JetRec.JetRecConf import JetRecTool
  ToolSvc += JetRecTool("jetrec2")
  jetrec2 = ToolSvc.jetrec2
  jetrec2.OutputContainer = "MyCopiedJets"
  jetrec2.InputContainer = clname
  jetrec2.OutputLevel = INFO
  jetalg.Tools += [jetrec2]

# Add filter for copied jets.
if 1:
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import NSubjettinessTool
  ToolSvc += NSubjettinessTool("nsubjettiness")
  nsubjettiness = ToolSvc.nsubjettiness
  jetrec2.JetModifiers += [nsubjettiness]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import NSubjettinessRatiosTool
  ToolSvc += NSubjettinessRatiosTool("nsubjettinessratios")
  nsubjettinessratios = ToolSvc.nsubjettinessratios
  jetrec2.JetModifiers += [nsubjettinessratios]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KTSplittingScaleTool
  ToolSvc += KTSplittingScaleTool("ktsplittingscale")
  ktsplittingscale = ToolSvc.ktsplittingscale
  jetrec2.JetModifiers += [ktsplittingscale]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import DipolarityTool
  ToolSvc += DipolarityTool("dipolarity")
  dipolarity = ToolSvc.dipolarity
  jetrec2.JetModifiers += [dipolarity]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import AngularityTool
  ToolSvc += AngularityTool("angularity")
  angularity = ToolSvc.angularity
  jetrec2.JetModifiers += [angularity]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtDeltaRTool
  ToolSvc += KtDeltaRTool("ktdr")
  ktdr = ToolSvc.ktdr
  jetrec2.JetModifiers += [ktdr]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtMassDropTool
  ToolSvc += KtMassDropTool("ktmassdrop")
  ktmassdrop = ToolSvc.ktmassdrop
  jetrec2.JetModifiers += [ktmassdrop]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import PlanarFlowTool
  ToolSvc += PlanarFlowTool("planarflow")
  planarflow = ToolSvc.planarflow
  jetrec2.JetModifiers += [planarflow]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import CenterOfMassShapesTool
  ToolSvc += CenterOfMassShapesTool("centerofmassshapes")
  centerofmassshapes = ToolSvc.centerofmassshapes
  jetrec2.JetModifiers += [centerofmassshapes]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import EnergyCorrelatorTool
  ToolSvc += EnergyCorrelatorTool("energycorrelator")
  energycorrelator = ToolSvc.energycorrelator
  #energycorrelator.Beta = 0.5
  #energycorrelator.IncludeBeta2 = True
  #energycorrelator.IncludeECF4 = True
  jetrec2.JetModifiers += [energycorrelator]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import EnergyCorrelatorRatiosTool
  ToolSvc += EnergyCorrelatorRatiosTool("energycorrelatorratios")
  energycorrelatorratios = ToolSvc.energycorrelatorratios
  #energycorrelatorratios.IncludeBeta2 = True
  #energycorrelatorratios.IncludeECF4 = True
  jetrec2.JetModifiers += [energycorrelatorratios]
  
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import EnergyCorrelatorGeneralizedTool
  ToolSvc += EnergyCorrelatorGeneralizedTool("energycorrelatorgeneralized")
  energycorrelatorgeneralized = ToolSvc.energycorrelatorgeneralized
  jetrec2.JetModifiers += [energycorrelatorgeneralized]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import EnergyCorrelatorGeneralizedRatiosTool
  ToolSvc += EnergyCorrelatorGeneralizedRatiosTool("energycorrelatorgeneralizedratios")
  energycorrelatorgeneralizedratios = ToolSvc.energycorrelatorgeneralizedratios
  jetrec2.JetModifiers += [energycorrelatorgeneralizedratios]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import JetPullTool
  ToolSvc += JetPullTool("pull")
  pull = ToolSvc.pull
  #pull.IncludeTensorMoments = True
  jetrec2.JetModifiers += [pull]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import JetChargeTool
  ToolSvc += JetChargeTool("charge")
  charge = ToolSvc.charge
  jetrec2.JetModifiers += [charge]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import VolatilityTool
  ToolSvc += VolatilityTool("volatility")
  volatility = ToolSvc.volatility
  volatility.TruncationFactor = 0.0
  #jetrec2.JetModifiers += [volatility]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import ShowerDeconstructionTool
  ToolSvc += ShowerDeconstructionTool("sd")
  sd = ToolSvc.sd
  jetrec2.JetModifiers += [sd]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetMakerTool
  ToolSvc += SubjetMakerTool("subjetmaker")
  subjetmaker = ToolSvc.subjetmaker
  subjetmaker.type = "Kt"
  subjetmaker.R = 0.3
  subjetmaker.PtCut = 5000;
  jetrec2.JetModifiers += [subjetmaker]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetFinderTool
  ToolSvc += SubjetFinderTool("subjetfinder")
  subjetfinder = ToolSvc.subjetfinder
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetRecorderTool
  ToolSvc += SubjetRecorderTool("subjetrecorder")
  subjetrecorder = ToolSvc.subjetrecorder
  subjetfinder.JetAlgorithm = "Kt"
  subjetfinder.JetRadius = 0.3
  subjetrecorder.SubjetLabel = "Kt3Subjets"
  subjetrecorder.SubjetContainerName = "MyCopiedJets_Kt3Subjets"
  subjetfinder.SubjetRecorder = subjetrecorder
  jetrec2.JetModifiers += [subjetfinder]
 
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import BosonTagTool
  ToolSvc += BosonTagTool("bosontag")
  bosontag = ToolSvc.bosontag
  bosontag.Debug = True
  jetrec2.JetModifiers += [bosontag]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import BoostedXbbTagTool
  ToolSvc += BoostedXbbTagTool("boostedxbbtag")
  boostedxbbtag = ToolSvc.boostedxbbtag
  boostedxbbtag.Debug = True
  jetrec2.JetModifiers += [boostedxbbtag]

# Dump copied jets.
if 1:
  from JetRec.JetRecConf import JetDumper
  ToolSvc += JetDumper("jdmp2")
  jdmp2 = ToolSvc.jdmp2
  jdmp2.ContainerName = "MyCopiedJets"
  jdmp2.Detail = 2
  jdmp2.LineDetail = 3
  jdmp2.MaxObject = 20
  jdmp2.FloatMoments = ["Tau1", "Tau2", "Tau3"]
  jdmp2.FloatMoments += ["Tau1_wta", "Tau2_wta", "Tau3_wta"]
  jdmp2.FloatMoments += ["Tau21", "Tau32"]
  jdmp2.FloatMoments += ["Tau21_wta", "Tau32_wta"]
  jdmp2.FloatMoments += ["Split12", "Split23", "Split34"]
  jdmp2.FloatMoments += ["ZCut12", "ZCut23", "ZCut34"]
  jdmp2.FloatMoments += ["Dip12", "Dip13", "Dip23", "DipExcl12"]
  jdmp2.FloatMoments += ["Angularity"]
  jdmp2.FloatMoments += ["KtDR"]
  jdmp2.FloatMoments += ["Mu12"]
  jdmp2.FloatMoments += ["PlanarFlow"]
  jdmp2.FloatMoments += ["ThrustMin", "ThrustMaj"]
  jdmp2.FloatMoments += ["FoxWolfram0", "FoxWolfram1", "FoxWolfram2", "FoxWolfram3", "FoxWolfram4"]
  jdmp2.FloatMoments += ["Sphericity", "Aplanarity"]
  jdmp2.FloatMoments += ["ECF1", "ECF2", "ECF3"]#, "ECF4"]#, "ECF1_Beta2", "ECF2_Beta2", "ECF3_Beta2", "ECF4_Beta2"]
  jdmp2.FloatMoments += ["D2", "C1", "C2", "N2", "N3", "M2", "L1", "L2", "L3", "L4", "L5"]#, "C3"]#, "D2_Beta2", "C1_Beta2", "C2_Beta2", "C3_Beta2"]
  jdmp2.FloatMoments += ["PullMag", "PullPhi"]#, "Pull_C00", "Pull_C01", "Pull_C10", "Pull_C11"]
  jdmp2.FloatMoments += ["Charge"]
  #jdmp2.FloatMoments += ["Volatility"]
  jdmp2.FloatMoments += ["ShowerDeconstructionW", "ShowerDeconstructionTop"]
  jdmp2.IntMoments += ["BosonTag"]
  jdmp2.IntMoments += ["BoostedXbbTag"]
  jdmp2.FloatVectorMoments += ["SubjetKt30_e"]
 # jdmp2.FourVectorMoments = ["ActiveArea4vec"]
  jdmp2.AssociatedParticleVectors += ["Kt3Subjets"]
  jdmp2.OutputLevel = INFO
  jetalg.Tools += [jdmp2]

  ToolSvc += JetDumper("jdmp3")
  jdmp3 = ToolSvc.jdmp3
  jdmp3.ContainerName = "MyCopiedJets_Kt3Subjets"
  jdmp3.Detail = 2
  jdmp3.LineDetail = 3
  jdmp3.MaxObject = 20
  jdmp3.OutputLevel = INFO
  jdmp3.ElementLinkMoments += ["Parent"]
  jetalg.Tools += [jdmp3]


# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamXAOD", "xAOD.root" )

# Set up its contents:
xaodStream.AddItem( "xAOD::JetContainer_v1#*" )
xaodStream.AddItem( "xAOD::JetAuxContainer_v1#*" )
xaodStream.AddMetaDataItem( "xAOD::EventFormat_v1#*" )
xaodStream.Print()

# Split all branches:
ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [
        "DEFAULT_SPLITLEVEL='99'" ]

# Force POOL to just simply use the StoreGate keys as branch names:
ServiceMgr.AthenaPoolCnvSvc.SubLevelBranchName = "<key>"
