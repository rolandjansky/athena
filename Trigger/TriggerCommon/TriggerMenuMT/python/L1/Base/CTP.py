# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from AthenaCommon.Logging import logging
log = logging.getLogger("Menu.L1.Base.CTP") 

from ..Config.MonitorDef import MonitorDef
from ..Config.CTPInputConfig import CTPInputConfig
from .BunchGroupSet import BunchGroupSet
from .MonCounters import MenuMonCountersCollection

class CTP(object):

    def __init__(self):
        self.inputConnectors = CTPInputConfig.cablingLayout()
        self.random          = Random( names = ['Random0', 'Random1', 'Random2', 'Random3'], cuts = [1, 1, 1, 1] )
        self.bunchGroupSet   = BunchGroupSet()
        self.counters        = MenuMonCountersCollection()   # monitoring counters in the menu
        
        
    def setBunchGroupSetName(self, name):
        self.bunchGroupSet.name = name
        return self.bunchGroupSet

    def addBunchGroup(self, name, internalNumber, bunches):
        self.bunchGroupSet.addBunchGroup(name, internalNumber, bunches)


    def setupMonitoring(self, menuItems, menuThresholds):
        ##  # add the CTPIN counters
        ##  for counter in MonitorDef.ctpinCounters( menuThresholds ):
        ##      self.counters.addCounter( counter )

        # add the CTPMon counters
        for counter in MonitorDef.ctpmonCounters( menuThresholds ):
            self.counters.addCounter( counter )

        # mark the L1 Items that they should be monitored
        MonitorDef.applyItemCounter( menuItems )
        pass

    def json(self):
        confObj = odict()
        confObj["inputs"] = self.inputConnectors
        confObj["monitoring"] = self.counters.json()
        return confObj



class Random(object):
    def __init__(self, names, cuts):
        self.names = names
        self.cuts  = cuts

