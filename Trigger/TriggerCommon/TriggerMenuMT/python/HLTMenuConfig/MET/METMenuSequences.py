#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from ..Menu.MenuComponents import MenuSequence, RecoFragmentsPool

from TrigMissingETHypo.TrigMissingETHypoConfigMT import TrigMETCellHypoToolFromDict
from TrigMissingETHypo.TrigMissingETHypoConf import TrigMissingETHypoAlgMT

from .METRecoSequences import metAthSequence
from .ConfigHelpers import metRecoDictToString

def metMenuSequence(dummyFlags, **recoDict):
    recoSeq, inputSeq, sequenceOut = RecoFragmentsPool.retrieve(
            metAthSequence, None, **recoDict)

    hypoAlg = TrigMissingETHypoAlgMT(
            "METHypoAlg_{}".format(metRecoDictToString(recoDict)),
            METContainerKey = sequenceOut)

    return MenuSequence(Sequence    = recoSeq,
                        Maker       = inputSeq,
                        Hypo        = hypoAlg,
                        # TODO: Fix this!!
                        HypoToolGen = TrigMETCellHypoToolFromDict)
