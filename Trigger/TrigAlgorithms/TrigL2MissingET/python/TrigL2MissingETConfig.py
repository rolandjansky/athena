# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigL2MissingET.TrigL2MissingETConf import PESA__T2MissingET, T2CaloMissingET



class L2MissingET_Fex ( PESA__T2MissingET ) :
    __slots__ = []
    def __init__ (self, name="L2MissingET_Fex"):
        super(L2MissingET_Fex, self).__init__(name)

        # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
        from TrigL2MissingET.TrigL2MissingETMonitoring import T2MissingETOnlineMonitoring, T2MissingETCosmicMonitoring, T2MissingETValidationMonitoring
        validation = T2MissingETValidationMonitoring()
        online     = T2MissingETOnlineMonitoring()
        cosmic     = T2MissingETCosmicMonitoring() 

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 50]
        self.METLabel = "T2MissingET"
        self.AthenaMonTools = [ time, validation, online, cosmic ]
        self.DecodeDetMask=True

class L2CaloMissingET_Fex ( T2CaloMissingET ) :
    __slots__ = []
    def __init__ (self, name="L2CaloMissingET_Fex"):
        super(L2CaloMissingET_Fex, self).__init__(name)

        # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
        from TrigL2MissingET.TrigL2MissingETMonitoring import T2CaloMissingETOnlineMonitoring, T2CaloMissingETValidationMonitoring
        validation = T2CaloMissingETValidationMonitoring()
        online     = T2CaloMissingETOnlineMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 2]
        time.NumberOfHistBins=100
        self.OneByOne=False
        self.ReadL2L1=False
        self.doTiming=True
        #self.OneByOne=True;
        self.METLabel = "TrigL2MissingET_FEB"
        self.AthenaMonTools = [ online,validation,time ]
        #self.AthenaMonTools = [ online,time ]

class L2CaloMissingET_Fex_ReadL2L1 ( T2CaloMissingET ) :
    __slots__ = []
    def __init__ (self, name="L2CaloMissingET_Fex_ReadL2L1"):
        super(L2CaloMissingET_Fex_ReadL2L1, self).__init__(name)

        # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
        from TrigL2MissingET.TrigL2MissingETMonitoring import T2CaloMissingETOnlineMonitoring, T2CaloMissingETValidationMonitoring
        validation = T2CaloMissingETValidationMonitoring()
        online     = T2CaloMissingETOnlineMonitoring()
        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 2]
        time.NumberOfHistBins=100
        self.OneByOne=False
        self.ReadL2L1=True
        self.doTiming=True
        #self.OneByOne=True
        self.METLabel = "TrigL2MissingET_FEB"
        self.AthenaMonTools = [ online,validation,time ]
        #self.AthenaMonTools = [ online,time ]
        
        
