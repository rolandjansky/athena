# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import sys,os

class XML2JsonConverter(object):
    def __init__(self, tabsize = 4,  **args):
        self.tabsize = tabsize
        
        self.__doc = None
        self.__root = None
        self.__rules = {}
        self.xmlfilename = None
        

    def convertFile(self, inputfilename, jsonfilename = None ):
        if not inputfilename.endswith(".xml"):
            print "judging by the suffix, %s is not an xml file" % inputfilename
            return 1
        self.parseXMLFile( inputfilename )
        self.writeJsonFile( jsonfilename )
    
    def parseXMLFile(self,filename):
        self.xmlfilename = filename
        import xml.etree.cElementTree as ET
        self.doc = ET.parse(filename)
        self.root = self.doc.getroot()
        print "Parsed file %s" % filename
        return self.doc
        
    def loadRules(self,docname):
        self.rules = { 'asInt' : {},
                       'asArray' : [],
                       'asMultiArray' : [],
                       'ignoreElement' : [],
                       'ignoreAttrib' : [],
                       'promote' : {},
                       'rename' : {}   # renaming will be applied last 
                       }
        
        if docname == "LVL1Config":
            self.rules['asInt'] =  {
                "TriggerItem" : ["partition", "complex_deadtime", "ctpid"],
                "TriggerThreshold" : ["bitnum", "mapping", "version", "active", "id"],
                "TriggerThresholdValue" : [ "thresholdval", "etamin", "etamax", "phimin", "phimax", 
                                            "em_isolation", "window", "priority", "had_veto", "had_isolation"],
                "Signal" : [ "range_end", "range_begin" ],
                "Random" : ["cut0", "cut1", "cut2", "cut3"],
                "PrescaleSet" : ["menuPartition"],
                "Prescale" : ["ctpid"],
                "TriggerCondition" : ["multi"],
                "METSignificance" : ["xeMax", "teSqrtMax", "xsSigmaOffset", "teSqrtMin", "xsSigmaScale", "xeMin"],
                "MinimumTOBPt" : [ "priority", "etamin", "ptmin", "etamax", "window" ],
                "Parametrization" : [ "slope", "isobit", "priority", "etamin", "offset", "mincut", "etamax", "upperlimit"],
                "CaloInfo" : [ "global_em_scale", "global_jet_scale"],
                "BunchGroupSet" : [ "menuPartition" ],
                "BunchGroup" : [ "internalNumber" ],
                "MuctpiInfo" : [ "high_pt", "max_cand", "low_pt" ],
                "PrescaledClock" : [ "clock1", "clock2" ],
                "LVL1Config" : [ "l1Version", "ctpVersion" ]
                }
            self.rules['asArray'] = [ 'AND', 'OR' ]
            self.rules['asMultiArray'] = [ ('PrescaleSet', 'Prescale'),
                                           ('TriggerThreshold', 'TriggerThresholdValue'),
                                           ('TriggerCounterList', 'TriggerCounter'),
                                           ('Isolation', 'Parametrization'),
                                           ('TriggerMenu', 'TriggerItem'),
                                           ('CaloInfo', 'MinimumTOBPt'),
                                           ('CaloInfo', 'Isolation'),
                                           ('BunchGroupSet', 'BunchGroup'),
                                           ('OR', 'TriggerCondition'),
                                           ('TriggerThresholdList', 'TriggerThreshold'),
                                           ('AND', 'TriggerCondition'),
                                           ('AND', 'OR'),
                                           ('AND', 'InternalTrigger')
                                           ]
            self.rules['ignoreElement'] = []
            self.rules['promote'] = {"TriggerThresholdList" : "TriggerThreshold"}
            self.rules['rename'] = { "TriggerItem" : "items", "TriggerMenu" : "menu", "l1Version" : "version", "ctpid" : "ctpId",
                                     "complex_deadtime" : "complexDeadtime", "trigger_type" : "triggerType", "TriggerThreshold" : "thresholds",
                                     "triggerthreshold" : "triggerThreshold", "TriggerCondition" : "triggerCondition", "InternalTrigger" : "internalTrigger",
                                     "Cable" : "cable", "Signal" : "signal", "range_begin" : "rangeBegin", "range_end" : "rangeEnd",
                                     "TriggerThresholdValue" : "thresholdValues" }
            
        elif docname == "HLT_MENU":
            
            self.rules['asInt'] = {
                "CHAIN": ["EBstep", "chain_counter"],
                "SIGNATURE": ["signature_counter"]
                }
            self.rules['asMultiArray'] = [ ('SEQUENCE_LIST', 'SEQUENCE'),
                                           ('GROUP_LIST', 'GROUP'),
                                           ('SIGNATURE_LIST', 'SIGNATURE'),
                                           ('CHAIN_LIST', 'CHAIN'),
                                           ('STREAMTAG_LIST', 'STREAMTAG'),
                                           ('SIGNATURE', 'TRIGGERELEMENT')
                                           ]
            self.rules['ignoreElement'] = [ "SEQUENCE_LIST", "SIGNATURE_LIST", "TRIGGERTYPE_LIST" ]
            self.rules['ignoreAttrib'] = [ ("CHAIN", "level"),
                                           ("CHAIN", "EBstep"),
                                           ("CHAIN", "pass_through"),
                                           ("CHAIN", "prescale"),
                                           ("CHAIN", "rerun_prescale"),
                                           ("HLT_MENU", "prescale_set_name")]
            self.rules['promote'] = {"GROUP_LIST" : "GROUP", "STREAMTAG_LIST" : "STREAMTAG", "CHAIN_LIST" : "CHAIN"}
            self.rules['rename'] = { "GROUP" : "groups", "STREAMTAG" : "streams", "CHAIN" : "chains",
                                     "menu_name" : "name",
                                     "chain_name" : "name", "chain_counter" : "counter", "lower_chain_name" : "l1item",
                                     "stream" : "name"}

        elif docname == "TOPO_MENU":
            
            self.rules['asInt'] = {
                "Parameter": ["pos", "value"],
                "Bit": ["selection"],
                "Input": ["position"],
                "Output": ["bits", "fpga", "clock", "algoId", "module", "firstbit"],
                "SortAlgo": ["algoId"],
                "DecisionAlgo": ["algoId"],
                }
            self.rules['asMultiArray'] = [ ('OutputList', 'Output'),
                                           ('Fixed', 'Generic'),
                                           ('Fixed', 'Input'),
                                           ('TOPO_MENU', 'SortAlgo'),
                                           ('TOPO_MENU', 'DecisionAlgo'),
                                           ('Variable', 'Parameter'),
                                           ('Output', 'Bit'),
                                           ('SIGNATURE', 'TRIGGERELEMENT')
                                           ]
            self.rules['promote'] = { "OutputList" : "Output" }
            self.rules['rename'] = { "menu_name" : "name", "menu_version" : "version",
                                     "algname" : "algName", "algoId" : "algId", "firstbit" : "firstBit", "module" : "board", "triggerline" : "triggerLines",
                                     "SortAlgo" : "sortingAlgorithms", "DecisionAlgo" : "decisionAlgorithms",
                                     "Fixed" : "fixedParameters", "Input" : "inputs", "Output" : "outputs", "Generic" : "generics",
                                     "Variable" : "variableParameters", "Parameter" : "parameters"}
        else:
            print "Unknown document with root element %s" % docname
            return 1
        return 0


    def renameKey(self,key):
        return self.rules['rename'][key] if key in self.rules['rename'] else key

    
    def createJsonStruct(self, node):
        if node.tag in self.rules['asArray']:
            return [ {element.tag : self.createJsonStruct(element)} for element in node ]

        else:
            from collections import OrderedDict
            d = OrderedDict()
            # writing attributs of the element to json structure in alphabetical order
            # turn values into int according to the asInt rule
            for attr in sorted(node.attrib.keys()):
                if (node.tag, attr) in self.rules['ignoreAttrib']: # attributes to ignore
                    continue
                if node.tag in self.rules['asInt'] and attr in self.rules['asInt'][node.tag]:
                    d[self.renameKey(attr)] = int(node.attrib[attr])
                else:
                    d[self.renameKey(attr)] = node.attrib[attr]
            # looping over children
            for element in node:
                if element.tag in self.rules['ignoreElement']:
                    continue
                if (node.tag, element.tag) in self.rules['asMultiArray']:
                    # list of multiple identical elements (e.g. CHAINs)
                    key = self.renameKey(element.tag) 
                    if not key in d: d[key] = list()
                    d[key] += [ self.createJsonStruct(element) ]
                else:
                    # single element (e.g. CHAIN_LIST)
                    if element.tag in d:
                        raise RuntimeError("element %s already attached to node %s and not declared as MultiArray" % (element.tag, node.tag))

                    if element.tag in self.rules['promote']:
                        obj = self.createJsonStruct(element)
                        promoted = self.renameKey( self.rules['promote'][element.tag]) #e.g. Chains
                        d[promoted] = obj[promoted]
                    else:
                        d[self.renameKey(element.tag)] = self.createJsonStruct(element)

            return d



    def writeJsonFile(self, filename = None):
        import json
        if not filename:
            # use xml filename base
            filename = self.xmlfilename.rstrip(".xml")+".json"
        elif os.path.isdir(filename):
            filename = filename.rstrip("/") + "/" + self.xmlfilename.split("/")[-1].rstrip(".xml")+".json"
        self.loadRules(self.root.tag)
        d = self.createJsonStruct(self.root)
        FH = open(filename, "w")
        json.dump(d, FH, indent=self.tabsize, separators=(',', ': '))
        FH.close()
        print "Wrote file %s" % filename



    def checkJsonConformity(self, filename):
        print "Checking json conformity",
        import json
        FH = open(filename, "rb")
        try:
            json.load(FH)
            print " ... OK"
        except ValueError, e:
            print " ... FAILED"
            print e
        FH.close()
        


def test():
    if len(sys.argv)<=1:
        print "please specify xml file for conversion"
        return 1

    converter = XML2JsonConverter()
    converter.convertFile( sys.argv[1] )
    return 0


if __name__=="__main__":
    sys.exit(test())

