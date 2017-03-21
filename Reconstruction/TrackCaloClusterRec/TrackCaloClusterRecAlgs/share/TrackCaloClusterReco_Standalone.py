import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaCommon import CfgMgr
# svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/work/r/rjansky/InputHGTD/AOD.%" ]
# InputFiles = ["/afs/cern.ch/work/r/rjansky/InputHGTD/AOD.10041718._000096.pool.root.1",
# "/afs/cern.ch/work/r/rjansky/InputHGTD/AOD.10041718._000164.pool.root.1",
# "/afs/cern.ch/work/r/rjansky/InputHGTD/AOD.10041718._000312.pool.root.1"]

InputFiles = ["/afs/cern.ch/work/r/rjansky/mc15_13TeV.301267.Pythia8EvtGen_A14NNPDF23LO_Wprime_WZqqqq_m2000.merge.AOD.e3749_s2608_s2183_r8459_r7676/AOD.09378881._000006.pool.root.1"]
# InputFiles = ["/afs/cern.ch/user/n/ncalace/work/mc15_13TeV/AOD.09378881._000006.pool.root.1"]

svcMgr.EventSelector.InputCollections = InputFiles
Name = "WprimeWitTVA"

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections

from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetGeo = 'atlas'
#from RecExConfig.InputFilePeeker import inputFileSummary
#globalflags.DataSource = 'data' if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'
#globalflags.DetDescrVersion = inputFileSummary['geometry']
DetDescrVersion = 'ATLAS-R2-2015-03-01-00'
globalflags.DetDescrVersion = DetDescrVersion

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-MC15c-SDR-09')

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

loosetrackvertexassotool=CfgMgr.CP__LooseTrackVertexAssociationTool("LooseTrackVertexAssociationTool", dzSinTheta_cut=3, d0_cut=2) 
ToolSvc+=loosetrackvertexassotool 

from TrackCaloClusterRecTools.TrackCaloClusterRecToolsConf import TrackCaloClusterCreatorTool
TrackCaloClusterCreator = TrackCaloClusterCreatorTool(name                      = "TrackCaloClusterCreator",
                                                      VertexContainerName       = "PrimaryVertices",
                                                      LooseTrackVertexAssoTool  = loosetrackvertexassotool)
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

theApp.EvtMax = 1000

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
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import NSubjettinessTool
  ToolSvc += NSubjettinessTool("nsubjettiness")
  nsubjettiness = ToolSvc.nsubjettiness
  
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import NSubjettinessRatiosTool
  ToolSvc += NSubjettinessRatiosTool("nsubjettinessratios")
  nsubjettinessratios = ToolSvc.nsubjettinessratios
  
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KTSplittingScaleTool
  ToolSvc += KTSplittingScaleTool("ktsplittingscale")
  ktsplittingscale = ToolSvc.ktsplittingscale
  
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import DipolarityTool
  ToolSvc += DipolarityTool("dipolarity")
  dipolarity = ToolSvc.dipolarity
  
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import AngularityTool
  ToolSvc += AngularityTool("angularity")
  angularity = ToolSvc.angularity
  
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtDeltaRTool
  ToolSvc += KtDeltaRTool("ktdr")
  ktdr = ToolSvc.ktdr
  
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtMassDropTool
  ToolSvc += KtMassDropTool("ktmassdrop")
  ktmassdrop = ToolSvc.ktmassdrop
  
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import PlanarFlowTool
  ToolSvc += PlanarFlowTool("planarflow")
  planarflow = ToolSvc.planarflow
  
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import CenterOfMassShapesTool
  ToolSvc += CenterOfMassShapesTool("centerofmassshapes")
  centerofmassshapes = ToolSvc.centerofmassshapes
  
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import EnergyCorrelatorTool
  ToolSvc += EnergyCorrelatorTool("energycorrelator")
  energycorrelator = ToolSvc.energycorrelator
  #energycorrelator.Beta = 0.5
  #energycorrelator.IncludeBeta2 = True
  #energycorrelator.IncludeECF4 = True
  
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import EnergyCorrelatorRatiosTool
  ToolSvc += EnergyCorrelatorRatiosTool("energycorrelatorratios")
  energycorrelatorratios = ToolSvc.energycorrelatorratios
  #energycorrelatorratios.IncludeBeta2 = True
  #energycorrelatorratios.IncludeECF4 = True
  
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import JetPullTool
  ToolSvc += JetPullTool("pull")
  pull = ToolSvc.pull
  #pull.IncludeTensorMoments = True
  
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import JetChargeTool
  ToolSvc += JetChargeTool("charge")
  charge = ToolSvc.charge
  
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
   
  clname = "TrackCaloClusters"
  clname1 = "TrackCaloClustersCharged"
  clname2 = "TrackCaloClustersAll"
  
  clnames = [clname, clname1, clname2]
  
  from JetRec.JetRecConf import JetPseudojetRetriever
  from JetRec.JetRecConf import JetRecTool
  from JetRec.JetRecConf import JetAlgorithm

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
    psjget.OutputLevel = INFO
    psjget.SkipNegativeEnergy = True
    ToolSvc += psjget
    
    # Find jets.
    from JetRec.JetRecConf import JetFinder
    jfind = JetFinder("JetFinder_"+name)
    jfind.JetAlgorithm = "AntiKt"
    jfind.JetRadius = 1.0
    jfind.GhostArea = 0.01;
    jfind.PtMin = 2000.0
  #  jfind.OutputLevel = VERBOSE
    ToolSvc += jfind
      
    # JetRec tool for finding.
    jetrec = JetRecTool("JetRecTool_"+name)
    jetrec.OutputContainer = "AntiKt10"+name+"Jets"
    jetrec.OutputLevel = INFO
    jetrec.PseudoJetGetters += [psjget]
    jetrec.JetFinder = jfind  
    jetrec.JetModifiers += [nsubjettiness]
    jetrec.JetModifiers += [nsubjettinessratios]
    jetrec.JetModifiers += [ktsplittingscale]
    jetrec.JetModifiers += [dipolarity]
    jetrec.JetModifiers += [angularity]
    jetrec.JetModifiers += [ktdr]
    jetrec.JetModifiers += [ktmassdrop]
    jetrec.JetModifiers += [planarflow]
    jetrec.JetModifiers += [centerofmassshapes]
    jetrec.JetModifiers += [energycorrelator]
    jetrec.JetModifiers += [energycorrelatorratios]
    jetrec.JetModifiers += [pull]
    jetrec.JetModifiers += [charge]
    jetrec.JetModifiers += [subjetmaker]
    jetrec.JetModifiers += [subjetfinder]
    ToolSvc += jetrec
    
    jetPseudojetRetriever = JetPseudojetRetriever("JetPseudojetRetriever_"+name)
    ToolSvc += jetPseudojetRetriever
    
    # JetRec tool for finding.    
    jetrec_trimm = JetRecTool("JetRecTool_"+name+"Trimm")
    jetrec_trimm.JetGroomer = groomer
    jetrec_trimm.InputContainer  = "AntiKt10"+name+"Jets"
    jetrec_trimm.OutputContainer = "AntiKt10"+name+"TrimmedJets"
    jetrec_trimm.JetPseudojetRetriever = jetPseudojetRetriever
    jetrec_trimm.JetModifiers += [nsubjettiness]
    jetrec_trimm.JetModifiers += [nsubjettinessratios]
    jetrec_trimm.JetModifiers += [ktsplittingscale]
    jetrec_trimm.JetModifiers += [dipolarity]
    jetrec_trimm.JetModifiers += [angularity]
    jetrec_trimm.JetModifiers += [ktdr]
    jetrec_trimm.JetModifiers += [ktmassdrop]
    jetrec_trimm.JetModifiers += [planarflow]
    jetrec_trimm.JetModifiers += [centerofmassshapes]
    jetrec_trimm.JetModifiers += [energycorrelator]
    jetrec_trimm.JetModifiers += [energycorrelatorratios]
    jetrec_trimm.JetModifiers += [pull]
    jetrec_trimm.JetModifiers += [charge]
    jetrec_trimm.JetModifiers += [subjetmaker]
    jetrec_trimm.JetModifiers += [subjetfinder]
    ToolSvc += jetrec_trimm
      
    # Add the algorithm. It runs the demo tools.
    jetalg = JetAlgorithm("JetAlg_"+name)
    jetalg.OutputLevel = INFO
    jetalg.Tools += [jetrec, jetrec_trimm]
    topSequence += jetalg
    
  # Find jet inputs.
  from JetRec.JetRecConf import PseudoJetGetter
  psjget = PseudoJetGetter("PseudoJetGetter")
  psjget.InputContainer = "CaloCalTopoClusters"
  psjget.Label = "LCTopo"
  psjget.OutputContainer = "PseudoJetLCTopo"
  psjget.OutputLevel = INFO
  psjget.SkipNegativeEnergy = True
  ToolSvc += psjget
    
  # Find jets.
  from JetRec.JetRecConf import JetFinder
  jfind = JetFinder("JetFinder")
  jfind.JetAlgorithm = "AntiKt"
  jfind.JetRadius = 1.0
  jfind.GhostArea = 0.01;
  jfind.PtMin = 2000.0
  #  jfind.OutputLevel = VERBOSE
  ToolSvc += jfind
    
  # JetRec tool for finding.
  jetrec = JetRecTool("JetRecTool")
  jetrec.OutputContainer = "MyAntiKt10LCTopoJets"
  jetrec.OutputLevel = INFO
  jetrec.PseudoJetGetters += [psjget]
  jetrec.JetFinder = jfind  
  jetrec.JetModifiers += [nsubjettiness]
  jetrec.JetModifiers += [nsubjettinessratios]
  jetrec.JetModifiers += [ktsplittingscale]
  jetrec.JetModifiers += [dipolarity]
  jetrec.JetModifiers += [angularity]
  jetrec.JetModifiers += [ktdr]
  jetrec.JetModifiers += [ktmassdrop]
  jetrec.JetModifiers += [planarflow]
  jetrec.JetModifiers += [centerofmassshapes]
  jetrec.JetModifiers += [energycorrelator]
  jetrec.JetModifiers += [energycorrelatorratios]
  jetrec.JetModifiers += [pull]
  jetrec.JetModifiers += [charge]
  jetrec.JetModifiers += [subjetmaker]
  jetrec.JetModifiers += [subjetfinder]
  ToolSvc += jetrec
 
  from JetCalibTools.JetCalibToolsConf import JetCalibrationTool
  calib_tool=JetCalibrationTool('JetCalibTool',JetCollection="AntiKt10LCTopoTrimmedPtFrac5SmallR20",ConfigFile='JES_MC15recommendation_FatJet_June2015.config',CalibSequence='EtaJES_JMS',IsData=False)
  ToolSvc += calib_tool
  print calib_tool
    
  jetPseudojetRetriever = JetPseudojetRetriever("JetPseudojetRetriever")
  ToolSvc += jetPseudojetRetriever
  
  # JetRec tool for finding.
  jetrec_trimm = JetRecTool("JetRecToolTrimm")
  jetrec_trimm.JetGroomer = groomer
  jetrec_trimm.InputContainer        = "MyAntiKt10LCTopoJets"
  jetrec_trimm.OutputContainer       = "AntiKt10LCTopoTrimmedJets"
  jetrec_trimm.JetPseudojetRetriever = jetPseudojetRetriever
  jetrec_trimm.JetModifiers += [nsubjettiness]
  jetrec_trimm.JetModifiers += [nsubjettinessratios]
  jetrec_trimm.JetModifiers += [ktsplittingscale]
  jetrec_trimm.JetModifiers += [dipolarity]
  jetrec_trimm.JetModifiers += [angularity]
  jetrec_trimm.JetModifiers += [ktdr]
  jetrec_trimm.JetModifiers += [ktmassdrop]
  jetrec_trimm.JetModifiers += [planarflow]
  jetrec_trimm.JetModifiers += [centerofmassshapes]
  jetrec_trimm.JetModifiers += [energycorrelator]
  jetrec_trimm.JetModifiers += [energycorrelatorratios]
  jetrec_trimm.JetModifiers += [pull]
  jetrec_trimm.JetModifiers += [charge]
  jetrec_trimm.JetModifiers += [subjetmaker]
  jetrec_trimm.JetModifiers += [subjetfinder]
  jetrec_trimm.JetModifiers += [calib_tool]
  ToolSvc += jetrec_trimm
     
  # Add the algorithm. It runs the demo tools.
  jetalg = JetAlgorithm("JetAlg")
  jetalg.OutputLevel = INFO
  jetalg.Tools += [jetrec, jetrec_trimm]
  topSequence += jetalg

###end jet stuff

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "XAOD_"+Name+".pool.root" )
xaodStream.Stream.TakeItemsFromInput = True #this will only work for the event-by-event items. MetadataItems must still be specified
xaodStream.AddItem( "xAOD::TrackCaloClusterContainer#TrackCaloClusters")
xaodStream.AddItem( "xAOD::TrackCaloClusterAuxContainer#TrackCaloClustersAux.")
xaodStream.AddItem( "xAOD::TrackCaloClusterContainer#TrackCaloClustersCharged")
xaodStream.AddItem( "xAOD::TrackCaloClusterAuxContainer#TrackCaloClustersChargedAux.")
xaodStream.AddItem( "xAOD::TrackCaloClusterContainer#TrackCaloClustersAll")
xaodStream.AddItem( "xAOD::TrackCaloClusterAuxContainer#TrackCaloClustersAllAux.")
xaodStream.AddItem( "xAOD::JetContainer#*")
xaodStream.AddItem( "xAOD::JetAuxContainer#*")

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True

# dump configuration
from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config_algs.txt")

#ServiceMgr.StoreGateSvc.Dump=True

# include("InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py")
