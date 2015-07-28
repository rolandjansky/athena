# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'BeamLossMap'

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
    
