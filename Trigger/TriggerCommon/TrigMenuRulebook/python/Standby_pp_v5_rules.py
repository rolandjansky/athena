# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule


tag = 'Standby_pp_v5'

doALFA = False

NumBunches=110
mu4_unparediso_ps=NumBunches*0.1
if mu4_unparediso_ps < 1: mu4_unparediso_ps=1
print "Number of bucnhes: ", NumBunches

# SX check naming of L1 and HLT jet fwd items everywhere. Is this 'L1_J30.31ETA49' correct ?
# SX Remember : most important is that rate after L1 (not after HLT) is less than 500 Hz.
# SX find the definition of the HLT_*_ L1 seed for this under TriggerMenu/python/menu, either L1Seeds.py or other .py

rules = {

    #############################################################################################
    #
    #
    # Background triggers
    #
    #
    #############################################################################################

    #############################################################################################
    # Level-1

    # Triggers which where there in RUN 1 absent from the Run 2 L1_Bkg
    #    'L1_LUCID_A_C_EMPTY' : {    10000 : TriggerRule(PS=5000, comment='', rerun=0),    },
    #    'L1_LUCID_A_C_UNPAIRED_ISO' : {    10000 : TriggerRule(PS=900, comment='', rerun=0),    },
    #    'L1_LUCID_A_C_UNPAIRED_NONISO' : {    10000 : TriggerRule(PS=1500, comment='', rerun=0),    },
    #    'L1_LUCID_EMPTY' : {    10000 : TriggerRule(PS=400000, comment='', rerun=0),    },
    #    'L1_LUCID_UNPAIRED_ISO' : {    10000 : TriggerRule(PS=18000, comment='', rerun=0),    },
    #    'L1_LUCID_COMM_UNPAIRED_ISO' : {    10000 : TriggerRule(PS=900, comment='', rerun=0),    },
    #    'L1_LUCID_COMM_EMPTY' : {    10000 : TriggerRule(PS=900, comment='', rerun=0),    },
    #    'L1_BCM_AC_CA_UNPAIRED_ISO' : {    10000 : TriggerRule(PS=-1, comment='', rerun=0),    },
    #    'L1_FJ30_UNPAIRED_ISO' : {    10000 : TriggerRule(PS=22, comment='', rerun=0),    },


    # This was disabled in Run 1 standby menu. Enabling for now. 
    'L1_BCM_AC_CA_BGRP0'           : { 5000 : TriggerRule(PS=1000, comment='L1', rerun=0) },  # pay attention to this, last LB after SB declared and before we switch to physics goes up   
    'L1_BCM_Wide_EMPTY'            : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_BCM_Wide_UNPAIRED_ISO'     : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_BCM_Wide_UNPAIRED_NONISO'  : { 5000 : TriggerRule(PS=1000, comment='L1', rerun=0) }, 
    'L1_BCM_Wide_CALIB'            : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_BCM_Wide_ABORTGAPNOTCALIB' : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_BCM_AC_UNPAIRED_ISO'       : { 5000 : TriggerRule(PS=10, comment='L1', rerun=0) }, # in principle ISO rate low, but Mika claims ISO and NONISO should have the same and NONISO needs PS
    'L1_BCM_CA_UNPAIRED_ISO'       : { 5000 : TriggerRule(PS=10, comment='L1', rerun=0) },
    'L1_BCM_AC_UNPAIRED_NONISO'    : { 5000 : TriggerRule(PS=1000, comment='L1', rerun=0) },
    'L1_BCM_CA_UNPAIRED_NONISO'    : { 5000 : TriggerRule(PS=1000, comment='L1', rerun=0) },
    'L1_BCM_AC_ABORTGAPNOTCALIB'   : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_BCM_CA_ABORTGAPNOTCALIB'   : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_BCM_AC_CALIB'              : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_BCM_CA_CALIB'              : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },

    'L1_J30.31ETA49_UNPAIRED_NONISO'  : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J30.31ETA49_UNPAIRED_ISO'     : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J12_UNPAIRED_ISO'             : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J12_UNPAIRED_NONISO'          : { 5000 : TriggerRule(PS=100, comment='L1', rerun=0) },
    'L1_J12_ABORTGAPNOTCALIB'         : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) }, 


    'L1_MU4_UNPAIRED_ISO'     : { 5000 : TriggerRule(PS=mu4_unparediso_ps, comment='L1', rerun=0) },

    'L1_J50_UNPAIRED_ISO'     : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J50_UNPAIRED_NONISO'  : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J50_ABORTGAPNOTCALIB' : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },

    'L1_J12'         : { 10000 : TriggerRule(PS=25000, comment='L1', rerun=0) }, 
    'L1_J30.31ETA49' : { 10000 : TriggerRule(PS=12000, comment='L1', rerun=0) }, 

    #############################################################################################
    # HLT

    'HLT_noalg_bkg_L1Bkg'     : { 5000 : TriggerRule(PS=1, comment='Background', rerun=0) },
    'HLT_noalg_bkg_L1J12' : { 5000 : TriggerRule(PS=1, comment = 'Background', rerun = 0.0) },  
    'HLT_noalg_bkg_L1J30.31ETA49' : { 5000 : TriggerRule(PS=1, comment = 'Background', rerun = 0.0) }, 

    # SX comments
    # SC rate for background items should be ~ 100 Hz - source : Kunihiro
    # Flat Top with J12/FJ30 items (80Hz) and noalg_L1Bkg at 20Hz
    # put all PS at L!, because that is what matters for pixel and 500 Hz limit
    
    #############################################################################################
    #
    #
    # Monitoring + Collision
    #
    #
    #############################################################################################


    #############################################################################################
    # Level-1
    
    # Random triggers
    'L1_RD0_FILLED'        : { 5000 : TriggerRule(rate=10,   scaling="bunches", comment='L1', rerun=0) },
    'L1_RD0_UNPAIRED_ISO'  : { 5000 : TriggerRule(rate=5,    scaling="bunches", comment='L1', rerun=0) },
    'L1_RD0_EMPTY'         : { 5000 : TriggerRule(rate=10,   scaling="bunches", comment='L1', rerun=0) },
    'L1_RD1_EMPTY'         : { 5000 : TriggerRule(rate=5,    scaling="bunches", comment='L1', rerun=0) },
    'L1_RD2_EMPTY'        : { 5000 : TriggerRule(rate=10, scaling="bunches", comment='L1', rerun=0) },
    # total ~ 40 Hz
    
    'L1_CALREQ2'           : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },

    # This was at 1Hz in Run 1 - idcosmic - keep same Run2
    'L1_MU4_EMPTY'   : { 5000 : TriggerRule(PS=500, comment='L1', rerun=0) }, # 1 Hz just before physics data taking starts
    'L1_MU11_EMPTY'   : { 5000 : TriggerRule(PS=5, comment='L1', rerun=0) }, 

#    'L1_MU4'         : { 10000 : TriggerRule(PS=400, comment='L1', rerun=0) },
#    'L1_MU10'        : { 10000 : TriggerRule(PS=32, comment='L1', rerun=0) },

    # This was at 5Hz in Run 1 - L1 Standby input - keep same Run2
    'L1_EM7'         : { 5000 : TriggerRule(PS=700000, comment='L1', rerun=0) }, # unfortunately PS is driven by last 10 LB, after Stable Beam declared and before we move to physics menu, where rates goes up enormously and pixel cannot take more than 500 Hz. Same for other items here. Otheriwse PS could be much smaller, maybe PS=100 would do for all these.
    'L1_TAU8'        : { 5000 : TriggerRule(PS=700000, comment='L1', rerun=0) },
    'L1_J30'         : { 5000 : TriggerRule(PS=25000, comment='L1', rerun=0) },

    # these feed lar HLT streamers and L1Calo EMPTY. < 1 Hz each
    'L1_EM3_EMPTY'   : { 5000 : TriggerRule(PS=100, comment='L1', rerun=0) }, # to be revised, probably too high PS. Should be lumi independent.
    'L1_EM7_EMPTY'   : { 5000 : TriggerRule(PS=5, comment='L1', rerun=0) },
    'L1_J12_EMPTY'   : { 5000 : TriggerRule(PS=10, comment='L1', rerun=0) },
    'L1_J30_EMPTY'   : { 5000 : TriggerRule(PS=5, comment='L1', rerun=0) },
    'L1_TAU8_EMPTY'   : { 5000 : TriggerRule(PS=10, comment='L1', rerun=0) },
    'L1_J30.31ETA49_EMPTY'  : { 5000 : TriggerRule(PS=5, comment='L1', rerun=0) },

    #these end up in L1Standby and lar items. < 1 Hz each
    'L1_EM7_FIRSTEMPTY'   : { 5000 : TriggerRule(PS=10, comment='L1', rerun=0) }, # PS driven by last LB. Did not scale up for lumi as they should be lumi independent.
    'L1_TAU8_FIRSTEMPTY'   : { 5000 : TriggerRule(PS=10, comment='L1', rerun=0) },
    'L1_J12_FIRSTEMPTY'   : { 5000 : TriggerRule(PS=5000, comment='L1', rerun=0) },
    'L1_J30_FIRSTEMPTY'   : { 5000 : TriggerRule(PS=10, comment='L1', rerun=0) },

    # give ~ few Hz each, goes to L1 Standby and LAr items
    'L1_EM10VH'       : { 5000 : TriggerRule(PS=25000, comment='L1', rerun=0) }, # PS driven by last LB. scaled for lumi 5E33
    'L1_EM15'         : { 5000 : TriggerRule(PS=7000, comment='L1', rerun=0) }, # this gives 10 Hz at 5E33. feeds larpeb_EM15, which then rjeects 20%.

    # This was at 10Hz in Run 1, this ends up in L1Standby and Lar items (where there is also HLT rejection)
    'L1_EM20VHI'                : { 5000 : TriggerRule(PS=500, comment='L1', rerun=0) }, # feeds g40/60/80_larpeb HLT. rate 20 Hz at 5E33. with 20% HLT reject, gives 4 Hz

#   give ~ 25Hz  here, feed L1Standby
    'L1_EM22VHI'   : { 5000 : TriggerRule(PS=1000, comment='L1', rerun=0) }, # should get 8 Hz at 5E33
    'L1_J75'   : { 5000 : TriggerRule(PS=100, comment='L1', rerun=0) },  # should get 30 Hz at 5E33. larpbej items reduce that by ~ 1/1000 (of course L1Standby remains big)
    'L1_J75.31ETA49'  : { 5000 : TriggerRule(PS=1500, comment='L1', rerun=0) }, # should get 20 Hz at 5E33. larpbej items reduce that by ~ 1/1000 (of course L1Standby remains big)

    
    
    #############################################################################################
    # HLT

    'HLT_noalg_L1Calo'                        : { 5000 : TriggerRule(PS=-1, comment='L1Calo', rerun=0) },    

    'HLT_noalg_L1Standby'                     : { 5000 : TriggerRule(PS=10, comment='Standby', rerun=0) },

    # SX comment : L1Standby is for detector monitoring, all items mentioned here and not labelled EMPTY,RD,ZB,CALREQ,ALFA end up here. For now we miss basically only MU items to get some muon detector monitoring, is this ok?
    # SX right now, due to L1Bkg J12 and FJ30, L1Standby gets quite some rate (80 Hz), overlapping with L1Bkg stream. Is this a problem? else, should we remove L1J12 and L1FJ30 from L1Standby?
    
    #
    'HLT_noalg_idmon_L1RD0_EMPTY'             : { 5000 : TriggerRule(PS=1, ESValue=0, comment='IDMonitoring express', rerun=0) },
    'HLT_noalg_idmon_L1RD0_FILLED'            : { 5000 : TriggerRule(inputRate=5.0, comment='IDMonitoring', rerun=0) },
    'HLT_noalg_idmon_L1RD0_UNPAIRED_ISO'      : { 5000 : TriggerRule(PS=1, comment='IDMonitoring', rerun=0) }, 


    #Cosmic Streamers
    'HLT_id_cosmicid_L1MU4_EMPTY'             : { 5000 : TriggerRule(PS=1, ESValue=0, comment='express', rerun=0) },
    'HLT_id_cosmicid_L1MU11_EMPTY'            : { 5000 : TriggerRule(PS=1, ESValue=0, comment='express', rerun=0) },

    #Calibration/Monitoring
    #
    'HLT_larps_L1EM7_EMPTY'                   : { 5000 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0) },
    'HLT_larps_L1J12_EMPTY'                   : { 5000 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0) },
    'HLT_larps_L1J30_EMPTY'                   : { 5000 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0) },
    'HLT_larps_L1TAU8_EMPTY'                  : { 5000 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0) },

    #

    'HLT_larps_L1EM7_FIRSTEMPTY'              : { 5000 : TriggerRule(PS=1 , comment='LArCellsEmpty', rerun=0) }, 
    'HLT_larps_L1J12_FIRSTEMPTY'              : { 5000 : TriggerRule(PS=1, comment = 'LArCellsEmpty', rerun=0) }, 
    'HLT_larps_L1J30_FIRSTEMPTY'              : { 5000 : TriggerRule(PS=1, comment = 'LArCellsEmpty', rerun=0) }, 

    # 

    'HLT_g40_loose_larpeb' : { 5000 : TriggerRule(PS=1.0, comment = 'LArCells', rerun = 0.0) },
    'HLT_g60_loose_larpeb' : { 5000 : TriggerRule(PS=1.0, comment = 'LArCells', rerun = 0.0) },
    'HLT_g80_loose_larpeb' : { 5000 : TriggerRule(PS=1.0, comment = 'LArCells', rerun = 0.0) },
    'HLT_g12_loose_larpeb' : { 5000 : TriggerRule(PS=1, comment='LArCells', rerun=0) }, 
    'HLT_g20_loose_larpeb_L1EM15'   : { 5000 : TriggerRule(PS=1, comment='LArCells', rerun=0) },


    #    'HLT_conej40_larpebj'           : { 5000 : TriggerRule(PS=1, comment= "jet monitoring" ,rerun=0) }, # feed from L1J12, if HLT provides some rejection then ok to keep ?
    'HLT_conej165_larpebj'          :   { 5000 : TriggerRule(PS=1, comment= "jet monitoring" ,rerun=0) }, 
    'HLT_conej75_320eta490_larpebj' : { 5000 : TriggerRule(PS=1, comment= "jet monitoring" ,rerun=0) }, 
    'HLT_conej140_320eta490_larpebj':  { 5000 : TriggerRule(PS=1, comment= "jet monitoring" ,rerun=0) },

    #
    'HLT_tilecalib_laser'                     : { 5000 : TriggerRule(PS=1, comment='Tile', rerun=0) },


    'HLT_larcalib_L1EM3_EMPTY'                : { 5000 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0) },
    'HLT_larcalib_L1TAU8_EMPTY'               : { 5000 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0) },
    'HLT_larcalib_L1J12_EMPTY'                : { 5000 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0) },
    'HLT_larcalib_L1J3031ETA49_EMPTY'         : { 5000 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0) },
    'HLT_larcalib_L1EM7_EMPTY'                : { 10000 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0) }, 

    # larhec
    'HLT_larhec_L1J12_FIRSTEMPTY'             : { 5000 : TriggerRule(PS=1, comment = 'LArCellsEmpty', rerun = 0) },
    'HLT_larhec_L1J30_FIRSTEMPTY'             : { 10000 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0) }, 
    'HLT_larhec_L1EM7_FIRSTEMPTY'             : { 5000 : TriggerRule(PS=1, comment = 'LArCellsEmpty', rerun = 0) },
    'HLT_larhec_L1TAU8_FIRSTEMPTY'            : { 5000 : TriggerRule(PS=1, comment = 'LArCellsEmpty', rerun = 0) },

    ### EM chain was not in the original list, go to Express stream 
    'HLT_noalg_cosmiccalo_L1EM3_EMPTY' : { 5000 : TriggerRule(PS=1.0, comment = 'CosmicCalo', rerun = 0.0) }, 
    'HLT_noalg_cosmiccalo_L1J12_EMPTY' : { 5000 : TriggerRule(PS=1.0, ESValue=0, comment = 'CosmicCalo express', rerun = 0.0) }, 
    'HLT_noalg_cosmiccalo_L1J30_EMPTY' : { 5000 : TriggerRule(PS=1.0, ESValue=0, comment = 'CosmicCalo express', rerun = 0.0) }, 
    'HLT_noalg_cosmiccalo_L1J3031ETA49_EMPTY' : { 5000 : TriggerRule(PS=1.0, comment = 'CosmicCalo', rerun = 0.0) }, 

    'HLT_noalg_cosmiccalo_L1RD1_EMPTY'        : { 5000 : TriggerRule(rate=1.0, ESValue=0, comment = 'CosmicCalo express', rerun=0) },

    'HLT_larnoiseburst_rerun'                 : { 5000 : TriggerRule(PS=0,   comment = 'LArNoiseBurst', rerun=True) }, 
    # prescale out XE items as rate too herratic, dangerous for pixel
#    'HLT_larnoiseburst_L1XE35'                : { 10000 : TriggerRule(PS=1,   comment = 'LArNoiseBurst', rerun=0) }, 
#    'HLT_larnoiseburst_L1XE50'                : { 10000 : TriggerRule(PS=1,   comment = 'LArNoiseBurst', rerun=0) },
#    'HLT_larnoiseburst_L1J50'                 : { 10000 : TriggerRule(PS=1,   comment = 'LArNoiseBurst', rerun=0) },
#    'HLT_larnoiseburst_L1J75'                 : { 10000 : TriggerRule(PS=1,   comment = 'LArNoiseBurst', rerun=0) },

#    'HLT_larnoiseburst_L1J75_XE40'            : { 10000 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0) },
#    'HLT_larnoiseburst_L1J40_XE50'            : { 10000 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0) },
#    'HLT_larnoiseburst_L1J100'                : {   1 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0) },
#    'HLT_larnoiseburst_L1XE80'                : {   1 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0) },

    # monitorings
    'HLT_timeburner'                          : { 5000 : TriggerRule(PS=-1, comment = 'DISCARD', rerun=0) }, 
    'HLT_noalg_dcmmon_L1RD0_EMPTY'            : { 5000 : TriggerRule(rate=10.0, comment = 'monitoring_random', rerun=0) }, 
    #'HLT_noalg_dcmmon_L1RD2_EMPTY'            : { 10000 : TriggerRule(rate=10000.0, comment = 'monitoring_random', rerun = 0) },
    'HLT_robrequest'                          : { 5000 : TriggerRule(PS=-1, comment = 'DISCARD', rerun=0) }, 
    'HLT_robrequest_L1RD0_EMPTY'              : { 5000 : TriggerRule(PS=-1, comment = 'DISCARD', rerun=0) }, 


    # TGC NoiseBurst
    'HLT_noalg_L1TGC_BURST'                   : {  1 : TriggerRule(PS=1, comment='TGCNoiseBurst', rerun=0),


    # no pixel / sct / ibl 
    # CSC mon
    'HLT_cscmon_L1All'                        : { 5000 : TriggerRule(PS=100.0, comment = 'CSC', rerun = 0.0) }, 
    

    }

#################################################################################################
#
#
# alfa rules for special ALFA beam based alignment fill with beta=19 m ATR-11364
#
#
#################################################################################################

alfa_phys_Rules = {

    # Level-1 ALFA should be limited to 5kHz
    # CENTRAL DIFF (ALFA_CEP)
    'L1_ALFA_ANTI_ELASTIC_UNPAIRED_ISO'                       : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ELAST15'                                         : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ELAST18'                                         : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ELASTIC_UNPAIRED_ISO'                            : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_SYST17'                                          : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_SYST18'                                          : {   10000 : TriggerRule(PS=1, comment='') },
    # MBTS
    'L1_MBTS_1'                                               : {   10000 : TriggerRule(PS=100, comment='') }, 
    'L1_MBTS_2'                                               : {   10000 : TriggerRule(PS=100, comment='') },
    
    ########################################
    # HLT
    # ALFA streamers
    'HLT_noalg_L1ALFA_Phys'                                   : {   10000 : TriggerRule(PS=1, comment='') },
    'HLT_noalg_L1ALFA_SYS'                                    : {   10000 : TriggerRule(PS=1, comment='') },
    'HLT_noalg_L1ALFA_PhysAny'                                : {   10000 : TriggerRule(PS=10000, comment='') },
    # MBTS streamers
    'HLT_noalg_mb_L1MBTS_1'                                   : {   10000 : TriggerRule(PS=1, comment='L1PS=100') }, 
    'HLT_noalg_mb_L1MBTS_2'                                   : {   10000 : TriggerRule(PS=1, comment='L1PS=100') }, 
    
    # SM request
    # central diff (full r/o)  # SM request
    'HLT_mb_sptrk_vetombts2in_L1ALFA_CEP'                     : {   10000 : TriggerRule(PS=200, comment='') }, 
    # trigger/detectorcommissioning : PEB (IDFwd)
    'HLT_mb_sptrk_vetombts2in_peb_L1ALFA_ANY'                 : {   10000 : TriggerRule(PS=200, comment='') },
    }

if doALFA:
    rules.update(alfa_phys_Rules)


alfa_calib_Rules = {

    ########################################
    # Level-1
    'L1_ALFA_ANY'                                             : {   10000 : TriggerRule(PS=1, comment='') }, 

    # "ALFA_Calib"
    'L1_ALFA_A7L1L'                                           : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7L1L_OD'                                        : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7L1U'                                           : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7L1U_OD'                                        : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7L1_OD'                                         : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_A7R1L'                                           : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7R1L_OD'                                        : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7R1U'                                           : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7R1U_OD'                                        : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7R1_OD'                                         : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ANY_ABORTGAPNOTCALIB'                            : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ANY_A_EMPTY'                                     : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ANY_BGRP10'                                      : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ANY_CALIB'                                       : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ANY_C_EMPTY'                                     : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ANY_EMPTY'                                       : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ANY_FIRSTEMPTY'                                  : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ANY_UNPAIRED_ISO'                                : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ANY_UNPAIRED_NONISO'                             : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_B7L1L'                                           : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7L1L_OD'                                        : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7L1U'                                           : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7L1U_OD'                                        : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7L1_OD'                                         : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_B7R1L'                                           : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7R1L_OD'                                        : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7R1U'                                           : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7R1U_OD'                                        : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7R1_OD'                                         : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ELAST15_Calib'                                   : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ELAST18_Calib'                                   : {   10000 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_SHOWSYST5'                                       : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_BGRP10_ALFA_BGT'                                      : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_BGRP1_ALFA_BGT'                                       : {   10000 : TriggerRule(PS=1000, comment='') },
    'L1_BGRP4_ALFA_BGT'                                       : {   10000 : TriggerRule(PS=-1, comment='') },

    # "ALFA_SYS"  
    'L1_ALFA_SYST9'                                           : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_SYST10'                                          : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_SYST11'                                          : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_SYST12'                                          : {   10000 : TriggerRule(PS=-1, comment='') },
    # SYST17/18 used in ALFA_CEP
    #'L1_ALFA_SYST17'                                         : {   10000 : TriggerRule(PS=1, comment='') },
    #'L1_ALFA_SYST18'                                         : {   10000 : TriggerRule(PS=1, comment='') },

    # "ALFA_ELAS"
    'L1_ALFA_ELAST1'                                          : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST2'                                          : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST11'                                         : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST12'                                         : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST13'                                         : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST14'                                         : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST16'                                         : {   10000 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST17'                                         : {   10000 : TriggerRule(PS=-1, comment='') },
    # ELAST15/18 used in ALFA_CEP
    #'L1_ALFA_ELAST15'                                        : {   10000 : TriggerRule(PS=1, comment='') },
    #'L1_ALFA_ELAST18'                                        : {   10000 : TriggerRule(PS=1, comment='') },

    ########################################
    # PEB streamers
    'HLT_alfacalib'                                           : {   10000 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_ANY'                                : {   10000 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_ELAST15'                            : {   10000 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_ELAST18'                            : {   10000 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_A7L1_OD'                            : {   10000 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_B7L1_OD'                            : {   10000 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_A7R1_OD'                            : {   10000 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_B7R1_OD'                            : {   10000 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_SYS'                                : {   10000 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_ELAS'                               : {   10000 : TriggerRule(PS=1, comment='') },

    }

if doALFA:
    rules.update(alfa_calib_Rules)

