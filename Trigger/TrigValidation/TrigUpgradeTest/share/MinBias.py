#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
include("TrigUpgradeTest/testHLT_MT.py")

from TriggerMenuMT.HLTMenuConfig.CommonSequences.InDetSetup import makeInDetAlgs

viewAlgs, eventAlgs = makeInDetAlgs(whichSignature='MinBias', separateTrackParticleCreator='MinBias')

for viewAlg in viewAlgs:
        if "RoIs" in viewAlg.properties():
            viewAlg.RoIs = "FSRoI"
        if "roiCollectionName" in viewAlg.properties():
            viewAlg.roiCollectionName = "FSRoI"

from AthenaCommon.AlgSequence import AlgSequence
topSequence  = AlgSequence()
topSequence += viewAlgs

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
SpCountHypo.HypoInputDecisions="L1FS"
makeAndSetHypo( SpCountHypo, SPCountHypoTool, name="HLT_mbsptrk", OutputLevel=DEBUG)

SpCountHypo.HypoOutputDecisions="SPDecisions"
SpCountHypo.SpacePointsKey="HLT_SpacePointCounts"
topSequence += SpCountHypo

from TrigMinBias.TrigMinBiasConf import TrackCountHypoAlgMT, TrackCountHypoTool
TrackCountHypo=TrackCountHypoAlgMT()
TrackCountHypo.OutputLevel= DEBUG
makeAndSetHypo( TrackCountHypo, TrackCountHypoTool, name="HLT_mbsptrk", OutputLevel=DEBUG )

TrackCountHypo.HypoInputDecisions="SPDecisions"
TrackCountHypo.HypoOutputDecisions="TrackCountDecisions"
TrackCountHypo.tracksKey="HLT_xAODTracksMinBias"
TrackCountHypo.trackCountKey="HLT_TrackCount"
topSequence += TrackCountHypo

from TrigMinBias.TrackCountMonitoringMT import TrackCountMonitoring
TrackCountHypo.MonTool = TrackCountMonitoring()
