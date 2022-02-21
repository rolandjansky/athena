# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from TriggerMenuMT.HLT.Config.MenuComponents import Chain
from .ConfigHelpers import AlgConfig
from .METChainConfiguration import extractMETRecoDict
from TriggerMenuMT.HLT.Config.Utility.ChainMerging import mergeChainDefs
from AthenaCommon.Logging import logging

log = logging.getLogger(__name__)

def generateChains(flags, chainDict):
    log.debug("Received chain dict %s", chainDict)
    parts = []
    for subDict in chainDict["chainParts"]:
        if subDict["signature"] != "MET":
            raise ValueError(f"Non-MET signature '{subDict['signature']}' received!")
        recoDict = extractMETRecoDict(subDict)
        config = AlgConfig.fromRecoDict(**recoDict)
        parts.append(
            Chain(
                name=chainDict["chainName"],
                L1Thresholds=[subDict["L1threshold"]],
                ChainSteps=config.make_accumulator_steps(flags, chainDict),
                alignmentGroups=[subDict["alignmentGroup"]],
            )
        )
    return mergeChainDefs(parts, chainDict)
