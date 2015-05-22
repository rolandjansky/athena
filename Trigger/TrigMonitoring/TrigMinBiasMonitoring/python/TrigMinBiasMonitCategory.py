# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----NORMAL USER ACCESS-------
triggers = ["mb_sptrk",
            "mb_sptrk_noisesup",
            "mb_sptrk_costr",
            "noalg_mb_L1MBTS_1",
            "noalg_mb_L1MBTS_2",
            "noalg_mb_L1MBTS_1_1",
            "noalg_mb_L1TE10",
            "noalg_mb_L1TE10.0ETA25",
            #"noalg_mb_L1TE20",
            #"noalg_mb_L1TE20.0ETA25",
            "mb_perf_L1MBTS_2",	
            "mb_mbts_L1MBTS_1",
            "mb_mbts_L1MBTS_2",
            "mb_mbts_L1MBTS_1_1",
            #leading-track triggers
            #"mb_sptrk_pt4_L1MBTS_1",
            "mb_sptrk_pt4_L1MBTS_2",
            #"mb_sptrk_pt4_L1MBTS_1_1",
            #"mb_sptrk_pt6_L1MBTS_1",
            "mb_sptrk_pt6_L1MBTS_2",
            #"mb_sptrk_pt6_L1MBTS_1_1",
            #"mb_sptrk_pt8_L1MBTS_1",
            "mb_sptrk_pt8_L1MBTS_2",
            #"mb_sptrk_pt8_L1MBTS_1_1",
            #HMT triggers
            "mb_sp600_trk45_hmt_L1MBTS_1_1",	
            "mb_sp700_trk55_hmt_L1MBTS_1_1",
            "mb_sp900_trk60_hmt_L1MBTS_1_1",
            "mb_sp300_trk10_sumet50_hmt_L1MBTS_1_1",
            "mb_sp600_trk10_sumet50_hmt_L1MBTS_1_1",
            "mb_sp300_trk10_sumet60_hmt_L1MBTS_1_1",
            "mb_sp600_trk10_sumet60_hmt_L1MBTS_1_1",
            "mb_sp300_trk10_sumet70_hmt_L1MBTS_1_1",
            "mb_sp600_trk10_sumet70_hmt_L1MBTS_1_1",
            "mb_sp1400_hmtperf_L1TE10",	
            "mb_sp1400_hmtperf_L1TE10.0ETA25",	
            "mb_sp1400_hmtperf_L1TE20",	
            "mb_sp1400_hmtperf_L1TE20.0ETA25",
	    #ATR-11216
	    "mb_sptrk_vetombts2in_L1ALFA_CEP",
	    "mb_sptrk_vetombts2in_peb_L1ALFA_ANY",
            ]
		
#!----NORMAL USER ACCESS-------
		
#----ADVANCED USER ACCESS-----
#   PLEASE DO NOT MODIFY EXISTING CODE UNLESS YOU ARE PERFECTLY SURE WHAT YOU ARE DOING 

#forces uniqueness of items on the list
triggersFiltered = []
for x in triggers:
    if x not in triggersFiltered:
        triggersFiltered.append(x)

monitoring_minbias = triggersFiltered
