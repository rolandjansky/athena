# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

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


from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__PixelNtupleMaker
PixelMonitoringSkimmingTool = DerivationFramework__PixelNtupleMaker(name = "PixelMonitoringSkimmingTool",ContainerName="InDetTrackParticles")

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
streamName = primDPD.WriteDAOD_PIXELVALIDStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_PIXELVALIDStream )
PIXELVALIDStream = MSMgr.NewPoolRootStream( streamName, fileName )
PIXELVALIDStream.AcceptAlgs(["PixelxAOD_DF_KERN"])

excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition.-definingParametersCovMatrix.-vertexLink.-msosLink.-trackLink"

# Add generic event information
PIXELVALIDStream.AddItem("xAOD::EventInfo#*")
PIXELVALIDStream.AddItem("xAOD::EventAuxInfo#*")

PIXELVALIDStream.AddItem("xAOD::MuonContainer#Muons")
PIXELVALIDStream.AddItem("xAOD::MuonAuxContainer#MuonsAux.")
PIXELVALIDStream.AddItem("xAOD::TrackParticleContainer#PixelMonitoringTrack")
PIXELVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#PixelMonitoringTrackAux."+excludedAuxData)

PIXELVALIDStream.AddItem("xAOD::VertexContainer#PrimaryVertices")
PIXELVALIDStream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex")

PIXELVALIDStream.AddItem("xAOD::ElectronContainer#Electrons")
PIXELVALIDStream.AddItem("xAOD::ElectronAuxContainer#ElectronsAux.")
PIXELVALIDStream.AddItem("xAOD::TauJetContainer#TauJets")
PIXELVALIDStream.AddItem("xAOD::TauJetAuxContainer#TauJetsAux.")
PIXELVALIDStream.AddItem("xAOD::JetContainer#AntiKt4LCTopoJets")
PIXELVALIDStream.AddItem("xAOD::JetAuxContainer#AntiKt4LCTopoJetsAux.")
PIXELVALIDStream.AddItem("xAOD::BTaggingContainer#BTagging_AntiKt4LCTopo")
PIXELVALIDStream.AddItem("xAOD::BTaggingAuxContainer#BTagging_AntiKt4LCTopoAux.")
PIXELVALIDStream.AddItem("xAOD::MissingETContainer#MET_Calo")
PIXELVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_CaloAux.")
PIXELVALIDStream.AddItem("xAOD::MissingETContainer#MET_Track")
PIXELVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_TrackAux.")
PIXELVALIDStream.AddItem("xAOD::MissingETContainer#MET_LocHadTopo")
PIXELVALIDStream.AddItem("xAOD::MissingETContainer#MET_LocHadTopoRegions")
PIXELVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_LocHadTopoAux.")
PIXELVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_LocHadTopoRegionsAux.")
PIXELVALIDStream.AddItem("xAOD::MissingETContainer#MET_Core_AntiKt4LCTopo")
PIXELVALIDStream.AddItem("xAOD::MissingETContainer#MET_Reference_AntiKt4LCTopo")
PIXELVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_Core_AntiKt4LCTopoAux.")
PIXELVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_Reference_AntiKt4LCTopoAux.")
PIXELVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_Core_AntiKt4LCTopoAuxDyn.softConstituents")
if dumpTruthInfo:
    PIXELVALIDStream.AddItem("xAOD::MissingETContainer#MET_Truth")
    PIXELVALIDStream.AddItem("xAOD::MissingETContainer#MET_TruthRegions")
    PIXELVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_TruthAux.")
    PIXELVALIDStream.AddItem("xAOD::MissingETAuxContainer#MET_TruthRegionsAux.")

# Add truth-related information
if dumpTruthInfo:
    PIXELVALIDStream.AddItem("xAOD::TruthParticleContainer#*")
    PIXELVALIDStream.AddItem("xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension")
    PIXELVALIDStream.AddItem("xAOD::TruthVertexContainer#*")
    PIXELVALIDStream.AddItem("xAOD::TruthVertexAuxContainer#*")
    PIXELVALIDStream.AddItem("xAOD::TruthEventContainer#*")
    PIXELVALIDStream.AddItem("xAOD::TruthEventAuxContainer#*")
    PIXELVALIDStream.AddItem("xAOD::TruthPileupEventContainer#*")
    PIXELVALIDStream.AddItem("xAOD::TruthPileupEventAuxContainer#*")
    # add pseudo tracking in xAOD
    PIXELVALIDStream.AddItem("xAOD::TrackParticleContainer#InDetPseudoTrackParticles")
    PIXELVALIDStream.AddItem("xAOD::TrackParticleAuxContainer#InDetPseudoTrackParticlesAux."+excludedAuxData)

# Add trigger information (including metadata)
if dumpTriggerInfo:
    PIXELVALIDStream.AddMetaDataItem("xAOD::TriggerMenuContainer#TriggerMenu")
    PIXELVALIDStream.AddMetaDataItem("xAOD::TriggerMenuAuxContainer#TriggerMenuAux.")
    PIXELVALIDStream.AddItem("TileCellContainer#MBTSContainer")
    PIXELVALIDStream.AddItem("xAOD::TrigDecision#xTrigDecision")
    PIXELVALIDStream.AddItem("BCM_RDO_Container#BCM_RDOs")
    PIXELVALIDStream.AddItem("xAOD::TrigNavigation#TrigNavigation")
    PIXELVALIDStream.AddItem("xAOD::TrigConfKeys#TrigConfKeys")
    PIXELVALIDStream.AddItem("HLT::HLTResult#HLTResult_HLT")
    PIXELVALIDStream.AddItem("xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.")
    PIXELVALIDStream.AddItem("xAOD::TrigNavigationAuxInfo#TrigNavigationAux.")

if (printIdTrkDxAODConf):
    print PIXELVALIDStream
