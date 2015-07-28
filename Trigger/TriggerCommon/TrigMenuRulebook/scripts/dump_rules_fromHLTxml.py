#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# This code reads a HLT xml file and create the rules with PS = -1 and rerun = 0
# The PS=1 primaries can be enabled from list_of_primaries
# You can specify special chains list_of_specials_cosmic
#
## run it that way:
## python dump_rules_fromHLTxml.py HLTMenu_125.xml Physics_pp_v4

import xml.etree.cElementTree as ET
import sys
from sys import argv
import string

if len(argv) < 3:
    print "Error, you need to provide an input hlt xml"
    print "--hltxmlname : fileName for the hlt xml"
    print "--tag        : tag"
    sys.exit()

inputhltfile = sys.argv[1] 
tag = sys.argv[2]

#
# All triggers will be set to PS=-1, rerun = 0 besides the special ones defined below
#

list_of_l1_cosmic = {
#    'L1_RD0_FILLED'        : { 'PS' :    5555 }, # 900 Hz at L1
#    'L1_RD0_EMPTY'         : { 'PS' :  100000 }, #  50 Hz at L1
#    'L1_RD0_UNPAIRED_ISO'  : { 'PS' :  100000 }, #  50 Hz at L1
#    'L1_RD1_EMPTY'         : { 'PS' : 5000000 }, #   1 Hz at L1
}

list_of_specials_cosmic = {
#
#    # ==========>
#    # Newly added on 10 Feb
#
#   'HLT_l1calocalib'                    : { 'PS' : 1 },
#   'HLT_l1calocalib_L1BGRP7'            : { 'PS' : 1 },
#   'HLT_larps_L1J12_FIRSTEMPTY'         : { 'PS' : 1 },
#   'HLT_larps_L1J30_FIRSTEMPTY'         : { 'PS' : 1 },
#
#    # MinBias
#   'HLT_mb_perf_L1LUCID'                : { 'PS' : 1 },
#   'HLT_mb_perf_L1LUCID_EMPTY'          : { 'PS' : 1 },
#   'HLT_mb_perf_L1LUCID_UNPAIRED_ISO'   : { 'PS' : 1 },
#   'HLT_mb_sptrk_L1RD0_EMPTY'           : { 'PS' : 10 },
#   'HLT_mb_sptrk_L1RD0_UNPAIRED_ISO'    : { 'PS' : 10 },
#   'HLT_noalg_mb_L1MBTS_2_UNPAIRED_ISO' : { 'PS' : 1 },
#   'HLT_noalg_mb_L1RD0_UNPAIRED_ISO'    : { 'PS' : 10 },
#   'HLT_noalg_mb_L1RD1_EMPTY'           : { 'PS' : 1 }, # !!! we need to define RD1_EMPTY !!!
#   'HLT_noalg_mb_L1RD1_FILLED'          : { 'PS' : 1 }, # !!! we need to define RD1_FILLED !!!
#
#    # streamers
#   'HLT_noalg_L1All'                    : { 'PS' : 1 },
#   'HLT_noalg_L1Calo_EMPTY'             : { 'PS' : 1 },
#   'HLT_noalg_L1MinBias'                : { 'PS' : 1 },
#   'HLT_noalg_L1Standby'                : { 'PS' : 1 },
#   'HLT_noalg_bkg_L1Bkg'                : { 'PS' : 1 },
#   'HLT_noalg_bkg_L1RD0_EMPTY'          : { 'PS' : 10 },
#   'HLT_noalg_bkg_L1RD0_UNPAIRED_ISO'   : { 'PS' : 10 },
#   'HLT_noalg_bkg_L1RD1_FILLED'         : { 'PS' : 1 }, #!!!! we need to define RD1_FILLED rate !!!
#   'HLT_noalg_dcmmon_L1RD0_EMPTY'       : { 'PS' : 10 },
#   'HLT_noalg_idmon_L1RD0_FILLED'       : { 'PS' : 180 },
#   'HLT_noalg_idmon_L1RD0_UNPAIRED_ISO' : { 'PS' : 10 },
#
#    # zero-bias
#   'HLT_noalg_zb_L1ZB'                  : { 'PS' : 1 },
#   'HLT_noalg_zb_L1ZB_J20'              : { 'PS' : 1 },
#
#    # Needs to check !!
#   'HLT_robrequest'               : { 'PS' : 1 },
#   'HLT_robrequest_L1RD0_EMPTY'   : { 'PS' : 1 },
#   'HLT_timeburner'               : { 'PS' : 1 },
#
#    # no EB in M8
#   'HLT_eb_empty_L1RD0_EMPTY'   : { 'PS' : -1 },
#   'HLT_noalg_eb_L1BGRP7'       : { 'PS' : -1 },
#   'HLT_noalg_eb_L1RD0_EMPTY'   : { 'PS' : -1 },
#
#    # end of new addition on 10 Feb
#    # <==========
#
#    # kn: we need to put alfacalib back
#    # 'HLT_alfa_alfacalib' : {'PS' : 500 },
#    'HLT_costmonitor'    : {'PS' : 1   },
#    
#    'HLT_id_cosmicid_L1MU4' : { 'PS' :1,   'ESValue' : 1  }, 
#    'HLT_id_cosmicid_L1MU11': { 'PS' :1,   'ESValue' : 1  }, 
#    'HLT_id_cosmicid'       : { 'PS' :1000,  'ESValue' : 0  }, # increased PS from 100 to 1000
#    'HLT_id_cosmicid_trtxk'           : {'PS' : 1000 }, # increased PS from 100 to 1000
#    'HLT_id_cosmicid_trtxk_central'   : {'PS' : 1000 }, # increased PS from 100 to 1000
#
#    'HLT_larcalib_L1EM3_EMPTY'        : {'PS' : 1 }, 
#    'HLT_larcalib_L1TAU8_EMPTY'       : {'PS' : 1 }, 
#    'HLT_larcalib_L1J12_EMPTY'        : {'PS' : 1 },
#    'HLT_larcalib_L1J3032ETA49_EMPTY' : {'PS' : 1 }, 
#    
#    'HLT_larps_L1EM3_EMPTY'           : {'PS' : 1 },
#    'HLT_larps_L1EM7_EMPTY'           : {'PS' : 1 },
#    'HLT_larps_L1J12_EMPTY'           : {'PS' : 1 },
#    'HLT_larps_L1J30_EMPTY'           : {'PS' : 1 }, 
#    'HLT_larps_L1TAU8_EMPTY'          : {'PS' : 1 },
#
#    'HLT_larhec_L1J12_FIRSTEMPTY'     : {'PS' : 1 }, # added
#
#    'HLT_ibllumi_L1RD0_EMPTY'         : {'PS' : 1 }, # added
#    'HLT_ibllumi_L1RD0_FILLED'        : {'PS' : 1 }, # added
#    'HLT_ibllumi_L1RD0_UNPAIRED_ISO'  : {'PS' : 1 }, # added
#
#    'HLT_larnoiseburst_L1All'             : {'PS' : 1000 },# added
#    'HLT_larnoiseburst_L1RD0_EMPTY'       : {'PS' : 10 }, # added
#
#    'HLT_tilelarcalib_L1EM3_EMPTY'        : {'PS' : 1 }, # added
#    'HLT_tilelarcalib_L1TAU8_EMPTY'       : {'PS' : 1 }, # added
#    'HLT_tilelarcalib_L1J12_EMPTY'        : {'PS' : 1 }, # added
#    'HLT_tilelarcalib_L1J3032ETA49_EMPTY' : {'PS' : 1 }, # added
#
#    # data scouting
#    'HLT_j0_perf_ds1_L1All'          : {'PS' : 1 }, # added
#
#    # Momentarly switched off
#    'HLT_mb_sptrk'                   : { 'PS' : -1 }, 
#    'HLT_mb_sptrk_costr'             : { 'PS' : -1 }, 
#    'HLT_mb_sptrk_costr_L1RD0_EMPTY' : { 'PS' : -1 },   
#    'HLT_mb_sptrk_noisesup'          : { 'PS' : -1 }, 
#
#    # cosmic muons
#    'HLT_mu4_cosmic_L1MU4_EMPTY'           : { 'PS' : 1 }, 
#    'HLT_mu4_cosmic_L1MU11_EMPTY'          : { 'PS' : 1 }, 
#    'HLT_mu4_msonly_cosmic_L1MU11_EMPTY'   : { 'PS' : 1 },
#
#    'HLT_mu4_cosmicEF_L1MU4_EMPTY'         : { 'PS' : 1 }, 
#    'HLT_mu4_cosmicEF_L1MU11_EMPTY'        : { 'PS' : 1 }, 
#    'HLT_mu4_msonly_cosmicEF_L1MU11_EMPTY' : { 'PS' : 1 },
#    'HLT_mu4_msonly_cosmicEF_L1MU4_EMPTY'  : { 'PS' : 1 },
#
#    #Streamers
#    'HLT_noalg_L1Calo'                        : { 'PS' : 1 },
#    'HLT_noalg_cosmiccalo_L1EM3_EMPTY'        : { 'PS' : 4   }, # added
#    'HLT_noalg_cosmiccalo_L1RD1_EMPTY'        : { 'PS' : 4   },
#    'HLT_noalg_cosmiccalo_L1J3032ETA49_EMPTY' : { 'PS' : 1 }, # added
#    'HLT_noalg_cosmiccalo_L1J12_EMPTY'        : { 'PS' : 1.  },
#    'HLT_noalg_cosmiccalo_L1J30_EMPTY'        : { 'PS' : 1 , 'ESValue' : 0  },
#    'HLT_noalg_cosmiccalo_L1J12_FIRSTEMPTY'   : { 'PS' : 1 , 'ESValue' : 50  },  # add ESvalue, J12 adjusted
#    'HLT_noalg_cosmiccalo_L1J30_FIRSTEMPTY'   : { 'PS' : 1 , 'ESValue' : 1   },  # add ESvalue
#    'HLT_noalg_cosmicmuons_L1MU11_EMPTY'      : { 'PS' : 1 , 'ESValue' : 1   },
#    'HLT_noalg_cosmicmuons_L1MU4_EMPTY'       : { 'PS' : 1 , 'ESValue' : 50  },
#
#    'HLT_noalg_idcosmic_L1TRT'                : { 'PS' : 1   },
#
#    'HLT_noalg_idmon_L1RD0_EMPTY'             : { 'PS' : 10 , 'ESValue' : 100  }, # increased PS 
#
#    # L1Topo streamers
#    'HLT_noalg_L14INVM9999-AJ0s6-AJ0s6'   : {'PS' : 1 }, # added
#    'HLT_noalg_L13INVM9999-AJ0s6-AJ0s6'   : {'PS' : 1 }, # added
#    'HLT_noalg_L12INVM9999-AJ0s6-AJ0s6'   : {'PS' : 1 }, # added
#    'HLT_noalg_L11INVM9999-AJ0s6-AJ0s6'   : {'PS' : 1 }, # added
#    'HLT_noalg_L1350INVM9999-J30s6-J20s6' : {'PS' : 1 }, # added
#    'HLT_noalg_L1300INVM9999-J30s6-J20s6' : {'PS' : 1 }, # added
#    'HLT_noalg_L1250INVM9999-J30s6-J20s6' : {'PS' : 1 }, # added
#    'HLT_noalg_L1200INVM9999-J30s6-J20s6' : {'PS' : 1 }, # added
#    'HLT_noalg_L1HT200-AJ20s5.ETA49'      : {'PS' : 1 }, # added
#    'HLT_noalg_L1HT190-AJ20s5.ETA49'      : {'PS' : 1 }, # added
#    'HLT_noalg_L1HT1-AJ0all.ETA49'        : {'PS' : 1 }, # added
#    'HLT_noalg_L11INVM5-EMs2-EMs'         : {'PS' : 1 }, # added
#    'HLT_noalg_L11INVM5-EM6s2-EMs'        : {'PS' : 1 }, # added
#    'HLT_noalg_L11INVM5-EM12s2-EMs'       : {'PS' : 1 }, # added
#    'HLT_noalg_L10DETA10-Js1-Js2'         : {'PS' : 1 }, # added
#
#    # CSC Mon chain
#    'HLT_cscmon_L1EM3'  : {'PS' : 1} ,
#    'HLT_cscmon_L1J12'  : {'PS' : 1} ,
#    'HLT_cscmon_L1MU10' : {'PS' : 1} ,
#    
#    # MinBias
#    'HLT_noalg_mb_L1MBTS_2'                : { 'PS' : 1 },
#    'HLT_noalg_mb_L1RD0_EMPTY'             : { 'PS' : 10  }, # increased PS
#    'HLT_noalg_mb_L1RD0_FILLED'            : { 'PS' : 180 }, # increased PS
#
#    # Detector Calibration Streams    
#    'HLT_pixel_beam'      : {'PS' : 180  },# increased PS
#    'HLT_pixel_noise'     : {'PS' : 10 },  # increased PS
#    'HLT_sct_noise'       : {'PS' : 10 },  # increased PS
#    'HLT_tilecalib_laser' : {'PS' : 1 },
#    'HLT_mu0_muoncalib'   : {'PS' : 1 },
#
#    ### HLT algorithms: Tau
#    'HLT_tau1_cosmic_ptonly_L1MU4_EMPTY': { 'PS' : 1  },
#    'HLT_tau1_cosmic_track_L1MU4_EMPTY' : { 'PS' : 1  },
#    'HLT_tau8_cosmic_ptonly'            : { 'PS' : 1  },
#    'HLT_tau8_cosmic_track'             : { 'PS' : 1  },                                       
#
#    ### HLT algorithms: Jet, b-jet
#    'HLT_j0_perf_L1RD0_EMPTY'         : {'PS' : 10 }, # added
#    'HLT_j0_perf_L1MU10'              : {'PS' : 1 },
#    'HLT_j0_perf_L1J12'               : {'PS' : 1 },
#    'HLT_ht0_perf_L1J12'              : {'PS' : 1 },
#    'HLT_j0_L1J12'                    : {'PS' : 1 }, # added
#    'HLT_j0_jes_L1J12'                : {'PS' : 1 }, # added
#    'HLT_j0_lcw_jes_L1J12'            : {'PS' : 1 }, # added
#    'HLT_j0_sub_L1J12'                : {'PS' : 1 }, # added
#    'HLT_j0_nojcalib_L1J12'           : {'PS' : 1 }, # added
#    'HLT_j0_PS_L1J12'                 : {'PS' : 1 }, # added
#    'HLT_j0_a10_nojcalib_L1J12'       : {'PS' : 1 }, # added
#    'HLT_j0_a10_lcw_sub_L1J12'        : {'PS' : 1 }, # added
#    'HLT_j0_a10r_lcw_sub_L1J12'       : {'PS' : 1 }, # added
#    'HLT_ht0_L1J12'                   : {'PS' : 1 }, # added
#    'HLT_j0_a10_nojcalib_L1RD0_EMPTY' : {'PS' : 10 }, # added
#    'HLT_j0_a10r_lcw_sub_L1RD0_EMPTY' : {'PS' : 10 }, # added
#    'HLT_j15_L1J12'                   : {'PS' : 1 }, # added
#
#    'HLT_j0_perf_bperf_L1RD0_EMPTY'   : {'PS' : 10 }, # added
#    'HLT_j0_perf_bperf_L1MU10'        : {'PS' : 1 },
#    'HLT_j0_perf_bperf_L1J12'         : {'PS' : 1 },
#
#    ### HLT algorithms: e/gamma
#    'HLT_g0_perf_L1EM3HLIL'        : {'PS' : 1 }, # added
#    'HLT_e0_perf_L1EM3HLIL'        : {'PS' : 1 }, # added
#    'HLT_e3_loose'                 : {'PS' : 1 }, # added
#    'HLT_e3_lhloose'               : {'PS' : 1 }, # added
#    'HLT_e3_etcut'                 : {'PS' : 1 }, # added
#    'HLT_e3_etcut_L1EM3_EMPTY'     : {'PS' : 1 }, # added
#    'HLT_g3_etcut'                 : {'PS' : 1 }, # added
#    'HLT_g3_etcut_L1EM3_EMPTY'     : {'PS' : 1 }, # added
#    'HLT_e0_perf_L1EM3'            : {'PS' : 1 }, # added
#    'HLT_e0_L2Star_perf_L1EM3'     : {'PS' : 1 }, # added
#    'HLT_g0_perf_L1EM3'            : {'PS' : 1 }, # added
#
#    ### HLT algorithms: Bphys
#    'HLT_2mu4_bDimu'               : {'PS' : 1 }, # added
#    'HLT_2mu4_bDimu_novtx_noos'    : {'PS' : 1 }, # added
#    'HLT_2mu6_bDimu'               : {'PS' : 1 }, # added
#    'HLT_2mu6_bDimu_novtx_noos'    : {'PS' : 1 }, # added
#
#    ### HLT algorithms: xe
#    'HLT_xe0_L1All'            : {'PS' : -1 },               # added
#    'HLT_xe0_L1XE35'           : {'PS' : 1 , 'rerun' : 1 },# added
#    'HLT_xe0_tc_lcw_L1All'     : {'PS' : -1 },               # added
#    'HLT_xe0_tc_lcw_L1XE35'    : {'PS' : 1, 'rerun' : 1 }, # added
#    'HLT_xe0_tc_em_L1All'      : {'PS' : -1 },               # added
#    'HLT_xe0_tc_em_L1XE35'     : {'PS' : 1, 'rerun' : 1 }, # added
#    'HLT_xe0_tc_pueta_L1All'   : {'PS' : -1 },               # added
#    'HLT_xe0_tc_pueta_L1XE35'  : {'PS' : 1, 'rerun' : 1 }, # added
#    'HLT_xe0_tc_pufit_L1All'   : {'PS' : -1 },               # added
#    'HLT_xe0_tc_pufit_L1XE35'  : {'PS' : 1, 'rerun' : 1 }, # added
#    'HLT_xe0_tc_mht_L1All'     : {'PS' : -1 },               # added
#    'HLT_xe0_tc_mht_L1XE35'    : {'PS' : 1, 'rerun' : 1 }, # added
#    'HLT_xe0_L2FS_L1All'       : {'PS' : -1 },               # added
#    'HLT_xe0_L2FS_L1XE35'      : {'PS' : 1, 'rerun' : 1 }, # added
#    'HLT_xe0_l2fsperf_wEFMuFEB_wEFMu_L1All'  : {'PS' : -1 }, # added
#    'HLT_xe0_l2fsperf_wEFMuFEB_wEFMu_L1XE35' : {'PS' : 1 },  # added
#    'HLT_xe35'                               : {'PS' : 1 },  # added
#    'HLT_xe35_tc_lcw'                        : {'PS' : 1 },  # added
#    'HLT_xe35_tc_em'                         : {'PS' : 1 },  # added
#    'HLT_xe35_pueta'                         : {'PS' : 1 },  # added
#    'HLT_xe35_pufit'                         : {'PS' : 1 },  # added
#    'HLT_xe35_mht'                           : {'PS' : 1 },  # added
#    'HLT_xe35_L2FS'                          : {'PS' : 1 },  # added
#    'HLT_xe35_l2fsperf_wEFMuFEB_wEFMu'       : {'PS' : 1 },  # added
#    'HLT_te20'                               : {'PS' : 1 },  # added
#    'HLT_te20_tc_lcw'                        : {'PS' : 1 },  # added
#    'HLT_xs30'                               : {'PS' : 1 },  # added
#    'HLT_xs30_tc_lcw'                        : {'PS' : 1 },  # added
#
#    ### HLT algorithms: minb
#    'HLT_mb_mbts_L1MBTS_2'    : {'PS' : 1 },
#    'HLT_mb_perf_L1MBTS_2'    : {'PS' : 1 }, # added
#
#    
#    ### beamspot chains
#    'HLT_beamspot_allTE_L2StarB_L1RDO_FILLED'    : {'PS' : 100},
#    # FTF version not to be used
#    'HLT_beamspot_allTE_trkfast_L1RDO_FILLED'   : { 'PS' : -1 },

    }

#
# old ones used at M7
#

list_of_l1_cosmic_M7 = {
#    'L1_RD0_FILLED' : { 'PS' : 1000000 },
#    'L1_RD0_EMPTY'  : { 'PS' : 1000000 },
#    'L1_RD1_EMPTY'  : { 'PS' : 1000000 },
}
list_of_specials_cosmic_M7 = {
#
#
#    'HLT_alfa_alfacalib' : {'PS' : 500 },
#    'HLT_costmonitor'    : {'PS' : 1   },
#    
#    'HLT_id_cosmic'               : {'PS': 100 },
#    'HLT_id_cosmic_trtxk'         : {'PS': 100 }, 
#    'HLT_id_cosmic_trtxk_central' : {'PS': 100 }, 
#    'HLT_id_cosmic_L1MU11': { 'PS' :1, 'ESValue' : 1  }, 
#    'HLT_id_cosmic_L1MU4' : { 'PS' :1, 'ESValue' : 1  }, 
#
#    'HLT_larcalib_L1EM3_EMPTY' : {'PS' : 1 }, 
#    'HLT_larcalib_L1J10_EMPTY' : {'PS' : 1 }, 
#    'HLT_larcalib_L1MU10'      : {'PS' : 1 },
#    'HLT_larcalib_L1TAU8_EMPTY': {'PS' : 1 }, 
#    
#    'HLT_larps_L1J10_EMPTY'          : { 'PS' : 1 }, 
#    'HLT_larps_L1J30_EMPTY'          : { 'PS' : 1 }, 
#    'HLT_larps_L1EM3_EMPTY'          : {'PS' : 1  },
#    'HLT_larps_L1EM6_EMPTY'          : {'PS' : 1  },
#    'HLT_larps_L1TAU8_EMPTY'         : {'PS' : 1  },
#
#    #Momentarly switched off, see ATR-
#    'HLT_mb_sptrk'                   : { 'PS' : -1   }, 
#    'HLT_mb_sptrk_costr'             : { 'PS' : -1   }, 
#    'HLT_mb_sptrk_costr_L1RD0_EMPTY' : { 'PS' : -1   },   
#    'HLT_mb_sptrk_noisesup'          : { 'PS' : -1 }, 
#
#    'HLT_mu4_cosmicEF_L1MU11_EMPTY'  : { 'PS' : 1 }, 
#    'HLT_mu4_cosmicEF_L1MU4_EMPTY'   : { 'PS' : 1 }, 
#    'HLT_mu4_cosmic_L1MU11_EMPTY'    : { 'PS' : 1 }, 
#    'HLT_mu4_cosmic_L1MU4_EMPTY'     : { 'PS' : 1 }, 
#    'HLT_mu4_msonly_cosmicEF_L1MU11_EMPTY' : { 'PS' : 1   },
#    'HLT_mu4_msonly_cosmicEF_L1MU4_EMPTY'  : { 'PS' : 1   },
#    'HLT_mu4_msonly_cosmic_L1MU11_EMPTY'   : { 'PS' : 1   },
#
#    #Streamers
#    'HLT_noalg_L1Calo'                     : { 'PS' : 1   },
#    'HLT_noalg_cosmiccalo_L1RD1_EMPTY'     : { 'PS' : 4   },
#    'HLT_noalg_cosmiccalo_L1J10_EMPTY'     : { 'PS' : 1.   },
#    'HLT_noalg_cosmiccalo_L1J30_EMPTY'     : { 'PS' : 1.   },
#    'HLT_noalg_cosmiccalo_L1J10_FIRSTEMPTY': { 'PS' : 1.   },
#    'HLT_noalg_cosmiccalo_L1J30_FIRSTEMPTY': { 'PS' : 1.   },
#    'HLT_noalg_cosmicmuons_L1MU11_EMPTY'   : { 'PS' : 1 , 'ESValue' : 1  },
#    'HLT_noalg_cosmicmuons_L1MU4_EMPTY'    : { 'PS' : 1 , 'ESValue' : 50  },
#
#    'HLT_noalg_idcosmic_L1TRT'             : { 'PS' : 1   },
#
#    'HLT_noalg_idmon_L1RD0_EMPTY'          : { 'PS' : 1 , 'ESValue' : 2  },
#
#    #CSC Mon chain
#    'HLT_cscmon_L1EM3'  : {'PS' : 1} ,
#    'HLT_cscmon_L1J10'  : {'PS' : 1} ,
#    'HLT_cscmon_L1MU10' : {'PS' : 1} ,
#    
#    #MinBias
#    'HLT_noalg_mb_L1MBTS_2'                : { 'PS' : 1 },
#    'HLT_noalg_mb_L1RD0_EMPTY'             : { 'PS' : 1   },
#    'HLT_noalg_mb_L1RD0_FILLED'            : { 'PS' : 1   },
#
#    #Detector Calibration Streams    
#    'HLT_pixel_beam'  : { 'PS' : 1 },
#    'HLT_pixel_noise' : { 'PS' : 1 },
#    'HLT_sct_noise'   : { 'PS' : 1 },
#    'HLT_tilecalib_laser' : { 'PS' : 1},
#    'HLT_mu0_muoncalib'   : {'PS' : 1},
#
#    ###HLT algorithms
#    'HLT_tau1_cosmic_ptonly_L1MU4_EMPTY': { 'PS' : 1  },
#    'HLT_tau1_cosmic_track_L1MU4_EMPTY' : { 'PS' : 1  },
#    'HLT_tau8_cosmic_ptonly'            : { 'PS' : 1  },
#    'HLT_tau8_cosmic_track'             : { 'PS' : 1  },                                       
#
#    'HLT_ht0_perf_L1J10'       : {'PS' : 1 },
#    'HLT_j0_perf_L1J10'        : {'PS' : 1 },
#    'HLT_j0_perf_L1MU10'       : {'PS' : 1 },
#    'HLT_j0_perf_bperf_L1J10'  : {'PS' : 1 },
#    'HLT_j0_perf_bperf_L1MU10' : {'PS' : 1 },
#    'HLT_e0_L2Star_perf_L1EM3' : {'PS' : 1 },
#    'HLT_e0_perf_L1EM3'        : {'PS' : 1 },
#    'HLT_g0_perf_L1EM3'        : {'PS' : 1 },
#
#    'HLT_xe0_L1All'        : {'PS' : 10 },
#
#    'HLT_mu0_muoncalib'    : {'PS' : 1 },
#
#    'HLT_mb_mbts_L1MBTS_2'    : {'PS' : 1 },
#
#
    }

#
# primary triggers, PS = 1
#
list_of_primaries = [
#        'HLT_id_cosmic_ds',
#        'HLT_costmonitor'
]

rulebook_filename = "%s_rules.py" %(tag)
rulebookfile = open(rulebook_filename, "w")
print "Writing out %s " % rulebook_filename

rulebookfile.writelines("from RuleClasses import TriggerRule\n\n")
rulebookfile.writelines("tag = %s%s%s\n\n" % ("'", tag, "'" ))
rulebookfile.writelines("EBrun=False\n\n")
rulebookfile.writelines("rules = {\n")

print 'Writing rulesbook file %s from HLT xml file  %s with tag %s' % (rulebook_filename, inputhltfile, tag)
HLT_XML = ET.parse(inputhltfile)
HLT_INFO = HLT_XML.getroot()

list_of_chains = []
list_of_streams = []

number_of_items = 0
for e in HLT_INFO.findall('CHAIN_LIST/CHAIN'):
    number_of_items+=1
    name    = e.get('chain_name')
    list_of_chains.append(name)
    stream = list(e.iter("STREAMTAG"))
    for i in stream:
       stream_name = i.get('stream')
    list_of_streams.append(stream_name)

# list = zip(list_of_streams, list_of_chains)
list = zip(list_of_chains, list_of_streams)
list.sort()


#
# L1 rule first
#
for chain in list_of_l1_cosmic:
    ps = list_of_l1_cosmic[chain]['PS']
    rerun = 0.
    chain_name = "'%s'" % chain
    rulebookfile.writelines("%60s : { 10000 : TriggerRule(PS=%.1f, comment = 'L1', rerun = %.1f) }, \n" % (chain_name, ps, rerun))

# for stream, chain in the hlt list: 
for chain, stream in list: 
    ps = -1.
    ps_es = 0.
    rerun = 0.
    if chain in list_of_primaries:
        ps = 1
    if chain in list_of_specials_cosmic:
        ps = list_of_specials_cosmic[chain]['PS']
        if 'ESValue' in list_of_specials_cosmic[chain]:
            ps_es = list_of_specials_cosmic[chain]['ESValue']

    chain_name = "'%s'" % chain
    if ps_es > 0. :
        rulebookfile.writelines("%60s : { 10000 : TriggerRule(PS=%.1f, comment = '%s', rerun = %.1f, ESValue = %.1f) }, \n" % (chain_name, ps, stream, rerun, ps_es))
    else :
        rulebookfile.writelines("%60s : { 10000 : TriggerRule(PS=%.1f, comment = '%s', rerun = %.1f) }, \n" % (chain_name, ps, stream, rerun))

rulebookfile.writelines("        }\n")
