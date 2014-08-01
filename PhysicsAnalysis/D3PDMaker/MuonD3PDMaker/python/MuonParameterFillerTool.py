# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonD3PDMaker import MuonD3PDMakerConf



class MuonParameterFillerTool (MuonD3PDMakerConf.D3PD__MuonParameterFillerTool):
    def __init__ (self,
                  name,
                  Parameters = [],
                  **kwargs):
        Parameters = [str(x) for x in Parameters]
        MuonD3PDMakerConf.D3PD__MuonParameterFillerTool.__init__ (self,
                                                             name,
                                                             Parameters = Parameters,
                                                             **kwargs)
        
