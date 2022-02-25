# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

def TrigMETCellHypoToolFromDict(chainDict):
    """ Configure tool operating on met from cells"""
    # note for future developers, it seems that the chainDict has the information about the type of alg, it would be god to use it
    # not calling the class above as it tries to parse the name back
    # also there seems no property to decide if it is met from cells yet, not setting it therefore
    # possibly there would be only one function if the met source is available in the chainDict and settable tool property
    
    tool = CompFactory.TrigMissingETHypoTool( chainDict['chainName'] )
    tool.metThreshold = int(chainDict['chainParts'][0]['threshold'])
    
    return tool
            

def TrigMETCellHypoToolFromName(name, conf):
    from TriggerMenuMT.HLT.Config.Utility.DictFromChainName import dictFromChainName
    
    decodedDict = dictFromChainName(conf)
    decodedDict['chainName'] = name
    return TrigMETCellHypoToolFromDict( decodedDict )

if __name__ == "__main__":
    confCell = TrigMETCellHypoToolFromName("HLT_xe65_L1XE50", "HLT_xe65_L1XE50")
    assert confCell, "Cell tool not configured"


    
