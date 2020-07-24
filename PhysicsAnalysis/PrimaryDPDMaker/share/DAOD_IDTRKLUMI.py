#################
### Steering options
#################
## Load common flags
from AthenaCommon.JobProperties import jobproperties as athCommonFlags

# Select active sub-systems
dumpPixInfo=True
dumpSctInfo=False

# Bytestream errors (for sub-systems who have implemented it)
dumpBytestreamErrors=False

# Unassociated hits decorations
dumpUnassociatedHits=False

# Add LArCollisionTime augmentation tool
dumpLArCollisionTime=False

# Force to do not dump truth info if set to False
#  (otherwise determined by autoconf below)
dumpTruthInfo=True

# Saves partial trigger information in the output stream (none otherwise)
dumpTriggerInfo=True

# Print settings for main tools
printIdTrkDxAODConf = True


## Autoconfiguration adjustements
isIdTrkDxAODSimulation = False
if (globalflags.DataSource == 'geant4'):
    isIdTrkDxAODSimulation = True

if ( 'dumpTruthInfo' in dir() ):
    dumpTruthInfo = dumpTruthInfo and isIdTrkDxAODSimulation

## Other settings
# Prefix for decoration, if any
prefixName = ""

## More fine-tuning available for each tool/alg below (default value shown)

#################
### Setup tools
#################

#Setup charge->ToT back-conversion to restore ToT info as well
if dumpPixInfo: 
    from AthenaCommon.AlgSequence import AlgSequence 
    topSequence = AlgSequence() 
    from PixelCalibAlgs.PixelCalibAlgsConf import PixelChargeToTConversion 
    PixelChargeToTConversionSetter = PixelChargeToTConversion(name = "PixelChargeToTConversionSetter") 
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


if dumpSctInfo:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_PrepDataToxAOD
    xAOD_SCT_PrepDataToxAOD = SCT_PrepDataToxAOD( name = "xAOD_SCT_PrepDataToxAOD")
    ## Content steering Properties (default value shown as comment)
    xAOD_SCT_PrepDataToxAOD.OutputLevel=INFO
    xAOD_SCT_PrepDataToxAOD.UseTruthInfo        = dumpTruthInfo
    xAOD_SCT_PrepDataToxAOD.WriteRDOinformation = False
    #xAOD_SCT_PrepDataToxAOD.WriteSDOs           = True
    #xAOD_SCT_PrepDataToxAOD.WriteSiHits         = True # if available

    topSequence += xAOD_SCT_PrepDataToxAOD
    if (printIdTrkDxAODConf):
        print(xAOD_SCT_PrepDataToxAOD)
        print(xAOD_SCT_PrepDataToxAOD.properties())

if dumpPixInfo:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD
    xAOD_PixelPrepDataToxAOD = PixelPrepDataToxAOD( name = "xAOD_PixelPrepDataToxAOD")
    ## Content steering Properties (default value shown as comment)
    xAOD_PixelPrepDataToxAOD.OutputLevel          = INFO
    xAOD_PixelPrepDataToxAOD.UseTruthInfo         = dumpTruthInfo
    xAOD_PixelPrepDataToxAOD.WriteRDOinformation  = False
    xAOD_PixelPrepDataToxAOD.WriteNNinformation   = False
    xAOD_PixelPrepDataToxAOD.WriteSDOs            = True
    xAOD_PixelPrepDataToxAOD.WriteSiHits          = False # if available


    topSequence += xAOD_PixelPrepDataToxAOD
    if (printIdTrkDxAODConf):
        print(xAOD_PixelPrepDataToxAOD)
        print(xAOD_PixelPrepDataToxAOD.properties())


#################
### Setup derivation framework
#################
from AthenaCommon import CfgMgr

# DerivationJob is COMMON TO ALL DERIVATIONS
DerivationFrameworkJob = CfgMgr.AthSequencer("MySeq2")

# Set up stream auditor
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'DecisionSvc'):
        svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True


# Add the TSOS augmentation tool to the derivation framework
augmentationTools=[]

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackStateOnSurfaceDecorator
DFTSOS = DerivationFramework__TrackStateOnSurfaceDecorator(name = "DFTrackStateOnSurfaceDecorator",
                                                          ContainerName = "InDetTrackParticles",
                                                          DecorationPrefix = prefixName,
                                                          StoreTRT   = False,
                                                          StoreSCT   = dumpSctInfo,
                                                          StorePixel = dumpPixInfo,
                                                          IsSimulation = isIdTrkDxAODSimulation,
                                                          OutputLevel = INFO)

ToolSvc += DFTSOS
augmentationTools+=[DFTSOS]
if (printIdTrkDxAODConf):
    print(DFTSOS)
    print(DFTSOS.properties())


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
        print(DFEI)
        print(DFEI.properties())

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
        print(unassociatedHitsGetterTool)
        print(unassociatedHitsGetterTool.properties())

    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UnassociatedHitsDecorator
    unassociatedHitsDecorator = DerivationFramework__UnassociatedHitsDecorator (name ='unassociatedHitsDecorator',
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
            print(lArCollisionTimeDecorator)
            print(lArCollisionTimeDecorator.properties())



# Add the derivation job to the top AthAlgSeqeuence
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("DFTSOS_KERN",
                                                                        AugmentationTools = augmentationTools,
                                                                        OutputLevel =INFO)

topSequence += DerivationFrameworkJob
if (printIdTrkDxAODConf):
    print(DerivationFrameworkJob)
    print(DerivationFrameworkJob.properties())

#################
### Steer output file content
#################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_IDTRKLUMIStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_IDTRKLUMIStream )
IDTRKLUMIStream = MSMgr.NewPoolRootStream( streamName, fileName )
excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"

# Add generic event information
IDTRKLUMIStream.AddItem("xAOD::EventInfo#*")
IDTRKLUMIStream.AddItem("xAOD::EventAuxInfo#*")

# Add track particles collection and traclets (if available)
IDTRKLUMIStream.AddItem("xAOD::TrackParticleContainer#InDetTrackParticles")
IDTRKLUMIStream.AddItem("xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux."+excludedAuxData)
if InDetFlags.doTrackSegmentsPixel():
    IDTRKLUMIStream.AddItem("xAOD::TrackParticleContainer#InDetPixelTrackParticles")
    IDTRKLUMIStream.AddItem("xAOD::TrackParticleAuxContainer#InDetPixelTrackParticlesAux."+excludedAuxData)

# Add vertices
IDTRKLUMIStream.AddItem("xAOD::VertexContainer#PrimaryVertices")
IDTRKLUMIStream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV")

# Add links and measurements
IDTRKLUMIStream.AddItem("xAOD::TrackStateValidationContainer#*")
IDTRKLUMIStream.AddItem("xAOD::TrackStateValidationAuxContainer#*")
IDTRKLUMIStream.AddItem("xAOD::TrackMeasurementValidationContainer#*")
IDTRKLUMIStream.AddItem("xAOD::TrackMeasurementValidationAuxContainer#*")


# Add truth-related information
if dumpTruthInfo:
  IDTRKLUMIStream.AddItem("xAOD::TruthParticleContainer#*")
  IDTRKLUMIStream.AddItem("xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension")
  IDTRKLUMIStream.AddItem("xAOD::TruthVertexContainer#*")
  IDTRKLUMIStream.AddItem("xAOD::TruthVertexAuxContainer#*")
  IDTRKLUMIStream.AddItem("xAOD::TruthEventContainer#*")
  IDTRKLUMIStream.AddItem("xAOD::TruthEventAuxContainer#*")
  IDTRKLUMIStream.AddItem("xAOD::TruthPileupEventContainer#*")
  IDTRKLUMIStream.AddItem("xAOD::TruthPileupEventAuxContainer#*")

# Add trigger information
if dumpTriggerInfo:
    IDTRKLUMIStream.AddItem("xAOD::TrigT2MbtsBitsContainer#*")
    IDTRKLUMIStream.AddItem("xAOD::TrigDecision#xTrigDecision")
    IDTRKLUMIStream.AddItem("BCM_RDO_Container#BCM_RDOs")
    IDTRKLUMIStream.AddItem("xAOD::TrigNavigation#TrigNavigation")
    IDTRKLUMIStream.AddItem("xAOD::TrigConfKeys#TrigConfKeys")
    IDTRKLUMIStream.AddItem("HLT::HLTResult#HLTResult_HLT")
    IDTRKLUMIStream.AddItem("xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.")
    IDTRKLUMIStream.AddItem("xAOD::TrigNavigationAuxInfo#TrigNavigationAux.")
    IDTRKLUMIStream.AddItem("xAOD::TrigT2MbtsBits#HLT_T2Mbts")
    IDTRKLUMIStream.AddItem("xAOD::TrigT2MbtsBitsAuxContainer#HLT_T2MbtsAux.")
    IDTRKLUMIStream.AddItem("xAOD::TrigT2MbtsBitsAuxContainer#HLT_xAOD__TrigT2MbtsBitsContainer_T2MbtsAux.")

if (printIdTrkDxAODConf):
    print(IDTRKLUMIStream)
