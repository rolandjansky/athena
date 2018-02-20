# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### Trigger lists for SUSY2.py.
from DerivationFrameworkSUSY.SUSYCommonTriggerList import SingleLepton_2017,SingleLepton_2016,SingleLepton_2015
from DerivationFrameworkSUSY.SUSYCommonTriggerList import DiLepton_2017,DiLepton_2016,DiLepton_2015

singleelectron_trigger = [
  "HLT_e60_lhmedium",
  "HLT_e26_lhtight_iloose",
  "HLT_e24_lhtight_iloose",
  "HLT_e24_lhmedium_iloose_L1EM20VH",
  "HLT_e24_lhmedium_iloose_L1EM18VH",
  "HLT_e12_lhloose_L1EM10VH",
  "HLT_e15_lhloose_L1EM13VH",
  "HLT_e17_lhloose",
  "HLT_e17_lhloose",
  "HLT_e26_lhmedium_L1EM22VHI",
  "HLT_e24_lhmedium_L1EM20VH",
  "HLT_e17_lhmedium",
  "HLT_e9_lhmedium",
  "HLT_e12_loose_L1EM10VH",
  "HLT_e15_loose_L1EM13VH",
  "HLT_e17_loose",
  "HLT_e7_lhmedium",
  "HLT_e60_lhmedium_nod0",
  "HLT_e24_lhmedium_nod0_iloose_L1EM20VH",
  "HLT_e24_lhtight_nod0_iloose",
  "HLT_e26_lhtight_nod0_iloose",
  "HLT_e300_etcut",
  "HLT_e140_lhloose_nod0",
  "HLT_e26_lhtight_nod0_ivarloose",
  "HLT_e24_lhtight_nod0_ivarloose",
  "HLT_e24_lhmedium_nod0_L1EM20VH",
  "HLT_e120_lhloose"
]
dielectron_trigger = [
    "HLT_2e12_lhloose_L12EM10VH",
    "HLT_2e15_lhloose_L12EM13VH",
    "HLT_2e17_lhloose",
    "HLT_2e12_loose_L12EM10VH",
    "HLT_2e15_loose_L12EM13VH",
    "HLT_2e17_loose",
    "HLT_2e17_lhvloose_nod0",
    "HLT_2e12_lhvloose_nod0_L12EM10VH",
    "HLT_2e15_lhvloose_nod0_L12EM13VH",
    "HLT_2e17_lhvloose_nod0_L12EM15VHI",
    "HLT_2e24_lhvloose_nod0"
]
trielectron_trigger = [
    "HLT_e17_lhmedium_2e9_lhmedium",
    "HLT_e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH",
    "HLT_e17_medium_2e9_medium"
]

singlemuon_trigger = [
    "HLT_mu14",
    "HLT_mu24",
    "HLT_mu26_imedium",
    "HLT_mu24_imedium",
    "HLT_mu24_ivarmedium",
    "HLT_mu24_iloose_L1MU15",
    "HLT_mu20_iloose_L1MU15",
    "HLT_mu50",
    "HLT_mu60_0eta105_msonly",
    "HLT_mu14",
    "HLT_mu10",
    "HLT_mu22",
    "HLT_mu20",
    "HLT_mu18",
    "HLT_mu6",
    "HLT_mu6_msonly",
    "HLT_mu40",
    "HLT_mu24_iloose",
    "HLT_mu26_ivarmedium"
]
dimuon_trigger = [
    "HLT_2mu14",
    "HLT_2mu10",
    "HLT_mu24_mu8noL1",
    "HLT_mu22_mu8noL1",
    "HLT_mu20_mu8noL1",
    "HLT_mu18_mu8noL1",
    "HLT_2mu14_nomucomb",
    "HLT_2mu10_nomucomb",
    "HLT_mu22_mu8noL1_calotag_0eta010"
]
trimuon_trigger = [
    "HLT_mu24_2mu4noL1",
    "HLT_mu22_2mu4noL1",
    "HLT_mu20_2mu4noL1",
    "HLT_mu18_2mu4noL1",
    "HLT_3mu6",
    "HLT_3mu6_msonly"
]

elemuon_trigger = [
    "HLT_e17_lhloose_mu14",
    "HLT_e7_lhmedium_mu24",
    "HLT_e26_lhmedium_L1EM22VHI_mu8noL1",
    "HLT_e24_lhmedium_L1EM20VHI_mu8noL1",
    "HLT_e17_loose_mu14",
    "HLT_e26_lhmedium_nod0_L1EM22VHI_mu8noL1",
    "HLT_e7_lhmedium_nod0_mu24",
    "HLT_e17_lhloose_nod0_mu14",
    "HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1",
    "HLT_e26_lhmedium_nod0_mu8noL1"
]
trielemuon_trigger = [
    "HLT_e12_loose_2mu10",
    "HLT_2e12_loose_mu10",
    "HLT_e12_lhloose_2mu10",
    "HLT_2e12_lhloose_mu10",
    "HLT_e12_lhloose_nod0_2mu10",
    "HLT_2e12_lhloose_nod0_mu10"
]


singleLeptonTriggers = SingleLepton_2017+SingleLepton_2016+SingleLepton_2015
diLeptonTriggers = DiLepton_2017+DiLepton_2016+DiLepton_2015
triggersNavThin = singleelectron_trigger+dielectron_trigger+trielectron_trigger+singlemuon_trigger+dimuon_trigger+trimuon_trigger+elemuon_trigger+trielemuon_trigger+singleLeptonTriggers+diLeptonTriggers
