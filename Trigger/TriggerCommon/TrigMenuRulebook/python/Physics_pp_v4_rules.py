# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Physics_pp_v4'

EBrun=False

rules = {

    ################################################################################
    # L1 Rules for new L1 items (23.08.2012)
    #
    
    'L1_XE40': { 1000 : TriggerRule(PS=1, comment=''),
                 8001 : TriggerRule(PS=-1, comment='Disable for greater than 8e33'),
                 },    
    'L1_XE50': { 1000 : TriggerRule(PS=1, comment=''),
                 9001 : TriggerRule(PS=-1, comment='Disable for greater than 9e33'),
                 },    

    'L1_XE35': { 1000 : TriggerRule(PS=1, comment=''),
                 6200 : TriggerRule(PS=-1, comment=''),
                 },

    'L1_XE35_BGRP7': { 1000 : TriggerRule(PS=1, comment=''),
                       5701 : TriggerRule(PS=-1, comment='Emergency prescale'),
                       },

    'L1_2MU4_MU6': {1000: TriggerRule(PS=1.0, comment='end-of-fill BPhys'),
                    7401: TriggerRule(PS=-1, comment='disable at peak'),
                    }, 

    'L1_2MU4_MU6_BARREL': {1000: TriggerRule(PS=1.0, comment='end-of-fill BPhys'),
                           7500: TriggerRule(PS=-1, comment='disable at peak'),
                           },

    'L1_2MU4_BARRELONLY': {1000: TriggerRule(PS=1.0, comment='end-of-fill BPhys'),
                           8501: TriggerRule(PS=-1,  comment='disable at peak (>8.5e33)'),
                           },

    'L1_2FJ15' : {1000: TriggerRule(PS=1, comment='VBF for H->bb'),
                  5701: TriggerRule(PS=-1, comment='unstable L1 rate')},
    'L1_3J15_FJ15' : {1000: TriggerRule(PS=1, comment='VBF for H->bb'),
                      5701: TriggerRule(PS=-1, comment='unstable L1 rate')},


    'L1_2MU4_2EM3': {1000: TriggerRule(PS=1.0, comment='end-of-fill BPhys'),
                     6501: TriggerRule(PS=-1, comment='disable at peak'),
                     },

    'L1_EM6_MU10': {1000: TriggerRule(PS=1.0, comment='tau to mu gamma'),
                    6501: TriggerRule(PS=-1, comment='disable at peak'),
                    },

    'L1_2MU4_BARREL': {1000: TriggerRule(PS=1.0, comment='end-of-fill BPhys'),
                       6501: TriggerRule(PS=-1, comment='disable at peak'),
                       },


    'L1_EM14VH_FJ15' : {1000: TriggerRule(PS=1.0, comment='VBF single electron'),
                        5701: TriggerRule(PS=-1, comment='disable at peak'),},
    'L1_MU10_FJ15' : {1000: TriggerRule(PS=1.0, comment='VBF single muon'),
                      5701: TriggerRule(PS=-1, comment='disable at peak'),},
    'L1_2TAU8_TAU11I_EM10VH_FJ15' : {1000: TriggerRule(PS=1.0, comment='VBF e+tau'),
                                     5701: TriggerRule(PS=-1, comment='disable at peak'),},
    'L1_EM14VH_J15CF' : {1000: TriggerRule(PS=1.0, comment='VBF single electron'),
                         5701: TriggerRule(PS=-1, comment='disable at peak'),},
    'L1_MU10_J15CF' : {1000: TriggerRule(PS=1.0, comment='VBF single muon'),
                       5701: TriggerRule(PS=-1, comment='disable at peak'),},
    'L1_2TAU8_TAU11I_EM10VH_J15CF' : {1000: TriggerRule(PS=1.0, comment='VBF e+tau'),
                                      5701: TriggerRule(PS=-1, comment='disable at peak'),}, 
    

    ################################################################################
    # new chains for 17.1.5.14 using XE35, XE35_BGRP7: disabled
    'EF_j40_a4tchad_xe60T_tclcw_loose_dphi2j45xe10_delayed': { 1000 : TriggerRule(PS=-1, comment=''),},
    'EF_j40_a4tchad_xe60_tclcw_loose_dphi2j45xe10_delayed' : { 1000 : TriggerRule(PS=-1, comment=''),},


    ################################################################################
    # Primaries for  Muons:
    #
    #--- Single muon triggers
    'EF_mu24i_tight':                 {10000: TriggerRule(PS=1,ESRate=0.3, comment='Baseline trigger') },
    'EF_mu36_tight':                  {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_mu40_tight':                  {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
    'EF_mu50_MSonly_barrel_tight':    {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    #--- Single-muon-seeded EFFS multi muon triggers
    'EF_mu18_tight_mu8_EFFS':         {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_mu18_tight_2mu4_EFFS':        {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    #--- Multi muon triggers
    'EF_2mu13':                       {10000: TriggerRule(PS=1,ESRate=0.3,comment='Baseline trigger') },
    'EF_2mu15':                       {10000: TriggerRule(PS=1,comment='Backup baseline trigger') },
    'EF_3mu6_MSonly':                 {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_3mu6_MSonly_EMPTY':           {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_3mu6_MSonly_UNPAIRED_ISO':    {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_3mu6':                        {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    #--- Slow triggers
    #just for first run with 17.1.5.14!!!
    'L2_mu60_slow_tight1': {1000: TriggerRule(PS=1.0, comment='Slow particle search'),
                            5701: TriggerRule(PS=-1, comment='disable at peak'),
                            }, 
    'EF_mu60_slow_tight1':            {10000: TriggerRule(PS=1,comment='Baseline trigger') },


    'EF_mu60_slow_outOfTime_tight1':  {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    # -- Muon (+jet) + mET triggers
    'EF_mu24_j65_a4tchad_EFxe60_tclcw':          {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_mu4T_j65_a4tchad_xe70_tclcw_veryloose':  {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    # -- Muon + electron triggers
    'EF_mu18_tight_e7_medium1':       {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    #New low pt electron L2StarB chains: 17.1.5.21
    'EF_mu18_tight_e7_medium1_L2StarB': {10000: TriggerRule(PS=1.0,comment='Alternative Baseline trigger') }, 

    # -- Muon + photon triggers
    'EF_mu24_g20vh_medium':            {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_mu18_2g10_medium':             {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    # -- Muon + MET - May 18th
    'EF_2mu8_EFxe40_tclcw':            {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_2mu4T_xe60_tclcw':             {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    # -- Muon + Jets - May 18th
    'EF_mu24_tight_4j35_a4tchad':      {10000: TriggerRule(PS=1.0, comment='Baseline & Support for Top background evaluation') },

    #--- Single-muon-seeded EFFS multi muon with lower threshold - May 18th
    'EF_mu24_tight_mu6_EFFS':          {10000: TriggerRule(PS=1.0, comment='Baseline & Extra trigger for EFFS (lower pT)') },

    'EF_2mu6i_DiMu_DY_2j25_a4tchad':   {10000: TriggerRule(PS=1.0, comment='Baseline & Extra trigger for low mass Higgs') },


    # Disabled - May 18th
    'EF_2mu4T_xe60':                             {10000: TriggerRule(PS=-1,comment='Baseline trigger') },
    'EF_2mu8_EFxe30':                            {10000: TriggerRule(PS=-1,comment='Baseline trigger') },
    'EF_mu24_tight_EFxe40':                      {10000: TriggerRule(PS=-1,comment='Baseline trigger') },





    ########################################
    # Support Triggers for Muons:
    #
    #--- Background extraction
    'EF_mu24_tight':                  {10000: TriggerRule(PS=10, rerun=True, comment='Support for isolated trigger') },

    #--- Muon calibration stream
    'L2_mu15_l2cal':                  {10000: TriggerRule(PS=1, comment='Dedicated muon calibration stream') },

    # -- Muon DQ monitoring
    'EF_mu18_IDTrkNoCut_tight':       {10000:  TriggerRule(rate=0.1, ESValue=1, comment = 'Muon DQ in Express stream')},
    'EF_mu22_IDTrkNoCut_tight':       {10000:  TriggerRule(rate=0.5, ESValue=1, comment = 'Muon DQ in Express stream')},
    'EF_mu24_muCombTag_NoEF_tight':   {10000:  TriggerRule(rate=0.2, ESValue=1, comment = 'Muon DQ in Express stream')},
    'EF_mu18it_tight':                {10000:  TriggerRule(rate=0.2, ESValue=1, comment = 'Muon DQ in Express stream')},

    # -- For special toroid off runs
    'EF_mu10_muCombTag_NoEF_tight1' : { 10000 : TriggerRule(PS=-1, comment = 'Toroid off runs'), },
    'EF_mu15_muCombTag_NoEF_tight1' : { 10000 : TriggerRule(PS=-1, comment = 'Toroid off runs'), },
    'EF_mu20_muCombTag_NoEF_tight1' : { 10000 : TriggerRule(PS=-1, comment = 'Toroid off runs'), },
    'EF_mu10_muCombTag_NoEF_tight'  : { 10000 : TriggerRule(PS=-1, comment = 'Toroid off runs'), },
    'EF_mu15_muCombTag_NoEF_tight'  : { 10000 : TriggerRule(PS=-1, comment = 'Toroid off runs'), },
    'EF_mu20_muCombTag_NoEF_tight'  : { 10000 : TriggerRule(PS=-1, comment = 'Toroid off runs'), },

    #--- Muon + Jets for Background extraction
    'EF_mu24_tight_3j35_a4tchad':      {10000: TriggerRule(rate=1.0, comment='Support for Top background evaluation') },
    'EF_mu24_tight_3j45_a4tchad':      {10000: TriggerRule(PS=3.0, comment='Support for Top background evaluation') },
    'EF_mu24_tight_4j45_a4tchad':      {10000: TriggerRule(PS=1.0, comment='Backup for Top background evaluation') },

    'EF_mu8_4j45_a4tchad_L2FS':        {10000: TriggerRule(rate=0.5, comment='Support for SUSY background evaluation') },
    'EF_mu24_j65_a4tchad':             {10000: TriggerRule(rate=0.5, comment='Support for EF_mu24_j65_a4tchad_EFxe60_tclcw') },

    #--- Commissioning trigger for L2MuonSA
    'EF_mu4T_l2muonSA':                {10000: TriggerRule(PS=-1, comment='Commissioning of L2MuonSA for low pT')},

    #--- Commissioning muFast triggers:
    'EF_mu4T_muFast':                  {10000: TriggerRule(PS=0, rerun=True, comment='muFast commissioning')},
    'EF_mu24i_tight_muFast':           {10000: TriggerRule(PS=1, comment='muFast commissioning')},
    'EF_2mu13_muFast':                 {10000: TriggerRule(PS=1, comment='muFast commissioning')},
    'EF_2mu4T_DiMu_muFast':            {10000: TriggerRule(rate=1.0, comment='muFast commissioning')},


    #--- Efficiency measurement
    'EF_mu4T':                        {10000: TriggerRule(rate=1.0, rerun=True, comment='Efficiency for mu4T')},
    'EF_mu6':                         {10000: TriggerRule(rate=0.5, rerun=True, comment='Efficiency for mu6')},
    'EF_mu6i':                        {10000: TriggerRule(PS=0,rerun=True, comment='Efficiency for 2mu6i_DiMu_DY_2j25_a4tchad')},
    'EF_mu6_MSonly':                  {10000: TriggerRule(PS=0,rerun=True, comment='Efficiency for mu6_MSonly')},
    'EF_mu8':                         {10000: TriggerRule(PS=0,rerun=True, comment='Efficiency for mu8')},
    'EF_mu10':                        {10000: TriggerRule(PS=0,rerun=True, comment='Efficiency for mu10')},
    'EF_mu10_MSonly':                 {10000: TriggerRule(PS=0,rerun=True, comment='Efficiency for mu10_MSonly')},
    'EF_mu13':                        {10000: TriggerRule(PS=0,rerun=True, comment='Efficiency for mu13')},
    'EF_mu15':                        {10000: TriggerRule(rate=0.4,rerun=True, comment='Efficiency for mu15')},
    'EF_mu15i':                       {10000: TriggerRule(PS=0,rerun=True, comment='Efficiency for mu15i')},
    'EF_mu18':                        {10000: TriggerRule(PS=0,rerun=True, comment='Efficiency for mu18')},
    'EF_mu18_tight':                  {10000: TriggerRule(PS=0,rerun=True, comment='Efficiency for mu18')},
    'EF_mu24':                        {10000: TriggerRule(PS=0,rerun=True, comment='Efficiency for mu24')},
    #added for P1HLT,17.1.5.12
    'EF_mu4T_L2StarB':                {10000: TriggerRule(rate=1.0, rerun=True, comment='Efficiency for mu4T_L2StarB')},
    'EF_mu6_L2StarB':                 {10000: TriggerRule(rate=0.5, rerun=True, comment='Efficiency for mu6_L2StarB')},

    #--- L1 streamers
    'EF_L1MU10_NoAlg':                {10000: TriggerRule(rate=0.2, comment='L1 Streamer for MU10; low PT coincidence') },
    'EF_L1MU15_NoAlg':                {10000: TriggerRule(rate=0.3, comment='L1 Streamer for MU15; high PT coincidence') },#put to 0.2 later.
    'EF_L12MU4_NoAlg':                {10000: TriggerRule(rate=0.2, comment='L1 Streamer for 2MU4; di-muon') },

    #--- TrigL2MuonSA validation
    'EF_mu24i_tight_l2muonSA':        {10000: TriggerRule(PS=-1,comment='TrigL2MuonSA validation for single isolated trigger') },
    'EF_2mu13_l2muonSA':              {10000: TriggerRule(PS=-1,comment='TrigL2MuonSA validation for di-muon trigger') },

    #--- commissioning at beginning
    'EF_mu18_medium':                  {1000: TriggerRule(PS=-1,comment='Commissioning until 2e33')},
    ##  'EF_mu24_medium':              {3000: TriggerRule(PS=1,comment='Commissioning until 5e33')},
    'EF_mu24_medium':                  {3000: TriggerRule(PS=-1,comment='Commissioning until 5e33')},
    'EF_mu24_tight_MuonEF':            {3000: TriggerRule(PS=-1,comment='Commissioning until 5e33')},
    'EF_mu24_tight_MG':                {3000: TriggerRule(PS=-1,comment='Commissioning until 5e33')},
    'EF_mu24_tight_L2StarB':           {3000: TriggerRule(PS=-1,comment='Commissioning until 5e33')},
    'EF_mu24_tight_L2StarC':           {3000: TriggerRule(PS=-1,comment='Commissioning until 5e33')},
    'EF_mu24i_tight_MuonEF':           {3000: TriggerRule(PS=-1,comment='Commissioning until 5e33')},
    'EF_mu24i_tight_MG':               {3000: TriggerRule(PS=-1,comment='Commissioning until 5e33')},

    #unused filled chains:
    'EF_mu18i4_tight':                 {3000: TriggerRule(PS=-1,comment='Unused')},
    'EF_2mu10':                        {3000: TriggerRule(PS=-1,comment='Unused')},
    'EF_mu24_j65_a4tchad_EFxe40':      {3000: TriggerRule(PS=-1,comment='To be removed') },

    #--- background studies - hardcoded prescales instead of rate=0.1 due to missing prediction
    'EF_mu4_empty_NoAlg':              {#10000: TriggerRule(rate=0.1, comment='For background study')},
                                        10000: TriggerRule(PS=20000, comment='For background study')},
    'EF_mu4_firstempty_NoAlg':         {#10000: TriggerRule(rate=0.1, comment='For background study')},
                                        10000: TriggerRule(PS=300000, comment='For background study')},# put to 3000000 later...
    'EF_mu4_unpaired_iso_NoAlg':       {10000: TriggerRule(PS=1, comment='For background study - prescaled at L1 as well')},
    'EF_mu11_empty_NoAlg':             {10000: TriggerRule(PS=40, comment='For background study')},

    #unused empty chains:
    'EF_mu4_L1MU11_MSonly_cosmic':     {3000: TriggerRule(PS=-1,comment='Unused')},
    'EF_mu4_L1MU11_cosmic':            {3000: TriggerRule(PS=-1,comment='Unused')},
    'EF_mu4T_cosmic':                  {3000: TriggerRule(PS=-1,comment='Unused')},

    #--- special purposes
    'EF_mu10_muCombTag_NoEF_L1MU40':   {3000: TriggerRule(PS=-1,comment='For toriod off alignment runs only')},
    'EF_mu15_muCombTag_NoEF_L1MU40':   {3000: TriggerRule(PS=-1,comment='For toriod off alignment runs only')},
    'EF_mu20_muCombTag_NoEF_L1MU40':   {3000: TriggerRule(PS=-1,comment='For toriod off alignment runs only')},
    'L2_mu4_l2cal_empty':              {3000: TriggerRule(PS=-1,comment='For cosmic runs only')},



    ##### 5*10^33
    #  'L1_2MU4':                      {1000: TriggerRule(PS=1.0, comment='end-of-fill Baseline primary BPhys'),
    #                                   4001: TriggerRule(PS=10, comment='Rate Control;end-of-fill Baseline primary BPhys'),
    #                                  },

    'L1_2MU4':                         {1000: TriggerRule(PS=1.0, comment='end-of-fill Baseline primary BPhys'),
                                        6000: TriggerRule(PS=10, comment='Rate Control;end-of-fill Baseline primary BPhys'),
                                        },

    ########################################
    # Extra Triggers for Muons:
    #
    # -- Special triggers
    # disable EF_mu20it_tight : approved https://indico.cern.ch/conferenceDisplay.py?confId=202477
    # 23.08.2012
    'EF_mu20it_tight':                  {10000: TriggerRule(PS=-1,comment='Tau efficiency measurement; turned off after ICHEP') },

    #--- Single muon with lower threshold
    'EF_mu40_MSonly_barrel_tight':       {5000: TriggerRule(PS=1,comment='Baseline trigger') },

    # -- for H->ZZ*->llqq
    'EF_2mu6i_DiMu_DY_noVtx_noOS_2j25_a4tchad':     {5000: TriggerRule(PS=5.0,comment='Extra support trigger for low mass Higgs') },

    # -- Muon + jet + mEt with lower mEt
    'EF_mu24_j65_a4tchad_EFxe40_tclcw':     {5000: TriggerRule(PS=1,comment='Extra trigger for muon + jet + mEt (lower mEt)') },
    'EF_mu4T_j65_a4tchad_xe60_tclcw_loose': {5000: TriggerRule(PS=1,comment='Extra trigger for muon + jet + mEt (lower mEt)') },

    'EF_mu24_j65_a4tchad_EFxe40wMu_tclcw': {5001: TriggerRule(PS=1,comment='mu+MET triggers  for SUSY in validation 94597)') },
    'EF_mu24_j65_a4tchad_EFxe60wMu_tclcw': {5001: TriggerRule(PS=1,comment='mu+MET triggers  for SUSY in validation 94597)') },
    'EF_2mu8_EFxe40wMu_tclcw':             {5001: TriggerRule(PS=1,comment='mu+MET triggers  for SUSY in validation 94597)') },

    # -- Muon + topo-cluster mEt
  #missing:
    'EF_2mu8_EFxe30_tclcw':              {5000: TriggerRule(PS=1,comment='Extra trigger if CPU allows') },

    # -- Muon + bjet - Disabled - May 18th
    'EF_mu24_tight_b35_mediumEF_j35_a4tchad': {5000: TriggerRule(PS=-1,comment='Extra trigger for muon+ b-jet - needs client') },

    # -- Muon + photons with loose - Both disabled - May 18th
    'EF_mu18_2g10_loose':                {5000: TriggerRule(PS=-1,comment='Extra trigger for muon + 2 photons (medium to loose)') },
    'EF_mu24_g20vh_loose':               {5000: TriggerRule(PS=-1,comment='Extra trigger for muon + photon (medium to loose)') },

    #--- Slow triggers with lower cut
    'L2_mu40_slow_tight': {1000: TriggerRule(PS=1.0, comment='Slow particle search'),
                           5701: TriggerRule(PS=-1, comment='disable at peak'),
                           }, 

    'EF_mu40_slow_tight':               {5000: TriggerRule(PS=1,comment='Extra trigger for slow (lower pT)') },



    'EF_mu40_slow_outOfTime_tight':     {5000: TriggerRule(PS=1,comment='Extra trigger for slow OutOfTime (lower pT)') },


    ################################################################################

    ################################################################################
    # Primaries for  B-Physics:
    #
    'EF_2mu6_Jpsimumu':            {10000: TriggerRule(PS=1,ESRate=0.1,comment='Baseline trigger') },
    'EF_2mu6_Upsimumu':            {10000: TriggerRule(PS=1,ESRate=0.1,comment='Baseline trigger') },
    'EF_2mu6_Bmumu':               {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_2mu6_Bmumux':              {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    # New primaries after ICHEP:
    'EF_2mu6_Bmumux_L2StarA':      {10000: TriggerRule(PS=-1,comment='Baseline trigger') },
    'EF_2mu6_Bmumux_v2':           {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_2mu4T_Bmumux_v2':          {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_mu4Tmu6_Bmumux_v2':        {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_mu6T_2b55_medium_2j55_a4tchad_L1J20_matched': {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    #new trigger, 17.1.5.21, after MD3
    'EF_2mu6_Bmumux_BcMuMuDs_loose_L2StarB': {10000: TriggerRule(PS=1,comment='New trigger') },

    #Barrel triggers for B->mumu
    'EF_2mu4T_Bmumu_BarrelOnly':   { 9000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_mu4Tmu6_Bmumu_Barrel':     { 8000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_2mu4T_Bmumu_Barrel':       { 6000: TriggerRule(PS=1,comment='Baseline/end-of-fill trigger') },

    #3mu4T chains
    'EF_3mu4T_Tau':                {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    #mu18 Jpsi Tag&Probe
    'EF_mu18_tight_Jpsi_EFFS':                 {10000: TriggerRule(PS=1,comment='Bphysics tag&probe trigger') },
    'EF_mu18_tight_L2_2mu4T_Jpsimumu':         {10000: TriggerRule(PS=1,comment='Bphysics tag&probe trigger') },
    'EF_mu18_tight_L2_2mu4T_Jpsimumu_L2StarB': {10000: TriggerRule(PS=1,comment='Bphysics tag&probe L2StarB trigger') },

    'EF_2mu4T_Bmumux_v2_L2StarB':              {10000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
    'EF_2mu4T_Bmumux_Barrel_v2_L2StarB':       { 6000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
    'EF_2mu4T_Bmumux_BarrelOnly_v2_L2StarB':   { 9000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
    'EF_mu4Tmu6_Bmumux_v2_L2StarB':            {10000: TriggerRule(PS=1,comment='Baseline delayed trigger') },

    'EF_2mu4T_Jpsimumu_L2StarB':               { 1000: TriggerRule(PS=1,comment='Delayed stream')},
    'EF_2mu4T_Jpsimumu_Barrel_L2StarB':        { 6000: TriggerRule(PS=1,comment='Baseline/end-of-fill delayed  trigger') },
    'EF_2mu4T_Jpsimumu_BarrelOnly_L2StarB':    { 9000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
    'EF_mu4Tmu6_Jpsimumu_L2StarB':             { 3000: TriggerRule(PS=1,comment='For startup')},
    'EF_mu4Tmu6_Jpsimumu_Barrel_L2StarB':      { 8000: TriggerRule(PS=1,comment='Baseline delayed trigger') },

    'EF_mu4Tmu6_Bmumux_Barrel_v2_L2StarB':     {10000: TriggerRule(PS=1,comment='Baseline delayed trigger') },


    ########################################
    # Delayed Stream Triggers for B-Physics:
    #

    #di-muon
    'EF_2mu4T_Jpsimumu_BarrelOnly':   { 9000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
    'EF_mu4Tmu6_Jpsimumu_Barrel':     { 8000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
    'EF_2mu4T_Jpsimumu_Barrel':       { 6000: TriggerRule(PS=1,comment='Baseline/end-of-fill delayed  trigger') },

    'EF_2mu4T_Upsimumu_BarrelOnly':         { 9000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
    'EF_mu4Tmu6_Upsimumu_Barrel':           { 8000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
    'EF_2mu4T_Upsimumu_Barrel':             { 6000: TriggerRule(PS=1,comment='Baseline/end-of-fill delayed trigger') },
    'EF_2mu4T_Upsimumu_Barrel_L2StarB':     { 6000: TriggerRule(PS=1,comment='Baseline alternative delayed trigger') },
    'EF_2mu4T_Upsimumu_BarrelOnly_L2StarB': { 9000: TriggerRule(PS=1,comment='Baseline alternative delayed trigger') },
    'EF_mu4Tmu6_Upsimumu_L2StarB':          { 3000: TriggerRule(PS=1,comment='Baseline alternative delayed trigger') },
    'EF_mu4Tmu6_Upsimumu_Barrel_L2StarB':   { 8000: TriggerRule(PS=1,comment='Baseline alternative delayed trigger') },

    #goes into both muon and delayed stream, added for P1HLT,17.1.5.12
    'EF_2mu6_Upsimumu_L2StarB': {10000: TriggerRule(PS=1,comment='Baseline alternative trigger') },

    'EF_2mu4T_Bmumux_BarrelOnly':   { 9000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
    'EF_mu4Tmu6_Bmumux_Barrel':     { 8000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
    'EF_2mu4T_Bmumux_Barrel':       { 6000: TriggerRule(PS=1,comment='Baseline/end-of-fill delayed trigger') },

    'EF_2mu4T_Bmumux_L2StarB': {10000: TriggerRule(PS=-1,comment='Replaced by v2') },

    #3mu4T chains
    'EF_3mu4T':                {10000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
    'EF_3mu4T_DiMu':           {10000: TriggerRule(PS=1,comment='Baseline delayed trigger') },#BP: redundant?
    'EF_3mu4T_Jpsi':           {10000: TriggerRule(PS=1,comment='Baseline delayed trigger') },#BP: redundant?
    'EF_3mu4T_Upsi':           {10000: TriggerRule(PS=1,comment='Baseline delayed trigger') },#BP: redundant?

    #2mu+(2)e:
    'EF_2mu4T_DiMu_e5_tight1':       { 4000: TriggerRule(PS=1,comment='L1 limited delayed trigger'),
                                       4001: TriggerRule(PS=-1,comment='L1 limited delayed trigger'), },

    'EF_2mu4T_2e5_tight1':           {10000: TriggerRule(PS=1,comment='Baseline delayed trigger') },

    # Drell-Yan triggers
    'EF_2mu6_DiMu_DY20':             {10000: TriggerRule(rate=1.5, comment='SM Drell-Yan')},
    'EF_2mu6i_DiMu_DY':              {10000: TriggerRule(rate=1.5, comment='SM Drell-Yan')},
    'EF_2mu6_DiMu_DY25':             {10000: TriggerRule(PS=-1, comment='SM Drell-Yan - disabled for now')},
    'EF_2mu6i_DiMu_DY_noVtx_noOS':   {10000: TriggerRule(rate=0.5, comment='SM Drell-Yan - support trigger')},


    'EF_2mu4T_DiMu_noVtx_noOS_L2StarB':    {1000: TriggerRule(rate=1,comment='For startup')},
    'EF_2mu6_DiMu_noVtx_noOS_L2StarB':      {10000: TriggerRule(rate=1.0, comment='supporting BPhys')},
    'EF_2mu6_DiMu_L2StarB':                          {10000:TriggerRule(rate=1.0, comment='supporting BPhys')},
    'EF_mu6_Trk_Jpsi_loose_L2StarB':              {10000: TriggerRule(rate=4.0, comment='supporting BPhys')},
    'EF_mu6_Jpsimumu_tight_L2StarB':            {1000: TriggerRule(PS=1.0,comment='end-of-fill trigger - controlled by L1, MCG 28/4'),
                                                 5000: TriggerRule(PS=-1.0)},#maxRate=5.,


    ########################################
    # End-of-Fill Triggers for B-Physics:
    #
    #

    #Rate control:
  #  'L1_MU4':            { 1501: TriggerRule(rate=200, comment='end-of-fill primary BPhys'),
  #                         1250: TriggerRule(rate=2500, comment='end-of-fill primary BPhys'),
  #                         1000: TriggerRule(rate=5000, comment='end-of-fill primary BPhys'),
  #                         100:  TriggerRule(rate=7500, comment='end-of-fill primary BPhys'),
  #                         },

    #taken care of at top of file
  #  'L1_2MU4_2EM3':      { 1000: TriggerRule(PS=1.0, comment='end-of-fill BPhys'),
  #                         7201: TriggerRule(PS=-1.0, comment='disable at peak'),
  #                         },
  #  'L1_2MU4_BARREL':    { 1000: TriggerRule(PS=1.0, comment='end-of-fill BPhys'),
  #                         7201: TriggerRule(PS=-1.0, comment='disable at peak'),
  #                         },
  #  'L1_2MU4_MU6':       { 1000: TriggerRule(PS=1.0, comment='end-of-fill BPhys'),
  #                         7201: TriggerRule(PS=-1.0, comment='disable at peak'),
  #                         },

    'L1_MU6':            {   100: TriggerRule(PS=1.0, comment='end-of-fill Baseline primary BPhys'),
                             800: TriggerRule(rate=7500, comment='end-of-fill Baseline primary BPhys'),
                             1250: TriggerRule(rate=5000, comment='Rate Control;end-of-fill Baseline primary BPhys'),
                             1501: TriggerRule(rate=2000, comment='Rate Control;end-of-fill Baseline primary BPhys'),
                             4001: TriggerRule(rate=1000, comment='Rate Control;end-of-fill Baseline primary BPhys'),
                           },
    'EF_mu6_Trk_Jpsi_loose':    {10000: TriggerRule(rate=4.0, comment='supporting BPhys')},
    'EF_mu6_Jpsimumu_tight':   {  1000: TriggerRule(PS=1.0,comment='end-of-fill trigger - controlled by L1, MCG 28/4'),
                                  5000: TriggerRule(PS=-1.0)},#maxRate=5.,
    'EF_mu10_Upsimumu_tight_FS':{  500: TriggerRule(PS=1.0, comment='primary at low lumi)'),
                                  2000: TriggerRule(PS=-1.0,comment='End-of-fill trigger')},
    'EF_mu10_Jpsimumu':         { 1000: TriggerRule(PS=1.0, comment='primary BPhys'),
                                  2000: TriggerRule(PS=-1.0,comment='End-of-fill trigger')},

    ########################################
    # Support Triggers for B-Physics:
    'EF_2mu4T_Jpsimumu_IDTrkNoCut':     {10000: TriggerRule(rate=1.5,ESRate=0.1, comment='supporting BPhys')},
    'EF_mu4Tmu6_Jpsimumu_IDTrkNoCut':   {10000: TriggerRule(rate=1.5, comment='supporting BPhys')},

  ###BP: broken April 1
    'EF_2mu13_Zmumu_IDTrkNoCut':        {10000: TriggerRule(rate=0.1,ESRate=0.1, comment='monitoring')},
    #'EF_2mu13_Zmumu_IDTrkNoCut':        {10000: TriggerRule(PS=-1, comment='monitoring')},

    'EF_2mu6_DiMu':                     {10000: TriggerRule(rate=1.0, comment='supporting BPhys')},
    'EF_2mu6_DiMu_noVtx_noOS':          {10000: TriggerRule(rate=1.0, comment='supporting BPhys')},
    #'EF_2mu4T_DiMu_BarrelOnly':         { 9000: TriggerRule(rate=1,comment='Support trigger') },
    #'EF_mu4Tmu6_DiMu_Barrel':           { 8000: TriggerRule(rate=1,comment='Support trigger') },
    #'EF_2mu4T_DiMu_Barrel':             { 6000: TriggerRule(rate=1,comment='Support trigger') },

    #unused chains:
    'EF_2mu6':                          {10000: TriggerRule(PS=-1,comment='monitoring')},
    'EF_2mu4T':                         {10000: TriggerRule(PS=-1,comment='monitoring')},
    'EF_4mu4T':                         {10000: TriggerRule(PS=-1,comment='monitoring')},

    'EF_mu6_Jpsimumu_L2StarA':          {1000: TriggerRule(PS=1.0,comment='Support end-of-fill trigger - controlled by L1, MCG 28/4'),
                                         5000: TriggerRule(PS=-1.0)},#maxRate=5.,

    'EF_mu6_Trk_Jpsi_loose_L2StarA':    {10000: TriggerRule(rate=4.0, comment='supporting BPhys')},

    'EF_mu10_Jpsimumu_L2StarA':         {1000: TriggerRule(PS=1.0, comment='Support BPhys'),
                                         2000: TriggerRule(PS=-1.0,comment='End-of-fill trigger')},

    'EF_mu10_Upsimumu_tight_FS_L2StarA':{500: TriggerRule(PS=1.0, comment='Support BPhys at low lumi)'),
                                         2000: TriggerRule(PS=-1.0,comment='End-of-fill trigger')},



    ########################################
    # Extra Triggers for B-Physics:
    #

    #2mu4 chains
    'EF_2mu4T_Jpsimumu':         {1000: TriggerRule(PS=1,comment='For startup')},
    'EF_2mu4T_Upsimumu':         {1000: TriggerRule(PS=1,comment='For startup')},
    'EF_2mu4T_Bmumu':            {1000: TriggerRule(PS=1,comment='For startup')},
    'EF_2mu4T_Bmumux':           {1000: TriggerRule(PS=1,comment='For startup')},
    'EF_2mu4T_DiMu':             {1000: TriggerRule(rate=1,comment='For startup')},
    'EF_2mu4T_DiMu_noVtx_noOS':  {1000: TriggerRule(rate=1,comment='For startup')},
    'EF_2mu4T_DiMu_L2StarB':     {1000: TriggerRule(rate=1,comment='For startup; tracking studies - needs coherent prescale')},
    'EF_2mu4T_DiMu_L2StarC':     {1000: TriggerRule(rate=1,comment='For startup; tracking studies - needs coherent prescale')},
    'EF_2mu4T_DiMu_l2muonSA':    {1000: TriggerRule(PS=-1, comment='New algo test')},
    'EF_2mu4T_Upsimumu_L2StarB': {1000: TriggerRule(PS=1,comment='For startup')},

    #mu4Tmu6 chains
    'EF_mu4Tmu6_Jpsimumu':         {3000: TriggerRule(PS=1,comment='For startup')},
    'EF_mu4Tmu6_Upsimumu':         {3000: TriggerRule(PS=1,comment='For startup')},
    'EF_mu4Tmu6_Bmumu':            {3000: TriggerRule(PS=1,comment='For startup')},
    'EF_mu4Tmu6_Bmumux':           {3000: TriggerRule(PS=1,comment='For startup')},
    'EF_mu4Tmu6_DiMu':             {3000: TriggerRule(rate=1,comment='For startup')},
    #'EF_mu4Tmu6_DiMu_noVtx_noOS':  {3000: TriggerRule(rate=1,comment='For startup')},

    #prescaled chains
    'EF_2mu4T_Jpsimumu_BarrelOnly_prescaled':  {10000: TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },
    'EF_2mu4T_Jpsimumu_Barrel_prescaled':      {10000: TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },
    'EF_mu4Tmu6_Jpsimumu_Barrel_prescaled':    {10000: TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },
    'EF_mu4Tmu6_Jpsimumu_prescaled':           {10000: TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },
    'EF_2mu4T_Jpsimumu_prescaled':             {10000: TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },

    'EF_2mu6_Bmumux_v2_L2StarB':         {10000:                                         TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_2mu6_Jpsimumu_L2StarB':            {10000:                                         TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_2mu6_Bmumu_L2StarB':               {10000:                                         TriggerRule(PS=1,comment='Baseline trigger') },

    'EF_2mu4T_Jpsimumu_L2StarB_prescaled':                   {10000:                                                           TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },
    'EF_2mu4T_Jpsimumu_Barrel_L2StarB_prescaled':         {10000:                                                        TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },
    'EF_2mu4T_Jpsimumu_BarrelOnly_L2StarB_prescaled':  {10000:                                                     TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },
    'EF_mu4Tmu6_Jpsimumu_L2StarB_prescaled':               {10000:                                                         TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },
    'EF_mu4Tmu6_Jpsimumu_Barrel_L2StarB_prescaled':    {10000:                                                     TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },

    'EF_2mu4T_Bmumu_L2StarB':                         {1000:                                                    TriggerRule(PS=1,comment='For startup')},
    'EF_2mu4T_Bmumu_Barrel_L2StarB':            { 6000:                                               TriggerRule(PS=1,comment='Baseline/end-of-fill trigger') },
    'EF_2mu4T_Bmumu_BarrelOnly_L2StarB':     { 9000:                                            TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_mu4Tmu6_Bmumu_L2StarB':                  {3000:                                               TriggerRule(PS=1,comment='For startup')},
    'EF_mu4Tmu6_Bmumu_Barrel_L2StarB':        { 8000:                                             TriggerRule(PS=1,comment='Baseline trigger') },

    ################################################################################




    ################################################################################
    # Primaries for  E/gamma:
    #
    #--- Primary Electrons (PS=1)
    'EF_e24vhi_medium1':           {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'L2_e60_medium1':              {10000: TriggerRule(PTRate=1,comment='1 Hz of EM30'),},
    'EF_e60_medium1':              {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_2e12Tvh_loose1':           {10000: TriggerRule(PS=1,ESRate=0.4,comment='Baseline trigger') },
    'EF_e24vh_medium1_e7_medium1': {10000: TriggerRule(PS=1.0,comment='Primary'),},

    #New low pt electron L2StarB chains: 17.1.5.21
    'EF_e24vh_medium1_e7_medium1_L2StarB': {10000: TriggerRule(PS=1.0,comment='Alternative Primary'),}, 

    #added for P1HLT,17.1.5.12
    'EF_2e12Tvh_loose1_L2StarB':    {10000: TriggerRule(PS=1,comment='Alternative baseline trigger') },


     #---Primary Combined Electrons/Photons with Muon or MET or jets
    'EF_e24vh_medium1_EFxe30':     {10000: TriggerRule(PS=1.0,comment='Primary'),},
    'EF_g40_loose_L2EFxe60':       {10000: TriggerRule(PS=1.0,comment='Primary'),},
    'EF_e24vhi_loose1_mu8':        {10000: TriggerRule(PS=1.0,comment='Primary'),},
    'EF_e18_loose1_g25_medium':    {10000: TriggerRule(PS=1.0,comment='Primary'),},
    'EF_2mu10_MSonly_g10_loose':   {10000: TriggerRule(PS=1.0,comment='Primary'),},
    'EF_2mu10_MSonly_g10_loose_EMPTY':   {10000: TriggerRule(PS=1.0,comment='Primary'),},
    'EF_2mu10_MSonly_g10_loose_UNPAIRED_ISO':   {10000: TriggerRule(PS=1.0,comment='Primary'),},
    #defined under muons
    #'EF_mu18_tight_e7_medium1':    {10000: TriggerRule(PS=1.0,comment='Primary'),},
    #'EF_mu24_g20vh_medium':        {10000: TriggerRule(PS=1.0,comment='Primary'),},
    #'EF_mu18_2g10_medium':         {10000: TriggerRule(PS=1.0,comment='Primary'),},
    'EF_2e7T_medium1_mu6':         {10000: TriggerRule(PS=1.0,comment='Primary'),},
    'EF_e7T_medium1_2mu6':         {10000: TriggerRule(PS=1.0,comment='Primary'),},

    'EF_e12Tvh_medium1_mu8':       {10000: TriggerRule(PS=1.0,comment='Primary'),},
    'EF_e12Tvh_medium1_mu6_topo_medium': {10000: TriggerRule(PS=1.0,comment='Primary'),},

    #New low pt electron L2StarB chains: 17.1.5.21
    'EF_2e7T_medium1_L2StarB_mu6': {10000: TriggerRule(PS=1.0,comment='Alternative Primary'),}, 
    'EF_e7T_medium1_L2StarB_2mu6': {10000: TriggerRule(PS=1.0,comment='Alternative Primary'),},

    # 17.1.5.22: Switching on e12_L2StarB chains as they have been validated: 
    'EF_e12Tvh_medium1_L2StarB_mu6_topo_medium': {10000: TriggerRule(PS=1.0,comment='L2StarB version of topo'),}, 
    'EF_e12Tvh_medium1_L2StarB_mu8': {10000: TriggerRule(PS=1.0,comment='L2StarB version of topo'),}, 
    'EF_e12Tvh_medium1_L2StarB': {10000: TriggerRule(PS=0,rerun=True,comment='Support for alternative combined triggers')}, 


    # New primaries - May 18th
    'EF_e24vh_medium1_EFxe35_tclcw':     {10000: TriggerRule(PS=1.0, comment='Primary'),},
    'EF_e24vh_loose0_4j55_a4tchad_L2FS': {10000: TriggerRule(PS=1.0, comment='Support for Top background evaluation') },
    'EF_g40_loose_L2EFxe60_tclcw':       {10000: TriggerRule(PS=1.0, comment='Primary'),},
    'EF_g35_loose_g25_loose':            {10000: TriggerRule(PS=1.0, ESRate=0.4, comment='Primary'),},

    # Disabled - May 18th
    'EF_e24vh_medium1_EFxe40':     {10000: TriggerRule(PS=-1.0,comment='Primary'),},
    'EF_e24vh_medium1_EFxe30_tcem':     {5000: TriggerRule(PS=-1.0,comment='Extra'),},

    # Post ICHEP
    'EF_e24vhi_loose1':           {10000: TriggerRule(PS=0,rerun=True,comment='Support trigger in rerun'), },


    #tau mass primaries and support?

    #--- Primary Photons (PS=1)
    'EF_g30_medium_g20_medium':    {10000: TriggerRule(PS=1, comment='primary')},
    'EF_2g20vh_medium':            {10000: TriggerRule(PS=1, comment='primary')},
    'EF_2g40_loose':               {10000: TriggerRule(PS=1, comment='primary')},
    'EF_g200_etcut':               {10000: TriggerRule(PS=1,comment='primary')},
    'EF_g120_loose':               {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_3g15vh_loose':             {10000: TriggerRule(PS=1.0,comment='Primary'),},

    ########################################
    # Support Triggers for E/gamma:
    #
    'EF_e24vh_medium1':            {10000: TriggerRule(PS=10,rerun=True,ESRate=0.4,comment='Support for isolated trigger - after commissioning') },

    'EF_e24vh_medium1_TRT':        {10000: TriggerRule(PS=-1,comment='Commissioning??') },


    #--- JPsi T&P Electrons (PSed)
    #AT : L1 rules to be added for higher lumi points

  #   'L1_2EM3':      {1000: TriggerRule(rate=5000,comment='Rate Control;J/psi calibration'),
  #                    1501: TriggerRule(rate=3000,comment='Rate Control;J/psi calibration'),
  #                    2301: TriggerRule(rate=1500,comment='Rate Control;J/psi calibration'),
  #                    3000: TriggerRule(rate=800,comment='Rate Control;J/psi calibration'),
  #                    4000: TriggerRule(rate=300,comment='Rate Control;J/psi calibration'),},
  #   'L1_2EM3_EM6':  {1000: TriggerRule(rate=4000,comment='J/psi calibration'),
  #                    2301: TriggerRule(rate=2000,comment='Rate Control;J/psi calibration'),
  #                    3000: TriggerRule(rate=1000,comment='Rate Control;J/psi calibration'),
  #                    4000: TriggerRule(rate=500,comment='Rate Control;J/psi calibration'),},
  #   'L1_2EM3_EM12': {1000: TriggerRule(PS=1,comment='feeds e5_tight_e14_etcut_Jpsi'),
  #                    1501: TriggerRule(rate=6000,comment='feeds e5_tight1_e14_etcut_Jpsi'),
  #                    2000: TriggerRule(rate=4000,comment='feeds e5_tight1_e14_etcut_Jpsi'),
  #                    2301: TriggerRule(rate=3000,comment='feeds e5_tight1_e14_etcut_Jpsi'),
  #                    3000: TriggerRule(rate=1000,comment='feeds e5_tight1_e14_etcut_Jpsi'),
  #                    4000: TriggerRule(rate=500,comment='Rate Control;J/psi calibration'),},

    'L1_2EM3': {2000: TriggerRule(rate=600,comment='Rate Control;J/psi calibration'),
                3000: TriggerRule(rate=300,comment='Rate Control;J/psi calibration'),
                4000: TriggerRule(rate=200,comment='Rate Control;J/psi calibration'),
                5000: TriggerRule(rate=100,comment='Rate Control;J/psi calibration'),},

    'L1_2EM3_EM6': {2000: TriggerRule(rate=8000,comment='J/psi calibration'),
                    3000: TriggerRule(rate=4000,comment='Rate Control;J/psi calibration'),
                    4000: TriggerRule(rate=2000,comment='Rate Control;J/psi calibration'),
                    5000: TriggerRule(rate=800,comment='Rate Control;J/psi calibration'),},

    'L1_2EM3_EM12': {2000: TriggerRule(rate=10000,comment='feeds e5_tight1_e14_etcut_Jpsi'),
                     2500: TriggerRule(rate=8000,comment='feeds e5_tight1_e14_etcut_Jpsi'),
                     3000: TriggerRule(rate=5000,comment='feeds e5_tight1_e14_etcut_Jpsi'),
                     4000: TriggerRule(rate=2000,comment='feeds e5_tight1_e14_etcut_Jpsi'),
                     5000: TriggerRule(rate=500,comment='Rate Control;J/psi calibration'),},



    'EF_2e5_tight1_Jpsi':         {10000: TriggerRule(rate=1.0,comment='Primary; J/Psi, ')},
    'EF_e5_tight1_e4_etcut_Jpsi': {10000: TriggerRule(rate=3,comment='Primary; Performance from JPsi, rate mostly limited by L1')},
    'EF_e5_tight1_e9_etcut_Jpsi': {10000: TriggerRule(rate=6,comment='Primary; Performance from JPsi, rate mostly limited by L1')},
    'EF_e5_tight1_e14_etcut_Jpsi': {10000: TriggerRule(PS=1,comment='Primary; Performance from JPsi, rate mostly limited by L1'),},
    'EF_e9_tight1_e4_etcut_Jpsi':  {10000: TriggerRule(rate=6,comment='Primary; Performance from JPsi, rate mostly limited by L1')},
    'EF_e14_tight1_e4_etcut_Jpsi': {10000: TriggerRule(PS=1,comment='Primary; Performance from JPsi, rate mostly limited by L1'),},
    'EF_e5_tight1_e5_NoCut':       {10000: TriggerRule(rate=0.5,comment='Performance from JPsi')},


    #--- Upsilon T&P Electrons (PSed)
    #AT : L1 rates to be adjusted according to actual rates at P1
    #PJB 1/6: Ajusted rates savannah 94968
    'L1_2EM6_EM16VH': { 3000: TriggerRule(PS=1,comment='feeds e20vhT_medium1/tight1_g6T_etcut_Upsi '),
                        4500: TriggerRule(rate=4000,comment='feeds e20vhT_medium1/tight1_g6T_etcut_Upsi '),
                        5000: TriggerRule(rate=1500,comment='feeds e20vhT_medium1/tight1_g6T_etcut_Upsi '),
                        6000: TriggerRule(rate=500,comment='feeds e20vhT_medium1/tight1_g6T_etcut_Upsi '),},
    'EF_e20vhT_tight1_g6T_etcut_Upsi': {10000: TriggerRule(rate=4,maxRate=4.,comment='Performance from Upsilon')},
    'EF_e20vhT_medium1_g6T_etcut_Upsi': {10000: TriggerRule(PS=-1,comment='Performance from Upsilon')},

    #--- W T&P Electrons (PSed)
    #AT: L1 limited, should we control them by L1 rules? L1 rate is ~const vs lumi
    #BP: to be adjusted
  #  'L1_EM16V_XE20': {3000: TriggerRule(rate=1000,maxRate=1500,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*')},

    'L1_EM16V_XE20': {3000: TriggerRule(rate=1000,maxRate=1500,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
                      4000: TriggerRule(rate=500,maxRate=1500,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*')},

    # PO: g20_loose and mu4T_j110 is L1 driven, to give unique rate, increase L1 rate
    # At L2, set PS=1.0.  At EF, set rate=1.0
    # Note that g20_loose is handled lower
    'L1_EM12':    {3000: TriggerRule(rate=150, comment='Seeding EF_g20_loose for prompt and hadcalib')},
    'L1_MU4_J50': {3000: TriggerRule(rate=40, comment='Seeding EF_mu4T_j110 for prompt and hadcalib')},
    # Don't need to do those since they use different L2 chains, EF decisions will be uncorrelated!
    #'L2_mu4T_j105_c4cchad': {3000: TriggerRule(PS=1.0, comment='Seeding EF_mu4T_j110 for prompt and hadcalib')},
    #'L2_mu4T_j50_a4TTem': {3000: TriggerRule(PS=1.0, comment='Seeding EF_mu4T_j110 for prompt and hadcalib')},


  #  'L1_EM6_XS45':   {3000: TriggerRule(rate=1000,maxRate=1500,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*')},

    #'L1_EM6_XS45':   {100: TriggerRule(PS=1,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  3000: TriggerRule(PS=2,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  4000: TriggerRule(PS=3,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  5000: TriggerRule(PS=6,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  6000: TriggerRule(PS=11,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*')},



    #'L1_EM6_XS50':   {100: TriggerRule(PS=1,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  3000: TriggerRule(PS=2,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  4000: TriggerRule(PS=3,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  5000: TriggerRule(PS=6,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  6000: TriggerRule(PS=11,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*')},

    #'L1_EM6_XS55':   {100: TriggerRule(PS=1,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  3000: TriggerRule(PS=2,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  4000: TriggerRule(PS=3,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  5000: TriggerRule(PS=6,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  6000: TriggerRule(PS=11,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*')},

    'L1_EM6_XS55':   {1000: TriggerRule(PS=1,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
                      7401: TriggerRule(PS=-1, comment='disable at peak')},

    #'L1_EM6_XS60':   {100: TriggerRule(PS=1,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  3000: TriggerRule(PS=2,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  4000: TriggerRule(PS=3,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  5000: TriggerRule(PS=6,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
    #                  6000: TriggerRule(PS=11,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*')},

  #  'L1_EM6_XS45':   {3000: TriggerRule(rate=1000,maxRate=1500,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
  #                    4000: TriggerRule(rate=500,maxRate=1500,comment='Wen  performance trigger, seeding EF_e13_etcutTrk*')},


    'L2_e13_etcutTrk_xs55': {1000: TriggerRule(PS=1.0, comment='backup Wenu performance trigger'),
                             7401: TriggerRule(PS=-1, comment='disable at peak'),}, 
    'EF_e13_etcutTrk_xs60': {3000: TriggerRule(PS=-1,maxRate=6.,comment='Wenu performance trigger')},
    'EF_e13_etcutTrk_xs60_dphi2j15xe20': {3000: TriggerRule(PS=-1,maxRate=6.,comment='Wenu performance trigger')},
    'EF_e20_etcutTrk_xs60_dphi2j15xe20': {3000: TriggerRule(PS=-1,maxRate=6.,comment='backup Wenu performance trigger')},
    'EF_e20_etcutTrk_xe30_dphi2j15xe20': {3000: TriggerRule(rate=5,maxRate=6.,comment='backup Wenu performance trigger')},

    # More: Post ICHEP


    'EF_e13_etcutTrk_xs45_L1XS60':              {3000: TriggerRule(PS=-1,comment='backup Wenu performance trigger')},
    'EF_e13_etcutTrk_xs45_L1XS50_dphi2j15xe20': {3000: TriggerRule(PS=-1,comment='backup Wenu performance trigger')},
    'EF_e13_etcutTrk_xs45_L1XS55_dphi2j15xe20': {3000: TriggerRule(PS=1,comment='backup Wenu performance trigger')},
    'EF_e13_etcutTrk_xs45_L1XS60_dphi2j15xe20': {3000: TriggerRule(PS=-1,comment='backup Wenu performance trigger')},
    'EF_e13_etcutTrk_xs50_L1XS50_dphi2j15xe20': {3000: TriggerRule(PS=-1,comment='backup Wenu performance trigger')},
    'EF_e20_etcutTrk_xs45_L1XS45_dphi2j15xe20': {3000: TriggerRule(PS=-1,maxRate=6.,comment='backup Wenu performance trigger')},


    #--- PO: To ensure unique rate
    'L2_g40_loose':              {10000: TriggerRule(rate=16.0,comment='Used in hadcalib')},
    'L2_g60_loose':              {10000: TriggerRule(rate=20.0,comment='Used in hadcalib')},
    'L2_mu4T_j140_c4cchad':      {10000: TriggerRule(rate=10.0,comment='Used in hadcalib')},

    #--- Prescaled Photons
    'L2_g20_loose':              {10000: TriggerRule(PS=1.0, PTRate=1.0,comment='Bootstrap and LAr studies')},
    'EF_g20_loose':              {10000: TriggerRule(rate=1.0, rerun=True, comment='Baseline prescaled primary; Direct photons')},
    'EF_g40_loose':              {10000: TriggerRule(rate=1.0, rerun=True,comment='Baseline prescaled primary; Direct photons')},
    'EF_g60_loose':              {10000: TriggerRule(rate=1.0,comment='Baseline prescaled Primary; Direct photons')},
    'EF_g80_loose':              {10000: TriggerRule(rate=1.0,comment='Baseline prescaled Primary; Direct photons')},
    'EF_g100_loose':             {10000: TriggerRule(rate=1.0,comment='Baseline prescaled Primary; Direct photons')},

    'EF_g10_loose':              {10000: TriggerRule(rate=0,rerun=True,comment='Support for combined trigger')},
    'EF_g10_medium':             {10000: TriggerRule(rate=0,rerun=True,comment='Support for combined trigger')},
    'EF_g12Tvh_loose':           {10000: TriggerRule(rate=0,rerun=True,comment='Support for combined trigger')},
    'EF_g20_medium':             {10000: TriggerRule(rate=0,rerun=True,comment='Support for combined trigger')},#AT: do we also need g20vh_medium?
    'EF_g15_loose':              {10000: TriggerRule(rate=0,rerun=True,comment='Support for combined trigger')}, #AT: do we also need g15vh_loose?
    'EF_g15vh_medium':           {10000: TriggerRule(rate=0,rerun=True,comment='Support for combined trigger')},

    'EF_g30_loose':              {10000: TriggerRule(PS=0, rerun=True, comment='')},
    'EF_g30_medium':             {10000: TriggerRule(PS=0, rerun=True, comment='')},

     #--- Monitoring

    'EF_e24vh_tight1_e15_NoCut_Zee':      {10000: TriggerRule(PS=1, ESRate=0.4,comment='Monitoring')},

    #--- Alternative Tracking

  #Broken chains
    'EF_e5_tight1_e4_etcut_Jpsi_L2StarB': {10000: TriggerRule(rate=1.0,comment='Alternative Tracking; Coh PS with EF_jpsi_e5_tight1_e4_etcut;')},
    'EF_e5_tight1_e4_etcut_Jpsi_L2StarC': {10000: TriggerRule(rate=1.0,comment='Alternative Tracking; Coh PS with EF_jpsi_e5_tight1_e4_etcut;')},
  #  'EF_e24vh_medium1_TRT':               {10000: TriggerRule(rate=0.5,comment='Alternative Tracking')},
    'EF_e24vh_medium1_L2StarB':           {10000: TriggerRule(rate=1, comment='Alternative Tracking')},
    'EF_e24vh_medium1_L2StarC':           {10000: TriggerRule(rate=1, comment='Alternative Tracking')},
    'EF_e24vh_medium1_IDTrkNoCut':        {10000: TriggerRule(rate=0.5,ESRate=0.4,comment='monitoring ')},

  #--- Efficiency and Background extractions
    'EF_e11_etcut':                      {10000: TriggerRule(PS=-1.0,comment='Background estimation for low-pt electrons')},
    'EF_g20_etcut':                      {10000: TriggerRule(PS=-1.0,comment='Background estimations via cut reversal')},
    'EF_e24vh_loose':                    {10000: TriggerRule(PS=-1.0,comment='Cross-check for e24vh_medium, background estimation via loose selection')},
    'EF_e24vh_loose1':                   {10000: TriggerRule(PS=-1.0,comment='Cross-check for e24vh_medium, background estimation via loose selection')},
    'EF_e24vh_loose0':                   {10000: TriggerRule(PS=125.0,rerun=True,comment='Cross-check for e24vh_medium, background estimation via loose selection')},
    'EF_e15vh_medium1':                  {10000: TriggerRule(rate=0.4,rerun=True,comment='Support for combined trigger & ID efficiency')},
    'EF_e7T_loose1':                     {10000: TriggerRule(PS=0,rerun=True,comment='Support for combined triggers')},#PS=0 or rate=0?
    'EF_e7T_medium1':                    {10000: TriggerRule(PS=0,rerun=True,comment='Support for combined triggers')}, #PS=0 or rate=0?
    'EF_e12Tvh_loose1':                  {10000: TriggerRule(PS=0,rerun=True,comment='Support for combined triggers')}, #PS=0 or rate=0?
    #added for P1HLT,17.1.5.12
    'EF_e12Tvh_loose1_L2StarB': {10000: TriggerRule(PS=0,rerun=True,comment='Support for combined triggers')},
    'EF_e7_medium1':                     {10000: TriggerRule(PS=0,rerun=True,comment='Support for combined triggers')}, #PS=0 or rate=0?
    #added for P1HLT,17.1.5.12
    'EF_e7_medium1_L2StarB': {10000: TriggerRule(PS=0, rerun=True ,comment='Support for combined triggers')},

    'EF_e12Tvh_medium1':                 {10000: TriggerRule(rate=0.,rerun=True,comment='Support for combined triggers')},
    'EF_e18_medium1':                    {10000: TriggerRule(rate=0,rerun=True,comment='Support for combined trigger')},
    'EF_e18vh_medium1':                  {10000: TriggerRule(rate=0,rerun=True,comment='Support for combined trigger')},
    'EF_e18_loose1':                     {10000: TriggerRule(rate=0,rerun=True,comment='Support for combined trigger')},

    'EF_e5_medium1':                     {10000: TriggerRule(rate=0.25,comment='Extra for ID efficiency')},
    'EF_g24_etcut':                      {10000: TriggerRule(rate=1.0,comment='Extra for ID efficiency')},
    'EF_e5_loose0':                      {10000: TriggerRule(rate=1.0,comment='Extra for ID efficiency')},
    'EF_e15vh_loose0':                   {10000: TriggerRule(rate=1.0,comment='Extra for ID efficiency')},
    'EF_e60_loose0':                     {10000: TriggerRule(rate=0.5,comment='Extra for ID efficiency')},

    'L2_e5_tight1':                      {10000: TriggerRule(PS=-1., comment='Trigger eff. for J/Psi - replaced by e5_etcut')},
    'EF_e5_tight1':                      {10000: TriggerRule(PS=-1., comment='Trigger eff. for J/Psi - replaced by e5_etcut')},
    'L2_e5_etcut':                       {10000: TriggerRule(PTRate=1.0, comment='1HZ EM3 ouput')},
    'EF_e5_etcut':                       {10000: TriggerRule(rate=1.0, comment='Trigger eff. for J/Psi & ID efficiency')},

    #New low pt electron L2StarB chains: 17.1.5.21
    'EF_e7T_loose1_L2StarB': {10000: TriggerRule(PS=0,rerun=True,comment='Support for future analyses')}, 
    'EF_e7T_medium1_L2StarB': {10000: TriggerRule(PS=0,rerun=True,comment='Support for alternative combined triggers')}, 


    ########################################
    # Extra Triggers for E/gamma:
    #
    'EF_e22vh_medium1':            {4000: TriggerRule(PS=-1,comment='Primary up to 4e33, to be prescaled as isolation is commissioned ')},
    'EF_e22vhi_medium1':           {1000: TriggerRule(PS=-1,comment='Primary up to 4e33')},
    'EF_e22vh_loose':              {4000: TriggerRule(rate=1.0,maxRate=1.0,depends_on='EF_e22vhi_medium1',comment='Cross-check for e22vh_medium, background estimation via loose selection')},
    'EF_e22vh_loose1':             {4000: TriggerRule(rate=1.0,maxRate=1.0,depends_on='EF_e22vhi_medium1',comment='Cross-check for e22vh_medium, background estimation via loose selection')},
    'EF_e22vh_loose0':             {4000: TriggerRule(rate=1.0,maxRate=1.0,depends_on='EF_e22vhi_medium1',comment='Cross-check for e22vh_medium, background estimation via loose selection')},
    'EF_e22vh_medium1_IDTrkNoCut': {4000: TriggerRule(rate=0.5,maxRate=1.0,depends_on='EF_e22vhi_medium1',comment='monitoring ')},

    'EF_e24vh_loose0_4j45_a4tchad_L2FS':  {10000: TriggerRule(PS=3.0, comment='Support for Top background evaluation') },
    'EF_e24vh_loose0_3j45_a4tchad_L2FS':  {10000: TriggerRule(PS=15.0, comment='Support for Top background evaluation') },


  #  'EF_e24vh_medium1':            {1000: TriggerRule(PS=1, rerun=True, ESRate=0.4, comment='Primary from 4e33, to be prescaled to PS=10 as soon as isolation is commissioned; ES and SUSY')},
     #BP: the old tracking chains are for startup only
    'EF_e24vh_medium1_SiTrk':      {1000: TriggerRule(PS=-1, comment='Alternative Tracking')},
    'EF_e24vh_medium1_IdScan':     {1000: TriggerRule(PS=-1, comment='Alternative Tracking')},
    #'EF_e24vh_medium1_SiTrk':      {1000: TriggerRule(PS=-1, depends_on='EF_e24vh_medium1',comment='Alternative Tracking')},
    #'EF_e24vh_medium1_IdScan':     {1000: TriggerRule(PS=-1, depends_on='EF_e24vh_medium1',comment='Alternative Tracking')},


    'EF_e18vh_medium1_2e7T_medium1':  {4000: TriggerRule(PS=1.0,comment='Extra Primary - L1 limited, SUSY'),},

    #New low pt electron L2StarB chains: 17.1.5.21
    'EF_e18vh_medium1_2e7T_medium1_L2StarB': {4000: TriggerRule(PS=1.0,comment='Extra Alternative Primary - L1 limited, SUSY'),}, 

    # Disabled - May 18th
    'EF_e24vh_medium1_b35_mediumEF_j35_a4tchad':  {5000: TriggerRule(PS=-1,comment='Extra Primary'),}, #BP: check stream
    'EF_e45_medium1':              {5000: TriggerRule(PS=-1,comment='Backup primary, prescalable at higher lumi; SM, top, Higgs, Susy, exotics'),},
    'EF_2e7T_loose1_mu6':          {5000: TriggerRule(PS=-1,comment='Extra'),},
    'EF_e7T_loose1_2mu6':          {5000: TriggerRule(PS=-1,comment='Extra'),},
    'EF_2g15vh_medium_g10_medium': {5000: TriggerRule(PS=-1,comment='Extra'),}, #BP: needed?

     #topo MET (CPU limited)
    'EF_e24vh_medium1_EFxe35_tcem':     {5000: TriggerRule(PS=1.0,comment='Extra'),},

    'EF_j180_a10tcem_e45_loose1':  {5000: TriggerRule(PS=1.0,comment='Extra Primary for Boosted Top '),}, #should maybe be under jets
    'EF_j240_a10tcem_e45_loose1':  {5000: TriggerRule(PS=1.0,comment='Extra Primary for Boosted Top'),},  #should maybe be under jets
    'EF_j240_a10tcem_e60_loose1':  {5000: TriggerRule(PS=1.0,comment='Extra Primary for Boosted Top'),},  #should maybe be under jets


    'EF_g30_loose_g20_loose':      {1000: TriggerRule(PS=-1, comment='Replaced by g35_loose_g25_loose in ES')},
    #'EF_g30_loose_g20_loose':      {1000: TriggerRule(rate=0.4, ESValue=1, comment='primary until end-of-april TS')},
    #'EF_g30_loose_g20_loose':      {1000: TriggerRule(rate=0.4,ESRate=0.4, comment='primary until end-of-april TS')},
    'EF_g35_loose_g30_loose':      {1000: TriggerRule(PS=1, comment='primary until end-of-april TS')}, #BP: needed?
    'EF_g40_loose_g25_loose':      {1000: TriggerRule(PS=1, comment='primary until end-of-april TS')}, #BP: needed?
    'EF_g40_loose_g30_loose':      {1000: TriggerRule(PS=1, comment='primary until end-of-april TS')}, #BP: needed?
    'EF_e18_medium1_g25_loose':    {1000: TriggerRule(PS=1.0,comment='Primary'),},                     #BP: needed?


    ################################################################################



    ################################################################################
    # Primaries for  Tau:
    #
    #single tau
    'EF_tau125_medium1':                   {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    #ditau
    'EF_2tau38T_medium1':                  {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_tau29Ti_tight1_tau20Ti_tight1':    {10000: TriggerRule(PS=1.,comment='Baseline with tighter tracking') },

    #tau + electron
    'EF_tau20Ti_medium1_e18vh_medium1':    {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_tau38T_medium1_e18vh_medium1':     {10000: TriggerRule(PS=1,comment='Baseline trigger - non-isolated backup') },

    #tau + muon
    'EF_tau20_medium1_mu15':               {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_tau20_medium1_mu18':               {10000: TriggerRule(PS=1.,comment='Backup') },
    'EF_tau20_medium1_mu15i':              {10000: TriggerRule(PS=1.,comment='backup with isolated mu') },
    'EF_tau20T_medium1_mu15i':             {10000: TriggerRule(PS=1.,comment='backup with isolated mu') },

    # New Primaries - May 18th
    'EF_tau29Ti_medium1_tau20Ti_medium1':        {10000: TriggerRule(PS=1.,comment='Baseline trigger') },
    'EF_tau100_loose1_tau70_loose1':             {10000: TriggerRule(PS=1.,comment='Baseline trigger') },
    'EF_tau115_medium1':                         {10000: TriggerRule(PS=1.,comment='Baseline trigger') },
    'EF_tau115_medium1_llh':                     {10000: TriggerRule(PS=1.,comment='Baseline trigger') },
    'EF_2tau38T_medium1_llh':                    {10000: TriggerRule(PS=1.,comment='Baseline trigger with llh') },
    'EF_tau29T_medium1_xe55_tclcw_tight':        {10000: TriggerRule(PS=1.,comment='Baseline trigger') },

    ########################################
    # Supporting single Triggers for Tau:
    #

    'EF_tau125_IDTrkNoCut':                {10000:  TriggerRule(rate=0.4,ESValue=1., comment = 'Express stream')},
    'EF_tau29_IDTrkNoCut':                 {10000:  TriggerRule(rate=0.4,ESValue=1., comment = 'Express stream') },
    'EF_tauNoCut_L1TAU40':                 {10000:  TriggerRule(rate=0.6, ESRate=0.4, comment = 'Express stream') },
    'L2_tauNoCut_L1TAU40':                 {10000:  TriggerRule(PTRate=0.5) },
    'EF_tauNoCut':                         {10000:  TriggerRule(rate=0.6, ESRate=0.4, comment = 'Express stream') }, #ESValue means PS for ES stream, which is multiplied with rate
    'L2_tauNoCut':                         {10000:  TriggerRule(PTRate=0.5) },
    'L2_tau125_medium1':                   {10000:  TriggerRule(PTRate=0.2) },

    'EF_tau20_medium1':                    {10000: TriggerRule(rate=0.4,ESValue=1,rerun='True',comment='Support for baseline lep-tau, ALSO used in express stream') },
    'EF_tau20T_medium':                    {10000: TriggerRule(rate=0.1,rerun='True',comment='Support for baseline lep-tau') },
    'EF_tau20T_medium1':                   {10000: TriggerRule(rate=0.1,rerun='True',comment='Support for baseline ditau') },
    'EF_tau20Ti_medium1':                  {10000: TriggerRule(rate=0.1,rerun='True',comment='Support for baseline ditau') },
    'EF_tau20Ti_tight1':                   {10000: TriggerRule(rate=0.1,rerun='True',comment='Support for baseline ditau') },

    'EF_tau29_medium1':                    {10000: TriggerRule(rate=0.1,rerun='True',comment='Support for baseline di-tau') },
    'EF_tau29T_medium':                    {10000: TriggerRule(rate=0.1,rerun='True',comment='Support for baseline lep-tau') },
    'EF_tau29T_medium1':                   {10000: TriggerRule(rate=0.1,rerun='True',comment='Support for baseline xe-tau') },
    'EF_tau29Ti_medium1':                  {10000: TriggerRule(rate=0.1,rerun='True',comment='Support for baseline xe-tau') },
    'EF_tau29Ti_tight1':                   {10000: TriggerRule(rate=0.1,rerun='True',comment='Support for baseline ditau') },

    'EF_tau38T_medium1':                   {10000: TriggerRule(rate=0.1,rerun='True',comment='Support for baseline di-tau') },

    'EF_tau29_medium_2stTest':             {1000: TriggerRule(rate=0.5,comment='Alternative tracking') },
    'EF_tau29_medium_L2StarA':             {1000: TriggerRule(rate=0.5,comment='Alternative tracking') },
    'EF_tau29_medium_L2StarB':             {1000: TriggerRule(rate=0.5,comment='Alternative tracking') },
    'EF_tau29_medium_L2StarC':             {1000: TriggerRule(rate=0.5,comment='Alternative tracking') },


    'EF_tau20_medium1_llh':                {10000: TriggerRule(PS=0,rerun='True',comment='Support for lep-tau') },
    'EF_tau20Ti_tight1_llh':               {10000: TriggerRule(PS=0,rerun='True',comment='Support for ditau llh') },
    'EF_tau29Ti_tight1_llh':               {10000: TriggerRule(PS=0,rerun='True',comment='Support for ditau llh') },

    'EF_tau29Ti_medium1_llh':              {10000: TriggerRule(PS=0,rerun='True',comment='Support for xe-tau llh') },
    'EF_tau20Ti_medium1_llh':              {10000: TriggerRule(PS=0,rerun='True',comment='Support for ditau llh') },
    'EF_tau38T_medium1_llh':               {10000: TriggerRule(PS=0,rerun='True',comment='Support for di-tau') },

    # Post ICHEP
    'EF_tau100_loose1':                    {10000: TriggerRule(PS=0,rerun='True',comment='Support for baseline') },
    'EF_tau70_loose1':                     {10000: TriggerRule(PS=0,rerun='True',comment='Support for baseline') },

    ########################################
    # Extra Triggers for Tau:
    #

    'EF_2tau29_medium1':                         {1000: TriggerRule(PS=-1,comment='Baseline trigger in Low Lumi') }, #BP: needed?
    'EF_tau29T_medium1_tau20T_medium1':          {4000: TriggerRule(PS=-1,comment='Baseline trigger in Low Lumi') },


    #  'EF_tau20T_medium1_e15vh_medium1':            {4000: TriggerRule(PS=1,comment='Baseline trigger in Low Lum') },
    'EF_tau20T_medium1_e15vh_medium1':            {4000: TriggerRule(PS=-1,comment='Baseline trigger in Low Lum') },

    'EF_tau29Ti_medium1_xe40_tight':             {4000: TriggerRule(PS=1.,comment='Baseline trigger if L1 allows') },
    'EF_tau38T_medium1_xe40_tight':              {4000: TriggerRule(PS=1.,comment='Lower met threshold backup to non-isolated chain') },

    'EF_tau29Ti_medium1_xe55_tclcw':             {4000: TriggerRule(PS=1.,comment='Loose L1 MET') },
    'EF_tau29Ti_medium1_xe55_tclcw_tight':       {5000: TriggerRule(PS=1.,comment='Baseline with hadronic topo MET') },
    'EF_tau38T_medium1_xe55_tclcw_tight':        {5000: TriggerRule(PS=1.,comment='Baseline non-isolation with hadronic topo MET') },

    # New triggers - May 18th - All turned on
    'EF_tau38T_medium1_xe50_tclcw_tight':        {5000: TriggerRule(PS=1.,comment='Extras') },
    'EF_tau29T_medium1_xe50_tclcw_tight':        {5000: TriggerRule(PS=1.,comment='Extras') },
    'EF_tau29Ti_medium1_xe50_tclcw_tight':       {5000: TriggerRule(PS=1.,comment='Extras') },
    'EF_tau38T_medium1_xe55_tclcw':              {5000: TriggerRule(PS=1.,comment='Extras') },
    'EF_tau29T_medium1_xe55_tclcw':              {5000: TriggerRule(PS=1.,comment='Extras') },


    # Disabled triggers (don't used "medium")
    'EF_tau20T_medium_e15vh_medium1':      {5000: TriggerRule(PS=-1.,comment='Unused') },
    'EF_tau20_medium':                     {5000: TriggerRule(PS=-1.,comment='Unused') },
    'EF_tau20_medium_mu15':                {5000: TriggerRule(PS=-1.,comment='Unused') },
    'EF_tau29T_medium1_xe45_tight':        {5000: TriggerRule(PS=-1.,comment='Unused - use tau29Ti_medium1_xe45_tight instead') },
    'EF_2tau29T_medium1':                  {5000: TriggerRule(PS=-1.,comment='Unused - use 2tau29Ti_medium1 instead') },
    # Extra disabled - May 18th
    'EF_tau29T_medium1_xe40_tight':              {1000: TriggerRule(PS=-1.,comment='"loose" tau and met for startup') },
    'EF_tau38T_medium1_xe45_tight':              {10000: TriggerRule(PS=-1,comment='Baseline trigger - non-isolated backup') },
    'EF_tau29Ti_medium1_xe45_tight':             {10000: TriggerRule(PS=-1,comment='Baseline trigger') },
    'EF_tau29Ti_medium1_llh_xe40_tight':         {1000: TriggerRule(PS=-1.,comment='Supports baseline with llh') },
    'EF_tau29Ti_medium1_llh_xe45_tight':         {1000: TriggerRule(PS=-1.,comment='Supports baseline with llh') },
    'EF_tau20Ti_medium1_llh_e18vh_medium1':      {1000: TriggerRule(PS=-1.,comment='Baseline trigger with llh') },
    'EF_tau38T_medium1_llh_e18vh_medium1':       {1000: TriggerRule(PS=-1.,comment='Baseline trigger, non-isolated with llh') },
    'EF_tau20_medium1_llh_mu15':                 {1000: TriggerRule(PS=-1.,comment='Supports baseline + LLH') },


    ########################################
    # llh triggers (for startup phase only):
    #ditau
    'EF_tau29Ti_medium1_llh_tau20Ti_medium1_llh':{1000: TriggerRule(PS=-1.,comment='Support for Baseline trigger with llh') },
    'EF_tau29Ti_tight1_llh_tau20Ti_tight1_llh':  {1000: TriggerRule(PS=-1.,comment='Baseline trigger with llh') },

    #single tau
    'EF_tau125_medium1_llh':                     {1000: TriggerRule(PS=1.,comment='Baseline single tau with llh') },
    'EF_tau125_medium1_L2StarA':                 {1000: TriggerRule(PS=1.,comment='Baseline single tau with L2StarA') },
    'EF_tau125_medium1_L2StarB':                 {1000: TriggerRule(PS=1.,comment='Baseline single tau with L2StarB') },
    'EF_tau125_medium1_L2StarC':                 {1000: TriggerRule(PS=1.,comment='Baseline single tau with L2StarC') },


    ########################################
    # new tau chains with lower thresholds (L2loose)
    # savannah: https://savannah.cern.ch/bugs/?9656...
##    'EF_tau27Ti_medium1_L2loose_tau18Ti_medium1_L2loose' : {10000: TriggerRule(PS=1.,comment='Baseline with loosened L2/EF threshold') },
##    'EF_tau18Ti_medium1_L2loose_e18vh_medium1' : {10000: TriggerRule(PS=1.,comment='Baseline with loosened L2/EF threshold') },
    'EF_tau27T_medium1_L2loose_xe50_tclcw_tight' : {10000: TriggerRule(PS=1.,comment='Baseline with loosened L2/EF threshold') },
##    'EF_tau20_medium1_L2loose_mu15' : {10000: TriggerRule(PS=1.,comment='Baseline with loosened L2/EF threshold') },

    ###https://savannah.cern.ch/bugs/?98353 #5
    'EF_tau27Ti_medium1_L2loose_tau18Ti_medium1_L2loose' : {1000: TriggerRule(PS=1.,comment='Including back the loosened L2/EF threshold'),
                                                            6501: TriggerRule(PS=-1, comment='limiting the rate')}, 
    'EF_tau18Ti_medium1_L2loose_e18vh_medium1' : {1000: TriggerRule(PS=1.,comment='Including back the loosened L2/EF threshold'),
                                                  8501: TriggerRule(PS=-1, comment='Limit L2 rate')},
    'EF_tau20_medium1_L2loose_mu15' : {1000: TriggerRule(PS=1.,comment='Including back the loosened L2/EF threshold'),
                                       8501: TriggerRule(PS=-1, comment='Limit L2 rate')}, 
    


    ################################################################################



    ################################################################################
    # Primaries for  MET(+jets):
    #

    'EF_xe80':                {10000: TriggerRule(PS=1,ESRate=0.1,comment='Baseline trigger') },
    'EF_xe80_tight':          {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
    'EF_xe90':                {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
    'EF_xe90_tight':          {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
    'EF_xe100':               {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
    'EF_xe100_tight':         {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
    'EF_xe110':               {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },

    #jet+met
    'EF_j80_a4tchad_xe100_tclcw_loose':        {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_j80_a4tchad_xe85_tclcw_dphi2j45xe10':  {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_j110_a4tchad_xe90_tclcw_loose':        {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_j110_a4tchad_xe100_tclcw_veryloose':   {10000: TriggerRule(PS=1,comment='Backup trigger') },
    'EF_j145_a4tchad_L2EFxe90_tclcw':          {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_j170_a4tchad_EFxe80_tclcw':            {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    # New primaries - May 18th
    'EF_xe80T_tclcw':         {10000: TriggerRule(PS=1,comment='Uses XE50_BGRP7 trigger') },
    'EF_xe80_tclcw_tight':    {10000: TriggerRule(PS=1,comment='topo met - XE60') },


    ########################################
    # Support Triggers for MET(+jets):
    # (prescaled jets are elsewhere

    'EF_xe30':                {10000: TriggerRule(rate=0.25,ESRate=0.1,comment='Support trigger') },
    'EF_xe50':                {10000: TriggerRule(rate=0.25,comment='Support trigger') },
    'EF_xe60':                {10000: TriggerRule(rate=0.25,comment='Support trigger') },
  #  'EF_xe70':                {10000: TriggerRule(rate=0.25,maxRate=10,comment='Support trigger') },
    'EF_xe70':                {10000: TriggerRule(rate=0.25,comment='Support trigger') },

    'EF_xs30':                {10000: TriggerRule(rate=0.02,comment='Support trigger - 0.2 Hz requested, but need to limit L1') },
    'EF_xs45':                {10000: TriggerRule(rate=0.02,comment='Support trigger - 0.2 Hz requested, but need to limit L1') },
    'EF_xs60':                {10000: TriggerRule(rate=0.05,comment='Support trigger - 0.2 Hz requested, but need to limit L1') },
    'EF_xs75':                {10000: TriggerRule(rate=0.2,comment='Support trigger') },
    'EF_xs100':               {10000: TriggerRule(rate=0.2,comment='Support trigger') },

    #--- Deal with non-linearity of TE triggers
    'EF_te600': {1000: TriggerRule(rate=0.05,comment='Support trigger'),
                 7000: TriggerRule(rate=0.001,comment='Support trigger') },
    'EF_te1000':{1000: TriggerRule(rate=0.05,comment='Support trigger'),
                 7000: TriggerRule(rate=0.0005,comment='Support trigger') },
    'EF_te1400':{1000: TriggerRule(rate=0.1,ESRate=0.,comment='Support trigger'),
                 7000: TriggerRule(PS=1000,ESRate=0.,comment='Support trigger')},
    'EF_te1600':{1000: TriggerRule(rate=0.25,comment='Support trigger'),
                 8100: TriggerRule(PS=10,comment='Support trigger')},

    'EF_xe30_tclcw':             {10000: TriggerRule(rate=0.25,comment='Support trigger') },
    'EF_xe_NoCut_allL1':  {10000: TriggerRule(PS=0,rerun=True,comment='Calculates met on accepted events')},
    'EF_xe_NoCut_FEB_allL1':  {10000: TriggerRule(PS=0,rerun=True,comment='Calculates FEB met on accepted events')},
    'EF_xe_NoCut_tclcw_allL1':  {10000: TriggerRule(PS=0,rerun=True,comment='Calculates topo met on accepted events')},
    ########################################
    # Extra Triggers for MET(+jets):
    #

    #inside bunch train triggers:
    'EF_xe60T':               {1000: TriggerRule(PS=-1,comment='Uses XE40_BGRP7 trigger - startup only') },
    'EF_xe80T':               {8000: TriggerRule(PS=1,comment='Uses XE50_BGRP7 trigger') },
    'EF_xe80T_tclcw_loose':   {4000: TriggerRule(PS=1,comment='Uses XE40_BGRP7 trigger') },
    'EF_xe80_tclcw_loose':    {4000: TriggerRule(PS=1,comment='Uses XE40 trigger'),
                               8100: TriggerRule(PS=-1,comment='Uses XE40 trigger')},
    'EF_xe100T_tclcw':        {4000: TriggerRule(PS=1,comment='Uses XE40_BGRP7 trigger') },
    'EF_xe100T_tclcw_loose':  {4000: TriggerRule(PS=1,comment='Uses XE40_BGRP7 trigger') },

    #topo cluster MET - only if CPU allows

    'EF_xe75_tclcw':              {4000: TriggerRule(PS=-1,comment='topo met - XE50') },
    'EF_xe80_tclcw':              {4000: TriggerRule(PS=1,comment='topo met - XE50') },
    'EF_xe80_tclcw_verytight':    {5000: TriggerRule(PS=1,comment='topo met - XE70') },
    'EF_xe90_tclcw':              {4000: TriggerRule(PS=1,comment='topo met - XE50') },
    'EF_xe90_tclcw_tight':        {5000: TriggerRule(PS=1,comment='topo met - XE60') },
    'EF_xe90_tclcw_verytight':    {5000: TriggerRule(PS=1,comment='topo met - XE50') },
    'EF_xe100_tclcw_loose':       {4000: TriggerRule(PS=1,comment='topo met - XE50') },
    'EF_xe100_tclcw':             {5000: TriggerRule(PS=1,comment='topo met - XE60') },
    'EF_xe100_tclcw_verytight':   {5000: TriggerRule(PS=1,comment='topo met - XE70') },
    'EF_xe80_tclcw_L2L1Check':    {4000: TriggerRule(PS=1,comment='topo met test chain'),
                                   8100: TriggerRule(PS=-1,comment='topo met test chain')},

    #lower L1 MET in jet+MET
    'EF_j80_a4tchad_xe100_tclcw_veryloose':       {4000: TriggerRule(PS=1,comment='Uses J30_XE40') },
    'EF_j80_a4tchad_xe70_tclcw_dphi2j45xe10':     {4000: TriggerRule(PS=1,comment='Uses J30_XE40') },
    'EF_j110_a4tchad_xe90_tclcw_veryloose':       {4000: TriggerRule(PS=1,comment='Uses J50_XE35') },
    'EF_j145_2j45_a4tchad_L2EFxe70_tclcw':        {4000: TriggerRule(PS=1,comment='Uses 3J15_J50') },

    #lower EF MET
    'EF_j110_a4tchad_xe75_tclcw':                 {5000: TriggerRule(PS=1,comment='Uses J50_XE40') },
    'EF_j145_a4tchad_L2EFxe70_tclcw':             {4000: TriggerRule(PS=1,comment='') },
    'EF_j170_a4tchad_EFxe70_tclcw':               {4000: TriggerRule(PS=1,comment='') },

    #uses "tclcw" jets
    'EF_j110_a4tclcw_xe100_tclcw_veryloose':  {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_j145_a4tclcw_L2EFxe90_tclcw':     {4000: TriggerRule(PS=1,comment='Uses J30_XE40') },

    # Disabled - May 18th
    'EF_xe80T_loose':         {4000: TriggerRule(PS=-1,comment='Uses XE40_BGRP7 trigger') },


    ########################################
    # Additional inclusive XE triggers
    # savannah: https://savannah.cern.ch/bugs/?9635...
    'EF_xe80_tclcw_veryloose': {10000: TriggerRule(PS=1, comment='Inclusive MET baseline whn L1 available') },
    'EF_xe80T_tclcw_veryloose': {10000: TriggerRule(PS=1, comment='Inclusive MET baseline whn L1 available') },
    'EF_xe60_tclcw_loose_delayed': {1000: TriggerRule(PS=1, comment='Delayed Inclusive MET baseline when L1 available and SFO capacity'),
                                    5800: TriggerRule(PS=-1, comment='Delayed Inclusive MET baseline when L1 available and SFO capacity') },
    'EF_xe60T_tclcw_loose_delayed': {1000: TriggerRule(PS=1, comment='Delayed Inclusive MET baseline when L1 available and SFO capacity'),
                                     5800: TriggerRule(PS=-1, comment='Delayed Inclusive MET baseline when L1 available and SFO capacity') },

    ################################################################################

    #### New triggers in 17.1.5.16/17.1.5.17

    # Rerun for new low threshold tau triggers
    'EF_tau27Ti_medium1_L2loose' :  {10000: TriggerRule(PS=0, rerun=True,comment='For rerun of new low tau items')},
    'EF_tau18Ti_medium1_L2loose' :  {10000: TriggerRule(PS=0, rerun=True,comment='For rerun of new low tau items')},
    'EF_tau27T_medium1_L2loose' :   {10000: TriggerRule(PS=0, rerun=True,comment='For rerun of new low tau items')},
    'EF_tau20_medium1_L2loose' :    {10000: TriggerRule(PS=0, rerun=True,comment='For rerun of new low tau items')},
    
    # Monopole triggers
#    'EF_g_nocut_hiptrtL2' :          {10000: TriggerRule(PS=1, comment='Primary for monopole trigger')},

    'L2_g_nocut_hiptrtL2' :          {1000: TriggerRule(PS=1, comment='Primary for monopole trigger')},
    #                                  6501: TriggerRule(PS=-1, comment='Disabled above 6.5')},
    
    'EF_g_nocut_hiptrtL2' :          {1000: TriggerRule(PS=1, comment='Primary for monopole trigger')},
     #                                 6501: TriggerRule(PS=-1, comment='Disabled above 6.5')},
    
    'EF_g_nocut_hiptrtL2_L1EM30'  :  {10000: TriggerRule(PS=-1,comment='Backup monopole trigger')},
    
    # disable new tau triggers
    #chain was remamed
    #'EF_tau27Ti_loose2_tau18Ti_loose2_topo' :  {10000: TriggerRule(PS=-1,comment='Loose tau trigger to be revised')},
    'EF_tau20_loose2_mu15' :                   {10000: TriggerRule(PS=-1, comment='Too high rate')},
    'EF_tau18Ti_loose2_e18vh_medium1' :        {10000: TriggerRule(PS=-1,comment='Too high rate')},

####https://savannah.cern.ch/bugs/?98353 #5
    'EF_tau27Ti_loose2_tau18Ti_loose2_vbf' : {10000: TriggerRule(PS=-1,comment='dihadronic tau w/ VBF -- not used')}, 
    'EF_tau18Ti_loose2_e18vh_medium1_vbf' :    {10000: TriggerRule(PS=1,comment='tau+e VBF')},





    #added for 17.1.5.18
    #    'EF_tau20_loose2_mu15_deta20' : {10000: TriggerRule(PS=-1, comment='Backup')},
    #    'EF_tau18Ti_loose2_e18vh_medium1_deta20': {10000: TriggerRule(PS=-1, comment='Backup')},
    #modified in 17.1.5.22
    'EF_tau18Ti_loose2_e18vh_medium1_deta20': {1000: TriggerRule(PS=1, comment='tau+e with delta-eta trigger'), 
                                               5701: TriggerRule(PS=-1, comment='Limit L2 rate')}, 
    'EF_tau20_loose2_mu15_deta20' : {1000: TriggerRule(PS=1, comment='tau+mu with delta-eta trigger'), 
                                     5701: TriggerRule(PS=-1, comment='tau+mu with delta-eta trigger')}, 
                                     
    #added in 17.1.5.20
    'EF_tau27Ti_loose2_tau18Ti_loose2': {10000: TriggerRule(PS=0, rerun=True, comment='Support trigger for tau27Ti_loose2_tau18Ti_loose2_deta25')}, 

    ####https://savannah.cern.ch/bugs/?98353 #5
    'EF_tau27T_loose2_tau18T_loose2_vbf_fj': {1000: TriggerRule(PS=-1, comment='VBF for Htautau')},
    'EF_tau27T_loose2_tau18T_loose2_vbf_cf': {1000: TriggerRule(PS=-1, comment='VBF for Htautau')}, 

    #topological cut ditau trigger 17.1.5.21
    'EF_tau27Ti_loose2_tau18Ti_loose2_deta20': {1000: TriggerRule(PS=1, comment='alternative topo dihadronic tau'), 
                                                5701: TriggerRule(PS=-1, comment='alternative topo dihadronic tau')},
    
    'EF_tau27Ti_loose2_tau18Ti_loose2_deta25': {10000: TriggerRule(PS=-1, comment='switching this one off')},


    'EF_tau18Ti_loose2_e18vh_medium1_deta25': {1000: TriggerRule(PS=-1, comment='tau+e with delta-eta trigger'),},
    'EF_tau20_loose2_mu15_deta25' : {1000: TriggerRule(PS=-1, comment='tau+mu with delta-eta trigger'),},


    #Switching off _deta18 chains as they are (erroneously) identical to _deta20: 
    'EF_tau27Ti_loose2_tau18Ti_loose2_deta18': {1000: TriggerRule(PS=-1, comment='erroneously identical to deta20'),}, 
    'EF_tau20_loose2_mu15_deta18': {1000: TriggerRule(PS=-1, comment='erroneously identical to deta20'),}, 
    'EF_tau18Ti_loose2_e18vh_medium1_deta18': {1000: TriggerRule(PS=-1, comment='erroneously identical to deta20'),}, 
    


    
    'EF_e12Tvh_loose1_vbf_L1TAU11I_EM10VH_J15CF': {1000: TriggerRule(PS=1, comment='VBF trigger')},
    'EF_e12Tvh_loose1_vbf_L1TAU11I_EM10VH_FJ15': {1000: TriggerRule(PS=1, comment='VBF trigger')},
    'EF_mu13_vbf_cf': {1000: TriggerRule(PS=1, comment='VBF trigger')},
    'EF_mu13_vbf_fj': {1000: TriggerRule(PS=1, comment='VBF trigger')},
    'EF_e18vh_medium1_vbf_cf': {1000: TriggerRule(PS=1, comment='VBF for Htautau and semileptonic H->VV')},
    'EF_e18vh_medium1_vbf_fj': {1000: TriggerRule(PS=1, comment='VBF for Htautau and semileptonic H->VV')},

    'EF_e18vh_medium1_vbf_2L1TAU11I_EM14VH': {1000: TriggerRule(PS=1, comment='VBF for Htautau'),
                                              8501: TriggerRule(PS=-1, comment='Limit L2 rate')},
    'EF_mu15_vbf_L1TAU8_MU10' : {1000: TriggerRule(PS=1, comment='VBF mu-tau trigger'),
                                 8501: TriggerRule(PS=-1, comment='Limit L2 rate')}, 


    #New (17.1.5.18) VBF + b-jets
    'EF_b35_medium_j35_a4tchad_vbf_3L1J15_FJ15': {1000: TriggerRule(PS=1, comment='VBF trigger Hbb')},
    'EF_b35_medium_j35_a4tchad_L2FS_vbf_3L1J15_FJ15': {1000: TriggerRule(PS=1, comment='VBF trigger Hbb')},
    'EF_b35_medium_j35_a4tchad_vbf_2L1FJ15': {1000: TriggerRule(PS=1, comment='VBF trigger Hbb')},
    'EF_b35_medium_j35_a4tchad_L2FS_vbf_2L1FJ15': {1000: TriggerRule(PS=1, comment='VBF trigger Hbb')},
    'EF_b35_NoCut_j35_a4tchad_L2FS_vbf_2L1FJ15': {1000: TriggerRule(rate=0.2,comment='Support Trigger')},
    'EF_b35_NoCut_j35_a4tchad_vbf_2L1FJ15': {1000: TriggerRule(rate=0.2,comment='Support Trigger')},
    'EF_b35_NoCut_j35_a4tchad_L2FS_vbf_3L1J15_FJ15': {1000: TriggerRule(rate=0.2,comment='Support Trigger')},
    'EF_b35_NoCut_j35_a4tchad_vbf_3L1J15_FJ15': {1000: TriggerRule(rate=0.2,comment='Support Trigger')},
        
    
    # this should be only in the trunk TMP (name different in release)

    'L2_mu60_slow1_tight1': {1000: TriggerRule(PS=1.0, comment='Slow particle search'),
                             6501: TriggerRule(PS=-1, comment='disable at peak'),}, 

    # new slow muon triggers
    'EF_mu60_slow1_tight1' :  {10000: TriggerRule(PS=1, comment='Slow muon backup trigger')},

    # LAr noise burst chains to be disabled
    'EF_xe55_LArNoiseBurst': { 0: TriggerRule(PS=-1, comment='LArNoiseBurst'),
                               8001:  TriggerRule(PS=1,ESValue=1,comment='LArNoiseBurst enabling'), },

    #Savannah 97280  (uncomment the following once validation is ok, after 17.1.5.17)
    'EF_g20Tvh_medium': {10000: TriggerRule(PS=0, rerun=True, comment='For understanding TauMass trigger rejection')}, 
    'EF_mu4Ti' : {10000: TriggerRule(PS=0, rerun=True, comment='For understanding TauMass trigger rejection')},
    'EF_mu10i' : {10000: TriggerRule(PS=0, rerun=True, comment='For understanding TauMass trigger rejection')},
    'EF_mu20i_tight' : {10000: TriggerRule(PS=0, rerun=True, comment='For understanding TauMass trigger rejection')},
    'EF_g12Tvh_medium': {10000: TriggerRule(PS=0, rerun=True, comment='For understanding TauMass trigger rejection')}, 
    

    
    #Removed for 17.1.5.18
    #'EF_g20Tvh_loose' : {10000: TriggerRule(PS=-1, comment='Will need a new rule')},
    #'EF_mu4Ti'        : {10000: TriggerRule(PS=-1, comment='Will need a new rule')},
    #'EF_mu10i'        : {10000: TriggerRule(PS=-1, comment='Will need a new rule')},
    #'EF_mu20i_tight'  : {10000: TriggerRule(PS=-1, comment='Will need a new rule')},




    ################################################################################
    # Primaries for  B-jets:
    #
    'EF_2b55_loose_4j55_a4tchad_L2FS':          {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_2b55_loose_4j55_a4tchad':               {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_b55_medium_4j55_a4tchad':               {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_b55_medium_4j55_a4tchad_L2FS':          {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_b80_loose_j80_a4tchad_xe75_tclcw':      {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_b45_looseEF_j145_a4tchad_L2EFxe60_tclcw': {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_b110_loose_j110_a4tchad_xe60_tclcw':    {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_b45_medium_j145_j45_a4tchad_ht500':     {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_b165_medium_j165_a4tchad_ht500':        {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_2b35_loose_j145_2j35_a4tchad':          {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_2b35_loose_j145_j100_j35_a4tchad':      {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
    'EF_2b55_loose_j145_j55_a4tchad':           {10000: TriggerRule(PS=1,comment='Baseline trigger') },

    'EF_b45_tight_4j45_a4tchad':                {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
    'EF_b45_tight_4j45_a4tchad_L2FS':           {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
    'EF_b45_tight_j145_j45_a4tchad_ht400':      {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
    'EF_b55_tight_4j55_a4tchad':                {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
    'EF_b55_tight_4j55_a4tchad_L2FS':           {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },

    # New primaries - May 18th
    'EF_2b35_loose_4j35_a4tchad_L2FS':           {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_2b35_loose_j145_j35_a4tchad':            {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_b45_medium_4j45_a4tchad_L2FS':           {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_b45_mediumEF_j110_j45_xe60_tclcw':       {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_b35_loose_j110_2j35_a4tchad_EFxe80_tclcw': {10000: TriggerRule(PS=1,comment='Baseline trigger') },
    'EF_b240_loose_j240_a10tcem_L2FS':           {10000: TriggerRule(PS=1,comment='Baseline trigger') },


    #single b-tagged jets - should be in coherent prescale with regular jets
    'EF_b15_looseEF_j15_a4tchad':               {10000: TriggerRule(rate=0.1,comment='b-jet trigger') },
    'EF_b25_looseEF_j25_a4tchad':               {10000: TriggerRule(rate=0.1,comment='b-jet trigger') },
    'EF_b35_looseEF_j35_a4tchad_L1J15':         {10000: TriggerRule(rate=0.1,comment='b-jet trigger') },
    'EF_b45_looseEF_j45_a4tchad_L1J15':         {10000: TriggerRule(rate=0.1,comment='b-jet trigger') },
    'EF_b55_looseEF_j55_a4tchad':               {10000: TriggerRule(rate=0.1,comment='b-jet trigger') },
    'EF_b80_looseEF_j80_a4tchad':               {10000: TriggerRule(rate=0.1,comment='b-jet trigger') },
    'EF_b110_looseEF_j110_a4tchad':             {10000: TriggerRule(rate=0.1,comment='b-jet trigger') },
    'EF_b145_loose_j145_a4tchad':               {10000: TriggerRule(rate=0.1,comment='b-jet trigger') },
    'EF_b180_loose_j180_a4tchad_L2j140':        {10000: TriggerRule(rate=0.1,comment='b-jet trigger') },
    'EF_b220_loose_j220_a4tchad_L2j140':        {10000: TriggerRule(rate=0.1,comment='b-jet trigger') },
    'EF_b280_loose_j280_a4tchad_L2j140':        {10000: TriggerRule(rate=0.1,comment='b-jet trigger') },
    'EF_b360_loose_j360_a4tchad_L2j140':        {10000: TriggerRule(PS=1,comment='b-jet trigger') },

    # Disabled ex-primaries - May 18th
    'EF_2b35_medium_3j35_a4tchad_4L1J15':       {10000: TriggerRule(PS=-1,comment='Baseline trigger') },
    'EF_2b35_medium_3j35_a4tchad_L2FS_4L1J15':  {10000: TriggerRule(PS=-1,comment='Baseline trigger') },
    'EF_b55_tight_3j55_a4tchad_4L1J15':         {10000: TriggerRule(PS=-1,comment='Backup Baseline trigger') },
    'EF_b55_tight_3j55_a4tchad_L2FS_4L1J15':    {10000: TriggerRule(PS=-1,comment='Backup Baseline trigger') },


    ########################################
    # Support triggers for bjets:
    #

    'EF_b15_NoCut_j15_a4tchad':               {10000: TriggerRule(rate=0.2,comment='Should be in coherent prescale?') },
    'EF_b35_NoCut_4j35_a4tchad':              {10000: TriggerRule(rate=0.2,comment='Should be in coherent prescale?') },
    'EF_b35_NoCut_4j35_a4tchad_L2FS':         {10000: TriggerRule(rate=0.2,comment='Should be in coherent prescale?') },
    'EF_b55_NoCut_j55_a4tchad':               {10000: TriggerRule(rate=0.4,ESValue=1,comment='Should be in coherent prescale?') },
    'EF_b55_NoCut_j55_a4tchad_L2FS':          {10000: TriggerRule(rate=0.2,comment='Should be in coherent prescale?') },

    'EF_b45_NoCut_4j45_a4tchad_L2FS':         {10000: TriggerRule(rate=0.5,comment='support for b-tagging')},
    'EF_b45_NoCut_4j45_a4tchad':              {10000: TriggerRule(rate=0.5,comment='support for b-tagging')},


    ########################################
    # muon+jets for btagging measurements:

    'EF_mu4T_j15_a4tchad_matched':               {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
    'EF_mu4T_j15_a4tchad_matchedZ':              {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
    'EF_mu4T_j25_a4tchad_matched':               {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
    'EF_mu4T_j25_a4tchad_matchedZ':              {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
    'EF_mu4T_j35_a4tchad_matched':               {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
    'EF_mu4T_j35_a4tchad_matchedZ':              {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },

    'EF_mu4T_j45_a4tchad_matched':               {10000: TriggerRule(PS=-1,comment='disabled') },
    'EF_mu4T_j45_a4tchad_matchedZ':              {10000: TriggerRule(PS=-1,comment='disabled') },
    'EF_mu4T_j55_a4tchad_matched':               {10000: TriggerRule(rate=1.0,ESRate=0.4,comment='Should be in coherent prescale') },
    'EF_mu4T_j55_a4tchad_matchedZ':              {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
    'EF_mu4T_j65_a4tchad_matched':               {10000: TriggerRule(PS=-1,comment='disabled') },
    'EF_mu4T_j80_a4tchad_matched':               {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
    'EF_mu4T_j110_a4tchad_matched':              {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
    'EF_mu4T_j145_a4tchad_matched':              {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
    'EF_mu4T_j180_a4tchad_matched':              {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
    'EF_mu4T_j220_a4tchad_matched':              {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
    'EF_mu4T_j280_a4tchad_matched':              {10000: TriggerRule(PS=1,comment='Should be unprescaled') },
    'EF_mu4T_j360_a4tchad_matched':              {10000: TriggerRule(PS=1,comment='Assumed to be unprescaled') },

    'EF_mu4T_j45_a4tchad_L2FS_matched':          {10000: TriggerRule(PS=-1,comment='disabled') },
    'EF_mu4T_j45_a4tchad_L2FS_matchedZ':         {10000: TriggerRule(PS=-1,comment='disabled') },
    'EF_mu4T_j55_a4tchad_L2FS_matched':          {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
    'EF_mu4T_j55_a4tchad_L2FS_matchedZ':         {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
    'EF_mu4T_j65_a4tchad_L2FS_matched':          {10000: TriggerRule(PS=-1,comment='disabled') },
    'EF_mu4T_j80_a4tchad_L2FS_matched':          {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
    'EF_mu4T_j110_a4tchad_L2FS_matched':         {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
    'EF_mu4T_j145_a4tchad_L2FS_matched':         {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
    'EF_mu4T_j180_a4tchad_L2FS_matched':         {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
    'EF_mu4T_j220_a4tchad_L2FS_matched':         {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
    'EF_mu4T_j280_a4tchad_L2FS_matched':         {10000: TriggerRule(PS=1,comment='Should be unprescaled') },
    'EF_mu4T_j360_a4tchad_L2FS_matched':         {10000: TriggerRule(PS=1,comment='Assumed to be unprescaled') },

    ########################################
    # Extra Triggers for B-jets:
    #

    #looser selection with higher EF rate
    'EF_2b35_loose_4j35_a4tchad':                {5000: TriggerRule(PS=1,comment='b-jet trigger') },
    'EF_b45_medium_4j45_a4tchad':                {5000: TriggerRule(PS=1,comment='b-jet trigger. Will be Disabled after Apr TS') },
    'EF_b145_medium_j145_a4tchad_ht400':         {5000: TriggerRule(PS=1,comment='b-jet trigger') },
    'EF_2b45_loose_j145_j45_a4tchad':            {5000: TriggerRule(PS=1,comment='b-jet trigger') },
    'EF_2b45_loose_j145_j45_a4tchad_EFxe40_tclcw': {5000: TriggerRule(PS=1,comment='b-jet trigger') },


    'EF_b45_medium_j145_j45_a4tchad_ht400':      {5000: TriggerRule(PS=1,comment='b-jet trigger - Disabled after 6e33'),
                                                  6000: TriggerRule(PS=-1, comment='b-jet trigger - Disabled after 6e33') },
    #6000: TriggerRule(PS=-1,comment='b-jet trigger. Will be Disabled after Apr TS')},

    # Fat jet - experimental:
    'EF_b240_loose_j240_a10tcem_L2j140':         {5000: TriggerRule(PS=1,comment='b-jet trigger') },

    #L1 limited triggers
    'EF_b80_loose_j80_a4tchad_xe55_tclcw':       {4000: TriggerRule(PS=-1,comment='b-jet trigger') },
    'EF_b80_loose_j80_a4tchad_xe70_tclcw':       {4000: TriggerRule(PS=1,comment='b-jet trigger') },
    'EF_b80_loose_j80_a4tchad_xe60_tclcw':       {4000: TriggerRule(PS=1,comment='b-jet trigger') },
    'EF_b55_mediumEF_j110_j55_xe60_tclcw':       {4000: TriggerRule(PS=1,comment='b-jet trigger') },
    'EF_b110_loose_j110_a4tchad_xe55_tclcw':     {4000: TriggerRule(PS=1,comment='b-jet trigger') },

    # Disabled Extras - May 18th
    'EF_2b35_loose_3j35_a4tchad_4L1J15':         {5000: TriggerRule(PS=-1,comment='b-jet trigger') },
    'EF_2b35_loose_3j35_a4tchad_L2FS_4L1J15':    {5000: TriggerRule(PS=-1,comment='b-jet trigger') },
    'EF_2b35_loose_j110_2j35_a4tchad':           {4000: TriggerRule(PS=-1,comment='b-jet trigger') },
    'EF_b55_medium_3j55_a4tchad_4L1J15':         {5000: TriggerRule(PS=-1,comment='b-jet trigger. Will be Disabled after Apr TS') },
    'EF_b55_medium_3j55_a4tchad_L2FS_4L1J15':    {5000: TriggerRule(PS=-1,comment='b-jet trigger. Will be Disabled after Apr TS') },


    ################################################################################




    #########################################
    # Exotic triggers
    #

    'EF_j30_a4tcem_eta13_xe30_empty':           {10000: TriggerRule(PS=1.,comment='Baseline menu - autoprescale at L1') },
    'EF_j30_a4tcem_eta13_xe30_firstempty':      {10000: TriggerRule(PS=1.,comment='Baseline menu - autoprescale at L1') },
    'EF_j50_a4tcem_eta13_xe50_empty':           {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
    'EF_j50_a4tcem_eta13_xe50_firstempty':      {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
    'EF_j50_a4tcem_eta25_xe50_empty':           {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
    'EF_j50_a4tcem_eta25_xe50_firstempty':      {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
    'EF_mu10i_loose_g12Tvh_medium_TauMass':      {10000: TriggerRule(PS=1.,comment='Baseline menu - Tau->mugamma') },
    'L2_mu20i_tight_g5_medium_TauMass':          {1000: TriggerRule(PS=1,comment='Baseline menu - Tau->mugamma'),
                                                  6501: TriggerRule(PS=1,comment='Baseline menu - Tau->mugamma')},
    'EF_mu20i_tight_g5_medium_TauMass':          {1000: TriggerRule(PS=1,comment='Baseline menu - Tau->mugamma'),
                                                  6501: TriggerRule(PS=1,comment='Baseline menu - Tau->mugamma')},
    
    'EF_mu10i_loose_g12Tvh_medium':              {10000: TriggerRule(rate=0.1,comment='Support Baseline menu - Tau->mugamma') },
    'EF_mu20i_tight_g5_medium':                  {10000: TriggerRule(rate=0.1,comment='Support Baseline menu - Tau->mugamma') },
  #extra
    'EF_mu20i_tight_g5_loose_TauMass':           {5000: TriggerRule(PS=1.,comment='Baseline menu - Tau->mugamma') },
  #L1 limited:
    'EF_mu4Ti_g20Tvh_medium_TauMass':            {10000: TriggerRule(PS=1.0,comment='Baseline menu'),},

    'EF_mu10i_g10_medium_TauMass':               {4000: TriggerRule(PS=1.0,comment='Extra Primary for Tau->mugamma'),},
    'EF_mu4Ti_g20Tvh_medium':                    {4000: TriggerRule(rate=0.1,comment='Support;Extra Primary for Tau->mugamma'),},
    'EF_mu10i_g10_medium':                       {4000: TriggerRule(rate=0.1,comment='Support;Extra Primary for Tau->mugamma'),},

    'EF_j35_a4tcem_L1TAU_LOF_HV':               {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
    'EF_j35_a4tcem_L1TAU_LOF_HV_EMPTY':         {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
    'EF_j35_a4tcem_L1TAU_LOF_HV_UNPAIRED_ISO':  {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
    'EF_l2j25_a4tcem_Trackless_HV':             {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
    'EF_l2j25_a4tcem_Trackless_HV_EMPTY':       {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
    'EF_l2j25_a4tcem_Trackless_HV_UNPAIRED_ISO':{10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
    'EF_l2j30_a4tcem_2L1MU10_HV':               {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
    'EF_l2j30_a4tcem_2L1MU10_ExtendedEta_HV':   {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
    'EF_l2j30_a4tcem_2L1MU4_HV_EMPTY':          {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
    'EF_l2j30_a4tcem_2L1MU6_HV_UNPAIRED_ISO':   {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },

    'EF_L1RD0_firstempty_NoAlg':                {10000: TriggerRule(PS=10,comment='Baseline menu - long-lived particles') },


    #Rate control for jet trigger
    'L1_5J10':                                  { 1000 : TriggerRule(PS=-1,  comment=''),
                                                  4001 : TriggerRule(PS=-1, comment='pileup sensitive'), },

    # Disabled - May 18th
    'EF_l2j30_a4tcem_2L1MU6_HV':                {10000: TriggerRule(PS=-1.,comment='Baseline menu - long-lived particles') },


    #########################################
    # MinBias triggers
    #

    #BP: to be enabled when randoms are working (savannah #93955)
    #'L2_rd0_filled_NoAlg':                 {6000: TriggerRule(PS=2,comment='Unbiased triggers-reducing rate at the beginning of fill'),
    #                                        1000: TriggerRule(PS=1,comment='Unbiased triggers')},

    'EF_rd0_filled_NoAlg':                 {10000: TriggerRule(rate=5/1000.,comment='Unbiased triggers')}, #hack around prescale of 1000 in rate prediction
    #'EF_rd0_empty_NoAlg':                  {10000 : TriggerRule(rate=0.5,ESValue=1, comment='See also special background streamer'),     },
    'EF_rd0_empty_NoAlg':                  {10000 : TriggerRule(PS=16,ESValue=1, comment='See also special background streamer'),     },


    #temporary set and cap random triggers
  #  'L1_RD0_FILLED':                       {10000: TriggerRule(PS=14*50,comment='set to 100 Hz with 50 bunches ')},
  #  'L1_RD0_FILLED':                       {10000: TriggerRule(rate=200,scaling="bunches",comment='set to 100 Hz with 50 bunches '),},
  ###savannah #93955
    #'L1_RD0_FILLED':                        { 5000: TriggerRule(rate=100,scaling="bunches",comment='minimal rate at high lumi'),
    #                                           4000: TriggerRule(rate=200,scaling="bunches"),
    #                                           1000: TriggerRule(rate=400,scaling="bunches",comment='end-of-fill rate increase')},
    ###savannah 98385
##    'L1_RD0_FILLED': { 5000: TriggerRule(rate=100,scaling="bunches",comment='minimal rate at high lumi'), 
##                       4000: TriggerRule(rate=2000,scaling="bunches")}, 
    ####98385
    'L1_RD0_FILLED': { 5000: TriggerRule(rate=100,scaling="bunches",comment='minimal rate at high lumi'),
                       4000: TriggerRule(rate=1700,scaling="bunches"),
                       3500: TriggerRule(rate=2000,scaling="bunches")}, 
    

  # below does not work! have to set by hand
    #'L1_RD0_FIRSTEMPTY':                   {10000: TriggerRule(rate=1,scaling="bunches",comment='set to 1 Hz with full ring ')},
    #'L1_RD0_EMPTY':                        {10000: TriggerRule(rate=10,scaling="bunches",comment='set to 10 Hz with 2000 empty bunches ')},
    #'L1_RD0_UNPAIRED_ISO':                 {10000: TriggerRule(rate=5,scaling="bunches",comment='set to 5 Hz with 25 unpaired bunches ')},
  #by hand:
    'L1_RD0_FIRSTEMPTY':                   {10000: TriggerRule(PS=1400*1300,comment='set to 1 Hz with full ring ')},
    'L1_RD0_EMPTY':                        {10000: TriggerRule(PS=1400*40*1.1,comment='set to 10 Hz with 350 empty bunches ')},
    'L1_RD0_UNPAIRED_ISO':                 {10000: TriggerRule(PS=1400*10*0.11,comment='set to 5 Hz with 25 unpaired bunches ')},


    # Triggers only used at low mu:
    'EF_mbSpTrk_empty':                    {10000: TriggerRule(PS=-1,comment='Unbiased noise triggers')},
    'EF_mbLucid_eff_unpaired_iso':         {10000: TriggerRule(PS=-1,comment='Unbiased noise triggers')},
    'EF_mbLucid_eff_empty':                {10000: TriggerRule(PS=-1,comment='Unbiased noise triggers')},
    'EF_mbLucid_eff':                      {10000: TriggerRule(PS=-1,comment='Unbiased noise triggers')},
    'EF_mbMbts_1_NoAlg':                   {10000: TriggerRule(PS=-1,comment='Unbiased noise triggers')},
    'EF_mbMbts_1_eff':                     {10000: TriggerRule(PS=-1,comment='Unbiased noise triggers')},
    'EF_mbMbts_2_unpaired_iso_NoAlg':      {10000: TriggerRule(PS=-1,comment='Unbiased noise triggers')},
    'EF_mbMbts_2_NoAlg':                   {10000: TriggerRule(PS=-1,comment='Unbiased noise triggers')},
    'EF_mbSpTrk_unpaired_iso':             {10000: TriggerRule(PS=-1,comment='Unbiased noise triggers')},
    'EF_mbSpTrkVtxMh':                     {10000: TriggerRule(PS=-1,comment='Unbiased noise triggers')},
    'EF_mbSpTrk':                          {10000: TriggerRule(PS=-1,comment='Unbiased noise triggers')},
    'EF_mbSpTrk_noiseSup':                 {10000: TriggerRule(PS=-1,comment='Unbiased noise triggers')},
    'EF_rd0_unpaired_iso_NoAlg':           {10000 : TriggerRule(PS=-1, comment='See also special background streamer')},


    #standard jet triggers
    'EF_fj80_a4tchad':                  { 0 : TriggerRule(rate=0.5, comment='')},
    # savannah 93956, running at 1Hz for now because enabled late - moved to 0.5 Hz - May 18th
    'EF_4j45_a4tchad_L2FS':               {10000: TriggerRule(rate=0.5,comment='for control regions/background in multijet analysis')},
    #susy request 94932
    #this one is disabled
    'EF_6j35_a4tchad_L2FS_5L2j15': {1: TriggerRule(PS=-1,comment='not used'),},
    #these two get 1hz not 0.5hz like other prescaled triggers above (so commented in above)
    'EF_5j45_a4tchad_L2FS': {1: TriggerRule(rate=1.0,comment='for control regions/background in multijet analysis'),},
    'EF_4j65_a4tchad_L2FS': {1: TriggerRule(rate=1.0,comment='for control regions/background in multijet analysis'),},
    #
    'L1_J10_FIRSTEMPTY':                  { 100 : TriggerRule(PS=100, comment=''), },


    #########################################
    # ZH triggers
    # savannah: https://savannah.cern.ch/bugs/?9638...
    'EF_xe60_tclcw_loose_b35_mediumEF_j35_a4tchad':      {10000: TriggerRule(PS=1,  comment='ZH->nnbb trigger') },
    'EF_xe60T_tclcw_loose_b35_mediumEF_j35_a4tchad':     {10000: TriggerRule(PS=1,  comment='ZH->nnbb trigger') },
    'EF_b80_loose_j80_a4tchad_xe60_tclcw_loose' :        {10000: TriggerRule(PS=-1, comment='Possible backup for ZH->nnbb trigger') },
    'EF_b80_loose_j80_a4tchad_xe60T_tclcw_loose' :       {10000: TriggerRule(PS=-1, comment='Possible backup for ZH->nnbb trigger') },
    'EF_j40_a4tchad_xe60_tclcw_dphi2j45xe10_delayed ' :  {10000: TriggerRule(PS=-1, comment='Possible backup for ZH->nnbb trigger') },
    'EF_j40_a4tchad_xe60T_tclcw_dphi2j45xe10_delayed ' : {10000: TriggerRule(PS=-1, comment='Possible backup for ZH->nnbb trigger') },


}


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


calibRules = {
    ################################################################################
    #
    # Calibration and various monitoring streams

    #######################################
    # online ID monitoring
    #BP: these should be IDMon chains and include unpaired_iso
    'L2_IDMon_empty':   {    0: TriggerRule(PS=2, comment='Monitoring - target 5 Hz'),    },
    'L2_IDMon_filled':  {    0: TriggerRule(rate=5, comment='Monitoring'),    },
    'L2_IDMon_unpaired_iso':   {    0: TriggerRule(PS=1, comment='Monitoring - target 5 Hz'),    },


    #######################################
    # offline ID monitoring
    'L2_PixelNoise' : {100: TriggerRule(rate=10, comment='calibration for InDet')    },
    'L2_SCTNoise' :   {100: TriggerRule(rate=10, comment='calibration for InDet')    },
    'EF_InDetMon_FS':      {100: TriggerRule(rate=0.4/100.,ESValue=1,comment="Trigger monitoring")}, #the /100 is due to PS=100 in "no prescale" menu

    #######################################
    # Offline luminosity measurement
    'L2_PixelBeam' : {100: TriggerRule(rate=100, comment='luminosity calibration.')    },

    #######################################
    # Cost monitoring
    'L2_cost_monitor' :  {    0: TriggerRule(propagate=False,PS=2000, comment='Cost monitoring'),    },
    'EF_cost_monitor' :  {    0: TriggerRule(propagate=False,PS=100, comment='Cost monitoring'),    },

    #######################################
    # Tile
    'EF_TileCalib_laser' : {100: TriggerRule(PS=1, comment='Tile calibration')    },

    #######################################
    # ID Calib - copied over from 2011

    'L2_trk9_Central_Tau_IDCalib' :   {100: TriggerRule(PS=12, comment='IDTracks')    },
    'L2_trk9_Fwd_Tau_IDCalib'  :      {100: TriggerRule(PS=12, comment='IDTracks')    },
    'L2_trk16_Central_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk16_Fwd_Tau_IDCalib' :      {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk29_Central_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk29_Fwd_Tau_IDCalib' :      {100: TriggerRule(PS=1, comment='IDTracks')    },

    #######################################
    # Beamspot

    'L2_vtxbeamspot_activeTE_L2Star_B_peb': { 500 : TriggerRule(rate=3), },
    'L2_vtxbeamspot_FSTracks_L2Star_B'    : { 500 : TriggerRule(inputRate=1000),
                                              7500 : TriggerRule(PS=2), },
    'L2_vtxbeamspot_allTE_L2Star_B_peb'   : { 500 : TriggerRule(rate=30), },

    #######################################
    # LAr calib
    'EF_g12Tvh_loose_larcalib':      {100: TriggerRule(rate=1,comment='LAr PEB calibration stream')},
    'EF_g20_loose_larcalib':         {100: TriggerRule(fraction=1,depends_on='EF_g20_loose',comment='LAr PEB calibration stream')},
    'EF_g40_loose_larcalib':         {100: TriggerRule(fraction=1,depends_on='EF_g40_loose',comment='LAr PEB calibration stream')},
    'EF_g60_loose_larcalib':         {100: TriggerRule(fraction=1,depends_on='EF_g60_loose',comment='LAr PEB calibration stream')},
    'EF_g80_loose_larcalib':         {100: TriggerRule(fraction=1,depends_on='EF_g80_loose',comment='LAr PEB calibration stream')},

    'L2_j40_c4cchad_larcalib' :      {100: TriggerRule(rate=1,comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    'L2_j165_c4cchad_larcalib' :     {100: TriggerRule(rate=1,comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    'L2_fj75_c4cchad_larcalib':      {100: TriggerRule(rate=1,comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    'L2_fj140_c4cchad_larcalib':     {100: TriggerRule(rate=1,comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz

    #######################################
    # CosmicCalo
    # BP: note these needs fixed prescales as rates are not predictable

    #BP: to use rates once available
    #'L1_RD1_EMPTY':         {500: TriggerRule(rate=1.0,scaling="bunches", comment="beam background") },
    'L1_RD1_EMPTY':          {500: TriggerRule(PS=2000*1400*1.1, comment="Large empty calo events ~1 Hz") },
    'L1_EM6_EMPTY'  : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L1_FJ30_EMPTY' : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L1_J10_EMPTY'  : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L1_J30_EMPTY'  : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L1_TAU8_EMPTY' : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_L1FJ30_empty_NoAlg' : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_L1J10_empty_NoAlg'  : { 5000: TriggerRule(PS=50, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_L1J30_empty_NoAlg'  : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_PreS_L1EM6_empty'   : { 5000: TriggerRule(PS=35, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_PreS_L1J10_empty'   : { 5000: TriggerRule(PS=40, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_PreS_L1J30_empty'   : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_PreS_L1TAU8_empty'  : { 5000: TriggerRule(PS=45, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_em3_empty_larcalib' : {    0: TriggerRule(PS=-1, comment='cosmic chain for LAr'),    },
    'L2_em6_empty_larcalib' : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_fj30_empty_larcalib': { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_g10_NoCut_cosmic'   : { 5000: TriggerRule(PS=45, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_j10_empty_larcalib' : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_tau8_empty_larcalib': { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_tauNoCut_cosmic'    : { 5000: TriggerRule(PS=55, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },

    'EF_rd1_empty_NoAlg' :  {    0: TriggerRule(PS=1, comment='streamer for L1_RD1_EMPTY'),    },
    'EF_xe10000_L1RD1'   :  {    0: TriggerRule(PS=-1, comment='test trigger, prescaled off'), },
    'EF_PreS_L1EM3_empty' : {    0: TriggerRule(PS=-1, comment='rejection of pre-sampler noise in LAr, disabled in stable beam'),    },
    'EF_PreS_L1EM6_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),    },
    'EF_PreS_L1TAU8_empty' : {   0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),    },#PS=21 (#94047)
    'EF_PreS_L1J10_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),    },
    'EF_PreS_L1J30_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),    },
    'EF_g5_NoCut_cosmic' : {     0: TriggerRule(PS=-1, comment='cosmic chain for egamma, disabled in stable beam'),    },
    'EF_g10_NoCut_cosmic' : {    0: TriggerRule(PS=1, comment='cosmic chain for egamma'),    }, #PS=120 changed due to #94047
    'EF_tauNoCut_cosmic' :  {    0: TriggerRule(PS=1, comment='cosmic chain for taus'),    },#PS=7 changed due to #94047

    'EF_j30_u0uchad_empty_LArNoiseBurst' : {    0: TriggerRule(PS=1, comment='LArNoiseBurst'),    },
    'EF_j165_u0uchad_LArNoiseBurst' :      {    0: TriggerRule(PS=1,ESValue=1, comment='LArNoiseBurst'),    },
    'EF_j55_u0uchad_firstempty_LArNoiseBurst': {    0: TriggerRule(PS=1,ESValue=1, comment='LArNoiseBurst'),    },
    'EF_j55_u0uchad_empty_LArNoiseBurst': {    0: TriggerRule(PS=1,ESValue=1, comment='LArNoiseBurst'),    },
    # https://savannah.cern.ch/bugs/?93569
    'EF_xe45_LArNoiseBurst':               {    0: TriggerRule(PS=1,ESValue=1, comment='LArNoiseBurst'),    },
    #'EF_xe55_LArNoiseBurst':               {    0: TriggerRule(PS=1,ESValue=1, comment='LArNoiseBurst'),    },
    #'EF_xe60_LArNoiseBurst':               {    0: TriggerRule(PS=1,ESValue=1, comment='Backup LArNoiseBurst'),    },

    #new in 17.1.5.12
    'EF_j165_u0uchad_LArNoiseBurstT' :           { 0: TriggerRule(PS=1, comment='LArNoiseBurst'), },
    'EF_j55_u0uchad_firstempty_LArNoiseBurstT' : { 0: TriggerRule(PS=1, comment='LArNoiseBurst'), },
    #and disabled in 17.1.5.13
    'EF_j100_u0uchad_LArNoiseBurstT':            { 0: TriggerRule(PS=-1, comment='LArNoiseBurst replaced by j80'), },
    'EF_j130_u0uchad_LArNoiseBurstT':            { 0: TriggerRule(PS=-1, comment='LArNoiseBurst replaced by j80'), },
    #new in 17.1.5.13, ESValue changed in 17.1.5.14 due to streaming change!!
    'EF_j80_u0uchad_LArNoiseBurstT':             { 0: TriggerRule(PS=1, ESValue=1, comment='LArNoiseBurst'), },
    'EF_j35_u0uchad_empty_LArNoiseBurst':        { 0: TriggerRule(PS=1, ESValue=1, comment='LArNoiseBurst'), },
    'EF_j35_u0uchad_firstempty_LArNoiseBurst':   { 0: TriggerRule(PS=1, ESValue=1, comment='LArNoiseBurst'), },


    'EF_L1J10_empty_NoAlg' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },#PS=10 (#94047)
    'EF_L1J30_empty_NoAlg' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },
    'EF_L1FJ30_empty_NoAlg' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },
    'EF_PreS_L1J30_firstempty' : {100: TriggerRule(PS=10, ESValue=0, comment='rejection of pre-sampler noise in LAr, physics chain') },
    'EF_L1J30_firstempty_NoAlg'   :{
                 #does not seem to work 100:  TriggerRule(rate=1.0,ESRate=1.0, comment='Baseline primary; long-lived particles'),
                 500:  TriggerRule(PS=5,ESValue=10, comment='Baseline primary; long-lived particles')
                 },#temporary hack
    'EF_L1J10_firstempty_NoAlg'   :{
                 #does not seem to work 100:  TriggerRule(rate=1.0,ESRate=1.0, comment='Calorimeter noise monitoring'),
                 500: TriggerRule(PS=50,ESValue=10,scaling="bunches", comment="beam background")
                 }, #temporary hack

    #######################################
    #ZeroBias
    'L1_ZB' :            {     5000 : TriggerRule(PS=5000, comment='target is ~15 Hz at 5e33'),    },
    'EF_zerobias_NoAlg': {     0 : TriggerRule(PS=10, comment='Aim for 1.5 Hz'),},
    #BP: streaming is broken
    'EF_zerobias_Overlay_NoAlg': {     0 : TriggerRule(PS=1, comment='Aim for 15 Hz')},
    #'EF_zerobias_Overlay_NoAlg': {     0 : TriggerRule(PS=-1, comment='Aim for 15 Hz')},

    #Test items
    'EF_HLTTimeBurner' :     {    0: TriggerRule(PS=-1, comment='For testing during no-Beam only'),  },
    'EF_ROBRequestMon' :     {    0: TriggerRule(PS=-1, comment='For testing during no-Beam only'),  },
    'EF_ROBRequestMonUnseeded': { 0: TriggerRule(PS=-1, comment='For testing during no-Beam only'),  },
    'EF_L1Calo_NoAlg' :      {    0: TriggerRule(PS=-1, comment='Streamers for commissioning only'),  },
    'EF_L1MinBias_NoAlg' :   {    0: TriggerRule(PS=-1, comment='Streamers for commissioning only'),  },
    'EF_L1Muon_NoAlg' :      {    0: TriggerRule(PS=-1, comment='Streamers for commissioning only'),  },
    'EF_Standby':            { 100 : TriggerRule(PS=-1, comment='Standby streamer'),},
    'L2_ALFA_Calib':         { 100 : TriggerRule(PS=-1, comment='commissioning trigger for ALFA, not to be used during stable beams'), },
    'EF_ALFA_Phys':          { 100 : TriggerRule(PS=-1, comment='commissioning trigger for ALFA, not to be used during stable beams '), },

    #######################################
    # Cosmic chains:
    'EF_CosmicsAllTeIDSCAN_AllPhysics_TrkHypo' : {100: TriggerRule(PS=-1,  comment='cosmic chain for InDet studies')    },
    'EF_CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo' : {100: TriggerRule(PS=-1, comment='cosmic chain for InDet studies')    },
    'EF_CosmicsAllTeSiTrack_AllPhysics_TrkHypo' : {100: TriggerRule(PS=-1,  comment='cosmic chain for InDet studies')    },
    'EF_CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo' : {100: TriggerRule(PS=-1,  comment='cosmic chain for InDet studies')    },
    'EF_CosmicsAllTeTRTxK_TRTTrkHypo_Pixel' : {100: TriggerRule(PS=-1,  comment='cosmic chain for InDet studies')    },
    'EF_CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics' : {100: TriggerRule(PS=-1, comment='cosmic chain for InDet studies')    },
    'EF_Cosmics_MU11_TRTxK_TRTTrkHypo_AllID': { 100 : TriggerRule(PS=-1, comment='cosmic'), },
    'EF_Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel': { 100 : TriggerRule(PS=1, comment='cosmic'), },
    'EF_CosmicsAllTeEFID' : {100: TriggerRule(PS=-1,comment='cosmic chain for InDet studies')    },
    'EF_L1TRT_NoAlg' : {100: TriggerRule(PS=-1, comment='streamer for L1_TRT')    },

    #######################################
    # VdM chains
    'L2_VdM_BGRP7' : {100: TriggerRule(PS=-1, comment='For VdM scans only')    },
    'L2_VdM_MBTS_2_BGRP7' : {100: TriggerRule(PS=-1, comment='For VdM scans only')    },

   ##############################################################

    ############################################################## 
    #  EB chains disabled when getting physics PS
    #  to get keys for EB run, set EBrun=True
    ############################################################## 
    'EF_eb_random':                  { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), }, 
    'EF_eb_random_empty':            { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), }, 
    'EF_eb_random_firstempty':       { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), }, 
    'EF_eb_random_unpaired_iso':     { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), }, 
    'EF_eb_physics_empty':           { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), }, 
    'EF_eb_physics_firstempty':      { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), }, 
    'EF_eb_physics_unpaired_iso':    { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), }, 
    'EF_eb_physics_unpaired_noniso': { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), }, 
    'EF_eb_physics':                 { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), }, 
    'EF_high_eb_physics':            { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), }, 
    'EF_eb_physics_noL1PS':          { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), }, 



    ############################################################## 
    # L1Calo chain for 25ns
    ##############################################################     
    'EF_L1Calocalibration': {100: TriggerRule(PS=-1, comment='Useful for 25ns running') }, 
    'EF_L1Calocalibration_L1BGRP7': {100: TriggerRule(PS=-1, comment='Useful for 25ns running') }, 

    }

rules=dict(rules,**calibRules)

####################################################################################
#
# Jet triggers
#


delayedcalibRules = {
    #############################################################
    # Delayed jet chains, following current style would require
    # too many subcategories, better to do explicitly:
    #############################################################

    # Jets
    'EF_j15_a4tchad_jetcalibdelayed':   {100: TriggerRule( rate=1.0, comment='Delayed calibration trigger'), },
    'EF_j25_a4tchad_jetcalibdelayed':   {100: TriggerRule( rate=0.5, comment='Delayed calibration trigger'), },
    'EF_j35_a4tchad_jetcalibdelayed':   {100: TriggerRule( rate=0.5, comment='Delayed calibration trigger'), },
    'EF_j55_a4tchad_jetcalibdelayed':   {100: TriggerRule( rate=0.5, comment='Delayed calibration trigger'), },
    'EF_j80_a4tchad_jetcalibdelayed':   {100: TriggerRule( rate=0.5, comment='Delayed calibration trigger'), },
    'EF_j110_a4tchad_jetcalibdelayed':  {100: TriggerRule( rate=0.5, comment='Delayed calibration trigger'), },
    # new in 17.1.5.13
    'EF_mu4T_j15_a4tchad_matched_jetcalibdelayed'      : { 100: TriggerRule( rate=1.0, comment='Delayed calibration trigger'), },
    'EF_mu4T_j15_a4tchad_matchedZ_jetcalibdelayed'     : { 100: TriggerRule( rate=1.0, comment='Delayed calibration trigger'), },
    'EF_mu4T_j80_a4tchad_matched_jetcalibdelayed'      : { 100: TriggerRule( rate=0.5, comment='Delayed calibration trigger'), },
    'EF_mu4T_j80_a4tchad_L2FS_matched_jetcalibdelayed' : { 100: TriggerRule( rate=0.5, comment='Delayed calibration trigger'), },

    # Photons
    'EF_g20_loose_jetcalibdelayed':     {100: TriggerRule( rate=1.0, comment='Delayed calibration trigger'), },
    'EF_g40_loose_jetcalibdelayed':     {100: TriggerRule( rate=1.0, comment='Delayed calibration trigger'), },
    'EF_g60_loose_jetcalibdelayed':     {100: TriggerRule( rate=1.0, comment='Delayed calibration trigger'), },

    # Muons
    'EF_mu4T_j110_a4tchad_matched_jetcalibdelayed':       {100: TriggerRule( rate=0.75 , comment='Delayed calibration trigger'), },
    'EF_mu4T_j145_a4tchad_matched_jetcalibdelayed':       {100: TriggerRule( rate=0.75 , comment='Delayed calibration trigger'), },
    'EF_mu4T_j180_a4tchad_matched_jetcalibdelayed':       {100: TriggerRule( rate=0.75 , comment='Delayed calibration trigger'), },
    'EF_mu4T_j220_a4tchad_matched_jetcalibdelayed':       {100: TriggerRule( rate=0.75 , comment='Delayed calibration trigger'), },
    'EF_mu4T_j110_a4tchad_L2FS_matched_jetcalibdelayed':  {100: TriggerRule( rate=0.75 , comment='Delayed calibration trigger'), },
    'EF_mu4T_j145_a4tchad_L2FS_matched_jetcalibdelayed':  {100: TriggerRule( rate=0.75 , comment='Delayed calibration trigger'), },
    'EF_mu4T_j180_a4tchad_L2FS_matched_jetcalibdelayed':  {100: TriggerRule( rate=0.75 , comment='Delayed calibration trigger'), },
    'EF_mu4T_j220_a4tchad_L2FS_matched_jetcalibdelayed':  {100: TriggerRule( rate=0.75 , comment='Delayed calibration trigger'), },

    }

rules=dict(rules, **delayedcalibRules)

###################################
# Triggers for hadronic calibration
#
# End-of-Fill
# hadCalibTriggers=['EF_hadCalib_trk9_L1RD0', 'EF_hadCalib_trk18_L1RD0']
#hadCalibRules=dict(map(None,hadCalibTriggers,len(hadCalibTriggers)*[{
#                1000: TriggerRule(rate=0.75, comment='end-of-fill, isolated pions for hadronic calibration'),
#                4001: TriggerRule(rate=0.25, comment='end-of-fill, isolated pions for hadronic calibration'),
#                }]))
###savannah #93955
hadCalibRules = {

    #'EF_hadCalib_trk9_L1RD0':    {   6000: TriggerRule(PS=2,comment='half RND rate start-of-fill'),
    #                                 4000: TriggerRule(PS=1,comment='accomodate to RND rate'),
    #                                 1000: TriggerRule(PS=2,comment='accomodate to RND rate')},

    'EF_hadCalib_trk9_L1RD0': { 6000: TriggerRule(PS=2,comment='half RND rate start-of-fill'), 
                                4000: TriggerRule(PS=1,comment='accomodate to RND rate')}, 

    'EF_hadCalib_trk18_L1RD0':   {   6000: TriggerRule(PS=2,comment='half RND rate start-of-fill'),
                                     4000: TriggerRule(PS=1,comment='accomodate to RND rate')},
    }

rules=dict(rules,**hadCalibRules)

# Entire run DISABLED!! See #93955
hadCalibTriggers2=['EF_hadCalib_trk18_L1HA8', 'EF_hadCalib_trk9_L1HA8']
hadCalibRules2=dict(map(None,hadCalibTriggers2,len(hadCalibTriggers2)*[{
                10000: TriggerRule(PS=-1, comment='entire fill, isolated pions for hadronic calibration'),
                }]))
rules=dict(rules,**hadCalibRules2)

#
# Unprescaled Triggers
#
unprescaledJetTriggers=['EF_j360_a4tchad','EF_j360_a4tthad','EF_j360_a4tclcw','EF_j460_a4tchad',
                        'EF_j360_a10tcem','EF_j460_a10tcem','EF_j460_a10tclcw',
                        'EF_L1J350_NoAlg',
                        'EF_fj180_a4tchad','EF_fj220_a4tchad',
                        'EF_2j250_a2tcem',
                        'EF_2j360_a10tclcw_L2FS','EF_2j360_a10tcem_L2FS',
                        'EF_2j360_a10tclcw','EF_2j360_a10tcem',
                        'EF_3j170_a4tchad_L1J75','EF_3j170_a4tclcw_L1J75','EF_3j180_a4tchad_L1J75',
                        'EF_4j80_a4tchad_L2FS','EF_4j80_a4tclcw_L2FS',
                        'EF_4j110_a4tchad_L2FS','EF_4j110_a4tclcw_L2FS',
                        'EF_5j55_a4tchad_L2FS',
                        'EF_5j65_a4tchad_L2FS',
                        'EF_6j45_a4tchad_L2FS_5L2j15','EF_6j45_a4tclcw_L2FS_5L2j15',
                        'EF_6j55_a4tchad_L2FS_5L2j15',
                        'EF_7j45_a4tchad_L2FS_5L2j15',
                        'EF_j145_a4tchad_ht700_L2FS','EF_j170_a4tchad_ht700',
                        'EF_j170_a4tchad_ht800', 'EF_j145_a4tchad_ht800_L2FS', # back-up chains
                        'EF_j180_j145_a4tchad_m2000_dy34',
                        'EF_2j200_a2tcem_m0_dy04',
                        'EF_5j60_a4tchad_L2FS','EF_5j60_a4tclcw_L2FS','EF_6j50_a4tchad_L2FS_5L2j15',
                        'EF_6j50_a4tclcw_L2FS_5L2j15',
                        #'EF_7j40_a4tchad_L2FS_5L2j15','EF_7j40_a4tclcw_L2FS_5L2j15',
                        #'EF_8j40_a4tchad_L2FS_5L2j15','EF_8j40_a4tclcw_L2FS_5L2j15','EF_8j35_a4tchad_L2FS_5L2j15','EF_8j35_a4tclcw_L2FS_5L2j15',
                        'EF_5j55_a4tchad_L2FSPS',
                        'EF_6j55_a4tchad_L2FSPS',
                        'EF_7j55_a4tchad_L2FSPS',
                        'EF_4j110_a4tchad_L2FSPS',
                        'EF_5j60_a4tchad_L2FSPS',
#                        'EF_7j40_a4tchad_L2FSPS_5L2j15',
                        'EF_6j50_a4tchad_L2FSPS_5L2j15',
#                        'EF_8j35_a4tchad_L2FSPS_5L2j15',
                        'EF_4j80_a4tchad_L2FShad',
                        'EF_4j80_a4tchad_L2FShadPS',
                        'EF_5j55_a4tchad_L2FShadPS',
                        'EF_6j45_a4tchad_L2FShadPS',
                        'EF_b360_NoCut_j360_a4tchad',
                        # new multijets add to 'unprescaledJetTriggers' (21 September)
                        'EF_5j50_j45_a4tchad_L2FS_5L2j15',
                        'EF_5j50_2j45_a4tchad_L2FS_5L2j15',
                        'EF_5j50_j45_a4tclcw_L2FS_5L2j15',
                        'EF_5j50_2j45_a4tclcw_L2FS_5L2j15',
                        ]

unprescaledJetRules=dict(map(None,unprescaledJetTriggers,len(unprescaledJetTriggers)*[{
                10000:TriggerRule(PS=1,comment='unprescaled jet trigger'),
                }]))
rules=dict(rules,**unprescaledJetRules)


# disable over 9e33
# removed these from the unprescaledJetTriggers list!!!
# disableat9e33JetTriggers = ['EF_7j40_a4tchad_L2FS_5L2j15',
#                             'EF_7j40_a4tclcw_L2FS_5L2j15',
#                             'EF_7j40_a4tchad_L2FSPS_5L2j15',
#                             'EF_8j40_a4tchad_L2FS_5L2j15',
#                             'EF_8j40_a4tclcw_L2FS_5L2j15',
#                             'EF_8j35_a4tchad_L2FS_5L2j15',
#                             'EF_8j35_a4tclcw_L2FS_5L2j15',
#                             'EF_8j35_a4tchad_L2FSPS_5L2j15']


# disableat9e33JetRules=dict(map(None,disableat9e33JetTriggers,len(disableat9e33JetTriggers)*[{
#     1000:TriggerRule(PS=1,comment='disable at 9e33 jet trigger'),
#     9001:TriggerRule(PS=-1,comment='disable at 9e33 jet trigger'),
#     }]))
# rules=dict(rules,**disableat9e33JetRules)

disableat74e32JetTriggers = ['EF_7j40_a4tchad_L2FS_5L2j15',
                             'EF_7j40_a4tclcw_L2FS_5L2j15',
                             'EF_7j40_a4tchad_L2FSPS_5L2j15',
                             'EF_7j55_a4tchad_L2FSPS',
                             'EF_7j45_a4tchad_L2FS_5L2j15', 
                             'EF_8j40_a4tchad_L2FS_5L2j15',
                             'EF_8j40_a4tclcw_L2FS_5L2j15',
                             'EF_8j35_a4tchad_L2FS_5L2j15',
                             'EF_8j35_a4tclcw_L2FS_5L2j15',
                             'EF_8j35_a4tchad_L2FSPS_5L2j15']


disableat74e32JetRules=dict(map(None,disableat74e32JetTriggers,len(disableat74e32JetTriggers)*[{
    1000:TriggerRule(PS=1,comment='disable at 74e32 jet trigger'),
    7401:TriggerRule(PS=-1,comment='disable at 74e32 jet trigger'),
    }]))
rules=dict(rules,**disableat74e32JetRules)





#
# Jet triggers for Express Stream
#
rules['EF_j180_a4tchad']={10000:TriggerRule(rate=0.5,PTRate=0.5,ESRate=0.4,
                                            comment='prescaled jet trigger; performance; monitoring'),
                          }
expressJetTriggers=['EF_fj55_a4tchad_L2FS','EF_j35_a4tchad']
expressJetRules=dict(map(None,expressJetTriggers,len(expressJetTriggers)*[{
                2501:TriggerRule(rate=0.5,ESRate=0.4,
                                  comment='prescaled jet trigger; performance; monitoring'),
                }]))
rules=dict(rules,**expressJetRules)
#
# Jets with Pass Through requirements
#
L2passthroughJetTriggers=['L2_j140_c4cchad','L2_fj140_c4cchad',
                          'L2_j165_c4cchad',
                          'L2_fj10_a4TTem_fj40_c4cchad',  # This seeds EF_fj45_a4tchad_L2FS
                          'L2_j40_c4cchad']
L2passthroughJetRules=dict(map(None,L2passthroughJetTriggers,len(L2passthroughJetTriggers)*[{
                           10000:TriggerRule(PTRate=0.5,
                           comment='prescaled jet trigger; performance'),
                          }]))
rules=dict(rules,**L2passthroughJetRules)


#####
passthroughJetTriggers=['EF_j15_a4tchad','EF_fj15_a4tchad']


passthroughJetRules=dict(map(None,passthroughJetTriggers,len(passthroughJetTriggers)*[{
                2501:TriggerRule(rate=0.5,PTRate=0.5,
                                  comment='prescaled jet trigger; performance'),
                }]))
rules=dict(rules,**passthroughJetRules)
rules['L2_j10000_c4cchad_L1RD0']={10000: TriggerRule(inputRate=1000,comment='runs L2 jets on random triggers')}
rules['L2_j10_a4TThad_j100000_c4cchad_L1RD0']={10000: TriggerRule(inputRate=1000,comment='runs L2FS calibrated jets on random triggers')}
rules['L2_fj10_a4TTem_fj10000_c4cchad_L1RD0']={10000: TriggerRule(inputRate=1000,comment='runs L1.5 jets on random triggers')}
#added for P1HLT,17.1.5.12
rules['L2_j10000_a4cchad_L1RD0']={10000: TriggerRule(inputRate=1000,comment='runs L2PS jets on random triggers')}

# Not in the menu yet
#rules['L2_j10_a4TThad_j100000_c4cchad_L1RD0']={10000: TriggerRule(inputRate=1000,comment='runs L1.5 jets on random triggers')}

#
# Standard Prescaled Jets
#
#CB taken fj80_a4tchad out here, adding above
#
# May 18th, Note that j280_a4tchad, 'EF_j240_a10tcem', 'EF_j360_a10tclcw', 'EF_4j65_a4tchad_L2FS', 'EF_5j45_a4tchad_L2FS',
# are mirrored in the HadDelayed stream.  There are currently kept in the JetTauETmiss stream for safety.
# Jun 2nd 'EF_4j65_a4tchad_L2FS', 'EF_5j45_a4tchad_L2FS', get 1Hz, 'EF_6j35_a4tchad_L2FS_5L2j15' disabled 94932
prescaledJetTriggers=['EF_j%s_a4tchad' % i for i in [25,55,80,110,145,220]]+\
                     ['EF_fj%s_a4tchad' % i for i in [25,35,110,145]]+\
                     ['EF_j35_a4tclcw','EF_j35_a4tthad','EF_j35_a10tcem',
                      'EF_j110_a10tcem_L2FS',
                      'EF_j240_a10tclcw','EF_2j240_a10tclcw',
                      'EF_2j240_a10tcem_L2FS','EF_2j240_a10tclcw_L2FS',
                      'EF_2j200_a2tcem','EF_2j240_a10tcem',
                      'EF_3j25_a4tchad','EF_3j55_a4tchad_L2FS',
                      'EF_4j25_a4tchad','EF_4j55_a4tchad_L2FS',
                      'EF_5j25_a4tchad',
                      'EF_5j45_a4tclcw_L2FS',
                      'EF_j45_a4tchad_L2FS_L1J15',
                      'EF_fj45_a4tchad_L2FS',
                      #added in 17.1.5.12
                      'EF_fj55_a4tchad_L2FShad',
                      # May 18th - These are mirrored in the HadDelayed stream, but are kept for safety
                      'EF_j280_a4tchad', 'EF_j240_a10tcem','EF_j360_a10tclcw',
                      # Jun 2nd both get 1Hz  'EF_5j45_a4tchad_L2FS', 'EF_4j65_a4tchad_L2FS'; 'EF_6j35_a4tchad_L2FS_5L2j15' disabled
                      #added for 17.1.5.12
                      'EF_b80_NoCut_j80_a4tchad',
                      'EF_b110_NoCut_j110_a4tchad',
                      'EF_b145_NoCut_j145_a4tchad',
                      'EF_b180_NoCut_j180_a4tchad',
                      'EF_b220_NoCut_j220_a4tchad',
                      'EF_b280_NoCut_j280_a4tchad',
                      'EF_5j50_a4tchad_L2FS',
                      'EF_5j50_a4tclcw_L2FS',
                      ]
prescaledJetRules=dict(map(None,prescaledJetTriggers,len(prescaledJetTriggers)*[{
                2501:TriggerRule(rate=0.5,comment='prescaled jet trigger'),
                }]))
rules=dict(rules,**prescaledJetRules)

delayedJetTriggers=  [ 'EF_j220_a10tcem_delayed',
                       'EF_j240_a10tcem_delayed',
                       'EF_j280_a4tchad_delayed',
                       'EF_j360_a10tclcw_delayed',
                       'EF_j145_a4tchad_ht500_L2FS_delayed',
                       'EF_4j65_a4tchad_L2FS_delayed',
                       'EF_5j45_a4tchad_L2FS_delayed',
                     ]

delayedJetRules=dict(map(None,delayedJetTriggers,len(delayedJetTriggers)*[{
    2501:TriggerRule(PS=1.0,comment='delayed stream jet triggers'),
    6501:TriggerRule(PS=-1,comment='delayed stream jet triggers, disable because of SFO limit'),
    }])) 
rules=dict(rules,**delayedJetRules)



#add FSHad and b360_NoCut_j360_a4tchad chains in P1HLT,17.1.5.12
lowmuUnprescaledJetTriggers=['EF_j25_a4tchad_L1MBTS','EF_j25_a4tchad_L1TE20',
                             'EF_fj25_a4tchad_L1MBTS','EF_fj25_a4tchad_L1TE20',
                             'EF_2j15_a4tchad_deta35_FC_L1MBTS',
                             'EF_2j15_a4tchad_deta35_FC_L1TE20',
                             'EF_2j25_a4tchad_deta35_FC_L1MBTS',
                             'EF_2j25_a4tchad_deta35_FC_L1TE20',
                             'EF_j25_fj25_a4tchad_deta50_FC_L1MBTS',
                             'EF_j25_fj25_a4tchad_deta50_FC_L1TE20',
                             'EF_fj45_a4tchad_L2FS_L1MBTS',
                             'EF_fj45_a4tchad_L2FS_L1TE20',
                             'EF_2fj45_a4tchad_deta50_FB_L2FS_L1MBTS',
                             'EF_2fj45_a4tchad_deta50_FB_L2FS_L1TE20'
                             ]
lowmuUnprescaledJetRules=dict(map(None,lowmuUnprescaledJetTriggers,len(lowmuUnprescaledJetTriggers)*[{
                #1:TriggerRule(PS=1,comment='unprescaled jet trigger; low mu'),
                10:TriggerRule(PS=-1,comment='unprescaled jet trigger; low mu only'),
                }]))
rules=dict(rules,**lowmuUnprescaledJetRules)

lowmuPrescaledJetTriggers=['EF_j10_a4tchadloose','EF_j10_a4tchadloose_L1MBTS',
                           'EF_fj10_a4tchadloose','EF_fj10_a4tchadloose_L1MBTS',
                           'EF_j15_a4tchad_L1MBTS','EF_j15_a4tchad_L1TE20',
                           'EF_fj15_a4tchad_L1MBTS','EF_fj15_a4tchad_L1TE20',
                           'EF_3j10_a4tchadloose_L1MBTS','EF_4j10_a4tchadloose_L1MBTS',
                           'EF_2j10_a4tchadloose_deta35_FC_L1MBTS',
                           'EF_j10_fj10_a4tchadloose_deta50_FC_L1MBTS',
                           ]
lowmuPrescaledJetRules=dict(map(None,lowmuPrescaledJetTriggers,len(lowmuPrescaledJetTriggers)*[{
                #1:TriggerRule(rate=2,maxRate=25.0,comment='prescaled jet trigger; low mu'),
                10:TriggerRule(PS=-1,comment='prescaled jet trigger; low mu only'),
                }]))
rules=dict(rules,**lowmuPrescaledJetRules)

unusedJetTriggers=['EF_L1FJ100_NoAlg','EF_L1JE500_NoAlg',
                   'EF_hadCalib_trk9', 'EF_hadCalib_trk9phi',
                   'EF_hadCalib_trk18',
                   'EF_j45_a4tchad',
                   'EF_3j45_a4tchad_L2FS',
                   'EF_4j55_a4tchad_L2FSPS',
                   'EF_5j45_a4tchad_L2FS_5L1J10',
                   'EF_6j45_a4tchad_L2FS',
                   'EF_7j35_a4tchad_L2FS_5L2j15','EF_7j35_a4tclcw_L2FS_5L2j15',
                   'EF_j145_a4tchad_ht600_L2FS','EF_j170_a4tchad_ht600',
                   # Cleaning triggers not yet validated
                   'EF_j45_a4tchad_L2FS_L1J15_testCleaning', 'EF_j55_a4tchad_testCleaning',
                   'EF_j360_a4tchad_testCleaning', 'EF_fj45_a4tchad_L2FS_testCleaning',
                   ]
unusedJetRules=dict(map(None,unusedJetTriggers,len(unusedJetTriggers)*[{
                1:TriggerRule(PS=-1,comment='not used'),
                }]))
rules=dict(rules,**unusedJetRules)


#quick hack to only get baseline triggers
#for rule in rules:
#    if not rule.startswith('EF'): continue
#    if len(rules[rule])!=1: continue
#    if rules[rule].keys()[0]!=10000:
#        rules[rule][rules[rule].keys()[0]] =TriggerRule(PS=-1,comment='Disabled trigger')





########################################################
# Rules for the Enhanced bias triggers (savannah #97241)
########################################################

if EBrun:
    
    rules['EF_eb_physics_noL1PS']=          { 7400 : TriggerRule(PS=2700, comment='EnhancedBias chains'),
                                              6900 : TriggerRule(PS=2517, comment='EnhancedBias chains'),
                                              6300 : TriggerRule(PS=2230, comment='EnhancedBias chains'),
                                              5700 : TriggerRule(PS=2080, comment='EnhancedBias chains'),
                                              4600 : TriggerRule(PS=1680, comment='EnhancedBias chains'),
                                              3700 : TriggerRule(PS=1350, comment='EnhancedBias chains'),
                                              2900 : TriggerRule(PS=1060, comment='EnhancedBias chains'),
                                              2300 : TriggerRule(PS=840, comment='EnhancedBias chains'),
                                              1200 : TriggerRule(PS=440, comment='EnhancedBias chains'), }
    
    #Seeded by RANDOM
    rules['EF_eb_physics']=                 { 7400 : TriggerRule(PS=9, comment='EnhancedBias chains'),
                                              6900 : TriggerRule(PS=8.4, comment='EnhancedBias chains'),
                                              6300 : TriggerRule(PS=7.6, comment='EnhancedBias chains'),
                                              5700 : TriggerRule(PS=6.9, comment='EnhancedBias chains'),
                                              4600 : TriggerRule(PS=5.6, comment='EnhancedBias chains'),
                                              3700 : TriggerRule(PS=4.5, comment='EnhancedBias chains'),
                                              2900 : TriggerRule(PS=3.5, comment='EnhancedBias chains'),
                                              2300 : TriggerRule(PS=2.8, comment='EnhancedBias chains'),
                                              1200 : TriggerRule(PS=1.4, comment='EnhancedBias chains'), }
    
    #Seeded by RANDOM
    rules['EF_high_eb_physics']=            { 10000 : TriggerRule(PS=1, comment='EnhancedBias chains'),}
    
    
    #Over-write rules of standard triggers    
    rules['L1_RD0_FILLED']=                  {10000: TriggerRule(rate=5000,scaling="bunches",comment='seed for EnhancedBias run')}
    
    rules['L2_rd0_filled_NoAlg']=            { 5000: TriggerRule(rate=100,scaling="bunches",comment='Enhanced bias'),
                                               4000: TriggerRule(rate=200,scaling="bunches",comment='Enhanced bias'),
                                               1000: TriggerRule(rate=400,scaling="bunches",comment='Enhanced bias')}
    
    rules['EF_rd0_filled_NoAlg']=            {10000: TriggerRule(rate=5/1000.,comment='Enhanced bias')} #hack around prescale of 1000 in rate prediction
    
    
    
    rules['EF_eb_physics_empty']=           { 7400 : TriggerRule(PS=24, comment='EnhancedBias chains'),
                                              6900 : TriggerRule(PS=22, comment='EnhancedBias chains'),
                                              6300 : TriggerRule(PS=20.4, comment='EnhancedBias chains'),
                                              5700 : TriggerRule(PS=18.5, comment='EnhancedBias chains'),
                                              4600 : TriggerRule(PS=14.9, comment='EnhancedBias chains'),
                                              3700 : TriggerRule(PS=12, comment='EnhancedBias chains'),
                                              2900 : TriggerRule(PS=9.4, comment='EnhancedBias chains'),
                                              2300 : TriggerRule(PS=7.5, comment='EnhancedBias chains'),
                                              1200 : TriggerRule(PS=3.9, comment='EnhancedBias chains'), }
    
    
    rules['EF_eb_physics_firstempty']=      { 7400 : TriggerRule(PS=14, comment='EnhancedBias chains'),
                                              6900 : TriggerRule(PS=13, comment='EnhancedBias chains'),
                                              6300 : TriggerRule(PS=11.9, comment='EnhancedBias chains'),
                                              5700 : TriggerRule(PS=10.8, comment='EnhancedBias chains'),
                                              4600 : TriggerRule(PS=8.7, comment='EnhancedBias chains'),
                                              3700 : TriggerRule(PS=7, comment='EnhancedBias chains'),
                                              2900 : TriggerRule(PS=5.5, comment='EnhancedBias chains'),
                                              2300 : TriggerRule(PS=4.4, comment='EnhancedBias chains'),
                                              1200 : TriggerRule(PS=2.3, comment='EnhancedBias chains'), }
    
    
    rules['EF_eb_physics_unpaired_iso']=    { 7400 : TriggerRule(PS=1.4, comment='EnhancedBias chains'),
                                              5700 : TriggerRule(PS=1, comment='EnhancedBias chains'),}
    
    
    rules['EF_eb_physics_unpaired_noniso']= { 10000 : TriggerRule(PS=1, comment='EnhancedBias chains'), }
    
    rules['L1_RD1_FILLED']=                 { 10000: TriggerRule(PS=135714,scaling="bunches",comment='5 KHz, seed for EnhancedBias run')}
    rules['EF_eb_random']=                  { 100 : TriggerRule(rate=14, comment='EnhancedBias chains'), }
    
    
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


