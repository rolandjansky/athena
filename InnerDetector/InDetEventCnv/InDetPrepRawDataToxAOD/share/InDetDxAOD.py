#################
### Steering options
#################
## Load common flags
from AthenaCommon.JobProperties import jobproperties as athCommonFlags
from DerivationFrameworkInDet.InDetCommon import *

from InDetPrepRawDataToxAOD.InDetDxAODJobProperties import InDetDxAODFlags

# Select active sub-systems
dumpPixInfo = InDetDxAODFlags.DumpPixelInfo()
dumpSctInfo = InDetDxAODFlags.DumpSctInfo()
dumpTrtInfo = InDetDxAODFlags.DumpTrtInfo()

# Thin hits to store only the ones on-track
thinHitsOnTrack= InDetDxAODFlags.ThinHitsOnTrack()

# Thin track collection, if necessary
# Example (p_T > 1.0 GeV && delta_z0 < 5 mm):
# InDetTrackParticles.pt > 1*GeV && abs(DFCommonInDetTrackZ0AtPV) < 5.0
thinTrackSelection = InDetDxAODFlags.ThinTrackSelection() ##"InDetTrackParticles.pt > 0.1*GeV"

# Bytestream errors (for sub-systems who have implemented it)
dumpBytestreamErrors=InDetDxAODFlags.DumpByteStreamErrors() #True

# Unassociated hits decorations
dumpUnassociatedHits= InDetDxAODFlags.DumpUnassociatedHits() #True

# Add LArCollisionTime augmentation tool
dumpLArCollisionTime=InDetDxAODFlags.DumpLArCollisionTime() #True

# Force to do not dump truth info if set to False
#  (otherwise determined by autoconf below)
dumpTruthInfo=InDetDxAODFlags.DumpTruthInfo() # True

# Saves partial trigger information in the output stream (none otherwise)
dumpTriggerInfo= InDetDxAODFlags.DumpTriggerInfo()  #True

# Print settings for main tools
printIdTrkDxAODConf = InDetDxAODFlags.PrintIdTrkDxAODConf()  # True

# Create split-tracks if running on cosmics
makeSplitTracks = InDetDxAODFlags.MakeSplitCosmicTracks() and athCommonFlags.Beam.beamType() == 'cosmics'

## Autoconfiguration adjustements
isIdTrkDxAODSimulation = False
if (globalflags.DataSource == 'geant4'):
    isIdTrkDxAODSimulation = True

if ( 'dumpTruthInfo' in dir() ):
    dumpTruthInfo = dumpTruthInfo and isIdTrkDxAODSimulation

if InDetFlags.doSLHC():
    dumpTrtInfo=False

## Other settings
# Prefix for decoration, if any
prefixName = ""

## More fine-tuning available for each tool/alg below (default value shown)

#################
### Setup tools
#################
if dumpTrtInfo:
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
    TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
    ServiceMgr += TRTStrawNeighbourSvc

    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
    TRTCalibDBSvc=TRT_CalDbSvc()
    ServiceMgr += TRTCalibDBSvc

    from TRT_ToT_Tools.TRT_ToT_ToolsConf import TRT_ToT_dEdx
    TRT_dEdx_Tool = TRT_ToT_dEdx(name="TRT_ToT_dEdx")
    ToolSvc += TRT_dEdx_Tool

#Setup charge->ToT back-conversion to restore ToT info as well
if dumpPixInfo: 
    from AthenaCommon.AlgSequence import AlgSequence 
    topSequence = AlgSequence() 
    from PixelCalibAlgs.PixelCalibAlgsConf import PixelChargeToTConversion 
    PixelChargeToTConversionSetter = PixelChargeToTConversion(name = "PixelChargeToTConversionSetter") 
    topSequence += PixelChargeToTConversionSetter 
    if (printIdTrkDxAODConf):
        print PixelChargeToTConversionSetter
        print PixelChargeToTConversionSetter.properties()

#Setup SCT extension efficiency algorithm if running pixel tracklets
#if InDetFlags.doTrackSegmentsPixel():
#    include ("SCTExtension/SCTExtensionAlg.py")

#Setup split-tracks reconstruction in cosmic-mode and produce xAOD::TrackParticles
if makeSplitTracks:
    # Set input/output container names
    # Setup algorithm to create split tracks
    from InDetTrackSplitterTool.InDetTrackSplitterToolConf import InDet__InDetTrackSplitterTool
    splittertoolcomb= InDet__InDetTrackSplitterTool(name="SplitterTool",
                                                    TrackFitter=ToolSvc.InDetTrackFitter,
                                                    OutputUpperTracksName = "TracksUpperSplit",
                                                    OutputLowerTracksName = "TracksLowerSplit") 
    ToolSvc += splittertoolcomb

    from InDetTrackValidation.InDetTrackValidationConf import InDet__InDetSplittedTracksCreator
    splittercomb=InDet__InDetSplittedTracksCreator(name='CombinedTrackSplitter',
    TrackSplitterTool     = splittertoolcomb,
    TrackCollection       = "Tracks",
    OutputTrackCollection = "Tracks_split")
    topSequence+=splittercomb
    if (printIdTrkDxAODConf):
        print splittercomb
        print splittercomb.properties()

    # Create xAOD::TrackParticles out of them
    from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
    InDetxAODSplitParticleCreatorTool = Trk__TrackParticleCreatorTool(name = "InDetSplitxAODParticleCreatorTool", 
                                                                      Extrapolator            = InDetExtrapolator,
                                                                      TrackSummaryTool        = InDetTrackSummaryToolSharedHits,
                                                                      ForceTrackSummaryUpdate = False,
                                                                      KeepParameters          = True)
    ToolSvc += InDetxAODSplitParticleCreatorTool
    # The following adds truth information, but needs further testing
    #include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
    #if isIdTrkDxAODSimulation:
    #    InDetSplitTracksTruth = ConfiguredInDetTrackTruth("Tracks_split",'SplitTrackDetailedTruth','SplitTrackTruth')

    xAODSplitTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg('InDetSplitTrackParticles')
    xAODSplitTrackParticleCnvAlg.xAODContainerName = 'InDetSplitTrackParticles'
    xAODSplitTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = 'InDetSplitTrackParticles'
    xAODSplitTrackParticleCnvAlg.TrackParticleCreator = InDetxAODSplitParticleCreatorTool
    xAODSplitTrackParticleCnvAlg.TrackContainerName = 'Tracks_split'
    xAODSplitTrackParticleCnvAlg.ConvertTrackParticles = False
    xAODSplitTrackParticleCnvAlg.ConvertTracks = True
    xAODSplitTrackParticleCnvAlg.AddTruthLink = False #isIdTrkDxAODSimulation
    if (isIdTrkDxAODSimulation):
        xAODSplitTrackParticleCnvAlg.TrackTruthContainerName = 'SplitTrackTruth'
    xAODSplitTrackParticleCnvAlg.PrintIDSummaryInfo = True
    topSequence += xAODSplitTrackParticleCnvAlg


#################
### Setup decorators tools
#################

if dumpTrtInfo:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import TRT_PrepDataToxAOD
    xAOD_TRT_PrepDataToxAOD = TRT_PrepDataToxAOD( name = "xAOD_TRT_PrepDataToxAOD")
    ## Content steering Properties (default value shown as comment)
    xAOD_TRT_PrepDataToxAOD.OutputLevel=INFO
    xAOD_TRT_PrepDataToxAOD.UseTruthInfo = dumpTruthInfo
    #xAOD_TRT_PrepDataToxAOD.WriteSDOs    = True

    topSequence += xAOD_TRT_PrepDataToxAOD
    if (printIdTrkDxAODConf):
        print xAOD_TRT_PrepDataToxAOD
        print xAOD_TRT_PrepDataToxAOD.properties()

if dumpSctInfo:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_PrepDataToxAOD
    xAOD_SCT_PrepDataToxAOD = SCT_PrepDataToxAOD( name = "xAOD_SCT_PrepDataToxAOD")
    ## Content steering Properties (default value shown as comment)
    xAOD_SCT_PrepDataToxAOD.OutputLevel=INFO
    xAOD_SCT_PrepDataToxAOD.UseTruthInfo        = dumpTruthInfo
    xAOD_SCT_PrepDataToxAOD.WriteRDOinformation = True
    #xAOD_SCT_PrepDataToxAOD.WriteSDOs           = True
    #xAOD_SCT_PrepDataToxAOD.WriteSiHits         = True # if available

    topSequence += xAOD_SCT_PrepDataToxAOD
    if (printIdTrkDxAODConf):
        print xAOD_SCT_PrepDataToxAOD
        print xAOD_SCT_PrepDataToxAOD.properties()

if dumpPixInfo:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD
    xAOD_PixelPrepDataToxAOD = PixelPrepDataToxAOD( name = "xAOD_PixelPrepDataToxAOD")
    ## Content steering Properties (default value shown as comment)
    xAOD_PixelPrepDataToxAOD.OutputLevel          = INFO
    xAOD_PixelPrepDataToxAOD.UseTruthInfo         = dumpTruthInfo
    xAOD_PixelPrepDataToxAOD.WriteRDOinformation  = InDetDxAODFlags.DumpPixelRdoInfo()
    xAOD_PixelPrepDataToxAOD.WriteNNinformation   = InDetDxAODFlags.DumpPixelNNInfo()
    #xAOD_PixelPrepDataToxAOD.WriteSDOs            = True
    #xAOD_PixelPrepDataToxAOD.WriteSiHits          = True # if available
    if InDetFlags.doSLHC():
        xAOD_PixelPrepDataToxAOD.WriteNNinformation=False

    topSequence += xAOD_PixelPrepDataToxAOD
    if (printIdTrkDxAODConf):
        print xAOD_PixelPrepDataToxAOD
        print xAOD_PixelPrepDataToxAOD.properties()


#################
### Setup Augmentation tools
#################
augmentationTools=[]

from AthenaCommon import CfgMgr

# Set up stream auditor
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'DecisionSvc'):
        svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True


# Add the TSOS augmentation tool to the derivation framework
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackStateOnSurfaceDecorator
DFTSOS = DerivationFramework__TrackStateOnSurfaceDecorator(name = "DFTrackStateOnSurfaceDecorator",
                                                          ContainerName = "InDetTrackParticles",
                                                          DecorationPrefix = prefixName,
                                                          StoreTRT   = dumpTrtInfo,
                                                          StoreSCT   = dumpSctInfo,
                                                          StorePixel = dumpPixInfo,
                                                          IsSimulation = isIdTrkDxAODSimulation,
                                                          OutputLevel = INFO)
if dumpTrtInfo:
    #Add tool to calculate TRT-based dEdx
    DFTSOS.TRT_ToT_dEdx = TRT_dEdx_Tool

ToolSvc += DFTSOS
augmentationTools+=[DFTSOS]
if (printIdTrkDxAODConf):
    print DFTSOS
    print DFTSOS.properties()

# If requested, decorate also split tracks (for cosmics)
if makeSplitTracks:
    DFTSOS_SplitTracks = DerivationFramework__TrackStateOnSurfaceDecorator(name = "DFSplitTracksTrackStateOnSurfaceDecorator",
                                                          ContainerName = "InDetSplitTrackParticles",
                                                          DecorationPrefix = prefixName,
                                                          StoreTRT   = dumpTrtInfo,
                                                          TrtMsosName = 'TRT_SplitTracks_MSOSs',
                                                          StoreSCT   = dumpSctInfo,
                                                          SctMsosName = 'SCT_SplitTracks_MSOSs',
                                                          StorePixel = dumpPixInfo,
                                                          PixelMsosName = 'Pixel_SplitTracks_MSOSs',
                                                          IsSimulation = isIdTrkDxAODSimulation,
                                                          OutputLevel = INFO)
    ToolSvc += DFTSOS_SplitTracks
    augmentationTools += [DFTSOS_SplitTracks]

# Add BS error augmentation tool
if dumpBytestreamErrors:
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EventInfoBSErrDecorator
    DFEI = DerivationFramework__EventInfoBSErrDecorator(name = "DFEventInfoBSErrDecorator",
                                                        ContainerName = "EventInfo",
                                                        DecorationPrefix = prefixName,
                                                        OutputLevel =INFO)
    ToolSvc += DFEI
    augmentationTools+=[DFEI]
    if (printIdTrkDxAODConf):
        print DFEI
        print DFEI.properties()

# Add Unassociated hits augmentation tool
if dumpUnassociatedHits:
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UnassociatedHitsGetterTool 
    unassociatedHitsGetterTool = DerivationFramework__UnassociatedHitsGetterTool (name = 'unassociatedHitsGetter',
                                                                                  TrackCollection = "Tracks",
                                                                                  PixelClusters = "PixelClusters",
                                                                                  SCTClusterContainer = "SCT_Clusters",
                                                                                  TRTDriftCircleContainer = "TRT_DriftCircles")
    ToolSvc += unassociatedHitsGetterTool
    if (printIdTrkDxAODConf):
        print unassociatedHitsGetterTool
        print unassociatedHitsGetterTool.properties()

    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UnassociatedHitsDecorator
    unassociatedHitsDecorator = DerivationFramework__UnassociatedHitsDecorator (name ='unassociatedHitsDecorator',
                                                                                UnassociatedHitsGetter = unassociatedHitsGetterTool,
                                                                                ContainerName = "EventInfo",
                                                                                DecorationPrefix = prefixName,
                                                                                OutputLevel =INFO)
    ToolSvc += unassociatedHitsDecorator
    augmentationTools+=[unassociatedHitsDecorator]
    if (printIdTrkDxAODConf):
        print unassociatedHitsDecorator
        print unassociatedHitsDecorator.properties()

# Add LArCollisionTime augmentation tool
if dumpLArCollisionTime:
    from LArCellRec.LArCollisionTimeGetter import LArCollisionTimeGetter
    from RecExConfig.ObjKeyStore           import cfgKeyStore
    # We can only do this if we have the cell container.
    if cfgKeyStore.isInInput ('CaloCellContainer', 'AllCalo'):
        LArCollisionTimeGetter (topSequence)

        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__LArCollisionTimeDecorator
        lArCollisionTimeDecorator = DerivationFramework__LArCollisionTimeDecorator (name ='lArCollisionTimeDecorator',
                                                                                    ContainerName = "EventInfo",
                                                                                    DecorationPrefix = prefixName+"LArCollTime_",
                                                                                    OutputLevel =INFO)
        ToolSvc += lArCollisionTimeDecorator
        augmentationTools+=[lArCollisionTimeDecorator]
        if (printIdTrkDxAODConf):
            print lArCollisionTimeDecorator
            print lArCollisionTimeDecorator.properties()


#====================================================================
# Skimming Tools
#====================================================================
skimmingTools = []

#minimumbiasTrig = '(L1_RD0_FILLED)'
#
#if not IsMonteCarlo:
#  from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
#  TrigSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "TrigSkimmingTool", expression = minimumbiasTrig)
#  ToolSvc += TrigSkimmingTool
#  skimmingTools.append(TrigSkimmingTool)
#  print "InDetDxAOD.py TrigSkimmingTool: ", TrigSkimmingTool

#====================================================================
# Thinning Tools
#====================================================================
thinningTools = []

# TrackParticles directly
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
IDTRKThinningTool = DerivationFramework__TrackParticleThinning(name = "IDTRKThinningTool",
                                                                 ThinningService         = "IDTRKThinningSvc",
                                                                 SelectionString         = thinTrackSelection,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                 ThinHitsOnTrack = thinHitsOnTrack)
ToolSvc += IDTRKThinningTool
thinningTools.append(IDTRKThinningTool)

#====================================================================
# Create the derivation Kernel and setup output stream
#====================================================================
# Add the derivation job to the top AthAlgSeqeuence
# DerivationJob is COMMON TO ALL DERIVATIONS
DerivationFrameworkJob = CfgMgr.AthSequencer("MySeq2")
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("DFTSOS_KERN",
                                                                       AugmentationTools = augmentationTools,
                                                                       SkimmingTools = skimmingTools,
                                                                       ThinningTools = thinningTools,
                                                                       OutputLevel = INFO)

topSequence += DerivationFrameworkJob
if (printIdTrkDxAODConf):
    print DerivationFrameworkJob
    print DerivationFrameworkJob.properties()

#################
### Steer output file content
#################
## Add service for metadata
ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool(
"TriggerMenuMetaDataTool" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]


## Steer output file
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_IDTRKVALIDStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_IDTRKVALIDStream )
IDTRKVALIDStream = MSMgr.NewPoolRootStream( streamName, fileName )
IDTRKVALIDStream.AcceptAlgs(["DFTSOS_KERN"])
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="IDTRKThinningSvc", outStreams=[evtStream] )

excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"

# Add generic event information
IDTRKVALIDStream.AddItem("xAOD::EventInfo#*")
IDTRKVALIDStream.AddItem("xAOD::EventAuxInfo#*")

# Add track particles collection and traclets (if available)
IDTRKVALIDStream.AddItem("xAOD::TrackParticleContainer#InDetTrackParticles")
IDTRKVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux."+excludedAuxData)
if InDetFlags.doTrackSegmentsPixel():
    IDTRKVALIDStream.AddItem("xAOD::TrackParticleContainer#InDetPixelTrackParticles")
    IDTRKVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#InDetPixelTrackParticlesAux."+excludedAuxData)
# Add split tracks, if requested
if makeSplitTracks:
    IDTRKVALIDStream.AddItem("xAOD::TrackParticleContainer#InDetSplitTrackParticles")
    IDTRKVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#InDetSplitTrackParticlesAux."+excludedAuxData)

# Add vertices
IDTRKVALIDStream.AddItem("xAOD::VertexContainer#PrimaryVertices")
IDTRKVALIDStream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex")

# Add links and measurements
IDTRKVALIDStream.AddItem("xAOD::TrackStateValidationContainer#*")
IDTRKVALIDStream.AddItem("xAOD::TrackStateValidationAuxContainer#*")
IDTRKVALIDStream.AddItem("xAOD::TrackMeasurementValidationContainer#*")
IDTRKVALIDStream.AddItem("xAOD::TrackMeasurementValidationAuxContainer#*")

# Add info about electrons and muons (are small containers)
IDTRKVALIDStream.AddItem("xAOD::MuonContainer#Muons")
IDTRKVALIDStream.AddItem("xAOD::MuonAuxContainer#MuonsAux.")
IDTRKVALIDStream.AddItem("xAOD::ElectronContainer#Electrons")
IDTRKVALIDStream.AddItem("xAOD::ElectronAuxContainer#ElectronsAux.")
IDTRKVALIDStream.AddItem("xAOD::TrackParticleContainer#GSFTrackParticles")
IDTRKVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#GSFTrackParticlesAux."+excludedAuxData)

# Add truth-related information
if dumpTruthInfo:
  IDTRKVALIDStream.AddItem("xAOD::TruthParticleContainer#*")
  IDTRKVALIDStream.AddItem("xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension")
  IDTRKVALIDStream.AddItem("xAOD::TruthVertexContainer#*")
  IDTRKVALIDStream.AddItem("xAOD::TruthVertexAuxContainer#*")
  IDTRKVALIDStream.AddItem("xAOD::TruthEventContainer#*")
  IDTRKVALIDStream.AddItem("xAOD::TruthEventAuxContainer#*")

# Add trigger information (including metadata)
if dumpTriggerInfo:
    IDTRKVALIDStream.AddMetaDataItem("xAOD::TriggerMenuContainer#TriggerMenu")
    IDTRKVALIDStream.AddMetaDataItem("xAOD::TriggerMenuAuxContainer#TriggerMenuAux.")
    IDTRKVALIDStream.AddItem("TileCellContainer#MBTSContainer")
    IDTRKVALIDStream.AddItem("xAOD::TrigDecision#xTrigDecision")
    IDTRKVALIDStream.AddItem("BCM_RDO_Container#BCM_RDOs")
    IDTRKVALIDStream.AddItem("xAOD::TrigNavigation#TrigNavigation")
    IDTRKVALIDStream.AddItem("xAOD::TrigConfKeys#TrigConfKeys")
    IDTRKVALIDStream.AddItem("HLT::HLTResult#HLTResult_HLT")
    IDTRKVALIDStream.AddItem("xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.")
    IDTRKVALIDStream.AddItem("xAOD::TrigNavigationAuxInfo#TrigNavigationAux.")

if (printIdTrkDxAODConf):
    print IDTRKVALIDStream
