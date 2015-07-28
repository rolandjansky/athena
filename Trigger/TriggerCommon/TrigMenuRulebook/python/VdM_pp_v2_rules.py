# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'VdM_pp_v2'

rules = {

  #Target 14 kHz of ID PEB chains
  'L1_MBTS_2_BGRP7' : {0 : TriggerRule(PS=2)},
  'L2_VdM_MBTS_2_BGRP7' : {0 : TriggerRule(PS=1)},
  #Target 7 kHz of tile PEB
  'L2_VdM_Monitoring' : {0 : TriggerRule(PS=2)},

  #Make sure we can get up to ~5kHz for the beamspot stress tests
  'L1_MBTS_2' : {0 : TriggerRule(rate=5000)},
  #But start off with 100 Hz
  'L2_vtxbeamspot_FSTracks_SiTrk_MBTS' : {0 : TriggerRule(inputRate=100)},  

  #10 Hz of MinBias
  'EF_mbMbts_1_eff' : {0 : TriggerRule(rate=1)},
  'EF_mbLucid_eff' :  {0 : TriggerRule(rate=1)},
  'EF_mbZdc_eff' :    {0 : TriggerRule(rate=1)},
  'EF_mb_BCM_Wide_eff' : {0 : TriggerRule(rate=1)},
  'EF_rd0_filled_NoAlg' : {0 : TriggerRule(PS=56000)},

  }

