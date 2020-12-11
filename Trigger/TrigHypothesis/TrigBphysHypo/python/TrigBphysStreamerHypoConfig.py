# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigBphysHypo.TrigBphysHypoConf import TrigBphysStreamerHypoTool

def TrigBphysStreamerHypoToolFromDict(chainDict):
    tool = TrigBphysStreamerHypoTool(chainDict['chainName'])
    return tool
