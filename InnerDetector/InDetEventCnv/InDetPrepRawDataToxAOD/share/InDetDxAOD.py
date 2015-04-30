#################
### Steering options
#################
# Select active sub-systems
dumpPixInfo=True
dumpSctInfo=True
dumpTrtInfo=False

# Bytestream errors (for sub-systems who have implemented it)
dumpBytestreamErrors=True

# Force to do not dump truth info if set to False
#  (otherwise determined by autoconf below)
dumpTruthInfo=True

# Print settings for main tools
printIdTrkDxAODConf = True

## Autoconfiguration adjustements
isIdTrkDxAODSimulation = False
if (globalflags.DataSource == 'geant4'):
    isIdTrkDxAODSimulation = True

if ( 'dumpTruthInfo' in dir() ):
    dumpTruthInfo = dumpTruthInfo and isIdTrkDxAODSimulation

print "isIdTrkDxAODSimulation=", isIdTrkDxAODSimulation
print "dumpTruthInfo=", dumpTruthInfo

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

if dumpPixInfo: 
    from AthenaCommon.AlgSequence import AlgSequence 
    topSequence = AlgSequence() 
    from PixelCalibAlgs.PixelCalibAlgsConf import PixelChargeToTConversion 
    PixelChargeToTConversionSetter = PixelChargeToTConversion(name = "PixelChargeToTConversionSetter") 
    topSequence += PixelChargeToTConversionSetter 
    if (printIdTrkDxAODConf):
        print PixelChargeToTConversionSetter
        print PixelChargeToTConversionSetter.properties()


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
    xAOD_SCT_PrepDataToxAOD.WriteRDOinformation = False
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
    xAOD_PixelPrepDataToxAOD.WriteRDOinformation  = False
    xAOD_PixelPrepDataToxAOD.WriteNNinformation   = False
    #xAOD_PixelPrepDataToxAOD.WriteSDOs            = True
    #xAOD_PixelPrepDataToxAOD.WriteSiHits          = True # if available
    if InDetFlags.doSLHC():
        xAOD_PixelPrepDataToxAOD.WriteNNinformation=False

    topSequence += xAOD_PixelPrepDataToxAOD
    if (printIdTrkDxAODConf):
        print xAOD_PixelPrepDataToxAOD
        print xAOD_PixelPrepDataToxAOD.properties()


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

# Add the derivation job to the top AthAlgSeqeuence
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("DFTSOS_KERN",
                                                                        AugmentationTools = augmentationTools,
                                                                        OutputLevel =INFO)

topSequence += DerivationFrameworkJob
if (printIdTrkDxAODConf):
    print DerivationFrameworkJob
    print DerivationFrameworkJob.properties()

#################
### Steer output file content
#################
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_IDTRKVALIDStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_IDTRKVALIDStream )
IDTRKVALIDStream = MSMgr.NewPoolRootStream( streamName, fileName )
excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"
# Add generic event information
IDTRKVALIDStream.AddItem("xAOD::EventInfo#*")
IDTRKVALIDStream.AddItem("xAOD::EventAuxInfo#*")
# Add track particles collection
IDTRKVALIDStream.AddItem("xAOD::TrackParticleContainer#InDetTrackParticles")
IDTRKVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux."+excludedAuxData)
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

if (printIdTrkDxAODConf):
    print IDTRKVALIDStream
