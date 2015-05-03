# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----NORMAL USER ACCESS-------
triggers = ["mb_sptrk",	
            "mb_sp2000_trk70_hmt", 
            "mb_sp2000_pusup600_trk70_hmt", 
            "mb_sp2000_pusup600_trk70_hmt_L1TE30", 
            "mb_sp2000_pusup700_trk70_hmt_L1TE30", 
            "mb_sp2000_pusup600_trk70_hmt_L1TE40",	
            "mb_sp1800_hmtperf_L1TE20",	
            "mb_perf_L1MBTS_1",	
            "mb_perf_L1MBTS_2",	
            "mb_idperf_L1MBTS_2",
            "noalg_mb_L1MBTS_1",
            "noalg_mb_L1MBTS_1_1",
            "noalg_mb_L1MBTS_2",
            "mb_mbts_L1MBTS_2",
            "mb_sptrk_noisesup", 
            "mb_sptrk_costr"]
		
#!----NORMAL USER ACCESS-------
		
#----ADVANCED USER ACCESS-----
#   PLEASE DO NOT MODIFY EXISTING CODE UNLESS YOU ARE PERFECTLY SURE WHAT YOU ARE DOING 

#forces uniqueness of items on the list
triggersFiltered = []
for x in triggers:
    if x not in triggersFiltered:
        triggersFiltered.append(x)

monitoring_minbias = triggersFiltered
