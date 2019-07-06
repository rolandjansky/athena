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

from TrigT2MinBias.TrigT2MinBiasConf import TrigCountSpacePointsMT
SpCount=TrigCountSpacePointsMT()
SpCount.OutputLevel= DEBUG
topSequence += SpCount

from TrigT2MinBias.TrigT2MinBiasConf import SPCountHypoAlgMT, SPCountHypoTool
SpCountHypo=SPCountHypoAlgMT()
SpCountHypoTool1=SPCountHypoTool("HLT_mbsptrk")
SpCountHypoTool1.OutputLevel=DEBUG

SpCountHypoTool1.totNumPixSP=20;
SpCountHypoTool1.totNumSctSP=5;

topSequence.InDetTrigTrackParticleCreatorAlgMinBias.roiCollectionName="FSRoI"
topSequence.InDetTrigTrackParticleCreatorAlgMinBias.TrackName = "TrigFastTrackFinder_Tracks"
topSequence.InDetTrigTrackParticleCreatorAlgMinBias.roiCollectionName="FSRoI"

SpCountHypo.OutputLevel= DEBUG
SpCountHypo.HypoTools+=[SpCountHypoTool1]
SpCountHypo.HypoInputDecisions="FSDecisions"
SpCountHypo.HypoOutputDecisions="SPDecisions"
SpCountHypo.SpacePointsKey="HLT_SPCounts"
topSequence += SpCountHypo

from TrigT2MinBias.TrigT2MinBiasConf import TrigCountSpacePointsMT
SpCount=TrigCountSpacePointsMT()
SpCount.OutputLevel= DEBUG
SpCount.SpacePointsKey="HLT_SPCounts"
topSequence += SpCount

from TrigT2MinBias.TrigT2MinBiasMonitoringMT import SpCountMonitoring
SpCount.MonTool = SpCountMonitoring()
