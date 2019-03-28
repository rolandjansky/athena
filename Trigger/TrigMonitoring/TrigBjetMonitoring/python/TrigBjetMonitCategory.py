# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TriggerJobOpts.TriggerFlags import TriggerFlags

###########################

# FTK chains used in combination MC, collisions or cosmics

# v7 + v8 chains
monitoring_bjet_FTK = ["S_HLT_j35_boffperf_split_FTK_L1J15_FTK", "E_HLT_j35_boffperf_split_FTKVtx_L1J15_FTK", "E_HLT_j35_boffperf_split_FTKRefit_L1J15_FTK",
                       "E_HLT_j150_boffperf_split_j50_boffperf_split_FTK_L1J100_FTK", "E_HLT_j150_boffperf_split_j50_boffperf_split_FTKVtx_L1J100_FTK",
                       "E_HLT_j150_boffperf_split_j50_boffperf_split_FTKRefit_L1J100_FTK"]

############################

#General trigger chains  -  default values

# v7 chains (added the L1 seed) + v8 chain
monitoring_bjet      = ["E_HLT_j35_boffperf_split_L1J15", "E_HLT_j55_boffperf_split_L1J20", "S_HLT_j85_gsc110_boffperf_split_L1J20", 
                        "S_HLT_j45_gsc55_boffperf_split_L1J15", "E_HLT_j225_gsc300_boffperf_split_L1J100",
                        "E_HLT_j150_boffperf_split_j50_boffperf_split_L1J100",
                        "E_HLT_j35_boffperf_split", "E_HLT_j55_boffperf_split", "S_HLT_j85_gsc110_boffperf_split",
                        "S_HLT_j45_gsc55_boffperf_split", "E_HLT_j225_gsc300_boffperf_split"]
#if TriggerFlags.doFTK():
monitoring_bjet += monitoring_bjet_FTK
# v7 chains (added the L1 seed)
monitoring_mujet     = ["S_HLT_mu4_j45_gsc55_boffperf_split_dr05_dz02_L1MU4", "E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02_L1BTAG-MU6J20", 
                        "E_HLT_mu6_j110_gsc150_boffperf_split_dr05_dz02_L1MU6_J40", "E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02_L1MU6_J20",
                        "S_HLT_mu4_j45_gsc55_boffperf_split_dr05_dz02","E_HLT_mu6_j110_gsc150_boffperf_split_dr05_dz02", "E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02"]

##############################

# triggers in pp collisions   -  used for q431 test

# v7 chains (added the L1 seed) + v8 chain
monitoring_bjet_pp      = ["E_HLT_j35_boffperf_split_L1J15", "E_HLT_j55_boffperf_split_L1J20", "S_HLT_j85_gsc110_boffperf_split_L1J20",
                           "S_HLT_j45_gsc55_boffperf_split_L1J15", "E_HLT_j225_gsc300_boffperf_split_L1J100",
                           "E_HLT_j150_boffperf_split_j50_boffperf_split_L1J100",
                           "E_HLT_j35_boffperf_split", "E_HLT_j55_boffperf_split", "S_HLT_j85_gsc110_boffperf_split",
                           "S_HLT_j45_gsc55_boffperf_split", "E_HLT_j225_gsc300_boffperf_split"]
#if TriggerFlags.doFTK():
monitoring_bjet_pp += monitoring_bjet_FTK
# v7 chains (added the L1 seed)
monitoring_mujet_pp     = ["S_HLT_mu4_j45_gsc55_boffperf_split_dr05_dz02_L1MU4", "E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02_L1BTAG-MU6J20", 
                           "E_HLT_mu6_j110_gsc150_boffperf_split_dr05_dz02_L1MU6_J40", "E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02_L1MU6_J20",
                           "S_HLT_mu4_j45_gsc55_boffperf_split_dr05_dz02","E_HLT_mu6_j110_gsc150_boffperf_split_dr05_dz02", "E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02"]

################################

#  cosmics triggers

monitoring_bjet_cosmic = ["S_HLT_j0_perf_boffperf_L1RD0_EMPTY", "S_HLT_j0_perf_boffperf_L1J12_EMPTY", "S_HLT_j0_perf_boffperf_L1MU10"]

################################

# triggers for HI runs    -  used for q314 test

# L1 seed not (yet) added
monitoring_bjet_hi    = ["E_HLT_j40_boffperf_split", "S_HLT_j60_boffperf_split", "S_HLT_j100_boffperf_split"]
#if TriggerFlags.doFTK():
monitoring_bjet_hi += monitoring_bjet_FTK
# L1 seed not (yet) added 
monitoring_mujet_hi    = ["E_HLT_mu4_j20_ion_dr05", "S_HLT_mu4_j60_ion_dr05", "S_HLT_mu6_j30_ion_dr05"]

#################################

# Triggers for MC  -  used for q221 test

# v7 chains (added the L1 seed) + v8 chain
monitoring_bjet_validation      = ["E_HLT_j35_boffperf_split_L1J15", "E_HLT_j55_boffperf_split_L1J20", "S_HLT_j85_gsc110_boffperf_split_L1J20", 
                                   "S_HLT_j45_gsc55_boffperf_split_L1J15", "E_HLT_j225_gsc300_boffperf_split_L1J100",
                                   "E_HLT_j150_boffperf_split_j50_boffperf_split_L1J100",
                                   "E_HLT_j35_boffperf_split", "E_HLT_j55_boffperf_split", "S_HLT_j85_gsc110_boffperf_split",
                                   "S_HLT_j45_gsc55_boffperf_split", "E_HLT_j225_gsc300_boffperf_split"]
#if TriggerFlags.doFTK():
monitoring_bjet_validation += monitoring_bjet_FTK
# v7 chains (added the L1 seed)
monitoring_mujet_validation     = ["S_HLT_mu4_j45_gsc55_boffperf_split_dr05_dz02_L1MU4", "E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02_L1BTAG-MU6J20", 
                                   "E_HLT_mu6_j110_gsc150_boffperf_split_dr05_dz02_L1MU6_J40", "E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02_L1MU6_J20",
                                   "S_HLT_mu4_j45_gsc55_boffperf_split_dr05_dz02","E_HLT_mu6_j110_gsc150_boffperf_split_dr05_dz02", "E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02"]

###################################
