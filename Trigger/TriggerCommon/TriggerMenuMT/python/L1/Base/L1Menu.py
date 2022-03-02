# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from .CTP import CTP
from .Items import MenuItemsCollection
from .Thresholds import MenuThresholdsCollection
from .TopoAlgorithms import MenuTopoAlgorithmsCollection, AlgType, AlgCategory
from .Boards import MenuBoardsCollection
from .Connectors import MenuConnectorsCollection, CType
from .MenuUtils import get_smk_psk_Name
from .Limits import Limits
from .L1MenuFlags import L1MenuFlags
from .ThresholdType import ThrType
from ..Config.TypeWideThresholdConfig import getTypeWideThresholdConfig

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
        allowedInputs['Topo1Opt0'] = ['MU', 'eEM', 'eTAU',              'gJ',  'gLJ',                     ] # TOPO1A, FPGA1
        allowedInputs['Topo1Opt1'] = ['MU', 'eEM', 'eTAU',              'gJ',  'gLJ',                     ] # TOPO1A, FPGA2
        allowedInputs['Topo1Opt2'] = ['MU',        'eTAU', 'cTAU', 'j',               'gXE', 'gTE', 'gMHT'] # TOPO1B, FPGA1
        allowedInputs['Topo1Opt3'] = ['MU',        'eTAU', 'cTAU', 'j',               'gXE', 'gTE', 'gMHT'] # TOPO1B, FPGA2
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


    def checkCTPINconnectors(self):
        for conn in self.connectors:
            if conn.ctype == CType.CTPIN:
               if len(conn.triggerLines)>31:
                   raise RuntimeError("Too many CTP inputs in %s: %i but a max of 31 are allowed" %(conn.name,len(conn.triggerLines)))

    def checkCountCTPInputsOutput(self):
        from collections import namedtuple
        ctpInput = namedtuple('ctpInput',"name, conn, nbit")
        ctpInputs = []
        ctpUnusedInputs = []
        ctpOutputs = []
        thrNames = [] 
        ctpInputBitSets = dict()
        ctpInputNameSets = dict()
        ctpUnusedInputBitSets = dict()
        ctpUnusedInputNameSets = dict()
        for item in self.items:
            ctpOutputs.append(item.name)
            for thrName in item.thresholdNames():
                if thrName[:3]=='ZB_':
                    thrName = thrName[3:]
                if thrName not in thrNames:
                    thrNames.append(thrName)
        thrNames_notFound = []
        for thrName in thrNames:
            thrName_found = False
            for conn in self.connectors:
                if conn.ctype != CType.ELEC:
                    for tl in conn.triggerLines:
                        if thrName == tl.name:
                            ctpInputs.append(ctpInput(name=thrName,conn=conn.name,nbit=tl.nbits))
                            thrName_found = True
                else:
                     for fpga in conn.triggerLines:
                        for clock in conn.triggerLines[fpga]:
                            for tl in conn.triggerLines[fpga][clock]:
                                if thrName == tl.name:
                                    ctpInputs.append(ctpInput(name=thrName,conn=conn.name,nbit=tl.nbits))
                                    thrName_found = True
            if not thrName_found:
                thrNames_notFound.append(thrName)

        for conn in self.connectors:
            if conn.ctype != CType.ELEC:
                for tl in conn.triggerLines:
                    thrName = tl.name
                    if thrName[:3]=='ZB_':
                        thrName = thrName[3:]
                    usedInput = False
                    for ctpIn in ctpInputs:
                       if thrName == ctpIn.name:
                           usedInput = True
                    if not usedInput:
                       ctpUnusedInputs.append(ctpInput(name=thrName,conn=conn.name,nbit=tl.nbits))
            else:
                for fpga in conn.triggerLines:
                    for clock in conn.triggerLines[fpga]:
                        for tl in conn.triggerLines[fpga][clock]:
                           thrName = tl.name
                           if thrName[:3]=='ZB_':
                               thrName = thrName[3:]
                           usedInput = False
                           for ctpIn in ctpInputs:
                              if thrName == ctpIn.name:
                                  usedInput = True
                           if not usedInput:
                              ctpUnusedInputs.append(ctpInput(name=thrName,conn=conn.name,nbit=tl.nbits))

        if len(thrNames_notFound)>0:
            log.error("Thresholds [%s] are not found", ",".join(thrNames_notFound)) 
            log.error("Input thresholds are [%s]", ",".join([ input.name for input in ctpInputs]))
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

        for ctpIn in ctpUnusedInputs:
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

            if thrset not in ctpUnusedInputBitSets:
                ctpUnusedInputBitSets[thrset] = 0
                ctpUnusedInputNameSets[thrset] = []
            if thrName not in ctpUnusedInputNameSets[thrset]:
                ctpUnusedInputNameSets[thrset].append(thrName)
                ctpUnusedInputBitSets[thrset] += ctpIn.nbit

        totalInputs = 0
        log.info("Check total number of CTP input and output bits:")
        log.info("Number of output bits: %i", len(ctpOutputs) )
        for thrset in ctpInputBitSets:
            log.info("Used inputs in %s: %i thresholds and %i  bits", thrset, len(ctpInputNameSets[thrset]), ctpInputBitSets[thrset]  )
            if thrset is not None:
                log.debug("Threshold set %s: %s", thrset, ",".join(ctpInputNameSets[thrset]) )
            else:
                log.info("Unrecognised CTP input bits: %s", ",".join(ctpInputNameSets[thrset]) )
            totalInputs += ctpInputBitSets[thrset]
        log.info("Number of used inputs bits: %i" , totalInputs )
        totalUnusedInputs = 0
        for thrset in ctpUnusedInputBitSets:
            log.debug("Unused thresholds in %s: %i thresholds and %i  bits", thrset, len(ctpUnusedInputNameSets[thrset]), ctpUnusedInputBitSets[thrset]  )
            if thrset is not None:
                log.debug("Unused threshold set %s: %s", thrset, ",".join(ctpUnusedInputNameSets[thrset]) )
            else:
                log.debug("Unrecognised CTP input bits: %s", ",".join(ctpUnusedInputNameSets[thrset]) )
            totalUnusedInputs += ctpUnusedInputBitSets[thrset]
        log.debug("Number of un-used inputs bits: %i" , totalUnusedInputs )

        # Fail menu generation for menus going to P1:
        if ( totalInputs > Limits.MaxTrigItems or len(ctpOutputs) > Limits.MaxTrigItems ):
            raise RuntimeError("Both the numbers of inputs and outputs need to be not greater than %i in a physics menu!" % Limits.MaxTrigItems)

    # Avoid that L1 item is defined only for BGRP0 as this include also the CALREQ BGRP2 (ATR-24781)
    def checkBGRP(self):
        for item in self.items:
            if len(item.bunchGroups)==1 and item.bunchGroups[0]=='BGRP0':
               raise RuntimeError("L1 item %s is defined with only BGRP0, ie it can trigger also in the CALREQ BGRP2 bunches. Please add another bunch group (ATR-24781)" % item.name) 


    def checkPtMinToTopo(self):
        # check that the ptMinToTopo for all types of thresholds is lower than the minimum Et cuts applied in multiplicity and decision algorithms

        # collect the ptMinToTopo values
        ptMin = {}
        for thrtype in ThrType.Run3Types():
            ttconfig = getTypeWideThresholdConfig(thrtype)
            inputtype = thrtype.name
            if inputtype == 'cTAU':
                inputtype = 'eTAU'
            for key, value in ttconfig.items():
                if "ptMinToTopo" in key:
                    if inputtype in ptMin:
                        if ptMin[inputtype] > value:
                             ptMin[inputtype] = value
                    else: 
                        ptMin[inputtype] = value

        # loop over multiplicity algorithms and get the min et values
        thresholdMin = {}
        for algo in self.topoAlgos.topoAlgos[AlgCategory.MULTI][AlgType.MULT]:
             alg = self.topoAlgos.topoAlgos[AlgCategory.MULTI][AlgType.MULT][algo]
             threshold = alg.threshold
             inputtype = alg.input
             if 'cTAU' in inputtype:
                 inputtype = 'eTAU'
             elif any(substring in inputtype for substring in ['XE','TE','MHT']):
                 continue
             thr = self.thresholds.thresholds[threshold]
             minEt = 99999 
             if hasattr(thr, 'thresholdValues'):
                 etvalues = thr.thresholdValues
                 for etvalue in etvalues:
                     et = etvalue.value
                     if et < minEt:
                         minEt = et
             if hasattr(thr, 'et'):
                 if thr.et < minEt:
                     minEt = thr.et
             if inputtype in thresholdMin:
                 if minEt < thresholdMin[inputtype]:
                     thresholdMin[inputtype] = minEt
             else:
                 thresholdMin[inputtype] = minEt

        # loop over sorting algorithms and get the min et values
        for algo in self.topoAlgos.topoAlgos[AlgCategory.TOPO][AlgType.SORT]:
             alg = self.topoAlgos.topoAlgos[AlgCategory.TOPO][AlgType.SORT][algo]
             if alg.inputvalue == 'MuonTobs':
                 continue
             for (pos, variable) in enumerate(alg.variables): 
                 if variable.name == "MinET":
                     value = variable.value/10 # convert energies from 100MeV to GeV units
                     inputtype = ''
                     if alg.inputvalue == 'eEmTobs':
                         inputtype = 'eEM'
                     elif alg.inputvalue == 'eTauTobs':
                         inputtype = 'eTAU'
                     elif alg.inputvalue == 'jJetTobs':
                         inputtype = 'jJ'
                     else:
                         raise RuntimeError("checkPtMinToTopo: input type %s in sorting algo not recognised" % alg.inputvalue)
                     if inputtype in thresholdMin:
                         if value < thresholdMin[inputtype]:
                              thresholdMin[inputtype] = value
                     else:
                         thresholdMin[inputtype] = value      
        for thr in thresholdMin:
            if thr in ptMin:
                 if thresholdMin[thr] < ptMin[thr]:
                     raise RuntimeError("checkPtMinToTopo: for threshold type %s the minimum threshold %i is less than ptMinToTopo %i" % (thr, thresholdMin[thr], ptMin[thr]))
            else:
                 raise RuntimeError("checkPtMinToTopo: for threshold type %s the minimum threshold is %i and no ptMinToTopo value is found" % (thr, thresholdMin[thr]))  

