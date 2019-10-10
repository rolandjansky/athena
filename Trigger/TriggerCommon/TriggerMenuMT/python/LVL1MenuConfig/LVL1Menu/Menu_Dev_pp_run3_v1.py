# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def defineMenu():
    """
    Defines the following LvlFlags:
    
    thresholds   .... list of all threshold names in the menu (includes forced thresholds)
    items        .... list of all L1 item names in the menu
    CtpIdMap     .... map of item names to CTP IDs
    """
    import Menu_LS2_v1 
    from ..LVL1.Lvl1Flags import Lvl1Flags

    Menu_LS2_v1.defineMenu()

    Lvl1Flags.thresholds = [
        ]

    
    Lvl1Flags.items = [
                
        ]
    

    # the CTP ID mapping still has to be defined, currently it is taken from the order of the items
    #Free slot [ATR-14858], 16-08-16
    #[82,143,377,378,379,406,409,416,480]

    

    Lvl1Flags.CtpIdMap = {
}


    Lvl1Flags.prescales = {}

