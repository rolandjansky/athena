# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function


# Used for combination MC, collisions or cosmics


#General trigger chains  -  default values

monitoring_bjet      = ["E_HLT_j35_boffperf_split","E_HLT_j55_boffperf_split","S_HLT_j85_gsc110_boffperf_split","S_HLT_j45_gsc55_boffperf_split","E_HLT_j225_gsc300_boffperf_split",
                        "E_HLT_j45_subjesgscIS_ftf_boffperf_split_L1J20","E_HLT_j45_subjesgscIS_ftf_bmv2c1070_split_L1J20","E_HLT_j275_subjesgscIS_ftf_boffperf_split_L1J100",
                        "E_HLT_j275_subjesgscIS_ftf_bdl1r60_split_L1J100","E_HLT_j300_subjesgscIS_ftf_bdl1r70_split_L1J100",
                        "E_HLT_j360_subjesgscIS_ftf_bdl1r77_split_L1J100","E_HLT_j45_subjesgscIS_ftf_bdl1r70_split_L1J20",
                        "E_HLT_j275_pf_subjesgscIS_ftf_bdl1r60_split_L1J100","E_HLT_j275_pf_subjesgscIS_ftf_boffperf_split_L1J100",
                        "E_HLT_j300_pf_subjesgscIS_ftf_bdl1r70_split_L1J100","E_HLT_j360_pf_subjesgscIS_ftf_bdl1r77_split_L1J100","E_HLT_j45_pf_subjesgscIS_ftf_bdl1r70_split_L1J20",
                        "E_HLT_j45_pf_subjesgscIS_ftf_boffperf_split_L1J20"]

monitoring_mujet     = ["S_HLT_mu4_j45_gsc55_boffperf_split_dr05_dz02","E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02_L1BTAG-MU6J20","E_HLT_mu6_j110_gsc150_boffperf_split_dr05_dz02",
                        "E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02"]

# triggers in pp collisions   -  used for q431 test

monitoring_bjet_pp      = ["E_HLT_j35_boffperf_split","E_HLT_j55_boffperf_split","S_HLT_j85_gsc110_boffperf_split","S_HLT_j45_gsc55_boffperf_split","E_HLT_j225_gsc300_boffperf_split",
                           "E_HLT_j45_subjesgscIS_ftf_boffperf_split_L1J20","E_HLT_j45_subjesgscIS_ftf_bmv2c1070_split_L1J20","E_HLT_j275_subjesgscIS_ftf_boffperf_split_L1J100",
                           "E_HLT_j275_subjesgscIS_ftf_bdl1r60_split_L1J100","E_HLT_j300_subjesgscIS_ftf_bdl1r70_split_L1J100",
                           "E_HLT_j360_subjesgscIS_ftf_bdl1r77_split_L1J100","E_HLT_j45_subjesgscIS_ftf_bdl1r70_split_L1J20",
                           "E_HLT_j275_pf_subjesgscIS_ftf_bdl1r60_split_L1J100","E_HLT_j275_pf_subjesgscIS_ftf_boffperf_split_L1J100",
                           "E_HLT_j300_pf_subjesgscIS_ftf_bdl1r70_split_L1J100","E_HLT_j360_pf_subjesgscIS_ftf_bdl1r77_split_L1J100","E_HLT_j45_pf_subjesgscIS_ftf_bdl1r70_split_L1J20",
                           "E_HLT_j45_pf_subjesgscIS_ftf_boffperf_split_L1J20"]


monitoring_mujet_pp  = ["S_HLT_mu4_j45_gsc55_boffperf_split_dr05_dz02","E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02_L1BTAG-MU6J20","E_HLT_mu6_j110_gsc150_boffperf_split_dr05_dz02",
                        "E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02"]

################################

#  cosmics triggers

monitoring_bjet_cosmic = ["S_HLT_j0_perf_boffperf_L1RD0_EMPTY", "S_HLT_j0_perf_boffperf_L1J12_EMPTY", "S_HLT_j0_perf_boffperf_L1MU10"]


# triggers for HI runs    -  used for q314 test

monitoring_bjet_hi    = ["E_HLT_j40_boffperf_split", "S_HLT_j60_boffperf_split", "S_HLT_j100_boffperf_split"]


monitoring_mujet_hi    = ["E_HLT_mu4_j20_ion_dr05", "S_HLT_mu4_j60_ion_dr05", "S_HLT_mu6_j30_ion_dr05"]

# Triggers for MC  -  used for q221 test

monitoring_bjet_validation = ["E_HLT_j35_boffperf_split","E_HLT_j55_boffperf_split","S_HLT_j85_gsc110_boffperf_split","S_HLT_j45_gsc55_boffperf_split","E_HLT_j225_gsc300_boffperf_split",
                              "E_HLT_j45_subjesgscIS_ftf_boffperf_split_L1J20","E_HLT_j45_subjesgscIS_ftf_bmv2c1070_split_L1J20","E_HLT_j275_subjesgscIS_ftf_boffperf_split_L1J100",
                              "E_HLT_j275_subjesgscIS_ftf_bdl1r60_split_L1J100","E_HLT_j300_subjesgscIS_ftf_bdl1r70_split_L1J100",
                              "E_HLT_j360_subjesgscIS_ftf_bdl1r77_split_L1J100","E_HLT_j45_subjesgscIS_ftf_bdl1r70_split_L1J20",
                              "E_HLT_j275_pf_subjesgscIS_ftf_bdl1r60_split_L1J100","E_HLT_j275_pf_subjesgscIS_ftf_boffperf_split_L1J100",
                              "E_HLT_j300_pf_subjesgscIS_ftf_bdl1r70_split_L1J100","E_HLT_j360_pf_subjesgscIS_ftf_bdl1r77_split_L1J100","E_HLT_j45_pf_subjesgscIS_ftf_bdl1r70_split_L1J20",
                              "E_HLT_j45_pf_subjesgscIS_ftf_boffperf_split_L1J20"]



monitoring_mujet_validation = ["S_HLT_mu4_j45_gsc55_boffperf_split_dr05_dz02","E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02_L1BTAG-MU6J20","E_HLT_mu6_j110_gsc150_boffperf_split_dr05_dz02",
                               "E_HLT_mu6_j60_gsc85_boffperf_split_dr05_dz02"]

print ("In TrigBjetMonitCategory: monitoring_bjet")
print (monitoring_bjet)
