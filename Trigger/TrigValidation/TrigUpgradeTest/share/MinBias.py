#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
eventAlgs,viewAlgs = makeInDetAlgs(whichSignature='FS', separateTrackParticleCreator='MinBias')

from AthenaCommon.AlgSequence import AlgSequence
topSequence  = AlgSequence()
topSequence.L1Decoder.ChainToCTPMapping={"HLT_mbsptrk":"L1_RD0_FILLED"}
topSequence += eventAlgs

topSequence.InDetSCTRawDataProvider_FS.RoIs   = "FSRoI"
topSequence.InDetPixelRawDataProvider_FS.RoIs = "FSRoI"
topSequence.InDetTRTRawDataProvider_FS.RoIs   = "FSRoI"
topSequence.InDetSCT_Clusterization_FS.RoIs   = "FSRoI"
topSequence.InDetPixelClusterization_FS.RoIs  = "FSRoI"

from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_MinBias
theFTF              = TrigFastTrackFinder_MinBias()
theFTF.isRoI_Seeded = True
theFTF.RoIs         = "FSRoI"
topSequence += theFTF

topSequence.InDetTrigTrackParticleCreatorAlgMinBias.roiCollectionName="FSRoI"
topSequence.InDetTrigTrackParticleCreatorAlgMinBias.TrackName = "TrigFastTrackFinder_Tracks"

from TrigMinBias.TrigMinBiasConf import TrackCountHypoAlgMT, TrackCountHypoTool
TrackCountHypo=TrackCountHypoAlgMT()
TrackCountHypoTool1=TrackCountHypoTool("HLT_mbsptrk")
TrackCountHypoTool1.OutputLevel=DEBUG

TrackCountHypo.OutputLevel= DEBUG
TrackCountHypo.HypoTools+=[TrackCountHypoTool1]
TrackCountHypo.HypoInputDecisions="FSDecisions"
TrackCountHypo.HypoOutputDecisions="TrackCountDecisions"
TrackCountHypo.tracksKey="HLT_xAODTracksMinBias"
topSequence += TrackCountHypo

from TrigMinBias.TrackCountMonitoringMT import TrackCountMonitoring
TrackCountHypo.MonTool = TrackCountMonitoring()
