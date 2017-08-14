# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#the Lowest unprescaled triggers for the individual periods are listed

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
                "HLT_noalg_L1J400"
                ]

triggers_ele = [
                #2015
                "HLT_e24_lhmedium_L1EM20VH",
                "HLT_e60_lhmedium",
                "HLT_e120_lhloose",
                
                #2016
                #Period A-D3
                "HLT_e24_lhtight_nod0_ivarloose",
                "HLT_e60_lhmedium_nod0",
                "HLT_e60_medium",
                "HLT_e140_lhloose_nod0",
                "HLT_e300_etcut",
                
                #Period D4-
                "HLT_e26_lhtight_nod0_ivarloose",
                #"HLT_e60_lhmedium_nod0", # => already in earlier period 
                #"HLT_e60_medium",
                #"HLT_e140_lhloose_nod0 " # => already in earlier period 
               ]

triggers_muon = [
                #2015
                "HLT_mu20_iloose_L1MU15",
                "HLT_mu40",
                "HLT_mu60_0eta105_msonly",
                
                #2016
                #Period A
                "HLT_mu24_iloose",
                "HLT_mu24_iloose_L1MU15", # Different naming scheme in MC
                "HLT_mu24_ivarloose",
                "HLT_mu24_ivarloose_L1MU15", # Different naming scheme in MC
                "HLT_mu40",
                "HLT_mu50",
                
                #Period B-D3
                "HLT_mu24_ivarmedium",
                "HLT_mu24_imedium",
                #"HLT_mu50", # => already in earlier period 
                
                #Period D4-E
                "HLT_mu26_ivarmedium",
                "HLT_mu26_imedium"
                #"HLT_mu50" # => already in earlier period 
                
                #Period F-
                #"HLT_mu26_ivarmedium", # => already in earlier period 
                #"HLT_mu26_imedium", # => already in earlier period 
                #"HLT_mu50" # => already in earlier period 
                ]













