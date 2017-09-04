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
                "HLT_4j100",
                "HLT_5j85",
                "HLT_5j85_lcw",
                "HLT_5j75_0eta250",
                "HLT_6j45_0eta240",
                "HLT_7j45"
]

triggers_lep = [
                "HLT_mu60_0eta105_msonly",
                "HLT_g140_loose",
                "HLT_2g50_loose",
                "HLT_mu26_ivarmedium"
]
