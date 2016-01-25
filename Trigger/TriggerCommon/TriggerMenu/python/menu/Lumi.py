# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags  import TriggerFlags
from TriggerMenu.menu.MenuUtil    import *
import re

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.menu.Lumi' )


def lumi(triggerPythonConfig):
    no_prescale = False
    no_Bphysics = False
    if 'no_prescale' in TriggerFlags.triggerMenuSetup(): no_prescale = True
    if 'no_Bphysics' in TriggerFlags.triggerMenuSetup(): no_Bphysics = True

    L1Prescales = {}
    HLTPrescales = {}
    
    menu_name = TriggerFlags.triggerMenuSetup()
    ## Do some aliasing here
    if menu_name == 'Physics_default': menu_name = 'Physics_pp_v5'
    elif menu_name == 'MC_loose_default': menu_name = 'MC_pp_v5_loose_mc_prescale'
    elif menu_name == 'MC_tight_default': menu_name = 'MC_pp_v5_tight_mc_prescale'
    elif menu_name == 'default_loose': menu_name = 'MC_pp_v5_loose_mc_prescale'
    elif menu_name == 'default_tight': menu_name = 'MC_pp_v5_tight_mc_prescale'

    log.info( 'Menu name: '+ menu_name)

    # if menu_name.startswith('Physics_pp_v4') :
    #     log.info('in physics pp v4 menu setup')
    #     from TriggerMenu.menu.Physics_pp_v4 import setupMenu, Prescales#, StreamConfig
    #     setupMenu()
    #     if 'cosmics_prescale' in menu_name:
    #         L1Prescales = Prescales.L1Prescales_cosmics
    #         HLTPrescales = Prescales.HLTPrescales_cosmics
    #     else:
    #         L1Prescales = Prescales.L1Prescales
    #         HLTPrescales = Prescales.HLTPrescales

    # elif  menu_name.startswith('MC_pp_v4'):
    #     from TriggerMenu.menu.MC_pp_v4 import setupMenu, Prescales#, StreamConfig
    #     setupMenu()
    #     if 'tight_mc_prescale' in menu_name:
    #         L1Prescales = Prescales.L1Prescales_tight_mc_prescale
    #         HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
    #     elif 'loose_mc_prescale' in menu_name:
    #         L1Prescales = Prescales.L1Prescales_loose_mc_prescale
    #         HLTPrescales = Prescales.HLTPrescales_loose_mc_prescale
    #         log.info(" HLTPrescales %s" % HLTPrescales)
    #     elif 'upgrade_mc_prescale' in menu_name:
    #         L1Prescales = Prescales.L1Prescales_upgrade_mc_prescale
    #         HLTPrescales = Prescales.HLTPrescales_upgrade_mc_prescale            
    #         log.info(" HLTPrescales %s" % HLTPrescales)
    #     else:
    #         L1Prescales = Prescales.L1Prescales
    #         HLTPrescales = Prescales.HLTPrescales

    if menu_name.startswith('Physics_pp_v5') :
        log.info('In physics pp v5 menu setup')
        from TriggerMenu.menu.Physics_pp_v5 import setupMenu, Prescales
        setupMenu()
        if 'cosmics_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_cosmics
            HLTPrescales = Prescales.HLTPrescales_cosmics
        elif 'tight' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_physics_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_physics_prescale
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales
    elif menu_name.startswith('Physics_pp_v6') :
        log.info('In physics pp v6 menu setup')
        from TriggerMenu.menu.Physics_pp_v6 import setupMenu, Prescales
        setupMenu()
        if 'cosmics_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_cosmics
            HLTPrescales = Prescales.HLTPrescales_cosmics
        elif 'tight' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_physics_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_physics_prescale
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales

    elif menu_name.startswith('DC14') :
        log.info('In DC14 menu setup')
        from TriggerMenu.menu.DC14 import setupMenu, Prescales
        setupMenu()
        if 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        elif 'loose_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_loose_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_loose_mc_prescale
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales

    elif menu_name.startswith('LS1_v1') :
        log.info('In LS1_v1 menu setup')
        from TriggerMenu.menu.LS1_v1 import setupMenu, Prescales
        setupMenu()
        if 'cosmics_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_cosmics
            HLTPrescales = Prescales.HLTPrescales_cosmics
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales

    elif  menu_name.startswith('MC_pp_v5'):
        from TriggerMenu.menu.MC_pp_v5 import setupMenu, Prescales
        setupMenu()
        if 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        elif 'loose_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_loose_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_loose_mc_prescale
            log.info(" HLTPrescales %s" % HLTPrescales)
        elif 'special_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_special_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_special_mc_prescale
            log.info(" HLTPrescales %s" % HLTPrescales)
        elif 'no_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_no_prescale
            HLTPrescales = Prescales.HLTPrescales_no_prescale
            log.info(" HLTPrescales %s" % HLTPrescales)
        elif 'upgrade_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_upgrade_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_upgrade_mc_prescale            
            log.info(" HLTPrescales %s" %  HLTPrescales)
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales
    elif  menu_name.startswith('MC_pp_v6'):
        from TriggerMenu.menu.MC_pp_v6 import setupMenu, Prescales
        setupMenu()
        if 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        elif 'tightperf_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tightperf_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tightperf_mc_prescale
            log.info(" HLTPrescales %s" % HLTPrescales)
        elif 'loose_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_loose_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_loose_mc_prescale
            log.info(" HLTPrescales %s" % HLTPrescales)
        elif 'special_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_special_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_special_mc_prescale
            log.info(" HLTPrescales %s" % HLTPrescales)
        elif 'no_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_no_prescale
            HLTPrescales = Prescales.HLTPrescales_no_prescale
            log.info(" HLTPrescales %s" % HLTPrescales)
        elif 'upgrade_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_upgrade_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_upgrade_mc_prescale            
            log.info(" HLTPrescales %s" %  HLTPrescales)
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales

    elif menu_name.startswith('Physics_HI_v3') :
        log.info('Physics_HI_v3 menu setup')
        from TriggerMenu.menu.Physics_HI_v3 import setupMenu, Prescales
        setupMenu()
        if 'cosmics_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_cosmics
            HLTPrescales = Prescales.HLTPrescales_cosmics
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales
            

    elif menu_name.startswith('MC_HI_v3') :
        log.info('MC_HI_v3 menu setup')
        from TriggerMenu.menu.MC_HI_v3 import setupMenu, Prescales
        setupMenu()
        if 'cosmics_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_cosmics
            HLTPrescales = Prescales.HLTPrescales_cosmics
        elif 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales
            
    else:
        log.fatal ('Menu with name %s is not known in this version of TriggerMenu! ', menu_name)
        return

    return (L1Prescales, HLTPrescales)




def applyPrescales(triggerPythonConfig, HLTPrescales):
    no_prescale = False
    no_Bphysics = False
    mc_loose_tight_prescale = False
    if 'no_prescale' in TriggerFlags.triggerMenuSetup() or \
       TriggerFlags.triggerMenuSetup() == 'default': no_prescale = True
    #now also the loose and tight prescales in mc
    if 'loose_mc_prescale' in TriggerFlags.triggerMenuSetup() or \
       'tight_mc_prescale' in TriggerFlags.triggerMenuSetup() or \
       'tightperf_mc_prescale' in TriggerFlags.triggerMenuSetup() or \
       'special_mc_prescale' in TriggerFlags.triggerMenuSetup(): mc_loose_tight_prescale = True
        
    if 'no_Bphysics' in TriggerFlags.triggerMenuSetup(): no_Bphysics = True

    #if L1Prescales==None and TriggerFlags.L1PrescaleSet()!='':
    #    log.warning('Cannot find L1 prescale set %s, not explicitly setting them' % \
    #                TriggerFlags.L1PrescaleSet())
    #    L1Prescales = {}
    if HLTPrescales==None and TriggerFlags.HLTPrescaleSet()!='':
        log.warning('Cannot find HLT prescale set %s, not explicitly setting them' % \
                    TriggerFlags.HLTPrescaleSet())
        HLTPrescales = {}
        
    #-----------------------------------------------------------------------
    hlt_sigs = allSignatures()
    
    if TriggerFlags.doHLT():
        applyHLTPrescale(triggerPythonConfig, HLTPrescales)

    if TriggerFlags.L1PrescaleSet() == 'None':
        for item in triggerPythonConfig.allItems.values():
            item.prescale = '1' 
    if TriggerFlags.HLTPrescaleSet() == 'None':
        for  sig in triggerPythonConfig.allChains.values():
            for chain in sig:
                chain.prescale = '1'
                chain.pass_through = '0'
                tmp_stag = chain.stream_tag
                chain.stream_tag = []
                for s in tmp_stag:
                    chain.addStreamTag(s[0], s[1], s[2], 1)

    if no_prescale:
        resetAllPrescales(triggerPythonConfig)

    if TriggerFlags.disableRandomPrescale() or no_prescale or mc_loose_tight_prescale or \
           TriggerFlags.L1PrescaleSet() == 'None':
        triggerPythonConfig.CTPInfo().random.rate1 = -7
        triggerPythonConfig.CTPInfo().random.rate2 = 1


    if no_Bphysics:
        
        BphysSig = TriggerFlags.BphysicsSlice.signatures()
        log.info('Listing Bphysics chains %s ' % BphysSig)
        
        if BphysSig is not None:
            for name,val in triggerPythonConfig.allChains.iteritems():
                if name in BphysSig and '_FS' in name:
                    
                    log.info('Chain with name %s will be inactive' % name)
                    val[0].prescale='-1'
                    val[1].prescale='-1'
                
