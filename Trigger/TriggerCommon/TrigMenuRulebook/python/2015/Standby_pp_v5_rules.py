# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Standby_pp_v5'

doALFA = False

from runOptions import filled_bunches as NumBunches
mu4_unpairediso_ps=NumBunches*0.1
if mu4_unpairediso_ps < 1: mu4_unpairediso_ps=1
ZDC_A_C_ps = NumBunches*30
ZDC_A_ps   = NumBunches*150

# SX check naming of L1 and HLT jet fwd items everywhere. Is this 'L1_J30.31ETA49' correct ?
# SX Remember : most important is that rate after L1 (not after HLT) is less than 500 Hz.
# SX find the definition of the HLT_*_ L1 seed for this under TriggerMenu/python/menu, either L1Seeds.py or other .py

#################################################################################################
#
#
# Background triggers
#
#
#################################################################################################

background_Rules = {

    #############################################################################################
    # Level-1

    'L1_BCM_AC_CA_BGRP0'           : { 5000 : TriggerRule(PS=2000, comment='L1', rerun=0) },  # pay attention to this, last LB after SB declared and before we switch to physics goes up   
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

    'L1_MU4_UNPAIRED_ISO'     : { 5000 : TriggerRule(PS=mu4_unpairediso_ps, comment='L1', rerun=0) },

    'L1_J50_UNPAIRED_ISO'     : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J50_UNPAIRED_NONISO'  : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J50_ABORTGAPNOTCALIB' : { 5000 : TriggerRule(PS=1, comment='L1', rerun=0) },

    #managed by the AutpPSer NCB mode
    'L1_J12_BGRP12'                   : { 5000 : TriggerRule(PS=25000, comment='L1', rerun=0) },
    'L1_J30.31ETA49_BGRP12'                   : { 5000 : TriggerRule(PS=5000, comment='L1', rerun=0) },

    'L1_J12'         : { 5000 : TriggerRule(PS=25000, comment='L1', rerun=0) }, 
    'L1_J30.31ETA49' : { 5000 : TriggerRule(PS=5000, comment='L1', rerun=0) }, 

    #############################################################################################
    # HLT

    'HLT_noalg_bkg_L1Bkg'     : { 5000 : TriggerRule(PS=1, comment='Background', rerun=0) },
    'HLT_noalg_bkg_L1J12_BGRP12'         : { 5000 : TriggerRule(PS=1, comment = 'Background', rerun = 0.0) },  
    'HLT_noalg_bkg_L1J30.31ETA49_BGRP12' : { 5000 : TriggerRule(PS=1, comment = 'Background', rerun = 0.0) }, 
}

#################################################################################################
#
#
# Monitoring rules
#
#
#################################################################################################

monitoring_Rules = {

    # SX comments
    # SC rate for background items should be ~ 100 Hz - source : Kunihiro
    # Flat Top with J12/FJ30 items (80Hz) and noalg_L1Bkg at 20Hz
    # put all PS at L!, because that is what matters for pixel and 500 Hz limit
    
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
    'L1_J12_FIRSTEMPTY'   : { 5000 : TriggerRule(PS=100, comment='L1', rerun=0) },
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

    'L1_XE35'   : { 5000 : TriggerRule(PS=10000, comment='L1', rerun=0) },  # feeds larnoiseburst 
    'L1_XE50'   : { 5000 : TriggerRule(PS=7, comment='L1', rerun=0) },  # feeds larnoiseburst
    'L1_XE70'   : { 5000 : TriggerRule(PS=5, comment='L1', rerun=0) },  # feeds larnoiseburst 
    'L1_XE80'   : { 5000 : TriggerRule(PS=5, comment='L1', rerun=0) },  # feeds larnoiseburst 
    'L1_J50'   : { 5000 : TriggerRule(PS=3000, comment='L1', rerun=0) },  # feeds larnoiseburst
    'L1_J100'   : { 5000 : TriggerRule(PS=600, comment='L1', rerun=0) },  # feeds larnoiseburst
    'L1_J75_XE40'   : { 5000 : TriggerRule(PS=600, comment='L1', rerun=0) },  # feeds larnoiseburst
    'L1_J40_XE50'   : { 5000 : TriggerRule(PS=600, comment='L1', rerun=0) },  # feeds larnoiseburst
    
    
    #############################################################################################
    # HLT

    'HLT_noalg_L1Calo'                        : { 5000 : TriggerRule(PS=-1, comment='L1Calo', rerun=0) },    

    'HLT_noalg_L1Standby'                     : { 5000 : TriggerRule(PS=10, comment='Standby', rerun=0) },

    # SX comment : L1Standby is for detector monitoring, all items mentioned here and not labelled EMPTY,RD,ZB,CALREQ,ALFA end up here. For now we miss basically only MU items to get some muon detector monitoring, is this ok?
    # SX right now, due to L1Bkg J12 and FJ30, L1Standby gets quite some rate (80 Hz), overlapping with L1Bkg stream. Is this a problem? else, should we remove L1J12 and L1FJ30 from L1Standby?

    #=======================================
    # Monitoring/calibration items to be disabled in Standby
    # - IDTracks
    # - PixelBeam
    # - PixelNoise
    # - SCTNoise
    # - Muon calibration
    # - L1Calo

    #=======================================
    # TGC NoiseBurst
    'HLT_noalg_L1TGC_BURST'                   : {  1 : TriggerRule(PS=1, comment='TGCNoiseBurst', rerun=0) },

    #=======================================
    # CSC monitoring
    'HLT_cscmon_L1All'                        : { 5000 : TriggerRule(PS=100.0, comment = 'CSC', rerun = 0.0) }, 

    #=======================================
    # LArNoiseBurst
    'HLT_larnoiseburst_rerun'                 : { 5000 : TriggerRule(PS=0,   comment = 'LArNoiseBurst', rerun=True) }, 

    'HLT_larnoiseburst_L1J75_XE40'            : { 5000 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0) },
    'HLT_larnoiseburst_L1J40_XE50'            : { 5000 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0) },
    'HLT_larnoiseburst_L1J100'                : { 5000 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0) },
    'HLT_larnoiseburst_L1XE80'                : { 5000 : TriggerRule(PS=1, comment='LArNoiseBurst', rerun=0) },

    # prescale out XE items as rate too herratic, dangerous for pixel
    'HLT_larnoiseburst_L1XE35'                : { 5000 : TriggerRule(PS=1,   comment = 'LArNoiseBurst', rerun=0) }, 
    'HLT_larnoiseburst_L1XE50'                : { 5000 : TriggerRule(PS=1,   comment = 'LArNoiseBurst', rerun=0) },
    'HLT_larnoiseburst_L1J50'                 : { 5000 : TriggerRule(PS=1,   comment = 'LArNoiseBurst', rerun=0) },
    'HLT_larnoiseburst_L1J75'                 : { 5000 : TriggerRule(PS=10,  comment = 'LArNoiseBurst', rerun=0) },#needs additional prescale, since J75 feeds also other standby items which can get more rate. This should give 1 Hz at squeeze or warm start

    #=======================================
    # LArCells
    'HLT_g3_loose_larpeb'           : { 5000 : TriggerRule(PS=-1, comment = 'LArCells', rerun = 0.0) }, # added explicit PS=-1 rule
    'HLT_g12_loose_larpeb'          : { 5000 : TriggerRule(PS=1, comment='LArCells', rerun=0) }, 
    'HLT_g20_loose_larpeb_L1EM15'   : { 5000 : TriggerRule(PS=1, comment='LArCells', rerun=0) },
    'HLT_g40_loose_larpeb'          : { 5000 : TriggerRule(PS=1, comment = 'LArCells', rerun = 0.0) },
    'HLT_g60_loose_larpeb'          : { 5000 : TriggerRule(PS=1, comment = 'LArCells', rerun = 0.0) },
    'HLT_g80_loose_larpeb'          : { 5000 : TriggerRule(PS=1, comment = 'LArCells', rerun = 0.0) },

    'HLT_conej40_larpebj'           : { 5000 : TriggerRule(PS=-1, comment= "jet monitoring" ,rerun=0) }, # feed from L1J12, if HLT provides some rejection then ok to keep ?
    'HLT_conej165_larpebj'          : { 5000 : TriggerRule(PS=1, comment= "jet monitoring" ,rerun=0) }, 
    'HLT_conej75_320eta490_larpebj' : { 5000 : TriggerRule(PS=1, comment= "jet monitoring" ,rerun=0) }, 
    'HLT_conej140_320eta490_larpebj': { 5000 : TriggerRule(PS=1, comment= "jet monitoring" ,rerun=0) },

    #=======================================
    # HLT_IDCosmic
    'HLT_id_cosmicid_L1MU4_EMPTY'             : { 5000 : TriggerRule(PS=1, ESValue=0, comment='express', rerun=0) },
    'HLT_id_cosmicid_L1MU11_EMPTY'            : { 5000 : TriggerRule(PS=1, ESValue=0, comment='express', rerun=0) },

    #=======================================
    # Tile
    'HLT_tilecalib_laser'                     : { 5000 : TriggerRule(PS=1, comment='Tile', rerun=0) },

    #=======================================
    # ID Monitoring
    'HLT_noalg_idmon_L1RD0_EMPTY'             : { 5000 : TriggerRule(PS=1, ESValue=0, comment='IDMonitoring express', rerun=0) },
    'HLT_noalg_idmon_L1RD0_FILLED'            : { 5000 : TriggerRule(inputRate=5.0, comment='IDMonitoring', rerun=0) },
    'HLT_noalg_idmon_L1RD0_UNPAIRED_ISO'      : { 5000 : TriggerRule(PS=1, comment='IDMonitoring', rerun=0) }, 

    #=======================================
    # CosmicCalo
    'HLT_larps_L1EM3_EMPTY'                   : { 5000 : TriggerRule(PS=-1 , comment='CosmicCalo', rerun=0) }, # added explicit PS=-1 rule
    'HLT_larps_L1EM7_EMPTY'                   : { 5000 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0) },
    'HLT_larps_L1J12_EMPTY'                   : { 5000 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0) },
    'HLT_larps_L1J30_EMPTY'                   : { 5000 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0) },
    'HLT_larps_L1TAU8_EMPTY'                  : { 5000 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0) },
    'HLT_larps_L1J3031ETA49_EMPTY'            : { 5000 : TriggerRule(PS=1, comment='CosmicCalo', rerun=0) }, 

    ### EM chain was not in the original list, go to Express stream 
    'HLT_noalg_cosmiccalo_L1EM3_EMPTY'        : { 5000 : TriggerRule(PS=1.0, comment = 'CosmicCalo', rerun = 0.0) }, 
    'HLT_noalg_cosmiccalo_L1J12_EMPTY'        : { 5000 : TriggerRule(PS=1.0, ESValue=0, comment = 'CosmicCalo express', rerun = 0.0) }, 
    'HLT_noalg_cosmiccalo_L1J30_EMPTY'        : { 5000 : TriggerRule(PS=1.0, ESValue=0, comment = 'CosmicCalo express', rerun = 0.0) }, 
    'HLT_noalg_cosmiccalo_L1J3031ETA49_EMPTY' : { 5000 : TriggerRule(PS=1.0, comment = 'CosmicCalo', rerun = 0.0) }, 
    'HLT_noalg_cosmiccalo_L1RD1_EMPTY'        : { 5000 : TriggerRule(rate=1.0, ESValue=0, comment = 'CosmicCalo express', rerun=0) },

    #=======================================
    # LArCellsEmpty
    'HLT_larps_L1EM7_FIRSTEMPTY'              : { 5000 : TriggerRule(PS=1 , comment='LArCellsEmpty', rerun=0) }, 
    'HLT_larps_L1J12_FIRSTEMPTY'              : { 5000 : TriggerRule(PS=1, comment = 'LArCellsEmpty', rerun=0) }, 
    'HLT_larps_L1J30_FIRSTEMPTY'              : { 5000 : TriggerRule(PS=1, comment = 'LArCellsEmpty', rerun=0) }, 

    'HLT_larcalib_L1EM3_EMPTY'                : { 5000 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0) },
    'HLT_larcalib_L1EM7_EMPTY'                : { 5000 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0) }, 
    'HLT_larcalib_L1TAU8_EMPTY'               : { 5000 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0) },
    'HLT_larcalib_L1J12_EMPTY'                : { 5000 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0) },
    'HLT_larcalib_L1J3031ETA49_EMPTY'         : { 5000 : TriggerRule(PS=1, comment='LArCellsEmpty', rerun=0) },

    'HLT_larhec_L1J12_FIRSTEMPTY'             : { 5000 : TriggerRule(PS=1, comment = 'LArCellsEmpty', rerun = 0) },
    'HLT_larhec_L1J30_FIRSTEMPTY'             : { 5000 : TriggerRule(PS=1, comment = 'LArCellsEmpty', rerun = 0) }, 
    'HLT_larhec_L1EM7_FIRSTEMPTY'             : { 5000 : TriggerRule(PS=1, comment = 'LArCellsEmpty', rerun = 0) },
    'HLT_larhec_L1TAU8_FIRSTEMPTY'            : { 5000 : TriggerRule(PS=1, comment = 'LArCellsEmpty', rerun = 0) },

    #=======================================
    # general monitorings
    'HLT_timeburner'                          : { 5000 : TriggerRule(PS=-1, comment = 'DISCARD', rerun=0) }, 

    'HLT_robrequest'                          : { 5000 : TriggerRule(PS=-1, comment = 'DISCARD', rerun=0) }, 
    'HLT_robrequest_L1RD0_EMPTY'              : { 5000 : TriggerRule(PS=-1, comment = 'DISCARD', rerun=0) }, 

    'HLT_noalg_dcmmon_L1RD0_EMPTY'            : { 5000 : TriggerRule(rate=10.0, comment = 'monitoring_random', rerun=0) }, 
    # chain to stress-test ROS
    #'HLT_noalg_dcmmon_L1RD2_EMPTY'            : { 10000 : TriggerRule(rate=10000.0, comment = 'monitoring_random', rerun = 0) },
    }




#################################################################################################
#
#
# Enable ZDC triggers for commisioning
#
#
#################################################################################################

L1_list=['L1_ZDC_A','L1_ZDC_C' ]
monitoring_Rules.update(dict(map(None,L1_list,len(L1_list)*[{
    400 : TriggerRule(PS=ZDC_A_ps, comment=''),
    }])))

# supposed 10Hz/bunch (mu=0.5)
L1_list=['L1_ZDC_AND' ]
monitoring_Rules.update(dict(map(None,L1_list,len(L1_list)*[{
    400 : TriggerRule(PS=ZDC_A_C_ps, comment=''),
    }])))

L1_list=['L1_ZDC_A_C' ]
monitoring_Rules.update(dict(map(None,L1_list,len(L1_list)*[{
    400 : TriggerRule(PS=ZDC_A_C_ps, comment=''),
    }])))




#################################################################################################
#
#
# ALFA rules
#
#
#################################################################################################

#=======================================
# alfa calibration rules

alfa_Rules_calibration = {
    'L1_ALFA_B7L1U'  : { 10 : TriggerRule(PS=-1, comment='alfa calib (ATR-11756)') },
    'L1_ALFA_B7L1L'  : { 10 : TriggerRule(PS=-1, comment='alfa calib (ATR-11756)') },
    'L1_ALFA_A7L1U'  : { 10 : TriggerRule(PS=-1, comment='alfa calib (ATR-11756)') },
    'L1_ALFA_A7L1L'  : { 10 : TriggerRule(PS=-1, comment='alfa calib (ATR-11756)') },
    'L1_ALFA_A7R1U'  : { 10 : TriggerRule(PS=-1, comment='alfa calib (ATR-11756)') },
    'L1_ALFA_A7R1L'  : { 10 : TriggerRule(PS=-1, comment='alfa calib (ATR-11756)') },
    'L1_ALFA_B7R1U'  : { 10 : TriggerRule(PS=-1, comment='alfa calib (ATR-11756)') },
    'L1_ALFA_B7R1L'  : { 10 : TriggerRule(PS=-1, comment='alfa calib (ATR-11756)') },
    
    'HLT_alfacalib'  : { 10 : TriggerRule(PS=-1, comment='target 100 Hz, alfa calib (ATR-11756)') },
    }


#=======================================
# alfa rules for special ALFA beam based alignment fill with beta=19 m ATR-11364

alfa_Rules_physics = {

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

if doALFA == False:
    alfa_Rules_physics={}
    HLT_list=[
    'HLT_noalg_L1ALFA_Phys',
    'HLT_noalg_L1ALFA_SYS',
    'HLT_noalg_L1ALFA_PhysAny',
    'HLT_noalg_mb_L1MBTS_1',
    'HLT_noalg_mb_L1MBTS_2',
    'HLT_mb_sptrk_vetombts2in_L1ALFA_CEP',
    'HLT_mb_sptrk_vetombts2in_peb_L1ALFA_ANY',
    ]
    alfa_Rules_physics.update(dict(map(None,HLT_list,len(HLT_list)*[{
        1000 : TriggerRule(PS=-1, comment=''), 
        }])))

#=======================================

alfa_Rules={}

RulesList=[alfa_Rules_physics, alfa_Rules_calibration]
for Rules in RulesList:
    for newrule in Rules.keys():
        if alfa_Rules.has_key(newrule):
            print 'FATAL     Standby_pp_v5_rules     Duplicated rule inside ALFA rule. Cannot be added:',newrule
    alfa_Rules.update(Rules)
    

#################################################################################################
#
#
# disable rules
#
#
#################################################################################################

disabled_Rules={}

# --------------------------------------
# monitoring chains but not relevant for standby (muon, ID)

HLT_list=[
    # L1Calo saturated events
    'HLT_satu20em_l1satmon_L1EM22VHI_FIRSTEMPTY',
    'HLT_satu20em_l1satmon_L1EM20VH_FIRSTEMPTY',
    'HLT_satu20em_l1satmon_L1J100',
    'HLT_satu20em_l1satmon_L1J100.31ETA49',
    'HLT_satu20em_l1satmon_L1J100.31ETA49_FIRSTEMPTY',
    'HLT_satu20em_l1satmon_L1J100_FIRSTEMPTY',
    # LAr demonstrator Topo items
    'HLT_noalg_larcells_L1LAR-EM',
    'HLT_noalg_larcells_L1LAR-J',
    # beam spot
    'HLT_beamspot_activeTE_L2StarB_pebTRT',
    'HLT_beamspot_activeTE_trkfast_pebTRT',
    'HLT_beamspot_allTE_L2StarB_pebTRT',
    'HLT_beamspot_allTE_trkfast_pebTRT',
    'HLT_beamspot_trkFS_L2StarB',
    'HLT_beamspot_trkFS_trkfast',
    # cost monitoring
    'HLT_costmonitor',
    # ID tracks
    'HLT_idcalib_trk16_central',
    'HLT_idcalib_trk16_fwd',
    'HLT_idcalib_trk29_central',
    'HLT_idcalib_trk29_fwd',
    'HLT_idcalib_trk9_central',
    'HLT_idcalib_trk9_fwd',
    # zero-bias
    'HLT_j40_L1ZB',
    'HLT_noalg_zb_L1ZB',
    # pixel beam
    'HLT_lumipeb_L1RD0_ABORTGAPNOTCALIB',
    'HLT_lumipeb_L1RD0_EMPTY',
    'HLT_lumipeb_L1RD0_FILLED',
    'HLT_lumipeb_L1RD0_UNPAIRED_ISO',
    # muon calibration
    'HLT_mu0_muoncalib',
    'HLT_mu0_muoncalib_L1MU15',
    'HLT_mu0_muoncalib_L1MU4_EMPTY',
    'HLT_mu0_muoncalib_ds3',
    'HLT_mu0_muoncalib_ds3_L1MU15',
    'HLT_mu0_muoncalib_ds3_L1MU4_EMPTY',
    # tau overlay
    'HLT_noalg_to_L12MU20_OVERLAY',
    # sct noise
    'HLT_sct_noise', 
    ]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# physics chains

HLT_list=['HLT_g20_etcut_L1EM15I_mu4_iloose_taumass', 'HLT_g20_loose_L1EM15I_mu4_iloose_taumass', 'HLT_g20_etcut_L1EM15I_mu4_taumass', 'HLT_g10_etcut_L1EM8I_mu10_taumass', 'HLT_g10_loose_L1EM8I_mu10_iloose_taumass', 'HLT_g10_etcut_L1EM8I_mu10_iloose_taumass', 'HLT_2e12_lhloose_HLTCalo_L12EM10VH', 'HLT_2e12_lhloose_L12EM10VH', 'HLT_2e12_lhloose_cutd0dphideta_L12EM10VH', 'HLT_2e12_lhloose_mu10', 'HLT_2e12_lhloose_nod0_L12EM10VH', 'HLT_2e12_lhloose_nod0_mu10', 'HLT_2e12_lhloose_nodeta_L12EM10VH', 'HLT_2e12_lhloose_nodphires_L12EM10VH', 'HLT_2e12_lhmedium_mu10', 'HLT_2e12_lhmedium_nod0_mu10', 'HLT_2e12_lhvloose_L12EM10VH', 'HLT_2e12_lhvloose_nod0_L12EM10VH', 'HLT_2e12_loose_L12EM10VH', 'HLT_2e12_loose_mu10', 'HLT_2e12_medium_mu10', 'HLT_2e12_vloose_L12EM10VH', 'HLT_2e15_lhloose_HLTCalo_L12EM13VH', 'HLT_2e15_lhloose_L12EM13VH', 'HLT_2e15_lhloose_cutd0dphideta_L12EM13VH', 'HLT_2e15_lhloose_nod0_L12EM13VH', 'HLT_2e15_lhvloose_L12EM13VH', 'HLT_2e15_lhvloose_nod0_L12EM13VH', 'HLT_2e15_loose_L12EM13VH', 'HLT_2e15_vloose_L12EM13VH', 'HLT_2e17_lhloose', 'HLT_2e17_lhloose_2j35_bperf', 'HLT_2e17_lhloose_HLTCalo', 'HLT_2e17_lhloose_L12EM15', 'HLT_2e17_lhloose_cutd0dphideta', 'HLT_2e17_lhloose_nod0', 'HLT_2e17_lhvloose', 'HLT_2e17_lhvloose_nod0', 'HLT_2e17_loose', 'HLT_2e17_loose_2j35_bperf', 'HLT_2e17_loose_L12EM15', 'HLT_2e17_vloose', 'HLT_2g10_loose', 'HLT_2g10_loose_mu20', 'HLT_2g10_medium_mu20', 'HLT_2g20_loose', 'HLT_2g20_loose_L12EM15', 'HLT_2g20_loose_g15_loose', 'HLT_2g20_tight', 'HLT_2g22_tight', 'HLT_2g25_tight', 'HLT_2g50_loose', 'HLT_2g60_loose_L12EM15VH', 'HLT_2j35_bmv2c2070_2j35_L13J25.0ETA23', 'HLT_2j35_bmv2c2070_split_2j35_L13J25.0ETA23', 'HLT_2j35_boffperf_2j35_L13J25.0ETA23', 'HLT_2j35_boffperf_2j35_L14J15.0ETA25', 'HLT_2j35_btight_2j35_L13J25.0ETA23', 'HLT_2j35_btight_2j35_L14J15.0ETA25', 'HLT_2j35_btight_split_2j35_L13J25.0ETA23', 'HLT_2j35_btight_split_2j35_L14J15.0ETA25', 'HLT_2j40_0eta490_invm250_L1XE55', 'HLT_2j45_bmedium_2j45_L13J25.0ETA23', 'HLT_2j45_bmedium_2j45_L14J15.0ETA25', 'HLT_2j45_bmedium_split_2j45_L13J25.0ETA23', 'HLT_2j45_bmedium_split_2j45_L14J15.0ETA25', 'HLT_2j45_bmv2c2077_2j45_L13J25.0ETA23', 'HLT_2j45_bmv2c2077_split_2j45_L13J25.0ETA23', 'HLT_2j45_boffperf_2j45_L13J25.0ETA23', 'HLT_2j45_boffperf_2j45_L14J15.0ETA25', 'HLT_2j45_btight_2j45', 'HLT_2j45_btight_2j45_L13J25.ETA23', 'HLT_2j45_btight_split_2j45', 'HLT_2j45_btight_split_2j45_L13J25.ETA23', 'HLT_2j55_bloose_L14J20.0ETA49', 'HLT_2j55_bmedium_2j55', 'HLT_2j55_bmedium_2j55_L13J25.ETA23', 'HLT_2j55_bmedium_L14J20.0ETA49', 'HLT_2j55_bmedium_split_2j55', 'HLT_2j55_bmedium_split_2j55_L13J25.ETA23', 'HLT_2j55_bperf_L14J20.0ETA49', 'HLT_2j65_boffperf_j65', 'HLT_2j65_btight_j65', 'HLT_2j65_btight_split_j65', 'HLT_2j70_bmedium_j70', 'HLT_2j70_bmedium_split_j70', 'HLT_2j70_btight_j70', 'HLT_2j70_btight_split_j70', 'HLT_2j75_bmedium_j75', 'HLT_2j75_bmedium_split_j75', 'HLT_2mu10', 'HLT_2mu10_bBmumu', 'HLT_2mu10_bBmumux_BcmumuDsloose', 'HLT_2mu10_bBmumux_BpmumuKp', 'HLT_2mu10_bBmumuxv2', 'HLT_2mu10_bDimu', 'HLT_2mu10_bDimu_noL2', 'HLT_2mu10_bDimu_noinvm_novtx_ss', 'HLT_2mu10_bDimu_novtx_noos', 'HLT_2mu10_bJpsimumu', 'HLT_2mu10_bJpsimumu_noL2', 'HLT_2mu10_bUpsimumu', 'HLT_2mu10_l2msonly', 'HLT_2mu10_l2msonly_bDimu_noL2', 'HLT_2mu10_l2msonly_bJpsimumu_noL2', 'HLT_2mu14', 'HLT_2mu14_2j35_bperf', 'HLT_2mu14_l2msonly', 'HLT_2mu4', 'HLT_2mu4_bBmumu', 'HLT_2mu4_bBmumux_BcmumuDsloose', 'HLT_2mu4_bBmumux_BpmumuKp', 'HLT_2mu4_bBmumuxv2', 'HLT_2mu4_bDimu', 'HLT_2mu4_bDimu_noL2', 'HLT_2mu4_bDimu_noinvm_novtx_ss', 'HLT_2mu4_bDimu_novtx_noos', 'HLT_2mu4_bJpsimumu', 'HLT_2mu4_bJpsimumu_noL2', 'HLT_2mu4_bUpsimumu', 'HLT_2mu4_l2msonly_bDimu_noL2', 'HLT_2mu4_l2msonly_bJpsimumu_noL2', 'HLT_2mu6', 'HLT_2mu6_10invm30_pt2_z10', 'HLT_2mu6_bBmumu', 'HLT_2mu6_bBmumux_BcmumuDsloose', 'HLT_2mu6_bBmumux_BpmumuKp', 'HLT_2mu6_bBmumuxv2', 'HLT_2mu6_bDimu', 'HLT_2mu6_bDimu_noL2', 'HLT_2mu6_bDimu_noinvm_novtx_ss', 'HLT_2mu6_bDimu_novtx_noos', 'HLT_2mu6_bJpsimumu', 'HLT_2mu6_bJpsimumu_noL2', 'HLT_2mu6_bUpsimumu', 'HLT_2mu6_l2msonly_bDimu_noL2', 'HLT_2mu6_l2msonly_bJpsimumu_noL2', 'HLT_2mu6_l2msonly_mu4_l2msonly_L12MU6_3MU4', 'HLT_3g15_loose', 'HLT_3g20_loose', 'HLT_3j175', 'HLT_3mu4', 'HLT_3mu4_bDimu', 'HLT_3mu4_bJpsi', 'HLT_3mu4_bTau', 'HLT_3mu4_bUpsi', 'HLT_3mu4_l2msonly', 'HLT_3mu6', 'HLT_3mu6_bDimu', 'HLT_3mu6_bJpsi', 'HLT_3mu6_bTau', 'HLT_3mu6_bUpsi', 'HLT_3mu6_msonly', 'HLT_3mu6_msonly_L1MU4_EMPTY', 'HLT_3mu6_msonly_L1MU4_UNPAIRED_ISO', 'HLT_4j100', 'HLT_4j25', 'HLT_4j45', 'HLT_4j85', 'HLT_4j85_jes', 'HLT_4j85_lcw', 'HLT_4j85_lcw_jes', 'HLT_4j85_lcw_nojcalib', 'HLT_4j85_nojcalib', 'HLT_5j25', 'HLT_5j45', 'HLT_5j55', 'HLT_5j60', 'HLT_5j70', 'HLT_5j85', 'HLT_5j85_jes', 'HLT_5j85_lcw', 'HLT_5j85_lcw_jes', 'HLT_5j85_lcw_nojcalib', 'HLT_5j85_nojcalib', 'HLT_6j25', 'HLT_6j45', 'HLT_6j45_0eta240', 'HLT_6j45_0eta240_L14J20', 'HLT_6j45_0eta240_L15J150ETA25', 'HLT_6j50_0eta240_L14J20', 'HLT_6j50_0eta240_L15J150ETA25', 'HLT_6j55_0eta240_L14J20', 'HLT_6j55_0eta240_L15J150ETA25', 'HLT_7j25', 'HLT_7j45', 'HLT_7j45_0eta240_L14J20', 'HLT_7j45_0eta240_L15J150ETA25', 'HLT_7j45_L14J20', 'HLT_e0_perf_L1EM15', 'HLT_e100_etcut', 'HLT_e100_lhvloose', 'HLT_e100_vloose', 'HLT_e10_etcut_L1EM7', 'HLT_e10_lhtight_idperf', 'HLT_e10_lhvloose_L1EM7', 'HLT_e10_tight_idperf', 'HLT_e10_vloose_L1EM7', 'HLT_e120_etcut', 'HLT_e120_lhloose', 'HLT_e120_lhloose_HLTCalo', 'HLT_e120_lhloose_nod0', 'HLT_e120_lhvloose', 'HLT_e120_loose', 'HLT_e120_vloose', 'HLT_e12_lhloose', 'HLT_e12_lhloose_2mu10', 'HLT_e12_lhloose_HLTCalo_L12EM10VH', 'HLT_e12_lhloose_L1EM10VH', 'HLT_e12_lhloose_cutd0dphideta_L1EM10VH', 'HLT_e12_lhloose_nod0', 'HLT_e12_lhloose_nod0_2mu10', 'HLT_e12_lhloose_nod0_L1EM10VH', 'HLT_e12_lhloose_nodeta_L1EM10VH', 'HLT_e12_lhloose_nodphires_L1EM10VH', 'HLT_e12_lhmedium', 'HLT_e12_lhmedium_2mu10', 'HLT_e12_lhmedium_nod0', 'HLT_e12_lhmedium_nod0_2mu10', 'HLT_e12_lhvloose_L1EM10VH', 'HLT_e12_lhvloose_nod0_L1EM10VH', 'HLT_e12_loose', 'HLT_e12_loose_2mu10', 'HLT_e12_loose_L1EM10VH', 'HLT_e12_medium', 'HLT_e12_medium_2mu10', 'HLT_e12_vloose_L1EM10VH', 'HLT_e13_etcut_trkcut_L1EM10', 'HLT_e13_etcut_trkcut_j20_perf_xe15_2dphi05', 'HLT_e13_etcut_trkcut_j20_perf_xe15_2dphi05_mt25', 'HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05', 'HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25', 'HLT_e13_etcut_trkcut_xs15', 'HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05', 'HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05_mt25', 'HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05', 'HLT_e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05_mt25', 'HLT_e13_etcut_trkcut_xs15_mt25', 'HLT_e140_lhloose', 'HLT_e140_lhloose_HLTCalo', 'HLT_e140_lhloose_nod0', 'HLT_e140_loose', 'HLT_e14_etcut', 'HLT_e14_etcut_e5_lhtight_Jpsiee', 'HLT_e14_etcut_e5_lhtight_nod0_Jpsiee', 'HLT_e14_etcut_e5_tight_Jpsiee', 'HLT_e14_lhtight', 'HLT_e14_lhtight_e4_etcut_Jpsiee', 'HLT_e14_lhtight_nod0', 'HLT_e14_lhtight_nod0_e4_etcut_Jpsiee', 'HLT_e14_tight', 'HLT_e14_tight_e4_etcut_Jpsiee', 'HLT_e15_etcut_L1EM7', 'HLT_e15_lhloose_HLTCalo_L12EM13VH', 'HLT_e15_lhloose_L1EM13VH', 'HLT_e15_lhloose_cutd0dphideta_L1EM13VH', 'HLT_e15_lhloose_nod0_L1EM13VH', 'HLT_e15_lhvloose_L1EM13VH', 'HLT_e15_lhvloose_L1EM7', 'HLT_e15_lhvloose_nod0_L1EM13VH', 'HLT_e15_loose_L1EM13VH', 'HLT_e15_vloose_L1EM13VH', 'HLT_e15_vloose_L1EM7', 'HLT_e17_lhloose', 'HLT_e17_lhloose_2e9_lhloose', 'HLT_e17_lhloose_HLTCalo', 'HLT_e17_lhloose_L1EM15', 'HLT_e17_lhloose_cutd0dphideta', 'HLT_e17_lhloose_cutd0dphideta_L1EM15', 'HLT_e17_lhloose_mu14', 'HLT_e17_lhloose_nod0', 'HLT_e17_lhloose_nod0_2e9_lhloose_nod0', 'HLT_e17_lhloose_nod0_L1EM15', 'HLT_e17_lhloose_nod0_mu14', 'HLT_e17_lhloose_nodeta_L1EM15', 'HLT_e17_lhloose_nodphires_L1EM15', 'HLT_e17_lhmedium', 'HLT_e17_lhmedium_2e9_lhmedium', 'HLT_e17_lhmedium_L1EM15HI', 'HLT_e17_lhmedium_iloose', 'HLT_e17_lhmedium_iloose_2e9_lhmedium', 'HLT_e17_lhmedium_iloose_L1EM15HI', 'HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo', 'HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_xe50', 'HLT_e17_lhmedium_nod0', 'HLT_e17_lhmedium_nod0_2e9_lhmedium_nod0', 'HLT_e17_lhmedium_nod0_L1EM15HI', 'HLT_e17_lhmedium_nod0_iloose', 'HLT_e17_lhmedium_nod0_iloose_2e9_lhmedium_nod0', 'HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo', 'HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_xe50', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50', 'HLT_e17_lhmedium_nod0_tau80_medium1_tracktwo', 'HLT_e17_lhmedium_tau25_medium1_tracktwo', 'HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50', 'HLT_e17_lhmedium_tau80_medium1_tracktwo', 'HLT_e17_lhvloose', 'HLT_e17_lhvloose_nod0', 'HLT_e17_loose', 'HLT_e17_loose_2e9_loose', 'HLT_e17_loose_L1EM15', 'HLT_e17_loose_mu14', 'HLT_e17_medium', 'HLT_e17_medium_2e9_medium', 'HLT_e17_medium_L1EM15HI', 'HLT_e17_medium_iloose', 'HLT_e17_medium_iloose_2e9_medium', 'HLT_e17_medium_iloose_L1EM15HI', 'HLT_e17_medium_iloose_tau25_medium1_tracktwo', 'HLT_e17_medium_iloose_tau25_medium1_tracktwo_xe50', 'HLT_e17_medium_tau25_medium1_tracktwo', 'HLT_e17_medium_tau25_medium1_tracktwo_xe50', 'HLT_e17_medium_tau80_medium1_tracktwo', 'HLT_e17_vloose', 'HLT_e18_etcut_trkcut_L1EM15', 'HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15', 'HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35', 'HLT_e18_etcut_trkcut_xs20', 'HLT_e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15', 'HLT_e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15_mt35', 'HLT_e18_etcut_trkcut_xs20_mt35', 'HLT_e200_etcut', 'HLT_e20_etcut_L1EM12', 'HLT_e20_lhmedium', 'HLT_e20_lhmedium_2g10_loose', 'HLT_e20_lhmedium_2g10_medium', 'HLT_e20_lhmedium_g35_loose', 'HLT_e20_lhmedium_nod0', 'HLT_e20_lhmedium_nod0_2g10_loose', 'HLT_e20_lhmedium_nod0_2g10_medium', 'HLT_e20_lhmedium_nod0_g35_loose', 'HLT_e20_lhvloose', 'HLT_e20_lhvloose_L1EM12', 'HLT_e20_lhvloose_nod0', 'HLT_e20_medium', 'HLT_e20_medium_2g10_loose', 'HLT_e20_medium_2g10_medium', 'HLT_e20_medium_g35_loose', 'HLT_e20_vloose', 'HLT_e20_vloose_L1EM12', 'HLT_e24_lhmedium_L1EM15VH', 'HLT_e24_lhmedium_L1EM15VH_g25_medium', 'HLT_e24_lhmedium_L1EM18VH', 'HLT_e24_lhmedium_L1EM20VH', 'HLT_e24_lhmedium_L1EM20VHI', 'HLT_e24_lhmedium_L1EM20VHI_mu8noL1', 'HLT_e24_lhmedium_cutd0dphideta_L1EM18VH', 'HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM18VH', 'HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM20VH', 'HLT_e24_lhmedium_idperf_L1EM20VH', 'HLT_e24_lhmedium_iloose_HLTCalo_L1EM18VH', 'HLT_e24_lhmedium_iloose_HLTCalo_L1EM20VH', 'HLT_e24_lhmedium_iloose_L1EM18VH', 'HLT_e24_lhmedium_iloose_L1EM20VH', 'HLT_e24_lhmedium_nod0_L1EM15VH', 'HLT_e24_lhmedium_nod0_L1EM15VH_g25_medium', 'HLT_e24_lhmedium_nod0_L1EM18VH', 'HLT_e24_lhmedium_nod0_L1EM20VHI', 'HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1', 'HLT_e24_lhmedium_nod0_iloose_L1EM18VH', 'HLT_e24_lhmedium_nod0_iloose_L1EM20VH', 'HLT_e24_lhmedium_nodeta_L1EM18VH', 'HLT_e24_lhmedium_nodeta_iloose_L1EM18VH', 'HLT_e24_lhmedium_nodphires_L1EM18VH', 'HLT_e24_lhmedium_nodphires_iloose_L1EM18VH', 'HLT_e24_lhtight_L1EM20VH', 'HLT_e24_lhtight_L1EM20VH_e15_etcut_Zee', 'HLT_e24_lhtight_cutd0dphideta_iloose', 'HLT_e24_lhtight_cutd0dphideta_iloose_L1EM20VH', 'HLT_e24_lhtight_iloose', 'HLT_e24_lhtight_iloose_HLTCalo', 'HLT_e24_lhtight_iloose_HLTCalo_L1EM20VH', 'HLT_e24_lhtight_iloose_L1EM20VH', 'HLT_e24_lhtight_iloose_L2EFCalo_L1EM20VH', 'HLT_e24_lhtight_nod0_L1EM20VH', 'HLT_e24_lhtight_nod0_iloose', 'HLT_e24_lhtight_nod0_iloose_L1EM20VH', 'HLT_e24_lhvloose_L1EM18VH', 'HLT_e24_lhvloose_L1EM18VH_3j20noL1', 'HLT_e24_lhvloose_L1EM18VH_4j20noL1', 'HLT_e24_lhvloose_L1EM18VH_5j15noL1', 'HLT_e24_lhvloose_L1EM18VH_6j15noL1', 'HLT_e24_lhvloose_L1EM20VH', 'HLT_e24_lhvloose_L1EM20VH_3j20noL1', 'HLT_e24_lhvloose_L1EM20VH_4j20noL1', 'HLT_e24_lhvloose_L1EM20VH_5j15noL1', 'HLT_e24_lhvloose_L1EM20VH_6j15noL1', 'HLT_e24_lhvloose_nod0_L1EM18VH', 'HLT_e24_lhvloose_nod0_L1EM18VH_3j20noL1', 'HLT_e24_lhvloose_nod0_L1EM18VH_4j20noL1', 'HLT_e24_lhvloose_nod0_L1EM18VH_5j15noL1', 'HLT_e24_lhvloose_nod0_L1EM18VH_6j15noL1', 'HLT_e24_lhvloose_nod0_L1EM20VH', 'HLT_e24_lhvloose_nod0_L1EM20VH_3j20noL1', 'HLT_e24_lhvloose_nod0_L1EM20VH_4j20noL1', 'HLT_e24_lhvloose_nod0_L1EM20VH_5j15noL1', 'HLT_e24_lhvloose_nod0_L1EM20VH_6j15noL1', 'HLT_e24_medium_L1EM15VH', 'HLT_e24_medium_L1EM15VH_g25_medium', 'HLT_e24_medium_L1EM18VH', 'HLT_e24_medium_L1EM20VH', 'HLT_e24_medium_L1EM20VHI', 'HLT_e24_medium_L1EM20VHI_mu8noL1', 'HLT_e24_medium_idperf_L1EM20VH', 'HLT_e24_medium_iloose_L1EM18VH', 'HLT_e24_medium_iloose_L1EM20VH', 'HLT_e24_tight_L1EM20VH', 'HLT_e24_tight_L1EM20VH_e15_etcut_Zee', 'HLT_e24_tight_iloose', 'HLT_e24_tight_iloose_HLTCalo_L1EM20VH', 'HLT_e24_tight_iloose_L1EM20VH', 'HLT_e24_tight_iloose_L2EFCalo_L1EM20VH', 'HLT_e24_vloose_L1EM18VH', 'HLT_e24_vloose_L1EM18VH_3j20noL1', 'HLT_e24_vloose_L1EM18VH_4j20noL1', 'HLT_e24_vloose_L1EM18VH_5j15noL1', 'HLT_e24_vloose_L1EM18VH_6j15noL1', 'HLT_e24_vloose_L1EM20VH', 'HLT_e24_vloose_L1EM20VH_3j20noL1', 'HLT_e24_vloose_L1EM20VH_4j20noL1', 'HLT_e24_vloose_L1EM20VH_5j15noL1', 'HLT_e24_vloose_L1EM20VH_6j15noL1', 'HLT_e25_etcut_L1EM15', 'HLT_e25_lhvloose_L1EM15', 'HLT_e25_vloose_L1EM15', 'HLT_e26_lhmedium_L1EM22VHI', 'HLT_e26_lhmedium_L1EM22VHI_mu8noL1', 'HLT_e26_lhmedium_nod0_L1EM22VHI', 'HLT_e26_lhmedium_nod0_L1EM22VHI_mu8noL1', 'HLT_e26_lhtight_cutd0dphideta_iloose', 'HLT_e26_lhtight_e15_etcut_Zee', 'HLT_e26_lhtight_iloose', 'HLT_e26_lhtight_iloose_2j35_bperf', 'HLT_e26_lhtight_iloose_HLTCalo', 'HLT_e26_lhtight_nod0_iloose', 'HLT_e26_lhtight_nod0_iloose_2j35_bperf', 'HLT_e26_lhtight_smooth_iloose', 'HLT_e26_lhvloose_L1EM20VH', 'HLT_e26_lhvloose_L1EM20VH_3j20noL1', 'HLT_e26_lhvloose_L1EM20VH_4j20noL1', 'HLT_e26_lhvloose_L1EM20VH_5j15noL1', 'HLT_e26_lhvloose_L1EM20VH_6j15noL1', 'HLT_e26_lhvloose_nod0_L1EM20VH', 'HLT_e26_lhvloose_nod0_L1EM20VH_3j20noL1', 'HLT_e26_lhvloose_nod0_L1EM20VH_4j20noL1', 'HLT_e26_lhvloose_nod0_L1EM20VH_5j15noL1', 'HLT_e26_lhvloose_nod0_L1EM20VH_6j15noL1', 'HLT_e26_medium_L1EM22VHI', 'HLT_e26_medium_L1EM22VHI_mu8noL1', 'HLT_e26_tight_e15_etcut_Zee', 'HLT_e26_tight_iloose', 'HLT_e26_tight_iloose_2j35_bperf', 'HLT_e26_vloose_L1EM20VH', 'HLT_e26_vloose_L1EM20VH_3j20noL1', 'HLT_e26_vloose_L1EM20VH_4j20noL1', 'HLT_e26_vloose_L1EM20VH_5j15noL1', 'HLT_e26_vloose_L1EM20VH_6j15noL1', 'HLT_e30_etcut_L1EM15', 'HLT_e30_lhvloose_L1EM15', 'HLT_e30_vloose_L1EM15', 'HLT_e40_etcut_L1EM15', 'HLT_e40_lhvloose', 'HLT_e40_lhvloose_L1EM15', 'HLT_e40_lhvloose_nod0', 'HLT_e40_vloose', 'HLT_e40_vloose_L1EM15', 'HLT_e4_etcut', 'HLT_e50_etcut_L1EM15', 'HLT_e50_lhvloose_L1EM15', 'HLT_e50_vloose_L1EM15', 'HLT_e5_etcut', 'HLT_e5_lhloose', 'HLT_e5_lhloose_idperf', 'HLT_e5_lhtight', 'HLT_e5_lhtight_e4_etcut', 'HLT_e5_lhtight_e4_etcut_Jpsiee', 'HLT_e5_lhtight_idperf', 'HLT_e5_lhtight_nod0', 'HLT_e5_lhtight_nod0_e4_etcut', 'HLT_e5_lhtight_nod0_e4_etcut_Jpsiee', 'HLT_e5_lhvloose', 'HLT_e5_lhvloose_nod0', 'HLT_e5_loose', 'HLT_e5_loose_idperf', 'HLT_e5_tight', 'HLT_e5_tight_e4_etcut', 'HLT_e5_tight_e4_etcut_Jpsiee', 'HLT_e5_tight_idperf', 'HLT_e5_vloose', 'HLT_e60_etcut', 'HLT_e60_lhloose', 'HLT_e60_lhloose_nod0', 'HLT_e60_lhloose_nod0_xe60noL1', 'HLT_e60_lhloose_xe60noL1', 'HLT_e60_lhmedium', 'HLT_e60_lhmedium_HLTCalo', 'HLT_e60_lhmedium_cutd0dphideta', 'HLT_e60_lhmedium_nod0', 'HLT_e60_lhvloose', 'HLT_e60_lhvloose_nod0', 'HLT_e60_loose', 'HLT_e60_loose_xe60noL1', 'HLT_e60_medium', 'HLT_e60_vloose', 'HLT_e70_etcut', 'HLT_e70_lhloose', 'HLT_e70_lhloose_nod0', 'HLT_e70_lhloose_nod0_xe70noL1', 'HLT_e70_lhloose_xe70noL1', 'HLT_e70_lhvloose', 'HLT_e70_loose', 'HLT_e70_loose_xe70noL1', 'HLT_e70_vloose', 'HLT_e7_lhmedium', 'HLT_e7_lhmedium_mu24', 'HLT_e7_lhmedium_nod0', 'HLT_e7_lhmedium_nod0_mu24', 'HLT_e7_medium', 'HLT_e7_medium_mu24', 'HLT_e80_etcut', 'HLT_e80_lhvloose', 'HLT_e80_vloose', 'HLT_e9_etcut', 'HLT_e9_etcut_e5_lhtight_Jpsiee', 'HLT_e9_etcut_e5_lhtight_nod0_Jpsiee', 'HLT_e9_etcut_e5_tight_Jpsiee', 'HLT_e9_lhloose', 'HLT_e9_lhloose_nod0', 'HLT_e9_lhmedium', 'HLT_e9_lhmedium_nod0', 'HLT_e9_lhtight', 'HLT_e9_lhtight_e4_etcut_Jpsiee', 'HLT_e9_lhtight_nod0', 'HLT_e9_lhtight_nod0_e4_etcut_Jpsiee', 'HLT_e9_loose', 'HLT_e9_medium', 'HLT_e9_tight', 'HLT_e9_tight_e4_etcut_Jpsiee', 'HLT_g0_hiptrt_L1EM18VH', 'HLT_g0_hiptrt_L1EM20VH', 'HLT_g0_hiptrt_L1EM20VHI', 'HLT_g0_hiptrt_L1EM22VHI', 'HLT_g0_perf_L1EM15', 'HLT_g100_loose', 'HLT_g10_etcut', 'HLT_g10_etcut_L1EM7_mu10_iloose_taumass', 'HLT_g10_etcut_L1EM7_mu10_taumass', 'HLT_g10_loose', 'HLT_g10_loose_L1EM7_mu10_iloose_taumass', 'HLT_g10_medium', 'HLT_g120_loose', 'HLT_g140_loose', 'HLT_g140_loose_HLTCalo', 'HLT_g15_loose', 'HLT_g15_loose_2mu10_msonly', 'HLT_g15_loose_2mu10_msonly_L1MU4_EMPTY', 'HLT_g15_loose_2mu10_msonly_L1MU4_UNPAIRED_ISO', 'HLT_g15_loose_L1EM3', 'HLT_g15_loose_L1EM7', 'HLT_g200_etcut', 'HLT_g20_etcut_L1EM12', 'HLT_g20_etcut_L1EM15_mu4_iloose_taumass', 'HLT_g20_etcut_L1EM15_mu4_taumass', 'HLT_g20_loose', 'HLT_g20_loose_2g15_loose_L12EM13VH', 'HLT_g20_loose_L1EM12', 'HLT_g20_loose_L1EM15', 'HLT_g20_loose_L1EM15_mu4_iloose_taumass', 'HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490', 'HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490_invm700', 'HLT_g20_tight', 'HLT_g25_loose', 'HLT_g25_loose_2j40_0eta490_3j25_0eta490_invm700', 'HLT_g25_loose_L1EM15', 'HLT_g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700', 'HLT_g25_medium', 'HLT_g25_medium_HLTCalo', 'HLT_g25_medium_mu24', 'HLT_g35_loose', 'HLT_g35_loose_L1EM15', 'HLT_g35_loose_L1EM15_g25_loose_L1EM15', 'HLT_g35_loose_L1EM22VHI_mu15noL1_mu2noL1', 'HLT_g35_loose_L1EM22VHI_mu18noL1', 'HLT_g35_loose_g25_loose', 'HLT_g35_medium', 'HLT_g35_medium_HLTCalo', 'HLT_g35_medium_HLTCalo_g25_medium_HLTCalo', 'HLT_g35_medium_g25_medium', 'HLT_g35_medium_g25_medium_L1EM7_EMPTY', 'HLT_g35_medium_g25_medium_L1EM7_UNPAIRED_ISO', 'HLT_g40_loose_L1EM15', 'HLT_g40_tight', 'HLT_g40_tight_xe40noL1', 'HLT_g45_loose_L1EM15', 'HLT_g45_tight', 'HLT_g45_tight_L1EM22VHI_xe45noL1', 'HLT_g45_tight_xe45noL1', 'HLT_g50_loose', 'HLT_g50_loose_L1EM15', 'HLT_g60_loose', 'HLT_g60_loose_L1EM15VH', 'HLT_g60_loose_xe60noL1', 'HLT_g70_loose', 'HLT_g70_loose_xe70noL1', 'HLT_g80_loose', 'HLT_ht1000_L1J100', 'HLT_ht700_L1J100', 'HLT_ht700_L1J75', 'HLT_ht850_L1J100', 'HLT_ht850_L1J75', 'HLT_j0_perf_L1RD0_FILLED', 'HLT_j0_perf_ds1_L1J100', 'HLT_j0_perf_ds1_L1J75', 'HLT_j100', 'HLT_j100_2j55_bmedium', 'HLT_j100_2j55_bmedium_split', 'HLT_j100_xe80', 'HLT_j110', 'HLT_j110_320eta490', 'HLT_j110_boffperf', 'HLT_j110_boffperf_split', 'HLT_j110_bperf', 'HLT_j110_bperf_split', 'HLT_j15', 'HLT_j150', 'HLT_j150_bmedium_j50_bmedium', 'HLT_j150_bmedium_split_j50_bmedium_split', 'HLT_j150_boffperf', 'HLT_j150_boffperf_j50_boffperf', 'HLT_j150_boffperf_split', 'HLT_j150_bperf', 'HLT_j150_bperf_split', 'HLT_j15_320eta490', 'HLT_j15_boffperf', 'HLT_j15_boffperf_split', 'HLT_j15_bperf', 'HLT_j15_bperf_split', 'HLT_j15_j15_320eta490', 'HLT_j175', 'HLT_j175_320eta490', 'HLT_j175_320eta490_jes', 'HLT_j175_320eta490_lcw', 'HLT_j175_320eta490_lcw_jes', 'HLT_j175_320eta490_lcw_nojcalib', 'HLT_j175_320eta490_nojcalib', 'HLT_j175_bmedium', 'HLT_j175_bmedium_j60_bmedium', 'HLT_j175_bmedium_split', 'HLT_j175_bmedium_split_j60_bmedium_split', 'HLT_j175_bmv2c2085', 'HLT_j175_bmv2c2085_split', 'HLT_j175_boffperf', 'HLT_j175_jes', 'HLT_j175_lcw', 'HLT_j175_lcw_jes', 'HLT_j175_lcw_nojcalib', 'HLT_j175_nojcalib', 'HLT_j200', 'HLT_j225_bloose', 'HLT_j225_bloose_split', 'HLT_j225_boffperf', 'HLT_j25', 'HLT_j25_320eta490', 'HLT_j25_boffperf', 'HLT_j25_boffperf_split', 'HLT_j25_bperf', 'HLT_j25_bperf_split', 'HLT_j25_j25_320eta490', 'HLT_j260', 'HLT_j260_320eta490', 'HLT_j260_320eta490_jes', 'HLT_j260_320eta490_lcw', 'HLT_j260_320eta490_lcw_jes', 'HLT_j260_320eta490_lcw_nojcalib', 'HLT_j260_320eta490_nojcalib', 'HLT_j260_boffperf', 'HLT_j260_boffperf_split', 'HLT_j260_bperf', 'HLT_j260_bperf_split', 'HLT_j300', 'HLT_j300_bloose', 'HLT_j300_bloose_split', 'HLT_j300_lcw_nojcalib', 'HLT_j30_jes_PS_llp_L1TAU30', 'HLT_j30_jes_PS_llp_L1TAU40', 'HLT_j30_jes_PS_llp_L1TAU60', 'HLT_j30_jes_PS_llp_L1TAU8_EMPTY', 'HLT_j30_jes_PS_llp_L1TAU8_UNPAIRED_ISO', 'HLT_j30_jes_PS_llp_noiso_L1TAU60', 'HLT_j30_muvtx', 'HLT_j30_muvtx_L1MU4_EMPTY', 'HLT_j30_muvtx_L1MU4_UNPAIRED_ISO', 'HLT_j30_muvtx_noiso', 'HLT_j30_xe10_razor100', 'HLT_j30_xe10_razor170', 'HLT_j30_xe10_razor185', 'HLT_j30_xe10_razor195', 'HLT_j30_xe60_razor100', 'HLT_j30_xe60_razor170', 'HLT_j30_xe60_razor185', 'HLT_j30_xe60_razor195', 'HLT_j320', 'HLT_j320_boffperf', 'HLT_j320_boffperf_split', 'HLT_j320_bperf', 'HLT_j320_bperf_split', 'HLT_j35', 'HLT_j35_320eta490', 'HLT_j35_boffperf', 'HLT_j35_boffperf_split', 'HLT_j35_bperf', 'HLT_j35_bperf_split', 'HLT_j35_j35_320eta490', 'HLT_j35_jes', 'HLT_j35_lcw', 'HLT_j35_lcw_jes', 'HLT_j35_lcw_nojcalib', 'HLT_j35_nojcalib', 'HLT_j360', 'HLT_j360_320eta490', 'HLT_j360_320eta490_jes', 'HLT_j360_320eta490_lcw', 'HLT_j360_320eta490_lcw_jes', 'HLT_j360_320eta490_lcw_nojcalib', 'HLT_j360_320eta490_nojcalib', 'HLT_j360_a10_lcw_sub_L1J100', 'HLT_j360_a10_sub_L1J100', 'HLT_j360_a10r_L1J100', 'HLT_j380', 'HLT_j380_jes', 'HLT_j380_lcw', 'HLT_j380_lcw_jes', 'HLT_j380_lcw_nojcalib', 'HLT_j380_nojcalib', 'HLT_j400', 'HLT_j400_boffperf', 'HLT_j400_boffperf_split', 'HLT_j400_bperf', 'HLT_j400_bperf_split', 'HLT_j400_jes', 'HLT_j400_lcw', 'HLT_j400_lcw_jes', 'HLT_j400_nojcalib', 'HLT_j400_sub', 'HLT_j420', 'HLT_j420_jes', 'HLT_j420_lcw', 'HLT_j420_lcw_jes', 'HLT_j420_lcw_nojcalib', 'HLT_j420_nojcalib', 'HLT_j440', 'HLT_j45', 'HLT_j45_320eta490', 'HLT_j45_L1RD0_FILLED', 'HLT_j45_boffperf', 'HLT_j45_boffperf_3j45', 'HLT_j45_boffperf_3j45_L13J15.0ETA25', 'HLT_j45_boffperf_3j45_L13J20', 'HLT_j45_boffperf_split', 'HLT_j45_boffperf_split_3j45', 'HLT_j45_boffperf_split_3j45_L13J15.0ETA25', 'HLT_j45_boffperf_split_3j45_L13J20', 'HLT_j45_bperf', 'HLT_j45_bperf_3j45', 'HLT_j45_bperf_3j45_L13J15.0ETA25', 'HLT_j45_bperf_3j45_L13J20', 'HLT_j45_bperf_3j45_L14J20', 'HLT_j45_bperf_split', 'HLT_j45_bperf_split_3j45', 'HLT_j45_bperf_split_3j45_L13J15.0ETA25', 'HLT_j45_bperf_split_3j45_L13J20', 'HLT_j45_j45_320eta490', 'HLT_j460', 'HLT_j460_a10_lcw_nojcalib_L1J100', 'HLT_j460_a10_lcw_sub_L1J100', 'HLT_j460_a10_nojcalib_L1J100', 'HLT_j460_a10_sub_L1J100', 'HLT_j460_a10r_L1J100', 'HLT_j55', 'HLT_j55_0eta240_xe50_L1J30_EMPTY', 'HLT_j55_320eta490', 'HLT_j55_L1RD0_FILLED', 'HLT_j55_boffperf', 'HLT_j55_boffperf_split', 'HLT_j55_bperf', 'HLT_j55_bperf_3j55', 'HLT_j55_bperf_split', 'HLT_j55_j55_320eta490', 'HLT_j55_j55_320eta490_L1RD0_FILLED', 'HLT_j60', 'HLT_j60_280eta320', 'HLT_j60_320eta490', 'HLT_j60_L1RD0_FILLED', 'HLT_j60_j60_320eta490', 'HLT_j65_boffperf_3j65_L13J25.0ETA23', 'HLT_j65_boffperf_3j65_L14J15.0ETA25', 'HLT_j65_btight_3j65_L13J25.0ETA23', 'HLT_j65_btight_3j65_L14J15.0ETA25', 'HLT_j65_btight_split_3j65_L13J25.0ETA23', 'HLT_j65_btight_split_3j65_L14J15.0ETA25', 'HLT_j70_bmedium_3j70_L13J25.0ETA23', 'HLT_j70_bmedium_3j70_L14J15.0ETA25', 'HLT_j70_bmedium_split_3j70_L13J25.0ETA23', 'HLT_j70_bmedium_split_3j70_L14J15.0ETA25', 'HLT_j70_boffperf_3j70_L13J25.0ETA23', 'HLT_j70_boffperf_3j70_L14J15.0ETA25', 'HLT_j70_btight_3j70', 'HLT_j70_btight_3j70_L13J25.ETA23', 'HLT_j70_btight_split_3j70', 'HLT_j70_btight_split_3j70_L13J25.ETA23', 'HLT_j75_bmedium_3j75', 'HLT_j75_bmedium_3j75_L13J25.ETA23', 'HLT_j75_bmedium_split_3j75', 'HLT_j75_bmedium_split_3j75_L13J25.ETA23', 'HLT_j80_xe80', 'HLT_j85', 'HLT_j85_280eta320', 'HLT_j85_280eta320_jes', 'HLT_j85_280eta320_lcw', 'HLT_j85_280eta320_lcw_jes', 'HLT_j85_280eta320_lcw_nojcalib', 'HLT_j85_280eta320_nojcalib', 'HLT_j85_320eta490', 'HLT_j85_L1RD0_FILLED', 'HLT_j85_boffperf', 'HLT_j85_boffperf_split', 'HLT_j85_bperf', 'HLT_j85_bperf_split', 'HLT_j85_j85_320eta490', 'HLT_j85_jes', 'HLT_j85_lcw', 'HLT_j85_lcw_jes', 'HLT_j85_lcw_nojcalib', 'HLT_j85_nojcalib', 'HLT_mu0_perf', 'HLT_mu10', 'HLT_mu10_bJpsi_Trkloose', 'HLT_mu10_idperf', 'HLT_mu10_msonly', 'HLT_mu10_mu10_l2msonly_bDimu_noL2', 'HLT_mu10_mu10_l2msonly_bJpsimumu_noL2', 'HLT_mu10_mu6_bBmumux_BcmumuDsloose', 'HLT_mu10_mu6_bBmumux_BpmumuKp', 'HLT_mu10_mu6_bBmumuxv2', 'HLT_mu11', 'HLT_mu11_L1MU10_2mu4noL1_nscan03_L1MU10_2MU6', 'HLT_mu11_2mu4noL1_nscan03_L1MU11_2MU6', 'HLT_mu11_l2msonly_L1MU10_2mu4noL1_nscan03_l2msonly_L1MU10_2MU6', 'HLT_mu11_l2msonly_2mu4noL1_nscan03_l2msonly_L1MU11_2MU6', 'HLT_mu13_mu13_idperf_Zmumu', 'HLT_mu14', 'HLT_mu14_iloose', 'HLT_mu14_iloose_tau25_medium1_tracktwo', 'HLT_mu14_iloose_tau25_medium1_tracktwo_xe50', 'HLT_mu14_iloose_tau25_perf_tracktwo', 'HLT_mu14_iloose_tau35_medium1_tracktwo', 'HLT_mu14_tau25_medium1_tracktwo', 'HLT_mu14_tau25_medium1_tracktwo_xe50', 'HLT_mu14_tau35_medium1_tracktwo', 'HLT_mu14_tau35_medium1_tracktwo_L1TAU20', 'HLT_mu18', 'HLT_mu18_2mu0noL1_JpsimumuFS', 'HLT_mu18_2mu4_JpsimumuL2', 'HLT_mu18_2mu4noL1', 'HLT_mu18_bJpsi_Trkloose', 'HLT_mu18_mu8noL1', 'HLT_mu20', 'HLT_mu20_2mu0noL1_JpsimumuFS', 'HLT_mu20_2mu4_JpsimumuL2', 'HLT_mu20_2mu4noL1', 'HLT_mu20_L1MU15', 'HLT_mu20_L1MU15_2j20noL1', 'HLT_mu20_L1MU15_3j20noL1', 'HLT_mu20_L1MU15_4j15noL1', 'HLT_mu20_L1MU15_5j15noL1', 'HLT_mu20_idperf', 'HLT_mu20_iloose_L1MU15', 'HLT_mu20_l2msonly_mu6noL1_nscan03_l2msonly', 'HLT_mu20_msonly', 'HLT_mu20_msonly_mu6noL1_msonly_nscan05', 'HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_EMPTY', 'HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO', 'HLT_mu20_mu6noL1_nscan03', 'HLT_mu20_mu8noL1', 'HLT_mu22', 'HLT_mu22_2mu4noL1', 'HLT_mu22_mu8noL1', 'HLT_mu24', 'HLT_mu24_2j20noL1', 'HLT_mu24_2mu4noL1', 'HLT_mu24_3j20noL1', 'HLT_mu24_4j15noL1', 'HLT_mu24_5j15noL1', 'HLT_mu24_L1MU15', 'HLT_mu24_idperf', 'HLT_mu24_iloose_L1MU15', 'HLT_mu24_imedium', 'HLT_mu24_mu8noL1', 'HLT_mu26', 'HLT_mu26_2j20noL1', 'HLT_mu26_3j20noL1', 'HLT_mu26_4j15noL1', 'HLT_mu26_5j15noL1', 'HLT_mu26_imedium', 'HLT_mu26_imedium_2j35_bperf', 'HLT_mu4', 'HLT_mu40', 'HLT_mu4_3j45_dr05_L1MU4_3J15', 'HLT_mu4_3j45_dr05_L1MU4_3J20', 'HLT_mu4_4j40_dr05_L1MU4_3J15', 'HLT_mu4_4j40_dr05_L1MU4_3J20', 'HLT_mu4_bJpsi_Trkloose', 'HLT_mu4_idperf', 'HLT_mu4_iloose_mu4_11invm60_noos', 'HLT_mu4_iloose_mu4_11invm60_noos_novtx', 'HLT_mu4_iloose_mu4_7invm9_noos', 'HLT_mu4_iloose_mu4_7invm9_noos_novtx', 'HLT_mu4_j15_bperf_split_dr05_dz02', 'HLT_mu4_j15_dr05', 'HLT_mu4_j25_bperf_split_dr05_dz02', 'HLT_mu4_j25_dr05', 'HLT_mu4_j35_bperf_split_dr05_dz02', 'HLT_mu4_j35_dr05', 'HLT_mu4_j40_dr05_3j40_L14J20', 'HLT_mu4_j55_bperf_split_dr05_dz02', 'HLT_mu4_j55_dr05', 'HLT_mu4_j60_dr05_2j35_L1MU4_3J15', 'HLT_mu4_j60_dr05_2j35_L1MU4_3J20', 'HLT_mu4_j70_dr05_L1MU4_J30', 'HLT_mu4_j70_dr05_L1MU4_J50', 'HLT_mu4_msonly', 'HLT_mu4_mu4_idperf_bJpsimumu_noid', 'HLT_mu4_mu4_l2msonly_bDimu_noL2', 'HLT_mu4_mu4_l2msonly_bJpsimumu_noL2', 'HLT_mu50', 'HLT_mu6', 'HLT_mu60_0eta105_msonly', 'HLT_mu6_2j40_0eta490_invm1000_L1MU6_J30.0ETA49_2J20.0ETA49', 'HLT_mu6_2j40_0eta490_invm400_L1MU6_J30.0ETA49_2J20.0ETA49', 'HLT_mu6_2j40_0eta490_invm600_L1MU6_J30.0ETA49_2J20.0ETA49', 'HLT_mu6_2j40_0eta490_invm800_L1MU6_J30.0ETA49_2J20.0ETA49', 'HLT_mu6_bJpsi_Trkloose', 'HLT_mu6_idperf', 'HLT_mu6_iloose_mu6_11invm24_noos', 'HLT_mu6_iloose_mu6_11invm24_noos_novtx', 'HLT_mu6_iloose_mu6_24invm60_noos', 'HLT_mu6_iloose_mu6_24invm60_noos_novtx', 'HLT_mu6_j110_bperf_split_dr05_dz02', 'HLT_mu6_j110_dr05', 'HLT_mu6_j150_bperf_split_dr05_dz02', 'HLT_mu6_j150_dr05', 'HLT_mu6_j175_bperf_split_dr05_dz02', 'HLT_mu6_j175_dr05', 'HLT_mu6_j260_bperf_split_dr05_dz02', 'HLT_mu6_j260_dr05', 'HLT_mu6_j320_bperf_split_dr05_dz02', 'HLT_mu6_j320_dr05', 'HLT_mu6_j400_bperf_split_dr05_dz02', 'HLT_mu6_j400_dr05', 'HLT_mu6_j50_dr05_2j35_L1MU4_3J15', 'HLT_mu6_j50_dr05_2j35_L1MU4_3J20', 'HLT_mu6_j60_dr05_L1MU6_J20', 'HLT_mu6_j60_dr05_L1MU6_J40', 'HLT_mu6_j85_bperf_split_dr05_dz02', 'HLT_mu6_j85_dr05', 'HLT_mu6_l2msonly_2mu4_l2msonly_L1MU6_3MU4', 'HLT_mu6_l2msonly_mu4_bDimu_noL2', 'HLT_mu6_l2msonly_mu4_bJpsimumu_noL2', 'HLT_mu6_l2msonly_mu4_l2msonly_bDimu_noL2', 'HLT_mu6_l2msonly_mu4_l2msonly_bJpsimumu_noL2', 'HLT_mu6_msonly', 'HLT_mu6_mu4_bBmumu', 'HLT_mu6_mu4_bBmumux_BcmumuDsloose', 'HLT_mu6_mu4_bBmumux_BpmumuKp', 'HLT_mu6_mu4_bBmumuxv2', 'HLT_mu6_mu4_bDimu', 'HLT_mu6_mu4_bDimu_noL2', 'HLT_mu6_mu4_bDimu_noinvm_novtx_ss', 'HLT_mu6_mu4_bDimu_novtx_noos', 'HLT_mu6_mu4_bJpsimumu', 'HLT_mu6_mu4_bJpsimumu_noL2', 'HLT_mu6_mu4_bUpsimumu', 'HLT_mu6_mu4_l2msonly_bDimu_noL2', 'HLT_mu6_mu4_l2msonly_bJpsimumu_noL2', 'HLT_mu6_mu6_l2msonly_bDimu_noL2', 'HLT_mu6_mu6_l2msonly_bJpsimumu_noL2', 'HLT_noalg_L1J400', 'HLT_noalg_L1MU10', 'HLT_noalg_L1MU11_EMPTY', 'HLT_noalg_L1MU15', 'HLT_noalg_L1MU20', 'HLT_noalg_L1MU20_FIRSTEMPTY', 'HLT_noalg_L1MU4_EMPTY', 'HLT_noalg_L1MU4_FIRSTEMPTY', 'HLT_noalg_L1MU4_UNPAIRED_ISO', 'HLT_noalg_L1RD0_EMPTY', 'HLT_noalg_L1RD0_FILLED', 'HLT_noalg_L1XE35', 'HLT_tau0_perf_ptonly_L1TAU12', 'HLT_tau0_perf_ptonly_L1TAU60', 'HLT_tau125_medium1_track', 'HLT_tau125_medium1_tracktwo', 'HLT_tau125_medium1_tracktwo_tau50_medium1_tracktwo_L1TAU12', 'HLT_tau125_perf_ptonly', 'HLT_tau125_perf_tracktwo', 'HLT_tau160_idperf_track', 'HLT_tau160_idperf_tracktwo', 'HLT_tau160_medium1_tracktwo', 'HLT_tau20_r1_idperf', 'HLT_tau25_idperf_track', 'HLT_tau25_idperf_tracktwo', 'HLT_tau25_loose1_ptonly', 'HLT_tau25_loose1_tracktwo', 'HLT_tau25_medium1_mvonly', 'HLT_tau25_medium1_ptonly', 'HLT_tau25_medium1_track', 'HLT_tau25_medium1_tracktwo', 'HLT_tau25_medium1_tracktwo_L1TAU12', 'HLT_tau25_medium1_tracktwo_L1TAU12IL', 'HLT_tau25_medium1_tracktwo_L1TAU12IT', 'HLT_tau25_perf_ptonly', 'HLT_tau25_perf_ptonly_L1TAU12', 'HLT_tau25_perf_track', 'HLT_tau25_perf_track_L1TAU12', 'HLT_tau25_perf_tracktwo', 'HLT_tau25_perf_tracktwo_L1TAU12', 'HLT_tau25_r1_idperf', 'HLT_tau25_tight1_ptonly', 'HLT_tau25_tight1_tracktwo', 'HLT_tau35_loose1_ptonly', 'HLT_tau35_loose1_tracktwo', 'HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo', 'HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1TAU20IM_2TAU12IM', 'HLT_tau35_medium1_ptonly', 'HLT_tau35_medium1_ptonly_L1TAU20', 'HLT_tau35_medium1_track', 'HLT_tau35_medium1_tracktwo', 'HLT_tau35_medium1_tracktwo_L1TAU20', 'HLT_tau35_medium1_tracktwo_L1TAU20IL', 'HLT_tau35_medium1_tracktwo_L1TAU20IT', 'HLT_tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12', 'HLT_tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_xe50', 'HLT_tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45', 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo', 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM', 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_xe50', 'HLT_tau35_medium1_tracktwo_xe70_L1XE45', 'HLT_tau35_perf_ptonly', 'HLT_tau35_perf_tracktwo', 'HLT_tau35_perf_tracktwo_tau25_perf_tracktwo', 'HLT_tau35_tight1_ptonly', 'HLT_tau35_tight1_tracktwo', 'HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo', 'HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1TAU20IM_2TAU12IM', 'HLT_tau50_medium1_tracktwo_L1TAU12', 'HLT_tau5_perf_ptonly_L1TAU8', 'HLT_tau80_medium1_track', 'HLT_tau80_medium1_tracktwo', 'HLT_tau80_medium1_tracktwo_L1TAU60', 'HLT_tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12', 'HLT_xe0noL1_l2fsperf', 'HLT_xe0noL1_l2fsperf_mht', 'HLT_xe0noL1_l2fsperf_pueta', 'HLT_xe0noL1_l2fsperf_pufit', 'HLT_xe0noL1_l2fsperf_tc_em', 'HLT_xe0noL1_l2fsperf_tc_lcw', 'HLT_xe100', 'HLT_xe100_L1XE70_BGRP7', 'HLT_xe100_L1XE80', 'HLT_xe100_L1XE80_BGRP7', 'HLT_xe100_mht', 'HLT_xe100_mht_L1XE70_BGRP7', 'HLT_xe100_mht_L1XE80', 'HLT_xe100_mht_L1XE80_BGRP7', 'HLT_xe100_mht_wEFMu', 'HLT_xe100_mht_wEFMu_L1XE70_BGRP7', 'HLT_xe100_mht_wEFMu_L1XE80', 'HLT_xe100_mht_wEFMu_L1XE80_BGRP7', 'HLT_xe100_pueta', 'HLT_xe100_pueta_L1XE70_BGRP7', 'HLT_xe100_pueta_L1XE80', 'HLT_xe100_pueta_L1XE80_BGRP7', 'HLT_xe100_pueta_wEFMu', 'HLT_xe100_pueta_wEFMu_L1XE70_BGRP7', 'HLT_xe100_pueta_wEFMu_L1XE80', 'HLT_xe100_pueta_wEFMu_L1XE80_BGRP7', 'HLT_xe100_pufit', 'HLT_xe100_pufit_L1XE70_BGRP7', 'HLT_xe100_pufit_L1XE80', 'HLT_xe100_pufit_L1XE80_BGRP7', 'HLT_xe100_pufit_wEFMu', 'HLT_xe100_pufit_wEFMu_L1XE70_BGRP7', 'HLT_xe100_pufit_wEFMu_L1XE80', 'HLT_xe100_pufit_wEFMu_L1XE80_BGRP7', 'HLT_xe100_tc_em', 'HLT_xe100_tc_em_L1XE70_BGRP7', 'HLT_xe100_tc_em_L1XE80', 'HLT_xe100_tc_em_L1XE80_BGRP7', 'HLT_xe100_tc_em_wEFMu', 'HLT_xe100_tc_em_wEFMu_L1XE70_BGRP7', 'HLT_xe100_tc_em_wEFMu_L1XE80', 'HLT_xe100_tc_em_wEFMu_L1XE80_BGRP7', 'HLT_xe100_tc_lcw', 'HLT_xe100_tc_lcw_L1XE70_BGRP7', 'HLT_xe100_tc_lcw_L1XE80', 'HLT_xe100_tc_lcw_L1XE80_BGRP7', 'HLT_xe100_tc_lcw_wEFMu', 'HLT_xe100_tc_lcw_wEFMu_L1XE70_BGRP7', 'HLT_xe100_tc_lcw_wEFMu_L1XE80', 'HLT_xe100_tc_lcw_wEFMu_L1XE80_BGRP7', 'HLT_xe100_wEFMu', 'HLT_xe100_wEFMu_L1XE70_BGRP7', 'HLT_xe100_wEFMu_L1XE80', 'HLT_xe100_wEFMu_L1XE80_BGRP7', 'HLT_xe120', 'HLT_xe120_L1XE80_BGRP7', 'HLT_xe120_mht', 'HLT_xe120_mht_L1XE80_BGRP7', 'HLT_xe120_mht_wEFMu', 'HLT_xe120_mht_wEFMu_L1XE80_BGRP7', 'HLT_xe120_pueta', 'HLT_xe120_pueta_L1XE80_BGRP7', 'HLT_xe120_pueta_wEFMu', 'HLT_xe120_pueta_wEFMu_L1XE80_BGRP7', 'HLT_xe120_pufit', 'HLT_xe120_pufit_L1XE80_BGRP7', 'HLT_xe120_pufit_wEFMu', 'HLT_xe120_pufit_wEFMu_L1XE80_BGRP7', 'HLT_xe120_tc_em', 'HLT_xe120_tc_em_L1XE80_BGRP7', 'HLT_xe120_tc_em_wEFMu', 'HLT_xe120_tc_em_wEFMu_L1XE80_BGRP7', 'HLT_xe120_tc_lcw', 'HLT_xe120_tc_lcw_L1XE80_BGRP7', 'HLT_xe120_tc_lcw_wEFMu', 'HLT_xe120_tc_lcw_wEFMu_L1XE80_BGRP7', 'HLT_xe120_wEFMu', 'HLT_xe120_wEFMu_L1XE80_BGRP7', 'HLT_xe35', 'HLT_xe35_L1XE35_BGRP7', 'HLT_xe35_mht', 'HLT_xe35_mht_L1XE35_BGRP7', 'HLT_xe35_mht_wEFMu', 'HLT_xe35_mht_wEFMu_L1XE35_BGRP7', 'HLT_xe35_pueta', 'HLT_xe35_pueta_L1XE35_BGRP7', 'HLT_xe35_pueta_wEFMu', 'HLT_xe35_pueta_wEFMu_L1XE35_BGRP7', 'HLT_xe35_pufit', 'HLT_xe35_pufit_L1XE35_BGRP7', 'HLT_xe35_pufit_wEFMu', 'HLT_xe35_pufit_wEFMu_L1XE35_BGRP7', 'HLT_xe35_tc_em', 'HLT_xe35_tc_em_L1XE35_BGRP7', 'HLT_xe35_tc_em_wEFMu', 'HLT_xe35_tc_em_wEFMu_L1XE35_BGRP7', 'HLT_xe35_tc_lcw', 'HLT_xe35_tc_lcw_L1XE35_BGRP7', 'HLT_xe35_tc_lcw_wEFMu', 'HLT_xe35_tc_lcw_wEFMu_L1XE35_BGRP7', 'HLT_xe35_wEFMu', 'HLT_xe35_wEFMu_L1XE35_BGRP7', 'HLT_xe50', 'HLT_xe50_L1XE35_BGRP7', 'HLT_xe50_mht', 'HLT_xe50_mht_L1XE35_BGRP7', 'HLT_xe50_pueta', 'HLT_xe50_pueta_L1XE35_BGRP7', 'HLT_xe50_pufit', 'HLT_xe50_pufit_L1XE35_BGRP7', 'HLT_xe50_tc_em', 'HLT_xe50_tc_em_L1XE35_BGRP7', 'HLT_xe50_tc_lcw', 'HLT_xe50_tc_lcw_L1XE35_BGRP7', 'HLT_xe60', 'HLT_xe60_L1XE40_BGRP7', 'HLT_xe60_mht', 'HLT_xe60_mht_L1XE40_BGRP7', 'HLT_xe60_mht_wEFMu', 'HLT_xe60_mht_wEFMu_L1XE40_BGRP7', 'HLT_xe60_pueta', 'HLT_xe60_pueta_L1XE40_BGRP7', 'HLT_xe60_pueta_wEFMu', 'HLT_xe60_pueta_wEFMu_L1XE40_BGRP7', 'HLT_xe60_pufit', 'HLT_xe60_pufit_L1XE40_BGRP7', 'HLT_xe60_pufit_wEFMu', 'HLT_xe60_pufit_wEFMu_L1XE40_BGRP7', 'HLT_xe60_tc_em', 'HLT_xe60_tc_em_L1XE40_BGRP7', 'HLT_xe60_tc_em_wEFMu', 'HLT_xe60_tc_em_wEFMu_L1XE40_BGRP7', 'HLT_xe60_tc_lcw', 'HLT_xe60_tc_lcw_L1XE40_BGRP7', 'HLT_xe60_tc_lcw_wEFMu', 'HLT_xe60_tc_lcw_wEFMu_L1XE40_BGRP7', 'HLT_xe60_wEFMu', 'HLT_xe60_wEFMu_L1XE40_BGRP7', 'HLT_xe70', 'HLT_xe70_L1XE45', 'HLT_xe70_L1XE50_BGRP7', 'HLT_xe70_mht', 'HLT_xe70_mht_L1XE50_BGRP7', 'HLT_xe70_mht_wEFMu', 'HLT_xe70_mht_wEFMu_L1XE50_BGRP7', 'HLT_xe70_pueta', 'HLT_xe70_pueta_L1XE50_BGRP7', 'HLT_xe70_pueta_wEFMu', 'HLT_xe70_pueta_wEFMu_L1XE50_BGRP7', 'HLT_xe70_pufit', 'HLT_xe70_pufit_L1XE50_BGRP7', 'HLT_xe70_pufit_wEFMu', 'HLT_xe70_pufit_wEFMu_L1XE50_BGRP7', 'HLT_xe70_tc_em', 'HLT_xe70_tc_em_L1XE50_BGRP7', 'HLT_xe70_tc_em_wEFMu', 'HLT_xe70_tc_em_wEFMu_L1XE50_BGRP7', 'HLT_xe70_tc_lcw', 'HLT_xe70_tc_lcw_L1XE50_BGRP7', 'HLT_xe70_tc_lcw_wEFMu', 'HLT_xe70_tc_lcw_wEFMu_L1XE50_BGRP7', 'HLT_xe70_wEFMu', 'HLT_xe70_wEFMu_L1XE50_BGRP7', 'HLT_xe80', 'HLT_xe80_L1XE50', 'HLT_xe80_L1XE50_BGRP7', 'HLT_xe80_L1XE60_BGRP7', 'HLT_xe80_L1XE70', 'HLT_xe80_L1XE70_BGRP7', 'HLT_xe80_mht', 'HLT_xe80_mht_L1XE50', 'HLT_xe80_mht_L1XE50_BGRP7', 'HLT_xe80_mht_L1XE60_BGRP7', 'HLT_xe80_mht_L1XE70', 'HLT_xe80_mht_L1XE70_BGRP7', 'HLT_xe80_mht_wEFMu', 'HLT_xe80_mht_wEFMu_L1XE50', 'HLT_xe80_mht_wEFMu_L1XE50_BGRP7', 'HLT_xe80_mht_wEFMu_L1XE60_BGRP7', 'HLT_xe80_mht_wEFMu_L1XE70', 'HLT_xe80_mht_wEFMu_L1XE70_BGRP7', 'HLT_xe80_pueta', 'HLT_xe80_pueta_L1XE50', 'HLT_xe80_pueta_L1XE50_BGRP7', 'HLT_xe80_pueta_L1XE60_BGRP7', 'HLT_xe80_pueta_L1XE70', 'HLT_xe80_pueta_L1XE70_BGRP7', 'HLT_xe80_pueta_wEFMu', 'HLT_xe80_pueta_wEFMu_L1XE50', 'HLT_xe80_pueta_wEFMu_L1XE50_BGRP7', 'HLT_xe80_pueta_wEFMu_L1XE60_BGRP7', 'HLT_xe80_pueta_wEFMu_L1XE70', 'HLT_xe80_pueta_wEFMu_L1XE70_BGRP7', 'HLT_xe80_pufit', 'HLT_xe80_pufit_L1XE50', 'HLT_xe80_pufit_L1XE50_BGRP7', 'HLT_xe80_pufit_L1XE60_BGRP7', 'HLT_xe80_pufit_L1XE70', 'HLT_xe80_pufit_L1XE70_BGRP7', 'HLT_xe80_pufit_wEFMu', 'HLT_xe80_pufit_wEFMu_L1XE50', 'HLT_xe80_pufit_wEFMu_L1XE50_BGRP7', 'HLT_xe80_pufit_wEFMu_L1XE60_BGRP7', 'HLT_xe80_pufit_wEFMu_L1XE70', 'HLT_xe80_pufit_wEFMu_L1XE70_BGRP7', 'HLT_xe80_tc_em', 'HLT_xe80_tc_em_L1XE50', 'HLT_xe80_tc_em_L1XE50_BGRP7', 'HLT_xe80_tc_em_L1XE60_BGRP7', 'HLT_xe80_tc_em_L1XE70', 'HLT_xe80_tc_em_L1XE70_BGRP7', 'HLT_xe80_tc_em_wEFMu', 'HLT_xe80_tc_em_wEFMu_L1XE50', 'HLT_xe80_tc_em_wEFMu_L1XE50_BGRP7', 'HLT_xe80_tc_em_wEFMu_L1XE60_BGRP7', 'HLT_xe80_tc_em_wEFMu_L1XE70', 'HLT_xe80_tc_em_wEFMu_L1XE70_BGRP7', 'HLT_xe80_tc_lcw', 'HLT_xe80_tc_lcw_L1XE50', 'HLT_xe80_tc_lcw_L1XE50_BGRP7', 'HLT_xe80_tc_lcw_L1XE60_BGRP7', 'HLT_xe80_tc_lcw_L1XE70', 'HLT_xe80_tc_lcw_L1XE70_BGRP7', 'HLT_xe80_tc_lcw_wEFMu', 'HLT_xe80_tc_lcw_wEFMu_L1XE50', 'HLT_xe80_tc_lcw_wEFMu_L1XE50_BGRP7', 'HLT_xe80_tc_lcw_wEFMu_L1XE60_BGRP7', 'HLT_xe80_tc_lcw_wEFMu_L1XE70', 'HLT_xe80_tc_lcw_wEFMu_L1XE70_BGRP7', 'HLT_xe80_wEFMu', 'HLT_xe80_wEFMu_L1XE50', 'HLT_xe80_wEFMu_L1XE50_BGRP7', 'HLT_xe80_wEFMu_L1XE60_BGRP7', 'HLT_xe80_wEFMu_L1XE70', 'HLT_xe80_wEFMu_L1XE70_BGRP7', 'HLT_xs15_L1XS20', 'HLT_xs20_L1XS30',]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# signatures, EB

HLT_list=['HLT_mu6_iloose_mu6_11invm60_noos', 'HLT_mu6_iloose_mu6_11invm60_noos_novtx', 'HLT_2e12_loose1_L12EM10VH', 'HLT_2e17_loose1', 'HLT_2mu4_bBmumuxv3', 'HLT_2mu4_wOvlpRm_bBmumuxv2', 'HLT_2mu4_wOvlpRm_bDimu', 'HLT_2mu4_wOvlpRm_bDimu_noinvm_novtx_ss', 'HLT_2mu4_wOvlpRm_bDimu_novtx_noos', 'HLT_3j175_jes_PS', 'HLT_3mu6_wOvlpRm_bDimu', 'HLT_4j45_test1', 'HLT_4j45_test2', 'HLT_e5_loose_L2Star_idperf', 'HLT_e0_L2Star_perf_L1EM15', 'HLT_e0_perf_L1EM3_EMPTY', 'HLT_e120_loose1', 'HLT_e17_loose_L1EM15VHJJ1523ETA49', 'HLT_e17_lhloose_L1EM15VHJJ1523ETA49', 'HLT_e17_etcut_L2StarA_L1EM15', 'HLT_e17_etcut_L2StarB_L1EM15', 'HLT_e17_etcut_L2StarC_L1EM15', 'HLT_e24_lhmedium_iloose_L2StarA_L1EM20VH', 'HLT_e24_medium1_L1EM18VH', 'HLT_e24_medium1_L2Star_idperf_L1EM18VH', 'HLT_e24_medium1_idperf_L1EM18VH', 'HLT_e24_medium1_iloose_L1EM18VH', 'HLT_e24_medium1_iloose_L2StarA_L1EM18VH', 'HLT_e24_medium_iloose_L2StarA_L1EM20VH', 'HLT_e24_tight1_iloose', 'HLT_e24_tight1_iloose_L1EM20VH', 'HLT_e26_tight1_iloose', 'HLT_e5_lhloose_L2StarA', 'HLT_e5_loose1_L2Star_idperf', 'HLT_e5_loose1_idperf', 'HLT_e5_loose_L2StarA', 'HLT_e5_tight1_e4_etcut', 'HLT_e5_tight1_e4_etcut_Jpsiee', 'HLT_e5_tight1_e4_etcut_Jpsiee_L1JPSI-1M5', 'HLT_e5_tight1_e4_etcut_L1JPSI-1M5', 'HLT_e60_medium1', 'HLT_eb_empty_L1RD0_EMPTY', 'HLT_eb_high_L1RD2_FILLED', 'HLT_eb_low_L1RD2_FILLED',  'HLT_g0_perf_L1EM3_EMPTY', 'HLT_g120_loose1', 'HLT_g35_loose1_g25_loose1', 'HLT_g35_medium1_g25_medium1', 'HLT_ht0_L1J12_EMPTY', 'HLT_j0_L1J12_EMPTY', 'HLT_j0_lcw_jes_L1J12', 'HLT_j0_perf_bperf_L1J12_EMPTY', 'HLT_j0_perf_bperf_L1MU10', 'HLT_j0_perf_bperf_L1RD0_EMPTY', 'HLT_j0_perf_ds1_L1All', 'HLT_j200_jes_PS', 'HLT_j85_0eta240_test1_j25_240eta490_test1', 'HLT_j85_0eta240_test2_j25_240eta490_test2', 'HLT_j85_2j45', 'HLT_j85_jes_test1', 'HLT_j85_jes_test2', 'HLT_j85_test1', 'HLT_j85_test1_2j45_test1', 'HLT_j85_test2', 'HLT_j85_test2_2j45_test2', 'HLT_mu6_L2Star_idperf', 'HLT_mu24_L2Star_idperf', 'HLT_mu0noL1_fsperf', 'HLT_mu4_r1extr', 'HLT_mu10_r1extr', 'HLT_mu20_r1extr', 'HLT_mu8_mucombTag_noEF_L1MU40', 'HLT_mu10_mucombTag_noEF_L1MU40', 'HLT_mu15_mucombTag_noEF_L1MU40', 'HLT_mu20_iloose_2mu4noL1', 'HLT_mu20_iloose_mu8noL1', 'HLT_mu20_imedium_mu8noL1', 'HLT_mu20_mucombTag_noEF_L1MU40', 'HLT_mu25_mucombTag_noEF_L1MU40', 'HLT_mu4_cosmic_L1MU11_EMPTY', 'HLT_mu4_cosmic_L1MU4_EMPTY', 'HLT_mu4_msonly_cosmic_L1MU11_EMPTY', 'HLT_mu4_msonly_cosmic_L1MU4_EMPTY', 'HLT_tau125_r1medium1', 'HLT_tau125_r1perf', 'HLT_tau1_cosmic_ptonly_L1MU4_EMPTY', 'HLT_tau1_cosmic_track_L1MU4_EMPTY', 'HLT_tau20_r1medium1', 'HLT_tau20_r1perf', 'HLT_tau25_medium1_calo', 'HLT_tau25_medium1_trackcalo', 'HLT_tau25_medium1_tracktwocalo', 'HLT_tau25_perf_calo', 'HLT_tau25_perf_calo_L1TAU12', 'HLT_tau25_perf_trackcalo', 'HLT_tau25_perf_tracktwocalo', 'HLT_tau25_r1perf', 'HLT_tau29_r1medium1', 'HLT_tau29_r1perf', 'HLT_tau35_medium1_calo', 'HLT_tau35_perf_ptonly_tau25_perf_ptonly_L1TAU20IM_2TAU12IM', 'HLT_tau35_perf_tracktwo_L1TAU20_tau25_perf_tracktwo_L1TAU12', 'HLT_tau8_cosmic_ptonly', 'HLT_tau8_cosmic_track', 'HLT_te20', 'HLT_te20_tc_lcw',  'HLT_xe35_L2FS', 'HLT_xe35_l2fsperf_wEFMuFEB_wEFMu', 'HLT_xs30', 'HLT_xs30_tc_lcw', 'HLT_j30_jes_PS_llp_noiso_L1LLP-NOMATCH', 'HLT_j30_jes_PS_llp_L1LLP-NOMATCH', 'HLT_xe35_L2FS_L1XE35_BGRP7', 'HLT_xe35_l2fsperf_wEFMuFEB_wEFMu_L1XE35_BGRP7',]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# low mu run chains

HLT_list=[
'HLT_j10_L1ALFA_Jet', 'HLT_j10_L1ALFA_UNPAIRED_ISO_Jet', 'HLT_j15_L1ALFA_Jet', 'HLT_j15_L1ALFA_UNPAIRED_ISO_Jet', 'HLT_j25_L1ALFA_Jet', 'HLT_j25_L1ALFA_UNPAIRED_ISO_Jet', 'HLT_j35_L1ALFA_Jet', 'HLT_j35_L1ALFA_UNPAIRED_ISO_Jet', 'HLT_lhcfpeb', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_A', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_C', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO', 'HLT_lhcfpeb_L1LHCF_EMPTY', 'HLT_lhcfpeb_L1LHCF_UNPAIRED_ISO', 'HLT_te40_L1MBTS_1_1', 'HLT_te50_L1MBTS_1_1', 'HLT_te50_L1RD3_FILLED', 'HLT_te50_L1TE10', 'HLT_te50_L1TE10.0ETA24', 'HLT_te50_L1TE10.0ETA25', 'HLT_te60_L1MBTS_1_1', 'HLT_te60_L1TE20', 'HLT_te60_L1TE20.0ETA24', 'HLT_te60_L1TE20.0ETA25', 'HLT_te70_L1MBTS_1_1', 'HLT_te80_L1MBTS_1_1',]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# streamers

HLT_list=[
'HLT_noalg_L12EM10VH', 'HLT_noalg_L12EM15', 'HLT_noalg_L12EM3', 'HLT_noalg_L12EM7', 'HLT_noalg_L12EM8VH_MU10', 'HLT_noalg_L12J15_XE55', 'HLT_noalg_L12MU4', 'HLT_noalg_L12MU6', 'HLT_noalg_L13J25.0ETA23', 'HLT_noalg_L13J40', 'HLT_noalg_L13MU4', 'HLT_noalg_L14J15', 'HLT_noalg_L14J20.0ETA49', 'HLT_noalg_L1ALFA_Diff', 'HLT_noalg_L1ALFA_TRT_Diff', 'HLT_noalg_L1ALFA_TRT_UNPAIRED_ISO_Diff', 'HLT_noalg_L1All', 'HLT_noalg_L1CALREQ2', 'HLT_noalg_L1Calo_EMPTY', 'HLT_noalg_L1EM12', 'HLT_noalg_L1EM15', 'HLT_noalg_L1EM15HI_2TAU12IM', 'HLT_noalg_L1EM15HI_2TAU12IM_J25_3J12', 'HLT_noalg_L1EM15HI_2TAU12IM_XE35', 'HLT_noalg_L1EM15HI_TAU40_2TAU15', 'HLT_noalg_L1EM15VH_3EM7', 'HLT_noalg_L1EM15VH_MU10', 'HLT_noalg_L1EM15_MU4', 'HLT_noalg_L1EM18VH', 'HLT_noalg_L1EM20VH', 'HLT_noalg_L1EM3', 'HLT_noalg_L1EM7', 'HLT_noalg_L1EM7_MU10', 'HLT_noalg_L1J15_J15.31ETA49', 'HLT_noalg_L1J20_J20.31ETA49', 'HLT_noalg_L1J30.0ETA49_2J20.0ETA49', 'HLT_noalg_L1J50', 'HLT_noalg_L1J75', 'HLT_noalg_L1J75.31ETA49', 'HLT_noalg_L1LHCF', 'HLT_noalg_L1LHCF_EMPTY', 'HLT_noalg_L1LHCF_UNPAIRED_ISO', 'HLT_noalg_L1LowLumi', 'HLT_noalg_L1MBTS_1_BGRP11', 'HLT_noalg_L1MBTS_1_BGRP9', 'HLT_noalg_L1MBTS_2_BGRP11', 'HLT_noalg_L1MBTS_2_BGRP9', 'HLT_noalg_L1MJJ-100', 'HLT_noalg_L1MU10_TAU12IM', 'HLT_noalg_L1MU10_TAU12IM_J25_2J12', 'HLT_noalg_L1MU10_TAU12IM_XE35', 'HLT_noalg_L1MU10_TAU12_J25_2J12', 'HLT_noalg_L1MU10_TAU20', 'HLT_noalg_L1MU4', 'HLT_noalg_L1MU4_3J15', 'HLT_noalg_L1MU4_J12', 'HLT_noalg_L1MU4_J30', 'HLT_noalg_L1MU6', 'HLT_noalg_L1MU6_J20', 'HLT_noalg_L1MU6_J30.0ETA49_2J20.0ETA49', 'HLT_noalg_L1MU6_J40', 'HLT_noalg_L1MinBias', 'HLT_noalg_L1RD0_BGRP11', 'HLT_noalg_L1RD0_BGRP9', 'HLT_noalg_L1TAU12', 'HLT_noalg_L1TAU12IL', 'HLT_noalg_L1TAU12IM', 'HLT_noalg_L1TAU12IT', 'HLT_noalg_L1TAU20', 'HLT_noalg_L1TAU20IL', 'HLT_noalg_L1TAU20IM', 'HLT_noalg_L1TAU20IM_2TAU12IM', 'HLT_noalg_L1TAU20IM_2TAU12IM_J25_2J20_3J12', 'HLT_noalg_L1TAU20IM_2TAU12IM_XE35', 'HLT_noalg_L1TAU20IT', 'HLT_noalg_L1TAU20_2J20_XE45', 'HLT_noalg_L1TAU20_2TAU12', 'HLT_noalg_L1TAU20_2TAU12_XE35', 'HLT_noalg_L1TAU30', 'HLT_noalg_L1TAU60', 'HLT_noalg_L1XE50', 'HLT_noalg_bkg_L1J12_BGRP0', 'HLT_noalg_bkg_L1J30.31ETA49_BGRP0', 'HLT_noalg_cosmiccalo_L1J12_FIRSTEMPTY', 'HLT_noalg_cosmiccalo_L1J30_FIRSTEMPTY', 'HLT_noalg_cosmicmuons_L1MU11_EMPTY', 'HLT_noalg_cosmicmuons_L1MU4_EMPTY', 'HLT_noalg_dcmmon_L1RD2_BGRP12', 'HLT_noalg_dcmmon_L1RD2_EMPTY', 'HLT_noalg_eb_L1ABORTGAPNOTCALIB_noPS', 'HLT_noalg_eb_L1EMPTY_noPS', 'HLT_noalg_eb_L1FIRSTEMPTY_noPS', 'HLT_noalg_eb_L1PhysicsHigh_noPS', 'HLT_noalg_eb_L1PhysicsLow_noPS', 'HLT_noalg_eb_L1RD0_EMPTY', 'HLT_noalg_eb_L1RD3_EMPTY', 'HLT_noalg_eb_L1RD3_FILLED', 'HLT_noalg_eb_L1UNPAIRED_ISO_noPS', 'HLT_noalg_eb_L1UNPAIRED_NONISO_noPS', 'HLT_noalg_idcosmic_L1TRT_EMPTY', 'HLT_noalg_idcosmic_L1TRT_FILLED', 'HLT_noalg_L1ALFA_ANY', 'HLT_noalg_L1BPH-2M-2MU4', 'HLT_noalg_L1BPH-2M-2MU6', 'HLT_noalg_L1BPH-2M-MU6MU4', 'HLT_noalg_L1BPH-4M8-2MU4', 'HLT_noalg_L1BPH-4M8-2MU6', 'HLT_noalg_L1BPH-4M8-MU6MU4', 'HLT_noalg_L1BPH-DR-2MU4', 'HLT_noalg_L1BPH-DR-2MU6', 'HLT_noalg_L1BPH-DR-MU6MU4', 'HLT_noalg_L1DR-MU10TAU12I', 'HLT_noalg_L1DR-TAU20ITAU12I', 'HLT_noalg_L1DY-BOX-2MU4', 'HLT_noalg_L1DY-BOX-2MU6', 'HLT_noalg_L1DY-BOX-MU6MU4', 'HLT_noalg_L1DY-DR-2MU4', 'HLT_noalg_L1EM10', 'HLT_noalg_L1EM10VH', 'HLT_noalg_L1EM13VH', 'HLT_noalg_L1EM15VH', 'HLT_noalg_L1EM20VHI', 'HLT_noalg_L1EM22VHI', 'HLT_noalg_L1EM8VH', 'HLT_noalg_L1J100', 'HLT_noalg_L1J12', 'HLT_noalg_L1J120', 'HLT_noalg_L1J15', 'HLT_noalg_L1J20', 'HLT_noalg_L1J25', 'HLT_noalg_L1J30', 'HLT_noalg_L1J40', 'HLT_noalg_L1J85', 'HLT_noalg_L1LFV-MU', 'HLT_noalg_L1TAU40', 'HLT_noalg_L1TAU8', 'HLT_noalg_L1TE10', 'HLT_noalg_L1TE10.0ETA24', 'HLT_noalg_L1TE20', 'HLT_noalg_L1TE20.0ETA24', 'HLT_noalg_L1TE30', 'HLT_noalg_L1TE30.0ETA24', 'HLT_noalg_L1TE40', 'HLT_noalg_L1TE50', 'HLT_noalg_L1Topo', 'HLT_noalg_L1XE40', 'HLT_noalg_L1XE45', 'HLT_noalg_L1XE55', 'HLT_noalg_L1XE60', 'HLT_noalg_L1XE70', 'HLT_noalg_L1XE80', 'HLT_noalg_L1XS20', 'HLT_noalg_L1XS30', 'HLT_noalg_L1XS40', 'HLT_noalg_L1XS50', 'HLT_noalg_L1XS60', 'HLT_noalg_bkg_L1J12','HLT_noalg_L1J100.31ETA49', 'HLT_noalg_L1J15.28ETA31', 'HLT_noalg_L1J15.31ETA49', 'HLT_noalg_L1J20.28ETA31', 'HLT_noalg_L1J20.31ETA49', 'HLT_noalg_L1J30.31ETA49', 'HLT_noalg_L1J50.31ETA49', 'HLT_noalg_bkg_L1J30.31ETA49', 'HLT_noalg_L1RD1_FILLED', 'HLT_noalg_L1RD2_FILLED', 'HLT_noalg_L1RD2_EMPTY', 'HLT_noalg_L1RD3_FILLED', 'HLT_noalg_L1RD3_EMPTY', 'HLT_noalg_standby_L1RD0_FILLED', 'HLT_noalg_standby_L1RD0_EMPTY',]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# beam spot

HLT_list=['HLT_beamspot_activeTE_L2StarB_peb', 'HLT_beamspot_activeTE_L2StarB_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_activeTE_L2StarB_pebTRT_L1TRT_FILLED', 'HLT_beamspot_activeTE_L2StarB_peb_L1TRT_EMPTY', 'HLT_beamspot_activeTE_L2StarB_peb_L1TRT_FILLED', 'HLT_beamspot_activeTE_trkfast_peb', 'HLT_beamspot_activeTE_trkfast_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_activeTE_trkfast_pebTRT_L1TRT_FILLED', 'HLT_beamspot_activeTE_trkfast_peb_L1TRT_EMPTY', 'HLT_beamspot_activeTE_trkfast_peb_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB', 'HLT_beamspot_allTE_L2StarB_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB_peb', 'HLT_beamspot_allTE_L2StarB_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_pebTRT_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB_peb_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_peb_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast', 'HLT_beamspot_allTE_trkfast_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast_peb', 'HLT_beamspot_allTE_trkfast_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_pebTRT_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast_peb_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_peb_L1TRT_FILLED', 'HLT_beamspot_trkFS_L2StarB_L1TRT_EMPTY', 'HLT_beamspot_trkFS_L2StarB_L1TRT_FILLED', 'HLT_beamspot_trkFS_trkfast_L1TRT_EMPTY', 'HLT_beamspot_trkFS_trkfast_L1TRT_FILLED', ]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# calibration

HLT_list=['HLT_alfacalib_L1ALFA_A7L1_OD', 'HLT_alfacalib_L1ALFA_A7R1_OD', 'HLT_alfacalib_L1ALFA_ANY', 'HLT_alfacalib_L1ALFA_B7L1_OD', 'HLT_alfacalib_L1ALFA_B7R1_OD', 'HLT_alfacalib_L1ALFA_ELAS', 'HLT_alfacalib_L1ALFA_ELAST15', 'HLT_alfacalib_L1ALFA_ELAST18', 'HLT_alfacalib_L1ALFA_SYS','HLT_alfacalib_L1ALFA_TRT', 'HLT_alfacalib_L1LHCF_ALFA_ANY_A', 'HLT_alfacalib_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO', 'HLT_alfacalib_L1LHCF_ALFA_ANY_C', 'HLT_alfacalib_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO', 'HLT_alfaidpeb_L1TRT_ALFA_ANY', 'HLT_cscmon_L1EM3', 'HLT_cscmon_L1J12', 'HLT_cscmon_L1MU10', 'HLT_ibllumi_L1RD0_ABORTGAPNOTCALIB', 'HLT_ibllumi_L1RD0_FILLED', 'HLT_ibllumi_L1RD0_UNPAIRED_ISO', 'HLT_id_cosmicid', 'HLT_id_cosmicid_trtxk', 'HLT_id_cosmicid_trtxk_central', 'HLT_l1calocalib', 'HLT_l1calocalib_L1BGRP9', 'HLT_larnoiseburst_L1All', 'HLT_larnoiseburst_loose_L1All', 'HLT_larnoiseburst_loose_rerun', 'HLT_lumipeb_vdm_L1MBTS_1', 'HLT_lumipeb_vdm_L1MBTS_1_BGRP11', 'HLT_lumipeb_vdm_L1MBTS_1_BGRP9', 'HLT_lumipeb_vdm_L1MBTS_1_UNPAIRED_ISO', 'HLT_lumipeb_vdm_L1MBTS_2', 'HLT_lumipeb_vdm_L1MBTS_2_BGRP11', 'HLT_lumipeb_vdm_L1MBTS_2_BGRP9', 'HLT_lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO', 'HLT_lumipeb_vdm_L1RD0_BGRP11', 'HLT_lumipeb_vdm_L1RD0_BGRP9', 'HLT_lumipeb_vdm_L1RD0_FILLED', 'HLT_lumipeb_vdm_L1RD0_UNPAIRED_ISO',]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# minbias

HLT_list=['HLT_mb_mbts_L1MBTS_1', 'HLT_mb_mbts_L1MBTS_1_1', 'HLT_mb_mbts_L1MBTS_1_1_EMPTY', 'HLT_mb_mbts_L1MBTS_1_1_UNPAIRED_ISO', 'HLT_mb_mbts_L1MBTS_1_EMPTY', 'HLT_mb_mbts_L1MBTS_1_UNPAIRED_ISO', 'HLT_mb_mbts_L1MBTS_2', 'HLT_mb_mbts_L1MBTS_2_EMPTY', 'HLT_mb_mbts_L1MBTS_2_UNPAIRED_ISO', 'HLT_mb_perf_L1LUCID', 'HLT_mb_perf_L1LUCID_EMPTY', 'HLT_mb_perf_L1LUCID_UNPAIRED_ISO', 'HLT_mb_perf_L1MBTS_2', 'HLT_mb_perf_L1RD1_FILLED', 'HLT_mb_sp1000_trk70_hmt_L1MBTS_1_1', 'HLT_mb_sp1200_trk75_hmt_L1MBTS_1_1', 'HLT_mb_sp1400_hmtperf_L1TE10', 'HLT_mb_sp1400_hmtperf_L1TE10.0ETA24', 'HLT_mb_sp1400_hmtperf_L1TE20', 'HLT_mb_sp1400_hmtperf_L1TE20.0ETA24', 'HLT_mb_sp1400_trk100_hmt_L1TE20', 'HLT_mb_sp1400_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1400_trk80_hmt_L1MBTS_1_1', 'HLT_mb_sp1400_trk90_hmt_L1TE10', 'HLT_mb_sp1400_trk90_hmt_L1TE10.0ETA24', 'HLT_mb_sp1800_hmtperf', 'HLT_mb_sp1800_hmtperf_L1TE20', 'HLT_mb_sp2000_pusup600_trk70_hmt', 'HLT_mb_sp2000_pusup600_trk70_hmt_L1TE30', 'HLT_mb_sp2000_pusup600_trk70_hmt_L1TE40', 'HLT_mb_sp2000_pusup700_trk50_sumet110_hmt_L1TE40', 'HLT_mb_sp2000_pusup700_trk50_sumet150_hmt_L1TE50', 'HLT_mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE30', 'HLT_mb_sp2000_pusup700_trk60_hmt_L1TE30', 'HLT_mb_sp2000_pusup700_trk70_hmt_L1TE30', 'HLT_mb_sp2000_pusup700_trk70_hmt_L1TE40', 'HLT_mb_sp2000_trk70_hmt', 'HLT_mb_sp2500_hmtperf_L1TE20', 'HLT_mb_sp2500_pusup750_trk90_hmt_L1TE40', 'HLT_mb_sp2_hmtperf', 'HLT_mb_sp2_hmtperf_L1MBTS_1_1', 'HLT_mb_sp3000_pusup800_trk120_hmt_L1TE50', 'HLT_mb_sp300_trk10_sumet40_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet50_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet50_hmt_L1RD3_FILLED', 'HLT_mb_sp300_trk10_sumet50_hmt_L1TE10', 'HLT_mb_sp300_trk10_sumet60_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet60_hmt_L1TE20', 'HLT_mb_sp300_trk10_sumet70_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet80_hmt_L1MBTS_1_1', 'HLT_mb_sp400_trk40_hmt_L1MBTS_1_1', 'HLT_mb_sp500_hmtperf', 'HLT_mb_sp600_trk10_sumet40_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet50_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet50_hmt_L1RD3_FILLED', 'HLT_mb_sp600_trk10_sumet50_hmt_L1TE10.0ETA24', 'HLT_mb_sp600_trk10_sumet60_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet60_hmt_L1TE20.0ETA24', 'HLT_mb_sp600_trk10_sumet70_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet80_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk45_hmt_L1MBTS_1_1', 'HLT_mb_sp700_trk50_hmt_L1MBTS_1_1', 'HLT_mb_sp700_trk50_hmt_L1RD3_FILLED', 'HLT_mb_sp700_trk55_hmt_L1MBTS_1_1', 'HLT_mb_sp900_trk60_hmt_L1MBTS_1_1', 'HLT_mb_sp900_trk65_hmt_L1MBTS_1_1', 'HLT_mb_sptrk','HLT_mb_sptrk_L1RD0_EMPTY', 'HLT_mb_sptrk_L1RD0_UNPAIRED_ISO', 'HLT_mb_sptrk_L1RD3_FILLED', 'HLT_mb_sptrk_costr', 'HLT_mb_sptrk_costr_L1RD0_EMPTY', 'HLT_mb_sptrk_noisesup', 'HLT_mb_sptrk_noisesup_L1RD0_EMPTY', 'HLT_mb_sptrk_noisesup_L1RD0_UNPAIRED_ISO', 'HLT_mb_sptrk_noisesup_L1RD3_FILLED', 'HLT_mb_sptrk_pt4_L1MBTS_1', 'HLT_mb_sptrk_pt4_L1MBTS_1_1', 'HLT_mb_sptrk_pt4_L1MBTS_2', 'HLT_mb_sptrk_pt4_L1RD3_FILLED', 'HLT_mb_sptrk_pt6_L1MBTS_1', 'HLT_mb_sptrk_pt6_L1MBTS_1_1', 'HLT_mb_sptrk_pt6_L1MBTS_2', 'HLT_mb_sptrk_pt6_L1RD3_FILLED', 'HLT_mb_sptrk_pt8_L1MBTS_1', 'HLT_mb_sptrk_pt8_L1MBTS_1_1', 'HLT_mb_sptrk_pt8_L1MBTS_2', 'HLT_mb_sptrk_pt8_L1RD3_FILLED', 'HLT_mb_sptrk_vetombts2in_peb_L1ALFA_ANY_UNPAIRED_ISO', 'HLT_noalg_mb_L1LUCID', 'HLT_noalg_mb_L1LUCID_EMPTY', 'HLT_noalg_mb_L1LUCID_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_1_1', 'HLT_noalg_mb_L1MBTS_1_1_EMPTY', 'HLT_noalg_mb_L1MBTS_1_1_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_1_EMPTY', 'HLT_noalg_mb_L1MBTS_1_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_2_EMPTY', 'HLT_noalg_mb_L1MBTS_2_UNPAIRED_ISO', 'HLT_noalg_mb_L1RD0_EMPTY', 'HLT_noalg_mb_L1RD0_FILLED', 'HLT_noalg_mb_L1RD0_UNPAIRED_ISO', 'HLT_noalg_mb_L1RD1_FILLED', 'HLT_noalg_mb_L1RD2_EMPTY', 'HLT_noalg_mb_L1RD2_FILLED', 'HLT_noalg_mb_L1RD3_EMPTY', 'HLT_noalg_mb_L1RD3_FILLED', 'HLT_noalg_mb_L1TE10', 'HLT_noalg_mb_L1TE10.0ETA24', 'HLT_noalg_mb_L1TE20', 'HLT_noalg_mb_L1TE20.0ETA24', 'HLT_noalg_mb_L1TE30', 'HLT_noalg_mb_L1TE40', 'HLT_noalg_mb_L1TE50','HLT_mb_sp1800_hmtperf_L1TE30.0ETA24','HLT_noalg_mb_L1TE30.0ETA24','HLT_noalg_L1TE0', 'HLT_noalg_L1TE0.0ETA24', 'HLT_noalg_L1TE15', 'HLT_noalg_L1TE5', 'HLT_noalg_L1TE5.0ETA24', 'HLT_noalg_mb_L1TE0', 'HLT_noalg_mb_L1TE0.0ETA24', 'HLT_noalg_mb_L1TE15', 'HLT_noalg_mb_L1TE5', 'HLT_noalg_mb_L1TE5.0ETA24', 'HLT_mb_sp1300_hmtperf_L1TE10', 'HLT_mb_sp1300_hmtperf_L1TE10.0ETA24', 'HLT_mb_sp1300_hmtperf_L1TE15', 'HLT_mb_sp1300_hmtperf_L1TE20', 'HLT_mb_sp1300_hmtperf_L1TE20.0ETA24', 'HLT_mb_sp1300_hmtperf_L1TE5', 'HLT_mb_sp1300_hmtperf_L1TE5.0ETA24', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE10', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE10.0ETA24', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE15', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE20', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE20.0ETA24', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE5', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE5.0ETA24', 'HLT_mb_sp1400_trk90_hmt_L1TE15', 'HLT_mb_sp1400_trk90_hmt_L1TE20', 'HLT_mb_sp1400_trk90_hmt_L1TE20.0ETA24', 'HLT_mb_sp1400_trk90_hmt_L1TE5', 'HLT_mb_sp1400_trk90_hmt_L1TE5.0ETA24', 'HLT_mb_sp1500_hmtperf_L1TE10', 'HLT_mb_sp1500_hmtperf_L1TE10.0ETA24', 'HLT_mb_sp1500_hmtperf_L1TE15', 'HLT_mb_sp1500_hmtperf_L1TE20', 'HLT_mb_sp1500_hmtperf_L1TE20.0ETA24', 'HLT_mb_sp1500_hmtperf_L1TE5', 'HLT_mb_sp1500_hmtperf_L1TE5.0ETA24', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE10', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE10.0ETA24', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE15', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE20', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE5', 'HLT_mb_sp1600_pusup550_trk100_hmt_L1TE5.0ETA24', 'HLT_mb_sp1600_trk100_hmt_L1TE10', 'HLT_mb_sp1600_trk100_hmt_L1TE10.0ETA24', 'HLT_mb_sp1600_trk100_hmt_L1TE15', 'HLT_mb_sp1600_trk100_hmt_L1TE20', 'HLT_mb_sp1600_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1600_trk100_hmt_L1TE5', 'HLT_mb_sp1600_trk100_hmt_L1TE5.0ETA24', 'HLT_mb_sp1700_hmtperf_L1TE10', 'HLT_mb_sp1700_hmtperf_L1TE10.0ETA24', 'HLT_mb_sp1700_hmtperf_L1TE15', 'HLT_mb_sp1700_hmtperf_L1TE20', 'HLT_mb_sp1700_hmtperf_L1TE20.0ETA24', 'HLT_mb_sp1700_hmtperf_L1TE5', 'HLT_mb_sp1700_hmtperf_L1TE5.0ETA24', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE10', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE10.0ETA24', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE15', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE20', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE20.0ETA24', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE5', 'HLT_mb_sp1800_pusup600_trk110_hmt_L1TE5.0ETA24', 'HLT_mb_sp1800_trk110_hmt_L1TE10', 'HLT_mb_sp1800_trk110_hmt_L1TE10.0ETA24', 'HLT_mb_sp1800_trk110_hmt_L1TE15', 'HLT_mb_sp1800_trk110_hmt_L1TE20', 'HLT_mb_sp1800_trk110_hmt_L1TE20.0ETA24', 'HLT_mb_sp1800_trk110_hmt_L1TE5', 'HLT_mb_sp1800_trk110_hmt_L1TE5.0ETA24', 'HLT_mb_sp2_hmtperf_L1TE0', 'HLT_mb_sp2_hmtperf_L1TE0.0ETA24', 'HLT_mb_sp900_pusup350_trk60_hmt_L1TE10', 'HLT_mb_sp900_pusup350_trk60_hmt_L1TE10.0ETA24', 'HLT_mb_sp900_pusup350_trk60_hmt_L1TE5', 'HLT_mb_sp900_pusup350_trk60_hmt_L1TE5.0ETA24', 'HLT_mb_sp900_trk60_hmt_L1TE10', 'HLT_mb_sp900_trk60_hmt_L1TE10.0ETA24', 'HLT_mb_sp900_trk60_hmt_L1TE5', 'HLT_mb_sp900_trk60_hmt_L1TE5.0ETA24', 'HLT_lumipeb_L1MBTS_2', 'HLT_mb_sptrk_vetombts2in_L1TRT_ALFA_EINE', 'HLT_mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY', 'HLT_mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY_UNPAIRED_ISO',]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

# --------------------------------------
# L1Topo seeded

HLT_list=['HLT_mu11_2mu4noL1_nscan03_L1MU11_LFV-MU', 'HLT_mu11_L1MU10_2mu4noL1_nscan03', 'HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25', 'HLT_2j40_0eta490_invm250', 'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-400', 'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-700', 'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-800', 'HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-900', 'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-400', 'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-700', 'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-800', 'HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-900', 'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-400', 'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-700', 'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-800', 'HLT_2j55_bmedium_L13J20_4J20.0ETA49_MJJ-900', 'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-400', 'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-700', 'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-800', 'HLT_2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-900', 'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-400', 'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-700', 'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-800', 'HLT_2j55_bperf_L13J20_4J20.0ETA49_MJJ-900', 'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-400', 'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-700', 'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-800', 'HLT_2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-900', 'HLT_2mu4_bBmumu_L1BPH-2M-2MU4', 'HLT_2mu4_bBmumu_L1BPH-2M-2MU4-B', 'HLT_2mu4_bBmumu_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bBmumu_L1BPH-4M8-2MU4', 'HLT_2mu4_bBmumu_L1BPH-4M8-2MU4-B', 'HLT_2mu4_bBmumu_L1BPH-4M8-2MU4-BO', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-B', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bBmumu_L1BPH-DR-2MU4-BPH-4M8-2MU4', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4-B', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4-B', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4-BO', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BPH-4M8-2MU4', 'HLT_2mu4_bBmumuxv2_L1BPH-2M-2MU4', 'HLT_2mu4_bBmumuxv2_L1BPH-2M-2MU4-B', 'HLT_2mu4_bBmumuxv2_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bBmumuxv2_L1BPH-4M8-2MU4', 'HLT_2mu4_bBmumuxv2_L1BPH-4M8-2MU4-B', 'HLT_2mu4_bBmumuxv2_L1BPH-4M8-2MU4-BO', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-B', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bBmumuxv2_L1BPH-DR-2MU4-BPH-4M8-2MU4', 'HLT_2mu4_bDimu_L1BPH-2M-2MU4', 'HLT_2mu4_bDimu_L1BPH-2M-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-4M8-2MU4', 'HLT_2mu4_bDimu_L1BPH-4M8-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-4M8-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bDimu_L1BPH-DR-2MU4-BPH-4M8-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-4M8-2MU4', 'HLT_2mu4_bJpsimumu_L1BPH-2M-2MU4', 'HLT_2mu4_bJpsimumu_L1BPH-2M-2MU4-B', 'HLT_2mu4_bJpsimumu_L1BPH-2M-2MU4-BO', 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4', 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-B', 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-BO', 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'HLT_2mu4_bJpsimumu_L1BPH-DR-2MU4-BPH-2M-2MU4', 'HLT_2mu4_bUpsimumu_L1BPH-2M-2MU4', 'HLT_2mu4_bUpsimumu_L1BPH-2M-2MU4-B', 'HLT_2mu4_bUpsimumu_L1BPH-2M-2MU4-BO', 'HLT_2mu6_bBmumu_L1BPH-2M-2MU6', 'HLT_2mu6_bBmumu_L1BPH-4M8-2MU6', 'HLT_2mu6_bBmumu_L1BPH-DR-2MU6', 'HLT_2mu6_bBmumu_L1BPH-DR-2MU6-BPH-2M-2MU6', 'HLT_2mu6_bBmumu_L1BPH-DR-2MU6-BPH-4M8-2MU6', 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-2M-2MU6', 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU6', 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6', 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6-BPH-2M-2MU6', 'HLT_2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6-BPH-4M8-2MU6', 'HLT_2mu6_bBmumuxv2_L1BPH-2M-2MU6', 'HLT_2mu6_bBmumuxv2_L1BPH-4M8-2MU6', 'HLT_2mu6_bBmumuxv2_L1BPH-DR-2MU6', 'HLT_2mu6_bBmumuxv2_L1BPH-DR-2MU6-BPH-2M-2MU6', 'HLT_2mu6_bBmumuxv2_L1BPH-DR-2MU6-BPH-4M8-2MU6', 'HLT_2mu6_bDimu_L1BPH-2M-2MU6', 'HLT_2mu6_bDimu_L1BPH-4M8-2MU6', 'HLT_2mu6_bDimu_L1BPH-DR-2MU6', 'HLT_2mu6_bDimu_L1BPH-DR-2MU6-BPH-2M-2MU6', 'HLT_2mu6_bDimu_L1BPH-DR-2MU6-BPH-4M8-2MU6', 'HLT_2mu6_bDimu_novtx_noos_L1BPH-2M-2MU6', 'HLT_2mu6_bDimu_novtx_noos_L1BPH-4M8-2MU6', 'HLT_2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6', 'HLT_2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6-BPH-2M-2MU6', 'HLT_2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6-BPH-4M8-2MU6', 'HLT_2mu6_bJpsimumu_L1BPH-2M-2MU6', 'HLT_2mu6_bJpsimumu_L1BPH-DR-2MU6', 'HLT_2mu6_bJpsimumu_L1BPH-DR-2MU6-BPH-2M-2MU6', 'HLT_2mu6_bUpsimumu_L1BPH-2M-2MU6', 'HLT_e13_etcut_L1EM10_W-MT25', 'HLT_e13_etcut_L1EM10_W-MT30', 'HLT_e13_etcut_L1W-NOMATCH', 'HLT_e13_etcut_L1W-NOMATCH_W-05RO-XEEMHT', 'HLT_e13_etcut_trkcut', 'HLT_e13_etcut_trkcut_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE', 'HLT_e13_etcut_trkcut_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20_xe20_mt25', 'HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', 'HLT_e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'HLT_e13_etcut_trkcut_xe20', 'HLT_e13_etcut_trkcut_xe20_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'HLT_e13_etcut_trkcut_xe20_mt25', 'HLT_e13_etcut_trkcut_xs15_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'HLT_e14_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM12', 'HLT_e14_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM12', 'HLT_e14_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM12', 'HLT_e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12', 'HLT_e14_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM12', 'HLT_e14_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12', 'HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15-TAU12I', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15TAU12I-J25', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I', 'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I', 'HLT_e17_lhmedium_nod0_tau80_medium1_tracktwo_L1EM15-TAU40', 'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15-TAU12I', 'HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15TAU12I-J25', 'HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I', 'HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I', 'HLT_e17_lhmedium_tau80_medium1_tracktwo_L1EM15-TAU40', 'HLT_e17_medium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'HLT_e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'HLT_e17_medium_tau25_medium1_tracktwo_L1EM15-TAU12I', 'HLT_e17_medium_tau25_medium1_tracktwo_L1EM15TAU12I-J25', 'HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I', 'HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I', 'HLT_e17_medium_tau80_medium1_tracktwo_L1EM15-TAU40', 'HLT_e18_etcut_L1EM15_W-MT35', 'HLT_e18_etcut_trkcut', 'HLT_e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE', 'HLT_e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30_xe35_mt35', 'HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0', 'HLT_e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30', 'HLT_e18_etcut_trkcut_xe35', 'HLT_e18_etcut_trkcut_xe35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30', 'HLT_e18_etcut_trkcut_xe35_mt35', 'HLT_e5_etcut_L1W-05DPHI-EMXE-1', 'HLT_e5_etcut_L1W-05DPHI-JXE-0', 'HLT_e5_etcut_L1W-05RO-XEHT-0', 'HLT_e5_etcut_L1W-10DPHI-EMXE-0', 'HLT_e5_etcut_L1W-10DPHI-JXE-0', 'HLT_e5_etcut_L1W-15DPHI-EMXE-0', 'HLT_e5_etcut_L1W-15DPHI-JXE-0', 'HLT_e5_etcut_L1W-250RO2-XEHT-0', 'HLT_e5_etcut_L1W-90RO2-XEHT-0', 'HLT_e5_etcut_L1W-HT20-JJ15.ETA49', 'HLT_e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5', 'HLT_e5_lhtight_e4_etcut_L1JPSI-1M5', 'HLT_e5_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5', 'HLT_e5_lhtight_nod0_e4_etcut_L1JPSI-1M5', 'HLT_e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5', 'HLT_e5_tight_e4_etcut_L1JPSI-1M5', 'HLT_e9_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM7', 'HLT_e9_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM7', 'HLT_e9_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM7', 'HLT_e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7', 'HLT_e9_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM7', 'HLT_e9_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7', 'HLT_g10_etcut_mu10_L1LFV-EM8I', 'HLT_g10_etcut_mu10_iloose_taumass_L1LFV-EM8I', 'HLT_g10_etcut_mu10_taumass', 'HLT_g10_loose_mu10_iloose_taumass_L1LFV-EM8I', 'HLT_g15_loose_2j40_0eta490_3j25_0eta490', 'HLT_g20_etcut_mu4_L1LFV-EM15I', 'HLT_g20_etcut_mu4_iloose_taumass_L1LFV-EM15I', 'HLT_g20_etcut_mu4_taumass', 'HLT_g20_loose_2j40_0eta490_3j25_0eta490', 'HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700', 'HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-900', 'HLT_g20_loose_mu4_iloose_taumass_L1LFV-EM15I', 'HLT_ht1000', 'HLT_ht1000_L1HT190-J15.ETA21', 'HLT_ht400', 'HLT_ht400_L1HT150-J20.ETA31', 'HLT_ht550', 'HLT_ht550_L1HT150-J20.ETA31', 'HLT_ht700', 'HLT_ht700_L1HT190-J15.ETA21', 'HLT_ht850', 'HLT_ht850_L1HT190-J15.ETA21', 'HLT_j100_xe80_L1J40_DPHI-CJ20XE50', 'HLT_j100_xe80_L1J40_DPHI-J20XE50', 'HLT_j100_xe80_L1J40_DPHI-J20s2XE50', 'HLT_j100_xe80_L1J40_DPHI-Js2XE50', 'HLT_j360_a10_lcw_nojcalib', 'HLT_j360_a10_lcw_nojcalib_L1HT150-J20.ETA31', 'HLT_j360_a10_lcw_sub', 'HLT_j360_a10_lcw_sub_L1HT150-J20.ETA31', 'HLT_j360_a10_nojcalib', 'HLT_j360_a10_nojcalib_L1HT150-J20.ETA31', 'HLT_j360_a10_sub', 'HLT_j360_a10_sub_L1HT150-J20.ETA31', 'HLT_j360_a10r', 'HLT_j460_a10_lcw_nojcalib', 'HLT_j460_a10_lcw_nojcalib_L1HT190-J15.ETA21', 'HLT_j460_a10_lcw_sub', 'HLT_j460_a10_lcw_sub_L1HT190-J15.ETA21', 'HLT_j460_a10_nojcalib', 'HLT_j460_a10_nojcalib_L1HT190-J15.ETA21', 'HLT_j460_a10_sub', 'HLT_j460_a10_sub_L1HT190-J15.ETA21', 'HLT_j460_a10r', 'HLT_j80_xe80_1dphi10_L1J40_DPHI-CJ20XE50', 'HLT_j80_xe80_1dphi10_L1J40_DPHI-J20XE50', 'HLT_j80_xe80_1dphi10_L1J40_DPHI-J20s2XE50', 'HLT_j80_xe80_1dphi10_L1J40_DPHI-Js2XE50', 'HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I', 'HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12-J25', 'HLT_mu14_tau25_medium1_tracktwo_L1MU10_TAU12I-J25', 'HLT_mu4_iloose_mu4_11invm60_noos_L1DY-BOX-2MU4', 'HLT_mu4_iloose_mu4_11invm60_noos_novtx_L1DY-BOX-2MU4', 'HLT_mu4_iloose_mu4_7invm9_noos_L1DY-BOX-2MU4', 'HLT_mu4_iloose_mu4_7invm9_noos_novtx_L1DY-BOX-2MU4', 'HLT_mu4_j60_dr05_2j35_L13J15_BTAG-MU4J30', 'HLT_mu4_j70_dr05_L1BTAG-MU4J30', 'HLT_mu6_2j40_0eta490_invm1000', 'HLT_mu6_2j40_0eta490_invm400', 'HLT_mu6_2j40_0eta490_invm600', 'HLT_mu6_2j40_0eta490_invm800', 'HLT_mu6_iloose_mu6_11invm24_noos_L1DY-BOX-2MU6', 'HLT_mu6_iloose_mu6_11invm24_noos_novtx_L1DY-BOX-2MU6', 'HLT_mu6_iloose_mu6_24invm60_noos_L1DY-BOX-2MU6', 'HLT_mu6_iloose_mu6_24invm60_noos_novtx_L1DY-BOX-2MU6', 'HLT_mu6_j50_dr05_2j35_L13J15_BTAG-MU6J25', 'HLT_mu6_j60_dr05_L1BTAG-MU6J25', 'HLT_mu6_mu4_bBmumu_L1BPH-2M-MU6MU4', 'HLT_mu6_mu4_bBmumu_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bBmumu_L1BPH-4M8-MU6MU4', 'HLT_mu6_mu4_bBmumu_L1BPH-4M8-MU6MU4-BO', 'HLT_mu6_mu4_bBmumu_L1BPH-DR-MU6MU4', 'HLT_mu6_mu4_bBmumu_L1BPH-DR-MU6MU4-BO', 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M-MU6MU4-B', 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-MU6MU4-B', 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-MU6MU4-BO', 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-DR-MU6MU4-B', 'HLT_mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-DR-MU6MU4-BO', 'HLT_mu6_mu4_bBmumuxv2_L1BPH-2M-MU6MU4', 'HLT_mu6_mu4_bBmumuxv2_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bBmumuxv2_L1BPH-4M8-MU6MU4', 'HLT_mu6_mu4_bBmumuxv2_L1BPH-4M8-MU6MU4-BO', 'HLT_mu6_mu4_bBmumuxv2_L1BPH-DR-MU6MU4', 'HLT_mu6_mu4_bBmumuxv2_L1BPH-DR-MU6MU4-BO', 'HLT_mu6_mu4_bDimu_L1BPH-2M-MU6MU4', 'HLT_mu6_mu4_bDimu_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bDimu_L1BPH-4M8-MU6MU4', 'HLT_mu6_mu4_bDimu_L1BPH-4M8-MU6MU4-BO', 'HLT_mu6_mu4_bDimu_L1BPH-BPH-DR-MU6MU4', 'HLT_mu6_mu4_bDimu_L1BPH-BPH-DR-MU6MU4-BO', 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-2M-MU6MU4-B', 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-4M8-MU6MU4-B', 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-4M8-MU6MU4-BO', 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-DR-MU6MU4-B', 'HLT_mu6_mu4_bDimu_novtx_noos_L1BPH-DR-MU6MU4-BO', 'HLT_mu6_mu4_bJpsimumu_L1BPH-2M-MU6MU4', 'HLT_mu6_mu4_bJpsimumu_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bJpsimumu_L1BPH-DR-MU6MU4', 'HLT_mu6_mu4_bJpsimumu_L1BPH-DR-MU6MU4-BO', 'HLT_mu6_mu4_bUpsimumu_L1BPH-2M-MU6MU4', 'HLT_mu6_mu4_bUpsimumu_L1BPH-2M-MU6MU4-BO', 'HLT_mu6_mu4_bUpsimumu_L1BPH-DR-MU6MU4', 'HLT_mu6_mu4_bUpsimumu_L1BPH-DR-MU6MU4-BO', 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1BOX-TAU20ITAU12I', 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I', 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25', 'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20ITAU12I-J25', 'HLT_mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I', 'HLT_mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25', 'HLT_mu14_iloose_tau25_medium1_tracktwo_L1MU10_TAU12I-J25', 'HLT_mu4_j35_bperf_split_dr05_dz02_L1BTAG-MU4J15', 'HLT_mu4_j55_bperf_split_dr05_dz02_L1BTAG-MU4J15', 'HLT_mu6_j110_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_mu6_j150_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_mu6_j175_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_mu6_j260_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_mu6_j320_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_mu6_j400_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_mu6_j85_bperf_split_dr05_dz02_L1BTAG-MU6J20', 'HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25', 'HLT_e17_medium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25', 'HLT_2j40_0eta490_invm250_xe80_L1XE70', 'HLT_g25_loose_L1EM20VH_4j35_0eta490', 'HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_2j35_0eta490', 'HLT_g25_medium_L1EM22VHI_4j35_0eta490_invm700', 'HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_3j35_0eta490_invm700', 'HLT_g35_medium_tau25_dikaon_tracktwo_L1TAU12', 'HLT_g35_medium_tau25_dikaontight_tracktwo_L1TAU12', 'HLT_g35_medium_tau35_dikaon_tracktwo_L1TAU12', 'HLT_g35_medium_tau35_dikaontight_tracktwo_L1TAU12', 'HLT_j40_0eta490_j30_0eta490_deta25_xe80_L1XE70', 'HLT_tau25_dikaon_tracktwo', 'HLT_tau25_dikaontight_tracktwo', 'HLT_tau35_dikaon_tracktwo_L1TAU12', 'HLT_tau35_dikaontight_tracktwo_L1TAU12', 'HLT_e24_lhtight_smooth_L1EM20VH', 'HLT_lumipeb_L1ALFA_BGT', 'HLT_lumipeb_L1ALFA_BGT_BGRP10', 'HLT_lumipeb_L1ALFA_BGT_UNPAIRED_ISO', 'HLT_lumipeb_L1MBTS_1', 'HLT_lumipeb_L1MBTS_1_UNPAIRED_ISO', 'HLT_lumipeb_L1MBTS_2_UNPAIRED_ISO', 'HLT_mb_sp1300_hmtperf_L1TE3', 'HLT_mb_sp1300_hmtperf_L1TE3.0ETA24', 'HLT_mb_sp1400_pusup550_trk90_hmt_L1TE25', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE10', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE10.0ETA24', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE15', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE20', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE25', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE5', 'HLT_mb_sp1500_pusup700_trk100_hmt_L1TE5.0ETA24', 'HLT_mb_sp1500_trk100_hmt_L1TE10', 'HLT_mb_sp1500_trk100_hmt_L1TE10.0ETA24', 'HLT_mb_sp1500_trk100_hmt_L1TE15', 'HLT_mb_sp1500_trk100_hmt_L1TE20', 'HLT_mb_sp1500_trk100_hmt_L1TE20.0ETA24', 'HLT_mb_sp1500_trk100_hmt_L1TE5', 'HLT_mb_sp1500_trk100_hmt_L1TE5.0ETA24', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE10', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE10.0ETA24', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE15', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE20', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE20.0ETA24', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE25', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE5', 'HLT_mb_sp1800_pusup800_trk110_hmt_L1TE5.0ETA24', 'HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE10', 'HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE15', 'HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE20', 'HLT_mb_sp2100_pusup1000_trk120_hmt_L1TE25', 'HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE10', 'HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE15', 'HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE20', 'HLT_mb_sp2400_pusup1100_trk130_hmt_L1TE25', 'HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE10', 'HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE15', 'HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE20', 'HLT_mb_sp2700_pusup1300_trk140_hmt_L1TE25', 'HLT_mb_sp2_hmtperf_L1TE3', 'HLT_mb_sp2_hmtperf_L1TE3.0ETA24', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE10', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE15', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE20', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE25', 'HLT_mb_sp3000_pusup1400_trk150_hmt_L1TE30', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE10', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE15', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE20', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE25', 'HLT_mb_sp3000_pusup1500_trk160_hmt_L1TE30', 'HLT_mb_sptrk_vetombts2in_L1ALFA_ANY', 'HLT_mb_sptrk_vetombts2in_L1ALFA_ANY_UNPAIRED_ISO', 'HLT_mb_sptrk_vetombts2in_L1TRT_ALFA_ANY', 'HLT_mb_sptrk_vetombts2in_L1TRT_ALFA_ANY_UNPAIRED_ISO', 'HLT_noalg_L1ALFA_CDiff_Phys', 'HLT_noalg_L1ALFA_Diff_Phys', 'HLT_noalg_L1ALFA_Jet_Phys', 'HLT_noalg_L1TE15.0ETA24', 'HLT_noalg_L1TE25', 'HLT_noalg_L1TE25.0ETA24', 'HLT_noalg_L1TE3', 'HLT_noalg_L1TE3.0ETA24', 'HLT_noalg_mb_L1TE15.0ETA24', 'HLT_noalg_mb_L1TE25', 'HLT_noalg_mb_L1TE25.0ETA24', 'HLT_noalg_mb_L1TE3', 'HLT_noalg_mb_L1TE3.0ETA24', 'HLT_noalg_mb_L1TE40.0ETA24', ]
disabled_Rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    1000 : TriggerRule(PS=-1, comment=''), 
    }])))

RulesList=[background_Rules, monitoring_Rules, disabled_Rules, alfa_Rules]

rules = {}

for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print 'FATAL     Standby_pp_v5_rules     Duplicated rule inside physics rule. Cannot be added:',newrule
    rules.update(Rules)

#################################################################################################
#################################################################################################
