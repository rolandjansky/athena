# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.l1menu.Menu_Physics_pp_v8' )


def defineMenu():
    """
    MC and Physics menu are identical
    """
    
    import Menu_MC_pp_v8
    from TriggerMenu.l1.Lvl1Flags import Lvl1Flags


    Menu_MC_pp_v8.defineMenu()

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
        'L1_RD3_BGRP15' : 510,
    })
    #Check that no two items have the same ctp-id
    inversemap = {}
    for l1,ctpid in Lvl1Flags.CtpIdMap().iteritems():
        if ctpid in inversemap:
            log.error ('Two items have the same CTPID assigned: %s %s'%(l1,inversemap[ctpid]))
        else:
            inversemap[ctpid] = l1

    #---------------------------------
    # L1menu in MCppV5 is basis 
    # BeamSplashes: adapt some thresholds and items
    #---------------------------------
    Lvl1Flags.ThresholdMap = {


#        'EM7'       : 'EM3HI',
#        'EM8VH'     : 'EM7',
#        'EM8I'      : 'EM8VH',
#        'EM10'      : 'EM8I',
#        'EM10VH'    : 'EM10VH',
#        'EM12'      : 'EM12',
#        'EM13VH'    : 'EM15',        
#        'EM15'      : 'EM15VH',
#        'EM15VH'    : 'EM15I',
#        'EM15I'     : 'EM15HI',
#        'EM15HI'    : 'EM18VH',
#        'EM18VH'    : 'EM20A',
#        'EM20VH'    : 'EM20C',
#        'EM20VHI'   : 'EM20VH',
#        'EM22VHI'   : 'EM20VHI',

#        'J75'       : 'J75A',    
#        'J85'       : 'J75C',
#        'J100'      : 'J85',         
#        'J120'      : 'J100',         
#        
#        'HA20'      : 'HA20A', 
#        'HA20IL'    : 'HA20C', 
#        'HA20IM'    : 'HA20', 
#        'HA20IT'    : 'HA20IL', 
#        'HA25'      : 'HA20IM', 
#        'HA25IT'    : 'HA20IT', 
#        'HA30'      : 'HA25', 
#        'HA40'      : 'HA25IT', 
#        'HA60'      : 'HA40', 

        }

    Lvl1Flags.ItemMap = {
#        'L1_EM22VHI' : 'L1_EM20A' , #eta [1.6,2.].
#        'L1_EM13VH'    : 'L1_EM20C',     #eta[-2,-1.6] 
#        'L1_J75'       : 'L1_J75A',         #[1.6,2.4]
#        'L1_J120'      : 'L1_J75C',         #[-2.4,-1.6]  
#        'L1_EM10'      : 'L1_EM3HI',
#        'L1_TAU30'     : 'L1_TAU20A', 
#        'L1_TAU60'     : 'L1_TAU20C', 
#        'L1_2EM13VH' : '',
#        'L1_3J75' : '',
#        'L1_J75_3J20' : '',
#        'L1_J75_XE40' :'',
#        'L1_MU6_J75': '',

#        MC-only L1 items, candidates to be remapped
#        'L1_J40.0ETA25_2J30_J20.31ETA49'                               :
#        'L1_J40.0ETA25_XE50'                                           :
#        'L1_KF-XE50'                                                   :
#        'L1_2EM8I'                                                     :
#        'L1_MU4_UNPAIRED_NONISO'                                       :
#        'L1_J75_3J20'                                                  :
#        'L1_2EM8I_J75'                                                 :
#        'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0'   :
#        'L1_DR25-TAU20ITAU12I-J25'                                     :
#        'L1_EM7_MU11'                                                  :
#        'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE'                  :
#        'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20'             :
#        'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE':
#        'L1_2MU4_J20_XE40_DPHI-J20s2XE30'                              :
#        'L1_J15.0ETA25_2J15.31ETA49'                                   :
#        'L1_DR25-TAU20ITAU12I'                                         :
#        'L1_EM7_UNPAIRED_NONISO'                                       :
#        'L1_DPHI-AJ20s2XE50'                                           :
#        'L1_2EM8I_J50'                                                 :
#        'L1_2MU4_J20_XE50_DPHI-J20s2XE30'                              :
#        'L1_TAU20IM_FTK'                                               :
#        'L1_TAU20IM_2TAU12IM_J25_2J20_3J12_FTK'                        :
#        'L1_DR-TAU20ITAU12I-J25_FTK'                                   :
#        'L1_TAU8_UNPAIRED_NONISO'                                      :
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
