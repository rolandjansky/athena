# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from TrigBphysHypo.TrigBphysHypoConf import TrigBphysStreamerHypoTool

def TrigBphysStreamerHypoToolFromDict(chainDict):
    tool = CompFactory.TrigBphysStreamerHypoTool(chainDict['chainName'])
    return tool
