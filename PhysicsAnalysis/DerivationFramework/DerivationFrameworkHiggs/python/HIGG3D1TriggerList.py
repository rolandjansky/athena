# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
################################################################
# the trigger list below is created as of Jul12, 2018 based on Twiki pages:
# https://twiki.cern.ch/twiki/bin/viewauth/Atlas/LowestUnprescaled
# https://twiki.cern.ch/twiki/bin/view/Atlas/L34TriggerMenu2017
# 2018 menu is based on the trigger API: https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI
#################################################################

# 2018 menu (covering 1.5e34 to 2.3e34 menus)
SingleLepton_2018 = [
    # Single Electron
    'HLT_e140_lhloose_nod0'
    'HLT_e140_lhloose_nod0_L1EM24VHI',
    'HLT_e140_lhloose_nod0_L1EM24VHIM',
    'HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e26_lhtight_nod0',
    'HLT_e28_lhtight_nod0_ivarloose',
    'HLT_e28_lhtight_nod0_ivarloose_L1EM24VHIM',
    'HLT_e28_lhtight_nod0_noringer_ivarloose',
    'HLT_e28_lhtight_nod0_noringer_ivarloose_L1EM24VHIM',
    'HLT_e300_etcut',
    'HLT_e300_etcut_L1EM24VHIM',
    'HLT_e32_lhtight_nod0_ivarloose',
    'HLT_e60_lhmedium_nod0'
    'HLT_e60_lhmedium_nod0_L1EM24VHI',
    'HLT_e60_lhmedium_nod0_L1EM24VHIM',
    'HLT_e80_lhmedium_nod0_L1EM24VHI',
    'HLT_e80_lhmedium_nod0_L1EM24VHIM',
    # Single Muon
    'HLT_mu26_ivarmedium',
    'HLT_mu28_ivarmedium',
    'HLT_mu50',
    'HLT_mu60',
    'HLT_mu60_0eta105_msonly',
    'HLT_mu60_msonly_3layersEC',
    'HLT_mu80',
    'HLT_mu80_msonly_3layersEC',
]
DiLepton_2018 = [
    # 1e + 1mu
    'HLT_e26_lhmedium_nod0_mu8noL1',
    'HLT_e17_lhloose_nod0_mu14',
    'HLT_e7_lhmedium_nod0_mu24',
    # 2e
    'HLT_2e17_lhvloose_nod0_L12EM15VHI',
    'HLT_2e19_lhvloose_nod0',
    'HLT_2e24_lhvloose_nod0',
    # 2e, Special Trigger
    'HLT_e28_lhtight_nod0_L1EM24VHIM_e15_etcut_L1EM7_Zee',
    'HLT_e28_lhtight_nod0_e15_etcut_L1EM7_Zee',
    # 2mu
    'HLT_2mu14',
    'HLT_2mu15_L12MU10',
    'HLT_mu20_ivarmedium_mu8noL1',
    'HLT_mu22_mu8noL1',
    'HLT_mu24_mu12noL1',
    'HLT_mu24_mu10noL1',
    'HLT_mu24_mu8noL1',
    'HLT_mu26_mu10noL1',
    'HLT_mu26_mu8noL1',
    'HLT_mu28_mu8noL1',
]
#2017 Menu
SingleLepton_2017 = [
    # Single electron
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
    # Single muon
    'HLT_mu26_ivarmedium',
    'HLT_mu50',
    'HLT_mu60',
    'HLT_mu60_0eta105_msonly',
]

DiLepton_2017 = [
    # 2e
    'HLT_2e17_lhvloose_nod0_L12EM15VHI',
    'HLT_2e24_lhvloose_nod0',
    # e + mu
    'HLT_e17_lhloose_nod0_mu14',
    'HLT_e26_lhmedium_nod0_mu8noL1',
    'HLT_e7_lhmedium_nod0_mu24',
    # 2 mu
    'HLT_2mu14',
    'HLT_mu22_mu8noL1',
    'HLT_mu22_mu8noL1_calotag_0eta010',
    'HLT_mu24_mu8noL1',
    'HLT_mu24_mu8noL1_calotag_0eta010',
    'HLT_mu26_mu10noL1',
]

#2016 Menu
SingleLepton_2016 = [
    # Single e
    'HLT_e24_lhtight_nod0_ivarloose',
    'HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e26_lhtight_smooth_ivarloose',
    'HLT_e28_lhtight_nod0_ringer_ivarloose',
    'HLT_e60_lhmedium_nod0',
    'HLT_e60_medium',
    'HLT_e140_lhloose_nod0',
    'HLT_e300_etcut',
    # Sinlge muon
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
    'HLT_mu60_0eta105_msonly',
]


DiLepton_2016 = [
    # 2e
    'HLT_2e15_lhvloose_nod0_L12EM13VH',
    'HLT_2e17_lhvloose_nod0',
    # e + mu
    'HLT_e17_lhloose_nod0_mu14',
    'HLT_e7_lhmedium_nod0_mu24',
    'HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1',
    'HLT_e26_lhmedium_nod0_L1EM22VHI_mu8noL1',
    # 2 mu
    'HLT_2mu10',
    'HLT_2mu10_nomucomb',
    'HLT_2mu14',
    'HLT_2mu14_nomucomb',
    'HLT_mu20_mu8noL1',
    'HLT_mu20_nomucomb_mu6noL1_nscan03',
    'HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb',
    'HLT_mu22_mu8noL1',
]

#2015 Menu Lowest Unprescaled + TriggerAPI
SingleLepton_2015 = [
    # Single electron
    #'HLT_e24_tight_iloose_HLTCalo_L1EM20VH',
    #'HLT_e24_lhmedium_nod0_iloose_L1EM20VH',
    #'HLT_e24_tight_iloose_L2EFCalo_L1EM20VH',
    #'HLT_e24_lhtight_iloose_HLTCalo_L1EM20VH',
    #'HLT_e24_tight_iloose_L1EM20VH',
    #'HLT_e24_lhtight_iloose',
    #'HLT_e24_lhmedium_iloose_HLTCalo_L1EM20VH',
    #'HLT_e24_medium_iloose_L1EM20VH',
    #'HLT_e24_lhtight_iloose_L2EFCalo_L1EM20VH',
    #'HLT_e24_medium_L1EM20VH',
    'HLT_e24_lhmedium_L1EM20VH',
    #'HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM20VH',
    #'HLT_e26_lhtight_smooth_iloose',
    'HLT_e60_lhmedium',
    #'HLT_e60_lhmedium_nod0',
    #'HLT_e60_lhmedium_HLTCalo',
    #'HLT_e120_loose',
    'HLT_e120_lhloose',
    #'HLT_e120_lhloose_HLTCalo',
    #'HLT_e120_lhloose_nod0',
    #'HLT_e200_etcut',
    # Single Muon
    'HLT_mu20_iloose_L1MU15',
    'HLT_mu40',
    'HLT_mu60_0eta105_msonly',
]

DiLepton_2015 = [
    # 2 e
    'HLT_2e12_lhloose_L12EM10VH',
    # e + mu
    'HLT_e17_lhloose_mu14',
    'HLT_e24_lhmedium_L1EM20VHI_mu8noL1',
    'HLT_e7_lhmedium_mu24',
    # 2 mu
    'HLT_2mu10',
    'HLT_mu18_mu8noL1',
]

#Mixed TriLepton
triggersTriLep = [
    # Trimuon triggers
    'HLT_3mu4',
    'HLT_3mu6',
    'HLT_3mu6_msonly',
    'HLT_3mu8_msonly',
    'HLT_mu24_2mu4noL1',
    'HLT_mu22_2mu4noL1',
    'HLT_mu20_2mu4noL1',
    'HLT_mu18_2mu4noL1',
    'HLT_mu22_mu8noL1_mu6noL1',
    'HLT_mu6_2mu4',
    # tri-electron
    'HLT_e17_lhloose_2e9_lhloose',
    'HLT_e17_lhloose_nod0_2e9_lhloose_nod0',
    'HLT_e17_lhloose_nod0_2e10_lhloose_nod0',
    'HLT_e17_lhloose_nod0_2e10_lhloose_nod0_L1EM15VH_3EM8VH',
    'HLT_e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH',
    'HLT_e24_lhloose_nod0_2e12_lhloose_nod0_L1EM20VH_3EM10VH',
    # mixed trilepton
    'HLT_e12_lhloose_2mu10',
    'HLT_e12_lhloose_nod0_2mu10',
    'HLT_e12_lhmedium_nod0_2mu10',
    'HLT_2e12_lhloose_mu10',
    'HLT_2e12_lhloose_nod0_mu10',
    'HLT_2e12_lhmedium_nod0_mu10',
]

SpecialLepton = [
    'HLT_e24_tight_L1EM20VH_e15_etcut_Zee',
    'HLT_e24_lhtight_L1EM20VH_e15_etcut_Zee',
    'HLT_e26_lhtight_nod0_e15_etcut_L1EM7_Zee',
    'HLT_mu11_nomucomb_2mu4noL1_nscan03_L1MU11_2MU6',
    'HLT_mu11_2mu4noL1_nscan03_L1MU11_2MU6',
    'HLT_mu20_mu6noL1_nscan03',
    'HLT_mu20_nomucomb_mu6noL1_nscan03',
    'HLT_mu20_msonly_mu6noL1_msonly_nscan05',
    'HLT_mu13_mu13_idperf_Zmumu',
    'HLT_mu18_2mu4_JpsimumuL2',
    'HLT_mu18_2mu0noL1_JpsimumuFS',
    'HLT_2mu6_10invm30_pt2_z10',
]

# combine, removing duplicates
SingleLepton_Combined=list(set(SingleLepton_2015+SingleLepton_2016+SingleLepton_2017+SingleLepton_2018))
Dilepton_Combined=list(set(DiLepton_2015+DiLepton_2016+DiLepton_2017+DiLepton_2018))

#Alternative TriggerSkimming for TrigNavigation
triggersNavThin=list(set(SingleLepton_2015+DiLepton_2015+SingleLepton_2016+DiLepton_2016+SingleLepton_2017+DiLepton_2017+SingleLepton_2018+DiLepton_2018))
