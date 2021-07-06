# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigmuRoI.TrigmuRoIConf import TrigmuRoI
from TrigmuRoI.TrigmuRoIMonitoring import TrigmuRoIMonitoring

class TrigmuRoIConfig (TrigmuRoI):

    __slots__ = []
    
    def __init__( self, name="TrigmuRoI" ):
        super( TrigmuRoIConfig, self ).__init__( name )

        self.MonTool = TrigmuRoIMonitoring()

        from TrigMuonRoITools.TrigMuonRoIToolsConfig import TrigMuonRoIToolCfg
        self.RoITool = TrigMuonRoIToolCfg()
