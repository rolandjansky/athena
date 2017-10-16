import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaCommon import CfgMgr

InputFiles = ["/eos/user/r/rjansky/TCCTestFiles/mc15_13TeV.301282.Pythia8EvtGen_A14NNPDF23LO_Wprime_WZqqqq_m4000.merge.AOD.e3743_s2608_s2183_r7772_r7676/AOD.08110078._000001.pool.root.1"]

svcMgr.EventSelector.InputCollections = InputFiles

# Name = Sample
Name = "WprimeCovMatch"

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
#from JetRecTools.JetRecToolsConf import JetConstituentModSequence, CaloClusterConstituentsOrigin, ClusterTimeCutTool
from JetRecTools.JetRecToolsConf import JetConstituentModSequence, CaloClusterConstituentsOrigin
ccco = CaloClusterConstituentsOrigin("JetConstit_LCOrigin") 
ToolSvc += ccco

#ctct = ClusterTimeCutTool("JetConstit_Timecut") 
#ToolSvc += ctct

PFSequence = JetConstituentModSequence("JetConstitSeq_LCOriginAndTime",
                                       InputContainer = "CaloCalTopoClusters",
                                       OutputContainer = "TimedCaloCalTopoClusters",
                                       InputType = "CaloCluster",
                                       Modifiers = [ccco],
                                       SaveAsShallow = False
                                       )
ToolSvc += PFSequence

#Configure the extrapolator
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator=AtlasExtrapolator("AtlasExtrapolator")
ToolSvc += theAtlasExtrapolator

from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import ParticleToCaloExtrapolationTool
ParticleToCaloExtrapolationTool = ParticleToCaloExtrapolationTool(name = "ParticleToCaloExtrapolationTool", 
                                                                  Extrapolator = theAtlasExtrapolator,
                                                                  ParticleType = "pion" )
#ParticleToCaloExtrapolationTool.OutputLevel = DEBUG
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
                                                                   UseCovariance                = True )
ParticleCaloCellAssociation.OutputLevel = DEBUG
ToolSvc+=ParticleCaloCellAssociation
print      ParticleCaloCellAssociation

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Add the algorithm.
jetalg = JetAlgorithm("JetAlg_OriginAndTime")
jetalg.OutputLevel = INFO
jetalg.Tools += [PFSequence]
topSequence += jetalg

from TrackParticleAssociationAlgs.TrackParticleAssociationAlgsConf import TrackParticleClusterAssociationAlg
TrackParticleClusterAssociation = TrackParticleClusterAssociationAlg(name = "TrackParticleClusterAssociationInDet",
                                                            ParticleCaloClusterAssociationTool = ParticleCaloCellAssociation,
                                                            TrackParticleContainerName = "InDetTrackParticles",
                                                            PtCut = 400.,
                                                            OutputCollectionPostFix = "Test",
                                                            CaloClusterLocation = "TimedCaloCalTopoClusters")
# TrackParticleClusterAssociation.OutputLevel = DEBUG
topSequence += TrackParticleClusterAssociation

print TrackParticleClusterAssociation

from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import TrackCaloClusterWeightsTool
TrackCaloClusterWeights = TrackCaloClusterWeightsTool(name                 = "TrackCaloClusterWeights"  )
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

#from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import ClusterFilterTool
#clusterfiltertool = ClusterFilterTool(name                       = "clusterfiltertool",
#                                      LooseTrackVertexAssoTool   = loosetrackvertexassotool,
#                                      TrackParticleContainerName = "InDetTrackParticles",
#                                      ConeSize                   = 0.2,
#                                      StoreParameters            = False)

#ToolSvc+=clusterfiltertool
#clusterfiltertool.OutputLevel = VERBOSE
#print clusterfiltertool

from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import TrackCaloClusterCreatorTool
TrackCaloClusterCreator = TrackCaloClusterCreatorTool(name                      = "TrackCaloClusterCreator",
                                                      VertexContainerName       = "PrimaryVertices",
                                                      LooseTrackVertexAssoTool  = loosetrackvertexassotool,
                                                      ApplyClusterFilter        = False  )
# TrackCaloClusterCreator.OutputLevel = VERBOSE
ToolSvc+=TrackCaloClusterCreator
print      TrackCaloClusterCreator

from TrackCaloClusterRecAlgs.TrackCaloClusterRecAlgsConf import TrackCaloClusterRecAlg
ParticleToCaloExtrapolation = TrackCaloClusterRecAlg(name                           = "TrackCaloClusterRecAlg",
                                                     OutputCollectionPostFix        = "Test",
                                                     CaloClustersLocation           = "TimedCaloCalTopoClusters",
                                                     TrackCaloClusterContainerName  = "TrackCaloClusters",
                                                     TrackCaloClusterWeightsTool    = TrackCaloClusterWeights,
                                                     TrackCaloClusterCreatorTool    = TrackCaloClusterCreator)
# ParticleToCaloExtrapolation.OutputLevel = VERBOSE
topSequence += ParticleToCaloExtrapolation

print ParticleToCaloExtrapolation

theApp.EvtMax = -1

### Jet Stuff
if 1:
  from JetRec.JetRecConf import JetFromPseudojet
  jetFromPseudojet = JetFromPseudojet("jetFromPseudojet",Attributes = [] )
  ToolSvc += jetFromPseudojet
  
  from JetRec.JetRecConf import JetTrimmer
  groomer = JetTrimmer("JetGroomer")
  groomer.RClus = 0.2  
  groomer.PtFrac = 0.05
  groomer.JetBuilder = jetFromPseudojet
  ToolSvc += groomer

  # Add filter for copied jets.
  #from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import NSubjettinessTool
  #ToolSvc += NSubjettinessTool("nsubjettiness")
  #nsubjettiness = ToolSvc.nsubjettiness
  
  #from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import NSubjettinessRatiosTool
  #ToolSvc += NSubjettinessRatiosTool("nsubjettinessratios")
  #nsubjettinessratios = ToolSvc.nsubjettinessratios
  
  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KTSplittingScaleTool
  # ToolSvc += KTSplittingScaleTool("ktsplittingscale")
  # ktsplittingscale = ToolSvc.ktsplittingscale
  
  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import DipolarityTool
  # ToolSvc += DipolarityTool("dipolarity")
  # dipolarity = ToolSvc.dipolarity
  
  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import AngularityTool
  # ToolSvc += AngularityTool("angularity")
  # angularity = ToolSvc.angularity
  
  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtDeltaRTool
  # ToolSvc += KtDeltaRTool("ktdr")
  # ktdr = ToolSvc.ktdr
  
  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtMassDropTool
  # ToolSvc += KtMassDropTool("ktmassdrop")
  # ktmassdrop = ToolSvc.ktmassdrop
  
  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import PlanarFlowTool
  # ToolSvc += PlanarFlowTool("planarflow")
  # planarflow = ToolSvc.planarflow
  
  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import CenterOfMassShapesTool
  # ToolSvc += CenterOfMassShapesTool("centerofmassshapes")
  # centerofmassshapes = ToolSvc.centerofmassshapes
  
  #from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import EnergyCorrelatorTool
  #ToolSvc += EnergyCorrelatorTool("energycorrelator")
  #energycorrelator = ToolSvc.energycorrelator
  
  #from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import EnergyCorrelatorRatiosTool
  #ToolSvc += EnergyCorrelatorRatiosTool("energycorrelatorratios")
  #energycorrelatorratios = ToolSvc.energycorrelatorratios
  
  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import JetPullTool
  # ToolSvc += JetPullTool("pull")
  # pull = ToolSvc.pull
  
  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import JetChargeTool
  # ToolSvc += JetChargeTool("charge")
  # charge = ToolSvc.charge
  
  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import VolatilityTool
  # ToolSvc += VolatilityTool("volatility")
  # volatility = ToolSvc.volatility
  # volatility.TruncationFactor = 0.0
  
  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetMakerTool
  # ToolSvc += SubjetMakerTool("subjetmaker")
  # subjetmaker = ToolSvc.subjetmaker
  # subjetmaker.type = "Kt"
  # subjetmaker.R = 0.2
  # subjetmaker.PtCut = 5000;
  
  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetFinderTool
  # ToolSvc += SubjetFinderTool("subjetfinder")
  # subjetfinder = ToolSvc.subjetfinder
  # from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import SubjetRecorderTool
  # ToolSvc += SubjetRecorderTool("subjetrecorder")
  # subjetrecorder = ToolSvc.subjetrecorder
  # subjetfinder.JetAlgorithm = "Kt"
  # subjetfinder.JetRadius = 0.2
  # subjetrecorder.SubjetLabel = "Kt2Subjets"
  # subjetrecorder.SubjetContainerName = "Kt2TrackCaloClusterSubJets"
  # subjetfinder.SubjetRecorder = subjetrecorder
  
  clname  = "TrackCaloClustersCombined"
#  clname1 = "TrackCaloClustersCombinedAndNeutral"
#  clname2 = "TrackCaloClustersCombinedAndCharged"
  clname3 = "TrackCaloClustersAll"
  
#  clnames = [clname, clname1, clname2, clname3]
  clnames = [clname, clname3]
  
  from JetRec.JetRecConf import JetPseudojetRetriever
  from JetRec.JetRecConf import JetRecTool

  for name in clnames:
  #--------------------------------------------------------------
  # Configure tools.
  #--------------------------------------------------------------
      
    # Find jet inputs.
    from JetRecTools.JetRecToolsConf import TCCPseudoJetGetter
    psjget =  TCCPseudoJetGetter("TCCPseudoJetGetter_"+name)
    psjget.InputContainer = name
    psjget.Label = "TrackCaloCluster"
    psjget.OutputContainer = "PseudoJetClusters"+name
    # psjget.OutputLevel = INFO
    psjget.SkipNegativeEnergy = True
    ToolSvc += psjget
    
    # Find jets.
    from JetRec.JetRecConf import JetFinder
    jfind = JetFinder("JetFinder_"+name)
    jfind.JetAlgorithm = "AntiKt"
    jfind.JetRadius = 1.0
    jfind.GhostArea = 0.01;
    jfind.PtMin = 200000.0
  #  jfind.OutputLevel = VERBOSE
    ToolSvc += jfind
      
    # JetRec tool for finding.
    jetrec = JetRecTool("JetRecTool_"+name)
    jetrec.OutputContainer = "AntiKt10"+name+"Jets"
    # jetrec.OutputLevel = INFO
    jetrec.PseudoJetGetters += [psjget]
    jetrec.JetFinder = jfind  
    #jetrec.JetModifiers += [nsubjettiness]
    #jetrec.JetModifiers += [nsubjettinessratios]
    # jetrec.JetModifiers += [ktsplittingscale]
    # jetrec.JetModifiers += [dipolarity]
    # jetrec.JetModifiers += [angularity]
    # jetrec.JetModifiers += [ktdr]
    # jetrec.JetModifiers += [ktmassdrop]
    # jetrec.JetModifiers += [planarflow]
    # jetrec.JetModifiers += [centerofmassshapes]
    #jetrec.JetModifiers += [energycorrelator]
    #jetrec.JetModifiers += [energycorrelatorratios]
    # jetrec.JetModifiers += [pull]
    # jetrec.JetModifiers += [charge]
    # jetrec.JetModifiers += [subjetmaker]
    # jetrec.JetModifiers += [subjetfinder]
    ToolSvc += jetrec
    
    jetPseudojetRetriever = JetPseudojetRetriever("JetPseudojetRetriever_"+name)
    ToolSvc += jetPseudojetRetriever
    
    # JetRec tool for finding.    
    jetrec_trimm = JetRecTool("JetRecTool_"+name+"Trimm")
    jetrec_trimm.JetGroomer = groomer
    jetrec_trimm.InputContainer  = "AntiKt10"+name+"Jets"
    jetrec_trimm.OutputContainer = "AntiKt10"+name+"TrimmedJets"
    jetrec_trimm.JetPseudojetRetriever = jetPseudojetRetriever
    #jetrec_trimm.JetModifiers += [nsubjettiness]
    #jetrec_trimm.JetModifiers += [nsubjettinessratios]
    # jetrec_trimm.JetModifiers += [ktsplittingscale]
    # jetrec_trimm.JetModifiers += [dipolarity]
    # jetrec_trimm.JetModifiers += [angularity]
    # jetrec_trimm.JetModifiers += [ktdr]
    # jetrec_trimm.JetModifiers += [ktmassdrop]
    # jetrec_trimm.JetModifiers += [planarflow]
    # jetrec_trimm.JetModifiers += [centerofmassshapes]
    #jetrec_trimm.JetModifiers += [energycorrelator]
    #jetrec_trimm.JetModifiers += [energycorrelatorratios]
    # jetrec_trimm.JetModifiers += [pull]
    # jetrec_trimm.JetModifiers += [charge]
    # jetrec_trimm.JetModifiers += [subjetmaker]
    # jetrec_trimm.JetModifiers += [subjetfinder]
    ToolSvc += jetrec_trimm
      
    # Add the algorithm. It runs the demo tools.
    jetalg = JetAlgorithm("JetAlg_"+name)
    # jetalg.OutputLevel = INFO
    jetalg.Tools += [jetrec, jetrec_trimm]
    topSequence += jetalg
    
  # Find jet inputs.
  from JetRec.JetRecConf import PseudoJetGetter
  psjget = PseudoJetGetter("PseudoJetGetter")
  psjget.InputContainer = "TimedCaloCalTopoClusters"
  psjget.Label = "LCTopoOrigin"
  psjget.OutputContainer = "PseudoJetLCTopoOrigin"
  # psjget.OutputLevel = INFO
  psjget.SkipNegativeEnergy = True
  ToolSvc += psjget
  
  psjget2 = PseudoJetGetter("cget_ghost")
  psjget2.InputContainer = "InDetTrackParticles"
  psjget2.Label = "GhostTrack"
  psjget2.OutputContainer = "PseudoJetGhostTrack"
  # psjget.OutputLevel = INFO
  psjget2.GhostScale = 1e-40
  ToolSvc += psjget2
    
  # Find jets.
  from JetRec.JetRecConf import JetFinder
  jfind = JetFinder("JetFinder")
  jfind.JetAlgorithm = "AntiKt"
  jfind.JetRadius = 1.0
  jfind.GhostArea = 0.01;
  jfind.PtMin = 200000.0
  #  jfind.OutputLevel = VERBOSE
  ToolSvc += jfind
   
  from JetMomentTools.JetMomentToolsConf import JetTrackSumMomentsTool
  TrackSumMomentsTool = JetTrackSumMomentsTool(  name = "trksummoms",
                                                 VertexContainer = "PrimaryVertices",
                                                 AssociatedTracks = "GhostTrack",
                                                 TrackVertexAssociation = jettva.TrackVertexAssociation,
                                                 RequireTrackPV = True)
  # TrackSumMomentsTool.OutputLevel = DEBUG                                               
  ToolSvc += TrackSumMomentsTool   

  # JetRec tool for finding.
  jetrec = JetRecTool("JetRecTool")
  jetrec.OutputContainer = "MyAntiKt10LCTopoJets"
  # jetrec.OutputLevel = INFO
  jetrec.PseudoJetGetters += [psjget, psjget2]
  jetrec.JetFinder = jfind  
  #jetrec.JetModifiers += [nsubjettiness]
  #jetrec.JetModifiers += [nsubjettinessratios]
  # jetrec.JetModifiers += [ktsplittingscale]
  # jetrec.JetModifiers += [dipolarity]
  # jetrec.JetModifiers += [angularity]
  # jetrec.JetModifiers += [ktdr]
  # jetrec.JetModifiers += [ktmassdrop]
  # jetrec.JetModifiers += [planarflow]
  # jetrec.JetModifiers += [centerofmassshapes]
  #jetrec.JetModifiers += [energycorrelator]
  #jetrec.JetModifiers += [energycorrelatorratios]
  # jetrec.JetModifiers += [pull]
  # jetrec.JetModifiers += [charge]
  # jetrec.JetModifiers += [subjetmaker]
  # jetrec.JetModifiers += [subjetfinder]
  # jetrec.JetModifiers += [TrackSumMomentsTool]
  ToolSvc += jetrec
 
  from JetCalibTools.JetCalibToolsConf import JetCalibrationTool
  calib_tool=JetCalibrationTool('JetCalibTool',JetCollection="AntiKt10LCTopoTrimmedPtFrac5SmallR20",ConfigFile='JES_MC15recommendation_FatJet_Nov2016_QCDCombinationUncorrelatedWeights.config',CalibSequence='EtaJES_JMS',IsData=False)
  ToolSvc += calib_tool
  print calib_tool
    
  jetPseudojetRetriever = JetPseudojetRetriever("JetPseudojetRetriever")
  ToolSvc += jetPseudojetRetriever
  
  # JetRec tool for finding.
  jetrec_trimm = JetRecTool("JetRecToolTrimm")
  jetrec_trimm.JetGroomer = groomer
  jetrec_trimm.InputContainer        = "MyAntiKt10LCTopoJets"
  jetrec_trimm.OutputContainer       = "MyAntiKt10LCTopoTrimmedJets"
  jetrec_trimm.JetPseudojetRetriever = jetPseudojetRetriever
  #jetrec_trimm.JetModifiers += [nsubjettiness]
  #jetrec_trimm.JetModifiers += [nsubjettinessratios]
  # jetrec_trimm.JetModifiers += [ktsplittingscale]
  # jetrec_trimm.JetModifiers += [dipolarity]
  # jetrec_trimm.JetModifiers += [angularity]
  # jetrec_trimm.JetModifiers += [ktdr]
  # jetrec_trimm.JetModifiers += [ktmassdrop]
  # jetrec_trimm.JetModifiers += [planarflow]
  # jetrec_trimm.JetModifiers += [centerofmassshapes]
  #jetrec_trimm.JetModifiers += [energycorrelator]
  #jetrec_trimm.JetModifiers += [energycorrelatorratios]
  # jetrec_trimm.JetModifiers += [pull]
  # jetrec_trimm.JetModifiers += [charge]
  # jetrec_trimm.JetModifiers += [subjetmaker]
  # jetrec_trimm.JetModifiers += [subjetfinder]
  jetrec_trimm.JetModifiers += [TrackSumMomentsTool]
  jetrec_trimm.JetModifiers += [calib_tool]
  ToolSvc += jetrec_trimm

     
  # Add the algorithm. It runs the demo tools.
  jetalg = JetAlgorithm("JetAlg")
  # jetalg.OutputLevel = INFO
  jetalg.Tools += [jetrec, jettva, jetrec_trimm]
  topSequence += jetalg

  from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
  truth_classifier_name='JetMCTruthClassifier'
  truthClassifier = MCTruthClassifier(name = truth_classifier_name,
                                           ParticleCaloExtensionTool="")
  ToolSvc += truthClassifier

  from ParticleJetTools.ParticleJetToolsConf import CopyTruthJetParticles
  copytruth = CopyTruthJetParticles(name = "truthpartcopy", OutputName="JetInputTruthParticles",
                                 MCTruthClassifier=truthClassifier)
  ToolSvc += copytruth
  
  # Find jet inputs.
  from JetRec.JetRecConf import PseudoJetGetter
  psjget = PseudoJetGetter("PseudoTruthJetGetter")
  psjget.InputContainer = "JetInputTruthParticles"
  psjget.Label = "Truth"
  psjget.OutputContainer = "PseudoJetTruth"
  # psjget.OutputLevel = INFO
  psjget.GhostScale = 0.0
  psjget.SkipNegativeEnergy = True
  ToolSvc += psjget

  # Find jets.
  jfind = JetFinder("JetFinderTruth")
  jfind.JetAlgorithm = "AntiKt"
  jfind.JetRadius = 1.0
  jfind.GhostArea = 0.0;
  jfind.PtMin = 150000.0
  #  jfind.OutputLevel = VERBOSE
  ToolSvc += jfind
   
  # JetRec tool for finding.
  jetrec = JetRecTool("JetRecToolTruth")
  jetrec.OutputContainer = "MyAntiKt10TruthJets"
  # jetrec.OutputLevel = INFO
  jetrec.PseudoJetGetters += [psjget]
  jetrec.JetFinder = jfind  
  #jetrec.JetModifiers += [nsubjettiness]
  #jetrec.JetModifiers += [nsubjettinessratios]
  # jetrec.JetModifiers += [ktsplittingscale]
  # jetrec.JetModifiers += [dipolarity]
  # jetrec.JetModifiers += [angularity]
  # jetrec.JetModifiers += [ktdr]
  # jetrec.JetModifiers += [ktmassdrop]
  # jetrec.JetModifiers += [planarflow]
  # jetrec.JetModifiers += [centerofmassshapes]
  #jetrec.JetModifiers += [energycorrelator]
  #jetrec.JetModifiers += [energycorrelatorratios]
  # jetrec.JetModifiers += [pull]
  # jetrec.JetModifiers += [charge]
  # jetrec.JetModifiers += [subjetmaker]
  # jetrec.JetModifiers += [subjetfinder]
  # jetrec.JetModifiers += [TrackSumMomentsTool]
  ToolSvc += jetrec
 
  # JetRec tool for finding.
  jetrec_trimm = JetRecTool("JetRecToolTruthTrimm")
  jetrec_trimm.JetGroomer = groomer
  jetrec_trimm.InputContainer        = "MyAntiKt10TruthJets"
  jetrec_trimm.OutputContainer       = "MyAntiKt10TruthTrimmedJets"
  jetrec_trimm.JetPseudojetRetriever = jetPseudojetRetriever
  #jetrec_trimm.JetModifiers += [nsubjettiness]
  #jetrec_trimm.JetModifiers += [nsubjettinessratios]
  # jetrec_trimm.JetModifiers += [ktsplittingscale]
  # jetrec_trimm.JetModifiers += [dipolarity]
  # jetrec_trimm.JetModifiers += [angularity]
  # jetrec_trimm.JetModifiers += [ktdr]
  # jetrec_trimm.JetModifiers += [ktmassdrop]
  # jetrec_trimm.JetModifiers += [planarflow]
  # jetrec_trimm.JetModifiers += [centerofmassshapes]
  #jetrec_trimm.JetModifiers += [energycorrelator]
  #jetrec_trimm.JetModifiers += [energycorrelatorratios]
  # jetrec_trimm.JetModifiers += [pull]
  # jetrec_trimm.JetModifiers += [charge]
  # jetrec_trimm.JetModifiers += [subjetmaker]
  # jetrec_trimm.JetModifiers += [subjetfinder]
  ToolSvc += jetrec_trimm
  

  # Add the algorithm. It runs the demo tools.
  jetalg = JetAlgorithm("JetAlgTruth")

  # jetalg.OutputLevel = INFO
  jetalg.Tools += [copytruth, jetrec, jetrec_trimm]
  topSequence += jetalg




###end jet stuff

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "XAOD_"+Name+".pool.root" )
# xaodStream.Stream.TakeItemsFromInput = True #this will only work for the event-by-event items. MetadataItems must still be specified
xaodStream.AddItem("xAOD::EventInfo#*")
xaodStream.AddItem("xAOD::EventAuxInfo#*")
xaodStream.AddItem("xAOD::EventShape#*")
xaodStream.AddItem("xAOD::EventShapeAuxInfo#*")
xaodStream.AddItem("xAOD::VertexContainer#*")
xaodStream.AddItem("xAOD::VertexAuxContainer#*")
# xaodStream.AddItem( "xAOD::TrackCaloClusterContainer#TrackCaloClusters")
# xaodStream.AddItem( "xAOD::TrackCaloClusterAuxContainer#TrackCaloClustersAux.")
# xaodStream.AddItem( "xAOD::TrackCaloClusterContainer#TrackCaloClustersCombined")
# xaodStream.AddItem( "xAOD::TrackCaloClusterAuxContainer#TrackCaloClustersCombinedAux.")
# xaodStream.AddItem( "xAOD::TrackCaloClusterContainer#TrackCaloClustersAll")
# xaodStream.AddItem( "xAOD::TrackCaloClusterAuxContainer#TrackCaloClustersAllAux.")
# xaodStream.AddItem( "xAOD::TrackCaloClusterContainer#TrackCaloClustersAllTrack")
# xaodStream.AddItem( "xAOD::TrackCaloClusterAuxContainer#TrackCaloClustersAllTrackAux.")
xaodStream.AddItem( "xAOD::JetContainer#My*")
xaodStream.AddItem( "xAOD::JetAuxContainer#My*")
xaodStream.AddItem( "xAOD::JetContainer#*TrackCaloClusters*")
xaodStream.AddItem( "xAOD::JetAuxContainer#*TrackCaloClusters*")
# xaodStream.AddItem( "xAOD::CaloClusterContainer#*")
# xaodStream.AddItem( "xAOD::CaloClusterAuxContainer#*")

#from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
#pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True

