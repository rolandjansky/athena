#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags as TF
from TriggerMenu.TriggerConfigL1Topo import TriggerConfigL1Topo

def generateL1TopoMenu(menu):

    from AthenaCommon.Logging import logging
    log = logging.getLogger("TriggerConfigL1Topo")
    log.setLevel(logging.INFO)


    # what menu to build
    TF.triggerMenuSetup = menu

    # TPC for L1Topo
    tpcl1 = TriggerConfigL1Topo( outputFile = TF.outputL1TopoConfigFile() )

    # build the menu structure
    tpcl1.generateMenu()

    # write xml file
    tpcl1.writeXML()

def main():

    if len(sys.argv)==1:
        generateL1TopoMenu(menu="MC_pp_v8")
        generateL1TopoMenu(menu="Physics_pp_v8")
        generateL1TopoMenu(menu="MC_pp_v7")
        generateL1TopoMenu(menu="Physics_pp_v7")
        generateL1TopoMenu(menu="MC_pp_v6")
        generateL1TopoMenu(menu="Physics_pp_v6")
        generateL1TopoMenu(menu="Physics_HI_v4")
        generateL1TopoMenu(menu="MC_HI_v4")
        generateL1TopoMenu(menu="Physics_HI_v5")
        generateL1TopoMenu(menu="MC_HI_v5")
        generateL1TopoMenu(menu="LS1_v1" )
        generateL1TopoMenu(menu="DC14")
        generateL1TopoMenu(menu="MC_PhaseII")
        return 0

    if sys.argv[1] in ["Physics_HI_v5", "MC_HI_v5","Physics_HI_v4", "MC_HI_v4", "LS1_v1", "DC14","Physics_pp_v8_lowmu","Physics_pp_v8", "MC_pp_v8","Physics_pp_v7", "MC_pp_v7","Physics_pp_v6", "MC_pp_v6", "MC_PhaseII"]: # explicit names for TMXML nightly
        generateL1TopoMenu(menu=sys.argv[1])
        return 0
    
    tmp_menu = sys.argv[1].lower()

    if tmp_menu.startswith("ph"): # for interactive production
        if 'v6' in tmp_menu:
            generateL1TopoMenu(menu="Physics_pp_v6")
        else:
            generateL1TopoMenu(menu="Physics_pp_v7")
        return 0

    if tmp_menu.startswith("mc"):
        if 'v6' in tmp_menu:
            generateL1TopoMenu(menu="MC_pp_v6")
        else:
            generateL1TopoMenu(menu="MC_pp_v7")
        return 0

    if sys.argv[1].lower().startswith("hiph"):
        generateL1TopoMenu(menu="Physics_HI_v5")
        return 0

    if sys.argv[1].lower().startswith("himc"):
        generateL1TopoMenu(menu="MC_HI_v5")
        return 0
    
    if sys.argv[1].lower().startswith("hipph"):
        generateL1TopoMenu(menu="Physics_HI_v4")
        return 0

    if sys.argv[1].lower().startswith("hipmc"):
        generateL1TopoMenu(menu="MC_HI_v4")
        return 0    

    if sys.argv[1].lower().startswith("ls"):
        generateL1TopoMenu(menu="LS1_v1")
        return 0

    if sys.argv[1].lower().startswith("dc"):
        generateL1TopoMenu(menu="DC14")
        return 0

    generateL1TopoMenu(menu=sys.argv[1])


if __name__=="__main__":
    import sys
    sys.exit( main() )

