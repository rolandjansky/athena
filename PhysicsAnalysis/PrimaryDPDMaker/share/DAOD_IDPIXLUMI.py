#################
### Steering options
#################
## Load common flags
from AthenaCommon.JobProperties import jobproperties as athCommonFlags

from PrimaryDPDMaker.DxAODPixLumiJobProperties import DxAODPixLumiFlags

# Select active sub-systems
dumpPixInfo = DxAODPixLumiFlags.DumpPixelInfo()

# Print settings for main tools
printIdTrkDxAODConf = True


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


#################
### Setup decorators tools
#################


if dumpPixInfo:
    import InDetRecExample.TrackingCommon as TrackingCommon
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD
    xAOD_PixelPrepDataToxAOD = PixelPrepDataToxAOD( name = "xAOD_PixelPrepDataToxAOD",
                                                    ClusterSplitProbabilityName = TrackingCommon.pixelClusterSplitProbName())
    ## Content steering Properties (default value shown as comment)
    xAOD_PixelPrepDataToxAOD.OutputLevel          = INFO
    xAOD_PixelPrepDataToxAOD.UseTruthInfo         = False
    xAOD_PixelPrepDataToxAOD.WriteRDOinformation  = False
    xAOD_PixelPrepDataToxAOD.WriteNNinformation   = False
    xAOD_PixelPrepDataToxAOD.WriteSDOs            = True
    xAOD_PixelPrepDataToxAOD.WriteSiHits          = False # if available
    xAOD_PixelPrepDataToxAOD.OutputClusterContainer = "xAOD_PixelClusters" #NB, this must match the name used to define the thinning string


    topSequence += xAOD_PixelPrepDataToxAOD
    if (printIdTrkDxAODConf):
        print(xAOD_PixelPrepDataToxAOD)
        print(xAOD_PixelPrepDataToxAOD.properties())


#################
### Setup derivation framework
#################

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *


#====================================================================
# SET UP STREAM  
#====================================================================
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_IDPIXLUMIStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_IDPIXLUMIStream )
IDPIXLUMIStream = MSMgr.NewPoolRootStream( streamName, fileName )
IDPIXLUMIStream.AcceptAlgs(["DFTSOS_KERN"])

# Add generic event information
IDPIXLUMIStream.AddItem("xAOD::EventInfo#*")
IDPIXLUMIStream.AddItem("xAOD::EventAuxInfo#*")


#################
### Setup Augmentation tools
#################
augmentationTools=[]

#====================================================================
# THINNING  
#====================================================================

# Establish the thinning helper (which will set up the services behind the scenes)
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
IDPIXLUMIThinningHelper = ThinningHelper( "IDPIXLUMIThinningHelper" )
IDPIXLUMIThinningHelper.AppendToStream( IDPIXLUMIStream )

thinningTools = []

if dumpPixInfo: 
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackMeasurementThinning
    thinning_expression = DxAODPixLumiFlags.ModulesSelection()
    IDPIXLUMIThinningTool = DerivationFramework__TrackMeasurementThinning( name = "IDPIXLUMIThinningTool",
        StreamName = streamName,
        SelectionString = thinning_expression,
        TrackMeasurementValidationKey = "xAOD_PixelClusters")
    ToolSvc += IDPIXLUMIThinningTool
    thinningTools.append(IDPIXLUMIThinningTool)


#====================================================================
# Create the derivation Kernel and setup output stream
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("DFTSOS_KERN",
                                                                       AugmentationTools = augmentationTools,
                                                                       ThinningTools = thinningTools,
                                                                       OutputLevel =INFO)

if (printIdTrkDxAODConf):
    print(DerivationFrameworkJob)
    print(DerivationFrameworkJob.properties())

 
#====================================================================
# SLIMMING
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
IDPIXLUMISlimmingHelper = SlimmingHelper("IDPIXLUMISlimmingHelper")
excludedVtxAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"
IDPIXLUMISlimmingHelper.AppendToDictionary = {'xAOD_PixelClusters': 'xAOD::TrackMeasurementValidationContainer', 'xAOD_PixelClustersAux': 'xAOD::TrackMeasurementValidationAuxContainer'}
IDPIXLUMISlimmingHelper.ExtraVariables = [ "xAOD_PixelClusters.bec.layer.phi_module.eta_module.sizePhi.sizeZ.nRDO.charge.ToT.LVL1A.isFake.gangedPixel.isSplit" ]
IDPIXLUMISlimmingHelper.AllVariables = [ "InDetTrackParticles", "PrimaryVertices."+excludedVtxAuxData ]
IDPIXLUMISlimmingHelper.SmartCollections = [ "InDetTrackParticles", "PrimaryVertices" ]
IDPIXLUMISlimmingHelper.AppendContentToStream(IDPIXLUMIStream)


# Add trigger information

if (printIdTrkDxAODConf):
    print(IDPIXLUMIStream)
