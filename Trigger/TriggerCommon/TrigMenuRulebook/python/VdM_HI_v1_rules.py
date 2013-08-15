# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'VdM_HI_v1'

rules = {
    
    # VdM scan
    #'L2_VdM_LUCID_BGRP7' : {0 : TriggerRule(PS=1)},
    'L2_VdM_ZDC_A_C_BGRP7' : {0 : TriggerRule(rate=2500)},
    'L1ZDC_A_C_BGRP7_NoAlg' : {0 : TriggerRule(rate=30)},

    # Length-scale scan
    'L2_VdM_ZDC_A_C_VTE50' : {0 : TriggerRule(PS=1)},
    
    #Make sure we can get up to ~5kHz for the beamspot stress tests
    #'L1_MBTS_2' : {0 : TriggerRule(rate=5000)},
    #But start off with 100 Hz
    'L2_vtxbeamspot_FSTracks_SiTrk' : {0 : TriggerRule(PS=1)},  
    
    #10 Hz of MinBias
    #'EF_mbMbts_1_eff' : {0 : TriggerRule(rate=1)},
    #'EF_mbLucid_eff' :  {0 : TriggerRule(rate=1)},
    #'EF_mbZdc_eff' :    {0 : TriggerRule(rate=1)},
    #'EF_mb_BCM_Wide_eff' : {0 : TriggerRule(rate=1)},
    #'EF_rd0_filled_NoAlg' : {0 : TriggerRule(PS=56000)},

  }

