# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
        self.topoAlgos.addAlgo(algo, category)


    def addBoard(self, boardDef):
        return self.boards.addBoard(boardDef) 


    def addConnector(self, connDef):
        self.connectors.addConnector(connDef)


    def setupCTPMonitoring(self):
        self.ctp.setupMonitoring(self.items, self.thresholds)
        
