# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----NORMAL USER ACCESS-------
triggers = ["mb_sptrk",
            "mb_sp",
            "mb_sptrk_ion_L1ZDC_A_C_VTE50",
            #"mb_sptrk_noisesup",
            #"mb_sptrk_costr",
	    "noalg_mb_L1RD0_FILLED",
            "noalg_mb_L1MBTS_1",
            #"noalg_mb_L1MBTS_2",
            "noalg_mb_L1MBTS_1_1",
	    "noalg_mb_L1ZDC_A_C",
            "noalg_mb_L1TE10",
            "noalg_mb_L1TE20",
	    "noalg_mb_L1TE30",
	    "noalg_mb_L1TE50",
            "mb_perf_L1MBTS_1_1",	
            "mb_mbts_L1MBTS_1_1",
            #leading-track triggers
            "mb_sptrk_pt4_L1MBTS_1",
            #"mb_sptrk_pt4_L1MBTS_2",
            #"mb_sptrk_pt4_L1MBTS_1_1",
            "mb_sptrk_pt6_L1MBTS_1",
            #"mb_sptrk_pt6_L1MBTS_2",
            #"mb_sptrk_pt6_L1MBTS_1_1",
            "mb_sptrk_pt8_L1MBTS_1",
            #"mb_sptrk_pt8_L1MBTS_2",
            #"mb_sptrk_pt8_L1MBTS_1_1",
            #HMT triggers
	    "mb_sp700_hmtperf_L1TE5",
	    "mb_sp1500_hmtperf_L1TE10",
	    "mb_sp2100_hmtperf_L1TE10",
	    "mb_sp1600_pusup1100_trk90_hmt_L1TE10",
	    "mb_sp1400_pusup550_trk90_hmt_L1TE10",
	    #ATR-11216
	    #"mb_sptrk_vetombts2in_L1ALFA_CEP",
	    #"mb_sptrk_vetombts2in_peb_L1ALFA_ANY",
	    #UPC
            "mb_sptrk_vetombts2in_L1ZDC_A_C_VTE50",
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
