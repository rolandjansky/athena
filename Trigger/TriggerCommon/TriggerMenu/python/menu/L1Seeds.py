# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerMenu.menu.L1Seeds")


#######################################
# trigger type definitions
######################################
from TriggerMenu.l1.TriggerTypeDef import TT

cl_type     = TT.calo      | TT.phys
mb_type     = TT.minb      | TT.phys
tgc_type    = TT.tgc    | TT.phys  #obsolete!!
rpcout_type = TT.rpcout | TT.phys
rpcin_type  = TT.rpcin  | TT.phys             

calo_exceptions = set([])


#######################################################
# obtain the l1 items according to the the trigger type
# function taken originally from TriggerPythonConfig
#######################################################
def Lvl1ItemByTriggerType(l1object, triggertypebit, triggertypebitmask):
    """For a triggertypebit between 0 and 7, returns a list of names of
    those Lvl1 items that have that bit set in the triggertype"""
    if triggertypebit<0 or triggertypebit>0xFF:
        raise RuntimeError('TriggerPythonConfig.Lvl1ItemByTriggerType(triggertypebit,triggertypebitmask) needs to be called with 0<=triggertypebit<=0xFF, ' + \
                               + 'but is called with triggertypebit=%i' % triggertypebit)
    if triggertypebitmask<0 or triggertypebitmask>0xFF:
        raise RuntimeError('TriggerPythonConfig.Lvl1ItemByTriggerType(triggertypebit,triggertypebitmask) needs to be called with 0<=triggertypebitmask<=0xFF, ' + \
                               + 'but is called with triggertypebitmask=%i' % triggertypebitmask)
    
    itemsForMenu = [item for item in l1object if item.ctpid != -1]    
    if not itemsForMenu:
        log.error( 'No item defined for the L1 Menu, the TriggerConfL1 object does not contain items')
    res = [item.name for item in itemsForMenu if (triggertypebitmask & item.trigger_type)==triggertypebit ]
    return res

##############################
# define the various seeds
##############################
def getL1BackgroundSeed(menul1items):        
    l1background_seeds = 'L1_BCM_AC_CA_BGRP0,L1_BCM_AC_CA_UNPAIRED_ISO,L1_BCM_Wide_EMPTY,L1_BCM_Wide_UNPAIRED_ISO,L1_BCM_Wide_UNPAIRED_NONISO,L1_EM3_UNPAIRED_ISO,L1_FJ30_UNPAIRED_ISO,L1_J10_UNPAIRED_ISO,L1_J10_UNPAIRED_NONISO,L1_LUCID_A_C_EMPTY,L1_LUCID_A_C_UNPAIRED_ISO,L1_LUCID_A_C_UNPAIRED_NONISO,L1_LUCID_EMPTY,L1_LUCID_UNPAIRED_ISO,L1_MU4_UNPAIRED_ISO,L1_LUCID_COMM_UNPAIRED_ISO,L1_LUCID_COMM_EMPTY'

    # check if all the l1 background seeds given are in the current L1 menu
    l1bgditems = l1background_seeds.split(',')
    for item in l1bgditems:
        if item not in menul1items:
            log.error('L1 item %s from background seeds is not in current L1 menu' % item)
    
    return l1background_seeds



##############################
def getL1StandbySeed(l1items):        
    standby_seeds    = ",".join([ x for x in l1items if "_EMPTY" not in x and "CALREQ" not in x and "ZB" not in x])
    return standby_seeds


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
def getL1MinBiasSeed(l1seed, l1object):
    if ('EMPTY' in l1seed):
        l1minbias_seeds = ','.join([ x for x in Lvl1ItemByTriggerType(l1object, mb_type, mb_type)])
    else:
        l1minbias_seeds = ','.join([ x for x in Lvl1ItemByTriggerType(l1object, mb_type, mb_type)])
    return l1minbias_seeds


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


#####################################
# assigned the seeds to the L1 names
#####################################
def getSpecificL1Seeds(l1seedname, l1itemobject):
    l1items = [i.name for i in l1itemobject]
    l1ctpid = [i.ctpid for i in l1itemobject]
    L1Seed = ''
    if (l1seedname == 'L1_Bkg'):
        L1Seed = getL1BackgroundSeed(l1items)

    elif (l1seedname == 'L1_Standby'):
        L1Seed = getL1StandbySeed(l1items)

    elif (l1seedname in ['L1_Calo', 'L1_Calo_EMPTY']):
        L1Seed = getL1CaloSeed(l1seedname, l1itemobject)

    elif (l1seedname in ['L1_Muon', 'L1_Muon_EMPTY']):
        L1Seed = getL1MuonSeed(l1seedname, l1itemobject)

    elif (l1seedname in ['L1_MinBias', 'L1_MinBias_EMPTY']):
        L1Seed = getL1MinBiasSeed(l1seedname, l1itemobject)

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
        
