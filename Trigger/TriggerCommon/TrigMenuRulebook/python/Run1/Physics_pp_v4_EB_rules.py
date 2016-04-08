# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from RuleClasses import TriggerRule

tag = 'Physics_pp_v4'

rules = {

  'L1_RD0_FILLED':                       {10000: TriggerRule(PS=300,comment='set to 10 kHz ')},
  'L1_RD1_FILLED':                       {10000: TriggerRule(PS=70000,comment='set to 15 Hz ')},
  #BP: to be enabled when randoms are working
  'L2_rd0_filled_NoAlg':                 {10000: TriggerRule(PS=70,comment='Unbiased triggers')},
  
  'L2_IDMon_filled':                    {10000: TriggerRule(PS=70,comment='Unbiased triggers')},
  'L2_fj10_a4TTem_fj40_c4cchad':        {10000: TriggerRule(PS=70,comment='Unbiased triggers')},
  'L2_fj15_a4TTem_fj50_c4cchad':        {10000: TriggerRule(PS=70,comment='Unbiased triggers')},
  'L2_j10000_c4cchad_L1RD0':            {10000: TriggerRule(PS=70,comment='Unbiased triggers')},
  'L2_PixelBeam':                       {10000: TriggerRule(PS=70,comment='Unbiased triggers')},

  #  EB chains (for eb run check:
  # https://twiki.cern.ch/twiki/bin/viewauth/Atlas/TriggerOnlineExpertManual#EnhancedBias_run_configuration
  # eb_physics_noL1PS  30Hz (might not get that but ok)
  # high_eb_physics    60Hz (should be around 60Hz)
  # eb_physics         30Hz (should be around that)
  # eb_random          15Hz
  'EF_eb_random':                  { 100 : TriggerRule(PS=1, comment='EnhancedBias chains'), },
  'EF_eb_physics':                 { 100 : TriggerRule(PS=10, comment='EnhancedBias chains'), },
  'EF_high_eb_physics':            { 100 : TriggerRule(PS=2, comment='EnhancedBias chains'), },
  'EF_eb_physics_noL1PS':          { 100 : TriggerRule(PS=700, comment='EnhancedBias chains'), },

  'EF_eb_random_empty':            { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
  'EF_eb_random_firstempty':       { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
  'EF_eb_random_unpaired_iso':     { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
  'EF_eb_physics_empty':           { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
  'EF_eb_physics_firstempty':      { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
  'EF_eb_physics_unpaired_iso':    { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
  'EF_eb_physics_unpaired_noniso': { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },

  'EF_rd0_filled_NoAlg':                 {10000: TriggerRule(PS=10,comment='Unbiased triggers')}, # dont change 

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
  'EF_mu60_slow_tight1':            {10000: TriggerRule(PS=1,comment='Baseline trigger') },
  'EF_mu60_slow_outOfTime_tight1':  {10000: TriggerRule(PS=1,comment='Baseline trigger') },

  # -- Muon (+jet) + mET triggers
  'EF_mu24_tight_EFxe40':                      {10000: TriggerRule(PS=1,comment='Baseline trigger') },
  'EF_2mu4T_xe60':                             {10000: TriggerRule(PS=1,comment='Baseline trigger') },
  'EF_2mu8_EFxe30':                            {10000: TriggerRule(PS=1,comment='Baseline trigger') },
  'EF_mu24_j65_a4tchad_EFxe60_tclcw':          {10000: TriggerRule(PS=1,comment='Baseline trigger') },
  'EF_mu4T_j65_a4tchad_xe70_tclcw_veryloose':  {10000: TriggerRule(PS=1,comment='Baseline trigger') },
  'EF_2mu8_EFxe40_tclcw':                      {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') }, 

#  # -- Muon + electron triggers
  'EF_mu18_tight_e7_medium1':       {10000: TriggerRule(PS=1,comment='Baseline trigger') },
#
#  # -- Muon + photon triggers
  'EF_mu24_g20vh_medium':           {10000: TriggerRule(PS=1,comment='Baseline trigger') },
  'EF_mu18_2g10_medium':             {10000: TriggerRule(PS=1,comment='Baseline trigger') },

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

  #--- Muon + Jets for Background extraction
  'EF_mu24_tight_3j35_a4tchad':      {10000: TriggerRule(rate=1.0, comment='Support for Top background evaluation') },
  'EF_mu24_tight_3j45_a4tchad':      {10000: TriggerRule(PS=3.0, comment='Support for Top background evaluation') },
  'EF_mu24_tight_4j35_a4tchad':      {10000: TriggerRule(PS=1.0, comment='Support for Top background evaluation') },
  'EF_mu24_tight_4j45_a4tchad':      {10000: TriggerRule(PS=1.0, comment='Backup for Top background evaluation') }, 
  'EF_mu8_4j45_a4tchad_L2FS':        {10000: TriggerRule(rate=0.5, comment='Support for SUSY background evaluation') },
  'EF_mu24_j65_a4tchad':             {10000: TriggerRule(rate=0.5, comment='Support for EF_mu24_j65_a4tchad_EFxe60_tclcw') },

  #--- Commissioning trigger for L2MuonSA
  'EF_mu4T_l2muonSA':               {10000: TriggerRule(PS=0,rerun=True, comment='Commissioning of L2MuonSA for low pT')}, 
  
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

  #--- L1 streamers
  'EF_L1MU10_NoAlg':                {10000: TriggerRule(rate=0.2, comment='L1 Streamer for MU10; low PT coincidence') },
  'EF_L1MU15_NoAlg':                {10000: TriggerRule(rate=0.2, comment='L1 Streamer for MU15; high PT coincidence') },
  'EF_L12MU4_NoAlg':                {10000: TriggerRule(rate=0.2, comment='L1 Streamer for 2MU4; di-muon') },

  #--- TrigL2MuonSA validation
  'EF_mu24i_tight_l2muonSA':        {10000: TriggerRule(PS=1,comment='TrigL2MuonSA validation for single isolated trigger') },
  'EF_2mu13_l2muonSA':              {10000: TriggerRule(PS=1,comment='TrigL2MuonSA validation for di-muon trigger') },

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
  'EF_mu18i4_tight':                  {3000: TriggerRule(PS=-1,comment='Unused')},
  'EF_2mu10':                         {3000: TriggerRule(PS=-1,comment='Unused')},
  'EF_mu24_j65_a4tchad_EFxe40':      {3000: TriggerRule(PS=-1,comment='To be removed') },

  #--- background studies - hardcoded prescales instead of rate=0.1 due to missing prediction
  'EF_mu4_empty_NoAlg':              {#10000: TriggerRule(rate=0.1, comment='For background study')},
                                      10000: TriggerRule(PS=20000, comment='For background study')},
  'EF_mu4_firstempty_NoAlg':         {#10000: TriggerRule(rate=0.1, comment='For background study')},
                                      10000: TriggerRule(PS=500000, comment='For background study')},
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
  'L1_2MU4':                         {1000: TriggerRule(PS=1.0, comment='end-of-fill Baseline primary BPhys'),
                                      4001: TriggerRule(PS=10, comment='Rate Control;end-of-fill Baseline primary BPhys'),
                                      },


  ########################################
  # Extra Triggers for Muons:
  #
  # -- Special triggers
  'EF_mu20it_tight':                  {10000: TriggerRule(PS=1,comment='Tau efficiency measurement; Tau CP special request') },

  #--- Single-muon-seeded EFFS multi muon with lower threshold
  'EF_mu24_tight_mu6_EFFS':            {5000: TriggerRule(PS=1,comment='Extra trigger for EFFS (lower pT)') },

  #--- Single muon with lower threshold
  'EF_mu40_MSonly_barrel_tight':       {5000: TriggerRule(PS=1,comment='Baseline trigger') },

  # -- for H->ZZ*->llqq
  'EF_2mu6i_DiMu_DY_2j25_a4tchad':     {5000: TriggerRule(PS=1,comment='Extra trigger for low mass Higgs') },
  'EF_2mu6i_DiMu_DY_noVtx_noOS_2j25_a4tchad':     {5000: TriggerRule(rate=0.5,comment='Extra support trigger for low mass Higgs') },

  #prescaled chains
  'EF_2mu4T_Jpsimumu_BarrelOnly_prescaled':  {10000: TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },
  'EF_2mu4T_Jpsimumu_Barrel_prescaled':      {10000: TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },
  'EF_mu4Tmu6_Jpsimumu_Barrel_prescaled':    {10000: TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },
  'EF_mu4Tmu6_Jpsimumu_prescaled':           {10000: TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') },
  'EF_2mu4T_Jpsimumu_prescaled':             {10000: TriggerRule(PS=10,comment='Prescaled Trigger for Muon Stream') }, 
  
  # -- Muon + jet + mEt with lower mEt
  'EF_mu24_j65_a4tchad_EFxe40_tclcw':     {5000: TriggerRule(PS=1,comment='Extra trigger for muon + jet + mEt (lower mEt)') },
  'EF_mu4T_j65_a4tchad_xe60_tclcw_loose': {5000: TriggerRule(PS=1,comment='Extra trigger for muon + jet + mEt (lower mEt)') },

  # -- Muon + topo-cluster mEt
#missing:
  'EF_2mu4T_xe60_tclcw':               {5000: TriggerRule(PS=1,comment='Extra trigger if CPU allows') },
  'EF_2mu8_EFxe30_tclcw':              {5000: TriggerRule(PS=1,comment='Extra trigger if CPU allows') },

  # -- Muon + bjet
  'EF_mu24_tight_b35_mediumEF_j35_a4tchad': {5000: TriggerRule(PS=1,comment='Extra trigger for muon+ b-jet - needs client') },

  # -- Muon + photons with loose
  'EF_mu24_g20vh_loose':               {5000: TriggerRule(PS=1,comment='Extra trigger for muon + photon (medium to loose)') },
  'EF_mu18_2g10_loose':                {5000: TriggerRule(PS=1,comment='Extra trigger for muon + 2 photons (medium to loose)') },

  #--- Slow triggers with lower cut
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

  #Barrel triggers for B->mumu
  'EF_2mu4T_Bmumu_BarrelOnly':   { 9000: TriggerRule(PS=1,comment='Baseline trigger') },
  'EF_mu4Tmu6_Bmumu_Barrel':     { 8000: TriggerRule(PS=1,comment='Baseline trigger') },
  'EF_2mu4T_Bmumu_Barrel':       { 6000: TriggerRule(PS=1,comment='Baseline/end-of-fill trigger') },

  #3mu4T chains
  'EF_3mu4T_Tau':                {10000: TriggerRule(PS=1,comment='Baseline trigger') },

  ########################################
  # Delayed Stream Triggers for B-Physics:
  #

  #di-muon
  'EF_2mu4T_Jpsimumu_BarrelOnly':   { 9000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
  'EF_mu4Tmu6_Jpsimumu_Barrel':     { 8000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
  'EF_2mu4T_Jpsimumu_Barrel':       { 6000: TriggerRule(PS=1,comment='Baseline/end-of-fill delayed  trigger') },

  'EF_2mu4T_Upsimumu_BarrelOnly':      { 9000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
  'EF_mu4Tmu6_Upsimumu_Barrel':        { 8000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
  'EF_2mu4T_Upsimumu_Barrel':          { 6000: TriggerRule(PS=1,comment='Baseline/end-of-fill delayed trigger') },

  'EF_2mu4T_Bmumux_BarrelOnly':   { 9000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
  'EF_mu4Tmu6_Bmumux_Barrel':     { 8000: TriggerRule(PS=1,comment='Baseline delayed trigger') },
  'EF_2mu4T_Bmumux_Barrel':       { 6000: TriggerRule(PS=1,comment='Baseline/end-of-fill delayed trigger') },

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
  'EF_2mu4T_DiMu_BarrelOnly':         { 9000: TriggerRule(rate=1,comment='Support trigger') },
  'EF_mu4Tmu6_DiMu_Barrel':           { 8000: TriggerRule(rate=1,comment='Support trigger') },
  'EF_2mu4T_DiMu_Barrel':             { 6000: TriggerRule(rate=1,comment='Support trigger') },

  #unused chains:
  'EF_2mu6':                          {10000: TriggerRule(PS=-1,comment='monitoring')},
  'EF_2mu4T':                         {10000: TriggerRule(PS=-1,comment='monitoring')},
  'EF_4mu4T':                         {10000: TriggerRule(PS=-1,comment='monitoring')},

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
  'EF_2mu4T_DiMu_l2muonSA':     {1000: TriggerRule(rate=1.0, comment='New algo test')},

  #mu4Tmu6 chains
  'EF_mu4Tmu6_Jpsimumu':         {3000: TriggerRule(PS=1,comment='For startup')},
  'EF_mu4Tmu6_Upsimumu':         {3000: TriggerRule(PS=1,comment='For startup')},
  'EF_mu4Tmu6_Bmumu':            {3000: TriggerRule(PS=1,comment='For startup')},
  'EF_mu4Tmu6_Bmumux':           {3000: TriggerRule(PS=1,comment='For startup')},
  'EF_mu4Tmu6_DiMu':             {3000: TriggerRule(rate=1,comment='For startup')},
  'EF_mu4Tmu6_DiMu_noVtx_noOS':  {3000: TriggerRule(rate=1,comment='For startup')},

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

   #---Primary Combined Electrons/Photons with Muon or MET or jets
  'EF_e24vh_medium1_EFxe30':     {10000: TriggerRule(PS=1.0,comment='Primary'),},                                        
  'EF_e24vh_medium1_EFxe40':     {10000: TriggerRule(PS=1.0,comment='Backup'),},    
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
  'L1_2EM3':      {1000: TriggerRule(rate=5000,comment='Rate Control;J/psi calibration'),
                   1501: TriggerRule(rate=3000,comment='Rate Control;J/psi calibration'),
                   2301: TriggerRule(rate=1500,comment='Rate Control;J/psi calibration'),
                   3000: TriggerRule(rate=800,comment='Rate Control;J/psi calibration'),
                   4000: TriggerRule(rate=300,comment='Rate Control;J/psi calibration'),},
  'L1_2EM3_EM6':  {1000: TriggerRule(rate=4000,comment='J/psi calibration'),
                   2301: TriggerRule(rate=2000,comment='Rate Control;J/psi calibration'),
                   3000: TriggerRule(rate=1000,comment='Rate Control;J/psi calibration'),
                   4000: TriggerRule(rate=500,comment='Rate Control;J/psi calibration'),},
  'L1_2EM3_EM12': {1000: TriggerRule(PS=1,comment='feeds e5_tight_e14_etcut_Jpsi'),
                   1501: TriggerRule(rate=6000,comment='feeds e5_tight1_e14_etcut_Jpsi'),
                   2000: TriggerRule(rate=4000,comment='feeds e5_tight1_e14_etcut_Jpsi'),
                   2301: TriggerRule(rate=3000,comment='feeds e5_tight1_e14_etcut_Jpsi'),
                   3000: TriggerRule(rate=1000,comment='feeds e5_tight1_e14_etcut_Jpsi'),
                   4000: TriggerRule(rate=500,comment='Rate Control;J/psi calibration'),},

  
  'EF_2e5_tight1_Jpsi':         {10000: TriggerRule(rate=1.0,comment='Primary; J/Psi, ')},
  'EF_e5_tight1_e4_etcut_Jpsi': {10000: TriggerRule(rate=3,comment='Primary; Performance from JPsi, rate mostly limited by L1')},
  'EF_e5_tight1_e9_etcut_Jpsi': {10000: TriggerRule(rate=6,comment='Primary; Performance from JPsi, rate mostly limited by L1')},
  'EF_e5_tight1_e14_etcut_Jpsi': {10000: TriggerRule(PS=1,comment='Primary; Performance from JPsi, rate mostly limited by L1'),},
  'EF_e9_tight1_e4_etcut_Jpsi':  {10000: TriggerRule(rate=6,comment='Primary; Performance from JPsi, rate mostly limited by L1')},
  'EF_e14_tight1_e4_etcut_Jpsi': {10000: TriggerRule(PS=1,comment='Primary; Performance from JPsi, rate mostly limited by L1'),},
  'EF_e5_tight1_e5_NoCut':       {10000: TriggerRule(rate=0.5,comment='Performance from JPsi')},
 
 
  #--- Upsilon T&P Electrons (PSed)
  #AT : L1 rates to be adjusted according to actual rates at P1
  'L1_2EM6_EM16VH': {3000: TriggerRule(PS=1,comment='feeds e20vhT_medium1/tight1_g6T_etcut_Upsi '),
                     4000: TriggerRule(rate=500,comment='feeds e20vhT_medium1/tight1_g6T_etcut_Upsi '),},
 
  'EF_e20vhT_medium1_g6T_etcut_Upsi':       {10000: TriggerRule(rate=4,maxRate=4.,comment='Performance from Upsilon')},
  'EF_e20vhT_tight1_g6T_etcut_Upsi':       {10000: TriggerRule(rate=4,maxRate=4.,comment='Performance from Upsilon')},
 
 
  #--- W T&P Electrons (PSed)
  #AT: L1 limited, should we control them by L1 rules? L1 rate is ~const vs lumi
  #BP: to be adjusted
#  'L1_EM16V_XE20': {3000: TriggerRule(rate=1000,maxRate=1500,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*')},
  'L1_EM16V_XE20': {3000: TriggerRule(rate=1000,maxRate=1500,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
                    4000: TriggerRule(rate=500,maxRate=1500,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*')},
#  'L1_EM6_XS45':   {3000: TriggerRule(rate=1000,maxRate=1500,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*')},
  'L1_EM6_XS45':   {3000: TriggerRule(rate=1000,maxRate=1500,comment='Wenu performance trigger, seeding EF_e13_etcutTrk*'),
                    4000: TriggerRule(rate=500,maxRate=1500,comment='Wen  performance trigger, seeding EF_e13_etcutTrk*')},
                   
  'EF_e13_etcutTrk_xs60': {3000: TriggerRule(rate=5,maxRate=6.,comment='Wenu performance trigger')},
  'EF_e13_etcutTrk_xs60_dphi2j15xe20': {3000: TriggerRule(rate=5,maxRate=6.,comment='Wenu performance trigger')}, 
  'EF_e20_etcutTrk_xs60_dphi2j15xe20': {3000: TriggerRule(rate=5,maxRate=6.,comment='backup Wenu performance trigger')},
  'EF_e20_etcutTrk_xe30_dphi2j15xe20': {3000: TriggerRule(rate=5,maxRate=6.,comment='backup Wenu performance trigger')},

     #--- Prescaled Photons
  'L2_g20_loose':              {10000: TriggerRule(PTRate=1.0,comment='Bootstrap and LAr studies')},
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
  'EF_e24vh_loose0':                   {10000: TriggerRule(rate=1.0,comment='Cross-check for e24vh_medium, background estimation via loose selection')},
  'EF_e15vh_medium1':                  {10000: TriggerRule(rate=0.4,rerun=True,comment='Support for combined trigger & ID efficiency')}, 
  'EF_e7T_loose1':                     {10000: TriggerRule(PS=0,rerun=True,comment='Support for combined triggers')},#PS=0 or rate=0?
  'EF_e7T_medium1':                    {10000: TriggerRule(PS=0,rerun=True,comment='Support for combined triggers')}, #PS=0 or rate=0?
  'EF_e12Tvh_loose1':                  {10000: TriggerRule(PS=0,rerun=True,comment='Support for combined triggers')}, #PS=0 or rate=0?
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
  'L2_e5_etcut':                       {10000: TriggerRule(PTValue=1, rate=1.0,comment='Trigger eff. for J/Psi & ID efficiency')},
  'EF_e5_etcut':                       {10000: TriggerRule(comment='Trigger eff. for J/Psi & ID efficiency')},
  

  ########################################
  # Extra Triggers for E/gamma:
  #
  'EF_e22vh_medium1':            {4000: TriggerRule(PS=-1,comment='Primary up to 4e33, to be prescaled as isolation is commissioned ')},
  'EF_e22vhi_medium1':           {1000: TriggerRule(PS=-1,comment='Primary up to 4e33')},
  'EF_e22vh_loose':              {4000: TriggerRule(rate=1.0,maxRate=1.0,depends_on='EF_e22vhi_medium1',comment='Cross-check for e22vh_medium, background estimation via loose selection')},
  'EF_e22vh_loose1':             {4000: TriggerRule(rate=1.0,maxRate=1.0,depends_on='EF_e22vhi_medium1',comment='Cross-check for e22vh_medium, background estimation via loose selection')},
  'EF_e22vh_loose0':             {4000: TriggerRule(rate=1.0,maxRate=1.0,depends_on='EF_e22vhi_medium1',comment='Cross-check for e22vh_medium, background estimation via loose selection')},
  'EF_e22vh_medium1_IDTrkNoCut': {4000: TriggerRule(rate=0.5,maxRate=1.0,depends_on='EF_e22vhi_medium1',comment='monitoring ')},

  'EF_e24vh_loose0_4j55_a4tchad_L2FS':  {10000: TriggerRule(PS=1.0, comment='Support for Top background evaluation') },
  'EF_e24vh_loose0_4j45_a4tchad_L2FS':  {10000: TriggerRule(PS=3.0, comment='Support for Top background evaluation') },
  'EF_e24vh_loose0_3j45_a4tchad_L2FS':  {10000: TriggerRule(PS=15.0, comment='Support for Top background evaluation') }, 

#  'EF_e24vh_medium1':            {1000: TriggerRule(PS=1, rerun=True, ESRate=0.4, comment='Primary from 4e33, to be prescaled to PS=10 as soon as isolation is commissioned; ES and SUSY')},
  'EF_e45_medium1':              {5000: TriggerRule(PS=1.0,comment='Backup primary, prescalable at higher lumi; SM, top, Higgs, Susy, exotics'),},
   #BP: the old tracking chains are for startup only
  'EF_e24vh_medium1_SiTrk':      {1000: TriggerRule(PS=-1, depends_on='EF_e24vh_medium1',comment='Alternative Tracking')},
  'EF_e24vh_medium1_IdScan':     {1000: TriggerRule(PS=-1, depends_on='EF_e24vh_medium1',comment='Alternative Tracking')},

  'EF_2g15vh_medium_g10_medium': {5000: TriggerRule(PS=1.0,comment='Extra'),}, #BP: needed?
  'EF_2e7T_loose1_mu6':          {5000: TriggerRule(PS=1.0,comment='Extra'),},                     
  'EF_e7T_loose1_2mu6':          {5000: TriggerRule(PS=1.0,comment='Extra'),},
  'EF_e18vh_medium1_2e7T_medium1':  {4000: TriggerRule(PS=1.0,comment='Extra Primary - L1 limited, SUSY'),},
  'EF_e24vh_medium1_b35_mediumEF_j35_a4tchad':  {5000: TriggerRule(PS=1.0,comment='Extra Primary'),}, #BP: check stream

   #topo MET (CPU limited)
  'EF_e24vh_medium1_EFxe30_tcem':     {5000: TriggerRule(PS=1.0,comment='Extra'),},    
  'EF_e24vh_medium1_EFxe35_tclcw':    {5000: TriggerRule(PS=1.0,comment='Extra'),},    
  'EF_e24vh_medium1_EFxe35_tcem':     {5000: TriggerRule(PS=1.0,comment='Extra'),},    
  'EF_g40_loose_L2EFxe60_tclcw':      {5000: TriggerRule(PS=1.0,comment='Extra'),},

  'EF_j180_a10tcem_e45_loose1':  {5000: TriggerRule(PS=1.0,comment='Extra Primary for Boosted Top '),}, #should maybe be under jets
  'EF_j240_a10tcem_e45_loose1':  {5000: TriggerRule(PS=1.0,comment='Extra Primary for Boosted Top'),},  #should maybe be under jets
  'EF_j240_a10tcem_e60_loose1':  {5000: TriggerRule(PS=1.0,comment='Extra Primary for Boosted Top'),},  #should maybe be under jets

  'EF_g30_loose_g20_loose':      {1000: TriggerRule(rate=0.4, ESValue=1, comment='primary until end-of-april TS')},
  #'EF_g30_loose_g20_loose':      {1000: TriggerRule(rate=0.4,ESRate=0.4, comment='primary until end-of-april TS')},
  'EF_g35_loose_g25_loose':      {1000: TriggerRule(PS=1, comment='primary until end-of-april TS')},
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

  #tau + met
  'EF_tau29Ti_medium1_xe45_tight':       {10000: TriggerRule(PS=1,comment='Baseline trigger') },
  'EF_tau38T_medium1_xe45_tight':        {10000: TriggerRule(PS=1,comment='Baseline trigger - non-isolated backup') },

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
  
 'EF_tau20_medium1':                     {10000: TriggerRule(rate=0.4,ESValue=1,rerun='True',comment='Support for baseline lep-tau, ALSO used in express stream') },
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


  ########################################
  # Extra Triggers for Tau:
  #

  'EF_2tau29_medium1':                         {1000: TriggerRule(PS=-1,comment='Baseline trigger in Low Lumi') }, #BP: needed?
  'EF_tau29T_medium1_tau20T_medium1':          {4000: TriggerRule(PS=-1,comment='Baseline trigger in Low Lumi') },
  'EF_tau29Ti_medium1_tau20Ti_medium1':        {5000: TriggerRule(PS=1.,comment='Support for Baseline trigger') },


  #  'EF_tau20T_medium1_e15vh_medium1':            {4000: TriggerRule(PS=1,comment='Baseline trigger in Low Lum') }, 
  'EF_tau20T_medium1_e15vh_medium1':            {4000: TriggerRule(PS=-1,comment='Baseline trigger in Low Lum') },

  'EF_tau29T_medium1_xe40_tight':              {1000: TriggerRule(PS=1.,comment='"loose" tau and met for startup') },
  'EF_tau29Ti_medium1_xe40_tight':             {4000: TriggerRule(PS=1.,comment='Baseline trigger if L1 allows') },
  'EF_tau38T_medium1_xe40_tight':              {4000: TriggerRule(PS=1.,comment='Lower met threshold backup to non-isolated chain') },

  'EF_tau29Ti_medium1_xe55_tclcw':             {4000: TriggerRule(PS=1.,comment='Loose L1 MET') },
  'EF_tau29Ti_medium1_xe55_tclcw_tight':       {5000: TriggerRule(PS=1.,comment='Baseline with hadronic topo MET') },
  'EF_tau38T_medium1_xe55_tclcw_tight':        {5000: TriggerRule(PS=1.,comment='Baseline non-isolation with hadronic topo MET') },


  # Disabled triggers (don't used "medium")
  'EF_tau20T_medium_e15vh_medium1':      {5000: TriggerRule(PS=-1.,comment='Unused') },
  'EF_tau20_medium':                     {5000: TriggerRule(PS=-1.,comment='Unused') },
  'EF_tau20_medium_mu15':                {5000: TriggerRule(PS=-1.,comment='Unused') },
  'EF_tau29T_medium1_xe45_tight':        {5000: TriggerRule(PS=-1.,comment='Unused - use tau29Ti_medium1_xe45_tight instead') },
  'EF_2tau29T_medium1':                  {5000: TriggerRule(PS=-1.,comment='Unused - use 2tau29Ti_medium1 instead') },

  ########################################
  # llh triggers (for startup phase only):
  #ditau
  'EF_2tau38T_medium1_llh':                    {1000: TriggerRule(PS=1.,comment='Baseline trigger with llh') },
  'EF_tau29Ti_medium1_llh_tau20Ti_medium1_llh':{1000: TriggerRule(PS=-1.,comment='Support for Baseline trigger with llh') },
  'EF_tau29Ti_tight1_llh_tau20Ti_tight1_llh':  {1000: TriggerRule(PS=-1.,comment='Baseline trigger with llh') },

  #tau + e
  'EF_tau20Ti_medium1_llh_e18vh_medium1':      {1000: TriggerRule(PS=1.,comment='Baseline trigger with llh') },
  'EF_tau38T_medium1_llh_e18vh_medium1':       {1000: TriggerRule(PS=1.,comment='Baseline trigger, non-isolated with llh') },

  #tau + mu
  'EF_tau20_medium1_llh_mu15':                 {1000: TriggerRule(PS=1.,comment='Supports baseline + LLH') },

  #tau + met
  'EF_tau29Ti_medium1_llh_xe40_tight':         {1000: TriggerRule(PS=1.,comment='Supports baseline with llh') },
  'EF_tau29Ti_medium1_llh_xe45_tight':         {1000: TriggerRule(PS=1.,comment='Supports baseline with llh') },

  #single tau
  'EF_tau125_medium1_llh':                     {1000: TriggerRule(PS=1.,comment='Baseline single tau with llh') },
  'EF_tau125_medium1_L2StarA':                 {1000: TriggerRule(PS=1.,comment='Baseline single tau with L2StarA') },
  'EF_tau125_medium1_L2StarB':                 {1000: TriggerRule(PS=1.,comment='Baseline single tau with L2StarB') },
  'EF_tau125_medium1_L2StarC':                 {1000: TriggerRule(PS=1.,comment='Baseline single tau with L2StarC') },




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

  ########################################
  # Support Triggers for MET(+jets):
  # (prescaled jets are elsewhere

  'EF_xe30':                {10000: TriggerRule(rate=0.25,ESRate=0.1,comment='Support trigger') },
  'EF_xe50':                {10000: TriggerRule(rate=0.25,comment='Support trigger') },
  'EF_xe60':                {10000: TriggerRule(rate=0.25,comment='Support trigger') },
#  'EF_xe70':                {10000: TriggerRule(rate=0.25,maxRate=10,comment='Support trigger') },
 'EF_xe70':                {10000: TriggerRule(rate=0.25,maxRate=20,comment='Support trigger') },

  'EF_xs30':                {10000: TriggerRule(rate=0.02,comment='Support trigger - 0.2 Hz requested, but need to limit L1') },
  'EF_xs45':                {10000: TriggerRule(rate=0.02,comment='Support trigger - 0.2 Hz requested, but need to limit L1') },
  'EF_xs60':                {10000: TriggerRule(rate=0.05,comment='Support trigger - 0.2 Hz requested, but need to limit L1') },
  'EF_xs75':                {10000: TriggerRule(rate=0.2,comment='Support trigger') },
  'EF_xs100':               {10000: TriggerRule(rate=0.2,comment='Support trigger') },

  'EF_te600':               {10000: TriggerRule(rate=0.05,comment='Support trigger') },
  'EF_te1000':              {10000: TriggerRule(rate=0.05,comment='Support trigger') },
  'EF_te1400':              {10000: TriggerRule(rate=0.1,ESRate=0.1,maxRate=1,comment='Support trigger') },
  'EF_te1600':              {10000: TriggerRule(rate=0.25,maxRate=1,comment='Support trigger') },

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
  'EF_xe80T_loose':         {4000: TriggerRule(PS=1,comment='Uses XE40_BGRP7 trigger') },
  'EF_xe80T_tclcw':         {5000: TriggerRule(PS=1,comment='Uses XE50_BGRP7 trigger') },
  'EF_xe80T_tclcw_loose':   {4000: TriggerRule(PS=1,comment='Uses XE40_BGRP7 trigger') },
  'EF_xe100T_tclcw':        {4000: TriggerRule(PS=1,comment='Uses XE40_BGRP7 trigger') },
  'EF_xe100T_tclcw_loose':  {4000: TriggerRule(PS=1,comment='Uses XE40_BGRP7 trigger') },

  #topo cluster MET - only if CPU allows

  'EF_xe75_tclcw':              {4000: TriggerRule(PS=-1,comment='topo met - XE50') },
  'EF_xe80_tclcw':              {4000: TriggerRule(PS=1,comment='topo met - XE50') },
  'EF_xe80_tclcw_tight':        {5000: TriggerRule(PS=1,comment='topo met - XE60') },
  'EF_xe80_tclcw_verytight':    {5000: TriggerRule(PS=1,comment='topo met - XE70') },
  'EF_xe90_tclcw':              {4000: TriggerRule(PS=1,comment='topo met - XE50') },
  'EF_xe90_tclcw_tight':        {5000: TriggerRule(PS=1,comment='topo met - XE60') },
  'EF_xe90_tclcw_verytight':    {5000: TriggerRule(PS=1,comment='topo met - XE50') },
  'EF_xe100_tclcw_loose':       {4000: TriggerRule(PS=1,comment='topo met - XE50') },
  'EF_xe100_tclcw':             {5000: TriggerRule(PS=1,comment='topo met - XE60') },
  'EF_xe100_tclcw_verytight':   {5000: TriggerRule(PS=1,comment='topo met - XE70') },

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


  ################################################################################




  ################################################################################
  # Primaries for  B-jets:
  #
  'EF_2b35_medium_3j35_a4tchad_4L1J15':       {10000: TriggerRule(PS=1,comment='Baseline trigger') },
  'EF_2b35_medium_3j35_a4tchad_L2FS_4L1J15':  {10000: TriggerRule(PS=1,comment='Baseline trigger') },
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
  'EF_b55_tight_3j55_a4tchad_4L1J15':         {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
  'EF_b55_tight_3j55_a4tchad_L2FS_4L1J15':    {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') },
  'EF_b55_tight_4j55_a4tchad_L2FS':           {10000: TriggerRule(PS=1,comment='Backup Baseline trigger') }, 

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

  ########################################
  # Support triggers for bjets:
  #

  'EF_b15_NoCut_j15_a4tchad':               {10000: TriggerRule(rate=0.2,comment='Should be in coherent prescale?') },
  'EF_b35_NoCut_4j35_a4tchad':              {10000: TriggerRule(rate=0.2,comment='Should be in coherent prescale?') },
  'EF_b35_NoCut_4j35_a4tchad_L2FS':         {10000: TriggerRule(rate=0.2,comment='Should be in coherent prescale?') },
  'EF_b55_NoCut_j55_a4tchad':               {10000: TriggerRule(rate=0.4,ESValue=1,comment='Should be in coherent prescale?') },
  'EF_b55_NoCut_j55_a4tchad_L2FS':          {10000: TriggerRule(rate=0.2,comment='Should be in coherent prescale?') },


  ########################################
  # muon+jets for btagging measurements:

  'EF_mu4T_j15_a4tchad_matched':               {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
  'EF_mu4T_j15_a4tchad_matchedZ':              {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
  'EF_mu4T_j25_a4tchad_matched':               {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
  'EF_mu4T_j25_a4tchad_matchedZ':              {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
  'EF_mu4T_j35_a4tchad_matched':               {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
  'EF_mu4T_j35_a4tchad_matchedZ':              {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },

  'EF_mu4T_j45_a4tchad_L1J15_matched':               {10000: TriggerRule(PS=-1,comment='disabled') },
  'EF_mu4T_j45_a4tchad_L1J15_matchedZ':              {10000: TriggerRule(PS=-1,comment='disabled') },
  'EF_mu4T_j55_a4tchad_matched':               {10000: TriggerRule(rate=1.0,ESRate=0.4,comment='Should be in coherent prescale') },
  'EF_mu4T_j55_a4tchad_matchedZ':              {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
  'EF_mu4T_j65_a4tchad_matched':               {10000: TriggerRule(PS=-1,comment='disabled') },
  'EF_mu4T_j80_a4tchad_matched':               {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
  'EF_mu4T_j110_a4tchad_matched':              {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
  'EF_mu4T_j145_a4tchad_matched':              {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
  'EF_mu4T_j180_a4tchad_matched':              {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
  'EF_mu4T_j220_a4tchad_matched':              {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
  'EF_mu4T_j280_a4tchad_matched':              {10000: TriggerRule(PS=1,comment='Should be unprescaled') },

  'EF_mu4T_j45_a4tchad_L2FS_L1J15_matched':          {10000: TriggerRule(PS=-1,comment='disabled') },
  'EF_mu4T_j45_a4tchad_L2FS_L1J15_matchedZ':         {10000: TriggerRule(PS=-1,comment='disabled') },
  'EF_mu4T_j55_a4tchad_L2FS_matched':          {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
  'EF_mu4T_j55_a4tchad_L2FS_matchedZ':         {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
  'EF_mu4T_j65_a4tchad_L2FS_matched':          {10000: TriggerRule(PS=-1,comment='disabled') },
  'EF_mu4T_j80_a4tchad_L2FS_matched':          {10000: TriggerRule(rate=1.0,comment='Should be in coherent prescale') },
  'EF_mu4T_j110_a4tchad_L2FS_matched':         {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
  'EF_mu4T_j145_a4tchad_L2FS_matched':         {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
  'EF_mu4T_j180_a4tchad_L2FS_matched':         {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
  'EF_mu4T_j220_a4tchad_L2FS_matched':         {10000: TriggerRule(rate=0.25,comment='Should be in coherent prescale') },
  'EF_mu4T_j280_a4tchad_L2FS_matched':         {10000: TriggerRule(PS=1,comment='Should be unprescaled') },

  ########################################
  # Extra Triggers for B-jets:
  #

  #looser selection with higher EF rate
  'EF_2b35_loose_3j35_a4tchad_4L1J15':         {5000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_2b35_loose_3j35_a4tchad_L2FS_4L1J15':    {5000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_2b35_loose_4j35_a4tchad_L2FS':           {5000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_2b35_loose_4j35_a4tchad':                {5000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_b45_medium_4j45_a4tchad':                {5000: TriggerRule(PS=1,comment='b-jet trigger. Will be Disabled after Apr TS') },
  'EF_b45_medium_4j45_a4tchad_L2FS':           {5000: TriggerRule(PS=1,comment='b-jet trigger. Will be Disabled after Apr TS') },
  'EF_b55_medium_3j55_a4tchad_4L1J15':         {5000: TriggerRule(PS=1,comment='b-jet trigger. Will be Disabled after Apr TS') },
  'EF_b55_medium_3j55_a4tchad_L2FS_4L1J15':    {5000: TriggerRule(PS=1,comment='b-jet trigger. Will be Disabled after Apr TS') },
  'EF_b45_medium_j145_j45_a4tchad_ht400':      {5000: TriggerRule(PS=1,comment='b-jet trigger. Will be Disabled after Apr TS') },
  'EF_b145_medium_j145_a4tchad_ht400':         {5000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_2b35_loose_j145_j35_a4tchad':            {5000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_2b45_loose_j145_j45_a4tchad':            {5000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_2b45_loose_j145_j45_a4tchad_EFxe40_tclcw': {5000: TriggerRule(PS=1,comment='b-jet trigger') },

  # Fat jet - experimental:
  'EF_b240_loose_j240_a10tcem_L2FS':           {5000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_b240_loose_j240_a10tcem_L2j140':         {5000: TriggerRule(PS=1,comment='b-jet trigger') },
 
  #L1 limited triggers
  'EF_b80_loose_j80_a4tchad_xe55_tclcw':       {4000: TriggerRule(PS=-1,comment='b-jet trigger') },
  'EF_b80_loose_j80_a4tchad_xe70_tclcw':       {4000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_b80_loose_j80_a4tchad_xe60_tclcw':       {4000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_b45_mediumEF_j110_j45_xe60_tclcw':       {4000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_b55_mediumEF_j110_j55_xe60_tclcw':       {4000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_b110_loose_j110_a4tchad_xe55_tclcw':     {4000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_b35_loose_j110_2j35_a4tchad_EFxe80_tclcw': {4000: TriggerRule(PS=1,comment='b-jet trigger') },
  'EF_2b35_loose_j110_2j35_a4tchad':           {4000: TriggerRule(PS=1,comment='b-jet trigger') },

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
  'EF_mu20i_tight_g5_medium_TauMass':          {10000: TriggerRule(PS=1.,comment='Baseline menu - Tau->mugamma') },
  'EF_mu10i_loose_g12Tvh_medium':              {10000: TriggerRule(rate=0.1,comment='Support Baseline menu - Tau->mugamma') },
  'EF_mu20i_tight_g5_medium':                  {10000: TriggerRule(rate=0.1,comment='Support Baseline menu - Tau->mugamma') },
#extra
  'EF_mu20i_tight_g5_loose_TauMass':           {5000: TriggerRule(PS=1.,comment='Baseline menu - Tau->mugamma') },
#L1 limited:
  'EF_mu4Ti_g20Tvh_medium_TauMass':            {4000: TriggerRule(PS=1.0,comment='Extra Primary for Tau->mugamma'),},
  'EF_mu10i_g10_medium_TauMass':               {4000: TriggerRule(PS=1.0,comment='Extra Primary for Tau->mugamma'),},
  'EF_mu4Ti_g20Tvh_medium':                    {4000: TriggerRule(rate=0.1,comment='Support;Extra Primary for Tau->mugamma'),},
  'EF_mu10i_g10_medium':                       {4000: TriggerRule(rate=0.1,comment='Support;Extra Primary for Tau->mugamma'),},

  'EF_j35_a4tcem_L1TAU_LOF_HV':               {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
  'EF_j35_a4tcem_L1TAU_LOF_HV_EMPTY':         {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
  'EF_j35_a4tcem_L1TAU_LOF_HV_UNPAIRED_ISO':  {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
  'EF_l2j25_a4tcem_Trackless_HV':             {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
  'EF_l2j25_a4tcem_Trackless_HV_EMPTY':       {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
  'EF_l2j25_a4tcem_Trackless_HV_UNPAIRED_ISO':{10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
  'EF_l2j30_a4tcem_2L1MU6_HV':                {10000: TriggerRule(PS=-1.,comment='Baseline menu - long-lived particles') },
  'EF_l2j30_a4tcem_2L1MU10_HV':               {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') }, 
  'EF_l2j30_a4tcem_2L1MU4_HV_EMPTY':          {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },
  'EF_l2j30_a4tcem_2L1MU6_HV_UNPAIRED_ISO':   {10000: TriggerRule(PS=1.,comment='Baseline menu - long-lived particles') },

  'EF_L1RD0_firstempty_NoAlg':                {10000: TriggerRule(PS=10,comment='Baseline menu - long-lived particles') },


  #Rate control for jet trigger
  'L1_5J10':                                  { 1000 : TriggerRule(PS=-1,  comment=''),
                                                4001 : TriggerRule(PS=-1, comment='pileup sensitive'), },
  

  #########################################
  # MinBias triggers
  #

 #BP: to be enabled when randoms are working
#  'L2_rd0_filled_NoAlg':                 {10000: TriggerRule(rate=100,comment='Unbiased triggers')},
#  'EF_rd0_filled_NoAlg':                 {10000: TriggerRule(rate=5/1000.,comment='Unbiased triggers')}, #hack around prescale of 1000 in rate prediction
#  'EF_rd0_empty_NoAlg':                  {10000 : TriggerRule(rate=0.5,ESValue=1, comment='See also special background streamer'),     },
  'EF_rd0_empty_NoAlg':                  {10000 : TriggerRule(PS=16,ESValue=1, comment='See also special background streamer'),     },
  

  #temporary set and cap random triggers
#  'L1_RD0_FILLED':                       {10000: TriggerRule(PS=14*50,comment='set to 100 Hz with 50 bunches ')},
#  'L1_RD0_FILLED':                       {10000: TriggerRule(rate=200,scaling="bunches",comment='set to 100 Hz with 50 bunches '),},
# below does not work! have to set by hand
  #'L1_RD0_FIRSTEMPTY':                   {10000: TriggerRule(rate=1,scaling="bunches",comment='set to 1 Hz with full ring ')},
  #'L1_RD0_EMPTY':                        {10000: TriggerRule(rate=10,scaling="bunches",comment='set to 10 Hz with 2000 empty bunches ')},
  #'L1_RD0_UNPAIRED_ISO':                 {10000: TriggerRule(rate=5,scaling="bunches",comment='set to 5 Hz with 25 unpaired bunches ')},
#by hand:
  'L1_RD0_FIRSTEMPTY':                   {10000: TriggerRule(PS=1400*1300,comment='set to 1 Hz with full ring ')},
  'L1_RD0_EMPTY':                        {10000: TriggerRule(PS=1400*40,comment='set to 10 Hz with 350 empty bunches ')},
  'L1_RD0_UNPAIRED_ISO':                 {10000: TriggerRule(PS=1400*25/5,comment='set to 5 Hz with 25 unpaired bunches ')},


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
  'EF_rd0_unpaired_iso_NoAlg':           {10000 : TriggerRule(PS=-1, comment='See also special background streamer'),     },


  #standard jet triggers
  'EF_fj80_a4tchad':                  { 0 : TriggerRule(rate=0.5, comment=''), },

  #
  'L1_J10_FIRSTEMPTY':                  { 100 : TriggerRule(PS=100, comment=''), },


}

backgroundRules={
    'L1_LUCID_A_C_EMPTY' :           {    0 : TriggerRule(PS=5000, comment=''),    },
    'L1_LUCID_A_C_UNPAIRED_ISO' :    {    0 : TriggerRule(PS=900, comment=''),    },
    'L1_LUCID_A_C_UNPAIRED_NONISO' : {    0 : TriggerRule(PS=1500, comment=''),    },
    'L1_LUCID_EMPTY' :               {    0 : TriggerRule(PS=400000, comment=''),    },
    'L1_LUCID_UNPAIRED_ISO' :        {    0 : TriggerRule(PS=18000, comment=''),    },
    'L1_LUCID_COMM_UNPAIRED_ISO' :   {    0 : TriggerRule(PS=900, comment=''),    },
    'L1_LUCID_COMM_EMPTY' :          {    0 : TriggerRule(PS=900, comment=''),    },
    'L1_MU4_UNPAIRED_ISO' :          {    0 : TriggerRule(PS=900, comment=''),    },
#    'L1_BCM_Wide_EMPTY' :            {    0 : TriggerRule(PS=1, comment=''),    },
#    'L1_BCM_Wide_UNPAIRED_ISO' :     {    0 : TriggerRule(PS=1, comment=''),    },
#    'L1_BCM_Wide_UNPAIRED_NONISO' :  {    0 : TriggerRule(PS=64, comment=''),    },
#    'L1_BCM_AC_CA_BGRP0' :           {    0 : TriggerRule(PS=10000, comment=''),    },
#    'L1_BCM_AC_CA_UNPAIRED_ISO' :    {    0 : TriggerRule(PS=30, comment=''),    },
    'L1_EM3_UNPAIRED_ISO' :          {    0 : TriggerRule(PS=22, comment=''),    },
    'L1_FJ30_UNPAIRED_ISO' :         {    0 : TriggerRule(PS=22, comment=''),    },
    'L1_J10_UNPAIRED_ISO' :          {    0 : TriggerRule(PS=22, comment=''),    },
    'L1_J10_UNPAIRED_NONISO' :       {    0 : TriggerRule(PS=22, comment=''),    },
    'EF_Background':                 {    1 : TriggerRule(PS=1, comment=''),     },
    'EF_Background_RD0_UNPAIRED_ISO':{    1 : TriggerRule(PS=5, comment='target 1 Hz'),     },
#    'EF_Background_RD0_EMPTY':       {    1 : TriggerRule(rate=1, comment=''),     },
    'EF_Background_RD0_EMPTY':       {    1 : TriggerRule(PS=16, comment='target 1 Hz'),     },
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
    #'L2_IDMon_filled':  {    0: TriggerRule(rate=5, comment='Monitoring'),    },
    'L2_IDMon_unpaired_iso':   {    0: TriggerRule(PS=1, comment='Monitoring - target 5 Hz'),    },


    #######################################
    # offline ID monitoring
    'L2_PixelNoise' : {100: TriggerRule(rate=10, comment='calibration for InDet')    },
    'L2_SCTNoise' :   {100: TriggerRule(rate=10, comment='calibration for InDet')    },
    'EF_InDetMon_FS':      {100: TriggerRule(rate=0.4/100.,ESValue=1,comment="Trigger monitoring")}, #the /100 is due to PS=100 in "no prescale" menu

    #######################################
    # Offline luminosity measurement
    #'L2_PixelBeam' : {100: TriggerRule(rate=100, comment='luminosity calibration.')    },
    
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
    'L2_vtxbeamspot_FSTracks_L2Star_B'    : { 500 : TriggerRule(inputRate=1000), },
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
    'L1_RD1_EMPTY':          {500: TriggerRule(PS=2000*1400, comment="Large empty calo events ~1 Hz") },
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
    'EF_PreS_L1EM3_empty' : {    0: TriggerRule(PS=-1, comment='rejection of pre-sampler noise in LAr, disabled in stable beam'),    },
    'EF_PreS_L1EM6_empty' : {    0: TriggerRule(PS=25, comment='rejection of pre-sampler noise in LAr'),    },
    'EF_PreS_L1TAU8_empty' : {   0: TriggerRule(PS=21, comment='rejection of pre-sampler noise in LAr'),    },
    'EF_PreS_L1J10_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),    },
    'EF_PreS_L1J30_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),    },
    'EF_g5_NoCut_cosmic' : {     0: TriggerRule(PS=-1, comment='cosmic chain for egamma, disabled in stable beam'),    },
    'EF_g10_NoCut_cosmic' : {    0: TriggerRule(PS=120, comment='cosmic chain for egamma'),    },
    'EF_tauNoCut_cosmic' :  {    0: TriggerRule(PS=17, comment='cosmic chain for taus'),    },

    'EF_j30_u0uchad_empty_LArNoiseBurst' : {    0: TriggerRule(PS=1, comment='LArNoiseBurst'),    },
    'EF_j165_u0uchad_LArNoiseBurst' :      {    0: TriggerRule(PS=1,ESValue=1, comment='LArNoiseBurst'),    },
    'EF_j55_u0uchad_firstempty_LArNoiseBurst': {    0: TriggerRule(PS=1,ESValue=1, comment='LArNoiseBurst'),    },
    'EF_j55_u0uchad_empty_LArNoiseBurst': {    0: TriggerRule(PS=1,ESValue=1, comment='LArNoiseBurst'),    },
    'EF_xe55_LArNoiseBurst':               {    0: TriggerRule(PS=1,ESValue=1, comment='LArNoiseBurst'),    },
    'EF_xe60_LArNoiseBurst':               {    0: TriggerRule(PS=1,ESValue=1, comment='Backup LArNoiseBurst'),    },
    
    'EF_L1J10_empty_NoAlg' : {    0: TriggerRule(PS=10, comment='cosmic chain for LAr'),    },
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
    #  EB chains
#   'EF_eb_random':                  { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
#   'EF_eb_random_empty':            { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
#   'EF_eb_random_firstempty':       { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
#   'EF_eb_random_unpaired_iso':     { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
#   'EF_eb_physics_empty':           { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
#   'EF_eb_physics_firstempty':      { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
#   'EF_eb_physics_unpaired_iso':    { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
#   'EF_eb_physics_unpaired_noniso': { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
#   'EF_eb_physics':                 { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
#   'EF_high_eb_physics':            { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },
#   'EF_eb_physics_noL1PS':          { 100 : TriggerRule(PS=-1, comment='EnhancedBias chains'), },

    }

rules=dict(rules,**calibRules)

####################################################################################
#
# Jet triggers 
#
# Triggers for hadronic calibration
#


# End-of-Fill
hadCalibTriggers=['EF_hadCalib_trk9_L1RD0', 'EF_hadCalib_trk18_L1RD0', 'EF_hadCalib_trk9_L1HA8'] 
hadCalibRules=dict(map(None,hadCalibTriggers,len(hadCalibTriggers)*[{
                4001: TriggerRule(rate=0.5, comment='end-of-fill, isolated pions for hadronic calibration'),
                }]))
rules=dict(rules,**hadCalibRules)

# Entire run
hadCalibTriggers2=['EF_hadCalib_trk18_L1HA8']
hadCalibRules2=dict(map(None,hadCalibTriggers2,len(hadCalibTriggers2)*[{
    10000: TriggerRule(rate=0.25, comment='entire fill, isolated pions for hadronic calibration'),
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
                        'EF_6j50_a4tclcw_L2FS_5L2j15','EF_7j40_a4tchad_L2FS_5L2j15','EF_7j40_a4tclcw_L2FS_5L2j15',
                        'EF_8j40_a4tchad_L2FS_5L2j15','EF_8j40_a4tclcw_L2FS_5L2j15','EF_8j35_a4tchad_L2FS_5L2j15','EF_8j35_a4tclcw_L2FS_5L2j15',
                        'EF_5j55_a4tchad_L2FSPS',
                        'EF_6j55_a4tchad_L2FSPS',
                        'EF_7j55_a4tchad_L2FSPS',
                        'EF_4j110_a4tchad_L2FSPS',
                        'EF_5j60_a4tchad_L2FSPS',
                        'EF_7j40_a4tchad_L2FSPS_5L2j15',
                        'EF_6j50_a4tchad_L2FSPS_5L2j15',
                        'EF_8j35_a4tchad_L2FSPS_5L2j15', 
                        ]
unprescaledJetRules=dict(map(None,unprescaledJetTriggers,len(unprescaledJetTriggers)*[{
                10000:TriggerRule(PS=1,comment='unprescaled jet trigger'),
                }]))
rules=dict(rules,**unprescaledJetRules)

#
# Jet triggers for Express Stream
#
rules['EF_j180_a4tchad']={10000:TriggerRule(rate=0.5,PTValue=1,ESRate=0.4,
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
                          'L2_fj10_a4TTem_fj40_c4cchad',
                          'L2_j40_c4cchad']
L2passthroughJetRules=dict(map(None,L2passthroughJetTriggers,len(L2passthroughJetTriggers)*[{
                10000:TriggerRule(PTRate=0.5,
                                  comment='prescaled jet trigger; performance'),
                }]))
rules=dict(rules,**L2passthroughJetRules)


#####
passthroughJetTriggers=['EF_j15_a4tchad','EF_fj15_a4tchad',
                        'EF_fj45_a4tchad_L2FS',
                        'EF_j145_a4tchad','EF_fj145_a4tchad',
                        ]


passthroughJetRules=dict(map(None,passthroughJetTriggers,len(passthroughJetTriggers)*[{
                2501:TriggerRule(rate=0.5,PTValue=1,
                                  comment='prescaled jet trigger; performance'),
                }]))
rules=dict(rules,**passthroughJetRules)
rules['L2_j10000_c4cchad_L1RD0']={10000: TriggerRule(inputRate=1000,comment='runs L2 jets on random triggers')}
rules['L2_fj10_a4TTem_fj10000_c4cchad_L1RD0']={10000: TriggerRule(inputRate=1000,comment='runs L1.5 jets on random triggers')} 
#
# Standard Prescaled Jets
#
#CB taken fj80_a4tchad out here, adding above
prescaledJetTriggers=['EF_j%s_a4tchad' % i for i in [25,55,80,110,145,220,280]]+\
                     ['EF_fj%s_a4tchad' % i for i in [25,35,110]]+\
                     ['EF_j35_a4tclcw','EF_j35_a4tthad','EF_j35_a10tcem',
                      'EF_j110_a10tcem_L2FS',
                      'EF_j240_a10tcem','EF_j240_a10tclcw','EF_2j240_a10tclcw','EF_j360_a10tclcw',
                      'EF_2j240_a10tcem_L2FS','EF_2j240_a10tclcw_L2FS',
                      'EF_2j200_a2tcem','EF_2j240_a10tcem',
                      'EF_3j25_a4tchad','EF_3j55_a4tchad_L2FS',
                      'EF_4j25_a4tchad','EF_4j55_a4tchad_L2FS',
                      'EF_4j65_a4tchad_L2FS',
                      'EF_5j25_a4tchad','EF_5j45_a4tchad_L2FS',
                      'EF_5j45_a4tclcw_L2FS',
                      'EF_6j35_a4tchad_L2FS_5L2j15',
                      'EF_j45_a4tchad_L2FS_L1J15' 
                      ]
prescaledJetRules=dict(map(None,prescaledJetTriggers,len(prescaledJetTriggers)*[{
                2501:TriggerRule(rate=0.5,comment='prescaled jet trigger'),
                }]))
rules=dict(rules,**prescaledJetRules)

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
                             'EF_2fj45_a4tchad_deta50_FB_L2FS_L1TE20',
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
                   'EF_3j45_a4tchad_L2FS','EF_4j45_a4tchad_L2FS',
                   'EF_4j55_a4tchad_L2FSPS',
                   'EF_5j45_a4tchad_L2FS_5L1J10',
                   'EF_6j45_a4tchad_L2FS',
                   'EF_7j35_a4tchad_L2FS_5L2j15','EF_7j35_a4tclcw_L2FS_5L2j15',
                   'EF_j145_a4tchad_ht600_L2FS','EF_j170_a4tchad_ht600',
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
