# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#the Lowest unprescaled triggers for the individual periods are listed

triggers_met = [
    #2018
    'HLT_xe100_pufit_xe75_L1XE60',
    'HLT_xe110_pufit_L1XE55',
    'HLT_xe110_pufit_L1XE60',
    'HLT_xe110_pufit_L1XE70',
    'HLT_xe110_pufit_wEFMu_L1XE55',
    'HLT_xe110_pufit_xe65_L1XE55',
    'HLT_xe110_pufit_xe65_L1XE60',
    'HLT_xe110_pufit_xe70_L1XE50',
    'HLT_xe120_mht_xe80_L1XE55',
    'HLT_xe120_mht_xe80_L1XE60',
    'HLT_xe120_pufit_L1XE55',
    'HLT_xe120_pufit_L1XE60',
    'HLT_xe120_pufit_L1XE70',
    'HLT_xe120_pufit_wEFMu_L1XE55',
    'HLT_xe120_pufit_wEFMu_L1XE60',

    #2017
    'HLT_xe90_pufit_L1XE50',
    'HLT_xe100_pufit_L1XE50',
    'HLT_xe100_pufit_L1XE55',
    'HLT_xe110_pufit_L1XE50',
    'HLT_xe110_pufit_L1XE55',
    'HLT_xe110_pufit_L1XE60',
    'HLT_xe120_pufit_L1XE50',
    'HLT_xe120_pufit_L1XE55',
    'HLT_xe120_pufit_L1XE60',

    #2016
    'HLT_xe80_tc_lcw_L1XE50',
    'HLT_xe90_mht_L1XE50',
    'HLT_xe90_tc_lcw_wEFMu_L1XE50',
    'HLT_xe90_mht_wEFMu_L1XE50',
    'HLT_xe100_L1XE50',
    'HLT_xe100_tc_em_L1XE50',
    'HLT_xe110_pueta_L1XE50',
    'HLT_xe110_mht_L1XE50',
    'HLT_xe120_tc_lcw_L1XE50',
    'HLT_xe120_pueta',
    'HLT_xe120_pufit',

    #2015
    'HLT_xe70_mht',
    ]

triggers_ele = [
    #2015
    "HLT_e24_lhmedium_L1EM20VH",
    "HLT_e60_lhmedium",
    "HLT_e120_lhloose",
    
    #2016
    "HLT_e24_lhtight_nod0_ivarloose",
    "HLT_e26_lhtight_nod0_ivarloose",
    "HLT_e60_lhmedium_nod0",
    "HLT_e60_medium",
    "HLT_e140_lhloose_nod0",
    "HLT_e300_etcut",
                
    #2017
    'HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e26_lhtight_nod0_ivarloose_L1EM22VHIM',
    'HLT_e28_lhtight_nod0_ivarloose',
    'HLT_e28_lhtight_nod0_ivarloose_L1EM24VHIM',
    'HLT_e60_lhmedium_nod0',
    'HLT_e60_lhmedium_nod0_L1EM24VHI',
    'HLT_e140_lhloose_nod0',
    'HLT_e140_lhloose_nod0_L1EM24VHI',
    'HLT_e300_etcut',
    'HLT_e300_etcut_L1EM24VHI',

    #2018
    'HLT_e140_lhloose_nod0_L1EM24VHI',
    'HLT_e140_lhloose_nod0_L1EM24VHIM',
    'HLT_e28_lhtight_nod0_ivarloose',
    'HLT_e28_lhtight_nod0_ivarloose_L1EM24VHIM',
    'HLT_e28_lhtight_nod0_noringer_ivarloose',
    'HLT_e28_lhtight_nod0_noringer_ivarloose_L1EM24VHIM',
    'HLT_e300_etcut',
    'HLT_e300_etcut_L1EM24VHIM',
    'HLT_e32_lhtight_nod0_ivarloose',
    'HLT_e60_lhmedium_nod0_L1EM24VHI',
    'HLT_e60_lhmedium_nod0_L1EM24VHIM',
    'HLT_e80_lhmedium_nod0_L1EM24VHI',
    'HLT_e80_lhmedium_nod0_L1EM24VHIM',
    ]

triggers_muon = [
    #2015
    "HLT_mu20_iloose_L1MU15",
    "HLT_mu40",
    "HLT_mu60_0eta105_msonly",
                
    #2016
    'HLT_mu24_iloose',
    'HLT_mu24_iloose_L1MU15',
    'HLT_mu24_ivarloose',
    'HLT_mu24_ivarloose_L1MU15',
    'HLT_mu24_ivarmedium',
    'HLT_mu24_imedium',
    'HLT_mu26_ivarmedium',
    'HLT_mu26_imedium',
    'HLT_mu40',
    'HLT_mu50',
    
    #2017
    'HLT_mu26_ivarmedium',
    'HLT_mu50',
    'HLT_mu60',
    'HLT_mu60_0eta105_msonly',

    #2018
    'HLT_mu26_ivarmedium',
    'HLT_mu28_ivarmedium',
    'HLT_mu60',
    'HLT_mu60_0eta105_msonly',
    'HLT_mu60_msonly_3layersEC',
    'HLT_mu80',
    'HLT_mu80_msonly_3layersEC',
    ]

triggers_photon = [
    #2015
    'HLT_g120_loose',
    'HLT_g200_etcut',

    #2016
    'HLT_g140_loose',
    'HLT_g300_etcut',        
    
    #2017
    'HLT_g140_loose',
    'HLT_g140_tight',
    'HLT_g200_loose',
    'HLT_g300_etcut',
    'HLT_g300_etcut_L1EM24VHI',        

    #2018
    'HLT_g140_tight',
    'HLT_g140_tight_L1EM24VHIM',
    'HLT_g200_loose',
    'HLT_g200_loose_L1EM24VHIM',
    'HLT_g300_etcut_L1EM24VHI',
    'HLT_g300_etcut_L1EM24VHIM',
]












