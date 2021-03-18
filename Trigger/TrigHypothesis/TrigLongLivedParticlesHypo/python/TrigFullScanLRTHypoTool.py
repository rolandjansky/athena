# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigLRTHypoTool')

def TrigLRTHypoToolFromDict( chainDict ):
    """ Use menu decoded chain dictionary to configure the tool """

    name = chainDict['chainName']
    from AthenaConfiguration.ComponentFactory import CompFactory
    tool = CompFactory.FastTrackFinderLRTHypoTool(name)

    return tool


def TrigLRTHypoToolFromName( name, conf ):
    """ provides configuration of the hypo tool giben the chain name
    The argument will be replaced by "parsed" chain dict. For now it only serves simplest chain HLT_eXYZ.
    """
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName

    decodedDict = dictFromChainName(conf)

    return TrigLRTHypoToolFromDict( decodedDict )



if __name__ == "__main__":
    tool = TrigLRTHypoToolFromDict("HLT_unconvtrk50_isohpttrack_imedium_L1XE50", "HLT_unconvtrk50_isohpttrack_imedium_L1XE50")
    assert tool, "Not configured simple tool"


    log.info("ALL OK")
