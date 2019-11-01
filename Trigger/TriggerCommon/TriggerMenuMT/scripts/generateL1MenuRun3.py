#!/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import sys,os


def parseCmdLine():
    import argparse
    parser = argparse.ArgumentParser()
    # mandatory argument is the menu
    parser.add_argument("menu", help="the menu to generate", nargs='?', default="mc8")
    parser.add_argument("-v", "--verbose", help="increase output verbosity", action="count", default=0)
    parser.add_argument("-x", "--xml", dest="writeXML", help="enable xml file creation (currently not implemented)", action="store_true", default=False)
    parser.add_argument("--destdir", dest="dest", help="directory for output files", default = "./")
    return parser.parse_args()
    

def generateL1Menu(menu, cmdline):    

    # logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger(sys.argv[0].split('/')[-1])
    log.setLevel(logging.INFO)
    logging.getLogger("TriggerMenuMT.LVL1.Lvl1Menu").setLevel(logging.INFO)

    # setup
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.triggerMenuSetup = menu

    # L1 menu generation
    from TriggerMenuMT.L1.L1MenuConfig import L1MenuConfig
    l1cfg = L1MenuConfig()

    l1cfg.writeJSON( outputFile = TriggerFlags.outputLVL1configFile(), destdir = cmdline.dest)

    if cmdline.writeXML:
        outfilename = l1cfg.writeXML()
        # consistency checker
        checkResult = os.system("get_files -xmls -symlink LVL1config.dtd > /dev/null")
        checkResult = os.system("xmllint --noout --dtdvalid LVL1config.dtd %s" % outfilename)
        if checkResult == 0:
            log.info("XML file %s is conform with LVL1config.dtd" % outfilename)
        else:
            log.error("The XML does not follow the document type definition LVL1config.dtd")

    return l1cfg.l1menu


def main():

    cmdline = parseCmdLine()

    menuList = [
        ("phy8" , "Physics_pp_v8"),
        ("phy"  , "Physics_pp_v8"),
        ("mc8"  , "MC_pp_v8"     ),
        ("mc"   , "MC_pp_v8"     ),
        ("mcp"  , ["MC_pp_v8_no_prescale", "MC_pp_v8_loose_mc_prescale", "MC_pp_v8_tight_mc_prescale"]),
        ("ls"   , "LS2_v1"       ),
        ("cosmic", "Cosmic_pp_run3_v1"),
        ("hiphy4","Physics_HI_v4"),
        ("hiphy", "Physics_HI_v4"),
        ("himc4", "MC_HI_v4"     ),
        ("himc",  "MC_HI_v4"     ),
    ]

    for k,menu in menuList:
        if cmdline.menu.lower().startswith(k):
            if not isinstance(menu,list):
                menu = [menu]
            for m in menu:
                l1menu = generateL1Menu(menu=m, cmdline=cmdline)
            break

    return 0

if __name__=="__main__":
    sys.exit( main() )
        
        
