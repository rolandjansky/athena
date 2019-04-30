#====================================================================
# IDTIDE1.py
# Contact: bnachman@cern.ch
# 
# Caveat: This is still under development.
#       - storing strategy: so far use AddItem() method. But in future,
#         should move to SlimmingHelper() method.
#====================================================================
 
# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

from DerivationFrameworkInDet.InDetCommon import *
from InDetPrepRawDataToxAOD.InDetDxAODJobProperties import InDetDxAODFlags

from AthenaCommon.Logging import logging
msg = logging.getLogger( "IDTIDE1" )
_info = msg.info

#Steering options
idDxAOD_doPix=True
idDxAOD_doSct=True
idDxAOD_doTrt=False

# IsMonteCarlo=(globalflags.DataSource == 'geant4')

if 'DerivationFrameworkIsMonteCarlo' not in dir() :
  DerivationFrameworkIsMonteCarlo=( globalflags.DataSource=='geant4' )

IsMonteCarlo=DerivationFrameworkIsMonteCarlo

#====================================================================
# CP GROUP TOOLS
#====================================================================
from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
IDTIDE1IPETool = Trk__TrackToVertexIPEstimator(name = "IDTIDE1IPETool")
ToolSvc += IDTIDE1IPETool
_info(IDTIDE1IPETool)

#Setup tools
if idDxAOD_doTrt:
  from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
  TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
  ServiceMgr += TRTStrawNeighbourSvc
  from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
  TRTCalibDBSvc=TRT_CalDbSvc()
  ServiceMgr += TRTCalibDBSvc


#====================================================================
# AUGMENTATION TOOLS
#====================================================================
augmentationTools = []
# Add unbiased track parameters to track particles
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackToVertexWrapper
IDTIDE1TrackToVertexWrapper= DerivationFramework__TrackToVertexWrapper(name = "IDTIDE1TrackToVertexWrapper",
                                                                       TrackToVertexIPEstimator = IDTIDE1IPETool,
                                                                       DecorationPrefix = "IDTIDE1",
                                                                       ContainerName = "InDetTrackParticles")
ToolSvc += IDTIDE1TrackToVertexWrapper 
augmentationTools.append(IDTIDE1TrackToVertexWrapper)
_info(IDTIDE1TrackToVertexWrapper)

# Add decoration with truth parameters if running on simulation
if IsMonteCarlo:
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParametersForTruthParticles
    TruthDecor = DerivationFramework__TrackParametersForTruthParticles( name = "TruthTPDecor",
                                                                        DecorationPrefix = "")
    ToolSvc += TruthDecor
    augmentationTools.append(TruthDecor)
    _info(TruthDecor)


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackStateOnSurfaceDecorator
DFTSOS = DerivationFramework__TrackStateOnSurfaceDecorator(name = "DFTrackStateOnSurfaceDecorator",
                                                          ContainerName = "InDetTrackParticles",
                                                          IsSimulation = False,
                                                          DecorationPrefix = "",
                                                          StoreTRT   = idDxAOD_doTrt,
                                                          StoreSCT   = idDxAOD_doSct,
                                                          StorePixel = idDxAOD_doPix,
                                                          OutputLevel =INFO)
ToolSvc += DFTSOS
augmentationTools.append(DFTSOS)
_info(DFTSOS)

# Sequence for skimming kernel (if running on data) -> PrepDataToxAOD -> ID TIDE kernel
IDTIDESequence = CfgMgr.AthSequencer("IDTIDESequence")

#====================================================================
# SKIMMING TOOLS 
#====================================================================
skimmingTools = []
if not IsMonteCarlo:
  sel_jet600 = 'AntiKt4EMTopoJets.JetConstitScaleMomentum_pt >= 600.*GeV'
  sel_jet800 = 'AntiKt4EMTopoJets.JetConstitScaleMomentum_pt >= 800.*GeV'
  sel_jet1000 = 'AntiKt4EMTopoJets.JetConstitScaleMomentum_pt >= 1000.*GeV'

  desd_jetA = '(HLT_j100 || HLT_j110 || HLT_j150 || HLT_j175 || HLT_j200 || HLT_j260 || HLT_j300)'
  desd_jetB = '( HLT_j320 )'
  desd_jetC = '( HLT_j360 ||  HLT_j380 || HLT_j400 )'
  desd_jetD = '( HLT_j420 && !HLT_j460 )'
  desd_jetE = '( HLT_j460 )'
  desd_jetF = '( HLT_j460 && count('+sel_jet600+')>0 && count('+sel_jet800+')==0 )'
  desd_jetG = '( HLT_j460 && count('+sel_jet800+')>0 && count('+sel_jet1000+')==0 )'
  desd_jetH = '( HLT_j460 && count('+sel_jet1000+')>0 )'

  prescaleA = 20
  prescaleB = 10
  prescaleC = 40
  prescaleD = 30
  prescaleE = 20
  prescaleF = 10
  prescaleG = 5
  prescaleH = 1

  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__PrescaleTool

  IDTIDE_SkimmingToolA = DerivationFramework__xAODStringSkimmingTool(name = "IDTIDE_SkimmingToolA", expression = desd_jetA)
  ToolSvc += IDTIDE_SkimmingToolA
  IDTIDE_PrescaleToolA = DerivationFramework__PrescaleTool(name="IDTIDE_PrescaleToolA",Prescale=prescaleA)
  ToolSvc += IDTIDE_PrescaleToolA
  IDTIDE_ANDToolA = DerivationFramework__FilterCombinationAND(name="IDTIDE_ANDToolA",FilterList=[IDTIDE_SkimmingToolA,IDTIDE_PrescaleToolA] )
  ToolSvc += IDTIDE_ANDToolA

  IDTIDE_SkimmingToolB = DerivationFramework__xAODStringSkimmingTool(name = "IDTIDE_SkimmingToolB", expression = desd_jetB)
  ToolSvc += IDTIDE_SkimmingToolB
  IDTIDE_PrescaleToolB = DerivationFramework__PrescaleTool(name="IDTIDE_PrescaleToolB",Prescale=prescaleB)
  ToolSvc += IDTIDE_PrescaleToolB
  IDTIDE_ANDToolB = DerivationFramework__FilterCombinationAND(name="IDTIDE_ANDToolB",FilterList=[IDTIDE_SkimmingToolB,IDTIDE_PrescaleToolB] )
  ToolSvc += IDTIDE_ANDToolB

  IDTIDE_SkimmingToolC = DerivationFramework__xAODStringSkimmingTool(name = "IDTIDE_SkimmingToolC", expression = desd_jetC)
  ToolSvc += IDTIDE_SkimmingToolC
  IDTIDE_PrescaleToolC = DerivationFramework__PrescaleTool(name="IDTIDE_PrescaleToolC",Prescale=prescaleC)
  ToolSvc += IDTIDE_PrescaleToolC
  IDTIDE_ANDToolC = DerivationFramework__FilterCombinationAND(name="IDTIDE_ANDToolC",FilterList=[IDTIDE_SkimmingToolC,IDTIDE_PrescaleToolC] )
  ToolSvc += IDTIDE_ANDToolC

  IDTIDE_SkimmingToolD = DerivationFramework__xAODStringSkimmingTool(name = "IDTIDE_SkimmingToolD", expression = desd_jetD)
  ToolSvc += IDTIDE_SkimmingToolD
  IDTIDE_PrescaleToolD = DerivationFramework__PrescaleTool(name="IDTIDE_PrescaleToolD",Prescale=prescaleD)
  ToolSvc += IDTIDE_PrescaleToolD
  IDTIDE_ANDToolD = DerivationFramework__FilterCombinationAND(name="IDTIDE_ANDToolD",FilterList=[IDTIDE_SkimmingToolD,IDTIDE_PrescaleToolD] )
  ToolSvc += IDTIDE_ANDToolD

  IDTIDE_SkimmingToolE = DerivationFramework__xAODStringSkimmingTool(name = "IDTIDE_SkimmingToolE", expression = desd_jetE)
  ToolSvc += IDTIDE_SkimmingToolE
  IDTIDE_PrescaleToolE = DerivationFramework__PrescaleTool(name="IDTIDE_PrescaleToolE",Prescale=prescaleE)
  ToolSvc += IDTIDE_PrescaleToolE
  IDTIDE_ANDToolE = DerivationFramework__FilterCombinationAND(name="IDTIDE_ANDToolE",FilterList=[IDTIDE_SkimmingToolE,IDTIDE_PrescaleToolE] )
  ToolSvc += IDTIDE_ANDToolE

  IDTIDE_SkimmingToolF = DerivationFramework__xAODStringSkimmingTool(name = "IDTIDE_SkimmingToolF", expression = desd_jetF)
  ToolSvc += IDTIDE_SkimmingToolF
  IDTIDE_PrescaleToolF = DerivationFramework__PrescaleTool(name="IDTIDE_PrescaleToolF",Prescale=prescaleF)
  ToolSvc += IDTIDE_PrescaleToolF
  IDTIDE_ANDToolF = DerivationFramework__FilterCombinationAND(name="IDTIDE_ANDToolF",FilterList=[IDTIDE_SkimmingToolF,IDTIDE_PrescaleToolF] )
  ToolSvc += IDTIDE_ANDToolF

  IDTIDE_SkimmingToolG = DerivationFramework__xAODStringSkimmingTool(name = "IDTIDE_SkimmingToolG", expression = desd_jetG)
  ToolSvc += IDTIDE_SkimmingToolG
  IDTIDE_PrescaleToolG = DerivationFramework__PrescaleTool(name="IDTIDE_PrescaleToolG",Prescale=prescaleG)
  ToolSvc += IDTIDE_PrescaleToolG
  IDTIDE_ANDToolG = DerivationFramework__FilterCombinationAND(name="IDTIDE_ANDToolG",FilterList=[IDTIDE_SkimmingToolG,IDTIDE_PrescaleToolG] )
  ToolSvc += IDTIDE_ANDToolG

  IDTIDE_SkimmingToolH = DerivationFramework__xAODStringSkimmingTool(name = "IDTIDE_SkimmingToolH", expression = desd_jetH)
  ToolSvc += IDTIDE_SkimmingToolH

  IDTIDE_ORTool = DerivationFramework__FilterCombinationOR(name="myLogicalCombination", FilterList=[IDTIDE_ANDToolA,IDTIDE_ANDToolB,IDTIDE_ANDToolC,IDTIDE_ANDToolD,IDTIDE_ANDToolE,IDTIDE_ANDToolF,IDTIDE_ANDToolG,IDTIDE_SkimmingToolH] )
  ToolSvc += IDTIDE_ORTool

  skimmingTools.append(IDTIDE_ORTool)
  _info( "IDTIDE1.py IDTIDE_ORTool: %s", IDTIDE_ORTool)
  
  # Add the skimming kernel to the sequence
  from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
  IDTIDESequence += CfgMgr.DerivationFramework__DerivationKernel("IDTIDE1KernelPresel",
                                                                 SkimmingTools = skimmingTools)

#Setup decorators tools
if idDxAOD_doTrt:
  from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import TRT_PrepDataToxAOD
  xAOD_TRT_PrepDataToxAOD = TRT_PrepDataToxAOD( name = "xAOD_TRT_PrepDataToxAOD")
  xAOD_TRT_PrepDataToxAOD.OutputLevel=INFO
  xAOD_TRT_PrepDataToxAOD.UseTruthInfo=IsMonteCarlo
  _info( "Add TRT xAOD TrackMeasurementValidation: %s" , xAOD_TRT_PrepDataToxAOD)
  IDTIDESequence += xAOD_TRT_PrepDataToxAOD

if idDxAOD_doSct:
  from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_PrepDataToxAOD
  xAOD_SCT_PrepDataToxAOD = SCT_PrepDataToxAOD( name = "xAOD_SCT_PrepDataToxAOD")
  xAOD_SCT_PrepDataToxAOD.OutputLevel=INFO
  xAOD_SCT_PrepDataToxAOD.UseTruthInfo=IsMonteCarlo
  _info("Add SCT xAOD TrackMeasurementValidation: %s", xAOD_SCT_PrepDataToxAOD)
  IDTIDESequence += xAOD_SCT_PrepDataToxAOD

if idDxAOD_doPix:
  from PixelCalibAlgs.PixelCalibAlgsConf import PixelChargeToTConversion 
  PixelChargeToTConversionSetter = PixelChargeToTConversion(name = "PixelChargeToTConversionSetter") 
  IDTIDESequence += PixelChargeToTConversionSetter 
  _info("Add Pixel xAOD ToTConversionSetter: %s Properties: %s", PixelChargeToTConversionSetter, PixelChargeToTConversionSetter.properties())
  from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD
  xAOD_PixelPrepDataToxAOD = PixelPrepDataToxAOD( name = "xAOD_PixelPrepDataToxAOD")
  xAOD_PixelPrepDataToxAOD.OutputLevel=INFO
  xAOD_PixelPrepDataToxAOD.UseTruthInfo=IsMonteCarlo
  _info( "Add Pixel xAOD TrackMeasurementValidation: %s", xAOD_PixelPrepDataToxAOD)
  IDTIDESequence += xAOD_PixelPrepDataToxAOD


#====================================================================
# THINNING TOOLS 
#====================================================================
thinningTools = []

# TrackParticles directly
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
IDTIDE1ThinningTool = DerivationFramework__TrackParticleThinning(name = "IDTIDE1ThinningTool",
                                                                 ThinningService         = "IDTIDE1ThinningSvc",
                                                                 SelectionString         = "abs(DFCommonInDetTrackZ0AtPV) < 5.0",
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                 ThinHitsOnTrack =  InDetDxAODFlags.ThinHitsOnTrack())
ToolSvc += IDTIDE1ThinningTool
thinningTools.append(IDTIDE1ThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if IsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  IDTIDE1TruthThinningTool = DerivationFramework__MenuTruthThinning(name = "IDTIDE1TruthThinningTool",
      ThinningService            = "IDTIDE1ThinningSvc",
      WritePartons               = True,
      WriteHadrons               = True,
      WriteBHadrons              = True, 
      WriteGeant                 = True,
      GeantPhotonPtThresh        = 20000,
      WriteTauHad                = True,
      PartonPtThresh             = -1.0,
      WriteBSM                   = True,
      WriteBosons                = True,
      WriteBosonProducts         = True, 
      WriteBSMProducts           = True,
      WriteTopAndDecays          = True, 
      WriteEverything            = True,
      WriteAllLeptons            = True,
      WriteLeptonsNotFromHadrons = True,
      WriteStatus3               = True,
      WriteFirstN                = -1,
      PreserveAncestors          = True, 
      PreserveGeneratorDescendants = True)
  ToolSvc += IDTIDE1TruthThinningTool
  thinningTools.append(IDTIDE1TruthThinningTool)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
IDTIDESequence += CfgMgr.DerivationFramework__DerivationKernel("IDTIDE1Kernel",
                                                               AugmentationTools = augmentationTools,
                                                               ThinningTools = thinningTools,
                                                               OutputLevel =INFO)

DerivationFrameworkJob += IDTIDESequence
#====================================================================
# SET UP STREAM  
#====================================================================
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_IDTIDEStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_IDTIDEStream )
IDTIDE1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
IDTIDE1Stream.AcceptAlgs(["IDTIDE1Kernel"])

#idtideSeq = CfgMgr.AthSequencer("IDTIDE1Sequence")
#DerivationFrameworkJob += idtideSeq
#addTrackSumMoments("AntiKt4EMTopo")
#addDefaultTrimmedJets(idtideSeq,"IDTIDE1")

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="IDTIDE1ThinningSvc", outStreams=[evtStream] )

#====================================================================
# CONTENT LIST  
#====================================================================
IDTIDE1Stream.AddItem("xAOD::EventInfo#*")
IDTIDE1Stream.AddItem("xAOD::EventAuxInfo#*")
IDTIDE1Stream.AddItem("xAOD::EventShape#*")
IDTIDE1Stream.AddItem("xAOD::EventShapeAuxInfo#*")
IDTIDE1Stream.AddItem("xAOD::TriggerMenuContainer#*")
IDTIDE1Stream.AddItem("xAOD::TriggerMenuContainer#TriggerMenu")
IDTIDE1Stream.AddItem("xAOD::TriggerMenuAuxContainer#*")
IDTIDE1Stream.AddItem("xAOD::TriggerMenuAuxContainer#TriggerMenuAux.")
IDTIDE1Stream.AddItem("xAOD::TrigConfKeys#*")
IDTIDE1Stream.AddItem("xAOD::TrigDecision#*")
IDTIDE1Stream.AddItem("xAOD::TrigDecisionAuxInfo#*")
#IDTIDE1Stream.AddItem("xAOD::TrigNavigation#*")
#IDTIDE1Stream.AddItem("xAOD::TrigNavigationAuxInfo#*")
IDTIDE1Stream.AddItem("xAOD::TrackParticleContainer#InDetTrackParticles")
IDTIDE1Stream.AddItem("xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition")
IDTIDE1Stream.AddItem("xAOD::TrackParticleClusterAssociationContainer#*")
IDTIDE1Stream.AddItem("xAOD::TrackParticleClusterAssociationAuxContainer#*")
IDTIDE1Stream.AddItem("xAOD::TrackStateValidationContainer#*")
IDTIDE1Stream.AddItem("xAOD::TrackStateValidationAuxContainer#*")
IDTIDE1Stream.AddItem("xAOD::TrackMeasurementValidationContainer#*")
IDTIDE1Stream.AddItem("xAOD::TrackMeasurementValidationAuxContainer#*")
IDTIDE1Stream.AddItem("xAOD::VertexContainer#PrimaryVertices")
IDTIDE1Stream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex")
IDTIDE1Stream.AddItem("xAOD::ElectronContainer#Electrons")
IDTIDE1Stream.AddItem("xAOD::ElectronAuxContainer#ElectronsAux.")
IDTIDE1Stream.AddItem("xAOD::PhotonContainer#Photons")
IDTIDE1Stream.AddItem("xAOD::PhotonAuxContainer#PhotonsAux.")
IDTIDE1Stream.AddItem("xAOD::MuonContainer#Muons")
IDTIDE1Stream.AddItem("xAOD::MuonAuxContainer#MuonsAux.")
IDTIDE1Stream.AddItem("xAOD::TauJetContainer#TauJets")
IDTIDE1Stream.AddItem("xAOD::TauJetAuxContainer#TauJetsAux.")
IDTIDE1Stream.AddItem("xAOD::JetContainer#AntiKt4EMTopoJets")
IDTIDE1Stream.AddItem("xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.")
IDTIDE1Stream.AddItem("xAOD::JetContainer#AntiKt2PV0TrackJets")
IDTIDE1Stream.AddItem("xAOD::JetAuxContainer#AntiKt2PV0TrackJetsAux.")
#IDTIDE1Stream.AddItem("xAOD::JetContainer#AntiKt3PV0TrackJets")
#IDTIDE1Stream.AddItem("xAOD::JetAuxContainer#AntiKt3PV0TrackJetsAux.")
#IDTIDE1Stream.AddItem("xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets")
#IDTIDE1Stream.AddItem("xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.")
IDTIDE1Stream.AddItem("xAOD::BTaggingContainer#BTagging_AntiKt4EMTopo")
IDTIDE1Stream.AddItem("xAOD::BTaggingAuxContainer#BTagging_AntiKt4EMTopoAux.")
IDTIDE1Stream.AddItem("xAOD::BTaggingContainer#BTagging_AntiKt2Track")
IDTIDE1Stream.AddItem("xAOD::BTaggingAuxContainer#BTagging_AntiKt2TrackAux.")
#IDTIDE1Stream.AddItem("xAOD::BTaggingContainer#BTagging_AntiKt3Track")
#IDTIDE1Stream.AddItem("xAOD::BTaggingAuxContainer#BTagging_AntiKt3TrackAux.")
if IsMonteCarlo:
  IDTIDE1Stream.AddItem("xAOD::TruthParticleContainer#*")
  IDTIDE1Stream.AddItem("xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension")
  IDTIDE1Stream.AddItem("xAOD::TruthVertexContainer#*")
  IDTIDE1Stream.AddItem("xAOD::TruthVertexAuxContainer#*-vxTrackAtVertex")
  IDTIDE1Stream.AddItem("xAOD::TruthEventContainer#*")
  IDTIDE1Stream.AddItem("xAOD::TruthEventAuxContainer#*")
  IDTIDE1Stream.AddItem("xAOD::JetContainer#AntiKt4TruthJets")
  IDTIDE1Stream.AddItem("xAOD::JetAuxContainer#AntiKt4TruthJetsAux.")
