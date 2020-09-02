#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


# this script is to help find differences between the old (xml) and new (json) L1 menus
# it also checks inputs of to the HLT and if they are available in L1

# this helps with the consistent transition from old to new L1 menu. It should be 
# deleted once we have concluded this transition

import sys

from AthenaCommon.Logging import log
from TriggerMenuMT.LVL1MenuConfig.LVL1.XMLReader import L1MenuXMLReader
from TrigConfIO.L1TriggerConfigAccess import L1MenuAccess
from TrigConfIO.HLTTriggerConfigAccess import HLTMenuAccess

def compareBothL1Menus(l1items_xml, l1items_json):
    # legacyCaloItems = [ k for k,v in l1items_json.items() if 'legacy' in v  ]

    itemNames_xml = [x['name'] for x in l1items_xml]
    itemNames_json = l1items_json.keys()
    ids_xml = dict([ (x['name'],int(x['ctpid'])) for x in l1items_xml])
    ids_json = dict([ (x['name'],x['ctpid']) for x in l1items_json.values()])

    if l1items_xml and l1items_json:
        itemsOnlyInJson =  list( set(itemNames_json) - set(itemNames_xml)  )
        itemsOnlyInXML =  list( set(itemNames_xml) - set(itemNames_json)  )
        log.info("These %i items are new in the json version" , len(itemsOnlyInJson))
        log.info(itemsOnlyInJson)
        log.info("\nThese %i items have disappeared in the json version" , len(itemsOnlyInXML))
        log.info(itemsOnlyInXML)

    inboth = set(itemNames_json).intersection(set(itemNames_xml))
    noMatchId = []
    for name in sorted(inboth):
        if ids_xml[name] != ids_json[name]:
            noMatchId += [(name, ids_xml[name], ids_json[name])]
    
    log.info("\nFrom %i items that are in both, these %i have non-matching CTP id's" , len(inboth),len(noMatchId))
    log.info("Name, CTPID in xml, CTPID in json")
    for x in noMatchId:
        log.info(x)






def main():
    
    l1items_xml = None
    l1items_json = None
    hltinput = None

    if len(sys.argv) <3:
        log.info("Please specify at least two menu files (L1Menu.xml, L1Menu.json, HLTMenu.json)")

    for fileName in sys.argv[1:]:
        if fileName.endswith(".json"):
            if fileName.startswith("L1"):
                l1menu = L1MenuAccess(fileName)
                l1items_json = l1menu.items()
            else:
                hltmenu = HLTMenuAccess(fileName)
                hltinput = hltmenu.chains()
        elif fileName.endswith(".xml"):
            l1menu = L1MenuXMLReader(fileName)
            l1items_xml = l1menu.getL1Items()


    if l1items_xml and l1items_json:
        compareBothL1Menus(l1items_xml, l1items_json)

    if l1items_json and hltinput:
        pass # check not yet implemented


    return 0
    
if __name__ == "__main__":
    sys.exit(main())
