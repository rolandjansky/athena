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

# Message logger
from AthenaCommon import Logging
msg = Logging.logging.getLogger('SCTxAOD')

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
### Setup skimming tools
#################
skimmingTools = []

# Applying prescales 
# https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DerivationFramework#Applying_prescales
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__PrescaleTool
from InDetPrepRawDataToxAOD.SCTxAODJobProperties import SCTxAODFlags

from RecExConfig.RecFlags import rec
msg.info("rec.triggerStream() is "+rec.triggerStream())
if not SCTxAODFlags.Prescale.is_locked():
    if rec.triggerStream()=='express':
        SCTxAODFlags.Prescale = SCTxAODFlags.PrescaleExpress()
    elif rec.triggerStream()=='IDprescaledL1':
        SCTxAODFlags.Prescale = SCTxAODFlags.PrescaleIDprescaledL1()
msg.info("SCTxAODFlags.Prescale() is "+str(SCTxAODFlags.Prescale()))

prescaleTool = DerivationFramework__PrescaleTool(name = "SCTxAOD_PrescaleTool",
                                                 Prescale = SCTxAODFlags.Prescale())
ToolSvc += prescaleTool
skimmingTools += [prescaleTool]

#################
### Setup decorators tools
#################

from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_PrepDataToxAOD
xAOD_SCT_PrepDataToxAOD = SCT_PrepDataToxAOD( name = "SCTxAOD_SCT_PrepDataToxAOD")
    ## Content steering Properties (default value shown as comment)
xAOD_SCT_PrepDataToxAOD.OutputLevel=INFO
xAOD_SCT_PrepDataToxAOD.UseTruthInfo        = dumpTruthInfo
xAOD_SCT_PrepDataToxAOD.WriteRDOinformation = True
    #xAOD_SCT_PrepDataToxAOD.WriteSDOs           = True
    #xAOD_SCT_PrepDataToxAOD.WriteSiHits         = True # if available

if printSctDxAODConf:
    msg.info(xAOD_SCT_PrepDataToxAOD)
    msg.info(xAOD_SCT_PrepDataToxAOD.properties())

from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_RawDataToxAOD
xAOD_SCT_RawDataToxAOD = SCT_RawDataToxAOD( name = "SCTxAOD_SCT_RawDataToxAOD")
xAOD_SCT_RawDataToxAOD.OutputLevel = INFO
if printSctDxAODConf:
    msg.info(xAOD_SCT_RawDataToxAOD)
    msg.info(xAOD_SCT_RawDataToxAOD.properties())


#################
### Setup derivation framework
#################
from AthenaCommon import CfgMgr

# DerivationJob is COMMON TO ALL DERIVATIONS
DerivationFrameworkJob = CfgMgr.AthSequencer("SCTxAOD_Seq")

# Set up stream auditor
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'DecisionSvc'):
        svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True


# Add the TSOS augmentation tool to the derivation framework
augmentationTools=[]

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackStateOnSurfaceDecorator
DFTSOS = DerivationFramework__TrackStateOnSurfaceDecorator(name = "SCTxAOD_DFTrackStateOnSurfaceDecorator",
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
    DFEI = DerivationFramework__EventInfoBSErrDecorator(name = "SCTxAOD_DFEventInfoBSErrDecorator",
                                                        ContainerName = "EventInfo",
                                                        DecorationPrefix = prefixName,
                                                        OutputLevel =INFO)
    ToolSvc += DFEI
    augmentationTools+=[DFEI]
    if printSctDxAODConf:
        msg.info(DFEI)
        msg.info(DFEI.properties())

# Add Unassociated hits augmentation tool
if dumpUnassociatedHits:
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UnassociatedHitsGetterTool 
    unassociatedHitsGetterTool = DerivationFramework__UnassociatedHitsGetterTool (name = 'SCTxAOD_unassociatedHitsGetter',
                                                                                  TrackCollection = "Tracks",
                                                                                  PixelClusters = "PixelClusters",
                                                                                  SCTClusterContainer = "SCT_Clusters",
                                                                                  TRTDriftCircleContainer = "TRT_DriftCircles")
    ToolSvc += unassociatedHitsGetterTool
    if printSctDxAODConf:
        msg.info(unassociatedHitsGetterTool)
        msg.info(unassociatedHitsGetterTool.properties())

    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UnassociatedHitsDecorator
    unassociatedHitsDecorator = DerivationFramework__UnassociatedHitsDecorator (name ='SCTxAOD_unassociatedHitsDecorator',
                                                                                UnassociatedHitsGetter = unassociatedHitsGetterTool,
                                                                                ContainerName = "EventInfo",
                                                                                DecorationPrefix = prefixName,
                                                                                OutputLevel =INFO)
    ToolSvc += unassociatedHitsDecorator
    augmentationTools+=[unassociatedHitsDecorator]
    if printSctDxAODConf:
        msg.info(unassociatedHitsDecorator)
        msg.info(unassociatedHitsDecorator.properties())


# Add the derivation job to the top AthAlgSeqeuence
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("SCTxAOD_PreSelection",
                                                                       SkimmingTools = skimmingTools,
                                                                       OutputLevel =INFO)
DerivationFrameworkJob += xAOD_SCT_PrepDataToxAOD
DerivationFrameworkJob += xAOD_SCT_RawDataToxAOD
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("SCTxAOD_DF_KERN",
                                                                        AugmentationTools = augmentationTools,
                                                                        OutputLevel =INFO)

topSequence += DerivationFrameworkJob
if printSctDxAODConf:
    msg.info(DerivationFrameworkJob)
    msg.info(DerivationFrameworkJob.properties())

#################
### Steer output file content
#################

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_SCTVALIDStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_SCTVALIDStream )
SCTVALIDStream = MSMgr.NewPoolRootStream( streamName, fileName )
SCTVALIDStream.AcceptAlgs(["SCTxAOD_DF_KERN"])
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

if printSctDxAODConf:
    msg.info(SCTVALIDStream)
