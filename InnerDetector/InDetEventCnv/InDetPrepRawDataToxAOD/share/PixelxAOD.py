#################
### Steering options
#################
## Load common flags
from AthenaCommon.JobProperties import jobproperties as athCommonFlags
from DerivationFrameworkInDet.InDetCommon import *

from InDetPrepRawDataToxAOD.InDetDxAODJobProperties import InDetDxAODFlags

DRAWZSel = InDetDxAODFlags.DRAWZSelection()

# Bytestream errors (for sub-systems who have implemented it)
dumpBytestreamErrors=InDetDxAODFlags.DumpByteStreamErrors() #True

# Force to do not dump truth info if set to False
#  (otherwise determined by autoconf below)
dumpTruthInfo=InDetDxAODFlags.DumpTruthInfo() # True

# Saves partial trigger information in the output stream (none otherwise)
dumpTriggerInfo= InDetDxAODFlags.DumpTriggerInfo()  #True

# Print settings for main tools
printIdTrkDxAODConf = InDetDxAODFlags.PrintIdTrkDxAODConf()  # True

## Autoconfiguration adjustements
isIdTrkDxAODSimulation = False
if (globalflags.DataSource == 'geant4'):
    isIdTrkDxAODSimulation = True

if ( 'dumpTruthInfo' in dir() ):
    dumpTruthInfo = dumpTruthInfo and isIdTrkDxAODSimulation

if DRAWZSel:
    dumpTriggerInfo=True

## Other settings
# Prefix for decoration, if any
prefixName = ""

## More fine-tuning available for each tool/alg below (default value shown)

#################
### Setup tools
#################
from AthenaCommon import CfgMgr
IDDerivationSequence = CfgMgr.AthSequencer("InDetDxAOD_seq")

#Setup charge->ToT back-conversion to restore ToT info as well
from PixelCalibAlgs.PixelCalibAlgsConf import PixelChargeToTConversion 
PixelChargeToTConversionSetter = PixelChargeToTConversion(name = "PixelChargeToTConversionSetter") 
IDDerivationSequence += PixelChargeToTConversionSetter 
if (printIdTrkDxAODConf):
    print PixelChargeToTConversionSetter
    print PixelChargeToTConversionSetter.properties()


DRAW_ZMUMU_SkimmingTool=None
if DRAWZSel:
    sel_muon1  = 'Muons.pt > 25*GeV && Muons.ptcone40/Muons.pt < 0.3 && Muons.passesIDCuts && (Muons.quality <= 2)'
    sel_muon2  = 'Muons.pt > 20*GeV && Muons.ptcone40/Muons.pt < 0.3 && Muons.passesIDCuts && (Muons.quality <= 2)'
    draw_zmumu = '( count (  DRZmumuMass > 70*GeV   &&  DRZmumuMass < 110*GeV ) >= 1 )'
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__InvariantMassTool
    DRZmumuMassTool = DerivationFramework__InvariantMassTool(name = "DRZmumuMassTool",
                                                             ContainerName              = "Muons",
                                                             ObjectRequirements         = sel_muon1,
                                                             SecondObjectRequirements   = sel_muon2,
                                                             MassHypothesis             = 105.66,
                                                             SecondMassHypothesis       = 105.66,
                                                             StoreGateEntryName         = "DRZmumuMass")
    ToolSvc += DRZmumuMassTool
    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
    DRAW_ZMUMU_SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "DRAW_ZMUMU_SkimmingTool",
                                                                        expression = draw_zmumu)
    ToolSvc += DRAW_ZMUMU_SkimmingTool
    print DRAW_ZMUMU_SkimmingTool


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__PixelOfflineMonitoring
PixelMonitoringSkimmingTool = DerivationFramework__PixelOfflineMonitoring(name = "PixelMonitoringSkimmingTool",ContainerName="InDetTrackParticles")

ToolSvc += PixelMonitoringSkimmingTool


#################
### Setup decorators tools
#################
from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD
xAOD_PixelPrepDataToxAOD = PixelPrepDataToxAOD( name = "xAOD_PixelPrepDataToxAOD")
xAOD_PixelPrepDataToxAOD.OutputLevel          = INFO
xAOD_PixelPrepDataToxAOD.UseTruthInfo         = dumpTruthInfo
xAOD_PixelPrepDataToxAOD.WriteRDOinformation  = True
xAOD_PixelPrepDataToxAOD.WriteNNinformation   = InDetDxAODFlags.DumpPixelNNInfo()
#xAOD_PixelPrepDataToxAOD.WriteSDOs            = True
#xAOD_PixelPrepDataToxAOD.WriteSiHits          = True # if available
if InDetFlags.doSLHC():
    xAOD_PixelPrepDataToxAOD.WriteNNinformation=False

IDDerivationSequence += xAOD_PixelPrepDataToxAOD
if (printIdTrkDxAODConf):
    print xAOD_PixelPrepDataToxAOD
    print xAOD_PixelPrepDataToxAOD.properties()


#################
### Setup Augmentation tools
#################
augmentationTools=[]

if DRAWZSel:
    augmentationTools.append(DRZmumuMassTool)

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
                                                          StoreTRT   = False,
                                                          StoreSCT   = False,
                                                          StorePixel = True,
                                                          IsSimulation = isIdTrkDxAODSimulation,
                                                          AssociationTool = InDetPrdAssociationTool,
                                                          OutputLevel = INFO)

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

# Add decoration with truth parameters if running on simulation
if isIdTrkDxAODSimulation:
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParametersForTruthParticles
    TruthDecor = DerivationFramework__TrackParametersForTruthParticles( name = "TruthTPDecor",
                                                                        TruthParticleContainerName = "TruthParticles",
                                                                        DecorationPrefix = "")
    ToolSvc += TruthDecor
    augmentationTools.append(TruthDecor)
    print TruthDecor

#====================================================================
# Skimming Tools
#====================================================================
skimmingTools = []

if DRAWZSel:
    skimmingTools.append(DRAW_ZMUMU_SkimmingTool)

skimmingTools.append(PixelMonitoringSkimmingTool)

#====================================================================
# Thinning Tools
#====================================================================
thinningTools = []

#====================================================================
# Create the derivation Kernel and setup output stream
#====================================================================
# Add the derivation job to the top AthAlgSeqeuence
# DerivationJob is COMMON TO ALL DERIVATIONS
IDDerivationSequence += CfgMgr.DerivationFramework__DerivationKernel("PixelxAOD_DF_KERN",
                                                                       AugmentationTools = augmentationTools,
                                                                       SkimmingTools = skimmingTools,
                                                                       ThinningTools = thinningTools,
                                                                       OutputLevel = INFO)

topSequence += IDDerivationSequence 
if (printIdTrkDxAODConf):
    print IDDerivationSequence 
    print IDDerivationSequence.properties()

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
streamName = primDPD.WriteDAOD_PixelVALIDStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_PixelVALIDStream )
PixelVALIDStream = MSMgr.NewPoolRootStream( streamName, fileName )
PixelVALIDStream.AcceptAlgs(["PixelxAOD_DF_KERN"])

excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition.-definingParametersCovMatrix.-vertexLink.-msosLink.-trackLink"

# Add generic event information
PixelVALIDStream.AddItem("xAOD::EventInfo#*")
PixelVALIDStream.AddItem("xAOD::EventAuxInfo#*")

PixelVALIDStream.AddItem("xAOD::MuonContainer#Muons")
PixelVALIDStream.AddItem("xAOD::MuonAuxContainer#MuonsAux.")
PixelVALIDStream.AddItem("xAOD::TrackParticleContainer#PixelMonitoringTrack")
PixelVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#PixelMonitoringTrackAux."+excludedAuxData)

PixelVALIDStream.AddItem("xAOD::VertexContainer#PrimaryVertices")
PixelVALIDStream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex")

PixelVALIDStream.AddItem("xAOD::ElectronContainer#Electrons")
PixelVALIDStream.AddItem("xAOD::ElectronAuxContainer#ElectronsAux.")
PixelVALIDStream.AddItem("xAOD::TauJetContainer#TauJets")
PixelVALIDStream.AddItem("xAOD::TauJetAuxContainer#TauJetsAux.")
PixelVALIDStream.AddItem("xAOD::JetContainer#AntiKt4LCTopoJets")
PixelVALIDStream.AddItem("xAOD::JetAuxContainer#AntiKt4LCTopoJetsAux.")
PixelVALIDStream.AddItem("xAOD::BTaggingContainer#BTagging_AntiKt4LCTopo")
PixelVALIDStream.AddItem("xAOD::BTaggingAuxContainer#BTagging_AntiKt4LCTopoAux.")
PixelVALIDStream.AddItem("xAOD::MissingETContainer#MET_Calo")
PixelVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_CaloAux.")
PixelVALIDStream.AddItem("xAOD::MissingETContainer#MET_Track")
PixelVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_TrackAux.")
PixelVALIDStream.AddItem("xAOD::MissingETContainer#MET_LocHadTopo")
PixelVALIDStream.AddItem("xAOD::MissingETContainer#MET_LocHadTopoRegions")
PixelVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_LocHadTopoAux.")
PixelVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_LocHadTopoRegionsAux.")
PixelVALIDStream.AddItem("xAOD::MissingETContainer#MET_Core_AntiKt4LCTopo")
PixelVALIDStream.AddItem("xAOD::MissingETContainer#MET_Reference_AntiKt4LCTopo")
PixelVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_Core_AntiKt4LCTopoAux.")
PixelVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_Reference_AntiKt4LCTopoAux.")
PixelVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_Core_AntiKt4LCTopoAuxDyn.softConstituents")
if dumpTruthInfo:
    PixelVALIDStream.AddItem("xAOD::MissingETContainer#MET_Truth")
    PixelVALIDStream.AddItem("xAOD::MissingETContainer#MET_TruthRegions")
    PixelVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_TruthAux.")
    PixelVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_TruthRegionsAux.")

# Add truth-related information
if dumpTruthInfo:
    PixelVALIDStream.AddItem("xAOD::TruthParticleContainer#*")
    PixelVALIDStream.AddItem("xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension")
    PixelVALIDStream.AddItem("xAOD::TruthVertexContainer#*")
    PixelVALIDStream.AddItem("xAOD::TruthVertexAuxContainer#*")
    PixelVALIDStream.AddItem("xAOD::TruthEventContainer#*")
    PixelVALIDStream.AddItem("xAOD::TruthEventAuxContainer#*")
    PixelVALIDStream.AddItem("xAOD::TruthPileupEventContainer#*")
    PixelVALIDStream.AddItem("xAOD::TruthPileupEventAuxContainer#*")
    # add pseudo tracking in xAOD
    PixelVALIDStream.AddItem("xAOD::TrackParticleContainer#InDetPseudoTrackParticles")
    PixelVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#InDetPseudoTrackParticlesAux."+excludedAuxData)

# Add trigger information (including metadata)
if dumpTriggerInfo:
    PixelVALIDStream.AddMetaDataItem("xAOD::TriggerMenuContainer#TriggerMenu")
    PixelVALIDStream.AddMetaDataItem("xAOD::TriggerMenuAuxContainer#TriggerMenuAux.")
    PixelVALIDStream.AddItem("TileCellContainer#MBTSContainer")
    PixelVALIDStream.AddItem("xAOD::TrigDecision#xTrigDecision")
    PixelVALIDStream.AddItem("BCM_RDO_Container#BCM_RDOs")
    PixelVALIDStream.AddItem("xAOD::TrigNavigation#TrigNavigation")
    PixelVALIDStream.AddItem("xAOD::TrigConfKeys#TrigConfKeys")
    PixelVALIDStream.AddItem("HLT::HLTResult#HLTResult_HLT")
    PixelVALIDStream.AddItem("xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.")
    PixelVALIDStream.AddItem("xAOD::TrigNavigationAuxInfo#TrigNavigationAux.")

if (printIdTrkDxAODConf):
    print PixelVALIDStream
