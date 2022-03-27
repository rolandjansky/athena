# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigLRTHypoTool')

def TrigLRTHypoToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """

    name = chainDict['chainName']
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.FastTrackFinderLRTHypoTool(name)

    return tool
