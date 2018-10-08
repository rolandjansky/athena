# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### Trigger lists for SUSY2.py.
from DerivationFrameworkSUSY.SUSYCommonTriggerList import SingleLepton_2018,SingleLepton_2017,SingleLepton_2016,SingleLepton_2015
from DerivationFrameworkSUSY.SUSYCommonTriggerList import DiLepton_2018,DiLepton_2017,DiLepton_2016,DiLepton_2015

trielectron_trigger = [
    "HLT_e17_lhloose_2e9_lhloose",
    "HLT_e17_lhloose_nod0_2e9_lhloose_nod0",
    "HLT_e17_lhloose_nod0_2e10_lhloose_nod0",
    "HLT_e17_lhloose_nod0_2e10_lhloose_nod0_L1EM15VH_3EM8VH",
    "HLT_e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH",
    "HLT_e24_lhloose_nod0_2e12_lhloose_nod0_L1EM20VH_3EM10VH",
]

trimuon_trigger = [
    "HLT_mu18_2mu4noL1",
    "HLT_mu20_2mu4noL1",
    "HLT_mu22_2mu4noL1",
    "HLT_mu24_2mu4noL1",
    "HLT_mu6_2mu4",
    "HLT_3mu4",
    "HLT_3mu6",
    "HLT_3mu6_msonly",
    "HLT_3mu8_msonly",
    "HLT_mu22_mu8noL1_mu6noL1",
]

trielemuon_trigger = [
    "HLT_e12_loose_2mu10",
    "HLT_2e12_loose_mu10",
    "HLT_e12_lhloose_2mu10",
    "HLT_2e12_lhloose_mu10",
    "HLT_e12_lhloose_nod0_2mu10",
    "HLT_2e12_lhloose_nod0_mu10",
    "HLT_2e12_lhmedium_nod0_mu10",
    "HLT_e12_lhmedium_nod0_2mu10",
]


singleLeptonTriggers = SingleLepton_2018+SingleLepton_2017+SingleLepton_2016+SingleLepton_2015
diLeptonTriggers = DiLepton_2018+DiLepton_2017+DiLepton_2016+DiLepton_2015
triggersNavThin = trielectron_trigger+trimuon_trigger+trielemuon_trigger+singleLeptonTriggers+diLeptonTriggers
