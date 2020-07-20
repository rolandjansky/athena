# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigIDTPMonitor.TrigIDTPMonitorConf import TrigIDTPMonitor, TrigIDTPMonitorElectron

class IDTPMonitor (TrigIDTPMonitor):
    __slots__ = []
    def __init__(self, name = "IDTPMonitor"):
        super( TrigIDTPMonitor, self ).__init__( name )
        
        from TrigIDTPMonitor.TrigIDTPMonitorMonitoring import TrigIDTPMonitorOnlineMonitor
        online = TrigIDTPMonitorOnlineMonitor()

        from TrigIDTPMonitor.TrigIDTPMonitorMonitoring import TrigIDTPMonitorValidationMonitor
        validation = TrigIDTPMonitorValidationMonitor()

        self.AthenaMonTools = [ validation, online ]

class IDTPMonitorElectron (TrigIDTPMonitorElectron):
    __slots__ = []
    def __init__(self, name = "IDTPMonitorElectron"):
        super( TrigIDTPMonitorElectron, self ).__init__( name )
        
        from TrigIDTPMonitor.TrigIDTPMonitorMonitoring import TrigIDTPMonitorElectronOnlineMonitor
        online = TrigIDTPMonitorElectronOnlineMonitor()

        from TrigIDTPMonitor.TrigIDTPMonitorMonitoring import TrigIDTPMonitorElectronValidationMonitor
        validation = TrigIDTPMonitorElectronValidationMonitor()

        self.AthenaMonTools = [ validation, online ]

