#====================================================================
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#====================================================================
# IDTIDE1.py
# Contact: atlas-cp-tracking-denseenvironments@cern.ch
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
from InDetRecExample import TrackingCommon
from AthenaCommon.CFElements import seqAND, parOR

from AthenaCommon.Logging import logging
msg = logging.getLogger( "IDTIDE1" )
_info = msg.info

#Steering options

idDxAOD_doPix=True
idDxAOD_doSct=True
idDxAOD_doTrt=False

select_aux_items=True
# should be true in production 
skimEvents=True

from AthenaCommon.JobProperties import jobproperties
from InDetPrepRawDataToxAOD.InDetDxAODJobProperties import InDetDxAODFlags
pix_from_InDetDxAOD = InDetDxAODFlags.DumpPixelInfo() and jobproperties.PrimaryDPDFlags.WriteDAOD_IDTRKVALIDStream.get_Value() is True
sct_from_InDetDxAOD = InDetDxAODFlags.DumpSctInfo()   and jobproperties.PrimaryDPDFlags.WriteDAOD_IDTRKVALIDStream.get_Value() is True
trt_from_InDetDxAOD = InDetDxAODFlags.DumpTrtInfo()   and jobproperties.PrimaryDPDFlags.WriteDAOD_IDTRKVALIDStream.get_Value() is True
need_pix_ToTList = idDxAOD_doPix and ( InDetDxAODFlags.DumpPixelRdoInfo() or InDetDxAODFlags.DumpPixelNNInfo() )

# IsMonteCarlo=(globalflags.DataSource == 'geant4')

if 'DerivationFrameworkIsMonteCarlo' not in dir() :
  DerivationFrameworkIsMonteCarlo=( globalflags.DataSource=='geant4' )

IsMonteCarlo=DerivationFrameworkIsMonteCarlo

#====================================================================
# Re-run jet reconstruction needed for preselection
#====================================================================
from JetRecConfig.StandardSmallRJets import AntiKt4EMTopo,AntiKt4EMPFlow
jetList = [AntiKt4EMTopo,AntiKt4EMPFlow]
from DerivationFrameworkJetEtMiss.JetCommon import addDAODJets
addDAODJets(jetList,DerivationFrameworkJob)

#====================================================================
# SET UP STREAM  
#====================================================================
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_IDTIDEStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_IDTIDEStream )
IDTIDE1Stream = MSMgr.NewPoolRootStream( streamName, fileName )

#idtideSeq = CfgMgr.AthSequencer("IDTIDE1Sequence")
#DerivationFrameworkJob += idtideSeq
#addTrackSumMoments("AntiKt4EMTopo")
#addDefaultTrimmedJets(idtideSeq,"IDTIDE1")

augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

#====================================================================
# CP GROUP TOOLS
#====================================================================
IDTIDE1IPETool = TrackingCommon.getTrackToVertexIPEstimator(name = "IDTIDE1IPETool")
_info(IDTIDE1IPETool)

#Setup tools
if idDxAOD_doTrt:
  from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
  TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
  ServiceMgr += TRTStrawNeighbourSvc
  from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
  TRTCalibDBTool=TRT_CalDbTool(name="TRT_CalDbTool")


#====================================================================
# AUGMENTATION TOOLS
#====================================================================
augmentationTools = []
tsos_augmentationTools=[]
# Add unbiased track parameters to track particles
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackToVertexWrapper
IDTIDE1TrackToVertexWrapper= DerivationFramework__TrackToVertexWrapper(name = "IDTIDE1TrackToVertexWrapper",
                                                                       TrackToVertexIPEstimator = IDTIDE1IPETool,
                                                                       DecorationPrefix = "IDTIDE1",
                                                                       ContainerName = "InDetTrackParticles")
ToolSvc += IDTIDE1TrackToVertexWrapper 
augmentationTools.append(IDTIDE1TrackToVertexWrapper)
_info(IDTIDE1TrackToVertexWrapper)

from InDetUsedInFitTrackDecoratorTool.InDetUsedInFitTrackDecoratorToolConf import InDet__InDetUsedInFitTrackDecoratorTool
IDTIDE1UsedInFitDecoratorTool = InDet__InDetUsedInFitTrackDecoratorTool(name                 = "IDTIDE1UsedInFitDecoratorTool",
                                                                          AMVFVerticesDecoName = "TTVA_AMVFVertices",
                                                                          AMVFWeightsDecoName  = "TTVA_AMVFWeights",
                                                                          TrackContainer       = "InDetTrackParticles",
                                                                          VertexContainer      = "PrimaryVertices" )
ToolSvc += IDTIDE1UsedInFitDecoratorTool

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UsedInVertexFitTrackDecorator
IDTIDE1UsedInFitDecorator = DerivationFramework__UsedInVertexFitTrackDecorator(name                   = "IDTIDE1UsedInFitDecorator",
                                                                                UsedInFitDecoratorTool = ToolSvc.IDTIDE1UsedInFitDecoratorTool )
ToolSvc += IDTIDE1UsedInFitDecorator
augmentationTools.append(IDTIDE1UsedInFitDecorator)

# Turned off by defult for the LRT tracks as they are not used. Added this option to turn it on for future studies.
if InDetDxAODFlags.DecoLRTTTVA() and InDetFlags.doR3LargeD0() and InDetFlags.storeSeparateLargeD0Container():

#====================================================================
# DECORATE THE LRT TRACKS WITH USED-IN-FIT TTVA VARIABLES
#====================================================================
  IDTIDE1UsedInFitDecoratorToolLRT = InDet__InDetUsedInFitTrackDecoratorTool(name                 = "IDTIDE1UsedInFitDecoratorToolLRT",
                                                                          AMVFVerticesDecoName = "TTVA_AMVFVertices",
                                                                          AMVFWeightsDecoName  = "TTVA_AMVFWeights",
                                                                          TrackContainer       = "InDetLargeD0TrackParticles",
                                                                          VertexContainer      = "PrimaryVertices" )
  ToolSvc += IDTIDE1UsedInFitDecoratorToolLRT

  IDTIDE1UsedInFitDecoratorLRT = DerivationFramework__UsedInVertexFitTrackDecorator(name                   = "IDTIDE1UsedInFitDecoratorLRT",
                                                                                UsedInFitDecoratorTool = ToolSvc.IDTIDE1UsedInFitDecoratorToolLRT )
  ToolSvc += IDTIDE1UsedInFitDecoratorLRT
  augmentationTools.append(IDTIDE1UsedInFitDecoratorLRT)

# @TODO eventually computed for other extra outputs. Possible to come  up with a solution to use a common Z0AtPV if there is more than one client ?
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParametersAtPV
DFCommonZ0AtPV = DerivationFramework__TrackParametersAtPV(name                       = "DFCommonZ0AtPV",
                                                          TrackParticleContainerName = "InDetTrackParticles",
                                                          VertexContainerName        = "PrimaryVertices",
                                                          Z0SGEntryName              = "IDTIDEInDetTrackZ0AtPV" )
ToolSvc += DFCommonZ0AtPV
augmentationTools.append(DFCommonZ0AtPV)


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackStateOnSurfaceDecorator
DFTSOS = DerivationFramework__TrackStateOnSurfaceDecorator(name = "DFTrackStateOnSurfaceDecorator",
                                                          ContainerName = "InDetTrackParticles",
                                                          IsSimulation = False,
                                                          DecorationPrefix = "",
                                                          StoreTRT   = idDxAOD_doTrt,
                                                          TRT_ToT_dEdx = TrackingCommon.getInDetTRT_dEdxTool() if idDxAOD_doTrt else "",
                                                          PRDtoTrackMap= "PRDtoTrackMap" + InDetKeys.UnslimmedTracks() if  jobproperties.PrimaryDPDFlags.WriteDAOD_IDTRKVALIDStream.get_Value() else "",
                                                          StoreSCT   = idDxAOD_doSct,
                                                          StorePixel = idDxAOD_doPix,
                                                          OutputLevel =INFO)
ToolSvc += DFTSOS
tsos_augmentationTools.append(DFTSOS)
_info(DFTSOS)

# Sequence for skimming kernel (if running on data) -> PrepDataToxAOD -> ID TIDE kernel
# sequence to be used for algorithm which should run before the IDTIDEPresel
IDTIDESequencePre   = DerivationFrameworkJob
IDTIDESequence = seqAND("IDTIDESequence")

# Add decoration with truth parameters if running on simulation
if IsMonteCarlo:
  # add track parameter decorations to truth particles but only if the decorations have not been applied already
  import InDetPhysValMonitoring.InDetPhysValDecoration
  meta_data = InDetPhysValMonitoring.InDetPhysValDecoration.getMetaData()
  from AthenaCommon.Logging import logging
  logger = logging.getLogger( "DerivationFramework" )
  if len(meta_data) == 0 :
    truth_track_param_decor_alg = InDetPhysValMonitoring.InDetPhysValDecoration.getInDetPhysValTruthDecoratorAlg()
    if  InDetPhysValMonitoring.InDetPhysValDecoration.findAlg([truth_track_param_decor_alg.getName()]) == None :
      IDTIDESequencePre += truth_track_param_decor_alg
    else :
      logger.info('Decorator %s already present not adding again.' % (truth_track_param_decor_alg.getName() ))
  else :
    logger.info('IDPVM decorations to track particles already applied to input file not adding again.')


#====================================================================
# SKIMMING TOOLS 
#====================================================================
skimmingTools = []
if not IsMonteCarlo and skimEvents:
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

# sequence for algorithms which should run after the preselection bu which can run in parallel
IDTIDESeqAfterPresel = parOR("IDTIDESeqAfterPresel")
IDTIDESequence += IDTIDESeqAfterPresel

#Setup decorators tools
if idDxAOD_doTrt and not trt_from_InDetDxAOD:
  from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import TRT_PrepDataToxAOD
  xAOD_TRT_PrepDataToxAOD = TRT_PrepDataToxAOD( name = "xAOD_TRT_PrepDataToxAOD")
  xAOD_TRT_PrepDataToxAOD.OutputLevel=INFO
  xAOD_TRT_PrepDataToxAOD.UseTruthInfo=IsMonteCarlo
  _info( "Add TRT xAOD TrackMeasurementValidation: %s" , xAOD_TRT_PrepDataToxAOD)
  IDTIDESeqAfterPresel += xAOD_TRT_PrepDataToxAOD

if idDxAOD_doSct and not sct_from_InDetDxAOD:
  from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_PrepDataToxAOD
  xAOD_SCT_PrepDataToxAOD = SCT_PrepDataToxAOD( name = "xAOD_SCT_PrepDataToxAOD")
  xAOD_SCT_PrepDataToxAOD.OutputLevel=INFO
  xAOD_SCT_PrepDataToxAOD.UseTruthInfo=IsMonteCarlo
  _info("Add SCT xAOD TrackMeasurementValidation: %s", xAOD_SCT_PrepDataToxAOD)
  IDTIDESeqAfterPresel += xAOD_SCT_PrepDataToxAOD

if idDxAOD_doPix and not pix_from_InDetDxAOD:
  if need_pix_ToTList :
    from PixelCalibAlgs.PixelCalibAlgsConf import PixelChargeToTConversion 
    PixelChargeToTConversionSetter = PixelChargeToTConversion(name = "PixelChargeToTConversionSetter",
                                                              ExtraOutputs = ['PixelClusters_ToTList'])
    # IDTIDESeqAfterPresel += PixelChargeToTConversionSetter 
    topSequence += PixelChargeToTConversionSetter
    _info("Add Pixel xAOD ToTConversionSetter: %s Properties: %s", PixelChargeToTConversionSetter, PixelChargeToTConversionSetter.properties())
  from InDetPrepRawDataToxAOD.InDetDxAODUtils import getPixelPrepDataToxAOD
  xAOD_PixelPrepDataToxAOD = getPixelPrepDataToxAOD(
                                                    # OutputLevel=INFO
                                                    )
  _info( "Add Pixel xAOD TrackMeasurementValidation: %s", xAOD_PixelPrepDataToxAOD)
  IDTIDESeqAfterPresel += xAOD_PixelPrepDataToxAOD


#====================================================================
# THINNING TOOLS 
#====================================================================
thinningTools = []

# TrackParticles directly
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
kw = {}
if not idDxAOD_doPix:
  kw['InDetTrackStatesPixKey'] = ''
  kw['InDetTrackMeasurementsPixKey'] = ''
if not idDxAOD_doSct:
  kw['InDetTrackStatesSctKey'] = ''
  kw['InDetTrackMeasurementsSctKey'] = ''
if not idDxAOD_doTrt:
  kw['InDetTrackStatesTrtKey'] = ''
  kw['InDetTrackMeasurementsTrtKey'] = ''
IDTIDE1ThinningTool = DerivationFramework__TrackParticleThinning(name = "IDTIDE1ThinningTool",
                                                                 StreamName              = streamName,
                                                                 SelectionString         = "abs(IDTIDEInDetTrackZ0AtPV) < 5.0",
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                 ThinHitsOnTrack =  InDetDxAODFlags.ThinHitsOnTrack(),
                                                                 **kw)
ToolSvc += IDTIDE1ThinningTool
thinningTools.append(IDTIDE1ThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if IsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  IDTIDE1TruthThinningTool = DerivationFramework__MenuTruthThinning(name = "IDTIDE1TruthThinningTool",
      StreamName                 = streamName,
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
IDTIDESkimmingSequence = seqAND("IDTIDESkimmingSequence")
IDTIDESeqAfterPresel += IDTIDESkimmingSequence
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
idtide_kernel = CfgMgr.DerivationFramework__DerivationKernel("IDTIDE1Kernel",
                                                             AugmentationTools = augmentationTools,
                                                             SkimmingTools = skimmingTools,
                                                             ThinningTools = [],
                                                             RunSkimmingFirst = True,
                                                             OutputLevel =INFO)

IDTIDEPostProcSequence = parOR("IDTIDEPostProcSequence")

# shared between IDTIDE and IDTRKVALID
dftsos_kernel = CfgMgr.DerivationFramework__DerivationKernel("DFTSOSKernel",
                                                             AugmentationTools = tsos_augmentationTools,
                                                             ThinningTools = [],
                                                             OutputLevel =INFO)
idtidethinning_kernel = CfgMgr.DerivationFramework__DerivationKernel("IDTIDEThinningKernel",
                                                             AugmentationTools = [],
                                                             ThinningTools = thinningTools,
                                                             OutputLevel =INFO)

IDTIDESkimmingSequence += idtide_kernel
IDTIDESkimmingSequence += IDTIDEPostProcSequence

IDTIDEPostProcSequence += dftsos_kernel
IDTIDEPostProcSequence += idtidethinning_kernel

DerivationFrameworkJob += IDTIDESequence
accept_algs=[ idtide_kernel.name() ]

# Set the accept algs for the stream
IDTIDE1Stream.AcceptAlgs( accept_algs )

#====================================================================
# CONTENT LIST  
#====================================================================
IDTIDE1Stream.AddItem("xAOD::EventInfo#*")
if not select_aux_items :
   IDTIDE1Stream.AddItem("xAOD::EventAuxInfo#*")
else :
   IDTIDE1Stream.AddItem('xAOD::EventAuxInfo#EventInfoAux' \
                      +'.TrtPhaseTime.actualInteractionsPerCrossing.averageInteractionsPerCrossing.backgroundFlags.bcid' \
                      +'.beamPosSigmaX.beamPosSigmaXY.beamPosSigmaY.beamPosSigmaZ.beamPosX.beamPosY.beamPosZ.beamStatus.beamTiltXZ.beamTiltYZ' \
                      + '.coreFlags.detDescrTags.detectorMask0.detectorMask1.detectorMask2.detectorMask3.eventNumber.eventTypeBitmask.extendedLevel1ID.forwardDetFlags.larFlags' \
                      + '.level1TriggerType.lumiBlock.lumiFlags.mcChannelNumber.mcEventNumber.mcEventWeights.muonFlags.pixelFlags.runNumber.sctFlags.statusElement.streamTagDets' \
                      + '.streamTagNames.streamTagObeysLumiblock.streamTagRobs.streamTagTypes.tileFlags.timeStamp.timeStampNSOffset.trtFlags')

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
IDTIDE1Stream.AddItem("xAOD::TrackParticleContainer#InDetLargeD0TrackParticles")
if not select_aux_items :
  IDTIDE1Stream.AddItem("xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition")
  IDTIDE1Stream.AddItem("xAOD::TrackParticleAuxContainer#InDetLargeD0TrackParticlesAux.-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition")
else :
  tp_items = '.IDTIDE1_biased_PVd0Sigma.IDTIDE1_biased_PVz0Sigma.IDTIDE1_biased_PVz0SigmaSinTheta.IDTIDE1_biased_d0.IDTIDE1_biased_d0Sigma.IDTIDE1_biased_z0.IDTIDE1_biased_z0Sigma' \
           + '.IDTIDE1_biased_z0SigmaSinTheta.IDTIDE1_biased_z0SinTheta.IDTIDE1_unbiased_PVd0Sigma.IDTIDE1_unbiased_PVz0Sigma.IDTIDE1_unbiased_PVz0SigmaSinTheta.IDTIDE1_unbiased_d0' \
           + '.IDTIDE1_unbiased_d0Sigma.IDTIDE1_unbiased_z0.IDTIDE1_unbiased_z0Sigma.IDTIDE1_unbiased_z0SigmaSinTheta.IDTIDE1_unbiased_z0SinTheta' \
           + '.TRTTrackOccupancy.TRTdEdx.TRTdEdxUsedHits.TTVA_AMVFVertices.TTVA_AMVFWeights' \
           + '.TrkBLX.TrkBLY.TrkBLZ.TrkIBLX.TrkIBLY.TrkIBLZ.TrkL1X.TrkL1Y.TrkL1Z.TrkL2X.TrkL2Y.TrkL2Z' \
           + '.beamlineTiltX.beamlineTiltY.btagIp_d0.btagIp_d0Uncertainty.btagIp_trackDisplacement.btagIp_trackMomentum.btagIp_z0SinTheta.btagIp_z0SinThetaUncertainty' \
           + '.chiSquared.d0.definingParametersCovMatrixDiag.definingParametersCovMatrixOffDiag.eProbabilityComb.eProbabilityHT.eProbabilityNN' \
           + '.expectInnermostPixelLayerHit.expectNextToInnermostPixelLayerHit.hitPattern.identifierOfFirstHit.msosLink.nBC_meas.numberDoF.numberOfContribPixelLayers' \
           + '.numberOfDBMHits.numberOfGangedFlaggedFakes.numberOfGangedPixels.numberOfIBLOverflowsdEdx.numberOfInnermostPixelLayerHits.numberOfInnermostPixelLayerOutliers' \
           + '.numberOfInnermostPixelLayerSharedHits.numberOfInnermostPixelLayerSplitHits.numberOfNextToInnermostPixelLayerHits.numberOfNextToInnermostPixelLayerOutliers' \
           + '.numberOfNextToInnermostPixelLayerSharedHits.numberOfNextToInnermostPixelLayerSplitHits.numberOfOutliersOnTrack.numberOfPhiHoleLayers.numberOfPhiLayers' \
           + '.numberOfPixelDeadSensors.numberOfPixelHits.numberOfPixelHoles.numberOfPixelOutliers.numberOfPixelSharedHits.numberOfPixelSplitHits.numberOfPixelSpoiltHits' \
           + '.numberOfPrecisionHoleLayers.numberOfPrecisionLayers.numberOfSCTDeadSensors.numberOfSCTDoubleHoles.numberOfSCTHits.numberOfSCTHoles.numberOfSCTOutliers' \
           + '.numberOfSCTSharedHits.numberOfSCTSpoiltHits.numberOfTRTDeadStraws.numberOfTRTHighThresholdHits.numberOfTRTHighThresholdHitsTotal.numberOfTRTHighThresholdOutliers' \
           + '.numberOfTRTHits.numberOfTRTHoles.numberOfTRTOutliers.numberOfTRTSharedHits.numberOfTRTTubeHits.numberOfTRTXenonHits.numberOfTriggerEtaHoleLayers' \
           + '.numberOfTriggerEtaLayers.numberOfUsedHitsdEdx' \
           + '.particleHypothesis.patternRecoInfo.phi.pixeldEdx.qOverP.radiusOfFirstHit.standardDeviationOfChi2OS.theta.trackFitter.trackLink.trackProperties' \
           + '.vx.vy.vz.z0'
  if IsMonteCarlo :
    tp_items +=  '.truthMatchProbability.truthParticleLink.truthOrigin.truthType'
  IDTIDE1Stream.AddItem("xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux" + tp_items)
  IDTIDE1Stream.AddItem("xAOD::TrackParticleAuxContainer#InDetLargeD0TrackParticlesAux" + tp_items)

IDTIDE1Stream.AddItem("xAOD::TrackParticleClusterAssociationContainer#InDetTrackParticlesClusterAssociations*")
IDTIDE1Stream.AddItem("xAOD::TrackParticleClusterAssociationAuxContainer#InDetTrackParticlesClusterAssociations*")
IDTIDE1Stream.AddItem("xAOD::TrackParticleClusterAssociationContainer#InDetLargeD0TrackParticlesClusterAssociations*")
IDTIDE1Stream.AddItem("xAOD::TrackParticleClusterAssociationAuxContainer#InDetLargeD0TrackParticlesClusterAssociations*")
# to build jets
IDTIDE1Stream.AddItem("xAOD::FlowElementContainer#JetETMissNeutralParticleFlowObjects")
IDTIDE1Stream.AddItem("xAOD::FlowElementAuxContainer#JetETMissNeutralParticleFlowObjectsAux.")
IDTIDE1Stream.AddItem("xAOD::FlowElementContainer#JetETMissChargedParticleFlowObjects")
IDTIDE1Stream.AddItem("xAOD::FlowElementAuxContainer#JetETMissChargedParticleFlowObjectsAux.")

keys = ['PixelMSOSs'] if idDxAOD_doPix else []
keys+= ['SCT_MSOSs']  if idDxAOD_doSct else []
keys+= ['TRT_MSOSs']  if idDxAOD_doTrt else []
for key in keys :
  IDTIDE1Stream.AddItem("xAOD::TrackStateValidationContainer#%s*" % key)
  IDTIDE1Stream.AddItem("xAOD::TrackStateValidationAuxContainer#%s*" % key)
keys = []
if idDxAOD_doPix and not select_aux_items :
  keys += ['PixelClusters'] if idDxAOD_doPix else []
elif idDxAOD_doPix :
  keys += ['PixelClustersAux' \
           +'.BiasVoltage.DCSState.DepletionVoltage.LVL1A.LorentzShift.NN_etaPixelIndexWeightedPosition.NN_localEtaPixelIndexWeightedPosition.NN_localPhiPixelIndexWeightedPosition' \
           + '.NN_matrixOfCharge.NN_matrixOfToT.NN_phiBS.NN_phiPixelIndexWeightedPosition.NN_sizeX.NN_sizeY.NN_thetaBS.NN_vectorOfPitchesY.Temperature.ToT.bec.broken.charge.detectorElementID' \
           + '.eta_module.eta_pixel_index.gangedPixel.globalX.globalY.globalZ.hasBSError.identifier.isFake.isSplit.layer.localX.localXError.localXYCorrelation.localY.localYError.nRDO.phi_module' \
           + '.phi_pixel_index.rdoIdentifierList.rdo_Aterm.rdo_Cterm.rdo_Eterm.rdo_charge.rdo_eta_pixel_index.rdo_phi_pixel_index.rdo_tot.sihit_barcode.sizePhi.sizeZ.splitProbability1' \
           + '.splitProbability2.truth_barcode'
           ]
  if IsMonteCarlo :
    keys[-1] += '.NN_barcode.NN_energyDep.NN_motherBarcode.NN_motherPdgid.NN_pathlengthX.NN_pathlengthY.NN_pathlengthZ.NN_pdgid.NN_phi.NN_positionsX.NN_positionsY' \
      + '.NN_positions_indexX.NN_positions_indexY.NN_theta.NN_trueP.sdo_depositsBarcode.sdo_depositsEnergy.sdo_words.sihit_endPosX.sihit_endPosY.sihit_endPosZ.sihit_energyDeposit' \
      + '.sihit_meanTime.sihit_pdgid.sihit_startPosX.sihit_startPosY.sihit_startPosZ'

keys+= ['SCT_Clusters']     if idDxAOD_doSct else []
keys+= ['TRT_DriftCircles'] if idDxAOD_doTrt else []
print('DEBUG keys %s' % (keys))
for key in keys :
  IDTIDE1Stream.AddItem("xAOD::TrackMeasurementValidationContainer#%s*" % key.split('Aux.',1)[0])
  IDTIDE1Stream.AddItem("xAOD::TrackMeasurementValidationAuxContainer#%s%s" % (key,"*" if key.find(".")<0 else ""))
IDTIDE1Stream.AddItem("xAOD::VertexContainer#PrimaryVertices")
IDTIDE1Stream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV")
IDTIDE1Stream.AddItem("xAOD::ElectronContainer#Electrons")
IDTIDE1Stream.AddItem("xAOD::ElectronAuxContainer#ElectronsAux.")
IDTIDE1Stream.AddItem("xAOD::PhotonContainer#Photons")
IDTIDE1Stream.AddItem("xAOD::PhotonAuxContainer#PhotonsAux.")
IDTIDE1Stream.AddItem("xAOD::MuonContainer#Muons")
IDTIDE1Stream.AddItem("xAOD::MuonAuxContainer#MuonsAux.")
IDTIDE1Stream.AddItem("xAOD::TauJetContainer#TauJets")
IDTIDE1Stream.AddItem("xAOD::TauJetAuxContainer#TauJetsAux.-VertexedClusters.")
IDTIDE1Stream.AddItem("xAOD::JetContainer#AntiKt4EMTopoJets")
IDTIDE1Stream.AddItem("xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.-PseudoJet")
IDTIDE1Stream.AddItem("xAOD::JetContainer#AntiKt4EMPFlowJets")
IDTIDE1Stream.AddItem("xAOD::JetAuxContainer#AntiKt4EMPFlowJetsAux.-PseudoJet")
IDTIDE1Stream.AddItem("xAOD::JetContainer#AntiKt2PV0TrackJets")
IDTIDE1Stream.AddItem("xAOD::JetAuxContainer#AntiKt2PV0TrackJetsAux.-PseudoJet")
#IDTIDE1Stream.AddItem("xAOD::JetContainer#AntiKt3PV0TrackJets")
#IDTIDE1Stream.AddItem("xAOD::JetAuxContainer#AntiKt3PV0TrackJetsAux.")
#IDTIDE1Stream.AddItem("xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets")
#IDTIDE1Stream.AddItem("xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.")
IDTIDE1Stream.AddItem("xAOD::BTaggingContainer#BTagging_AntiKt4EMTopo")
IDTIDE1Stream.AddItem("xAOD::BTaggingAuxContainer#BTagging_AntiKt4EMTopoAux.")
IDTIDE1Stream.AddItem("xAOD::BTaggingContainer#BTagging_AntiKt4EMPFlow")
IDTIDE1Stream.AddItem("xAOD::BTaggingAuxContainer#BTagging_AntiKt4EMPFlowAux.")
IDTIDE1Stream.AddItem("xAOD::BTaggingContainer#BTagging_AntiKt2Track")
IDTIDE1Stream.AddItem("xAOD::BTaggingAuxContainer#BTagging_AntiKt2TrackAux.")
#IDTIDE1Stream.AddItem("xAOD::BTaggingContainer#BTagging_AntiKt3Track")
#IDTIDE1Stream.AddItem("xAOD::BTaggingAuxContainer#BTagging_AntiKt3TrackAux.")
if IsMonteCarlo:
  IDTIDE1Stream.AddItem("xAOD::TruthParticleContainer#*")
  IDTIDE1Stream.AddItem("xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension")
  IDTIDE1Stream.AddItem("xAOD::TruthVertexContainer#*")
  IDTIDE1Stream.AddItem("xAOD::TruthVertexAuxContainer#*.-vxTrackAtVertex")
  IDTIDE1Stream.AddItem("xAOD::TruthEventContainer#*")
  IDTIDE1Stream.AddItem("xAOD::TruthEventAuxContainer#*")
  IDTIDE1Stream.AddItem("xAOD::JetContainer#AntiKt4TruthJets")
  IDTIDE1Stream.AddItem("xAOD::JetAuxContainer#AntiKt4TruthJetsAux.-PseudoJet")

print(IDTIDE1Stream)
