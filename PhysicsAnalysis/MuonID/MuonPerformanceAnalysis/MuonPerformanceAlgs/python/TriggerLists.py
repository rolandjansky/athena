# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#========================================================================================================================
def GetTriggerList_Zmumu():
    TriggerList_HLT = [
                "HLT_mu20_iloose_L1MU15",
                "HLT_mu20_ivarloose_L1MU15",
                "HLT_mu24_iloose_L1MU15",
                "HLT_mu24_ivarloose_L1MU15",
                "HLT_mu24_iloose",
                "HLT_mu24_ivarloose",
                "HLT_mu24_imedium",
                "HLT_mu24_ivarmedium",
                "HLT_mu26_imedium",
                "HLT_mu26_ivarmedium",
                "HLT_mu28_imedium",
                "HLT_mu28_ivarmedium",
                "HLT_mu40",
                "HLT_mu50",
                "HLT_AllTriggers", # this is needed, if you want to have combined (inclusive) efficiency of all HLT triggers
                ]

    TriggerList_L1 = ["L1_MU6",
                      "L1_MU4",
                      "L1_MU10",
                      "L1_MU15",
                      "L1_MU20",]

    return {
        "L1": TriggerList_L1,
        "HLT":TriggerList_HLT
    }

#========================================================================================================================
def GetTriggerList_Zmumu_RerunMode(): 
    '''
    These triggers are needed to compute multi-muon trigger SFs
    '''

    TriggerList_HLT = ["HLT_mu6",
                       "HLT_mu8",
                       "HLT_mu10",
                       "HLT_mu10_nomucomb",
                       "HLT_mu14",
                       "HLT_mu14_nomucomb",
                       "HLT_mu18",
                       "HLT_mu20_L1MU15",
                       "HLT_mu20",
                       "HLT_mu20_msonly",
                       "HLT_mu22",
                       "HLT_mu24",
                       "HLT_mu24_L1MU15",
                       "HLT_mu26",                       ]
    TriggerList_L1  = []
                              

    return {
        "L1": TriggerList_L1,
        "HLT":TriggerList_HLT
    }
     
#========================================================================================================================
def GetTriggerList_Zmumu_Observer(): 
    '''
    These triggers are needed to compute multi-muon trigger SFs
    '''

    TriggerList_HLT = [
                       "HLT_2mu10",
                       "HLT_2mu10_nomucomb",
                       "HLT_2mu14",
                       "HLT_2mu14_nomucomb",
                       "HLT_2mu15",
                       "HLT_2mu15_nomucomb",
                       "HLT_mu18_mu8noL1",
                       "HLT_mu20_mu8noL1",
                       "HLT_mu22_mu8noL1",
                       "HLT_mu24_mu8noL1",
                       "HLT_mu26_mu8noL1",
                       "HLT_mu60_0eta105_msonly",
                       ]
                       

    TriggerList_L1 = []

    return {
        "L1": TriggerList_L1,
        "HLT":TriggerList_HLT
    }

