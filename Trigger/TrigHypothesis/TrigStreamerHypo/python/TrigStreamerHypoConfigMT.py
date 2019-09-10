# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlgMT, TrigStreamerHypoToolMT

class StreamerHypoAlgMT(TrigStreamerHypoAlgMT):
    __slots__ = []
    def __init__(self, name, hTools=[]):
        super( StreamerHypoAlgMT, self ).__init__( name )

        if len(hTools)!=0: 
            self.HypoTools = hTools 

class StreamerHypoToolMT(TrigStreamerHypoToolMT):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( StreamerHypoToolMT, self ).__init__( name )
        # Nothing to configure

def StreamerHypoToolMTgenerator(chainDict):
    """ Configure streamer tool from chainDict """
    tool = StreamerHypoToolMT( chainDict['chainName'] )
    return tool
            
if __name__ == "__main__":
    tool = StreamerHypoToolMT('HLT_noalg_L1RD0_FILLED')
    assert tool, "tool not configured"
