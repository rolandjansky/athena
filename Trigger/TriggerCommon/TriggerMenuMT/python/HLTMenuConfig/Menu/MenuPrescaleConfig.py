# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
#from TriggerMenu.menu.MenuUtil import applyHLTPrescale, resetAllPrescales

from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )


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

    elif menu_name.startswith('Physics_pp_run3_v1'):
        log.info('Physics_pp_run3_v1 menu setup')
        from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import setupMenu, Prescales
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

    elif menu_name.startswith('PhysicsP1_pp_run3_v1'):
        log.info('PhysicsP1_pp_run3_v1 menu setup')
        from TriggerMenuMT.HLTMenuConfig.Menu.PhysicsP1_pp_run3_v1 import setupMenu, Prescales
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

    elif menu_name.startswith('MC_pp_run3_v1'):
        log.info('MC_pp_run3_v1 menu setup')
        from TriggerMenuMT.HLTMenuConfig.Menu.MC_pp_run3_v1 import setupMenu, Prescales
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

    elif menu_name.startswith('PhysicsP1_HI_run3_v1'):
        log.info('PhysicsP1_HI_run3_v1 menu setup')
        from TriggerMenuMT.HLTMenuConfig.Menu.PhysicsP1_HI_run3_v1 import setupMenu, Prescales
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

    elif menu_name.startswith('Dev_HI_run3_v1'):
        log.info('Dev_HI_run3_v1 menu setup')
        from TriggerMenuMT.HLTMenuConfig.Menu.Dev_HI_run3_v1 import setupMenu, Prescales
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

    elif menu_name.startswith('Cosmic_run3_v1'):
        log.info('Cosmic_run3_v1 menu setup')
        from TriggerMenuMT.HLTMenuConfig.Menu.Cosmic_run3_v1 import setupMenu, Prescales
        setupMenu()
        if 'cosmics_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_cosmics
            HLTPrescales = Prescales.HLTPrescales_cosmics
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales
            
    else:
        log.fatal ('Menu with name %s is not known in this version of TriggerMenu! ', menu_name)
        return

    return (L1Prescales, HLTPrescales)

def addSliceChainsToPrescales(flags, cosmic_prescales):
    signatures = []
    slice_props = [prop for prop in dir(flags) if prop.endswith("Slice")]
    for slice_prop in slice_props:
        slice = getattr(flags, slice_prop)
        if slice.signatures():
            signatures.extend(slice.signatures())
        else:
            log.debug('SKIPPING ' + str(slice_prop))

    chains = [s.name for s in signatures]
    combined = {chain: [-1, 0, 0] for chain in chains}
    combined.update(cosmic_prescales)
    from copy import deepcopy
    cosmic_prescales = deepcopy(combined)
