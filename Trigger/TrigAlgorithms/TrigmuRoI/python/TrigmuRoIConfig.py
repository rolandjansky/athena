# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigmuRoI.TrigmuRoIConf import TrigmuRoIMT
from TrigmuRoI.TrigmuRoIMonitoring import TrigmuRoIMTMonitoring

class TrigmuRoIMTConfig (TrigmuRoIMT):

    __slots__ = []
    
    def __init__( self, name="TrigmuRoIMT" ):
        super( TrigmuRoIMTConfig, self ).__init__( name )

        self.MonTool = TrigmuRoIMTMonitoring()

        from TrigMuonRoITools.TrigMuonRoIToolsConfig import TrigMuonRoIToolCfg
        self.RoITool = TrigMuonRoIToolCfg()
