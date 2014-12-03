#Steering options
idDxAOD_doPix=True
idDxAOD_doSct=True
idDxAOD_doTrt=True

DumpTruthInfo = True and globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool"

#Setup tools
if idDxAOD_doTrt:
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawNeighbourSvc
    TRTStrawNeighbourSvc=TRT_StrawNeighbourSvc()
    ServiceMgr += TRTStrawNeighbourSvc

    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
    TRTCalibDBSvc=TRT_CalDbSvc()
    ServiceMgr += TRTCalibDBSvc

#Setup decorators tools
if idDxAOD_doTrt:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import TRT_PrepDataToxAOD
    xAOD_TRT_PrepDataToxAOD = TRT_PrepDataToxAOD( name = "xAOD_TRT_PrepDataToxAOD")
    xAOD_TRT_PrepDataToxAOD.OutputLevel=INFO
    xAOD_TRT_PrepDataToxAOD.UseTruthInfo=DumpTruthInfo
    print "Add TRT xAOD PrepRawData:"
    print xAOD_TRT_PrepDataToxAOD
    topSequence += xAOD_TRT_PrepDataToxAOD

if idDxAOD_doSct:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_PrepDataToxAOD
    xAOD_SCT_PrepDataToxAOD = SCT_PrepDataToxAOD( name = "xAOD_SCT_PrepDataToxAOD")
    xAOD_SCT_PrepDataToxAOD.OutputLevel=INFO
    xAOD_SCT_PrepDataToxAOD.UseTruthInfo=DumpTruthInfo
    print "Add SCT xAOD PrepRawData:"
    print xAOD_SCT_PrepDataToxAOD
    topSequence += xAOD_SCT_PrepDataToxAOD

if idDxAOD_doPix:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD
    xAOD_PixelPrepDataToxAOD = PixelPrepDataToxAOD( name = "xAOD_PixelPrepDataToxAOD")
    xAOD_PixelPrepDataToxAOD.OutputLevel=INFO
    xAOD_PixelPrepDataToxAOD.UseTruthInfo=DumpTruthInfo
    print "Add SCT xAOD PrepRawData:"
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
streamName = "IDDET1"

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackStateOnSurfaceDecorator
DFTSOS = DerivationFramework__TrackStateOnSurfaceDecorator(name = "DFTrackStateOnSurfaceDecorator",
                                                          ContainerName = "InDetTrackParticles",
                                                          DecorationPrefix = streamName,
                                                          StoreTRT   = idDxAOD_doTrt,
                                                          StoreSCT   = idDxAOD_doSct,
																													StorePixel = idDxAOD_doPix,
                                                          OutputLevel =INFO)




ToolSvc += DFTSOS

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("DFTSOS_KERN",
                                                                        AugmentationTools = [DFTSOS],
                                                                        OutputLevel =INFO)

topSequence += DerivationFrameworkJob

# Tell it what information you want added to the file
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
fileName   = "InDetDxAOD.pool.root"
TestStream = MSMgr.NewPoolRootStream( streamName, fileName)
TestStream.AddItem("xAOD::EventInfo#*")
TestStream.AddItem("xAOD::EventAuxInfo#*")
TestStream.AddItem("xAOD::TrackParticleContainer#InDetTrackParticles")
TestStream.AddItem("xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.")
TestStream.AddItem("xAOD::VertexContainer#PrimaryVertices")
TestStream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.")
TestStream.AddItem("xAOD::PrepRawDataContainer#*")
TestStream.AddItem("xAOD::PrepRawDataAuxContainer#*")
TestStream.AddItem("xAOD::MeasurementStateOnSurfaceContainer#*")
TestStream.AddItem("xAOD::MeasurementStateOnSurfaceAuxContainer#*")

