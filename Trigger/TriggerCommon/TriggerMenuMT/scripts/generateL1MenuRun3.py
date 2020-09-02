#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import sys


def parseCmdLine(possibleMenus):
    import argparse
    parser = argparse.ArgumentParser()
    # mandatory argument is the menu
    parser.add_argument("menu", help="the menu to generate (possible menus: %s)" % ', '.join(possibleMenus), nargs='?', default="mc8")
    parser.add_argument("-v", "--verbose", help="increase output verbosity", action="count", default=0)
    parser.add_argument("--destdir", dest="dest", help="directory for output files", default = "./")
    return parser.parse_args()
    

def generateL1Menu(menu, cmdline):
    # logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger(sys.argv[0].split('/')[-1])
    log.setLevel(logging.INFO)
    logging.getLogger("TriggerMenuMT.LVL1.Lvl1Menu").setLevel(logging.INFO)

    # setup

    # L1 menu generation
    from TriggerMenuMT.L1.L1MenuConfig import L1MenuConfig
    l1cfg = L1MenuConfig( menu )

    from TrigConfigSvc.TrigConfigSvcCfg import getL1MenuFileName
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Trigger.triggerMenuSetup = menu
    l1cfg.writeJSON( outputFile = getL1MenuFileName(ConfigFlags), destdir = cmdline.dest)

    return l1cfg.l1menu


def generateDefaultMCBunchgroupSet(cmdline):
    from TriggerMenuMT.L1.Base.BunchGroupSet import createMCDefaultBunchGroupSet
    bgs = createMCDefaultBunchGroupSet()
    bgs.writeJSON( outputFile = "L1BunchGroupSet.json", destdir = cmdline.dest)
    


def main():


    availableMenus = dict([
        ("phy8" , "Physics_pp_v8"),
        ("phy"  , "Physics_pp_v8"),
        ("mc8"  , "MC_pp_v8"     ),
        ("mc"   , "MC_pp_v8"     ),
        ("mcp"  , ["MC_pp_v8_no_prescale", "MC_pp_v8_loose_mc_prescale", "MC_pp_v8_tight_mc_prescale"]),
        ("ls"   , "LS2_v1"       ),
        ("phyr3v1"   , "Physics_pp_run3_v1"       ),
        ("phyp1r3v1"   , "PhysicsP1_pp_run3_v1"       ),
        ("mcr3v1"   , "MC_pp_run3_v1"       ),
        ("cosmic", "Cosmic_run3_v1"),
        ("hip1r3v1", "PhysicsP1_HI_run3_v1" ),
        ("devhir3v1", "Dev_HI_run3_v1" ),
        ("hiphy4","Physics_HI_v4"),
        ("hiphy", "Physics_HI_v4"),
        ("himc4", "MC_HI_v4"     ),
        ("himc",  "MC_HI_v4"     ),
    ])

    cmdline = parseCmdLine(possibleMenus = availableMenus.keys())

    if cmdline.menu.lower() in availableMenus:
        menu = availableMenus[cmdline.menu.lower()]
        if not isinstance(menu,list):
            menu = [menu]
        for m in menu:
            generateL1Menu(menu=m, cmdline=cmdline)
    elif cmdline.menu.lower() == "bgrp":
        generateDefaultMCBunchgroupSet(cmdline=cmdline)
    else:
        generateL1Menu(menu=cmdline.menu, cmdline=cmdline)

    return 0

if __name__=="__main__":
    sys.exit( main() )
        
        
