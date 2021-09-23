# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################ 
#
# the trigger list below is created as of Oct23, 2017 based on Twiki pages:
# https://twiki.cern.ch/twiki/bin/viewauth/Atlas/LowestUnprescaled
# https://twiki.cern.ch/twiki/bin/view/Atlas/L34TriggerMenu2017
#
################################################################ 

#get all common menu items from the common list
from DerivationFrameworkSUSY.SUSYCommonTriggerList import *

#get b-jet triggers missing from the common list
SUSY10BJetTriggers = [
    'HLT_j225_bmv2c2060_split', #2016
    'HLT_2j35_bmv2c2050_split_2j35_L14J15',
    'HLT_2j70_bmv2c2070_split_j70',
    'HLT_2j45_bmv2c2077_split_3j45',
    'HLT_2j15_gsc35_bmv2c1050_split_2j15_gsc35_boffperf_split_L14J15.0ETA25', #2017
    'HLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15.0ETA25',
    'HLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15.0ETA25',
    'HLT_j150_gsc175_bmv2c1060_split_j45_gsc60_bmv2c1060_split',
    'HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30',
    'HLT_2j15_gsc35_bmv2c1050_split_3j15_gsc35_boffperf_split',
    'HLT_2j25_gsc45_bmv2c1060_split_3j25_gsc45_boffperf_split',
    'HLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21',
    'HLT_2j35_gsc55_bmv2c1050_split_ht300_L1HT190-J15s5.ETA21',
    'HLT_2j35_gsc55_bmv2c1060_split_ht300_L1HT190-J15s5.ETA21',
    'HLT_j450',
    'HLT_j225_gsc440_boffperf_split',
    'HLT_3j225',
    'HLT_4j120',
    'HLT_4j60_gsc115_boffperf_split',
    'HLT_2j35_bmv2c1040_split_2j35_L14J15.0ETA25', #2018
    'HLT_j45_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21',
    'HLT_2j45_gsc55_bmv2c1050_split_ht300_L1HT190-J15s5.ETA21'
    ]


SUSY10LeptonTriggers = SingleLepton_2015+SingleLepton_2016+SingleLepton_2017+SingleLepton_2018+DiLepton_2015+DiLepton_2016+DiLepton_2017+DiLepton_2018
SUSY10PhotonTriggers = SinglePhoton_2015+SinglePhoton_2016+SinglePhoton_2017+SinglePhoton_2018
SUSY10XETriggers = MET_2015+MET_2016+MET_2017+MET_2018

#all triggers
SUSY10AllTriggers = SUSY10LeptonTriggers + SUSY10PhotonTriggers + SUSY10XETriggers + JetTrigger_2016 + MultiBJet_2017 + MultiBJet_2018 + MultiBJet_2016 + MultiBJet_2015 + BJetMET_2018 + BJetMET_2017 + SUSY10BJetTriggers# + SUSY10CombinedXETriggers + SUSY10BjetTriggers + SUSY10RazorTriggers + SUSY10JetTriggers

#just those we need the navigation thinning for
SUSY10ThinTriggers = SUSY10LeptonTriggers + SUSY10PhotonTriggers + SUSY10BJetTriggers + MultiBJet_2015 + MultiBJet_2016 + MultiBJet_2017 + MultiBJet_2018

