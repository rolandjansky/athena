from TrigT2MinBias.TrigT2MinBiasConf import MbtsFexMT
from TrigT2MinBias.TrigT2MinBiasMonitoringMT import MbtsFexMTMonitoring

class MbtsFexMT( MbtsFexMT ):
    __slots__ = []
    def __init__ (self, name ):
        super(MbtsFexMT, self).__init__(name)
        # self.Threshold = trigT2MinBiasProperties.mbtsThreshold() # pC
        # time = TrigTimeHistToolConfig("MbtsFexMTTimers")
        # time.NumberOfHistBins = 100
        # time.TimerHistLimits = [0, 50]
        self.AthenaMonTools += [ MbtsFexMTMonitoring()]

L2MbtsFexMT = MbtsFexMT("L2MbtsFexMT")
fexes["L2MbtsFexMT"] = L2MbtsFexMT
