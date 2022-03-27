#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR
from AthenaCommon.CFElements import seqAND
from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlg, TrigStreamerHypoTool


def ALFAPerfSequence():
    from TrigOnlineMonitor.TrigOnlineMonitorConfig import TrigALFAROBMonitor
    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
    inputMakerAlg = InputMakerForRoI("IM_ALFAPerf", RoITool = ViewCreatorInitialROITool() )
#        inputMaker.RoIs="TimeBurnerInputRoIs"

    reco = parOR("ALFAPerfReco", [TrigALFAROBMonitor()])
    hypoAlg = TrigStreamerHypoAlg("ALFAPerfHypo")
    hypoAlg.RuntimeValidation = False 
    def alwaysRejectHypoToolGen(chainDict):
        return TrigStreamerHypoTool(chainDict["chainName"], Pass = False)

    viewSeq = seqAND("ALFAPerfRecoView", [inputMakerAlg, reco])

    return MenuSequence(Sequence    = viewSeq,
                        Maker       = inputMakerAlg,
                        Hypo        = hypoAlg,
                        HypoToolGen = alwaysRejectHypoToolGen)