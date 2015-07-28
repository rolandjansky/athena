# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Monitoring_pp_v5'


#################################################################################################
#
#
# detector calibration streams
#
#
#################################################################################################


########################################
# ID Tracks: 100 Hz total (ATR-10954)

stream_IDTracks_Rules = {
    # TAU8 ~ 100 Hz with 1b, PS=2 for 2b. Keep an eye on the rate.
    'HLT_idcalib_trk9_central'           : {  1 : TriggerRule(PS=1, comment='IDTracks', rerun=0) },
    'HLT_idcalib_trk9_fwd'               : {  1 : TriggerRule(PS=1, comment='IDTracks', rerun=0) },
    'HLT_idcalib_trk16_central'          : {  1 : TriggerRule(PS=1, comment='IDTracks', rerun=0) },
    'HLT_idcalib_trk16_fwd'              : {  1 : TriggerRule(PS=1, comment='IDTracks', rerun=0) },
    'HLT_idcalib_trk29_central'          : {  1 : TriggerRule(PS=1, comment='IDTracks', rerun=0) },
    'HLT_idcalib_trk29_fwd'              : {  1 : TriggerRule(PS=1, comment='IDTracks', rerun=0) },
    }

########################################
# Pixel Beam: 1000 Hz in 50ns run (500 Hz later in 25ns run)

stream_PixelBeam_Rules = { 
    'HLT_lumipeb_L1RD0_FILLED'           : {  1 : TriggerRule(PS=-1, comment='PixelBeam disabled in cosmic',  rerun=0),
                                             10 : TriggerRule(inputRate=900, comment='PixelBeam',  rerun=0) },
    'HLT_lumipeb_L1RD0_UNPAIRED_ISO'     : {  1 : TriggerRule(PS=-1, comment='PixelBeam disabled in cosmic',  rerun=0),
    				             10 : TriggerRule(inputRate=60,  comment='PixelBeam',  rerun=0) },
    'HLT_lumipeb_L1RD0_ABORTGAPNOTCALIB' : {  1 : TriggerRule(PS=-1, comment='PixelBeam disabled in cosmic',  rerun=0),
                                             10 : TriggerRule(inputRate=40,  comment='PixelBeam',  rerun=0) },
}

########################################
# Pixel Noise: 10 Hz

stream_PixelNoise_Rules = {
    'HLT_lumipeb_L1RD0_EMPTY'            : {  1 : TriggerRule(inputRate=10, comment='PixelNoise', rerun=0) }, 
    }

########################################
# SCT Noise: 10 Hz

stream_SCTNoise_Rules = {
    'HLT_sct_noise'                      : {  1 : TriggerRule(inputRate=10, scaling="bunches", comment='SCTNoise', rerun=0) },
    }

########################################
# Muon calibration

stream_MuonCalibration_Rules = {
    # MU4_EMPTY: cosmic
    'HLT_mu0_muoncalib_L1MU4_EMPTY'      : {  1 : TriggerRule(PS=1, comment='Muon_Calibration', rerun=0),
                                             10 : TriggerRule(PS=-1, comment='disabled in collision', rerun=0) },
    # MU15: beam
    'HLT_mu0_muoncalib_L1MU15'           : {  1 : TriggerRule(PS=-1, comment='disabled in cosmic', rerun=0),
                                             10 : TriggerRule(PS=1, comment='Muon_Calibration', rerun=0),
                                          10001 : TriggerRule(PS=-1, comment='MU15 is disabled', rerun=0) },
    # MU20: beam
    'HLT_mu0_muoncalib'                  : {  1 : TriggerRule(PS=-1, comment='disabled in cosmic', rerun=0),
                                             10 : TriggerRule(PS=1, comment='Muon_Calibration', rerun=0) },
    #
    # data scouting versions
    #
    'HLT_mu0_muoncalib_ds3'             : {  1 : TriggerRule(PS=-1, comment='disabled for now', rerun=0) },
    'HLT_mu0_muoncalib_ds3_L1MU15'      : {  1 : TriggerRule(PS=-1, comment='disabled for now', rerun=0) },
    'HLT_mu0_muoncalib_ds3_L1MU4_EMPTY' : {  1 : TriggerRule(PS=-1, comment='disabled for now', rerun=0) },
    }    

########################################
# TGC noise burst: unPS, at most a few events per run

stream_TGCNoiseBurst_Rules = {
    'HLT_noalg_L1TGC_BURST'              : {  1 : TriggerRule(PS=1, comment='TGCNoiseBurst', rerun=0) },
}

########################################
# CSC monitoring: 1% of all L1 accepts both cosmic and physics

stream_CSCMonitoring_Rules = {
    'HLT_cscmon_L1All'                   : {  1 : TriggerRule(PS=100, comment='CSC', rerun=0) },
}

########################################
# LAr Noise Burst

stream_LArNoiseBurst_Rules = {
    # rerun on all events
    'HLT_larnoiseburst_rerun'            : {   1 : TriggerRule(PS=0, comment='LArNoiseBurst', rerun=True) }, 

    # seeded items with unprescale
    'HLT_larnoiseburst_L1J75_XE40'       : {   1 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0) },
    'HLT_larnoiseburst_L1J40_XE50'       : {   1 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0) },
    'HLT_larnoiseburst_L1J100'           : {   1 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0) },
    'HLT_larnoiseburst_L1XE80'           : {   1 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0) },

    # seeded items disabled in physics run
    'HLT_larnoiseburst_L1XE35'           : {   1 : TriggerRule(PS=1,comment='LArNoiseBurst', rerun=0),
                                              10 : TriggerRule(PS=-1, comment='LArNoiseBurst, not usable with PSed XE35', rerun=0) },
    'HLT_larnoiseburst_L1XE50'           : {   1 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0),
                                              10 : TriggerRule(PS=-1, comment='LArNoiseBurst, not usable with PSed XE50', rerun=0) },
    'HLT_larnoiseburst_L1J50'            : {   1 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0),
                                              10 : TriggerRule(PS=-1, comment='LArNoiseBurst, not usable with PSed J50', rerun=0) },
    'HLT_larnoiseburst_L1J75'            : {   1 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0),
                                              10 : TriggerRule(PS=-1, comment='LArNoiseBurst, not usable with PSed J75', rerun=0) },
    }

########################################
# LArCells: 10 Hz

stream_LArCells_Rules = {

    #=======================================
    # EM based
    
    # EM3
    'HLT_g3_loose_larpeb'                : {  1 : TriggerRule(PS=1,  comment='LArCells', rerun=0), 
                                             10 : TriggerRule(PS=-1, comment='LArCells, disabled as only for cosmic item', rerun=0) },
    # EM10VH
    'HLT_g12_loose_larpeb'               : {  1 : TriggerRule(PS=1,  comment='LArCells', rerun=0), 
                                             10 : TriggerRule(rate=1.2, comment='LArCells', rerun=0) },
    # EM15
    'HLT_g20_loose_larpeb_L1EM15'        : {  1 : TriggerRule(PS=1,  comment='LArCells', rerun=0), 
                                             10 : TriggerRule(rate=1.2, comment='LArCells', rerun=0) },
    # EM20VHI
    'HLT_g40_loose_larpeb'               : {  1 : TriggerRule(PS=1,  comment='LArCells', rerun=0),
                                             10 : TriggerRule(rate=1.2, comment='LArCells', rerun=0) },
    'HLT_g60_loose_larpeb'               : {  1 : TriggerRule(PS=1,  comment='LArCells', rerun=0),
                                             10 : TriggerRule(rate=1.2, comment='LArCells', rerun=0) },
    'HLT_g80_loose_larpeb'               : {  1 : TriggerRule(PS=1,  comment='LArCells', rerun=0),
                                             10 : TriggerRule(rate=1.2, comment='LArCells', rerun=0) },

    #=======================================
    # Jet based

    # J12
    'HLT_conej40_larpebj'                : {  1 : TriggerRule(PS=1, comment= "LArCells" ,rerun=0),
                                             10 : TriggerRule(rate=1.2, comment= "LArCells" ,rerun=0) },
    # J75
    'HLT_conej165_larpebj'               : {  1 : TriggerRule(PS=1, comment= "LArCells" ,rerun=0),
                                             10 : TriggerRule(rate=1.2, comment= "LArCells" ,rerun=0) },
    # J30.31ETA49
    'HLT_conej75_320eta490_larpebj'      : {  1 : TriggerRule(PS=1, comment= "LArCells" ,rerun=0),
                                             10 : TriggerRule(rate=1.2, comment= "LArCells" ,rerun=0) },
    # J75.31ETA49
    'HLT_conej140_320eta490_larpebj'     : {  1 : TriggerRule(PS=1, comment= "LArCells" ,rerun=0),
                                             10 : TriggerRule(rate=1.2, comment= "LArCells" ,rerun=0) },
    }

########################################
# HLT_IDCosmic: 5 Hz in physics

stream_HLT_IDCosmic_Rules = {
    # efficiency 0 in cost file -> set input rate by hand
    'HLT_id_cosmicid_L1MU4_EMPTY'       : {  1 : TriggerRule(PS=1, comment='Express', rerun=0, ESValue=1),
                                            10 : TriggerRule(inputRate=5, scaling='bunches', comment='', rerun=0) },
    'HLT_id_cosmicid_L1MU11_EMPTY'      : {  1 : TriggerRule(PS=1, comment='Express', rerun=0, ESValue=1),
                                            10 : TriggerRule(inputRate=5, scaling='bunches', comment='', rerun=0) },
    }

########################################
# Tile: 3 Hz

stream_Tile_Rules = {
    'HLT_tilecalib_laser'                : { 1 : TriggerRule(PS=1, comment='Tile', rerun=0) },
    }

########################################
# ID Monitoring: 10 Hz

stream_IDMonitoring_Rules = {
    'HLT_noalg_idmon_L1RD0_EMPTY'       : {  1 : TriggerRule(inputRate=10, scaling="bunches", comment='IDMonitoring', rerun=0, ESRate=1.0),
                                            10 : TriggerRule(inputRate=0.5, scaling="bunches", comment='IDMonitoring', rerun=0, ESRate=0.5) },
    'HLT_noalg_idmon_L1RD0_FILLED'      : {  1 : TriggerRule(inputRate=5.0, scaling="bunches", comment='IDMonitoring', rerun=0) },
    'HLT_noalg_idmon_L1RD0_UNPAIRED_ISO': {  1 : TriggerRule(inputRate=5.0, scaling="bunches", comment='IDMonitoring', rerun=0) }, 
    }

########################################
# CosmicCalo: 5 Hz

stream_CosmicCalo_Rules = {
    'HLT_larps_L1EM3_EMPTY'                  : {  1 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0),
                                                 10 : TriggerRule(rate=0.5, scaling="bunches", comment='CosmicCalo', rerun=0) },              
    'HLT_larps_L1EM7_EMPTY'                  : {  1 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0),
                                                 10 : TriggerRule(rate=0.5, scaling="bunches", comment='CosmicCalo', rerun=0) },              
    'HLT_larps_L1J12_EMPTY'                  : {  1 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0),
                                                 10 : TriggerRule(rate=0.5, scaling="bunches", comment='CosmicCalo', rerun=0) },              
    'HLT_larps_L1J30_EMPTY'                  : {  1 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0),
                                                 10 : TriggerRule(rate=0.5, scaling="bunches", comment='CosmicCalo', rerun=0) },              
    'HLT_larps_L1TAU8_EMPTY'                 : {  1 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0),
                                                 10 : TriggerRule(rate=0.5, scaling="bunches", comment='CosmicCalo', rerun=0) },              
    'HLT_larps_L1J3031ETA49_EMPTY'           : {  1 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0),
                                                 10 : TriggerRule(rate=0.5, scaling="bunches", comment='CosmicCalo', rerun=0) },              

    'HLT_noalg_cosmiccalo_L1EM3_EMPTY'       : {  1 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0),
                                                 10 : TriggerRule(inputRate=0.5, scaling="bunches", comment='CosmicCalo', rerun=0) }, 
    'HLT_noalg_cosmiccalo_L1J12_EMPTY'       : {  1 : TriggerRule(PS=1, comment='CosmicCalo, Express', rerun=0, ESValue=1), 
                                                 10 : TriggerRule(inputRate=0.5, comment='CosmicCalo', rerun=0) }, 
    'HLT_noalg_cosmiccalo_L1J30_EMPTY'       : {  1 : TriggerRule(PS=1, comment='CosmicCalo, Express', rerun=0, ESValue=1), 
                                                 10 : TriggerRule(inputRate=0.5, comment='CosmicCalo', rerun=0) }, 
    'HLT_noalg_cosmiccalo_L1J3031ETA49_EMPTY': {  1 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0),
                                                 10 : TriggerRule(inputRate=0.5, comment='CosmicCalo', rerun=0) }, 

    # setting RD1_EMPTY rate to the (almost) max prescale 
    # RD1_EMPTY: zero-bias readout
    'L1_RD1_EMPTY'                            : { 1 : TriggerRule(PS=3657140, comment='CosmicCalo', rerun=0) },
    'HLT_noalg_cosmiccalo_L1RD1_EMPTY'        : { 1 : TriggerRule(inputRate=1.0, comment='CosmicCalo', rerun=0) },
}

########################################
# LArCellsEmpty: 10 Hz

stream_LArCellsEmpty_Rules = {
    'HLT_larps_L1J12_FIRSTEMPTY'         : {  1 : TriggerRule(PS=1 ,comment='LArCellsEmpty', rerun=0),
                                             10 : TriggerRule(rate=1.0 ,scaling="bunches", comment='LArCellsEmpty', rerun=0) },
    'HLT_larps_L1J30_FIRSTEMPTY'         : {  1 : TriggerRule(PS=1 ,comment='LArCellsEmpty', rerun=0),
                                             10 : TriggerRule(rate=1.0 ,scaling="bunches", comment='LArCellsEmpty', rerun=0) },
    'HLT_larps_L1EM7_FIRSTEMPTY'         : {  1 : TriggerRule(PS=1 ,comment='LArCellsEmpty', rerun=0),
                                             10 : TriggerRule(rate=1.0 ,scaling="bunches", comment='LArCellsEmpty', rerun=0) },

    'HLT_larcalib_L1EM3_EMPTY'           : {  1 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0),
                                             10 : TriggerRule(rate=1.0 ,scaling="bunches", comment='LArCellsEmpty', rerun=0) },
    'HLT_larcalib_L1EM7_EMPTY'           : {  1 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0),
                                             10 : TriggerRule(rate=1.0 ,scaling="bunches", comment='LArCellsEmpty', rerun=0) },
    'HLT_larcalib_L1TAU8_EMPTY'          : {  1 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0),
                                             10 : TriggerRule(rate=1.0 ,scaling="bunches", comment='LArCellsEmpty', rerun=0) },
    'HLT_larcalib_L1J12_EMPTY'           : {  1 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0),
                                             10 : TriggerRule(rate=1.0 ,scaling="bunches", comment='LArCellsEmpty', rerun=0) },
    'HLT_larcalib_L1J3031ETA49_EMPTY'    : {  1 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0),
                                             10 : TriggerRule(rate=1.0 ,scaling="bunches", comment='LArCellsEmpty', rerun=0) },

    'HLT_larhec_L1J12_FIRSTEMPTY'        : {  1 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0),
                                             10 : TriggerRule(rate=1.0 ,scaling="bunches", comment='LArCellsEmpty', rerun=0) },
    'HLT_larhec_L1J30_FIRSTEMPTY'        : {  1 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0),
                                             10 : TriggerRule(rate=1.0 ,scaling="bunches", comment='LArCellsEmpty', rerun=0) },
    'HLT_larhec_L1EM7_FIRSTEMPTY'        : {  1 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0),
                                             10 : TriggerRule(rate=1.0 ,scaling="bunches", comment='LArCellsEmpty', rerun=0) },
    'HLT_larhec_L1TAU8_FIRSTEMPTY'       : {  1 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0),
                                             10 : TriggerRule(rate=1.0 ,scaling="bunches", comment='LArCellsEmpty', rerun=0) },
}
    
#################################################################################################
#
#
# background stream
#
#
#################################################################################################


background_Rules = {

    # 20 Hz in physics
    # rate not predictable, needs manual adjustment on PS values

    #=======================================
    # Level-1
    # These are the L1 items for L1Bkg
    'L1_BCM_AC_CA_BGRP0'                 : {  1 : TriggerRule(PS=100000, comment='L1', rerun=0) },   
    'L1_BCM_Wide_EMPTY'                  : {  1 : TriggerRule(PS=10, comment='L1', rerun=0) },
    'L1_BCM_Wide_UNPAIRED_ISO'           : {  1 : TriggerRule(PS=3, comment='L1', rerun=0) },
    'L1_BCM_Wide_UNPAIRED_NONISO'        : {  1 : TriggerRule(PS=2000, comment='L1', rerun=0) },
    'L1_BCM_Wide_CALIB'                  : {  1 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_BCM_Wide_ABORTGAPNOTCALIB'       : {  1 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_BCM_AC_UNPAIRED_ISO'             : {  1 : TriggerRule(PS=10, comment='L1', rerun=0) },
    'L1_BCM_CA_UNPAIRED_ISO'             : {  1 : TriggerRule(PS=10, comment='L1', rerun=0) },
    'L1_BCM_AC_UNPAIRED_NONISO'          : {  1 : TriggerRule(PS=200, comment='L1', rerun=0) },
    'L1_BCM_CA_UNPAIRED_NONISO'          : {  1 : TriggerRule(PS=200, comment='L1', rerun=0) },
    'L1_BCM_AC_ABORTGAPNOTCALIB'         : {  1 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_BCM_CA_ABORTGAPNOTCALIB'         : {  1 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_BCM_AC_CALIB'                    : {  1 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_BCM_CA_CALIB'                    : {  1 : TriggerRule(PS=1, comment='L1', rerun=0) },

    'L1_J30.31ETA49_UNPAIRED_NONISO'     : {  1 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J30.31ETA49_UNPAIRED_ISO'        : {  1 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J12_UNPAIRED_ISO'                : {  1 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J12_UNPAIRED_NONISO'             : {  1 : TriggerRule(PS=10, comment='L1', rerun=0) },
    'L1_J12_ABORTGAPNOTCALIB'            : {  1 : TriggerRule(PS=1, comment='L1', rerun=0) },

    'L1_MU4_UNPAIRED_ISO'                : {  1 : TriggerRule(PS=900, comment='L1', rerun=0) },

    'L1_J50_UNPAIRED_ISO'                : {  1 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J50_UNPAIRED_NONISO'             : {  1 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J50_ABORTGAPNOTCALIB'            : {  1 : TriggerRule(PS=1, comment='L1', rerun=0) },



    #=======================================
    # HLT
    'HLT_noalg_bkg_L1Bkg'                : {  1 : TriggerRule(PS=-1, comment='Background disabled in cosmic', rerun=0),
                                             10 : TriggerRule(PS=1, comment='Background', rerun=0) },
}


#################################################################################################
#
#
# other common monitoring rules
#
#
#################################################################################################


common_monitoring_Rules = {
    'HLT_costmonitor'                     : {  1 : TriggerRule(PS=2500, comment='CostMonitoring', rerun=0) }, 
    'HLT_timeburner'                      : {  1 : TriggerRule(PS=-1, comment='DISCARD', rerun=0) }, 

    'HLT_noalg_dcmmon_L1RD0_EMPTY'        : {  1 : TriggerRule(rate=10.0, comment='monitoring_random', rerun=0) }, 
    # Chain to stress-test ROS
    #'HLT_noalg_dcmmon_L1RD2_EMPTY'       : {  1 : TriggerRule(rate=10000.0, comment='monitoring_random', rerun=0) }, 

    'L1_ALFA_ANY'                        : {  1 : TriggerRule(PS=200, comment='L1', rerun=0) },
#    'HLT_alfacalib_L1ALFA_ANY'           : {  1 : TriggerRule(PS=1, comment='ALFA Rules', rerun=0) },    
    'HLT_alfacalib_L1ALFA_ANY'           : {   1 : TriggerRule(PS=1, comment='ALFA Rules') },

    'HLT_robrequest'                      : {  1 : TriggerRule(PS=-1, comment='DISCARD', rerun=0) }, 
    'HLT_robrequest_L1RD0_EMPTY'          : {  1 : TriggerRule(PS=-1, comment='DISCARD', rerun=0) }, 
}

#################################################################################################
#################################################################################################


physics_rules = {}

RulesList=[stream_IDTracks_Rules, stream_PixelBeam_Rules, stream_PixelNoise_Rules, stream_SCTNoise_Rules, stream_MuonCalibration_Rules, stream_TGCNoiseBurst_Rules, stream_CSCMonitoring_Rules, stream_LArNoiseBurst_Rules, stream_LArCells_Rules, stream_HLT_IDCosmic_Rules, stream_Tile_Rules, stream_IDMonitoring_Rules, stream_CosmicCalo_Rules, stream_LArCellsEmpty_Rules, background_Rules, common_monitoring_Rules]
for Rules in RulesList:
    for newrule in Rules.keys():
        if physics_rules.has_key(newrule):
            print 'FATAL: duplicated rule cannot be added.',newrule
    physics_rules.update(Rules)

#################################################################################################
#################################################################################################
