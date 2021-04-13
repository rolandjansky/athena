#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import parOR
from AthenaCommon.CFElements import seqAND
from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlgMT, TrigStreamerHypoToolMT


def ALFAPerfSequence():
    from TrigOnlineMonitor.TrigOnlineMonitorConfig import TrigALFAROBMonitor
    from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
    inputMakerAlg = InputMakerForRoI("IM_ALFAPerf", RoITool = ViewCreatorInitialROITool() )
#        inputMaker.RoIs="TimeBurnerInputRoIs"

    reco = parOR("ALFAPerfReco", [TrigALFAROBMonitor()])
    hypoAlg = TrigStreamerHypoAlgMT("ALFAPerfHypo")
    hypoAlg.RuntimeValidation = False 
    def alwaysRejectHypoToolGen(chainDict):
        return TrigStreamerHypoToolMT(chainDict["chainName"], Pass = False)

    viewSeq = seqAND("ALFAPerfRecoView", [inputMakerAlg, reco])

    return MenuSequence(Sequence    = viewSeq,
                        Maker       = inputMakerAlg,
                        Hypo        = hypoAlg,
                        HypoToolGen = alwaysRejectHypoToolGen)