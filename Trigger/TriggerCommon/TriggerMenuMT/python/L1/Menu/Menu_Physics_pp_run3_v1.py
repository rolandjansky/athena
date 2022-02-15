# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def defineMenu():

    import TriggerMenuMT.L1.Menu.Menu_MC_pp_run3_v1 as mc_menu
    from TriggerMenuMT.L1.Base.L1MenuFlags import L1MenuFlags

    mc_menu.defineMenu()

    L1MenuFlags.ApplyCTPLimits = True

    L1MenuFlags.ThresholdMap = {
        # Do not use for L1Topo decision threshold!
        'jXEPerf100' :'',
    }

    L1MenuFlags.ItemMap = {
        # Phase-I items not for commissioning        
        #'L1_MU14FCH_jJ80' : '',
        #'L1_MU14FCH_jXE70': '',

        # jLJ thresholds for production
        'L1_jLJ60' : '',
        'L1_jLJ100': '',
        'L1_jLJ160': '',
        'L1_jLJ200': '',
        # MC Performance thresholds
        'L1_jXEPerf100':'', 
    } 

    #----------------------------------------------
    def remapItems():  
        itemsToRemove = []
        for itemIndex, itemName in enumerate(L1MenuFlags.items()):
            if (itemName in L1MenuFlags.ItemMap()):
                if (L1MenuFlags.ItemMap()[itemName] != ''):
                    L1MenuFlags.items()[itemIndex] = L1MenuFlags.ItemMap()[itemName]                                                
                else: 
                    itemsToRemove.append(itemIndex)

        for i in reversed(itemsToRemove):
            del L1MenuFlags.items()[i]
    #----------------------------------------------
                                           
    remapItems()

