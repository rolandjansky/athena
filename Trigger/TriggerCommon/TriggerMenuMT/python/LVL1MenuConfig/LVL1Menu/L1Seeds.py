# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerMenuMT.LVL1MenuConfig.L1Seeds")

from TriggerJobOpts.TriggerFlags  import TriggerFlags

#######################################
# trigger type definitions
######################################
from TriggerMenuMT.LVL1MenuConfig.LVL1.Lvl1Flags import Lvl1Flags
run1 = Lvl1Flags.CTPVersion()<=3

if run1:
    from TriggerMenu.l1.TriggerTypeDefRun1 import TT
    rpcout_type = TT.rpcout | TT.phys
    rpcin_type  = TT.rpcin  | TT.phys             
else:
    from TriggerMenuMT.LVL1MenuConfig.LVL1.TriggerTypeDef import TT
    rpcout_type = TT.muon   | TT.phys
    rpcin_type  = TT.muon   | TT.phys             


cl_type     = TT.calo      | TT.phys
mb_type     = TT.minb      | TT.phys


calo_exceptions = set([])


#######################################################
# obtain the l1 items according to the the trigger type
# function taken originally from TriggerPythonConfig
#######################################################
def Lvl1ItemByTriggerType(l1object, triggertype_pattern, triggertype_bitmask):
    """
    The function returns those items where the items triggertype, after applying the mask, matches the pattern.
    With this one can also select items where a certain bit is disabled
    """
    if triggertype_pattern<0 or triggertype_pattern>0xFF:
        raise RuntimeError('TriggerPythonConfig.Lvl1ItemByTriggerType(triggertype_pattern,triggertype_bitmask) needs to be called with 0<=triggertype_pattern<=0xFF, ' + \
                               + 'but is called with triggertype_pattern=%i' % triggertype_pattern)
    if triggertype_bitmask<0 or triggertype_bitmask>0xFF:
        raise RuntimeError('TriggerPythonConfig.Lvl1ItemByTriggerType(triggertype_pattern,triggertype_bitmask) needs to be called with 0<=triggertype_bitmask<=0xFF, ' + \
                               + 'but is called with triggertype_bitmask=%i' % triggertype_bitmask)
    itemsForMenu = [item for item in l1object if l1object[item]['ctpid'] != -1]
    if not itemsForMenu:
        log.error('No item defined for the L1 Menu, the TriggerConfL1 object does not contain items')
    res = [item for item in itemsForMenu if ( (triggertype_bitmask & int(l1object[item]['triggerType'],2)) == triggertype_pattern)]
    return res

##############################
# define the various seeds
##############################
def getL1BackgroundSeed(menul1items):        
    l1background_seeds = 'L1_BCM_AC_CA_BGRP0,L1_BCM_Wide_EMPTY,L1_BCM_Wide_UNPAIRED_ISO,L1_BCM_Wide_UNPAIRED_NONISO,L1_J30.31ETA49_UNPAIRED_ISO,L1_J12_UNPAIRED_ISO,L1_J12_UNPAIRED_NONISO,L1_J12_ABORTGAPNOTCALIB,L1_BCM_AC_UNPAIRED_ISO,L1_BCM_CA_UNPAIRED_ISO,L1_BCM_AC_UNPAIRED_NONISO,L1_BCM_CA_UNPAIRED_NONISO,L1_J30.31ETA49_UNPAIRED_NONISO,L1_BCM_AC_ABORTGAPNOTCALIB,L1_BCM_CA_ABORTGAPNOTCALIB,L1_BCM_Wide_ABORTGAPNOTCALIB,L1_BCM_AC_CALIB,L1_BCM_CA_CALIB,L1_BCM_Wide_CALIB,L1_J50_UNPAIRED_ISO,L1_J50_UNPAIRED_NONISO,L1_J50_ABORTGAPNOTCALIB'
    
    if TriggerFlags.triggerMenuSetup() == 'LS1_v1':
        l1background_seeds = 'L1_BCM_AC_CA_BGRP0,L1_BCM_AC_CA_UNPAIRED_ISO,L1_BCM_Wide_EMPTY,L1_BCM_Wide_UNPAIRED_ISO,L1_BCM_Wide_UNPAIRED_NONISO,L1_EM3_UNPAIRED_ISO,L1_FJ30_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO,L1_J10_UNPAIRED_NONISO,L1_LUCID_A_C_EMPTY,L1_LUCID_A_C_UNPAIRED_ISO,L1_LUCID_A_C_UNPAIRED_NONISO,L1_LUCID_EMPTY,L1_LUCID_UNPAIRED_ISO,L1_MU4_UNPAIRED_ISO,L1_LUCID_COMM_UNPAIRED_ISO,L1_LUCID_COMM_EMPTY'
        
    # check if all the l1 background seeds given are in the current L1 menu
    l1bgditems = l1background_seeds.split(',')
    for item in l1bgditems:
        if item not in menul1items:
            log.error('L1 item %s from background seeds is not in current L1 menu', item)
            
    return l1background_seeds

##############################
def getL1_ALFA_Diff_Phys_Seeds(menul1items):        
    # comma separated list fo l1 seeds
    l1_seeds = 'L1_ALFA_SDIFF5,L1_ALFA_SDIFF6,L1_ALFA_SDIFF7,L1_ALFA_SDIFF8,L1_MBTS_1_A_ALFA_C,L1_MBTS_1_C_ALFA_A,L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO,L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO,L1_MBTS_2_A_ALFA_C,L1_MBTS_2_C_ALFA_A,L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO,L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO,L1_LUCID_A_ALFA_C,L1_LUCID_C_ALFA_A,L1_LUCID_A_ALFA_C_UNPAIRED_ISO,L1_LUCID_C_ALFA_A_UNPAIRED_ISO,L1_EM3_ALFA_ANY,L1_EM3_ALFA_ANY_UNPAIRED_ISO,L1_TE5_ALFA_ANY,L1_TE5_ALFA_ANY_UNPAIRED_ISO'
     
    # check if all the l1 background seeds given are in the current L1 menu
    l1items = l1_seeds.split(',')
    for item in l1items:
        if item not in menul1items:
            log.error('L1 item %s from background seeds is not in current L1 menu', item)
            
    return l1_seeds

##############################
def getL1_ALFA_CDiff_Phys_Seeds (menul1items):        
    # comma separated list fo l1 seeds
    l1_seeds = 'L1_EM3_ALFA_EINE,L1_TE5_ALFA_EINE'
     
    # check if all the l1 background seeds given are in the current L1 menu
    l1items = l1_seeds.split(',')
    for item in l1items:
        if item not in menul1items:
            log.error('L1 item %s from background seeds is not in current L1 menu', item)
            
    return l1_seeds
##############################
def getL1_ALFA_Jet_Phys_Seeds (menul1items):        
    # comma separated list fo l1 seeds
    l1_seeds = 'L1_J12_ALFA_ANY,L1_J12_ALFA_ANY_UNPAIRED_ISO'
     
    # check if all the l1 background seeds given are in the current L1 menu
    l1items = l1_seeds.split(',')
    for item in l1items:
        if item not in menul1items:
            log.error('L1 item %s from background seeds is not in current L1 menu', item)
            
    return l1_seeds


##############################
def getL1StandbySeed(l1items):        
    standby_seeds    = ",".join([ x for x in l1items if "_EMPTY" not in x and "CALREQ" not in x and "ZB" not in x and "-" not in x and "CMU" not in x and "ALFA" not in x  and "RD" not in x and "BCM" not in x and "BGRP12" not in x])
    return standby_seeds
##############################
def getL1TopoSeed(l1items):
    l1topo_seeds = ",".join([ x for x in l1items if "-" in x or "CMU" in x ])
    return l1topo_seeds

##############################
def getL1CaloSeed(l1seed, l1object):
    if ('EMPTY' in l1seed):
        #l1calo_seeds = ','.join([ x for x in Lvl1ItemByTriggerType(l1object, cl_type, cl_type) \
        #                              if (x not in calo_exceptions)  ])
        l1calo_seeds = ','.join([ x for x in Lvl1ItemByTriggerType(l1object, cl_type, cl_type) \
                                      if ('EMPTY' in x and 'FIRSTEMPTY' not in x)  ])
    else:
        l1calo_seeds = ','.join([ x for x in Lvl1ItemByTriggerType(l1object, cl_type, cl_type) if ("EMPTY" not in x or "FIRSTEMPTY" in x) \
                                      and (x not in calo_exceptions)  ])
    return l1calo_seeds



##############################
def getL1TauSeed(l1items):

    tau_seeds    = ",".join([ x for x in l1items if "TAU" in x and "-" not in x and  "EMPTY" not in x  and "ISO" not in x])
    return tau_seeds

##############################
def getL1BSSeed(menul1items):
    l1_seeds = 'L1_J15,L1_3J15,L1_3J10,L1_4J10'
    # check if all the l1 background seeds given are in the current L1 menu
    l1items = l1_seeds.split(',')
    for item in l1items:
        if item not in menul1items:
            log.error('L1 item %s from beamspot seeds is not in current L1 menu', item)
            
    return l1_seeds


##############################
def getL1MinBiasSeed(l1seed, l1object):
    if ('EMPTY' in l1seed):
        l1minbias_seeds = ','.join([ x for x in Lvl1ItemByTriggerType(l1object, mb_type, mb_type)])
    else:
        l1minbias_seeds = ','.join([ x for x in Lvl1ItemByTriggerType(l1object, mb_type, mb_type)])
    return l1minbias_seeds

def getL1JetBS():
    return 'L1_J15,L1_3J15,L1_3J10,L1_4J10'

##############################
def getL1MuonSeed(l1seed, l1object):
    if ('EMPTY' in l1seed): # to only get MU*_EMPTY items
        muon_seeds_list = [ x for x in Lvl1ItemByTriggerType(l1object, rpcout_type, rpcout_type) if ('MU' in x and '_EMPTY' in x )  ]
        muon_seeds_list = list(set(muon_seeds_list))
        muon_seeds      = ','.join(muon_seeds_list)
    else: #this one does NOT include the EMPTY items
        muon_seeds_list = [ x for x in Lvl1ItemByTriggerType(l1object, rpcout_type, rpcout_type) if ('MU' in x and 'FIRSTEMPTY' in x )  ]
        muon_seeds_list += [ x for x in Lvl1ItemByTriggerType(l1object, rpcin_type, rpcin_type)  ]
        muon_seeds_list = list(set(muon_seeds_list))
        muon_seeds      = ','.join(muon_seeds_list)

    return muon_seeds

##############################
def getEBnoL1PSSeed(l1items, l1seedname):

    # All of these L1 items must be PS=1 for an EB campaign
    noL1PS_seeds = ''
    if ('L1_PhysicsLow' in l1seedname):
      noL1PS_seeds = 'L1_MU6_3MU4,L1_EM15HI_2TAU12IM_J25_3J12,L1_EM15HI_2TAU12IM_XE35,L1_EM15HI_TAU40_2TAU15,L1_EM15VH_MU10,L1_EM15VH_3EM7,L1_EM22VHI,L1_2EM8VH_MU10,L1_MU10_TAU12IM_J25_2J12,L1_MU10_TAU12IM_XE35,L1_MU10_TAU20IM,L1_4J15,L1_XE50,L1_2J15_XE55,L1_TAU60,L1_TAU20IM_2TAU12IM_J25_2J20_3J12,L1_TAU20IM_2TAU12IM_XE35,L1_TAU20IM_2J20_XE45,L1_MU20,L1_MU6_J30.0ETA49_2J20.0ETA49,L1_MU10_3J20,L1_J40.0ETA25_2J15.31ETA49,L1_3MU6,L1_MU10_2J15_J20,L1_J40.0ETA25_2J25_J20.31ETA49,L1_2MU6_3MU4,L1_MU11_2MU6'
    elif ('L1_PhysicsHigh' in l1seedname):
      noL1PS_seeds = 'L1_XE80,L1_J100.31ETA49,L1_J400,L1_6J15'
    elif ('L1_EMPTY' in l1seedname):
      noL1PS_seeds = 'L1_J12_EMPTY,L1_MU11_EMPTY,L1_TAU8_EMPTY,L1_TAU30_EMPTY,L1_EM7_EMPTY'
    elif ('L1_FIRSTEMPTY' in l1seedname):
      noL1PS_seeds = 'L1_J12_FIRSTEMPTY,L1_MU20_FIRSTEMPTY,L1_TAU8_FIRSTEMPTY,L1_EM7_FIRSTEMPTY'
    elif ('L1_UNPAIRED_ISO' in l1seedname):
      noL1PS_seeds = 'L1_J12_UNPAIRED_ISO,L1_J15.31ETA49_UNPAIRED_ISO,L1_BCM_Wide_UNPAIRED_ISO,L1_BCM_AC_UNPAIRED_ISO,L1_BCM_CA_UNPAIRED_ISO,L1_MU4_UNPAIRED_ISO,L1_EM7_UNPAIRED_ISO,L1_TAU8_UNPAIRED_ISO,L1_TAU30_UNPAIRED_ISO'
    elif ('L1_UNPAIRED_NONISO' in l1seedname):
      noL1PS_seeds = 'L1_J12_UNPAIRED_NONISO,L1_BCM_Wide_UNPAIRED_NONISO,L1_BCM_AC_UNPAIRED_NONISO,L1_BCM_CA_UNPAIRED_NONISO'
    elif ('L1_ABORTGAPNOTCALIB' in l1seedname): 
      noL1PS_seeds = 'L1_J12_ABORTGAPNOTCALIB'
    else:
      log.error('Do not know how to supply EnhancedBias L1 seeds for %s', l1seedname)

    # check if all the l1 seeds given are in the current L1 menu
    l1EBitems = noL1PS_seeds.split(',')
    for item in l1EBitems:
        if item not in l1items:
            log.error('L1 item %s from %s seed is not in current L1 menu (EnhancedBias)', item, l1seedname)
            
    return noL1PS_seeds

##############################
def getL1_ALFA_Phys(l1seed):

    L1ALFA_Phys_seeds = 'L1_ALFA_ELAST1,L1_ALFA_ELAST2,L1_ALFA_ELAST11,L1_ALFA_ELAST12,L1_ALFA_ELAST13,L1_ALFA_ELAST14,L1_ALFA_ELAST15,L1_ALFA_ELAST16,L1_ALFA_ELAST17,L1_ALFA_ELAST18,L1_ALFA_SHOWSYST5,L1_ALFA_ANY_A_EMPTY,L1_ALFA_ANY_C_EMPTY'

    # check if all the l1 background seeds given are in the current L1 menu
    l1bgditems = L1ALFA_Phys_seeds.split(',')
    for item in l1bgditems:
        if item not in l1seed:
            log.error('L1 item %s from L1ALFA_Phys seeds is not in current L1 menu', item)
            
    return L1ALFA_Phys_seeds

##############################
def getL1_ALFA_Phys_Any(l1seed):

    L1ALFA_Phys_Any_seeds = 'L1_ALFA_ANY,L1_ALFA_ANY_EMPTY,L1_ALFA_ANY_FIRSTEMPTY,L1_ALFA_ANY_UNPAIRED_ISO,L1_ALFA_ANY_UNPAIRED_NONISO,L1_ALFA_ANY_BGRP10'

    # check if all the l1 background seeds given are in the current L1 menu
    l1bgditems = L1ALFA_Phys_Any_seeds.split(',')
    for item in l1bgditems:
        if item not in l1seed:
            log.error('L1 item %s from L1ALFA_Phys_Any_seeds seeds is not in current L1 menu', item)
            
    return L1ALFA_Phys_Any_seeds

##############################
def getL1ALFA_Calib(l1seed):

    L1ALFA_Calib_seeds = 'L1_ALFA_B7L1U,L1_ALFA_B7L1L,L1_ALFA_A7L1U,L1_ALFA_A7L1L,L1_ALFA_A7R1U,L1_ALFA_A7R1L,L1_ALFA_B7R1U,L1_ALFA_B7R1L'
        #ATR-13743 'L1_ALFA_ELAST15_Calib,L1_ALFA_ELAST18_Calib,L1_ALFA_BGT,L1_ALFA_BGT_UNPAIRED_ISO,L1_ALFA_BGT_BGRP10,L1_ALFA_SHOWSYST5,L1_ALFA_ANY_EMPTY,L1_ALFA_ANY_FIRSTEMPTY,L1_ALFA_ANY_UNPAIRED_ISO,L1_ALFA_ANY_UNPAIRED_NONISO,L1_ALFA_ANY_BGRP10,L1_ALFA_ANY_CALIB,L1_ALFA_ANY_ABORTGAPNOTCALIB,
           #L1_ALFA_B7L1U_OD,L1_ALFA_B7L1L_OD,L1_ALFA_A7L1U_OD,L1_ALFA_A7L1L_OD,L1_ALFA_A7R1U_OD,L1_ALFA_A7R1L_OD,L1_ALFA_B7R1U_OD,L1_ALFA_B7R1L_OD,L1_ALFA_B7L1_OD,L1_ALFA_A7L1_OD,L1_ALFA_A7R1_OD,L1_ALFA_B7R1_OD,L1_ALFA_ANY_A_EMPTY,L1_ALFA_ANY_C_EMPTY'

    # check if all the l1 background seeds given are in the current L1 menu
    l1bgditems = L1ALFA_Calib_seeds.split(',')
    for item in l1bgditems:
        if item not in l1seed:
            log.error('L1 item %s from L1ALFA_Calib_seeds seeds is not in current L1 menu', item)
            
    return L1ALFA_Calib_seeds


##############################
def getL1ALFA_CEP(l1seed):

    L1ALFA_CEP_seeds = 'L1_ALFA_ELAST15,L1_ALFA_ELAST18,L1_ALFA_SYST17,L1_ALFA_SYST18,L1_ALFA_ELASTIC_UNPAIRED_ISO,L1_ALFA_ANTI_ELASTIC_UNPAIRED_ISO'

    # check if all the l1 background seeds given are in the current L1 menu
    l1bgditems = L1ALFA_CEP_seeds.split(',')
    for item in l1bgditems:
        if item not in l1seed:
            log.error('L1 item %s from L1ALFA_CEP_seeds seeds is not in current L1 menu', item)
            
    return L1ALFA_CEP_seeds



##############################
def getL1ALFA_SYS(l1seed):

    L1ALFA_SYS_seeds = 'L1_ALFA_SYST9,L1_ALFA_SYST10,L1_ALFA_SYST11,L1_ALFA_SYST12,L1_ALFA_SYST17,L1_ALFA_SYST18'

    # check if all the l1 background seeds given are in the current L1 menu
    l1bgditems = L1ALFA_SYS_seeds.split(',')
    for item in l1bgditems:
        if item not in l1seed:
            log.error('L1 item %s from L1ALFA_SYS_seeds seeds is not in current L1 menu', item)
            
    return L1ALFA_SYS_seeds

##############################
def getL1ALFA_ELAS(l1seed):

    L1ALFA_ELAS_seeds = 'L1_ALFA_ELAST1,L1_ALFA_ELAST2,L1_ALFA_ELAST11,L1_ALFA_ELAST12,L1_ALFA_ELAST13,L1_ALFA_ELAST14,L1_ALFA_ELAST15,L1_ALFA_ELAST16,L1_ALFA_ELAST17,L1_ALFA_ELAST18'

    # check if all the l1 background seeds given are in the current L1 menu
    l1bgditems = L1ALFA_ELAS_seeds.split(',')
    for item in l1bgditems:
        if item not in l1seed:
            log.error('L1 item %s from L1ALFA_ELAS_seeds seeds is not in current L1 menu', item)
            
    return L1ALFA_ELAS_seeds



#####################################
def getL1LowLumi(l1seed):

    L1LowLumi_seeds = 'L1_EM20VH,L1_2EM10VH,L1_MU15,L1_2MU6,L1_3MU4,L1_EM15VH_MU10,L1_EM15I_MU4,L1_EM7_MU10,L1_2EM8VH_MU10,L1_TAU60,L1_TAU20IM_2TAU12IM_J25_2J20_3J12,L1_EM15HI_2TAU12IM_XE35,L1_MU10_TAU12IM_XE35,L1_TAU20_2TAU12_XE35,L1_TAU20IM_2TAU12IM_XE35,L1_EM15HI_2TAU12IM,L1_EM15HI_2TAU12IM_J25_3J12,L1_EM15HI_TAU40_2TAU15,L1_MU10_TAU12IM_J25_2J12,L1_MU10_TAU12IM,L1_J75,L1_4J15,L1_XE50,L1_3J25.0ETA23,L1_3J40,L1_2J15_XE55,L1_MU6_J40,L1_J75.31ETA49'


    # check if all the l1 background seeds given are in the current L1 menu
    l1bgditems = L1LowLumi_seeds.split(',')
    for item in l1bgditems:
        if item not in l1seed:
            log.error('L1 item %s from L1LowLumi_seeds seeds is not in current L1 menu', item)
            
    return L1LowLumi_seeds
        
#####################################
# assigned the seeds to the L1 names
#####################################
def getSpecificL1Seeds(l1seedname, l1itemobject):
    l1items = l1itemobject.keys()
    L1Seed = ''
    if l1seedname == 'L1_J':
        L1Seed = getL1JetBS()
    if (l1seedname == 'L1_Bkg'):
        L1Seed = getL1BackgroundSeed(l1items)
#    elif (l1seedname == 'L1_ALFA_Diff_Phys' ):
#        L1Seed = getL1_ALFA_Diff_Phys_Seeds(l1items)
#    elif (l1seedname == 'L1_ALFA_CDiff_Phys' ):
#        L1Seed = getL1_ALFA_CDiff_Phys_Seeds(l1items)
#    elif (l1seedname == 'L1_ALFA_Jet_Phys' ):
#        L1Seed = getL1_ALFA_Jet_Phys_Seeds(l1items)

    elif (l1seedname in ['L1_PhysicsLow_noPS', 'L1_PhysicsHigh_noPS', 'L1_EMPTY_noPS', 'L1_FIRSTEMPTY_noPS', 'L1_UNPAIRED_ISO_noPS', 'L1_UNPAIRED_NONISO_noPS', 'L1_ABORTGAPNOTCALIB_noPS']):
        L1Seed =  getEBnoL1PSSeed(l1items, l1seedname)
        
    elif (l1seedname == 'L1_Standby'):
        L1Seed = getL1StandbySeed(l1items)

    elif (l1seedname == 'L1_Topo'):
        L1Seed = getL1TopoSeed(l1items)

    elif (l1seedname in ['L1_Calo', 'L1_Calo_EMPTY']):
        L1Seed = getL1CaloSeed(l1seedname, l1itemobject)

    elif (l1seedname in ['L1_Muon', 'L1_Muon_EMPTY']):
        L1Seed = getL1MuonSeed(l1seedname, l1itemobject)

    elif (l1seedname == 'L1_TAU'):
        L1Seed = getL1TauSeed(l1items)

    elif (l1seedname == 'L1_BS'):
        L1Seed = getL1BSSeed(l1items)
        
    elif (l1seedname in ['L1_MinBias', 'L1_MinBias_EMPTY']):
        L1Seed = getL1MinBiasSeed(l1seedname, l1itemobject)

#    elif (l1seedname == 'L1_ALFA_Phys'):
#        L1Seed = getL1_ALFA_Phys(l1items)
#
#    elif (l1seedname == 'L1_ALFA_PhysAny'):
#        L1Seed = getL1_ALFA_Phys_Any(l1items)
#
#    elif (l1seedname == 'L1_ALFA_Diff'):
#        L1Seed = getL1ALFA_Diff(l1items)
#
#    elif (l1seedname == 'L1_ALFA_TRT_Diff'):
#        L1Seed = getL1ALFA_TRT_Diff(l1items)
#        
    elif (l1seedname == 'L1_ALFA_Calib'):
        L1Seed = getL1ALFA_Calib(l1items)
#
#    elif (l1seedname == 'L1_ALFA_ELAS'):
#        L1Seed = getL1ALFA_ELAS(l1items)
#
#    elif (l1seedname == 'L1_ALFA_TRT_Calib'):
#        L1Seed = getL1ALFA_TRT_Calib(l1items)
#
#    elif (l1seedname == 'L1_ALFA_Jet'):
#        L1Seed = getL1ALFA_Jet(l1items)
#        
#    elif (l1seedname == 'L1_ALFA_SDiff'):
#        L1Seed = getL1ALFA_SDiff(l1items)
#
#    elif (l1seedname == 'L1_ALFA_CEP'):
#        L1Seed = getL1ALFA_CEP(l1items)
#
#    elif (l1seedname == 'L1_ALFA_TRT'):
#        L1Seed = getL1ALFA_TRT(l1items)
#
#    elif (l1seedname == 'L1_ALFA_SYS'):
#        L1Seed = getL1ALFA_SYS(l1items)


    elif (l1seedname == 'L1_LowLumi'):
        L1Seed = getL1LowLumi(l1items)
        
    elif (l1seedname == 'L1_All'):
        return ''

    elif (l1seedname == ''):
        log.warning('No L1item name given!')

    else: 
        #log.error('Given seed %s could not be found!' %l1seedname)
        return 'ERROR_'+l1seedname

    L1Seed = L1Seed.split(",")
    L1Seed.sort()
    L1Seed = ",".join(L1Seed)

    return L1Seed

#####################################
# map from l1item name to inputTE
#####################################
def getInputTEfromL1Item(l1item):
    
    L1Map = {
        'L1_TAU8_EMPTY':      ['HA8'],
        'L1_TAU8_FIRSTEMPTY': ['HA8'],
        'L1_TAU8_UNPAIRED_ISO': ['HA8'],
        'L1_TAU8_UNPAIRED_NONISO': ['HA8'],
        'L1_TAU12_EMPTY':      ['HA12'],
        'L1_TAU12_FIRSTEMPTY': ['HA12'],
        'L1_TAU12_UNPAIRED_ISO': ['HA12'],
        'L1_RD0_FIRSTEMPTY':  [''],
        'L1_TAU30'         :  ['HA30'],
        'L1_TAU30_EMPTY':      ['HA30'],
        'L1_TAU30_UNPAIRED_ISO': ['HA30'],
        'L1_TAU40'         :  ['HA40'],
        'L1_TAU60'         :  ['HA60'],
        'L1_TAU100'         :  ['HA100'],
        }

    if TriggerFlags.triggerMenuSetup() == 'LS1_v1': 
        L1Map['L1_CALREQ2']=['NIM30']
    else:
        L1Map['L1_CALREQ2']=['CAL2']
        
    if l1item in L1Map:
        TE = L1Map[l1item]
        log.debug('Mapped L1 input TE from %s to %s.', l1item, TE)
        return TE
    else:
        TE = l1item.replace("L1_","").split("_")[0]
        TE = TE[1:] if TE[0].isdigit() else TE
        return TE
    
