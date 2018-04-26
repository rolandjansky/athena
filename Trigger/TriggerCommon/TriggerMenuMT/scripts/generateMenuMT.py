#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def generateMenu(menu):

    from TriggerMenu import setUseNewTriggerMenu
    setUseNewTriggerMenu()

    from TriggerMenu.menu import GenerateMenu
    from TriggerJobOpts.TriggerFlags import TriggerFlags

    # what menu to build
    TriggerFlags.triggerMenuSetup = menu

    GenerateMenu.generateMenu()


if __name__=="__main__":
    generateMenu(menu="Physics_pp_v6")
