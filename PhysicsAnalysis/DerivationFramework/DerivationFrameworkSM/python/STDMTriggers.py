# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def list_combine_OR(triggerList):
    if (len(triggerList) == 0): return "(0)"
    return " (" + " || ".join(triggerList) + ") "

single_e_triggers=[
"HLT_e120_lhloose",
"HLT_e120_lhloosemc",
"HLT_e120_lhloose_nod0",
"HLT_e120_loose",
"HLT_e140_lhloose",
"HLT_e140_lhloose_nod0",
"HLT_e140_loose",
"HLT_e20_lhmedium_nod0_2g10_loose",
"HLT_e20_lhmedium_nod0_g35_loose",
"HLT_e24_lhmedium_iloose_L1EM18VH",
"HLT_e24_lhmedium_iloose_L1EM20VH",
"HLT_e24_lhmedium_L1EM18VH",
"HLT_e24_lhmedium_L1EM20VH",
"HLT_e24_lhmedium_L1EM20VHI",
"HLT_e24_lhmedium_nod0_L1EM15VH_g25_medium",
"HLT_e24_lhmedium_nod0_L1EM20VH",
"HLT_e24_lhtight_iloose",
"HLT_e24_lhtight_iloose_L1EM20VH",
"HLT_e24_lhtight_nod0_ivarloose",
"HLT_e24_lhvloose_nod0_L1EM20VH",
"HLT_e24_medium_iloose_L1EM18VH",
"HLT_e24_medium_iloose_L1EM20VH",
"HLT_e24_tight_iloose",
"HLT_e24_tight_iloose_L1EM20VH",
"HLT_e26_lhtight_iloose",
"HLT_e26_lhtight_nod0_ivarloose",
"HLT_e26_tight_iloose",
"HLT_e300_etcut",
"HLT_e60_lhmedium",
"HLT_e60_lhmedium_nod0",
"HLT_e60_medium"]

multi_e_triggers=[
"HLT_e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH",
"HLT_e17_lhloose_nod0_2e9_lhloose_nod0",
"HLT_e17_lhloose_nod0_2e10_lhloose_nod0_L1EM15VH_3EM8VH",
"HLT_2e12_lhloose_L12EM10VH",
"HLT_2e12_lhvloose_L12EM10VH",
"HLT_2e12_loose_L12EM10VH",
"HLT_2e15_lhloose_L12EM13VH",
"HLT_2e15_lhvloose_nod0_L12EM13VH",
"HLT_2e15_loose_L12EM13VH",
"HLT_2e17_lhloose",
"HLT_2e17_lhloose_L12EM15",
"HLT_2e17_lhvloose_nod0",
"HLT_2e17_lhvloose_nod0_L12EM15VHI",
"HLT_2e17_loose",
"HLT_2e17_loose_L12EM15",
"HLT_2e24_lhvloose_nod0"
]

single_mu_triggers=[
"HLT_mu20_iloose_L1MU15",
"HLT_mu24_iloose_L1MU15",
"HLT_mu24_iloose",
"HLT_mu24_imedium",
"HLT_mu24_ivarmedium",
"HLT_mu24_L1MU15",
"HLT_mu26_imedium",
"HLT_mu26_ivarmedium",
"HLT_mu26_medium",
"HLT_mu40",
"HLT_mu50",
"HLT_mu60_0eta105_msonly"
]

multi_mu_triggers=[
"HLT_2mu10",
"HLT_2mu14",
"HLT_2mu4_bUpsimumu",
"HLT_2mu6_10invm30_pt2_z10",
"HLT_2mu6_bBmumu",
"HLT_2mu6_bUpsimumu",
"HLT_mu20_mu8noL1",
"HLT_mu22_mu8noL1",
"HLT_mu22_mu8noL1_calotag_0eta010",
"HLT_mu24_mu8noL1",
]

mixed_emu_triggers=[
"HLT_e17_lhloose_nod0_mu14",
"HLT_e17_loose_mu14",
"HLT_e24_lhmedium_L1EM20VHI_mu8noL1",
"HLT_e26_lhmedium_nod0_mu8noL1",
"HLT_e7_lhmedium_nod0_mu24",
"HLT_e12_lhloose_nod0_2mu10",
"HLT_2e12_lhloose_nod0_mu10"
]
