import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaCommon import CfgMgr

##InputFiles = ["/eos/user/r/rjansky/TCCTestFiles/mc15_13TeV.301282.Pythia8EvtGen_A14NNPDF23LO_Wprime_WZqqqq_m4000.merge.AOD.e3743_s2608_s2183_r7772_r7676/AOD.08110078._000001.pool.root.1"]
##InputFiles = ["root://eosatlas//eos/atlas/user/n/ncalace/Samples_Physics/WPrime_4000/DAOD_JETM8.09331772._000001.pool.root.1"]
#InputFiles   = ["/eos/atlas/user/r/rjansky/mc15_13TeV.301282.Pythia8EvtGen_A14NNPDF23LO_Wprime_WZqqqq_m4000.merge.AOD.e3743_s2608_s2183_r7772_r7676/AOD.08110078._000001.pool.root.1"]
InputFiles   = ["/afs/cern.ch/user/n/ncalace/work/public/mc15_13TeV/AOD.08110078._000001.pool.root.1"]

svcMgr.EventSelector.InputCollections = InputFiles

# Name = Sample
Name = "WprimeCovMatch_AllTests_TCC"

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections

from AthenaCommon.GlobalFlags import globalflags
DetDescrVersion = 'ATLAS-R2-2015-03-01-00'
globalflags.DetDescrVersion = DetDescrVersion

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-MC15c-SDR-09')

print "globalflags.DetDescrVersion = ", globalflags.DetDescrVersion
# Set up geometry and BField
include("RecExCond/AllDet_detDescr.py")

from JetRec.JetRecConf import JetAlgorithm
from JetRecTools.JetRecToolsConf import JetConstituentModSequence, CaloClusterConstituentsOrigin, ClusterTimeCutTool
ccco = CaloClusterConstituentsOrigin("JetConstit_LCOrigin") 
ToolSvc += ccco

ctct = ClusterTimeCutTool("JetConstit_Timecut") 
ToolSvc += ctct

PFSequence = JetConstituentModSequence("JetConstitSeq_LCOriginAndTime",
                                       InputContainer = "CaloCalTopoClusters",
                                       OutputContainer = "TimedCaloCalTopoClusters",
                                       InputType = "CaloCluster",
                                       Modifiers = [ctct, ccco],
                                       SaveAsShallow = True)
ToolSvc += PFSequence

#Configure the extrapolator
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator=AtlasExtrapolator("AtlasExtrapolator")
ToolSvc += theAtlasExtrapolator

from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import ParticleToCaloExtrapolationTool
ParticleToCaloExtrapolationTool = ParticleToCaloExtrapolationTool(name = "ParticleToCaloExtrapolationTool", 
								  Extrapolator = theAtlasExtrapolator, 
                                                                  ParticleType = "pion",
                                                                  StoreParameters=True)
# ParticleToCaloExtrapolationTool.OutputLevel = DEBUG
ToolSvc += ParticleToCaloExtrapolationTool

print ParticleToCaloExtrapolationTool

from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
CaloClustersInCone = xAOD__CaloClustersInConeTool(name = "CaloClustersInCone", 
                                                  CaloClusterLocation = "TimedCaloCalTopoClusters")
# CaloClustersInCone.OutputLevel = DEBUG
ToolSvc += CaloClustersInCone

print CaloClustersInCone

from TrackToCalo.TrackToCaloConf import Rec__ParticleCaloClusterAssociationTool
ParticleCaloCellAssociation = Rec__ParticleCaloClusterAssociationTool(name                      = "ParticleCaloCellAssociationInDet",
                                                                   ParticleCaloExtensionTool    = ParticleToCaloExtrapolationTool,
                                                                   CaloClusterLocation          = "TimedCaloCalTopoClusters",
                                                                   ClustersInConeTool           = CaloClustersInCone,
                                                                   ConeSize                     = 0.1,
                                                                   UseCovariance                = True,
                                                                   StoreParameters              = True)
#ParticleCaloCellAssociation.OutputLevel = DEBUG
ToolSvc+=ParticleCaloCellAssociation
print      ParticleCaloCellAssociation

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Add the algorithm.
jetalg = JetAlgorithm("JetAlg_OriginAndTime")
#jetalg.OutputLevel = INFO
jetalg.Tools += [PFSequence]
topSequence += jetalg

from TrackParticleAssociationAlgs.TrackParticleAssociationAlgsConf import TrackParticleClusterAssociationAlg
TrackParticleClusterAssociation = TrackParticleClusterAssociationAlg(name = "TrackParticleClusterAssociationInDet",
                                                                     ParticleCaloClusterAssociationTool = ParticleCaloCellAssociation,
                                                                     TrackParticleContainerName = "InDetTrackParticles",
                                                                     PtCut = 400.,
                                                                     OutputCollectionPostFix = "Test",
                                                                     CaloClusterLocation = "TimedCaloCalTopoClusters")
#TrackParticleClusterAssociation.OutputLevel = DEBUG
topSequence += TrackParticleClusterAssociation

print TrackParticleClusterAssociation

from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import TrackCaloClusterWeightsTool
TrackCaloClusterWeights = TrackCaloClusterWeightsTool(name                 = "TrackCaloClusterWeights")
# TrackCaloClusterWeights.OutputLevel = VERBOSE
ToolSvc+=TrackCaloClusterWeights
print      TrackCaloClusterWeights

loosetrackvertexassotool=CfgMgr.CP__LooseTrackVertexAssociationTool("LooseTrackVertexAssociationTool", dzSinTheta_cut=3, d0_cut=2) 
ToolSvc+=loosetrackvertexassotool 

from JetRecTools.JetRecToolsConf import TrackVertexAssociationTool
jettva  =   TrackVertexAssociationTool( name                    = "tvassoc",
                                        TrackParticleContainer  = "InDetTrackParticles",
                                        TrackVertexAssociation  = "JetTrackVtxAssoc",
                                        VertexContainer         = "PrimaryVertices",
                                        TrackVertexAssoTool     = loosetrackvertexassotool
                                        )
ToolSvc+=jettva 
print      jettva

from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import ClusterFilterTool
clusterfiltertool = ClusterFilterTool(name                       = "clusterfiltertool",
				      LooseTrackVertexAssoTool   = loosetrackvertexassotool,
				      TrackParticleContainerName = "InDetTrackParticles",
				      ConeSize                   = 0.2,
				      PtFractionAtPV0            = 0.1,
				      StoreParameters            = True)

ToolSvc+=clusterfiltertool
#clusterfiltertool.OutputLevel = VERBOSE
print clusterfiltertool

from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import TrackCaloClusterCreatorTool
TrackCaloClusterCreator = TrackCaloClusterCreatorTool(name                      = "TrackCaloClusterCreator",
                                                      VertexContainerName       = "PrimaryVertices",
                                                      LooseTrackVertexAssoTool  = loosetrackvertexassotool,
                                                      ApplyClusterFilter        = False,
                                                      ClusterFilterTool         = clusterfiltertool)
#TrackCaloClusterCreator.OutputLevel = VERBOSE
ToolSvc+=TrackCaloClusterCreator
print      TrackCaloClusterCreator

from TrackCaloClusterRecAlgs.TrackCaloClusterRecAlgsConf import TrackCaloClusterRecAlg
ParticleToCaloExtrapolation = TrackCaloClusterRecAlg(name                           = "TrackCaloClusterRecAlg",
                                                     OutputCollectionPostFix        = "Test",
                                                     CaloClustersLocation           = "TimedCaloCalTopoClusters",
                                                     TrackCaloClusterContainerName  = "TrackCaloClusters",
                                                     TrackCaloClusterWeightsTool    = TrackCaloClusterWeights,
                                                     TrackCaloClusterCreatorTool    = TrackCaloClusterCreator,
                                                     StoreStatistics                = True )
#ParticleToCaloExtrapolation.OutputLevel = VERBOSE
topSequence += ParticleToCaloExtrapolation

print ParticleToCaloExtrapolation

theApp.EvtMax = 1000

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "XAOD_"+Name+".pool.root" )
xaodStream.AddItem( "xAOD::CaloClusterContainer#TimedCaloCalTopoClusters")
xaodStream.AddItem( "xAOD::ShallowAuxContainer#TimedCaloCalTopoClustersAux.")
xaodStream.AddItem( "xAOD::TrackParticleContainer#InDetTrackParticles")
xaodStream.AddItem( "xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.")
xaodStream.AddItem( "xAOD::TrackCaloClusterContainer#*")
xaodStream.AddItem( "xAOD::TrackCaloClusterAuxContainer#*")
xaodStream.AddItem( "xAOD::TruthParticleContainer#TruthParticles")
xaodStream.AddItem( "xAOD::TruthParticleAuxContainer#TruthParticlesAux.")

ServiceMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"
#ServiceMgr.StoreGateSvc.Dump=True
