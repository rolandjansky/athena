#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags as TF
from TriggerMenuMT.LVL1MenuConfig.TriggerConfigL1Topo import TriggerConfigL1Topo
from TriggerMenuMT.LVL1MenuConfig.L1Topo.L1TopoFlags import L1TopoFlags

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
        #generateL1TopoMenu(menu="MC_pp_v7")
        #generateL1TopoMenu(menu="Physics_pp_v7")
        generateL1TopoMenu(menu="LS2_v1" )
        generateL1TopoMenu(menu="Physics_pp_run3_v1" )
        generateL1TopoMenu(menu="PhysicsP1_pp_run3_v1" )
        generateL1TopoMenu(menu="MC_pp_run3_v1" )
        return 0

    if sys.argv[1] in ["LS2_v1","Physics_pp_v7", "MC_pp_v7", "Physics_pp_run3_v1", "PhysicsP1_pp_run3_v1", "MC_pp_run3_v1"]: # explicit names for TMXML nightly
        generateL1TopoMenu(menu=sys.argv[1])
        return 0

    if sys.argv[1].lower().startswith("ph"): # for interactive production
        generateL1TopoMenu(menu="Physics_pp_v7")
        return 0

    if sys.argv[1].lower().startswith("mc"):
        generateL1TopoMenu(menu="MC_pp_v7")
        return 0

    if sys.argv[1].lower().startswith("hiph"):
        generateL1TopoMenu(menu="Physics_HI_v3")
        return 0

    if sys.argv[1].lower().startswith("himc"):
        generateL1TopoMenu(menu="MC_HI_v3")
        return 0
    
    if sys.argv[1].lower().startswith("hipph"):
        generateL1TopoMenu(menu="Physics_HI_v4")
        return 0

    if sys.argv[1].lower().startswith("hipmc"):
        generateL1TopoMenu(menu="MC_HI_v4")
        return 0    

    if sys.argv[1].lower().startswith("ls"):
        generateL1TopoMenu(menu="LS2_v1")
        return 0

    if sys.argv[1].lower().startswith("dc"):
        generateL1TopoMenu(menu="DC14")
        return 0

    generateL1TopoMenu(menu=sys.argv[1])


if __name__=="__main__":
    import sys
    sys.exit( main() )

