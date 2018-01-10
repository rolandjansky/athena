# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#the Lowest unprescaled triggers for the individual periods are listed

# This is copied from SUSY12, maybe these should be taken from somewhere else? /CO

triggers_met = [
                #2015
                'HLT_xe70', 
                "HLT_xe70_tc_lcw",
                "HLT_xe70_mht",
                #'HLT_xe100', 
                
                #2016
                #Period A
                "HLT_xe80_tc_lcw_L1XE50",
                "HLT_xe90_mht_L1XE50", 
                "HLT_xe100_L1XE50",
                
                #Period B-C
                #"HLT_xe80_tc_lcw_L1XE50", # => already in earlier period 
                #"HLT_xe90_mht_L1XE50", # => already in earlier period 
                
                #Period D1-D3
                #"HLT_xe90_mht_L1XE50", # => already in earlier period 
                "HLT_xe120_tc_lcw_L1XE50",
                
                #Period D4-E3
                "HLT_xe100_mht_L1XE50", 
                "HLT_xe110_mht_L1XE50",
                
                #Period F1
                #"HLT_xe100_mht_L1XE50", # => already in earlier period 
                #"HLT_xe110_mht_L1XE50", # => already in earlier period 
                
                #Period F2-
                #"HLT_xe110_mht_L1XE50" # => already in earlier period 
                
                # "Experimental"
                "HLT_xe110_mht_L1XE50", 
                "HLT_xe120_mht_L1XE50",
                "HLT_xe130_mht_L1XE50",
                "HLT_xe140_mht_L1XE50",
                "HLT_xe100_L1XE50",
                "HLT_xe110_L1XE50",
                "HLT_xe120_L1XE50",
                "HLT_xe110_mht_L1XE55", 
                "HLT_xe120_mht_L1XE55",
                "HLT_xe130_mht_L1XE55",
                "HLT_xe140_mht_L1XE55",
                "HLT_xe100_L1XE55",
                "HLT_xe110_L1XE55",
                "HLT_xe120_L1XE55",
		"HLT_xe110_mht_L1XE50_AND_xe70_L1XE50",
		"HLT_xe110_mht_L1XE50_AND_xe70_L1XE55",

                # L1 backup
                "HLT_noalg_L1J400"
                ]

triggers_jet = [
                "HLT_j340", # single jet
                "HLT_j380", 
                "HLT_j400", 
                "HLT_j450", 
                "HLT_3j175", # 3 jets
                "HLT_3j200",
                "HLT_3j225",
                "HLT_2j275_j140",
                "HLT_2j250_j120",
                "HLT_2j220_j120", 
                "HLT_4j85", # 4 jets
                "HLT_4j100", 
                "HLT_4j110",
                "HLT_4j120",
                "HLT_4j130",
                "HLT_5j55", # 5 jets                
                "HLT_5j85", 
                "HLT_5j85_lcw",
                "HLT_5j75_0eta250",
                "HLT_5j65_0eta240_L14J150ETA25",
                "HLT_5j60",
                "HLT_5j70",
                "HLT_5j70_L14J15",
                "HLT_5j85_L14J15",
                "HLT_5j90",
                "HLT_5j100",
                "HLT_6j45_0eta240", # 6 jets
                "HLT_6j55_0eta240_L14J150ETA25",
                "HLT_6j45",
                "HLT_6j60",
                "HLT_6j70",
                "HLT_6j60_L14J15",
                "HLT_6j70_L14J15",
                "HLT_6j85",
                "HLT_7j45", # 7 jets
                "HLT_7j45_L14J15", 
                "HLT_7j50",
                "HLT_4j60_gsc100_boffperf_split", #gsc
                "HLT_4j60_gsc110_boffperf_split",
                "HLT_4j60_gsc115_boffperf_split",
                "HLT_4j70_gsc120_boffperf_split",
                "HLT_4j70_gsc130_boffperf_split",
                "HLT_5j50_gsc70_boffperf_split",
                "HLT_5j55_gsc75_boffperf_split",
                "HLT_5j60_gsc85_boffperf_split",
                "HLT_5j60_gsc90_boffperf_split",
                "HLT_5j60_gsc100_boffperf_split",
                "HLT_6j45_gsc60_boffperf_split",
                "HLT_6j50_gsc65_boffperf_split", 
                "HLT_6j50_gsc70_boffperf_split",
                "HLT_6j60_gsc85_boffperf_split",
                "HLT_7j25_gsc45_boffperf_split_L14J20",
                "HLT_7j25_gsc50_boffperf_split_L14J20"
                
]

triggers_lep = [
                "HLT_mu60_0eta105_msonly",
                "HLT_g140_loose",
                "HLT_2g50_loose",
                "HLT_mu26_ivarmedium"
]
