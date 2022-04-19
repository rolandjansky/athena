# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

def TrigMETHypoToolFromDict(chainDict):
    """Configure the Missing ET hypo tool"""
    return CompFactory.TrigMissingETHypoTool(
            chainDict["chainName"],
            metThreshold = int(chainDict["chainParts"][0]["threshold"]),
    )
