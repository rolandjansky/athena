#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys,os

from TriggerJobOpts.TriggerFlags import TriggerFlags as TF
from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1
from TriggerMenu.l1.Lvl1Flags import Lvl1Flags

def generateL1Menu(menu, useTopoMenu="MATCH"):

    from AthenaCommon.Logging import logging
    log = logging.getLogger("TriggerConfigLVL1")
    log.setLevel(logging.INFO)
    logging.getLogger("TriggerMenu.l1.Lvl1Menu").setLevel(logging.INFO)

    # what menu to build
    TF.triggerMenuSetup = menu

    # TPC for L1
    tpcl1 = TriggerConfigLVL1( outputFile = TF.outputLVL1configFile() )

    # build the menu structure
    tpcl1.generateMenu()

    # write xml file
    outfilename = tpcl1.writeXML()

    # consistency checker
    checkResult = os.system("get_files -xmls -symlink LVL1config.dtd > /dev/null")
    checkResult = os.system("xmllint --noout --dtdvalid LVL1config.dtd %s" % outfilename)
    if checkResult == 0:
        log.info("XML file %s is conform with LVL1config.dtd" % outfilename)
    else:
        log.error("the XML does not follow the document type definition LVL1config.dtd")
        
    return tpcl1.menu


def readL1MenuFromXML(menu="LVL1config_Physics_pp_v5.xml"):

    from AthenaCommon.Logging import logging
    log = logging.getLogger("TriggerConfigLVL1")
    log.setLevel(logging.INFO)
    
    fullname = None

    if '/' in menu:
        fullname = menu
    
    import os
    for path in os.environ['XMLPATH'].split(':'):
        if not 'TriggerMenuXML' in os.listdir(path):
            continue
        if menu in os.listdir("%s/TriggerMenuXML/" % path):
            fullname = "%s/TriggerMenuXML/%s" % (path,menu)
        break # we only want to look into the first TriggerMenuXML package

    if fullname:
        tpcl1 = TriggerConfigLVL1( inputFile = fullname, outputFile = "test.xml" )
        tpcl1.writeXML()
        return tpcl1.menu
    else:
        print "Did not find file %s" % menu
        return None



def findUnneededRun2():
    from TriggerJobOpts.TriggerFlags import TriggerFlags as TF
    from TriggerMenu.l1.Lvl1Flags import Lvl1Flags
    
    menus = ['Physics_pp_v5']

    for menu in menus:
        TF.triggerMenuSetup = menu
        tpcl1 = TriggerConfigLVL1()

        print set(tpcl1.registeredItems.keys()) - set(Lvl1Flags.items())



def findRequiredItemsFromXML():
    from TriggerJobOpts.TriggerFlags import TriggerFlags as TF
    from TriggerMenu.l1.Lvl1Flags import Lvl1Flags
    
    menus = ['Physics_pp_v5','MC_pp_v5']

    from TriggerMenu.l1.XMLReader import L1MenuXMLReader

    allItems = set()
    allThrs = set()

    path='/afs/cern.ch/atlas/software/builds/AtlasP1HLT/18.1.0.2/InstallArea/XML/TriggerMenuXML'

    for menu in menus:
        xmlfile = "%s/LVL1config_%s.xml" % (path,menu)
        r = L1MenuXMLReader(xmlfile)
        allItems.update( [x['name'] for x in r.getL1Items()] )
        allThrs.update( [x['name'] for x in r.getL1Thresholds()] )
        print menu, len(allItems), len(allThrs)

    from pickle import dump
    f = open("L1Items.pickle","w")
    dump([menus,allItems,allThrs],f)


def findUnneededRun1(what="items"):
    
    from pickle import load
    f = open("L1Items.pickle","r")
    [menus,allItems,allThrs] = load(f)

    # the TPC for L1
    tpcl1 = TriggerConfigLVL1( outputFile = TF.outputLVL1configFile() )

    if what=="items":
        unneeded = sorted(list(set( tpcl1.registeredItems.keys() ) - allItems))
    else:
        unneeded = sorted(list(set( tpcl1.registeredThresholds.keys() ) - allThrs))

    print "==> unneeded ",what,":",len(unneeded)

    import re
    p = re.compile('.*$')

    print [x for x in unneeded if p.match(x)]


def findFreeCTPIDs(menu):
    from pickle import load
    f = open("L1Items.pickle","r")
    [menus,allItems,allThrs] = load(f)

    TF.triggerMenuSetup = menu
    tpcl1 = TriggerConfigLVL1( outputFile = TF.outputLVL1configFile() )

    print set(Lvl1Flags.CtpIdMap().keys()) - allItems


    
def main():
    if len(sys.argv)==1:
#        generateL1Menu(menu="Physics_pp_v5")
#        generateL1Menu(menu="MC_pp_v5")
        generateL1Menu(menu="Physics_pp_v6")
        generateL1Menu(menu="MC_pp_v6")
#        generateL1Menu(menu="LS1_v1" )
        #generateL1Menu(menu="DC14")
#        generateL1Menu(menu="MC_pp_v5_no_prescale")
#        generateL1Menu(menu="MC_pp_v5_loose_mc_prescale")
#        generateL1Menu(menu="MC_pp_v5_tight_mc_prescale")
#        generateL1Menu(menu="MC_pp_v6_tight_mc_prescale")
#        generateL1Menu(menu="Physics_HI_v3")  # currently disabled since not defined in JobProp
#        generateL1Menu(menu="MC_HI_v3")  # currently disabled since not defined in JobProp
        return 0

    printCabling = False
    for arg in sys.argv:
        if arg.lower().startswith("cab"):
            printCabling = True


    if sys.argv[1].endswith(".xml"):
        readL1MenuFromXML(sys.argv[1])
        return 0
    
    if sys.argv[1].lower().startswith("phy"):
        menu = generateL1Menu(menu="Physics_pp_v6")
        if printCabling:
            menu.printCabling()
        return 0

    if sys.argv[1].lower().startswith("mc4"):
        generateL1Menu(menu="MC_pp_v4")
        return 0

    if sys.argv[1].lower().startswith("mc"):
        generateL1Menu(menu="MC_pp_v6")
        return 0

    if sys.argv[1].lower().startswith("mcp"):
        generateL1Menu(menu="MC_pp_v5_no_prescale")
        generateL1Menu(menu="MC_pp_v5_loose_mc_prescale")
        generateL1Menu(menu="MC_pp_v5_tight_mc_prescale")
        return 0

    if sys.argv[1].lower().startswith("ls"):
        menu = generateL1Menu(menu="LS1_v1")
        menu.printCabling()
        return 0

    if sys.argv[1].lower().startswith("dc14"):
        generateL1Menu(menu="DC14", useTopoMenu="Physics_pp_v5")
        return 0

    if sys.argv[1].lower().startswith("hiphy"):
        generateL1Menu(menu="Physics_HI_v3")
        return 0

    if sys.argv[1].lower().startswith("himc"):
        generateL1Menu(menu="MC_HI_v3")
        return 0

if __name__=="__main__":
    sys.exit( main() )
        
        
