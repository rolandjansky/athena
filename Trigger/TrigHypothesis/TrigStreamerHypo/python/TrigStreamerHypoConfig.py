# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlg, TrigStreamerHypoTool

class StreamerHypoAlg(TrigStreamerHypoAlg):
    __slots__ = []
    def __init__(self, name, hTools=[]):
        super( StreamerHypoAlg, self ).__init__( name )

        if len(hTools)!=0: 
            self.HypoTools = hTools 

class StreamerHypoTool(TrigStreamerHypoTool):
    __slots__ = []
    def __init__(self, name, **kwargs):
        super( StreamerHypoTool, self ).__init__( name )
        # Nothing to configure

def StreamerHypoToolGenerator(chainDict):
    """ Configure streamer tool from chainDict """
    tool = StreamerHypoTool( chainDict['chainName'] )
    return tool
            
if __name__ == "__main__":
    tool = StreamerHypoTool('HLT_noalg_L1RD0_FILLED')
    assert tool, "tool not configured"
