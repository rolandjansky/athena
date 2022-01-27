# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

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

    def setupMonitoring(self, menuName, menuItems, menuThresholds, connectors):
        ##  # add the CTPIN counters
        ##  for counter in MonitorDef.ctpinCounters( menuThresholds ):
        ##      self.counters.addCounter( counter )

        # add the CTPMon counters (selection defined in L1/Config/MonitorDef.py)
        for counter in MonitorDef.ctpmonCounters( menuThresholds, connectors ):
            self.counters.addCounter( counter )

        # add the CTPIN counters (selection defined in L1/Config/MonitorDef.py)
        for counter in MonitorDef.ctpinCounters( menuThresholds, connectors, self.inputConnectors["ctpin"] ):
            self.counters.addCounter( counter )

        # mark the L1 Items that they should be monitored
        MonitorDef.applyItemCounter( menuName, menuItems )
        pass

    def checkConnectorAvailability(self, availableConnectors, menuToLoad):
        inputConnectorList = []
        inputConnectorList += self.inputConnectors["optical"].values()
        inputConnectorList += self.inputConnectors["electrical"].values()
        inputConnectorList += self.inputConnectors["ctpin"]["slot7"].values()
        inputConnectorList += self.inputConnectors["ctpin"]["slot8"].values()
        inputConnectorList += self.inputConnectors["ctpin"]["slot9"].values()
        for connName in inputConnectorList:
            if connName != '' and connName not in availableConnectors:
                msg = "Connector '%s' requested in L1/Config/CTPConfig.py not defined as menu input. Please add it to L1/Menu/Menu_%s.py" % (connName, menuToLoad[1])
                log.error(msg)
                raise RuntimeError(msg)

    def json(self):
        confObj = odict()
        confObj["inputs"] = self.inputConnectors
        confObj["monitoring"] = self.counters.json()
        return confObj



class Random(object):
    def __init__(self, names, cuts):
        self.names = names
        self.cuts  = cuts

