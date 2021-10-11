# For using GeV units
import AthenaCommon.SystemOfUnits as Units

# Needed in order that PixelPrepDataToxAOD does not generate a potential memory leak error. 
import InDetRecExample.TrackingCommon as TrackingCommon

# We need to add this algorithm to get the TrackMeasurementValidationContainer
from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD, SCT_PrepDataToxAOD
#
# Getting the Pixel cluster container is more complicated than geting the SCT cluster container 
# You MUST minimally, have these items - a 'name' and this ClusterSplitProbabilityName or you get a memory leak ERROR
xAOD_PixelPrepDataToxAOD = PixelPrepDataToxAOD( name = "xAOD_PixelPrepDataToxAOD", 
                                                ClusterSplitProbabilityName = TrackingCommon.pixelClusterSplitProbName() )

from AthenaCommon.AlgSequence import AlgSequence
algSeq = AlgSequence()

algSeq += xAOD_PixelPrepDataToxAOD
algSeq += SCT_PrepDataToxAOD('SCT_PrepDataToxAOD')

from JetHitAssociation.JetHitAssociationConf import JetHitAssociation
jetHitAssociation = JetHitAssociation('JetHitAssociation')
jetHitAssociation.jetCollectionName = "AntiKt4EMPFlowJets"
jetHitAssociation.jetPtThreshold = 300 * Units.GeV
jetHitAssociation.dRmatchHitToJet = 0.4
#jetHitAssociation.OutputLevel = DEBUG

# Add algorithm to sequence
algSeq += jetHitAssociation