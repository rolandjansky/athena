# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
#from TriggerMenu.menu.MenuUtil import applyHLTPrescale, resetAllPrescales

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.menu.Lumi' )

def MenuPrescaleConfig(triggerConfigHLT):

    L1Prescales = {}
    HLTPrescales = {}
    
    menu_name = TriggerFlags.triggerMenuSetup()
    ## Do some aliasing here
    if menu_name == 'Physics_default': 
        menu_name = 'LS2_v1'
    elif menu_name == 'MC_loose_default': 
        menu_name = 'LS2_v1'
    elif menu_name == 'MC_tight_default': 
        menu_name = 'LS2_v1'

    log.info( 'Menu name: '+ menu_name)

    if menu_name.startswith('LS2_v1'):
        log.info('LS2_v1 menu setup')
        from TriggerMenuMT.HLTMenuConfig.Menu.LS2_v1 import setupMenu, Prescales
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


    elif menu_name.startswith('LS2_emu_v1'):
        log.info('LS2_v1 menu setup')
        from TriggerMenuMT.HLTMenuConfig.Menu.LS2_v1 import setupMenu, Prescales
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




# def applyPrescales(triggerPythonConfig, HLTPrescales):
#     no_prescale = False
#     if 'no_prescale' in TriggerFlags.triggerMenuSetup() or \
#        TriggerFlags.triggerMenuSetup() == 'default': no_prescale = True
        
#     if HLTPrescales==None and TriggerFlags.HLTPrescaleSet()!='':
#         log.warning('Cannot find HLT prescale set %s, not explicitly setting them' % \
#                     TriggerFlags.HLTPrescaleSet())
#         HLTPrescales = {}
        
#     if TriggerFlags.doHLT():
#         applyHLTPrescale(triggerPythonConfig, HLTPrescales)

#     if TriggerFlags.L1PrescaleSet() == 'None':
#         for item in triggerPythonConfig.allItems.values():
#             item.prescale = '1' 
#     if TriggerFlags.HLTPrescaleSet() == 'None':
#         for  sig in triggerPythonConfig.allChains.values():
#             for chain in sig:
#                 chain.prescale = '1'
#                 chain.pass_through = '0'
#                 tmp_stag = chain.stream_tag
#                 chain.stream_tag = []
#                 for s in tmp_stag:
#                     chain.addStreamTag(s[0], s[1], s[2], 1)

#     if no_prescale:
#         resetAllPrescales(triggerPythonConfig)


                
