#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()

from TrigHLTResultByteStream.TrigHLTResultByteStreamConf import HLTResultMTByteStreamDecoderAlg
decoder = HLTResultMTByteStreamDecoderAlg()
decoder.OutputLevel=DEBUG
topSequence += decoder


from TrigOutputHandling.TrigOutputHandlingConf import TriggerEDMDeserialiserAlg
deserialiser = TriggerEDMDeserialiserAlg()
deserialiser.OutputLevel=DEBUG
topSequence += deserialiser

from OutputStreamAthenaPool.OutputStreamAthenaPool import  createOutputStream
StreamESD=createOutputStream("StreamESD","myESDfromBS.pool.root",True)
topSequence.remove( StreamESD )
StreamESD.ItemList += [ "xAOD::TrigElectronContainer#HLT_xAOD__TrigElectronContainer_L2ElectronFex", 
                        "xAOD::TrackParticleContainer#HLT_xAOD_TrackParticleContainer_L2ElectronTracks",
                        "xAOD::TrigEMClusterContainer#HLT_xAOD__TrigEMClusterContainer_L2CaloClusters"]

StreamESD.ItemList += [ "xAOD::TrigElectronAuxContainer#HLT_xAOD__TrigElectronContainer_L2ElectronFexAux.", 
                        "xAOD::TrackParticleAuxContainer#HLT_xAOD_TrackParticleContainer_L2ElectronTracksAux.", 
                        "xAOD::TrigEMClusterAuxContainer#HLT_xAOD__TrigEMClusterContainer_L2CaloClustersAux."]

StreamESD.ItemList += [ "EventInfo#ByteStreamEventInfo" ]

decisions = [ "EgammaCaloDecisions", "L2CaloLinks", "FilteredEgammaCaloDecisions", "FilteredEMRoIDecisions", "EMRoIDecisions", "RerunEMRoIDecisions" ]
StreamESD.ItemList += [ "xAOD::TrigCompositeContainer#remap_"+d for d in decisions ]
StreamESD.ItemList += [ "xAOD::TrigCompositeAuxContainer#remap_"+d+"Aux." for d in decisions ]


from TrigOutputHandling.TrigOutputHandlingConf import HLTEDMCreator
egammaCreator = HLTEDMCreator("egammaCreator")
egammaCreator.FixLinks=False
egammaCreator.OutputLevel=DEBUG
egammaCreator.TrigCompositeContainer = [ "remap_"+d for d in decisions ]

egammaCreator.TrackParticleContainer = [ "HLT_xAOD_TrackParticleContainer_L2ElectronTracks" ]
egammaCreator.TrigElectronContainer  = [ "HLT_xAOD__TrigElectronContainer_L2ElectronFex" ]
egammaCreator.TrigEMClusterContainer = [ "HLT_xAOD__TrigEMClusterContainer_L2CaloClusters" ]

from TrigOutputHandling.TrigOutputHandlingConf import HLTEDMCreatorAlg
fillGaps = HLTEDMCreatorAlg( "FillMissingEDM" )
fillGaps.OutputTools = [ egammaCreator ]


outSequence = AthSequencer("AthOutSeq")
outSequence += fillGaps
outSequence += StreamESD

