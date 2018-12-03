# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.l1menu.Menu_MC_PhaseII' )


def defineMenu():
    """
    Use MC_pp_v7 L1 menu for this
    """
    
    import Menu_MC_pp_v7
    from TriggerMenu.l1.Lvl1Flags import Lvl1Flags


    Menu_MC_pp_v7.defineMenu()

    Lvl1Flags.thresholds += [
        ]

    Lvl1Flags.items += [
        # for running high rate tests in secondary CTP partitions
        #'L1_RD2_BGRP14', 
        'L1_RD3_BGRP15',
        ]

   #Make sure these triggers don't get assigned random CTPIDs
    Lvl1Flags.CtpIdMap().update({
        #'L1_RD2_BGRP14' : 463,
        'L1_RD3_BGRP15' : 464,
    })

    #---------------------------------
    # L1menu in MCppV5 is basis 
    # BeamSplashes: adapt some thresholds and items
    #---------------------------------
    Lvl1Flags.ThresholdMap = {
        }

    Lvl1Flags.ItemMap = {
        }
    
    #----------------------------------------------
    def remapThresholds():
        threshToRemove = []
        for index, thresholdName in enumerate(Lvl1Flags.thresholds()):
            if thresholdName in Lvl1Flags.ThresholdMap():
                if (Lvl1Flags.ThresholdMap()[thresholdName] != ''):
                    Lvl1Flags.thresholds()[index] = Lvl1Flags.ThresholdMap()[thresholdName]
                else:
                    threshToRemove.append(index)

        for t in reversed(threshToRemove):
            del Lvl1Flags.thresholds()[t]
    #----------------------------------------------
                    
    remapThresholds()

    #----------------------------------------------
    def remapItems():  
        itemsToRemove = []
        for itemIndex, itemName in enumerate(Lvl1Flags.items()):
            if (itemName in Lvl1Flags.ItemMap()):
                if (Lvl1Flags.ItemMap()[itemName] != ''):
                    Lvl1Flags.items()[itemIndex] = Lvl1Flags.ItemMap()[itemName]                                                
                else: 
                    itemsToRemove.append(itemIndex)

        for i in reversed(itemsToRemove):
            del Lvl1Flags.items()[i]
    #----------------------------------------------
                                           
    remapItems()
