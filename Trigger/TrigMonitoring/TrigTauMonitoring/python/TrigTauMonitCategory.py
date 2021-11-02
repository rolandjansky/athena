# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

monitoring_singleTau = [
    'tau25_medium1_tracktwo'
]

monitoring_tau = [
    'tau0_ptonly_L1TAU8',
    'tau0_ptonly_L1TAU60',
    'tau25_idperf_track',
    'tau25_idperf_tracktwo',
    'tau25_perf_tracktwo',
    'tau25_medium1_tracktwo',
    'tau35_perf_tracktwo_tau25_perf_tracktwo',
    'tau35_medium1_tracktwo_tau25_medium1_tracktwo',
    'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',
    'mu14_iloose_tau25_perf_tracktwo',
    'tau160_idperf_track',
    'tau160_idperf_tracktwo',
    'tau160_perf_tracktwo',
    'tau160_medium1_tracktwo',
    'tau1_cosmic_track_L1MU4_EMPTY',
    'tau1_cosmic_ptonly_L1MU4_EMPTY',
    'tau8_cosmic_ptonly',
    'tau8_cosmic_track'
]

monitoring_tau_pp = [
    # Run 2 data reprocessing
    'tau25_idperf_tracktwo',
    'tau25_idperf_tracktwoEF',
    'tau25_idperf_tracktwoMVA',
    'tau25_perf_tracktwo',
    'tau25_perf_tracktwoEF',
    'tau25_perf_tracktwoMVA',
    'tau25_medium1_tracktwo',
    'tau25_medium1_tracktwoEF',
    'tau25_mediumRNN_tracktwoMVA',
    # 2015
    'tau80_medium1_track',
    'tau80_medium1_tracktwo',
    'tau125_medium1_track',
    'tau125_medium1_tracktwo',  
    'tau160_medium1_tracktwo', # also 2016
    'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',
    'tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12',
    'tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45',
    # 2016
    'tau35_medium1_tracktwo_tau25_medium1_tracktwo',
    'tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12',
    'tau125_medium1_tracktwo_tau50_medium1_tracktwo_L1TAU12', 
    'tau35_medium1_tracktwo_xe70_L1XE45',
    # 2017
    'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25',
    'tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',
    'tau80_medium1_tracktwo_L1TAU60_tau35_medium1_tracktwo_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',
    'tau35_medium1_tracktwo_tau25_medium1_tracktwo_xe50',
    # 2018
    'tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_L1DR-TAU20ITAU12I-J25',
    'tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_03dR30_L1DR-TAU20ITAU12I-J25',
    'tau80_medium1_tracktwoEF_L1TAU60_tau35_medium1_tracktwoEF_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',
    'tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1DR-TAU20ITAU12I-J25',
    'tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dR30_L1DR-TAU20ITAU12I-J25',
    'tau80_mediumRNN_tracktwoMVA_L1TAU60_tau35_mediumRNN_tracktwoMVA_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',
    'tau35_medium1_tracktwoEF_xe70_L1XE45',
    'tau60_medium1_tracktwoEF_tau25_medium1_tracktwoEF_xe50',
    'tau60_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_xe50',
    'tau35_mediumRNN_tracktwoMVA_xe70_L1XE45',

     # Run 3 
    'tau0_ptonly_L1TAU100',
    'tau25_idperf_tracktwoMVA_L1TAU12IM',
    'tau25_idperf_tracktwoMVABDT_L1TAU12IM',
    'tau25_perf_tracktwoMVA_L1TAU12IM',
    'tau25_perf_tracktwoMVABDT_L1TAU12IM',
    'tau25_mediumRNN_tracktwoMVA_L1TAU12IM',
    'tau25_mediumRNN_tracktwoMVABDT_L1TAU12IM',
    'tau80_medium1_tracktwo_L1TAU60',
    'tau160_idperf_track_L1TAU100',
    'tau160_idperf_tracktwo_L1TAU100',
    'tau160_idperf_tracktwoEF_L1TAU100',
    'tau160_idperf_tracktwoMVA_L1TAU100',
    'tau160_idperf_tracktwoMVABDT_L1TAU100',
    'tau160_perf_track_L1TAU100',
    'tau160_perf_tracktwo_L1TAU100',
    'tau160_perf_tracktwoEF_L1TAU100',
    'tau160_perf_tracktwoMVA_L1TAU100',
    'tau160_perf_tracktwoMVABDT_L1TAU100',
    'tau160_mediumRNN_tracktwoMVA_L1TAU100', # valid also in 2018 for 2 data reprocessing
    'tau160_mediumRNN_tracktwoMVABDT_L1TAU100',
    'tau160_medium1_track_L1TAU100',
    'tau160_medium1_tracktwo_L1TAU100', # valid also in 2017 for 2 data reprocessing
    'tau160_medium1_tracktwoEF_L1TAU100', # valid also in 2018 for 2 data reprocessing
    'tau180_mediumRNN_tracktwoLLP_L1TAU100', #llp
    'tau180_tightRNN_tracktwoLLP_L1TAU100',  #llp
    'tau200_mediumRNN_tracktwoLLP_L1TAU100', #llp
    'tau200_tightRNN_tracktwoLLP_L1TAU100',  #llp
    'tau200_mediumRNN_tracktwoMVA_L1TAU100',
    'tau200_mediumRNN_tracktwoMVABDT_L1TAU100',
    'tau200_medium1_track_L1TAU100',
    'tau200_medium1_tracktwo_L1TAU100', # valid also in 2017 for 2 data reprocessing
    'tau200_medium1_tracktwoEF_L1TAU100',
    # Will keep this commented out for now
    #'tau80_mediumRNN_tracktwoMVA_tau60_mediumRNN_tracktwoMVA_L1TAU60_2TAU40',
    #'tau80_mediumRNN_tracktwoMVA_tau35_mediumRNN_tracktwoMVA_L1TAU60_DR-TAU20ITAU12I',
    #'tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1DR-TAU20ITAU12I-J25'
    #'HLT_tau80_mediumRNN_tracktwoLLP_tau60_mediumRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40',
    #'HLT_tau80_mediumRNN_tracktwoLLP_tau60_tightRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40',
    #'HLT_tau80_tightRNN_tracktwoLLP_tau60_tightRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40',
    #'HLT_tau100_mediumRNN_tracktwoLLP_tau80_mediumRNN_tracktwoLLP_03dRAB_L1TAU60_2TAU40',
    
    # Phase-1
    'tau25_mediumRNN_tracktwoMVABDT_L1eTAU12',
    'tau25_mediumRNN_tracktwoMVABDT_L1eTAU12M',
    'tau35_mediumRNN_tracktwoMVABDT_L1eTAU20',
    'tau160_mediumRNN_tracktwoMVABDT_L1eTAU100',
    'tau200_mediumRNN_tracktwoMVABDT_L1eTAU100',
    'tau180_mediumRNN_tracktwoLLP_L1eTAU100',

]

monitoring_tau_validation = [

        # tau0
        'tau0_ptonly_L1TAU8',
        'tau0_ptonly_L1TAU60',
        # tau25
        'tau25_ptonly_L1TAU12IM',
        'tau25_idperf_track_L1TAU12IM',
        'tau25_idperf_tracktwo_L1TAU12IM',
        'tau25_idperf_tracktwoEF_L1TAU12IM',
        'tau25_idperf_tracktwoMVA_L1TAU12IM',
        'tau25_idperf_tracktwoMVABDT_L1TAU12IM',
        'tau25_perf_track_L1TAU12IM',
        'tau25_perf_tracktwo_L1TAU12IM',
        'tau25_perf_tracktwoEF_L1TAU12IM',
        'tau25_perf_tracktwoMVA_L1TAU12IM',
        'tau25_perf_tracktwoMVABDT_L1TAU12IM',
        'tau25_looseRNN_tracktwoMVA_L1TAU12IM',
        'tau25_looseRNN_tracktwoMVABDT_L1TAU12IM',       
        'tau25_mediumRNN_tracktwoMVA_L1TAU12IM',
        'tau25_mediumRNN_tracktwoMVABDT_L1TAU12IM',
        'tau25_tightRNN_tracktwoMVA_L1TAU12IM',
        'tau25_tightRNN_tracktwoMVABDT_L1TAU12IM',
        'tau25_medium1_track_L1TAU12IM',
        'tau25_medium1_tracktwo_L1TAU12IM',
        'tau25_medium1_tracktwoEF_L1TAU12IM',
        # tau35
        'tau35_ptonly_L1TAU20IM',
        'tau35_idperf_track_L1TAU20IM',
        'tau35_idperf_tracktwo_L1TAU20IM',
        'tau35_idperf_tracktwoEF_L1TAU20IM',
        'tau35_idperf_tracktwoMVA_L1TAU20IM',
        'tau35_idperf_tracktwoMVABDT_L1TAU20IM',
        'tau35_perf_track_L1TAU20IM',
        'tau35_perf_tracktwo_L1TAU20IM',
        'tau35_perf_tracktwoEF_L1TAU20IM',
        'tau35_perf_tracktwoMVA_L1TAU20IM',
        'tau35_perf_tracktwoMVABDT_L1TAU20IM',
        'tau35_looseRNN_tracktwoMVA_L1TAU20IM',
        'tau35_looseRNN_tracktwoMVABDT_L1TAU20IM',
        'tau35_mediumRNN_tracktwoMVA_L1TAU20IM',
        'tau35_mediumRNN_tracktwoMVABDT_L1TAU20IM',
        'tau35_tightRNN_tracktwoMVA_L1TAU20IM',
        'tau35_tightRNN_tracktwoMVABDT_L1TAU20IM',
        # tau160
        'tau160_ptonly_L1TAU100',
        'tau160_idperf_track_L1TAU100',
        'tau160_idperf_tracktwo_L1TAU100',
        'tau160_idperf_tracktwoEF_L1TAU100',
        'tau160_idperf_tracktwoMVA_L1TAU100',
        'tau160_idperf_tracktwoMVABDT_L1TAU100',
        'tau160_perf_track_L1TAU100',
        'tau160_perf_tracktwo_L1TAU100',
        'tau160_perf_tracktwoEF_L1TAU100',
        'tau160_perf_tracktwoMVA_L1TAU100',
        'tau160_perf_tracktwoMVABDT_L1TAU100',
        'tau160_mediumRNN_tracktwoMVA_L1TAU100',
        'tau160_mediumRNN_tracktwoMVABDT_L1TAU100',
        'tau160_medium1_track_L1TAU100',
        'tau160_medium1_tracktwo_L1TAU100',
        'tau160_medium1_tracktwoEF_L1TAU100',
        # tau180
        'tau180_mediumRNN_tracktwoLLP_L1TAU100',
        'tau180_tightRNN_tracktwoLLP_L1TAU100',
        #tau200
        'tau200_mediumRNN_tracktwoLLP_L1TAU100',
        'tau200_tightRNN_tracktwoLLP_L1TAU100',
        # Phase I
        'tau25_mediumRNN_tracktwoMVABDT_L1eTAU12',
        'tau25_mediumRNN_tracktwoMVABDT_L1eTAU12M',
        'tau35_mediumRNN_tracktwoMVABDT_L1eTAU20',
        'tau160_mediumRNN_tracktwoMVABDT_L1eTAU100',
]

monitoring_tau_cosmic = [
    'tau1_cosmic_track_L1MU4_EMPTY',
    'tau1_cosmic_ptonly_L1MU4_EMPTY',
    'tau8_cosmic_ptonly',
    'tau8_cosmic_track'
]


monitoring_singleTau_cosmic = 'tau8_cosmic_track'
