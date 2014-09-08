#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags as TF
from TriggerMenu.TriggerConfigL1Topo import TriggerConfigL1Topo
from TriggerMenu.l1topo.L1TopoFlags import L1TopoFlags

def generateL1TopoMenu(menu):

    from AthenaCommon.Logging import logging
    log = logging.getLogger("TriggerConfigL1Topo")
    log.setLevel(logging.INFO)


    # what menu to build
    TF.triggerMenuSetup = menu

    # TPC for L1Topo
    tpcl1 = TriggerConfigL1Topo( outputFile = TF.outputL1TopoConfigFile(), menuName = TF.triggerMenuSetup() )

    # build the menu structure
    tpcl1.generateMenu()

    # write xml file
    tpcl1.writeXML()

if __name__=="__main__":
    generateL1TopoMenu(menu="Physics_pp_v5")

