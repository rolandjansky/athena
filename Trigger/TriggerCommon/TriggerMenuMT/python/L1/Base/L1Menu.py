# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from .CTP import CTP
from .Items import MenuItemsCollection
from .Thresholds import MenuThresholdsCollection
from .TopoAlgorithms import MenuTopoAlgorithmsCollection
from .Boards import MenuBoardsCollection
from .Connectors import MenuConnectorsCollection
from .MenuUtils import get_smk_psk_Name
from .Limits import Limits
from .L1MenuFlags import L1MenuFlags

from collections import OrderedDict as odict
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

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
        first = L1MenuFlags.MenuPartitioning()
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
        self.ctp.setupMonitoring(self.items, self.thresholds, self.connectors)
        
    def check(self):
        log.info("Doing L1 Menu checks")
        from collections import defaultdict as dd
        missing = dd(list)
        allThresholds = set([thr.name for thr in self.thresholds])
        allUsedThresholds = set()
        for item in self.items:
            for thrName in item.thresholdNames():
                if thrName not in allThresholds:
                    missing[thrName].append(item.name) 
                else:
                    allUsedThresholds.add(thrName)
                
        for thrName in sorted(missing.keys()):
            log.warning("Threshold %s (used by %s) is not defined in the menu", thrName,",".join(missing[thrName]))

        if len(allThresholds)-len(allUsedThresholds)>0:
            unusedThresholds = allThresholds.difference(allUsedThresholds)
            log.info("The following thresholds are unused")
            log.info("MU: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("MU")]))
            log.info("EM: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("EM")]))
            log.info("HA: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("HA")]))
            log.info("J: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("J")]))
            log.info("eFEX: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("e")]))
            log.info("jFEX: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("j")]))
            log.info("cTAU: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("cTAU")]))
            log.info("gFEX: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("g")]))

    def checkLegacyThresholds(self):
        from collections import defaultdict as dd 
        from ..Menu.LegacyMenuThresholds import legacyThresholds
        extraThresholds = dd(list)
        for item in self.items:
            for thrName in item.thresholdNames():
                if thrName[0] not in ('e','j','g', 'c') and not any(x in thrName for x in ["TOPO", "MU", "MBTS", "ZB", "ALFA", "ZDC", "AFP", "BCM"]):
                    if thrName not in legacyThresholds:
                        extraThresholds[thrName].append(item.name)

        for thrName in sorted(extraThresholds.keys()):
            log.warning("Threshold %s (used by %s) should not be used!", thrName,",".join(extraThresholds[thrName]))


    def checkBoardInputs(self, algo, connDefName, fpgaName ):
        if 'MuCTPi' in connDefName or 'Legacy' in connDefName:
            return
        boardName = connDefName+fpgaName

        allowedInputs = odict()
        allowedInputs['Topo1Opt0'] = ['MU', 'eEM', 'eTAU',              'gJ',  'gLJ',               ] # TOPO1A, FPGA1
        allowedInputs['Topo1Opt1'] = ['MU', 'eEM', 'eTAU',              'gJ',  'gLJ',               ] # TOPO1A, FPGA2
        allowedInputs['Topo1Opt2'] = ['MU',        'eTAU', 'cTAU', 'j',               'gXE', 'gTE', ] # TOPO1B, FPGA1
        allowedInputs['Topo1Opt3'] = ['MU',        'eTAU', 'cTAU', 'j',               'gXE', 'gTE', ] # TOPO1B, FPGA2
        allowedInputs['Topo2El0']  = ['MU',        'eTAU',         'j',      ] # TOPO2, FPGA1
        allowedInputs['Topo2El1']  = [      'eEM',                 'j',      ] # TOPO2, FPGA2
        allowedInputs['Topo3El0']  = [      'eEM', 'eTAU',         'j',      ] # TOPO3, FPGA1
        allowedInputs['Topo3El1']  = ['MU', 'eEM', 'eTAU',              'g', ] # TOPO3, FPGA2

        if boardName not in allowedInputs.keys():
            raise RuntimeError("Connector name %s not found" % boardName ) 

        if 'Mult_' in algo.name:
            if not (any(x in algo.threshold for x in allowedInputs[boardName])):
                raise RuntimeError("Algorithm %s in board %s with threshold %s not allowed" % (algo.name, boardName, algo.threshold )) 

        if 'Mult_' not in algo.name:
            for algoInput in algo.inputs:
                if not (any(x in algoInput for x in allowedInputs[boardName])):
                     raise RuntimeError("Algorithm %s in board %s with input %s not allowed" % (algo.name, boardName, algoInput ))


