import AthenaPoolCnvSvc.ReadAthenaPool
# svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/work/r/rjansky/InputHGTD/AOD.%" ]
# InputFiles = ["/afs/cern.ch/work/r/rjansky/InputHGTD/AOD.10041718._000096.pool.root.1",
# "/afs/cern.ch/work/r/rjansky/InputHGTD/AOD.10041718._000164.pool.root.1",
# "/afs/cern.ch/work/r/rjansky/InputHGTD/AOD.10041718._000312.pool.root.1"]

InputFiles = ["/afs/cern.ch/work/r/rjansky/mc15_13TeV.301267.Pythia8EvtGen_A14NNPDF23LO_Wprime_WZqqqq_m2000.merge.AOD.e3749_s2608_s2183_r8459_r7676/AOD.09378881._000006.pool.root.1"]

svcMgr.EventSelector.InputCollections = InputFiles
Name = "Wprime"

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections

from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetGeo = 'atlas'
#from RecExConfig.InputFilePeeker import inputFileSummary
#globalflags.DataSource = 'data' if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'
#globalflags.DetDescrVersion = inputFileSummary['geometry']
DetDescrVersion = 'ATLAS-R2-2015-03-01-00'
globalflags.DetDescrVersion = DetDescrVersion

# from AthenaCommon.DetFlags import DetFlags
# DetFlags.detdescr.BField_setOn()

# include("InDetSLHC_Example/preInclude.SLHC.py")
# include("InDetSLHC_Example/preInclude.SLHC.NoTRT.Reco.py")
# include("InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py")
# include("InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py")

#from AtlasGeoModel import SetGeometryVersion 
#from AtlasGeoModel import GeoModelInit 

#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc = GeoModelSvc()
#ServiceMgr += GeoModelSvc
#GeoModelSvc.AtlasVersion = DetDescrVersion
#print "GeoModelSvc.AtlasVersion = ", GeoModelSvc.AtlasVersion

print "globalflags.DetDescrVersion = ", globalflags.DetDescrVersion
# Set up geometry and BField
include("RecExCond/AllDet_detDescr.py")
# include('InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py')

#Configure the extrapolator
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator=AtlasExtrapolator("AtlasExtrapolator")
ToolSvc += theAtlasExtrapolator

from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import ParticleToCaloExtrapolationTool
ParticleToCaloExtrapolationTool = ParticleToCaloExtrapolationTool(name = "ParticleToCaloExtrapolationTool", 
									Extrapolator = theAtlasExtrapolator)
# ParticleToCaloExtrapolationTool.OutputLevel = DEBUG
ToolSvc += ParticleToCaloExtrapolationTool

print ParticleToCaloExtrapolationTool

from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
CaloClustersInCone = xAOD__CaloClustersInConeTool(name = "CaloClustersInCone", 
                                                  CaloClusterLocation = "CaloCalTopoClusters")
# CaloClustersInCone.OutputLevel = DEBUG
ToolSvc += CaloClustersInCone

print CaloClustersInCone

from TrackToCalo.TrackToCaloConf import Rec__ParticleCaloClusterAssociationTool
ParticleCaloCellAssociation = Rec__ParticleCaloClusterAssociationTool(name                 = "ParticleCaloCellAssociationInDet",
                                                                   ParticleCaloExtensionTool                = ParticleToCaloExtrapolationTool,
                                                                   ClustersInConeTool = CaloClustersInCone,
                                                                   ConeSize             = 0.1)
# ParticleCaloCellAssociation.OutputLevel = DEBUG
ToolSvc+=ParticleCaloCellAssociation
print      ParticleCaloCellAssociation

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TrackParticleAssociationAlgs.TrackParticleAssociationAlgsConf import TrackParticleClusterAssociationAlg
TrackParticleClusterAssociation = TrackParticleClusterAssociationAlg(name = "TrackParticleClusterAssociationInDet",
                                                            ParticleCaloClusterAssociationTool = ParticleCaloCellAssociation,
                                                            TrackParticleContainerName = "InDetTrackParticles",
                                                            PtCut = 0.4,
                                                            OutputCollectionPostFix = "Test")
# TrackParticleClusterAssociation.OutputLevel = DEBUG
topSequence += TrackParticleClusterAssociation

print TrackParticleClusterAssociation

from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import TrackCaloClusterWeightsTool
TrackCaloClusterWeights = TrackCaloClusterWeightsTool(name                 = "TrackCaloClusterWeights")
# TrackCaloClusterWeights.OutputLevel = VERBOSE
ToolSvc+=TrackCaloClusterWeights
print      TrackCaloClusterWeights

from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import TrackCaloClusterCreatorTool
TrackCaloClusterCreator = TrackCaloClusterCreatorTool(name                 = "TrackCaloClusterCreator")
# TrackCaloClusterCreator.OutputLevel = VERBOSE
ToolSvc+=TrackCaloClusterCreator
print      TrackCaloClusterCreator

from TrackCaloClusterRecAlgs.TrackCaloClusterRecAlgsConf import TrackCaloClusterRecAlg
ParticleToCaloExtrapolation = TrackCaloClusterRecAlg(name                           = "TrackCaloClusterRecAlg",
                                                     OutputCollectionPostFix        = "Test",
                                                     CaloClustersLocation           = "CaloCalTopoClusters",
                                                     TrackCaloClusterContainerName  = "TrackCaloClusters",
                                                     TrackCaloClusterWeightsTool    = TrackCaloClusterWeights,
                                                     TrackCaloClusterCreatorTool    = TrackCaloClusterCreator)
# ParticleToCaloExtrapolation.OutputLevel = VERBOSE
topSequence += ParticleToCaloExtrapolation

print ParticleToCaloExtrapolation

theApp.EvtMax = -1

### Jet Stuff

clname = "TrackCaloClusters"
clname1 = "TrackCaloClustersCharged"
clname2 = "TrackCaloClustersAll"

# Add the algorithm. It runs the demo tools.
from JetRec.JetRecConf import JetAlgorithm
topSequence += JetAlgorithm("jetalg")
jetalg = topSequence.jetalg
jetalg.OutputLevel = INFO

#--------------------------------------------------------------
# Configure tools.
#--------------------------------------------------------------

# JetRec tool for finding.
if 1:
  from JetRec.JetRecConf import JetRecTool
  ToolSvc += JetRecTool("jetrec")
  jetrec = ToolSvc.jetrec
  jetrec.OutputContainer = "AntiKt10TrackCaloClusterJets"
  jetrec.OutputLevel = INFO
  jetalg.Tools += [jetrec]

# Find jet inputs.
if 1:
  from JetRecTools.JetRecToolsConf import TCCPseudoJetGetter
  ToolSvc += TCCPseudoJetGetter("psjget")
  psjget = ToolSvc.psjget
  psjget.InputContainer = clname
  psjget.Label = "TrackCaloCluster"
  psjget.OutputContainer = "PseudoJetClusters"
  psjget.OutputLevel = INFO
  psjget.SkipNegativeEnergy = True
  jetrec.PseudoJetGetters += [psjget]

# Find jets.
if 1:
  from JetRec.JetRecConf import JetFinder
  ToolSvc += JetFinder("jfind")
  jfind = ToolSvc.jfind
  jfind.JetAlgorithm = "AntiKt"
  jfind.JetRadius = 1.0
  jfind.GhostArea = 0.01;
  jfind.PtMin = 2000.0
  jfind.OutputLevel = VERBOSE
  jetrec.JetFinder = jfind
  
# Add filter for copied jets.
if 1:
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import NSubjettinessTool
  ToolSvc += NSubjettinessTool("nsubjettiness")
  nsubjettiness = ToolSvc.nsubjettiness
  jetrec.JetModifiers += [nsubjettiness]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import NSubjettinessRatiosTool
  ToolSvc += NSubjettinessRatiosTool("nsubjettinessratios")
  nsubjettinessratios = ToolSvc.nsubjettinessratios
  jetrec.JetModifiers += [nsubjettinessratios]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KTSplittingScaleTool
  ToolSvc += KTSplittingScaleTool("ktsplittingscale")
  ktsplittingscale = ToolSvc.ktsplittingscale
  jetrec.JetModifiers += [ktsplittingscale]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import DipolarityTool
  ToolSvc += DipolarityTool("dipolarity")
  dipolarity = ToolSvc.dipolarity
  jetrec.JetModifiers += [dipolarity]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import AngularityTool
  ToolSvc += AngularityTool("angularity")
  angularity = ToolSvc.angularity
  jetrec.JetModifiers += [angularity]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtDeltaRTool
  ToolSvc += KtDeltaRTool("ktdr")
  ktdr = ToolSvc.ktdr
  jetrec.JetModifiers += [ktdr]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtMassDropTool
  ToolSvc += KtMassDropTool("ktmassdrop")
  ktmassdrop = ToolSvc.ktmassdrop
  jetrec.JetModifiers += [ktmassdrop]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import PlanarFlowTool
  ToolSvc += PlanarFlowTool("planarflow")
  planarflow = ToolSvc.planarflow
  jetrec.JetModifiers += [planarflow]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import CenterOfMassShapesTool
  ToolSvc += CenterOfMassShapesTool("centerofmassshapes")
  centerofmassshapes = ToolSvc.centerofmassshapes
  jetrec.JetModifiers += [centerofmassshapes]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import EnergyCorrelatorTool
  ToolSvc += EnergyCorrelatorTool("energycorrelator")
  energycorrelator = ToolSvc.energycorrelator
  #energycorrelator.Beta = 0.5
  #energycorrelator.IncludeBeta2 = True
  #energycorrelator.IncludeECF4 = True
  jetrec.JetModifiers += [energycorrelator]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import EnergyCorrelatorRatiosTool
  ToolSvc += EnergyCorrelatorRatiosTool("energycorrelatorratios")
  energycorrelatorratios = ToolSvc.energycorrelatorratios
  #energycorrelatorratios.IncludeBeta2 = True
  #energycorrelatorratios.IncludeECF4 = True
  jetrec.JetModifiers += [energycorrelatorratios]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import JetPullTool
  ToolSvc += JetPullTool("pull")
  pull = ToolSvc.pull
  #pull.IncludeTensorMoments = True
  jetrec.JetModifiers += [pull]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import JetChargeTool
  ToolSvc += JetChargeTool("charge")
  charge = ToolSvc.charge
  jetrec.JetModifiers += [charge]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import VolatilityTool
  ToolSvc += VolatilityTool("volatility")
  volatility = ToolSvc.volatility
  volatility.TruncationFactor = 0.0
  #jetrec.JetModifiers += [volatility]

  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import ShowerDeconstructionTool
  # ToolSvc += ShowerDeconstructionTool("sd")
  # sd = ToolSvc.sd
  # jetrec.JetModifiers += [sd]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetMakerTool
  ToolSvc += SubjetMakerTool("subjetmaker")
  subjetmaker = ToolSvc.subjetmaker
  subjetmaker.type = "Kt"
  subjetmaker.R = 0.2
  subjetmaker.PtCut = 5000;
  jetrec.JetModifiers += [subjetmaker]

  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetFinderTool
  ToolSvc += SubjetFinderTool("subjetfinder")
  subjetfinder = ToolSvc.subjetfinder
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetRecorderTool
  ToolSvc += SubjetRecorderTool("subjetrecorder")
  subjetrecorder = ToolSvc.subjetrecorder
  subjetfinder.JetAlgorithm = "Kt"
  subjetfinder.JetRadius = 0.2
  subjetrecorder.SubjetLabel = "Kt2Subjets"
  subjetrecorder.SubjetContainerName = "Kt2TrackCaloClusterSubJets"
  subjetfinder.SubjetRecorder = subjetrecorder
  jetrec.JetModifiers += [subjetfinder]
 
   
###end jet stuff

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "XAOD_"+Name+".pool.root" )
xaodStream.Stream.TakeItemsFromInput = True #this will only work for the event-by-event items. MetadataItems must still be specified
xaodStream.AddItem( "xAOD::TrackCaloClusterContainer#*")
xaodStream.AddItem( "xAOD::TrackCaloClusterAuxContainer#*")
xaodStream.AddItem( "xAOD::JetContainer#*")
xaodStream.AddItem( "xAOD::JetAuxContainer#*")

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True

# include("InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py")
