#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
createHLTMenuExternally=True
doWriteRDOTrigger = False
doWriteBS = False
include("TriggerJobOpts/runHLT_standalone.py")

from TrigUpgradeTest.TestUtils import makeChain
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm

chainName = "HLT_mb_sptrk_L1RD0_FILLED"


from TrigInDetConfig.InDetSetup import makeInDetAlgs
from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection, mapThresholdToL1DecisionCollection

idAlgs = makeInDetAlgs(whichSignature='MinBias', separateTrackParticleCreator='MinBias', rois=mapThresholdToL1RoICollection('FSNOSEED'))

from TrigT2MinBias.TrigT2MinBiasConf import TrigCountSpacePointsMT, SPCountHypoAlgMT, SPCountHypoTool
SpCount=TrigCountSpacePointsMT()
SpCount.OutputLevel= DEBUG
SpCount.SpacePointsKey="HLT_SpacePointCounts"

SpCountHypo = SPCountHypoAlgMT()
SpCountHypo.OutputLevel= DEBUG


def generateSPCountHypo(chainDict):
        hypo = SPCountHypoTool(chainDict["chainName"])
        # will set here thresholds
        return hypo


SpCountHypo.HypoOutputDecisions="SPDecisions"
SpCountHypo.SpacePointsKey="HLT_SpacePointCounts"

from TrigMinBias.TrigMinBiasConf import TrackCountHypoAlgMT, TrackCountHypoTool
TrackCountHypo=TrackCountHypoAlgMT()
TrackCountHypo.OutputLevel= DEBUG


TrackCountHypo.HypoInputDecisions="SPDecisions"
TrackCountHypo.HypoOutputDecisions="TrackCountDecisions"
TrackCountHypo.tracksKey="HLT_IDTrack_MinBias"
TrackCountHypo.trackCountKey="HLT_TrackCount"


from TrigMinBias.TrackCountMonitoringMT import TrackCountMonitoring
TrackCountHypo.MonTool = TrackCountMonitoring()

def generateTrackCountHypo(chainDict):
        hypo = TrackCountHypoTool(chainDict["chainName"])
        # will set here cuts
        return hypo

############### build menu
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR
from DecisionHandling.DecisionHandlingConf  import InputMakerForRoI

# TODO - split ID into two parts, one for SP counting another for the tracking, the tracking will go to the second step (Tracking) sequence
inputMakerSPCount = InputMakerForRoI(name="IMMinBiasSPCount", RoIs="HLT_MBSP")
stepSPCount = ChainStep( "stepSPCount",  [MenuSequence( Maker=inputMakerSPCount,
                                                        Sequence=parOR("SPCountReco", [inputMakerSPCount]+idAlgs + [ SpCount ] ),
                                                        Hypo=SpCountHypo, HypoToolGen=generateSPCountHypo )] )


inputMakerTrkCount = InputMakerForRoI(name="IMMinBiasTrkCount", RoIs="HLT_MBTRK")
stepTrkCount = ChainStep( "stepTrkCount",
                          [ MenuSequence( Maker=inputMakerTrkCount ,
                                          Sequence=parOR("TrkCountReco", [inputMakerTrkCount]),
                                          Hypo=TrackCountHypo,
                                          HypoToolGen=generateTrackCountHypo )] )

makeChain(chainName, ["FSNOSEED"], ChainSteps=[stepSPCount, stepTrkCount])

from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig import makeHLTTree
from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
makeHLTTree( triggerConfigHLT=TriggerConfigHLT )


from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON
generateJSON()
