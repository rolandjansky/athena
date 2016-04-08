#################
### Steering options
#################
## Load common flags
from AthenaCommon.JobProperties import jobproperties as athCommonFlags

# Bytestream errors (for sub-systems who have implemented it)
dumpBytestreamErrors=True

# Unassociated hits decorations
dumpUnassociatedHits=True

# Force to do not dump truth info if set to False
#  (otherwise determined by autoconf below)
dumpTruthInfo=True

# Saves partial trigger information in the output stream (none otherwise)
dumpTriggerInfo=True

# Print settings for main tools
printSctDxAODConf = True

## Autoconfiguration adjustements
isSctDxAODSimulation = False
if (globalflags.DataSource == 'geant4'):
    isSctDxAODSimulation = True

if ( 'dumpTruthInfo' in dir() ):
    dumpTruthInfo = dumpTruthInfo and isSctDxAODSimulation

## Other settings
# Prefix for decoration, if any
prefixName = ""

## More fine-tuning available for each tool/alg below (default value shown)


#################
### Setup decorators tools
#################

from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_PrepDataToxAOD
xAOD_SCT_PrepDataToxAOD = SCT_PrepDataToxAOD( name = "xAOD_SCT_PrepDataToxAOD")
    ## Content steering Properties (default value shown as comment)
xAOD_SCT_PrepDataToxAOD.OutputLevel=INFO
xAOD_SCT_PrepDataToxAOD.UseTruthInfo        = dumpTruthInfo
xAOD_SCT_PrepDataToxAOD.WriteRDOinformation = True
    #xAOD_SCT_PrepDataToxAOD.WriteSDOs           = True
    #xAOD_SCT_PrepDataToxAOD.WriteSiHits         = True # if available

topSequence += xAOD_SCT_PrepDataToxAOD
if (printSctDxAODConf):
    print xAOD_SCT_PrepDataToxAOD
    print xAOD_SCT_PrepDataToxAOD.properties()

from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_RawDataToxAOD
xAOD_SCT_RawDataToxAOD = SCT_RawDataToxAOD( name = "xAOD_SCT_RawDataToxAOD")
xAOD_SCT_RawDataToxAOD.OutputLevel = INFO
topSequence += xAOD_SCT_RawDataToxAOD
if printSctDxAODConf:
    print xAOD_SCT_RawDataToxAOD
    print xAOD_SCT_RawDataToxAOD.properties()


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
                                                          StoreSCT   = True,
                                                          StorePixel = False,
                                                          IsSimulation = isSctDxAODSimulation,
                                                          OutputLevel = INFO)
ToolSvc += DFTSOS
augmentationTools+=[DFTSOS]


# Add BS error augmentation tool
if dumpBytestreamErrors:
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EventInfoBSErrDecorator
    DFEI = DerivationFramework__EventInfoBSErrDecorator(name = "DFEventInfoBSErrDecorator",
                                                        ContainerName = "EventInfo",
                                                        DecorationPrefix = prefixName,
                                                        OutputLevel =INFO)
    ToolSvc += DFEI
    augmentationTools+=[DFEI]
    if (printSctDxAODConf):
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
    if (printSctDxAODConf):
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
    if (printSctDxAODConf):
        print unassociatedHitsDecorator
        print unassociatedHitsDecorator.properties()


# Add the derivation job to the top AthAlgSeqeuence
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("DFTSOS_KERN",
                                                                        AugmentationTools = augmentationTools,
                                                                        OutputLevel =INFO)

topSequence += DerivationFrameworkJob
if (printSctDxAODConf):
    print DerivationFrameworkJob
    print DerivationFrameworkJob.properties()

#################
### Steer output file content
#################

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_SCTVALIDStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_SCTVALIDStream )
SCTVALIDStream = MSMgr.NewPoolRootStream( streamName, fileName )
excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"

# Add generic event information
SCTVALIDStream.AddItem("xAOD::EventInfo#*")
SCTVALIDStream.AddItem("xAOD::EventAuxInfo#*")

# Add track particles collection and traclets (if available)
SCTVALIDStream.AddItem("xAOD::TrackParticleContainer#InDetTrackParticles")
SCTVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux."+excludedAuxData)

# Add vertices
SCTVALIDStream.AddItem("xAOD::VertexContainer#PrimaryVertices")
SCTVALIDStream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex")

# Add links and measurements
SCTVALIDStream.AddItem("xAOD::TrackStateValidationContainer#*")
SCTVALIDStream.AddItem("xAOD::TrackStateValidationAuxContainer#*")
SCTVALIDStream.AddItem("xAOD::TrackMeasurementValidationContainer#*")
SCTVALIDStream.AddItem("xAOD::TrackMeasurementValidationAuxContainer#*")

# Add info about electrons and muons (are small containers)
SCTVALIDStream.AddItem("xAOD::MuonContainer#Muons")
SCTVALIDStream.AddItem("xAOD::MuonAuxContainer#MuonsAux.")
SCTVALIDStream.AddItem("xAOD::ElectronContainer#Electrons")
SCTVALIDStream.AddItem("xAOD::ElectronAuxContainer#ElectronsAux.")
SCTVALIDStream.AddItem("xAOD::TrackParticleContainer#GSFTrackParticles")
SCTVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#GSFTrackParticlesAux."+excludedAuxData)

# Add SCT raw-data-objects
SCTVALIDStream.AddItem("xAOD::SCTRawHitValidationContainer#*")
SCTVALIDStream.AddItem("xAOD::SCTRawHitValidationAuxContainer#*")

# Add truth-related information
if dumpTruthInfo:
  SCTVALIDStream.AddItem("xAOD::TruthParticleContainer#*")
  SCTVALIDStream.AddItem("xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension")
  SCTVALIDStream.AddItem("xAOD::TruthVertexContainer#*")
  SCTVALIDStream.AddItem("xAOD::TruthVertexAuxContainer#*")
  SCTVALIDStream.AddItem("xAOD::TruthEventContainer#*")
  SCTVALIDStream.AddItem("xAOD::TruthEventAuxContainer#*")

# Add trigger information
if dumpTriggerInfo:
    SCTVALIDStream.AddItem("TileCellContainer#MBTSContainer")
    SCTVALIDStream.AddItem("xAOD::TrigDecision#xTrigDecision")
    SCTVALIDStream.AddItem("BCM_RDO_Container#BCM_RDOs")
    SCTVALIDStream.AddItem("xAOD::TrigNavigation#TrigNavigation")
    SCTVALIDStream.AddItem("xAOD::TrigConfKeys#TrigConfKeys")
    SCTVALIDStream.AddItem("HLT::HLTResult#HLTResult_HLT")
    SCTVALIDStream.AddItem("xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.")
    SCTVALIDStream.AddItem("xAOD::TrigNavigationAuxInfo#TrigNavigationAux.")

if (printSctDxAODConf):
    print SCTVALIDStream
