# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# Used for combination MC, collisions or cosmics


#General trigger chains

monitoring_bjet      = ["HLT_j35_boffperf_split", "HLT_j55_boffperf", "HLT_j55_boffperf_split", "HLT_j320_boffperf", "HLT_j320_boffperf_split", "HLT_j225_gsc360_boffperf_split"]

monitoring_mujet     = ["HLT_mu4_j55_bperf_split_dr05_dz02", "HLT_mu4_j55_bperf_split_dr05_dz02_L1BTAG-MU4J15", "HLT_mu6_j110_bperf_split_dr05_dz02"]

# triggers in pp collisions

monitoring_bjet_pp    = ["HLT_j35_boffperf_split", "HLT_j55_boffperf", "HLT_j55_boffperf_split", "HLT_j320_boffperf", "HLT_j320_boffperf_split", "HLT_j225_gsc360_boffperf_split"]

monitoring_mujet_pp   = ["HLT_mu4_j55_bperf_split_dr05_dz02", "HLT_mu4_j55_bperf_split_dr05_dz02_L1BTAG-MU4J15", "HLT_mu6_j110_bperf_split_dr05_dz02"]

################################

#  cosmics triggers

monitoring_bjet_cosmic = ["HLT_j0_perf_bperf_L1RD0_EMPTY", "HLT_j0_perf_bperf_L1J12_EMPTY", "HLT_j0_perf_bperf_L1MU10"]


# triggers for HI runs

monitoring_mujet_hi    = ["HLT_mu4_j20_ion_dr05", "HLT_mu4_j60_ion_dr05", "HLT_mu6_j30_ion_dr05"]

# Used for MC

monitoring_bjet_validation = ["HLT_j55_bperf"] 
