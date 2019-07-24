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

from TrigT2MinBias.TrigT2MinBiasConf import TrigCountSpacePointsMT, SPCountHypoAlgMT, SPCountHypoTool
SpCount=TrigCountSpacePointsMT()
SpCount.OutputLevel= DEBUG
SpCount.SpacePointsKey="HLT_SpacePointCounts"
topSequence += SpCount


def makeAndSetHypo( alg, hypoClass, **hypokwargs):
    hypoTool = hypoClass( **hypokwargs )
    tools = alg.HypoTools
    alg.HypoTools = tools + [ hypoTool ]
    # now, this seems to be simpler: alg.HypoTools += [hypoTool], but it gives issue with Configurables of different class beeing named the same (even if they are private tools!)

SpCountHypo = SPCountHypoAlgMT()
SpCountHypo.OutputLevel= DEBUG
makeAndSetHypo( SpCountHypo, SPCountHypoTool, name="HLT_mbsptrk", OutputLevel=DEBUG)
SpCountHypo.HypoInputDecisions="FSDecisions"
SpCountHypo.HypoOutputDecisions="SPDecisions"
SpCountHypo.SpacePointsKey="HLT_SpacePointCounts"
topSequence += SpCountHypo



topSequence.InDetTrigTrackParticleCreatorAlgMinBias.roiCollectionName="FSRoI"
topSequence.InDetTrigTrackParticleCreatorAlgMinBias.TrackName = "TrigFastTrackFinder_Tracks"
topSequence.InDetTrigTrackParticleCreatorAlgMinBias.roiCollectionName="FSRoI"


from TrigMinBias.TrigMinBiasConf import TrackCountHypoAlgMT, TrackCountHypoTool
TrackCountHypo=TrackCountHypoAlgMT()
TrackCountHypo.OutputLevel= DEBUG
makeAndSetHypo( TrackCountHypo, TrackCountHypoTool, name="HLT_mbsptrk", OutputLevel=DEBUG )
TrackCountHypo.HypoInputDecisions="SPDecisions"
TrackCountHypo.HypoOutputDecisions="TrackCountDecisions"
TrackCountHypo.tracksKey="HLT_xAODTracksMinBias"
TrackCountHypo.trackCountKey="HLT_TrackCount"
topSequence += TrackCountHypo
