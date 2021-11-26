# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from .CTP import CTP
from .Items import MenuItemsCollection
from .Thresholds import MenuThresholdsCollection
from .TopoAlgorithms import MenuTopoAlgorithmsCollection
from .Boards import MenuBoardsCollection
from .Connectors import MenuConnectorsCollection, CType
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
        self.ctp.setupMonitoring(self.menuName, self.items, self.thresholds, self.connectors)
        
    def check(self):
        log.info("Doing L1 Menu checks")
        from collections import defaultdict as dd
        missing = dd(list)
        allThresholds = set([thr.name for thr in self.thresholds])
        allUsedThresholds = set()
        for item in self.items:
            for thrName in item.thresholdNames():
                if 'SPARE' in thrName:
                    raise RuntimeError("CTP input %s is used by %s but SPARE thresholds are not to be used!" %(thrName, item) )
                if thrName not in allThresholds:
                    missing[thrName].append(item.name) 
                else:
                    allUsedThresholds.add(thrName)
                
        for thrName in sorted(missing.keys()):
            log.warning("Threshold %s (used by %s) is not defined in the menu", thrName,",".join(missing[thrName]))

        if len(allThresholds)-len(allUsedThresholds)>0:
            unusedThresholds = allThresholds.difference(allUsedThresholds)
            log.debug("The following thresholds are unused")
            log.debug("MU: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("MU")]))
            log.debug("EM: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("EM")]))
            log.debug("HA: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("HA")]))
            log.debug("J: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("J")]))
            log.debug("eFEX: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("e")]))
            log.debug("jFEX: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("j")]))
            log.debug("cTAU: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("cTAU")]))
            log.debug("gFEX: %s", ", ".join([thr for thr in unusedThresholds if thr.startswith("g")]))

    def checkLegacyThresholds(self):
        from collections import defaultdict as dd 
        from ..Menu.LegacyMenuThresholds import legacyThresholds
        extraThresholds = dd(list)
        for item in self.items:
            for thrName in item.thresholdNames():
                if thrName[:3]=='ZB_':
                    thrName = thrName[3:]
                if thrName[0] not in ('e','j','g', 'c') and thrName[:2] not in ["MU"] and "TOPO" not in thrName[:4]:
                    if thrName not in legacyThresholds:
                        extraThresholds[thrName].append(item.name)

        for thrName in sorted(extraThresholds.keys()):
            log.warning("Threshold %s (used by %s) should not be used!", thrName,",".join(extraThresholds[thrName]))

    def checkPerfThresholds(self):
        if 'MC' not in self.menuName:
            from collections import defaultdict as dd
            perfThresholds = dd(list)
            for item in self.items:
                for thrName in item.thresholdNames():
                    if 'Perf' in thrName:
                        perfThresholds[thrName].append(item.name)
            for thrName in sorted(perfThresholds.keys()):
                raise RuntimeError("Threshold %s (used by %s) should not be used!", thrName,",".join(perfThresholds[thrName]))

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


    def checkCountCTPInputsOutput(self):
        from collections import namedtuple
        ctpInput = namedtuple('ctpInput',"name, conn, nbit")
        ctpInputs = []
        ctpOutputs = []
        thrNames = [] 
        ctpInputBitSets = dict()
        ctpInputNameSets = dict()
        for item in self.items:
            ctpOutputs.append(item.name)
            for thrName in item.thresholdNames():
                if thrName[:3]=='ZB_':
                    thrName = thrName[3:]
                if thrName not in thrNames:
                    thrNames.append(thrName)
        for thrName in thrNames:
            for conn in self.connectors:
                if conn.ctype != CType.ELEC:
                    for tl in conn.triggerLines:
                        if thrName == tl.name:
                            ctpInputs.append(ctpInput(name=thrName,conn=conn.name,nbit=tl.nbits))
                else:
                     for fpga in conn.triggerLines:
                        for clock in conn.triggerLines[fpga]:
                            for tl in conn.triggerLines[fpga][clock]:
                                if thrName == tl.name:
                                    ctpInputs.append(ctpInput(name=thrName,conn=conn.name,nbit=tl.nbits))

        if len(thrNames) != len(ctpInputs):
            raise RuntimeError("Not all input thresholds found!")

        for ctpIn in ctpInputs:
            thrset = None
            thrName = ctpIn.name
            if thrName[:2] in ['EM','HA','XE','TE','XS']:
                thrset = 'legacyCalo'
            elif thrName[:1]=='J':
                thrset = 'legacyCalo'
            elif thrName[:2]=='MU':
                thrset = 'muon'
            elif thrName[:3] in ['ALF', 'MBT','AFP','BCM','CAL','NIM','ZDC','BPT','LUC']:
                thrset = 'detector'
            elif thrName[:6]=='R2TOPO':
                thrset = 'legacyTopo'
            elif thrName[:1] in ['e','j','c','g']:
                thrset = 'topo1'
            elif thrName[:4]=='TOPO':
                if 'Topo2' in ctpIn.conn:
                    thrset = 'topo2'
                elif 'Topo3' in ctpIn.conn:
                    thrset = 'topo3'

            if thrset not in ctpInputBitSets:
                ctpInputBitSets[thrset] = 0
                ctpInputNameSets[thrset] = []
            if thrName not in ctpInputNameSets[thrset]:
                ctpInputNameSets[thrset].append(thrName)
                ctpInputBitSets[thrset] += ctpIn.nbit

        totalInputs = 0
        log.info("Check total number of CTP input and output bits:")
        log.info("Number of output bits: %i", len(ctpOutputs) )
        for thrset in ctpInputBitSets:
            log.info("%s: %i thresholds and %i  bits", thrset, len(ctpInputNameSets[thrset]), ctpInputBitSets[thrset]  )
            if thrset is not None:
                log.debug("Threshold set %s: %s", thrset, ",".join(ctpInputNameSets[thrset]) )
            else:
                log.info("Unrecognised CTP input bits: %s", ",".join(ctpInputNameSets[thrset]) )
            totalInputs += ctpInputBitSets[thrset]
        log.info("Number of inputs bits: %i" , totalInputs )

        # Fail menu generation for menus going to P1:
        if ( totalInputs > 512 or len(ctpOutputs) > 512 ):
            if L1MenuFlags.ApplyCTPLimits():
                raise RuntimeError("Both the numbers of inputs and outputs need to be not greater than 512 in a physics menu!")

