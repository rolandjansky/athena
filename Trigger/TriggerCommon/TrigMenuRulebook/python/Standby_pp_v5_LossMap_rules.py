# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'BeamLossMap'
doALFA=False

rules={}

########################################
# first import physics_pp_v5 as it is

import Standby_pp_v5_rules

rules.update(Standby_pp_v5_rules.rules)



#=======================================
# disable Seed of Bkg stream

L1_list=['L1_BCM_AC_CA_BGRP0','L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO','L1_J30.31ETA49_UNPAIRED_ISO','L1_J12_UNPAIRED_ISO','L1_J12_UNPAIRED_NONISO','L1_J12_ABORTGAPNOTCALIB','L1_MU4_UNPAIRED_ISO','L1_BCM_AC_UNPAIRED_ISO','L1_BCM_CA_UNPAIRED_ISO','L1_BCM_AC_UNPAIRED_NONISO','L1_BCM_CA_UNPAIRED_NONISO','L1_J30.31ETA49_UNPAIRED_NONISO','L1_BCM_AC_ABORTGAPNOTCALIB','L1_BCM_CA_ABORTGAPNOTCALIB','L1_BCM_Wide_ABORTGAPNOTCALIB','L1_BCM_AC_CALIB','L1_BCM_CA_CALIB','L1_BCM_Wide_CALIB','L1_J50_UNPAIRED_ISO','L1_J50_UNPAIRED_NONISO','L1_J50_ABORTGAPNOTCALIB']
rules.update(dict(map(None,L1_list,len(L1_list)*[{
    10 : TriggerRule(PS=-1, comment='disabled for toroid off run'),
    }])))

#=======================================
# Enable chains for the beam loss map

rules['L1_BCM_AC_UNPAIRED_ISO']     = { 10000 : TriggerRule(PS=1, comment='L1 for Loss Map', rerun=0) }
rules['L1_BCM_CA_UNPAIRED_ISO']     = { 10000 : TriggerRule(PS=1, comment='L1 for Loss Map', rerun=0) }
rules['L1_J12_UNPAIRED_ISO']        = { 10000 : TriggerRule(PS=1, comment='L1 for Loss Map', rerun=0) }
rules['L1_BCM_AC_CA_BGRP0']         = { 10000 : TriggerRule(PS=1, comment='L1 for Loss Map', rerun=0) }

#=======================================

if doALFA:

    # ALFA rules

    # -- disable first
    L1_list=['L1_ALFA_ELAST1', 'L1_ALFA_ELAST2', 'L1_ALFA_ELAST11', 'L1_ALFA_ELAST12', 'L1_ALFA_ELAST13', 'L1_ALFA_ELAST14', 'L1_ALFA_ELAST15', 'L1_ALFA_ELAST15_Calib', 'L1_ALFA_ELAST16', 'L1_ALFA_ELAST17', 'L1_ALFA_ELAST18', 'L1_ALFA_ELAST18_Calib', 'L1_BGRP1_ALFA_BGT', 'L1_BGRP4_ALFA_BGT', 'L1_BGRP10_ALFA_BGT', 'L1_ALFA_SHOWSYST5', 'L1_ALFA_SYST9', 'L1_ALFA_SYST10', 'L1_ALFA_SYST11', 'L1_ALFA_SYST12', 'L1_ALFA_SYST17', 'L1_ALFA_SYST18', 'L1_ALFA_ANY', 'L1_ALFA_ANY_EMPTY', 'L1_ALFA_ANY_FIRSTEMPTY', 'L1_ALFA_ANY_UNPAIRED_ISO', 'L1_ALFA_ANY_UNPAIRED_NONISO', 'L1_ALFA_ANY_BGRP10', 'L1_ALFA_ANY_CALIB', 'L1_ALFA_ANY_ABORTGAPNOTCALIB', 'L1_ALFA_B7L1U', 'L1_ALFA_B7L1L', 'L1_ALFA_A7L1U', 'L1_ALFA_A7L1L', 'L1_ALFA_A7R1U', 'L1_ALFA_A7R1L', 'L1_ALFA_B7R1U', 'L1_ALFA_B7R1L', 'L1_ALFA_B1_EMPTY', 'L1_ALFA_B2_EMPTY', 'L1_ALFA_B7L1U_OD', 'L1_ALFA_B7L1L_OD', 'L1_ALFA_A7L1U_OD', 'L1_ALFA_A7L1L_OD', 'L1_ALFA_A7R1U_OD', 'L1_ALFA_A7R1L_OD', 'L1_ALFA_B7R1U_OD', 'L1_ALFA_B7R1L_OD', 'L1_ALFA_B7L1_OD', 'L1_ALFA_A7L1_OD', 'L1_ALFA_A7R1_OD', 'L1_ALFA_B7R1_OD',]
    rules.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(PS=-1, comment='disabled'),
                        }])))

# -- enable needed ones
    L1_list=['L1_ALFA_ELAST15', 'L1_ALFA_ELAST15_Calib', 'L1_ALFA_ELAST18', 'L1_ALFA_ELAST18_Calib','L1_ALFA_SYST11', 'L1_ALFA_SYST12', 'L1_ALFA_SYST17', 'L1_ALFA_SYST18', 'L1_ALFA_ANY', 'L1_ALFA_ANY_EMPTY', 'L1_ALFA_ANY_FIRSTEMPTY', 'L1_ALFA_ANY_UNPAIRED_ISO', 'L1_ALFA_ANY_UNPAIRED_NONISO', 'L1_ALFA_ANY_ABORTGAPNOTCALIB', 'L1_ALFA_B7L1_OD', 'L1_ALFA_A7L1_OD', 'L1_ALFA_A7R1_OD', 'L1_ALFA_B7R1_OD',] 
    rules.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(PS=1, comment=''),
                        }])))
    L1_list=['L1_BGRP1_ALFA_BGT']
    rules.update(dict(map(None,L1_list,len(L1_list)*[{
                        1000 : TriggerRule(PS=1000, comment=''),
                        }])))

# -- minbias
    HLT_list=['HLT_noalg_L1ALFA_Phys', 'HLT_noalg_L1ALFA_SYS']
    rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=1, comment=''), 
                        }])))
    HLT_list=['HLT_noalg_L1ALFA_PhysAny']
    rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=100, comment=''), 
                        }])))

# -- alfa calib
    HLT_list=['HLT_alfacalib','HLT_alfacalib_L1ALFA_ELAS','HLT_alfacalib_L1ALFA_SYS','HLT_alfacalib_L1ALFA_ANY']
    rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        1000 : TriggerRule(PS=1, comment=''), 
                        }])))
    
