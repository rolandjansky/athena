#################
### Steering options
#################
## Load common flags
from AthenaCommon.JobProperties import jobproperties as athCommonFlags

# Select active sub-systems
dumpPixInfo = True
dumpSctInfo = True
dumpTrtInfo = True

# Thin hits to store only the ones on-track
thinHitsOnTrack=True

# Thin track collection, if necessary
# Example (p_T > 1.0 GeV && delta_z0 < 5 mm):
# InDetTrackParticles.pt > 1*GeV && abs(DFCommonInDetTrackZ0AtPV) < 5.0
thinTrackSelection = "InDetTrackParticles.pt > 0.1*GeV"

# Bytestream errors (for sub-systems who have implemented it)
dumpBytestreamErrors=True

# Unassociated hits decorations
dumpUnassociatedHits=True

# Add LArCollisionTime augmentation tool
dumpLArCollisionTime=True

# Force to do not dump truth info if set to False
#  (otherwise determined by autoconf below)
dumpTruthInfo=True

# Saves partial trigger information in the output stream (none otherwise)
dumpTriggerInfo=True

# Print settings for main tools
printIdTrkDxAODConf=True

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

## Steer output file
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_IDNCBStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_IDNCBStream )
IDNCBStream = MSMgr.NewPoolRootStream( streamName, fileName )
IDNCBStream.AcceptAlgs(["DFIDNCB_KERN"])
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

#################
### Setup tools
#################
if dumpTrtInfo:
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
    TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
    ServiceMgr += TRTStrawNeighbourSvc

    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
    TRTCalibDBTool=TRT_CalDbTool(name="TRT_CalDbTool")


    from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import TRT_ToT_dEdx
    TRT_dEdx_Tool = TRT_ToT_dEdx(name="NCBTRT_ToT_dEdx")
    from InDetRecExample.TrackingCommon import getInDetTRT_LocalOccupancy
    TRT_dEdx_Tool.TRT_LocalOccupancyTool    = getInDetTRT_LocalOccupancy()
    ToolSvc += TRT_dEdx_Tool

#Setup charge->ToT back-conversion to restore ToT info as well
if dumpPixInfo: 
    from AthenaCommon.AlgSequence import AlgSequence 
    topSequence = AlgSequence() 
    from PixelCalibAlgs.PixelCalibAlgsConf import PixelChargeToTConversion 
    PixelChargeToTConversionSetter = PixelChargeToTConversion(name = "NCBPixelChargeToTConversionSetter") 
    topSequence += PixelChargeToTConversionSetter 
    if (printIdTrkDxAODConf):
        print(PixelChargeToTConversionSetter)
        print(PixelChargeToTConversionSetter.properties())

#Setup SCT extension efficiency algorithm if running pixel tracklets
#if InDetFlags.doTrackSegmentsPixel():
#    include ("SCTExtension/SCTExtensionAlg.py")

#################
### Setup decorators tools
#################

if dumpTrtInfo:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import TRT_PrepDataToxAOD
    xAOD_TRT_PrepDataToxAOD = TRT_PrepDataToxAOD( name = "NCBxAOD_TRT_PrepDataToxAOD")
    ## Content steering Properties (default value shown as comment)
    xAOD_TRT_PrepDataToxAOD.OutputLevel=INFO
    xAOD_TRT_PrepDataToxAOD.UseTruthInfo = dumpTruthInfo
    #xAOD_TRT_PrepDataToxAOD.WriteSDOs    = True

    topSequence += xAOD_TRT_PrepDataToxAOD
    if (printIdTrkDxAODConf):
        print(xAOD_TRT_PrepDataToxAOD)
        print(xAOD_TRT_PrepDataToxAOD.properties())

if dumpSctInfo:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_PrepDataToxAOD
    xAOD_SCT_PrepDataToxAOD = SCT_PrepDataToxAOD( name = "NCBxAOD_SCT_PrepDataToxAOD")
    ## Content steering Properties (default value shown as comment)
    xAOD_SCT_PrepDataToxAOD.OutputLevel=INFO
    xAOD_SCT_PrepDataToxAOD.UseTruthInfo        = dumpTruthInfo
    xAOD_SCT_PrepDataToxAOD.WriteRDOinformation = True
    #xAOD_SCT_PrepDataToxAOD.WriteSDOs           = True
    #xAOD_SCT_PrepDataToxAOD.WriteSiHits         = True # if available

    topSequence += xAOD_SCT_PrepDataToxAOD
    if (printIdTrkDxAODConf):
        print(xAOD_SCT_PrepDataToxAOD)
        print(xAOD_SCT_PrepDataToxAOD.properties())

if dumpPixInfo:
    import InDetRecExample.TrackingCommon as TrackingCommon
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD
    xAOD_PixelPrepDataToxAOD = PixelPrepDataToxAOD( name = "NCBxAOD_PixelPrepDataToxAOD",
                                                    ClusterSplitProbabilityName = TrackingCommon.combinedClusterSplitProbName())
    ## Content steering Properties (default value shown as comment)
    xAOD_PixelPrepDataToxAOD.OutputLevel          = INFO
    xAOD_PixelPrepDataToxAOD.UseTruthInfo         = dumpTruthInfo
    xAOD_PixelPrepDataToxAOD.WriteRDOinformation  = False
    xAOD_PixelPrepDataToxAOD.WriteNNinformation   = False
    #xAOD_PixelPrepDataToxAOD.WriteSDOs            = True
    #xAOD_PixelPrepDataToxAOD.WriteSiHits          = True # if available
    if InDetFlags.doSLHC():
        xAOD_PixelPrepDataToxAOD.WriteNNinformation=False

    topSequence += xAOD_PixelPrepDataToxAOD
    if (printIdTrkDxAODConf):
        print(xAOD_PixelPrepDataToxAOD)
        print(xAOD_PixelPrepDataToxAOD.properties())


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
DFIDNCB = DerivationFramework__TrackStateOnSurfaceDecorator(name = "DFTrackStateOnSurfaceDecorator",
                                                          ContainerName = "InDetTrackParticles",
                                                          DecorationPrefix = prefixName,
                                                          StoreTRT   = dumpTrtInfo,
                                                          StoreSCT   = dumpSctInfo,
                                                          StorePixel = dumpPixInfo,
                                                          IsSimulation = isIdTrkDxAODSimulation,
                                                          OutputLevel = INFO)

# this variable is redundant and remove from DerivationFrameworkInDet-00-00-56
#DFIDNCB.TRT_ToT_dEdx = TRT_dEdx_Tool
ToolSvc += DFIDNCB
augmentationTools+=[DFIDNCB]

if (printIdTrkDxAODConf):
    print(DFIDNCB)
    print(DFIDNCB.properties())

# Add BS error augmentation tool
if dumpBytestreamErrors:
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EventInfoBSErrDecorator
    DFEI = DerivationFramework__EventInfoBSErrDecorator(name = "DFIDNCBEventInfoBSErrDecorator",
                                                        ContainerName = "EventInfo",
                                                        DecorationPrefix = prefixName,
                                                        OutputLevel =INFO)
    ToolSvc += DFEI
    augmentationTools+=[DFEI]
    if (printIdTrkDxAODConf):
        print(DFEI)
        print(DFEI.properties())

# Add Unassociated hits augmentation tool
if dumpUnassociatedHits:
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UnassociatedHitsGetterTool 
    unassociatedHitsGetterTool = DerivationFramework__UnassociatedHitsGetterTool (name = 'NCBunassociatedHitsGetter',
                                                                                  TrackCollection = "Tracks",
                                                                                  PixelClusters = "PixelClusters",
                                                                                  SCTClusterContainer = "SCT_Clusters",
                                                                                  TRTDriftCircleContainer = "TRT_DriftCircles")
    ToolSvc += unassociatedHitsGetterTool
    if (printIdTrkDxAODConf):
        print(unassociatedHitsGetterTool)
        print(unassociatedHitsGetterTool.properties())

    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UnassociatedHitsDecorator
    unassociatedHitsDecorator = DerivationFramework__UnassociatedHitsDecorator (name ='NCBunassociatedHitsDecorator',
                                                                                UnassociatedHitsGetter = unassociatedHitsGetterTool,
                                                                                ContainerName = "EventInfo",
                                                                                DecorationPrefix = prefixName,
                                                                                OutputLevel =INFO)
    ToolSvc += unassociatedHitsDecorator
    augmentationTools+=[unassociatedHitsDecorator]

    if (printIdTrkDxAODConf):
        print(unassociatedHitsDecorator)
        print(unassociatedHitsDecorator.properties())

# Add LArCollisionTime augmentation tool
if dumpLArCollisionTime:
    from RecExConfig.ObjKeyStore           import cfgKeyStore
    # We can only do this if we have the cell container.
    if cfgKeyStore.isInInput ('CaloCellContainer', 'AllCalo'):

        from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__LArCollisionTimeDecorator
        lArCollisionTimeDecorator = DerivationFramework__LArCollisionTimeDecorator (name ='NCBlArCollisionTimeDecorator',
                                                                                    ContainerName = "EventInfo",
                                                                                    DecorationPrefix = prefixName+"LArCollTime_",
                                                                                    OutputLevel =INFO)
        ToolSvc += lArCollisionTimeDecorator
        augmentationTools+=[lArCollisionTimeDecorator]

        if (printIdTrkDxAODConf):
            print(lArCollisionTimeDecorator)
            print(lArCollisionTimeDecorator.properties())


#====================================================================
# Skimming Tools
#====================================================================
skimmingTools = []

#====================================================================
# Thinning Tools
#====================================================================

thinningTools = []

# TrackParticles directly
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
IDNCBThinningTool = DerivationFramework__TrackParticleThinning(name = "IDNCBThinningTool",
                                                                 StreamName              = streamName,
                                                                 SelectionString         = thinTrackSelection,
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                 ThinHitsOnTrack = thinHitsOnTrack)
ToolSvc += IDNCBThinningTool
thinningTools.append(IDNCBThinningTool)


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackMeasurementThinning

thin_sct = "SCT_Clusters.bec != 0"
IDNCBThinningTool_sct = DerivationFramework__TrackMeasurementThinning( name = "IDNCBSCTThinningTool",
    StreamName = streamName,
    SelectionString = thin_sct,
    TrackMeasurementValidationKey = "SCT_Clusters")
ToolSvc += IDNCBThinningTool_sct
thinningTools.append(IDNCBThinningTool_sct)

thin_pix = "PixelClusters.bec != 0"
IDNCBThinningTool_pix = DerivationFramework__TrackMeasurementThinning( name = "IDNCBPIXThinningTool",
    StreamName = streamName,
    SelectionString = thin_pix,
    TrackMeasurementValidationKey = "PixelClusters")
ToolSvc += IDNCBThinningTool_pix
thinningTools.append(IDNCBThinningTool_pix)

thin_trt = "(TRT_DriftCircles.bec != -2) && (TRT_DriftCircles.layer > 5)"
IDNCBThinningTool_trt = DerivationFramework__TrackMeasurementThinning( name = "IDNCBTRTThinningTool",
    StreamName = streamName,
    SelectionString = thin_trt,
    TrackMeasurementValidationKey = "TRT_DriftCircles")
ToolSvc += IDNCBThinningTool_trt
thinningTools.append(IDNCBThinningTool_trt)


#====================================================================
# Create the derivation Kernel and setup output stream
#====================================================================
# Add the derivation job to the top AthAlgSeqeuence
# DerivationJob is COMMON TO ALL DERIVATIONS
DerivationFrameworkJob = CfgMgr.AthSequencer("IDNCBSeq")
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("DFIDNCB_KERN",
                                                                       AugmentationTools = augmentationTools,
                                                                       SkimmingTools = skimmingTools,
                                                                       ThinningTools = thinningTools,
                                                                       OutputLevel = INFO)

topSequence += DerivationFrameworkJob
if (printIdTrkDxAODConf):
    print(DerivationFrameworkJob)
    print(DerivationFrameworkJob.properties())

#################
### Steer output file content
#################
## Add service for metadata
ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool(
"TriggerMenuMetaDataTool" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]



excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"
excludedTRTData = "-T0.-TRTboard.-TRTchip.-bitPattern.-driftTimeToTCorrection.-driftTimeHTCorrection.-highThreshold.-strawnumber"
excludedSCTData = "-detectorElementID.-hitsInThirdTimeBin.-rdo_groupsize"

# Add generic event information
IDNCBStream.AddItem("xAOD::EventInfo#*")
IDNCBStream.AddItem("xAOD::EventAuxInfo#*")

#SKC Add jets!
IDNCBStream.AddItem("xAOD::JetContainer#AntiKt4EMTopoJets")
IDNCBStream.AddItem("xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.")
IDNCBStream.AddItem("xAOD::JetContainer#AntiKt4LCTopoJets")
IDNCBStream.AddItem("xAOD::JetAuxContainer#AntiKt4LCTopoJetsAux.")
IDNCBStream.AddItem("xAOD::CaloClusterContainer#CaloCalTopoClusters")
IDNCBStream.AddItem("xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.")
IDNCBStream.AddItem("xAOD::MuonSegmentContainer#MuonSegments")
IDNCBStream.AddItem("xAOD::MuonSegmentAuxContainer#MuonSegmentsAux.")

IDNCBStream.AddItem("xAOD::MuonSegmentContainer#NCB_MuonSegments")
IDNCBStream.AddItem("xAOD::MuonSegmentAuxContainer#NCB_MuonSegmentsAux.")

IDNCBStream.AddItem("xAOD::TrackMeasurementValidationContainer#*")
IDNCBStream.AddItem("xAOD::TrackMeasurementValidationAuxContainer#*")

# Add vertices
IDNCBStream.AddItem("xAOD::VertexContainer#PrimaryVertices")
IDNCBStream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV")

# Add info about electrons and muons (are small containers)
IDNCBStream.AddItem("xAOD::MuonContainer#Muons")
IDNCBStream.AddItem("xAOD::MuonAuxContainer#MuonsAux.")
IDNCBStream.AddItem("xAOD::ElectronContainer#Electrons")
IDNCBStream.AddItem("xAOD::ElectronAuxContainer#ElectronsAux.")
IDNCBStream.AddItem("xAOD::TrackParticleContainer#GSFTrackParticles")
IDNCBStream.AddItem("xAOD::TrackParticleAuxContainer#GSFTrackParticlesAux."+excludedAuxData)

# Add truth-related information
if dumpTruthInfo:
  IDNCBStream.AddItem("xAOD::TruthParticleContainer#*")
  IDNCBStream.AddItem("xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension")
  IDNCBStream.AddItem("xAOD::TruthVertexContainer#*")
  IDNCBStream.AddItem("xAOD::TruthVertexAuxContainer#*")
  IDNCBStream.AddItem("xAOD::TruthEventContainer#*")
  IDNCBStream.AddItem("xAOD::TruthEventAuxContainer#*")

#SKC---always include BCM information!
IDNCBStream.AddItem("BCM_RDO_Container#BCM_RDOs")

#PJL - Slim the SCT and TRT data
IDNCBStream.AddItem("xAOD::TrackMeasurementValidationAuxContainer#TRT_DriftCirclesAux."+excludedTRTData)
IDNCBStream.AddItem("xAOD::TrackMeasurementValidationAuxContainer#SCT_ClustersAux."+excludedSCTData)

if dumpTriggerInfo:
    IDNCBStream.AddMetaDataItem("xAOD::TriggerMenuContainer#TriggerMenu")
    IDNCBStream.AddMetaDataItem("xAOD::TriggerMenuAuxContainer#TriggerMenuAux.")
    IDNCBStream.AddItem("TileCellContainer#MBTSContainer")
    IDNCBStream.AddItem("xAOD::TrigDecision#xTrigDecision")
    IDNCBStream.AddItem("xAOD::TrigNavigation#TrigNavigation")
    IDNCBStream.AddItem("xAOD::TrigConfKeys#TrigConfKeys")
    IDNCBStream.AddItem("HLT::HLTResult#HLTResult_HLT")
    IDNCBStream.AddItem("xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.")
    IDNCBStream.AddItem("xAOD::TrigNavigationAuxInfo#TrigNavigationAux.")

if (printIdTrkDxAODConf):
    print(IDNCBStream)
