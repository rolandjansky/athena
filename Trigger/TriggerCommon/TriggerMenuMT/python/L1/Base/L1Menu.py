# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from .CTP import CTP
from .Items import MenuItemsCollection
from .Thresholds import MenuThresholdsCollection
from .TopoAlgorithms import MenuTopoAlgorithmsCollection
from .Boards import MenuBoardsCollection
from .Connectors import MenuConnectorsCollection
from .MenuUtils import get_smk_psk_Name
from .Limits import Limits

from AthenaCommon.Logging import logging
log = logging.getLogger("Menu.L1.Base.L1Menu")

class L1Menu(object):
    """
    This class holds everything that is needed to define the menu
    """

    def __init__(self, menuName):
        self.menuName = menuName

        # items in menu
        self.items = MenuItemsCollection()
        
        # all thresholds that are in menu (new and legacy)
        self.thresholds = MenuThresholdsCollection()

        # all thresholds that are in menu (new and legacy)
        self.topoAlgos = MenuTopoAlgorithmsCollection()

        # all connectors between legacyCalo, muctpi, topo and the CTPIN/CTPCORE
        self.connectors = MenuConnectorsCollection()

        # board definition
        self.boards = MenuBoardsCollection()

        # CTP Info in the menu
        self.ctp = CTP()
        
        if self.menuName:
            smk_psk_Name = get_smk_psk_Name(self.menuName)
            self.items.menuName = smk_psk_Name["smkName"]
            self.items.pssName  = smk_psk_Name["pskName"]

    @staticmethod
    def partitioning():
        from Lvl1Flags import Lvl1Flags
        first = Lvl1Flags.MenuPartitioning()
        last = first[1:] + [ Limits.MaxTrigItems ]
        partitioning = dict( zip([1,2,3],zip(first,last)) )
        return partitioning

    def setBunchGroupSplitting(self, v = True):
        MenuItemsCollection.splitBunchGroups = v


    def addItem(self, item):
        self.items += item


    def getItem(self,name):
        return self.items.findItemByName(name)


    def addThreshold(self, threshold):
        self.thresholds += threshold


    def addTopoAlgo(self, algo, category):
        algo.setThresholds( self.thresholds ) # each algo gets a pointer to the full thresholds definition (for the extrainfo)
        self.topoAlgos.addAlgo(algo, category)


    def addBoard(self, boardDef):
        return self.boards.addBoard(boardDef) 


    def addConnector(self, connDef):
        self.connectors.addConnector(connDef)


    def setupCTPMonitoring(self):
        self.ctp.setupMonitoring(self.items, self.thresholds)
        
    def check(self):
        from collections import defaultdict as dd
        missing = dd(list)
        allThresholds = set([thr.name for thr in self.thresholds])
        allUsedThresholds = set()
        for item in self.items:
            for thrName in item.thresholdNames():
                allUsedThresholds.add(thrName)
                if thrName not in allThresholds:
                    missing[thrName].append(item.name) 
                
        for thrName in sorted(missing.keys()):
            log.warning("Threshold %s (used by %s) is not defined in the menu", thrName,",".join(missing[thrName]))

        if len(missing)>0:
            unusedThresholds = allThresholds.difference(allUsedThresholds)
            log.info("The following thresholds are unused")
            log.info("EM: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("EM")]))
            log.info("HA: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("HA")]))
            log.info("J: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("J")]))
        
