#################
### Steering options
#################
## Load common flags
from AthenaCommon.JobProperties import jobproperties as athCommonFlags

# Select active sub-systems
dumpPixInfo=True

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
        print PixelChargeToTConversionSetter
        print PixelChargeToTConversionSetter.properties()


#################
### Setup decorators tools
#################


if dumpPixInfo:
    from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD
    xAOD_PixelPrepDataToxAOD = PixelPrepDataToxAOD( name = "xAOD_PixelPrepDataToxAOD")
    ## Content steering Properties (default value shown as comment)
    xAOD_PixelPrepDataToxAOD.OutputLevel          = INFO
    xAOD_PixelPrepDataToxAOD.UseTruthInfo         = False
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

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *


#====================================================================
# SET UP STREAM  
#====================================================================
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_IDPIXLUMIStream.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_IDPIXLUMIStream )
IDPIXLUMIStream = MSMgr.NewPoolRootStream( streamName, fileName )
IDPIXLUMIStream.AcceptAlgs(["DFTSOS_KERN"])



#################
### Setup Augmentation tools
#################
augmentationTools=[]


#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EventInfoPixelDecorator
#DFEI = DerivationFramework__EventInfoPixelDecorator(name = "DFEventInfoPixelDecorator",
#                                                    DecorationPrefix = "PixelClusters",
#                                                    EventInfoKey = "EventInfo",
#                                                    SelectionString = "PixelClusters.bec==0",
#                                                    TrackMeasurementValidationKey = "PixelClusters",
#                                                    OutputLevel =INFO)
#ToolSvc += DFEI
#augmentationTools+=[DFEI]
#if (printIdTrkDxAODConf):
#    print DFEI
#    print DFEI.properties()


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
    thinning_expression = "(PixelClusters.layer == 0) && ((PixelClusters.eta_module >= -10 && PixelClusters.eta_module <= -6) || (PixelClusters.eta_module >= 5 && PixelClusters.eta_module <= 9)) && (((PixelClusters.eta_module <= -7||PixelClusters.eta_module >= 6) && (PixelClusters.eta_pixel_index > 0 && (PixelClusters.eta_pixel_index + PixelClusters.sizeZ-1) < 79)) || ((PixelClusters.eta_module > -7 && PixelClusters.eta_module < 6) && (PixelClusters.eta_pixel_index > 0 && (PixelClusters.eta_pixel_index + PixelClusters.sizeZ - 1) < 159))) && (PixelClusters.phi_pixel_index > 0 && (PixelClusters.phi_pixel_index + PixelClusters.sizePhi-1) < 335)"
    IDPIXLUMIThinningTool = DerivationFramework__TrackMeasurementThinning( name = "IDPIXLUMIThinningTool",
        ThinningService = IDPIXLUMIThinningHelper.ThinningSvc(),
        SelectionString = thinning_expression,
        TrackMeasurementValidationKey = "PixelClusters",
        ApplyAnd = False)
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
    print DerivationFrameworkJob
    print DerivationFrameworkJob.properties()

 
#====================================================================
# SLIMMING
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
IDPIXLUMISlimmingHelper = SlimmingHelper("IDPIXLUMISlimmingHelper")
IDPIXLUMISlimmingHelper.AppendToDictionary = {'PixelClusters': 'xAOD::TrackMeasurementValidationContainer', 'PixelClustersAux': 'xAOD::TrackMeasurementValidationAuxContainer'}
IDPIXLUMISlimmingHelper.ExtraVariables = [ "PixelClusters.bec.layer.phi_module.eta_module.sizePhi.sizeZ.nRDO.charge.ToT.LVL1A.isFake.gangedPixel.isSplit.eta_pixel_index.phi_pixel_index" ]
IDPIXLUMISlimmingHelper.AppendContentToStream(IDPIXLUMIStream)

## Add PrimaryVertices and InDetTrackParticles excluding variables that we don't want or can't be written to xAOD
## Keep list up-to-date with at least:
## https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetExample/InDetRecExample/trunk/share/WriteInDetAOD.py
## Note: can't be added through SlimmingHelper since these are xAOD containers that are in the static list of containers in a RAW2ALL steering
excludeAuxTrackParticles = "-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"
excludeAuxPrimaryVertices = "-vxTrackAtVertex"
IDPIXLUMIStream.AddItem(['xAOD::VertexContainer#PrimaryVertices'])
IDPIXLUMIStream.AddItem(['xAOD::VertexAuxContainer#PrimaryVerticesAux.'+excludeAuxPrimaryVertices])
IDPIXLUMIStream.AddItem(['xAOD::TrackParticleContainer#InDetTrackParticles'])
IDPIXLUMIStream.AddItem(['xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.'+excludeAuxTrackParticles])

# Add trigger information

if (printIdTrkDxAODConf):
    print IDPIXLUMIStream
