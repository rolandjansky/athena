# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

primary_single_ele = ['e17_lhloose_L1EM15','e24_lhmedium_L1EM18VH','e24_lhtight_L1EM20VH']
primary_single_ele_cutbased = ['e17_loose_L1EM15','e24_medium_L1EM18VH','e24_tight_L1EM20VH'] 
primary_double_ele = ['2e12_lhloose_L12EM10VH','2e15_lhloose_L12EM13VH','2e17_lhloose']
primary_double_ele_cutbased = ['2e12_loose_L12EM10VH','2e15_loose_L12EM13VH','2e17_loose']
monitoring_ele_idperf = ['e5_lhloose_idperf','e5_lhtight_idperf','e10_lhtight_idperf','e24_lhmedium_idperf_L1EM20VH']
monitoring_ele_idperf_cutbased = ['e5_loose_idperf','e5_tight_idperf','e10_tight_idperf','e24_medium_idperf_L1EM20VH']
monitoring_Zee = ['e24_lhtight_L1EM20VH_e15_etcut_Zee','e26_lhtight_e15_etcut_Zee']
monitoring_Jpsiee = ['e5_lhtight_e4_etcut','e5_lhtight_e4_etcut_Jpsiee']
primary_single_pho = ['g10_loose','g20_loose_L1EM15','g120_loose','g140_loose']
primary_double_pho = ['2g10_loose','g35_loose_g25_loose','g35_medium_g25_medium']

monitoring_egamma = primary_single_ele + primary_single_ele_cutbased + primary_double_ele + primary_double_ele_cutbased + monitoring_ele_idperf + monitoring_ele_idperf_cutbased + monitoring_Zee + monitoring_Jpsiee + primary_single_pho + primary_double_pho

