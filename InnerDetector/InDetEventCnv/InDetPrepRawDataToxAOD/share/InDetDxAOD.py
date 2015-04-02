#Steering options
idDxAOD_doPix=True
idDxAOD_doSct=True
idDxAOD_doTrt=True
if InDetFlags.doSLHC():
    idDxAOD_doTrt=False

DumpTruthInfo = True and globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool"

doReadBS=False

#Setup tools
if idDxAOD_doTrt:
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
    TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
    ServiceMgr += TRTStrawNeighbourSvc

    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
    TRTCalibDBSvc=TRT_CalDbSvc()
    ServiceMgr += TRTCalibDBSvc

if idDxAOD_doPix: 
    from AthenaCommon.AlgSequence import AlgSequence 
    topSequence = AlgSequence() 
    from PixelCalibAlgs.PixelCalibAlgsConf import PixelChargeToTConversion 
    PixelChargeToTConversionSetter = PixelChargeToTConversion(name = "PixelChargeToTConversionSetter") 
    topSequence += PixelChargeToTConversionSetter 

#Setup decorators tools
if idDxAOD_doTrt:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import TRT_PrepDataToxAOD
    xAOD_TRT_PrepDataToxAOD = TRT_PrepDataToxAOD( name = "xAOD_TRT_PrepDataToxAOD")
    xAOD_TRT_PrepDataToxAOD.OutputLevel=INFO
    xAOD_TRT_PrepDataToxAOD.UseTruthInfo=DumpTruthInfo
    print "Add TRT xAOD TrackMeasurementValidation:"
    print xAOD_TRT_PrepDataToxAOD
    topSequence += xAOD_TRT_PrepDataToxAOD

if idDxAOD_doSct:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_PrepDataToxAOD
    xAOD_SCT_PrepDataToxAOD = SCT_PrepDataToxAOD( name = "xAOD_SCT_PrepDataToxAOD")
    xAOD_SCT_PrepDataToxAOD.OutputLevel=INFO
    xAOD_SCT_PrepDataToxAOD.UseTruthInfo=DumpTruthInfo
    print "Add SCT xAOD TrackMeasurementValidation:"
    print xAOD_SCT_PrepDataToxAOD
    topSequence += xAOD_SCT_PrepDataToxAOD

if idDxAOD_doPix:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD
    xAOD_PixelPrepDataToxAOD = PixelPrepDataToxAOD( name = "xAOD_PixelPrepDataToxAOD")
    xAOD_PixelPrepDataToxAOD.OutputLevel=INFO
    xAOD_PixelPrepDataToxAOD.UseTruthInfo=DumpTruthInfo
    if InDetFlags.doSLHC():
        xAOD_PixelPrepDataToxAOD.WriteNNinformation=False
    print "Add Pixel xAOD TrackMeasurementValidation:"
    print xAOD_PixelPrepDataToxAOD
    topSequence += xAOD_PixelPrepDataToxAOD


#Setup derivation framework
from AthenaCommon import CfgMgr

# DerivationJob is COMMON TO ALL DERIVATIONS
DerivationFrameworkJob = CfgMgr.AthSequencer("MySeq2")

# Set up stream auditor
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'DecisionSvc'):
        svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True


#Add  the TSOS decoration  
prefixName = "IDDET1"
augmentationTools=[]

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackStateOnSurfaceDecorator
DFTSOS = DerivationFramework__TrackStateOnSurfaceDecorator(name = "DFTrackStateOnSurfaceDecorator",
                                                          ContainerName = "InDetTrackParticles",
                                                          DecorationPrefix = prefixName,
                                                          StoreTRT   = idDxAOD_doTrt,
                                                          StoreSCT   = idDxAOD_doSct,
                                                          StorePixel = idDxAOD_doPix,
                                                          OutputLevel =INFO)
ToolSvc += DFTSOS
augmentationTools+=[DFTSOS]

#Add BS error information
if doReadBS:
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EventInfoBSErrDecorator
    DFEI = DerivationFramework__EventInfoBSErrDecorator(name = "DFEventInfoBSErrDecorator",
                                                        ContainerName = "EventInfo",
                                                        DecorationPrefix = prefixName,
                                                        OutputLevel =INFO)
    ToolSvc += DFEI
    augmentationTools+=[DFEI]

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("DFTSOS_KERN",
                                                                        AugmentationTools = augmentationTools,
                                                                        OutputLevel =INFO)

topSequence += DerivationFrameworkJob

# Tell it what information you want added to the file
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_IDTRKVALIDStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_IDTRKVALIDStream )
IDTRKVALIDStream = MSMgr.NewPoolRootStream( streamName, fileName )
excludedAuxData = ".-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"
IDTRKVALIDStream.AddItem("xAOD::EventInfo#*")
IDTRKVALIDStream.AddItem("xAOD::EventAuxInfo#*")
IDTRKVALIDStream.AddItem("xAOD::TrackParticleContainer#InDetTrackParticles")
IDTRKVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux"+excludedAuxData)
IDTRKVALIDStream.AddItem("xAOD::VertexContainer#PrimaryVertices")
IDTRKVALIDStream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex")
IDTRKVALIDStream.AddItem("xAOD::TrackStateValidationContainer#*")
IDTRKVALIDStream.AddItem("xAOD::TrackStateValidationAuxContainer#*")
IDTRKVALIDStream.AddItem("xAOD::TrackMeasurementValidationContainer#*")
IDTRKVALIDStream.AddItem("xAOD::TrackMeasurementValidationAuxContainer#*")
if DumpTruthInfo:
  IDTRKVALIDStream.AddItem("xAOD::TruthParticleContainer#*")
  IDTRKVALIDStream.AddItem("xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension")
  IDTRKVALIDStream.AddItem("xAOD::TruthVertexContainer#*")
  IDTRKVALIDStream.AddItem("xAOD::TruthVertexAuxContainer#*")
  IDTRKVALIDStream.AddItem("xAOD::TruthEventContainer#*")
  IDTRKVALIDStream.AddItem("xAOD::TruthEventAuxContainer#*")
