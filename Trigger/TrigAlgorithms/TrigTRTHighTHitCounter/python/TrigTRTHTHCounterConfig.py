# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigTRTHighTHitCounter.TrigTRTHighTHitCounterConf import TrigTRTHTHCounter
#from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class TrigTRTHTHCounterFex(TrigTRTHTHCounter):
    __slots__ = []
    def __init__(self, name="TrigTRTHTHCounterFex"):
        super(TrigTRTHTHCounterFex,self).__init__(name)
        self.EtaHalfWidth =  0.1
        self.PhiHalfWidth = 0.1
        self.doFullScan =  False
        self.RoadWidth = 4
        self.nBinCoarse = 14
        self.nBinFine = 14
        self.WedgeMinEta = 0
        self.RoadMaxEta = 1.06
        self.WedgeNBin = 5
        
        #monTool = GenericMonitoringTool('MonTool')
        #self.MonTool = monTool
