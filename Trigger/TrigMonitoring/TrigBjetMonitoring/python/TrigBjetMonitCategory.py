# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# Used for combination MC, collisions or cosmics

monitoring_bjet      = ["HLT_j15_bperf", "HLT_j55_bperf", "HLT_j110_bperf", 
#                        "HLT_j0_perf_bperf_L1RD0_EMPTY", "HLT_j0_perf_bperf_L1J12_EMPTY"]
                        "HLT_j0_perf_bperf_L1RD0_EMPTY", "HLT_j0_perf_bperf_L1J12_EMPTY", "HLT_j0_perf_bperf_L1MU10"]


# Used for MC
#	monitoring_bjet      = ["HLT_j55_bperf"]


#  Used in cosmics
#	monitoring_bjet      = ["HLT_j0_perf_bperf_L1RD0_EMPTY"]
#       monitoring_bjet      = ["HLT_j0_perf_bperf_L1J12_EMPTY"]
#	monitoring_bjet      = ["HLT_j0_perf_bperf_L1MU10"]

#monitoring_mujet          = ["HLT_mu4_j55_match_bperf",
#                             "HLT_mu4_j15_bperf_split_dr05_dz02", "HLT_mu4_j55_bperf_split_dr05_dz02", "HLT_mu6_j110_bperf_split_dr05_dz02"]

monitoring_mujet          = ["HLT_mu4_j15_bperf_split_dr05_dz02", "HLT_mu4_j55_bperf_split_dr05_dz02", "HLT_mu6_j110_bperf_split_dr05_dz02",
                             "HLT_mu4_j25_bperf_split_dr05_dz02", "HLT_mu4_j35_bperf_split_dr05_dz02", "HLT_mu6_j85_bperf_split_dr05_dz02",
                             "HLT_mu6_j150_bperf_split_dr05_dz02", "HLT_mu6_j175_bperf_split_dr05_dz02", "HLT_mu6_j260_bperf_split_dr05_dz02",
                             "HLT_mu6_j320_bperf_split_dr05_dz02", "HLT_mu6_j400_bperf_split_dr05_dz02",
# adding HI triggers
                             "HLT_mu4_j20_ion_dr05", "HLT_mu4_j60_ion_dr05", "HLT_mu6_j30_ion_dr05"]


# Muon-jet parameters

EtThreshold               = 10.
PriVtxKey                 = "EFHistoPrmVtx"
