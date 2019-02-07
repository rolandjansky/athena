# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenu.menu.MenuUtil import applyHLTPrescale, resetAllPrescales

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.menu.Lumi' )

def lumi(triggerPythonConfig):

    L1Prescales = {}
    HLTPrescales = {}
    
    menu_name = TriggerFlags.triggerMenuSetup()
    ## Do some aliasing here
    if menu_name == 'Physics_default': menu_name = 'Physics_pp_v8'
    elif menu_name == 'MC_default': menu_name = 'MC_pp_v8'
    elif menu_name == 'MC_loose_default': menu_name = 'MC_pp_v7_loose_mc_prescale'
    elif menu_name == 'MC_tight_default': menu_name = 'MC_pp_v7_tight_mc_prescale'
    elif menu_name == 'default_loose': menu_name = 'MC_pp_v7_loose_mc_prescale'
    elif menu_name == 'default_tight': menu_name = 'MC_pp_v7_tight_mc_prescale'

    log.info( 'Menu name: '+ menu_name)


    if menu_name.startswith('Physics_pp_v7_primaries') :
        log.info('In physics pp v7 primaries menu setup')
        from TriggerMenu.menu.Physics_pp_v7_primaries import setupMenu, Prescales
        setupMenu()
        L1Prescales = Prescales.L1Prescales
        HLTPrescales = Prescales.HLTPrescales


    elif menu_name.startswith('Physics_pp_v7') :
        log.info('In physics pp v7 menu setup')
        from TriggerMenu.menu.Physics_pp_v7 import setupMenu, Prescales
        setupMenu()
        if 'cosmics_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_cosmics
            HLTPrescales = Prescales.HLTPrescales_cosmics
        elif 'tight' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_physics_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_physics_prescale
        elif 'unprescaled_only' in menu_name:
            L1Prescales = Prescales.L1Prescales_unprescaled_only_physics_prescale
            HLTPrescales = Prescales.HLTPrescales_unprescaled_only_physics_prescale
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales


    elif  menu_name.startswith('MC_pp_v7'):
        from TriggerMenu.menu.MC_pp_v7 import setupMenu, Prescales
        setupMenu()
        if 'loose_mc_prescale' in menu_name:
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
        elif 'BulkMCProd' in menu_name:
            L1Prescales = Prescales.L1Prescales_BulkMCProd_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_BulkMCProd_mc_prescale
            log.info(" HLTPrescales %s" % HLTPrescales)
        elif 'CPSampleProd' in menu_name:
            L1Prescales = Prescales.L1Prescales_CPSampleProd_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_CPSampleProd_mc_prescale
            log.info(" HLTPrescales %s" % HLTPrescales)
        elif 'TriggerValidation' in menu_name:
            L1Prescales = Prescales.L1Prescales_TriggerValidation_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_TriggerValidation_mc_prescale
            log.info(" HLTPrescales %s" % HLTPrescales)
        elif 'upgrade_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_upgrade_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_upgrade_mc_prescale            
            log.info(" HLTPrescales %s" %  HLTPrescales)
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
    
    elif menu_name.startswith('Physics_HI_v4') :
        log.info('Physics_HI_v4 menu setup')
        from TriggerMenu.menu.Physics_HI_v4 import setupMenu, Prescales
        setupMenu()
        if 'cosmics_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_cosmics
            HLTPrescales = Prescales.HLTPrescales_cosmics
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales
            

    elif menu_name.startswith('MC_HI_v4') :
        log.info('MC_HI_v4 menu setup')
        from TriggerMenu.menu.MC_HI_v4 import setupMenu, Prescales
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
    
    elif menu_name.startswith('Physics_HI_v5') :
        log.info('Physics_HI_v5 menu setup')
        from TriggerMenu.menu.Physics_HI_v5 import setupMenu, Prescales
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
            

    elif menu_name.startswith('MC_HI_v5') :
        log.info('MC_HI_v5 menu setup')
        from TriggerMenu.menu.MC_HI_v5 import setupMenu, Prescales
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
    elif menu_name.startswith('MC_PhaseII') :
        log.info('In MC_PhaseII menu setup')
        from TriggerMenu.menu.MC_PhaseII import setupMenu, Prescales
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
            
    else:
        log.fatal ('Menu with name %s is not known in this version of TriggerMenu! ', menu_name)
        return

    return (L1Prescales, HLTPrescales)




def applyPrescales(triggerPythonConfig, HLTPrescales):
    no_prescale = False
    if 'no_prescale' in TriggerFlags.triggerMenuSetup() or \
       TriggerFlags.triggerMenuSetup() == 'default': no_prescale = True
        
    #if L1Prescales==None and TriggerFlags.L1PrescaleSet()!='':
    #    log.warning('Cannot find L1 prescale set %s, not explicitly setting them' % \
    #                TriggerFlags.L1PrescaleSet())
    #    L1Prescales = {}
    if HLTPrescales==None and TriggerFlags.HLTPrescaleSet()!='':
        log.warning('Cannot find HLT prescale set %s, not explicitly setting them' % \
                    TriggerFlags.HLTPrescaleSet())
        HLTPrescales = {}
        
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


                
