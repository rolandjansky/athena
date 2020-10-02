#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


# this script is to help find differences between the old (xml) and new (json) L1 menus
# it also checks inputs of to the HLT and if they are available in L1

# this helps with the consistent transition from old to new L1 menu. It should be 
# deleted once we have concluded this transition

import sys
import xml.etree.cElementTree as ET
from collections import namedtuple
from itertools import chain

from AthenaCommon.Logging import logging, Constants
log = logging.getLogger("TestMenuMigration")
log.setLevel(Constants.DEBUG)
from TrigConfIO.L1TriggerConfigAccess import L1MenuAccess
from TrigConfIO.HLTTriggerConfigAccess import HLTMenuAccess

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def boolStr(bval):
    return f"{bcolors.BOLD}{bcolors.OKBLUE}pass{bcolors.ENDC}" if bval else f"{bcolors.BOLD}{bcolors.FAIL}fail{bcolors.ENDC}"


def _liststr(arg):
    if type(arg) == list:
        return ','.join(arg)
    return arg

class MenuComp:
    def __init__(self, files ):

        self.fn_r22_l1_json = None
        self.fn_r22_l1_xml  = None
        self.fn_r22_hlt     = None
        self.fn_r22_topo    = None
        self.fn_r211_topo   = None
        self.check = [True]

        for fileName in files:
            if fileName.endswith(".json"):
                if fileName.startswith("L1"):
                    self.fn_r22_l1_json = fileName
                else:
                    self.fn_r22_hlt = fileName
            elif fileName.endswith(".xml"):
                if fileName.startswith("L1Topo"):                
                    if "smk" in fileName.lower(): # EB run configuration => r21.1
                        self.fn_r211_topo = fileName
                    elif '22.0' in fileName: # r22
                        self.fn_r22_topo = fileName
                else:
                    if '22.0' in fileName:
                        self.fn_r22_l1_xml = fileName

        self.r22_l1_json = L1MenuAccess(self.fn_r22_l1_json) if self.fn_r22_l1_json else None
        self.r22_l1_xml  = ET.parse(self.fn_r22_l1_xml).getroot() if self.fn_r22_l1_xml else None
        self.r22_hlt     = HLTMenuAccess(self.fn_r22_hlt) if self.fn_r22_hlt else None
        self.r22_topo    = ET.parse(self.fn_r22_topo).getroot() if self.fn_r22_topo else None
        self.r211_topo   = ET.parse(self.fn_r211_topo).getroot() if self.fn_r211_topo else None
        log.info("Release 22 L1 Menu      : %s", self.fn_r22_l1_json)
        log.info("Release 22 L1 Menu (xml): %s", self.fn_r22_l1_xml )
        log.info("Release 22 HLT Menu     : %s", self.fn_r22_hlt    )
        log.info("Release 22 L1Topo       : %s", self.fn_r22_topo   )
        log.info("Release 21.1 L1Topo     : %s", self.fn_r211_topo  )

    def compareCTPIN(self):
        if self.r22_l1_xml is None or self.r22_l1_json is None: 
            return
        log.info("Comparing the CTPIN inputs in r21.1 and r22")
        log.info("===========================================")
        # CHECK 1: all CTPIN thresholds must be migrated        
        THR = namedtuple("THR", "conn, firstbit, nbits, thrtype, mapping, name")
        ctpinThresholdsXML = []
        for thr in self.r22_l1_xml.find("TriggerThresholdList").iterfind("TriggerThreshold"):
            if thr.attrib['input'] != 'ctpin':
                continue
            cable = thr.find("Cable")
            if cable.attrib["name"] == 'MUCTPI':
                continue
            signal = cable.find("Signal")
            ctpinThresholdsXML += [ THR( name = thr.attrib["name"], thrtype = thr.attrib["type"], mapping = int(thr.attrib["mapping"]),
                                         conn = cable.attrib["name"], firstbit = int(signal.attrib["range_begin"]), 
                                         nbits = int(signal.attrib["range_end"]) - int(signal.attrib["range_begin"]) + 1) ]
        ctpinThresholdsJSON = []
        for connName,connDef in self.r22_l1_json.connectors().items():
            if connDef['type'] != 'ctpin':
                continue
            allThresholds = self.r22_l1_json.thresholds()
            thr2typeMap = {}
            for thrType in self.r22_l1_json.thresholdTypes():
                for thr in self.r22_l1_json.thresholds(thrType):
                    thr2typeMap[thr] = thrType
            for tl in connDef['triggerlines']:
                thrName = tl['name']
                thr = allThresholds[thrName]
                ctpinThresholdsJSON += [  THR( name = thrName, thrtype = thr2typeMap[thrName], mapping = thr["mapping"],
                                               conn = connName, firstbit = tl['startbit'], nbits = tl['nbits']) ]
        onlyInXML = sorted(set(ctpinThresholdsXML) - set(ctpinThresholdsJSON))
        onlyInJSON = sorted(set(ctpinThresholdsJSON) - set(ctpinThresholdsXML))
        cc = len(self.check)
        self.check += [ len(onlyInXML) + len(onlyInJSON) ==0 ]
        log.info("CHECK %i: all ctpin inputs (except MUCTPI) must be correctly migrated from xml to json: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            if len(onlyInXML)>0:
                log.debug("These %i ctpin inputs have not been (correctly) migrated" , len(onlyInXML))
                for x in onlyInXML:
                    log.debug("    %s", x)
            if len(onlyInJSON)>0:
                log.debug("These %i ctpin inputs are new" , len(onlyInJSON))
                for x in onlyInJSON:
                    log.debug("    %s", x)
                
        #for x in onlyInXML:
        #print("In XML %i" % len(ctpinThresholdsXML))
        #print("In JSON %i" % len(ctpinThresholdsJSON))
        #for i,x in enumerate(ctpinThresholdsXML):
        #    print(f"{i}: {x}")
        #for i,x in enumerate(ctpinThresholdsJSON):
        #    print(f"{i}: {x}")
        #for i,x in enumerate(onlyInXML):
        #    print(f"{i}: {x}")



    def compareL1MenusXMLwithJSON(self):
        if self.r22_l1_xml is None or self.r22_l1_json is None: 
            return
        l1items_xml  = self.r22_l1_xml.find("TriggerMenu").findall("TriggerItem")
        l1items_json = self.r22_l1_json.items()
        log.info("Comparing the r22-xml and r22-json")
        log.info("==================================")

        itemNames_xml = [x.attrib['name'] for x in l1items_xml]
        itemNames_json = l1items_json.keys()
        ids_xml = dict([ (x.attrib['name'],int(x.attrib['ctpid'])) for x in l1items_xml])
        ids_json = dict([ (x['name'],x['ctpid']) for x in l1items_json.values()])


        # CHECK 1: All items migrated
        itemsOnlyInXML =  list( set(itemNames_xml) - set(itemNames_json) )
        cc = len(self.check)
        self.check += [ len(itemsOnlyInXML)==0 ]
        log.info("CHECK %i: All items from r22 xml must be migrated: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            log.debug("These %i legacy items are not migrated:", len(itemsOnlyInXML))
            for x in itemsOnlyInXML:
                log.debug("    %s", x)

        # CHECK 2: No new legacy items
        legacyItemNames_json = [x['name'] for x in l1items_json.values() if 'legacy' in x  ]
        legacyItemsOnlyInJson =  list( set(legacyItemNames_json) - set(itemNames_xml) )
        cc = len(self.check)
        self.check += [ len(legacyItemsOnlyInJson)==0 ]
        log.info("CHECK %i: There should be no extra legacy items in json that are not in xml: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            log.debug("These %i legacy items are new in the json version (should be none!)" , len(legacyItemsOnlyInJson))
            for x in legacyItemsOnlyInJson:
                log.debug("    %s", x)

        # CHECK 3: CTPID match
        inboth = set(itemNames_json).intersection(set(itemNames_xml))
        noMatchId = []
        for name in sorted(inboth):
            if ids_xml[name] != ids_json[name]:
                noMatchId += [(name, ids_xml[name], ids_json[name])]
        cc = len(self.check)
        self.check += [ len(noMatchId)==0 ]
        log.info("CHECK %i: CTP-IDs must match in xml and json: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            log.debug("From %i items that are in both, these %i have non-matching CTP id's (should be 0)" , len(inboth),len(noMatchId))
            log.debug("    Name, CTPID in xml, CTPID in json")
            for x in noMatchId:
                log.debug("    %s", x)


    def checkHltInputsPresent(self):
        if self.r22_l1_json is None or self.r22_hlt is None: 
            return
        log.info("Checking L1-HLT consistency")
        log.info("===========================")
        availableItems = set(self.r22_l1_json.items().keys())
        availableItems.add('')    

        neededItemNotInL1 = []
        for cname,cdef in self.r22_hlt["chains"].items():
            inputItem = cdef['l1item']
            if ',' in inputItem:
                requiredItems = inputItem.split(',')
            else:
                requiredItems = [ inputItem ]
            requiredItemsNotInL1 = [ i for i in requiredItems if i not in availableItems ] 
            if len(requiredItemsNotInL1)!=0:
                neededItemNotInL1 += [ (cname, cdef['l1item'], requiredItemsNotInL1) ]

        cc = len(self.check)
        self.check += [ len(neededItemNotInL1)==0 ]
        log.info("CHECK %i: All items required by HLT must be available: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            log.debug("These %i chains have input items that are not available in the L1 menu" , len(neededItemNotInL1))
            for (hlt,l1,missingItem) in neededItemNotInL1:
                if ',' in l1:
                    log.debug("    chain %s misses input item %s (this chain has multiple inputs!)", hlt,", ".join(missingItem))
                else:
                    log.debug("    chain %s misses input item %s", hlt,", ".join(missingItem))




    def compareL1TopoMenusRun2Legacy(self):
        if self.r211_topo is None or self.r22_l1_json is None:
            return
        log.info("Checking Legacy L1Topo migration")
        log.info("================================")

        OL = namedtuple('OL','board, fpga, clock, bit, triggerline')
        ALG = namedtuple('A','id, triggerline')
        AP = namedtuple('AP', 'name, klass, inputs, outputs, gPars, vPars')
        
        # triggerlines
        r2_xml = []
        # algos
        r2_algos_xml = {}
        r2_algDefs_xml = {}

        # extract info from xml file
        for output in (dict(x.items()) for x in self.r211_topo.find('OutputList')):
            algoname = output["algname"]
            algoId = int(output["algoId"])
            board = int(output["module"])
            fpga = int(output['fpga'])
            clock = int(output['clock'])
            bit = int(output['firstbit'])
            triggerline = output['triggerline'].split(',')
            for tl in triggerline:
                tl = tl.replace('.','p')
                r2_xml.append( OL(board=board, fpga=fpga, clock=clock, bit=bit, triggerline=tl) )
                r2_algos_xml[algoname.replace('.','p')] = ALG( id = algoId, triggerline=','.join(triggerline))
                bit += 1

        for sa in chain( self.r211_topo.iterfind('SortAlgo'),
                         self.r211_topo.iterfind('DecisionAlgo') ):
            isSortAlg = "value" in sa.find("Fixed").find("Output").attrib
            algName = sa.get('name')
            algType = sa.get('type')
            algId = int(sa.get('algoId'))
            fixedParam = sa.find("Fixed")
            inputs = [ x.attrib['value'] for x in fixedParam.iterfind("Input") ]
            if isSortAlg:
                outputs = [ fixedParam.find("Output").attrib["value"] ]
            else:
                outputs = []
                for o in fixedParam.find("Output").iterfind("Bit"):
                    outputs += [ o.attrib['name'].replace('.','p') ]
            gpars = {}
            for x in fixedParam.iterfind("Generic"):
                gpars[x.attrib['name']] = x.attrib['value']
            allvPars = sa.find("Variable").findall("Parameter")
            vpars = len(allvPars) * [None]
            for x in allvPars:
                vpars[int(x.attrib['pos'])] = (x.attrib['name'],x.attrib['value'],x.get('selection'))
            r2_algDefs_xml[algName.replace('.','p')] = AP( name = algName, klass = algType, inputs = inputs, outputs = outputs, gPars = gpars, vPars = vpars)
            r2_algos_xml[algName.replace('.','p')] = ALG( id = algId, triggerline = ','.join(outputs))

        # json connector names
        r3_json = []
        topoConn_json = self.r22_l1_json['connectors']
        for board in [0,1]:
            for fpga in [0,1]:
                for clock in [0,1]:                
                    set1 = topoConn_json['LegacyTopo%i' % board]['triggerlines']['fpga%i' % fpga]['clock%i' % clock]
                    for tl in set1:
                        tl_name = tl['name'].split('_',1)[-1]
                        bit = tl['startbit']
                        r3_json.append( OL(board=board, fpga=fpga, clock=clock, bit=bit, triggerline=tl_name) )

        # CHECK: topo lines
        linesOnlyInR2Xml = sorted(set([x for x in r2_xml]) - set([x for x in r3_json]))
        linesOnlyInR3Json = sorted(set([x for x in r3_json]) - set([x for x in r2_xml]))
        cc = len(self.check)
        self.check += [ len(linesOnlyInR3Json) + len(linesOnlyInR2Xml)==0 ]
        log.info("CHECK %i: Topo lines must match: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            if len(linesOnlyInR2Xml)>0:
                log.debug("These %i legacy topo items are missing in the legacy part of the new L1 menu" , len(linesOnlyInR2Xml))
                for x in linesOnlyInR2Xml:
                    log.debug(f"    {x.triggerline} on board {x.board}, fpga {x.fpga}, clock {x.clock}, bit {x.bit}")
            if len(linesOnlyInR3Json)>0:
                log.debug("These %i legacy topo items are new in the legacy part of the new L1 menu" , len(linesOnlyInR3Json))
                for x in linesOnlyInR3Json:
                    log.debug(f"    {x.triggerline} on board {x.board}, fpga {x.fpga}, clock {x.clock}, bit {x.bit}")

        # CHECK Algo names
        r3_algos_json = {}
        for algName,algDef in self.r22_l1_json['topoAlgorithms']['R2TOPO']['decisionAlgorithms'].items():
            r3_algos_json[algName] = ALG( id = algDef['algId'], triggerline=','.join(algDef['output']))
        for algName,algDef in self.r22_l1_json['topoAlgorithms']['R2TOPO']['sortingAlgorithms'].items():
            r3_algos_json[algName] = ALG( id = algDef['algId'], triggerline=','.join(algDef['output']))
        algosOnlyInR2Xml = sorted(set(r2_algos_xml) - set(r3_algos_json))
        algosOnlyInR3Json = sorted(set(r3_algos_json) - set(r2_algos_xml))
        cc = len(self.check)
        self.check += [ len(algosOnlyInR2Xml) + len(algosOnlyInR3Json)==0 ]
        log.info("CHECK %i: Algorithm names must match: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            if len(algosOnlyInR2Xml)>0:
                log.debug("These %i legacy topo algoriths are missing in the legacy part of the new L1 menu" , len(algosOnlyInR2Xml))
                for algName in algosOnlyInR2Xml:
                    log.debug(f"    {algName}")
            if len(algosOnlyInR3Json)>0:
                log.debug("These %i legacy topo items are new in the legacy part of the new L1 menu" , len(algosOnlyInR3Json))
                for algName in algosOnlyInR3Json:
                    log.debug(f"    {algName}")

        # CHECK Algo ID match
        algosWithoutMatchingId = []
        for algName in set(r2_algos_xml).intersection(set(r3_algos_json)):
            r2id = r2_algos_xml[algName].id
            r3id = r3_algos_json[algName].id
            if r2id != r3id:
                algosWithoutMatchingId += [(algName, r2id, r3id)]
        cc = len(self.check)
        self.check += [ len(algosWithoutMatchingId)==0 ]
        log.info("CHECK %i: All legacy topo algorithm IDs must match: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            log.debug("These %i legacy topo algos have non-matching IDs (old => new)" , len(algosWithoutMatchingId))
            for x in algosWithoutMatchingId:
                log.debug(f"    {x[0]}  ({x[1]} => {x[2]})")

        # CHECK algo class match
        klassMismatch = []
        for (algName, algDef) in chain( self.r22_l1_json['topoAlgorithms']['R2TOPO']['sortingAlgorithms'].items(),
                                        self.r22_l1_json['topoAlgorithms']['R2TOPO']['decisionAlgorithms'].items()):
            if algName not in r2_algDefs_xml:
                continue
            r2alg = r2_algDefs_xml[algName]
            if algDef['klass'] != r2alg.klass:
                klassMismatch += [ (algName, r2alg.klass, algDef['klass']) ]
        cc = len(self.check)
        self.check += [ len(klassMismatch)==0 ]
        log.info("CHECK %i: All class names must match: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            for algName, r2klass, r3klass in klassMismatch:
                log.debug(f"Class of algorithm {algName} changes from  {r2klass} to {r3klass}")
        

        # CHECK properties match
        fixedPropertyMismatch = []
        varPropertyMismatch = []
        varNumberMismatch = []
        for (algName, algDef) in chain( self.r22_l1_json['topoAlgorithms']['R2TOPO']['sortingAlgorithms'].items(),
                                        self.r22_l1_json['topoAlgorithms']['R2TOPO']['decisionAlgorithms'].items() ):
            r2alg = r2_algDefs_xml[algName]
            if _liststr(r2alg.inputs) != _liststr(algDef['input']):
                fixedPropertyMismatch += [ (algName, "Input", _liststr(r2alg.inputs), _liststr(algDef['input'])) ]
            if _liststr(r2alg.outputs) != _liststr(algDef['output']):
                fixedPropertyMismatch += [ (algName, "Output", _liststr(r2alg.outputs), _liststr(algDef['output'])) ]
            for (gname, gval) in algDef['fixedParameters']['generics'].items():
                if r2alg.gPars[gname] != str(gval['value']):
                    fixedPropertyMismatch += [ (algName, "Generic parameter " + gname, r2alg.gPars[gname], str(gval['value'])) ]
            if len(algDef['variableParameters']) != len(r2alg.vPars):
                varNumberMismatch += [ (algName, len(r2alg.vPars), len(algDef['variableParameters'])) ]
            else:
                for pos,par in enumerate(algDef['variableParameters']):
                    if r2alg.vPars[pos][0] != par['name']:
                        varPropertyMismatch += [ (algName, pos, "name", r2alg.vPars[pos][0], par['name']) ]
                    if r2alg.vPars[pos][1] != str(par['value']):
                        varPropertyMismatch += [ (algName, pos, "value", r2alg.vPars[pos][1], str(par['value'])) ]
                    if par.get('selection') is not None or r2alg.vPars[pos][2] in range(1,10):
                        if str(r2alg.vPars[pos][2]) != str(par.get('selection')):
                            varPropertyMismatch += [ (algName, pos, "selection", r2alg.vPars[pos][2], par.get('selection')) ]

        # CHECK number variables
        cc = len(self.check)
        self.check += [ len(varNumberMismatch)==0 ]
        log.info("CHECK %i: Number of algorithm variable parameters must match: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            for algName, l1, l2 in varNumberMismatch:
                log.debug(f"    Number of variable parameters for algorithm {algName} changes from  {l1} to {l2}")

        # CHECK algorithm properties
        cc = len(self.check)
        self.check += [ len(fixedPropertyMismatch) + len(varPropertyMismatch) ==0 ]
        log.info( "CHECK %i: Algorithm property values must match: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            for algName, var, r2val, r3val in fixedPropertyMismatch:
                log.debug(f"    {var} of algorithm {algName} changes from  {r2val} to {r3val}")
            for (algName, pos, nameOrVal, r2, r3) in varPropertyMismatch:
                log.debug(f"    Variable parameter at pos {pos} of algorithm {algName} changes {nameOrVal} from  {r2} to {r3}")
            


    def compareL1TopoMenusRun3(self):
        if self.r22_topo is None or self.r22_l1_json is None:
            return
        log.info("Checking Run 3 L1Topo migration")
        log.info("===============================")

        OL = namedtuple('OL','board, fpga, clock, bit, triggerline')
        ALG = namedtuple('A','id, triggerline')
        AP = namedtuple('AP', 'name, klass, inputs, outputs, gPars, vPars')
        
        # triggerlines
        r2_xml = []
        # algos
        r2_algos_xml = {}
        r2_algDefs_xml = {}

        # extract info from xml file
        for output in (dict(x.items()) for x in self.r22_topo.find('OutputList')):
            algoname = output["algname"]
            algoId = int(output["algoId"])
            board = int(output["module"])
            fpga = int(output['fpga'])
            clock = int(output['clock'])
            bit = int(output['firstbit'])
            triggerline = output['triggerline'].split(',')
            for tl in triggerline:
                tl = tl.replace('.','p')
                r2_xml.append( OL(board=board+2, fpga=fpga, clock=clock, bit=bit, triggerline=tl) )
                r2_algos_xml[algoname.replace('.','p')] = ALG( id = algoId, triggerline=','.join(triggerline))
                bit += 1

        for sa in chain( self.r22_topo.iterfind('SortAlgo'),
                         self.r22_topo.iterfind('DecisionAlgo') ):
            isSortAlg = "value" in sa.find("Fixed").find("Output").attrib
            algName = sa.get('name')
            algType = sa.get('type')
            algId = int(sa.get('algoId'))
            fixedParam = sa.find("Fixed")
            inputs = [ x.attrib['value'] for x in fixedParam.iterfind("Input") ]
            if isSortAlg:
                outputs = [ fixedParam.find("Output").attrib["value"] ]
            else:
                outputs = []
                for o in fixedParam.find("Output").iterfind("Bit"):
                    outputs += [ o.attrib['name'].replace('.','p') ]
            gpars = {}
            for x in fixedParam.iterfind("Generic"):
                gpars[x.attrib['name']] = x.attrib['value']
            allvPars = sa.find("Variable").findall("Parameter")
            vpars = len(allvPars) * [None]
            for x in allvPars:
                vpars[int(x.attrib['pos'])] = (x.attrib['name'],x.attrib['value'],x.get('selection'))
            r2_algDefs_xml[algName.replace('.','p')] = AP( name = algName, klass = algType, inputs = inputs, outputs = outputs, gPars = gpars, vPars = vpars)
            r2_algos_xml[algName.replace('.','p')] = ALG( id = algId, triggerline = ','.join(outputs))

        # json connector names
        r3_json = []
        topoConn_json = self.r22_l1_json['connectors']
        for board in [2,3]:
            for fpga in [0,1]:
                for clock in [0,1]:                
                    set1 = topoConn_json['Topo%iEl' % board]['triggerlines']['fpga%i' % fpga]['clock%i' % clock]
                    for tl in set1:
                        tl_name = tl['name'].split('_',1)[-1]
                        bit = tl['startbit']
                        r3_json.append( OL(board=board, fpga=fpga, clock=clock, bit=bit, triggerline=tl_name) )


        # CHECK 1: topo lines
        linesOnlyInR2Xml = sorted(set([x for x in r2_xml]) - set([x for x in r3_json]))
        linesOnlyInR3Json = sorted(set([x for x in r3_json]) - set([x for x in r2_xml]))
        cc = len(self.check)
        self.check += [ len(linesOnlyInR3Json) + len(linesOnlyInR2Xml)==0 ]
        log.info("CHECK %i: Topo lines must match: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            if len(linesOnlyInR2Xml)>0:
                log.debug("These %i topo lines are missing in the new L1 menu" , len(linesOnlyInR2Xml))
                for x in linesOnlyInR2Xml:
                    log.debug(f"    {x.triggerline} on board {x.board}, fpga {x.fpga}, clock {x.clock}, bit {x.bit}")
            if len(linesOnlyInR3Json)>0:
                log.debug("These %i legacy topo items are new in the legacy part of the new L1 menu" , len(linesOnlyInR3Json))
                for x in linesOnlyInR3Json:
                    log.debug(f"    {x.triggerline} on board {x.board}, fpga {x.fpga}, clock {x.clock}, bit {x.bit}")


        # CHECK 2: Algo names
        r3_algos_json = {}
        for algName,algDef in self.r22_l1_json['topoAlgorithms']['TOPO']['decisionAlgorithms'].items():
            r3_algos_json[algName] = ALG( id = algDef['algId'], triggerline=','.join(algDef['output']))
        for algName,algDef in self.r22_l1_json['topoAlgorithms']['TOPO']['sortingAlgorithms'].items():
            r3_algos_json[algName] = ALG( id = algDef['algId'], triggerline=','.join(algDef['output']))
        algosOnlyInR2Xml = sorted(set(r2_algos_xml) - set(r3_algos_json))
        algosOnlyInR3Json = sorted(set(r3_algos_json) - set(r2_algos_xml))
        cc = len(self.check)
        self.check += [ len(algosOnlyInR2Xml) + len(algosOnlyInR3Json)==0 ]
        log.info("CHECK %i: Algorithm names must match: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            if len(algosOnlyInR2Xml)>0:
                log.debug("These %i legacy topo algoriths are missing in the legacy part of the new L1 menu" , len(algosOnlyInR2Xml))
                for algName in algosOnlyInR2Xml:
                    log.debug(f"    {algName}")
            if len(algosOnlyInR3Json)>0:
                log.debug("These %i legacy topo items are new in the legacy part of the new L1 menu" , len(algosOnlyInR3Json))
                for algName in algosOnlyInR3Json:
                    log.debug(f"    {algName}")


        # CHECK algo class match
        klassMismatch = []
        for (algName, algDef) in chain( self.r22_l1_json['topoAlgorithms']['TOPO']['sortingAlgorithms'].items(),
                                        self.r22_l1_json['topoAlgorithms']['TOPO']['decisionAlgorithms'].items()):
            if algName not in r2_algDefs_xml:
                continue
            r2alg = r2_algDefs_xml[algName]
            if algDef['klass'] != r2alg.klass:
                klassMismatch += [ (algName, r2alg.klass, algDef['klass']) ]
        cc = len(self.check)
        self.check += [ len(klassMismatch)==0 ]
        log.info("CHECK %i: All class names must match: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            for algName, r2klass, r3klass in klassMismatch:
                log.debug(f"Class of algorithm {algName} changes from  {r2klass} to {r3klass}")
        

        # CHECK: properties match
        fixedPropertyMismatch = []
        varPropertyMismatch = []
        varNumberMismatch = []
        for (algName, algDef) in chain( self.r22_l1_json['topoAlgorithms']['TOPO']['sortingAlgorithms'].items(),
                                        self.r22_l1_json['topoAlgorithms']['TOPO']['decisionAlgorithms'].items() ):
            if algName not in r2_algDefs_xml:
                continue
            r2alg = r2_algDefs_xml[algName]
            if _liststr(r2alg.inputs) != _liststr(algDef['input']):
                fixedPropertyMismatch += [ (algName, "Input", _liststr(r2alg.inputs), _liststr(algDef['input'])) ]
            if _liststr(r2alg.outputs) != _liststr(algDef['output']):
                fixedPropertyMismatch += [ (algName, "Output", _liststr(r2alg.outputs), _liststr(algDef['output'])) ]
            for (gname, gval) in algDef['fixedParameters']['generics'].items():
                if r2alg.gPars[gname] != str(gval['value']):
                    fixedPropertyMismatch += [ (algName, "Generic parameter " + gname, r2alg.gPars[gname], str(gval['value'])) ]
            if len(algDef['variableParameters']) != len(r2alg.vPars):
                varNumberMismatch += [ (algName, len(r2alg.vPars), len(algDef['variableParameters'])) ]
            else:
                for pos,par in enumerate(algDef['variableParameters']):
                    if r2alg.vPars[pos][0] != par['name']:
                        varPropertyMismatch += [ (algName, pos, "name", r2alg.vPars[pos][0], par['name']) ]
                    if r2alg.vPars[pos][1] != str(par['value']):
                        varPropertyMismatch += [ (algName, "%i (%s)" % (pos,par['name']), "value", r2alg.vPars[pos][1], str(par['value'])) ]
                    if par.get('selection') is not None or r2alg.vPars[pos][2] in range(1,10):
                        if str(r2alg.vPars[pos][2]) != str(par.get('selection')):
                            varPropertyMismatch += [ (algName, pos, "selection", r2alg.vPars[pos][2], par.get('selection')) ]


        # CHECK number variables
        cc = len(self.check)
        self.check += [ len(varNumberMismatch)==0 ]
        log.info("CHECK %i: Number of algorithm variable parameters must match: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            for algName, l1, l2 in varNumberMismatch:
                log.debug(f"    Number of variable parameters for algorithm {algName} changes from  {l1} to {l2}")


        # CHECK algorithm properties
        cc = len(self.check)
        self.check += [ len(fixedPropertyMismatch) + len(varPropertyMismatch) ==0 ]
        log.info( "CHECK %i: Algorithm property values must match: %s", cc, boolStr(self.check[cc]) )
        if not self.check[cc]:
            for algName, var, r2val, r3val in fixedPropertyMismatch:
                log.debug(f"    {var} of algorithm {algName} changes from  {r2val} to {r3val}")
            for (algName, pos, nameOrVal, r2, r3) in varPropertyMismatch:
                log.debug(f"    Variable parameter at pos {pos} of algorithm {algName} changes {nameOrVal} from  {r2} to {r3}")
















def main():

    if len(sys.argv) <3:
        log.info("Please specify at least two menu files (L1Menu.xml, L1Menu.json, HLTMenu.json)")

    mc = MenuComp( sys.argv[1:] )

    mc.compareL1MenusXMLwithJSON()

    mc.compareCTPIN()

    mc.checkHltInputsPresent()

    mc.compareL1TopoMenusRun2Legacy()
    
    mc.compareL1TopoMenusRun3()

    return 0
    
if __name__ == "__main__":
    sys.exit(main())
