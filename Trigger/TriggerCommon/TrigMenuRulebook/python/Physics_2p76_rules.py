# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Physics_pp'

#some handy shortcuts to modify rules (see bottom for code)
disableEndOfFill=False
disablePriority1=False
disablePriority2=False
disableStartup=False
disablePending=True
disableNonCollision=False
disableStreamers=True

firstBeam=True
if firstBeam:
  disableEndOfFill=True
  disableStartup=True
  disableStreamers=False

rules = {
    #####################################################################
    # Everything by default is disabled.
    # primary 1e33

    # low pt eff
  'L1_MU0': {1000: TriggerRule(PS=1, comment='main HI trigger')}, 
  'L1_MU6': {1000: TriggerRule(PS=1, comment='main HI trigger')}, 
  'L1_MU10': {1000: TriggerRule(PS=1, comment='main HI trigger')}, 
  'L1_MU11': {1000: TriggerRule(PS=1, comment='main HI trigger')}, 
  'L1_MU15': {1000: TriggerRule(PS=1, comment='main HI trigger')}, 
  'L1_MU20': {1000: TriggerRule(PS=1, comment='main HI trigger')}, 

  # creates Muon stream which overlaps with L1Muon
  #'EF_mu4':            {1000: TriggerRule(PS=1, comment='Back-up chain for L1_MU0')},
  #'L2_mu4':            {1000: TriggerRule(PTRate=1.0, comment='')},
  #'L2_mu10_cal':       { 500: TriggerRule(PS=1, comment='muon calibration; PEB',),},

    # beam background - most of these should all be low rate and get PS=1 by construction
###BRIAN don't know rates with trains    'EF_mu0_firstempty_NoAlg':    { 500: TriggerRule(PS=1300, comment='beam background',),
###BRIAN  don't know rates with trains                                  501: TriggerRule(rate=0.5, comment='beam background',),}, #501 is real rule, 500 is for startup
###BRIAN don't know rates with trains   'EF_L1MU10_firstempty_NoAlg': { 500: TriggerRule(PS=140, comment='beam background',),
###BRIAN don't know rates with trains                                   501: TriggerRule(rate=0.5, comment='beam background',),},

    'EF_mu0_empty_NoAlg':         { 500: TriggerRule(rate=0.5, comment='beam/cosmic background',),},
    'EF_mu0_unpaired_iso_NoAlg':  { 500: TriggerRule(rate=0.5, comment='beam/cosmic background',),},
    #'EF_mu20_empty':              { 500: TriggerRule(rate=0.5, comment='beam/cosmic background',),},
    #'EF_2mu10_loose_empty':       { 500: TriggerRule(rate=0.5, comment='beam/cosmic background',),},
    #'EF_2mu10_empty':             { 500: TriggerRule(rate=0.5, comment='beam/cosmic background',),},
    #'EF_mu40_MSonly_empty':       { 500: TriggerRule(rate=0.5, comment='beam/cosmic background',),},


    #'EF_tauNoCut_L1TAU50': {1000:  TriggerRule(rate=0.6,ESValue=8)}, #temporary workaround ESRate=0.6)},
    #'L2_tauNoCut_L1TAU50': {1000:  TriggerRule(PTRate=5)},
    #'EF_tauNoCut':      {1000: TriggerRule(rate=1,ESValue=8)}, #ESRate=1.0)},
    #'L2_tauNoCut':      {1000: TriggerRule(PTRate=5)},

    
    #######################################################################
    # Jets
    # CENTRAL JETS - low-lumi rules (L=1) are only meant for startup phase
    # force J75 to remain unprescaled for jet+xe triggers

  'L1_J10' : {100: TriggerRule(PS=1,comment='Low energy primary')},
  'L1_J15' : {100: TriggerRule(PS=1,comment='Low energy primary')},
  'L1_J20' : {100: TriggerRule(PS=1,comment='Low energy primary')},
  'L1_J30' : {100: TriggerRule(PS=1,comment='Low energy primary')},
  'L1_J50' : {100: TriggerRule(PS=1,comment='Low energy primary')},
  'L1_J75' : {100: TriggerRule(PS=1,comment='Low energy primary')},
  'L1_J175' : {100: TriggerRule(PS=1,comment='Low energy primary')},
  'L1_J250' : {100: TriggerRule(PS=1,comment='Low energy primary')},

  'L1_FJ10' : {100: TriggerRule(PS=1,comment='Low energy primary')},
  'L1_FJ30' : {100: TriggerRule(PS=1,comment='Low energy primary')},
  'L1_FJ50' : {100: TriggerRule(PS=1,comment='Low energy primary')},
  'L1_FJ75' : {100: TriggerRule(PS=1,comment='Low energy primary')},

  'L1_3J10':{500:TriggerRule(PS=1)},
  'L1_4J10':{500:TriggerRule(PS=1)},
  'L1_5J10':{500:TriggerRule(PS=1)},
  'L1_6J10':{500:TriggerRule(PS=1)},

  #'EF_hadCalib_trk9':{   2:TriggerRule(rate=25.,comment='Jet calibration'),},
  #'EF_hadCalib_trk9phi':{   2:TriggerRule(rate=25.,comment='Jet calibration'),},


    ############################################################
    # minbias


  'EF_zerobias_NoAlg': { 100 : TriggerRule(rate=1, comment='zeroBias, prescaled primary'), },
  
  'L1_TE20': {1000: TriggerRule(PS=1,comment="for ridge CMS-like; end-of-fill trigger")          },
  'EF_mbSpTrkVtxMh': { 100 : TriggerRule(rate=0.5, comment='zeroBias, prescaled primary; end-of-fill trigger'), },

  # expect 64 bunches with 2kHz of MBTS_1 per bunch
  'EF_mbMbts_1_eff':     {100: TriggerRule(PS=700,ESRate=3,comment="old primary")},
  'EF_mbMbts_1_eff_unpaired_iso':     {100: TriggerRule(PS=1,comment="background studies for L1_MBTS_1")},
  'L2_mbMbts_2_Monitoring':     {100: TriggerRule(PS=1,comment="monitoring of the min bias sample")},
  'EF_mbLucid_eff':      {100: TriggerRule(rate=2.,comment="TBC"),},# 500: TriggerRule(PS=-1.0,)},
  'EF_mbZdc_eff':        {100: TriggerRule(rate=2.,comment="standard Minbias analysis")},
  'EF_InDetMon_FS':      {100: TriggerRule(rate=0.4,ESRate=0.4,comment="Trigger monitoring")},
     
  'L1_RD0_FILLED':       {100: TriggerRule(scaling="bunches")},  
  'EF_mbSpTrk':           {100: TriggerRule(rate=0.5,comment="standard Minbias analysis")},
  'EF_rd0_filled_NoAlg':  {100: TriggerRule(rate=2,ESValue=5.0,comment="standard Minbias analysis")}, #should be rate, not PS     
  'EF_mbRmMbts_1_NoCut_Time': {100: TriggerRule(rate=0.5,comment="standard Minbias analysis")},#should be rate, not PS
    
  'L1_BCM_Wide_BGRP0': {100: TriggerRule(PS=600000)}, 
  'EF_mb_BCM_Wide_eff': {100: TriggerRule(PS=1)}, 
  #     'EF_mb_BCM_HT_eff': {100: TriggerRule(PS=1000000)},
  #     'EF_mb_BCM_AC_CA_eff': {100: TriggerRule(PS=200000)},


    'EF_mbSpBg_unpaired_iso': {100: TriggerRule(PS=1)},
    'EF_mbRd0_eff_unpaired_iso': {100: TriggerRule(PS=1)},
    'EF_mbSpTrk_unpaired_iso': {100: TriggerRule(PS=1)},


    'L1_LUCID_A_C_UNPAIRED_NONISO':     {500: TriggerRule(PS=1,)},
    'L1_ZB':                            {500: TriggerRule(PS=10,)},

    'L1_RD0_FIRSTEMPTY':     {500: TriggerRule(PS=-40000.0,comment="TBC")},

    'L1_MBTS_2':      {500: TriggerRule(PS=10000,)},     
    'L1_MBTS_1_1':    {500: TriggerRule(PS=10000,)},     
    'L1_MBTS_4_4':     {500: TriggerRule(PS=10000,)}, 

######################################################
    # Cosmic, detector, etc

    #L1 items
    'L1_RD0_EMPTY' : { 100 : TriggerRule(scaling="bunches",comment='cosmic'), }, 
    'L1_TRT' : {100: TriggerRule(PS=-1, comment='primary for cosmic with TRT Fast Or') },
    #HLT
    'L2_PixelNoise' : {100: TriggerRule(rate=10, comment='calibration for InDet')    },
    'L2_SCTNoise' : {100: TriggerRule(rate=10, comment='calibration for InDet')    },
    'EF_rd0_empty_NoAlg' : {100: TriggerRule(rate=1,ESRate=0.5, comment='streamer for L1_RD0_EMPTY')    },
    'EF_L1TRT_NoAlg' : {100: TriggerRule(PS=-1, comment='streamer for L1_TRT')    },
    'EF_PreS_EM_tight' : {100: TriggerRule(PS=50, comment='rejection of pre-sampler noise in LAr') },
    'EF_PreS_EM_loose' : {100: TriggerRule(PS=10, comment='backup for PreS_tight')    },
    'EF_PreS_J_loose' : {100: TriggerRule(PS=10, comment='backup for PreS_tight')    },
    'EF_PreS_J_tight' : {100: TriggerRule(PS=10, comment='backup for PreS tight') },
    'EF_PreS_TAU_loose' : {100: TriggerRule(PS=90, comment='backup for PreS_tight')    },
    'EF_PreS_TAU_tight' : {100: TriggerRule(PS=90, comment='backup for PreS tight') },
    'EF_mu4_cosmic' : {100: TriggerRule(PS=-1, comment='cosmic chain for muons')    },
    'EF_mu4_MSonly_cosmic' : {100: TriggerRule(PS=-1, comment='cosmic chain for muons') },
    #'EF_mu11_empty_NoAlg' : {100: TriggerRule(rate=2, comment='cosmic chain for muons')    },
    #'EF_mu4_L1MU11_cosmic' : {100: TriggerRule(PS=1, comment='cosmic chain for muons')    }, #to match online
                                               #rate=0.1, comment='cosmic chain for muons')    },
    #'EF_mu4_L1MU11_MSonly_cosmic' : {100: TriggerRule(rate=0.1, comment='cosmic chain for muons')    },
    'EF_TileCalib_laser' : {100: TriggerRule(PS=1, comment='Tile calibration')    },
    'EF_CosmicsAllTeIDSCAN_AllPhysics_TrkHypo' : {100: TriggerRule(PS=-1,  comment='cosmic chain for InDet studies')    },
    'EF_CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo' : {100: TriggerRule(PS=-1, comment='cosmic chain for InDet studies')    },
    'EF_CosmicsAllTeSiTrack_AllPhysics_TrkHypo' : {100: TriggerRule(PS=-1,  comment='cosmic chain for InDet studies')    },
    'EF_CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo' : {100: TriggerRule(PS=-1,  comment='cosmic chain for InDet studies')    },
    'EF_CosmicsAllTeTRTxK_TRTTrkHypo_Pixel' : {100: TriggerRule(PS=-1,  comment='cosmic chain for InDet studies')    },
    'EF_CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics' : {100: TriggerRule(PS=-1, comment='cosmic chain for InDet studies')    },
    'EF_Cosmics_MU11_TRTxK_TRTTrkHypo_AllID': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
    'EF_Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel': { 100 : TriggerRule(PS=1, comment='cosmic'), },
    'EF_mu11_empty_NoAlg': { 100 : TriggerRule(PS=100000, comment='cosmic'), },
    'EF_CosmicsAllTeEFID' : {100: TriggerRule(PS=-1,comment='cosmic chain for InDet studies')    },
    'EF_g5_NoCut_cosmic' : { 100: TriggerRule(PS=50, comment='cosmic chain for egamma')    },
                            #100: TriggerRule(rate=0.1, comment='cosmic chain for egamma')    },


    'L2_em3_empty_larcalib' :  {100: TriggerRule(PS=1, comment='cosmic chain for LAr')    },
    'L2_tau5_empty_larcalib' : {100: TriggerRule(PS=1, comment='cosmic chain for LAr')    },
    'L2_j10_empty_larcalib' :  {100: TriggerRule(PS=1, comment='cosmic chain for LAr')    },
    'L2_fj10_empty_larcalib' : {100: TriggerRule(PS=1, comment='cosmic chain for LAr')    },
    'L2_L1J10_empty_NoAlg' :   {100: TriggerRule(PS=1, comment='cosmic chain for LAr')    },
    'L2_L1FJ10_empty_NoAlg' :  {100: TriggerRule(PS=1, comment='cosmic chain for LAr')    },

    'EF_g11_etcut_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g11_etcut',comment='LAr PEB calibration stream')},
    'EF_g15_loose_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g15_loose',comment='LAr PEB calibration stream')},
    'EF_g20_loose_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g20_loose',comment='LAr PEB calibration stream')},
    'EF_g40_loose_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g40_loose',comment='LAr PEB calibration stream')},
    'EF_g60_loose_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g60_loose',comment='LAr PEB calibration stream')},
    'EF_g80_loose_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g80_loose',comment='LAr PEB calibration stream')},

    'EF_j25_larcalib'      :  {100: TriggerRule(rate=5.0,comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    'EF_j50_larcalib'      :  {100: TriggerRule(rate=5.0,comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    'EF_j95_larcalib'      :  {100: TriggerRule(rate=5.0,comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    'EF_fj25_larcalib'      :  {100: TriggerRule(rate=5.0,comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    'EF_fj50_larcalib'      :  {100: TriggerRule(rate=5.0,comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz

    'EF_TileCalib_laser' : {100: TriggerRule(PS=1, comment='Tile calibration')    },

    'L2_trk9_Central_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk9_Fwd_Tau_IDCalib'  :  {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk16_Central_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk16_Fwd_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk29_Central_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk29_Fwd_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },

    'EF_L1J30_empty_NoAlg'        :{100:  TriggerRule(PS=1, comment='Baseline primary; long-lived particles')    },
    #'EF_L1J10_firstempty_NoAlg'   :{#real rule##100:  TriggerRule(rate=1.0,ESRate=1.0, comment='Calorimeter noise monitoring'),
  #500: TriggerRule(rate=1,ESRate=0.1,scaling="bunches", comment="beam background")}, #temporary hack
    #'EF_L1J30_firstempty_NoAlg'   :{#real rule##100:  TriggerRule(rate=1.0,ESRate=1.0, comment='Baseline primary; long-lived particles')
  #500:  TriggerRule(PS=1,ESRate=0.1, comment='Baseline primary; long-lived particles')    },#temporary hack
  'EF_L1J30_unpaired_NoAlg'     :{#real rule##100:  TriggerRule(rate=0.5, comment='Baseline primary; long-lived particles'),
                              500:  TriggerRule(PS=1, comment='Baseline primary; long-lived particles')    },#temporary hack

    
     
    'EF_ROBRequestMon': { 100 : TriggerRule(PS=-1, comment='test trigger'), },
    'EF_rd1_empty_NoAlg': { 100 : TriggerRule(PS=-1, comment='random trigger'), }, 
    'EF_mbRd1_eff': { 100 : TriggerRule(PS=10000000, comment='random triggers'), },     #these should be rate driven

    'EF_L1Muon_NoAlg': { 100 : TriggerRule(PS=1, ESRate=3.,comment='Streamers'), },
    'EF_L1Calo_NoAlg': { 100 : TriggerRule(PS=1, ESRate=3., comment='Streamers'), },  ## Should get ~200 Hz
    'EF_L1MinBias_NoAlg': { 100 : TriggerRule(PS=-1, comment='Streamers'), }, ## Should get ~200 Hz

# Beamspot
#     'L2_vtxbeamspot_activeTE_SiTrk_peb': { 500 : TriggerRule(PS=1), },
#     'L2_vtxbeamspot_FSTracks_SiTrk'    : { 500 : TriggerRule(PS=1), },
#     'L2_vtxbeamspot_allTE_SiTrk_peb'   : { 500 : TriggerRule(PS=1), },

		'L1_MBTS_2' : {500 : TriggerRule(rate=1000) },
		'L2_vtxbeamspot_activeTE_SiTrk_peb_MBTS': { 500 : TriggerRule(PS=1), },
		'L2_vtxbeamspot_FSTracks_SiTrk_MBTS'    : { 500 : TriggerRule(PS=40), },
		'L2_vtxbeamspot_allTE_SiTrk_peb_MBTS'   : { 500 : TriggerRule(PS=40), },		
		
    ##############################################################
    #  EB chains
   'EF_eb_random':                  { 100 : TriggerRule(PS=-1, comment='cosmic'), },
   'EF_eb_random_empty':            { 100 : TriggerRule(PS=-1, comment='cosmic'), },
   'EF_eb_random_firstempty':       { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
   'EF_eb_random_unpaired_iso':     { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
   'EF_eb_physics_empty':           { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
   'EF_eb_physics_firstempty':      { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
   'EF_eb_physics_unpaired_iso':    { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
   'EF_eb_physics_unpaired_noniso': { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
   'EF_eb_physics':                 { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
   'EF_high_eb_physics':            { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },

    ##############################################################
    # Streamers

    'L2_Background': { 100 : TriggerRule(PS=1, comment='cosmic'),
                       1000 : TriggerRule(PS=100000, comment='cosmic'), },
    'EF_SeededStreamerL1CaloXS': { 100 : TriggerRule(PS=-1, comment='Commissioning'), },
    #'L2_Standby': { 100 : TriggerRule(PS=-1, comment='cosmic'), },

    'L2_cost_monitor':  { 100: TriggerRule(PS=200, propagate=False, comment="Trigger monitoring"), },
    'EF_cost_monitor':  { 100: TriggerRule(PS=100, propagate=False, comment="Trigger monitoring"), },

    ##############################################################
    # Set scaling for non-lumi scaling items:
    'L1_J30_UNPAIRED':     {500: TriggerRule(PS=-1,scaling="bunches", comment="beam background") },
#    'L1_J30_FIRSTEMPTY':   {500: TriggerRule(PS=5,scaling="bunches", comment="beam background") },
#    'L1_J10_FIRSTEMPTY':   {500: TriggerRule(PS=5,scaling="bunches", comment="beam background") },
#    'L1_MU0_FIRSTEMPTY':   {500: TriggerRule(PS=1300,scaling="bunches", comment="beam background") },
    'L1_MU6_FIRSTEMPTY':   {500: TriggerRule(PS=200,scaling="bunches", comment="beam background") },
    'L1_2MU6_FIRSTEMPTY':   {500: TriggerRule(PS=3,scaling="bunches", comment="beam background") },
    'L1_2MU0_FIRSTEMPTY':   {500: TriggerRule(PS=45,scaling="bunches", comment="beam background") },
#    'L1_MU10_FIRSTEMPTY':  {500: TriggerRule(PS=140,scaling="bunches", comment="beam background") },
    'L1_EM3_FIRSTEMPTY':   {500: TriggerRule(PS=100,scaling="bunches", comment="beam background") },
    'L1_TAU5_FIRSTEMPTY':   {500: TriggerRule(PS=30,scaling="bunches", comment="beam background") },
    'L1_FJ10_FIRSTEMPTY':   {500: TriggerRule(PS=1,scaling="bunches", comment="beam background") },
#    'L1_RD0_EMPTY':        {500: TriggerRule(scaling="bunches", comment="beam background") },
    'L1_RD1_EMPTY':        {500: TriggerRule(PS=-1,scaling="bunches", comment="beam background") },


    'L1_MU0_J15_FIRSTEMPTY':  {100: TriggerRule(PS=1) },
    'L1_MU0_UNPAIRED_NONISO': { 500 : TriggerRule(PS=1), },
    'L1_EM3_UNPAIRED_NONISO': { 500 : TriggerRule(PS=1), },
    'L1_TAU5_UNPAIRED_NONISO': { 500 : TriggerRule(PS=1), },
    'L1_J10_UNPAIRED_NONISO': { 500 : TriggerRule(PS=1), },
    ##############################################################
    # DISABLE FOR RATE CALCULATION
#    'L1_MU0_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
#    'L1_2MU0_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
#    'L1_2MU6_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
#    'L1_MU10_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
#    'L1_MU11_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
#    'L1_TAU5_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
#    'L1_EM3_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
#    'L1_J30_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
#    'L1_J10_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
#    'L1_FJ10_EMPTY': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
#    'L1_TE500': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
    'L1_RD0_UNPAIRED_ISO': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
    'L1_MU0_UNPAIRED_ISO': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
#    'L1_multiseed': { 100 : TriggerRule(PS=1,comment='needed for rulebook'),},



 }


#disable end-of-fill triggers
if disableEndOfFill:
  for r in rules:
    keys=rules[r].keys()
    for k in keys:
        if 'comment' in rules[r][k].keys() and ('end-of-fill' in rules[r][k]['comment']):
            rules[r][k]['PS']=-1
#disable priority 1 triggers
if disablePriority1:
  for r in rules:
    keys=rules[r].keys()
    for k in keys:
        if 'comment' in rules[r][k].keys() and ('Priority 1' in rules[r][k]['comment']):
            rules[r][k]['PS']=-1

#disable priority 2 triggers
if disablePriority2:
  for r in rules:
    keys=rules[r].keys()
    for k in keys:
        if 'comment' in rules[r][k].keys() and ('Priority 2' in rules[r][k]['comment']):
            rules[r][k]['PS']=-1

#disable pending (non-approved) triggers
if disablePending:
  for r in rules:
    keys=rules[r].keys()
    for k in keys:
        if 'comment' in rules[r][k].keys() and ('Pending' in rules[r][k]['comment']):
            rules[r][k]['PS']=-1

#disable startup fill triggers
if disableStartup:
  for r in rules:
    keys=rules[r].keys()
    if 1 in keys:
      del rules[r][1]


#disable unpaired and firstempty
if disableNonCollision:
  for r in rules:
    if 'unpaired' in r or 'firstempty' in r:
      keys=rules[r].keys()
      for k in keys:
        rules[r][k]['PS']=-1

#disable unpaired and firstempty
if disableStreamers:
  for r in rules:
    if 'Streamers' in r:
      keys=rules[r].keys()
      for k in keys:
        rules[r][k]['PS']=-1


# While we don't have ISO bunches
#for r in rules:
#  if 'unpaired_iso' in r:
#    keys=rules[r].keys()
#    for k in keys:
#      rules[r][k]['PS']=-1
      

#print rules['EF_j10_a4_EFFS']
