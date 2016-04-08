# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Physics_pp_v4'

EBrun=True

rules = {

    # Randoms 
    # EL: I'm not sure this works, target should be 100 Hz
    #'EF_rd0_filled_NoAlg':   { 0: TriggerRule(rate=100/1000.,comment='Unbiased triggers')}, #hack around prescale of 1000 in rate prediction

    # Jets    
    'EF_j15_a4tchad'            : { 0: TriggerRule(rate=10.,comment='RD0 seeded'),
                                    1000: TriggerRule(PS=-1.,comment='not needed ~75% of RNDM rate at 5e33'), },
    'EF_j25_a4tchad'            : { 0: TriggerRule(rate=10.,comment='RD0 seeded'),
                                    1000: TriggerRule(PS=-1,comment='~8% of RNDM rate at 5e33'),}, 
    'EF_j35_a4tchad'            :  { 0: TriggerRule(rate=10.,comment='RD0 seeded')}, # ~1.6% of RNDM rate
    'EF_j55_a4tchad'            :  { 0: TriggerRule(rate=10.,comment='J15 seeded')},
    'EF_j80_a4tchad'            :  { 0: TriggerRule(rate=10.,comment='J30 seeded')},
    'EF_j110_a4tchad'           :  { 0: TriggerRule(rate=10.,comment='J50 seeded')},
    'EF_j145_a4tchad'           :  { 0: TriggerRule(rate=10.,comment='J75 seeded')},
    'EF_j220_a4tchad'           :  { 0: TriggerRule(rate=10.,comment='J75 seeded')},
    'EF_j360_a4tchad'           :  { 0: TriggerRule(PS=1.,comment='J75 seeded')},

    'EF_fj15_a4tchad'            : { 0: TriggerRule(PS=-1.,comment='RD0 seeded'),
                                     1000: TriggerRule(rate=10.,comment='not needed ~98% of RNDM rate at 5e33'), },                                     
    'EF_fj25_a4tchad'            : { 0: TriggerRule(rate=5.,comment='RD0 seeded')}, # ~1% of RNDM rate, 10Hz requires 1 KHz L2 rndm
    'EF_fj35_a4tchad'            : { 0: TriggerRule(rate=1.5,comment='RD0 seeded')}, # ~0.14% of RNDM rate  
    'EF_fj45_a4tchad_L2FS'      : { 0: TriggerRule(rate=2.5,comment='RD0 seeded')}, 
    'EF_fj80_a4tchad'           : { 0: TriggerRule(rate=10.,comment='FJ30 seeded')}, 
    'EF_fj110_a4tchad'          : { 0: TriggerRule(rate=10.,comment='FJ30 seeded')}, 
    'EF_fj145_a4tchad'          : { 0: TriggerRule(rate=10.,comment='FJ30 seeded')}, 

    # multijet
    'EF_3j25_a4tchad'              : { 0: TriggerRule(rate=5.,comment='multijets')}, 
    'EF_4j25_a4tchad'              : { 0: TriggerRule(rate=5.,comment='multijets')}, 
    'EF_5j25_a4tchad'              : { 0: TriggerRule(rate=5.,comment='multijets')}, 
    'EF_5j45_a4tchad_L2FS'         : { 0: TriggerRule(rate=5.,comment='multijets')}, 
    'EF_6j35_a4tchad_L2FS_5L2j15'  : { 0: TriggerRule(rate=5.,comment='multijets')}, 
    'EF_7j35_a4tchad_L2FS_5L2j15'  : { 0: TriggerRule(rate=5.,comment='multijets')}, 

    # Gamma  
    'EF_g10_loose'             : { 0: TriggerRule(rate=10.,comment='')},
    'EF_g20_loose'             : { 0: TriggerRule(rate=10.,comment='')},
    'EF_g30_loose'             : { 0: TriggerRule(rate=10.,comment='')},
    'EF_g40_loose'             : { 0: TriggerRule(rate=10.,comment='')},
    'EF_g60_loose'             : { 0: TriggerRule(rate=10.,comment='')},
    'EF_g80_loose'             : { 0: TriggerRule(rate=10.,comment='')},
    'EF_g100_loose'            : { 0: TriggerRule(rate=10.,comment='')},
    'EF_g120_loose'            : { 0: TriggerRule(PS=1.,comment='')},

    #*** Photons -  prescales adjusted to fill up HLT bandwidth available to egamma, by default not below 2 Hz each, preferably much higher
    #'EF_g20_loose':           { 0: TriggerRule(rate=2,comment='gamma trigger')},  
    #'EF_g60_loose':           { 0: TriggerRule(rate=2,comment='gamma trigger')},  

    # Electons    
    # *** Z T&P - unprescaled
    'EF_e24vhi_medium1'            : { 0: TriggerRule(PS=1,comment='unprescaled Z T&P')},  
    'EF_e60_medium1':              {10000: TriggerRule(PS=1,comment='unprescaled Z T&P') },
    'EF_e24vh_tight1_e15_NoCut_Zee': { 0: TriggerRule(PS=1,ESRate=0.4,comment='monitoring purpose')},
    'EF_e24vh_medium1':            {10000: TriggerRule(PS=10,rerun=True,ESRate=0.4,comment='Support for isolated trigger - after commissioning') },

    #*** W T&P - unprescaled
    'EF_e13_etcutTrk_xs45_L1XS50_dphi2j15xe20': { 0: TriggerRule(PS=1,comment='unprescaled W T&P')},
    'EF_e13_etcutTrk_xs45_L1XS55_dphi2j15xe20': { 0: TriggerRule(PS=1,comment='unprescaled W T&P consistent with current menu')},
    'EF_e20_etcutTrk_xe30_dphi2j15xe20':        { 0: TriggerRule(PS=1,comment='unprescaled W T&P')},

    #*** JPsi and Upsilon T&P (prioritise list) - prescales adjusted to fill up L1 bandwidth available to egamma (L1 limited triggers, HLT rate is small)
    # these are seeded from L1_2EM3_EM6, set PS for 2EM3 to 5 KHz max for now 
    'L1_2EM3_EM6' : { 0 : TriggerRule(rate=3000.0, comment='Max reasonable rate for Jpsi triggers'),
                      1001 : TriggerRule(rate=2000.0, comment='Max reasonable rate for Jpsi triggers')},
    'EF_e5_tight1_e4_etcut_Jpsi':  { 0: TriggerRule(PS=1,comment='Jpsi trigger')},
    'EF_e5_tight1_e9_etcut_Jpsi':  { 0: TriggerRule(PS=1,comment='Jpsi trigger')},
    'EF_e5_tight1_e14_etcut_Jpsi': { 0: TriggerRule(PS=1,comment='Jpsi trigger')},
    'EF_e9_tight1_e4_etcut_Jpsi':  { 0: TriggerRule(PS=1,comment='Jpsi trigger')},
    'EF_e14_tight1_e4_etcut_Jpsi': { 0: TriggerRule(PS=1,comment='Jpsi trigger')},

    'EF_e5_tight1_e5_NoCut':           { 0: TriggerRule(rate=10.,comment='Jpsi support trigger')},
    'EF_e20vhT_tight1_g6T_etcut_Upsi': { 0: TriggerRule(PS=1,comment='Upsi trigger')},

    # Limit to 10 Hz EF rate
    'EF_e24vh_medium1_IDTrkNoCut':     { 0: TriggerRule(rate=10,ESRate=0.3,comment='Support trigger, rerun=1?')},
    
    # these are seeded from L1_2EM3, set PS for 2EM3 to 5 KHz max for now 
    'L1_2EM3' : { 0 : TriggerRule(rate=3000.0, comment='Max reasonable rate for Jpsi triggers'),
                  1001 : TriggerRule(rate=2000.0, comment='Max reasonable rate for Jpsi triggers')},
    'EF_e5_tight1_e4_etcut_Jpsi_L2StarB': { 0: TriggerRule(PS=1,comment='Jpsi trigger')},
    'EF_e5_tight1_e4_etcut_Jpsi_L2StarC': { 0: TriggerRule(PS=1,comment='Jpsi trigger')},
    'EF_2e5_tight1_Jpsi':                 { 0: TriggerRule(PS=1,comment='Jpsi trigger')},

    #*** etcut -  prescales adjusted to fill up HLT bandwidth available to egamma, by default not below 2 Hz each, preferably much higher
    'EF_g200_etcut':           {  0: TriggerRule(PS=1,comment='etcut trigger')},
    'EF_g24_etcut':            { 2500: TriggerRule(rate=2.0,comment='etcut trigger'),
                                 0: TriggerRule(rate=10.0,comment='etcut trigger')},
    'EF_e5_etcut':             { 2500: TriggerRule(rate=2.0,comment='etcut trigger'),
                                 0: TriggerRule(rate=10.0,comment='etcut trigger')},
    'EF_g20_etcut':            { 2500: TriggerRule(rate=2.0,comment='etcut trigger'),
                                 0: TriggerRule(rate=10.0,comment='etcut trigger')},


    # Muons
    #--- Single muon triggers
    'EF_mu24i_tight':                 {10000: TriggerRule(PS=1,ESRate=0.3, comment='Baseline trigger') },
    'EF_mu36_tight':                  {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_mu40_tight':                  {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
    'EF_mu50_MSonly_barrel_tight':    {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    #--- Single-muon-seeded EFFS multi muon triggers
    'EF_mu18_tight_mu8_EFFS':         {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_mu18_tight_2mu4_EFFS':        {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_mu18_tight_Jpsi_EFFS':                 {10000: TriggerRule(PS=1,comment='Bphysics tag&probe trigger') },
    'EF_mu18_tight_L2_2mu4T_Jpsimumu':         {10000: TriggerRule(PS=1,comment='Bphysics tag&probe trigger') },

    # muon rerun
    'EF_mu4T':                        {10000: TriggerRule(rate=1.0, rerun=True, comment='Efficiency for mu4T')},
    'EF_mu6':                         {10000: TriggerRule(rate=1.0, rerun=True, comment='Efficiency for mu6')},
    'EF_mu13':                        {10000: TriggerRule(PS=0,rerun=True, comment='Efficiency for mu13')},
    'EF_mu18_tight':                  {10000: TriggerRule(PS=0,rerun=True, comment='Efficiency for mu18')},

    'EF_mu18_IDTrkNoCut_tight':       {10000:  TriggerRule(rate=10, ESValue=1, comment = 'Muon DQ in Express stream')},
    'EF_2mu4T_Jpsimumu_IDTrkNoCut':     {10000: TriggerRule(rate=10,ESRate=0.1, comment='supporting BPhys')},
    'EF_mu4Tmu6_Jpsimumu_IDTrkNoCut':   {10000: TriggerRule(rate=10, comment='supporting BPhys')},
    'EF_2mu13_Zmumu_IDTrkNoCut':        {10000: TriggerRule(rate=10,ESRate=0.1, comment='monitoring')},


    # tau for express stream
    'EF_tauNoCut_L1TAU40':                 {10000:  TriggerRule(rate=0.6, ESRate=0.4, comment = 'Express stream') },

    # xe
    'EF_xe80_tclcw':              {4000: TriggerRule(PS=1,comment='topo met - XE50') },
    'EF_xe30_tclcw':             {10000: TriggerRule(rate=5.0,comment='Support trigger') },
    'EF_xe_NoCut_allL1':  {10000: TriggerRule(PS=0,rerun=True,comment='Calculates met on accepted events')},
    'EF_xe_NoCut_FEB_allL1':  {10000: TriggerRule(PS=0,rerun=True,comment='Calculates FEB met on accepted events')},
    'EF_xe_NoCut_tclcw_allL1':  {10000: TriggerRule(PS=0,rerun=True,comment='Calculates topo met on accepted events')},


    #***L1 passthrough
    #all L1 EM thresholds as in EB stream should give a ~100 Hz starting point at 5e33 then raise to 200 Hz
    'EF_eb_physics_L1PS':  { 0: TriggerRule(rate=100, comment='EnhancedBias chains')},

    # L1Calocalibration ~3 KHz non-BGRP7, ~3 KHz BGRP7 (set both to PS=1 at L2)
    'L2_L1Calocalibration': { 0: TriggerRule(PS=3.4, comment='L1 Calo partial event building')},
    'EF_L1Calocalibration': { 0: TriggerRule(PS=-1, comment='L1 Calo partial event building')},

    'L1_BGRP7' :                    { 0 : TriggerRule(PS=4000, scaling="bunches", comment='500 Hz for 800 bunches adjust to 1.5 KHz'),    },
    'L2_L1Calocalibration_L1BGRP7': { 0: TriggerRule(PS=1, comment='L1 Calo partial event building')},
    'EF_L1Calocalibration_L1BGRP7': { 0: TriggerRule(PS=-1, comment='L1 Calo partial event building')},

    #######################################
    # Cost monitoring
    'L2_cost_monitor' :  {    0: TriggerRule(propagate=False,PS=2000, comment='Cost monitoring'),    },
    'EF_cost_monitor' :  {    0: TriggerRule(propagate=False,PS=100, comment='Cost monitoring'),    },

    #######################################
    # Beamspot

    'L2_vtxbeamspot_activeTE_L2Star_B_peb': { 500 : TriggerRule(rate=3), },
    'L2_vtxbeamspot_FSTracks_L2Star_B'    : { 500 : TriggerRule(inputRate=1000),
                                              7500 : TriggerRule(PS=2), },
    'L2_vtxbeamspot_allTE_L2Star_B_peb'   : { 500 : TriggerRule(rate=30), },

    #--- Muon calibration stream
    'L2_mu15_l2cal':                  {10000: TriggerRule(PS=1, comment='Dedicated muon calibration stream') },
    'L2_mu4_l2cal_empty':              {3000: TriggerRule(PS=-1,comment='For cosmic runs only')},

    #######################################
    # ID Calib - copied over from 2011

    'L2_trk9_Central_Tau_IDCalib' :   {100: TriggerRule(PS=12, comment='IDTracks')    },
    'L2_trk9_Fwd_Tau_IDCalib'  :      {100: TriggerRule(PS=12, comment='IDTracks')    },
    'L2_trk16_Central_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk16_Fwd_Tau_IDCalib' :      {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk29_Central_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk29_Fwd_Tau_IDCalib' :      {100: TriggerRule(PS=1, comment='IDTracks')    },

    #######################################
    # Tile
    'EF_TileCalib_laser' : {100: TriggerRule(PS=1, comment='Tile calibration')    },

    ######################################
    # Cosmic Calo Stream
    'EF_j30_u0uchad_empty_LArNoiseBurst' : {    0: TriggerRule(PS=1, comment='LArNoiseBurst'),    },
    'EF_rd1_empty_NoAlg' :  {    0: TriggerRule(PS=1, comment='streamer for L1_RD1_EMPTY'),    },
    'EF_g10_NoCut_cosmic' : {    0: TriggerRule(PS=1, comment='cosmic chain for egamma'),    },
    'EF_tauNoCut_cosmic' :  {    0: TriggerRule(PS=1, comment='cosmic chain for taus'),    },
    'EF_L1J10_empty_NoAlg' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },
    'EF_L1J30_empty_NoAlg' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },
    'EF_L1FJ30_empty_NoAlg' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },
    'EF_PreS_L1EM6_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),},
    'EF_PreS_L1TAU8_empty' : {   0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),},
    'EF_PreS_L1J10_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),},
    'EF_PreS_L1J30_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),},
    
    }


# set prescales for L1 items
noL1PSitems=['L1_2MU6','L1_MU6_2J20','L1_TAU15_XE40','L1_EM14VH_FJ15','L1_2MU10','L1_3EM6_EM14VH','L1_MU10_FJ15','L1_TAU20_XE35','L1_2FJ30','L1_2MU4_BARRELONLY','L1_XE70','L1_2TAU11I_TAU15','L1_XE40_BGRP7','L1_MU15','L1_2TAU20','L1_3J15_FJ15','L1_2EM12_EM16V','L1_XE60','L1_2MU4_MU6_BARREL','L1_2FJ15','L1_TAU15_XE35','L1_2TAU11I_EM14VH','L1_XE50','L1_J50_XE35','L1_XE35_BGRP7','L1_2EM6_MU6','L1_2EM10VH','L1_XE40','L1_J350','L1_TAU15I_XE35','L1_2TAU8_TAU15_FJ15','L1_EM6_2MU6','L1_FJ75','L1_2TAU8_TAU11I_EM10VH_FJ15','L1_TAU20_XE40','L1_TAU8_MU10','L1_MU4_J20_XE35','L1_EM10VH_MU6','L1_J30_XE40','L1_2MU4_XE40','L1_TE800','L1_3MU4','L1_3MU6','L1_TAU11_MU10','L1_EM18VH','L1_2TAU11_TAU20_EM14VH','L1_2TAU8_TAU11I_EM10VH_J15CF','L1_3J15_J50','L1_2TAU8_TAU15_J15CF','L1_EM6_XS55','L1_MU10_J20','L1_EM30','L1_J50_XE40','L1_J30_XE50','L1_EM14VH_J15CF','L1_TAU15I_XE40','L1_XE50_BGRP7','L1_J75','L1_TAU40','L1_MU20','L1_MU10_J15CF','L1_EM16VH_MU4','L1_4J15', 'L1_FJ50' ]

# not FJ50 moved to noL1PS and FJ30 moved high
eb_high_physics = ['L1_2EM3_EM12', 'L1_2MU4', 'L1_MU4_J30', 'L1_XS60', 'L1_FJ30', 'L1_EM16V_XE20', 'L1_MU4_J50', 'L1_2EM6_EM16VH', 'L1_XS50', 'L1_2MU4_MU6', 'L1_2MU4_BARREL', 'L1_2MU4_2EM3'] 

eb_physics = [ 'L1_XS45', 'L1_J50', 'L1_MU4_J15',  'L1_MU10', 'L1_J30', 'L1_TE500', 'L1_EM10VH', 'L1_EM16VH', 'L1_TE700', 'L1_XE30', 'L1_XE20', 'L1_3J15', 'L1_TAU20', 'L1_MU6', 'L1_EM12'] 

for item in noL1PSitems:
    rules[item]= { 0: TriggerRule(PS=1,comment='noL1PS trigger')}
    
for item in eb_high_physics:
    rules[item]= { 2500: TriggerRule(PS=10,comment='eb high physics trigger'), 
                   900:  TriggerRule(PS=5,comment='eb high physics trigger'), 
                   0:    TriggerRule(PS=1,comment='eb high physics trigger')}

for item in eb_physics:
    rules[item]= { 2500: TriggerRule(PS=80,comment='physics trigger'),
                   900: TriggerRule(PS=40,comment='physics trigger'),
                   0: TriggerRule(PS=8,comment='physics trigger')}

# override rules for 
rules['L1_2EM3_EM12']={ 2500: TriggerRule(PS=10,comment='eb high physics trigger'), 
                        900:  TriggerRule(PS=5,comment='eb high physics trigger'), 
                        400:  TriggerRule(PS=2,comment='eb high physics trigger'), 
                        200:  TriggerRule(PS=1,comment='eb high physics trigger')}

########################################################
# add monitoring from Monitoring rules
########################################################
import Monitoring_pp_v4_rules
rules.update(Monitoring_pp_v4_rules.physics_rules)

# ZeroBias  --place holder
# EL: I'm not sure how to configure this, target should be 50 Hz
rules['L1_ZB'] =                    {     0 : TriggerRule(rate=50, comment='target is 50 Hz at 5e33'),    }
rules['EF_zerobias_NoAlg']=         {     0 : TriggerRule(PS=1, comment='Aim for 5 Hz'),}
rules['EF_zerobias_Overlay_NoAlg']= {     0 : TriggerRule(PS=-1, comment='')}

########################################################
# add background chains
########################################################

backgroundRules={
    'L1_LUCID_A_C_EMPTY' :           {    0 : TriggerRule(PS=100000, comment=''),    },
    'L1_LUCID_A_C_UNPAIRED_ISO' :    {    0 : TriggerRule(PS=200*2, comment=''),    },
    'L1_LUCID_A_C_UNPAIRED_NONISO' : {    0 : TriggerRule(PS=1000*2, comment=''),    },
    'L1_LUCID_EMPTY' :               {    0 : TriggerRule(PS=2500000, comment=''),    },
    'L1_LUCID_UNPAIRED_ISO' :        {    0 : TriggerRule(PS=10000*2, comment=''),    },
    'L1_LUCID_COMM_UNPAIRED_ISO' :   {    0 : TriggerRule(PS=-1, comment=''),    },
    'L1_LUCID_COMM_EMPTY' :          {    0 : TriggerRule(PS=-1, comment=''),    },
    'L1_MU4_UNPAIRED_ISO' :          {    0 : TriggerRule(PS=900*0.11*2, comment=''),    },
    'L1_BCM_Wide_EMPTY' :            {    0 : TriggerRule(PS=10, comment=''),    },
    'L1_BCM_Wide_UNPAIRED_ISO' :     {    0 : TriggerRule(PS=1, comment=''),    },
    'L1_BCM_Wide_UNPAIRED_NONISO' :  {    0 : TriggerRule(PS=1, comment=''),    },
    'L1_BCM_AC_CA_BGRP0' :           {    0 : TriggerRule(PS=100000, comment=''),    },
    'L1_BCM_AC_CA_UNPAIRED_ISO' :    {    0 : TriggerRule(PS=1, comment=''),    },
    'L1_EM3_UNPAIRED_ISO' :          {    0 : TriggerRule(PS=1, comment=''),    },
    'L1_FJ30_UNPAIRED_ISO' :         {    0 : TriggerRule(PS=-1, comment=''),    },
    'L1_J10_UNPAIRED_ISO' :          {    0 : TriggerRule(PS=1, comment=''),    },
    'L1_J10_UNPAIRED_NONISO' :       {    0 : TriggerRule(PS=1, comment=''),    },
    'EF_Background':                 {    1 : TriggerRule(PS=1, comment=''),     },
    'EF_Background_RD0_UNPAIRED_ISO':{    1 : TriggerRule(PS=5*500/770, comment='target 1 Hz'),     },
    'EF_Background_RD0_EMPTY':       {    1 : TriggerRule(PS=16*40000/61600, comment='target 1 Hz'),     },
    }

rules=dict(rules,**backgroundRules)

########################################################
# Rules for the Enhanced bias triggers (savannah #97241)
########################################################

if EBrun:
    rules['EF_eb_physics_noL1PS']=          { 
                                              500  : TriggerRule(rate=30, comment='EnhancedBias chains'), 
                                            }
    
    #Seeded by RD0 seeded
    rules['EF_eb_physics']=                 { 500 : TriggerRule(PS=4, comment='EnhancedBias chains'), }
    
    #Seeded by RANDOM
    rules['EF_high_eb_physics']=            { 10000 : TriggerRule(PS=1, comment='EnhancedBias chains'),}
    
    #Over-write rules of standard triggers    
    rules['L1_RD0_FILLED']=                  {10000: TriggerRule(rate=5000,scaling="bunches",comment='seed for EnhancedBias run')}
    
    # assume scales with empty bgrps???
    rules['EF_eb_physics_empty']=           { 500 : TriggerRule(PS=160, comment='EnhancedBias chains'), }
    
    
    rules['EF_eb_physics_firstempty']=      { 500 : TriggerRule(PS=15, comment='EnhancedBias chains'), }
    
    
    rules['EF_eb_physics_unpaired_iso']=    { 500 : TriggerRule(PS=1, comment='EnhancedBias chains'), }
    
    
    rules['EF_eb_physics_unpaired_noniso']= { 500 : TriggerRule(PS=1, comment='EnhancedBias chains'), }
    
    rules['L1_RD1_FILLED']=                 { 10000: TriggerRule(rate=100,scaling="bunches",comment='200 Hz')} # PS=20350?
     

    rules['EF_eb_random']=                  { 100 : TriggerRule(rate=100, comment='EnhancedBias chains'), }
    
    
    #Seeded by L1_RD0_EMPTY, rate of 9 Hz
    rules['EF_eb_random_empty']=            { 100 : TriggerRule(rate=2, comment='2 Hz of EnhancedBias chains'), }
    
    rules['L1_RD0_EMPTY']=                   {10000: TriggerRule(rate=10,comment='set to 10 Hz with 350 empty bunches ')}
    
    
    #Seeded by L1_RD0_FIRSTEMPTY, rate of 2 Hz
    rules['EF_eb_random_firstempty']=       { 100 : TriggerRule(rate=2, comment='2 Hz of EnhancedBias chains'), }
    
    rules['L1_RD0_FIRSTEMPTY']=              {10000: TriggerRule(rate=2,comment='set to 2 Hz with full ring ')}
    rules['EF_L1RD0_firstempty_NoAlg']=      {10000: TriggerRule(PS=17,comment='Enhanced bias menu - long-lived particles') } #Hardcoded PS, adjust to RD0_FIRSTEMPY rate
    
    #Seeded by L1_RD0_UNPAIRED_ISO, rate of 5 Hz
    rules['EF_eb_random_unpaired_iso']=     { 100 : TriggerRule(rate=2, comment='2 Hz of EnhancedBias chains'), }
    
    rules['L1_RD0_UNPAIRED_ISO']=            {10000: TriggerRule(rate=5,comment='set to 5 Hz with 25 unpaired bunches ')}



# disable all others

disablelist = [
# 2MU6 seeded   
'EF_2mu6',
'EF_2mu6_Bmumu',
'EF_2mu6_Bmumu_L2StarB',
'EF_2mu6_Bmumux',
'EF_2mu6_Bmumux_BcMuMuDs_loose_L2StarB',
'EF_2mu6_Bmumux_L2StarA',
'EF_2mu6_Bmumux_v2',
'EF_2mu6_Bmumux_v2_L2StarB',
'EF_2mu6_DiMu',
'EF_2mu6_DiMu_DY20',
'EF_2mu6_DiMu_DY25',
'EF_2mu6_DiMu_L2StarB',
'EF_2mu6_DiMu_noVtx_noOS',
'EF_2mu6_DiMu_noVtx_noOS_L2StarB',
'EF_2mu6_Jpsimumu',
'EF_2mu6_Jpsimumu_L2StarB',
'EF_2mu6_Upsimumu',
'EF_2mu6_Upsimumu_L2StarB',
'EF_2mu6i_DiMu_DY',
'EF_2mu6i_DiMu_DY_2j25_a4tchad',
'EF_2mu6i_DiMu_DY_noVtx_noOS',
'EF_2mu6i_DiMu_DY_noVtx_noOS_2j25_a4tchad',
]

for rule in rules.keys():
    print "Have rule for ",rule
