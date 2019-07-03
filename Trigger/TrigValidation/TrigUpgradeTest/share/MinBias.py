#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs
eventAlgs,viewAlgs = makeInDetAlgs(whichSignature='FS', separateTrackParticleCreator='MinBias')


from AthenaCommon.AlgSequence import AlgSequence
topSequence  = AlgSequence()
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

topSequence.InDetTrigTrackParticleCreatorAlgMinBias.TrackName = "TrigFastTrackFinder_Tracks"
topSequence.InDetTrigTrackParticleCreatorAlgMinBias.roiCollectionName="FSRoI"


from TrigT2MinBias.TrigT2MinBiasConf import TrigCountSpacePointsMT
SpCount=TrigCountSpacePointsMT()
SpCount.OutputLevel= DEBUG
topSequence += SpCount

from TrigT2MinBias.TrigT2MinBiasMonitoringMT import SpCountMonitoring
SpCount.MonTool = SpCountMonitoring()
