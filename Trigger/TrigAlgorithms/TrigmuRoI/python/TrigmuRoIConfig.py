# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigmuRoI.TrigmuRoIConf import TrigmuRoI, TrigmuRoIMT
from TrigmuRoI.TrigmuRoIMonitoring import TrigmuRoIValidationMonitoring, TrigmuRoIOnlineMonitoring, TrigmuRoICosmicMonitoring, TrigmuRoIMTMonitoring
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig

class TrigmuRoIConfig (TrigmuRoI):

    __slots__ = []
    

    def __init__( self, name="TrigmuRoI" ):
        super( TrigmuRoIConfig, self ).__init__( name )

        validation = TrigmuRoIValidationMonitoring()
        online     = TrigmuRoIOnlineMonitoring()
        cosmic     = TrigmuRoICosmicMonitoring()

        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 5]

        self.AthenaMonTools = [ validation, online, cosmic, time ]

class TrigmuRoIMTConfig (TrigmuRoIMT):

    __slots__ = []
    
    def __init__( self, name="TrigmuRoIMT" ):
        super( TrigmuRoIMTConfig, self ).__init__( name )

        self.MonTool = TrigmuRoIMTMonitoring()

        from TrigMuonRoITools.TrigMuonRoIToolsConfig import TrigMuonRoIToolCfg
        self.RoITool = TrigMuonRoIToolCfg()
