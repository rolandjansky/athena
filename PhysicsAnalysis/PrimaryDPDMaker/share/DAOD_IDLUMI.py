#################
### Steering options
#################
## Load common flags
from AthenaCommon.JobProperties import jobproperties as athCommonFlags

# Select active sub-systems
dumpPixInfo=True

# Bytestream errors (for sub-systems who have implemented it)
dumpBytestreamErrors=False

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
        print PixelChargeToTConversionSetter
        print PixelChargeToTConversionSetter.properties()


#################
### Setup decorators tools
#################


if dumpPixInfo:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD
    xAOD_PixelPrepDataToxAOD = PixelPrepDataToxAOD( name = "IDLUMIxAOD_PixelPrepDataToxAOD")
    ## Content steering Properties (default value shown as comment)
    xAOD_PixelPrepDataToxAOD.OutputLevel          = INFO
    xAOD_PixelPrepDataToxAOD.UseTruthInfo         = dumpTruthInfo
    xAOD_PixelPrepDataToxAOD.WriteRDOinformation  = False
    xAOD_PixelPrepDataToxAOD.WriteNNinformation   = False
    xAOD_PixelPrepDataToxAOD.WriteSDOs            = True
    xAOD_PixelPrepDataToxAOD.WriteSiHits          = False # if available


    topSequence += xAOD_PixelPrepDataToxAOD
    if (printIdTrkDxAODConf):
        print xAOD_PixelPrepDataToxAOD
        print xAOD_PixelPrepDataToxAOD.properties()


#################
### Setup derivation framework
#################
#from AthenaCommon import CfgMgr

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

# Set up stream auditor
#from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#if not hasattr(svcMgr, 'DecisionSvc'):
#        svcMgr += CfgMgr.DecisionSvc()
#svcMgr.DecisionSvc.CalcStats = True


#====================================================================
# SET UP STREAM  
#====================================================================
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_IDLUMIStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_IDLUMIStream )
IDLUMIStream = MSMgr.NewPoolRootStream( streamName, fileName )
IDLUMIStream.AcceptAlgs(["DFIDLUMI_KERN"])



#################
### Setup Augmentation tools
#################
augmentationTools=[]

# Set up stream auditor
#from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#if not hasattr(svcMgr, 'DecisionSvc'):
#        svcMgr += CfgMgr.DecisionSvc()
#svcMgr.DecisionSvc.CalcStats = True


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


#====================================================================
# THINNING  
#====================================================================

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
IDLUMIThinningHelper = ThinningHelper( "IDLUMIThinningHelper" )
IDLUMIThinningHelper.AppendToStream( IDLUMIStream )

thinningTools = []

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackMeasurementThinning
thinning_expression = "PixelClusters.layer == 0"
IDLUMIThinningTool = DerivationFramework__TrackMeasurementThinning( name = "IDLUMIThinningTool",
    ThinningService = IDLUMIThinningHelper.ThinningSvc(),
    SelectionString = thinning_expression,
    TrackMeasurementValidationKey = "PixelClusters",
    ApplyAnd = False)
ToolSvc += IDLUMIThinningTool
thinningTools.append(IDLUMIThinningTool)


#====================================================================
# Create the derivation Kernel and setup output stream
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("DFIDLUMI_KERN",
                                                                       AugmentationTools = augmentationTools,
                                                                       ThinningTools = thinningTools,
                                                                       OutputLevel =INFO)

if (printIdTrkDxAODConf):
    print DerivationFrameworkJob
    print DerivationFrameworkJob.properties()

 
#====================================================================
# SLIMMING
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
IDLUMISlimmingHelper = SlimmingHelper("IDLUMISlimmingHelper")
IDLUMISlimmingHelper.AppendToDictionary = {'PixelClusters': 'xAOD::TrackMeasurementValidationContainer', 'PixelClustersAux': 'xAOD::TrackMeasurementValidationAuxContainer'}
IDLUMISlimmingHelper.ExtraVariables = [ "PixelClusters.globalX.globalY.globalZ.bec.layer.phi_module.eta_module.eta_pixel_index.phi_pixel_index.sizePhi.sizeZ.nRDO.charge.ToT.LVL1A.isFake.gangedPixel.isSplit" ]
IDLUMISlimmingHelper.AppendContentToStream(IDLUMIStream)

# Add truth-related information
if dumpTruthInfo:
  IDLUMIStream.AddItem("xAOD::TruthParticleContainer#*")
  IDLUMIStream.AddItem("xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension")
  IDLUMIStream.AddItem("xAOD::TruthVertexContainer#*")
  IDLUMIStream.AddItem("xAOD::TruthVertexAuxContainer#*")
  IDLUMIStream.AddItem("xAOD::TruthEventContainer#*")
  IDLUMIStream.AddItem("xAOD::TruthEventAuxContainer#*")
  IDLUMIStream.AddItem("xAOD::TruthPileupEventContainer#*")
  IDLUMIStream.AddItem("xAOD::TruthPileupEventAuxContainer#*")

# Add trigger information
if dumpTriggerInfo:
    IDLUMIStream.AddItem("xAOD::TrigT2MbtsBitsContainer#*")
    IDLUMIStream.AddItem("xAOD::TrigDecision#xTrigDecision")
    IDLUMIStream.AddItem("BCM_RDO_Container#BCM_RDOs")
    IDLUMIStream.AddItem("xAOD::TrigNavigation#TrigNavigation")
    IDLUMIStream.AddItem("xAOD::TrigConfKeys#TrigConfKeys")
    IDLUMIStream.AddItem("HLT::HLTResult#HLTResult_HLT")
    IDLUMIStream.AddItem("xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.")
    IDLUMIStream.AddItem("xAOD::TrigNavigationAuxInfo#TrigNavigationAux.")
    IDLUMIStream.AddItem("xAOD::TrigT2MbtsBits#HLT_T2Mbts")
    IDLUMIStream.AddItem("xAOD::TrigT2MbtsBitsAuxContainer#HLT_T2MbtsAux.")
    IDLUMIStream.AddItem("xAOD::TrigT2MbtsBitsAuxContainer#HLT_xAOD__TrigT2MbtsBitsContainer_T2MbtsAux.")

if (printIdTrkDxAODConf):
    print IDLUMIStream
