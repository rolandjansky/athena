# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags  import TriggerFlags
from TriggerMenuPython.Lvl1 import Lvl1, L1Prescales_lumi_1E31, L1Prescales_lumi_1E32
from TriggerMenuPython.Lvl1Flags import Lvl1Flags
from TriggerMenuPython.MenuUtil import *
import re

def lumi(triggerPythonConfig):
    no_prescale = False
    no_Bphysics = False
    if 'no_prescale' in TriggerFlags.triggerMenuSetup(): no_prescale = True
    if 'no_Bphysics' in TriggerFlags.triggerMenuSetup(): no_Bphysics = True

    L1Prescales = {}
    HLTPrescales = {}
    streamConfig = {}
    higherGroups = {}
    
    menu_name = TriggerFlags.triggerMenuSetup()
    ## Do some aliasing here
    if menu_name == 'Physics_default': menu_name = 'Physics_pp_v1'
    elif menu_name == 'MC_loose_default': menu_name = 'MC_pp_v1_loose_mc_prescale'
    elif menu_name == 'MC_tight_default': menu_name = 'MC_pp_v1_tight_mc_prescale'
    elif menu_name == 'default_loose': menu_name = 'MC_pp_v2_loose_mc_prescale'
    elif menu_name == 'default_tight': menu_name = 'MC_pp_v2_tight_mc_prescale'

    log.info( 'Menu name: '+ menu_name)
    TriggerFlags.Lvl1.RemapThresholdsAsListed = True
    if  'default' in menu_name:
        from TriggerMenuPython.MC_lumi1E31_simpleL1Calib import setupMCLumi1E31Menu, Prescales, StreamConfig, TriggerGroups, HigherGroups
        setupMCLumi1E31Menu()
        L1Prescales = Prescales.L1Prescales
        HLTPrescales = Prescales.HLTPrescales
        streamConfig = StreamConfig
        triggerGroups = TriggerGroups
        higherGroups = HigherGroups                
    # 2010-2011 physics menus
    elif menu_name.startswith('Physics_pp_v2') :
        from TriggerMenuPython.Physics_pp_v2 import setupMenu, Prescales, StreamConfig, TriggerGroups, HigherGroups
        setupMenu()
        if 'cosmics_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_cosmics
            HLTPrescales = Prescales.HLTPrescales_cosmics
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales
        triggerGroups = TriggerGroups
        streamConfig = StreamConfig
        higherGroups = HigherGroups
    elif menu_name.startswith('Physics_pp_v3') :
        from TriggerMenuPython.Physics_pp_v3 import setupMenu, Prescales, StreamConfig, TriggerGroups, HigherGroups
        setupMenu()
        if 'cosmics_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_cosmics
            HLTPrescales = Prescales.HLTPrescales_cosmics
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales
        triggerGroups = TriggerGroups
        streamConfig = StreamConfig
        higherGroups = HigherGroups
    elif menu_name.startswith('Physics_pp_v4') :
        from TriggerMenuPython.Physics_pp_v4 import setupMenu, Prescales, StreamConfig, TriggerGroups, HigherGroups
        setupMenu()
        if 'cosmics_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_cosmics
            HLTPrescales = Prescales.HLTPrescales_cosmics
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales
        triggerGroups = TriggerGroups
        streamConfig = StreamConfig
        higherGroups = HigherGroups
    elif  menu_name.startswith('MC_pp_v3'):
        from TriggerMenuPython.MC_pp_v3 import setupMenu, Prescales, StreamConfig, TriggerGroups, HigherGroups
        setupMenu()
        triggerGroups = TriggerGroups  
        streamConfig = StreamConfig
        higherGroups = HigherGroups
        if 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        elif 'loose_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_loose_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_loose_mc_prescale            
            print HLTPrescales
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales        
    elif  menu_name.startswith('MC_pp_v4'):
        from TriggerMenuPython.MC_pp_v4 import setupMenu, Prescales, StreamConfig, TriggerGroups, HigherGroups
        setupMenu()
        triggerGroups = TriggerGroups
        streamConfig = StreamConfig
        higherGroups = HigherGroups
        if 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        elif 'loose_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_loose_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_loose_mc_prescale
            print HLTPrescales
        elif 'upgrade_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_upgrade_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_upgrade_mc_prescale            
            print HLTPrescales
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales
    elif menu_name.startswith('L1_alfa_v1') :
        ## temporary L1 menu for ALFA tests
        from TriggerMenuPython.L1_alfa_v1 import setupMenu, Prescales, StreamConfig, TriggerGroups, HigherGroups
        setupMenu()
        L1Prescales = Prescales.L1Prescales
        HLTPrescales = Prescales.HLTPrescales
        triggerGroups = TriggerGroups
        streamConfig = StreamConfig
        higherGroups = HigherGroups
    elif menu_name.startswith('L1_alfa_v2') :
        ## temporary L1 menu for ALFA tests
        from TriggerMenuPython.L1_alfa_v2 import setupMenu, Prescales, StreamConfig, TriggerGroups, HigherGroups
        setupMenu()
        L1Prescales = Prescales.L1Prescales
        HLTPrescales = Prescales.HLTPrescales
        triggerGroups = TriggerGroups
        streamConfig = StreamConfig
        higherGroups = HigherGroups
    #special MC_pp_v2 primary menu
    elif  menu_name.startswith('MC_pp_v2_primary'):
        from TriggerMenuPython.MC_pp_v2_primary import setupMenu, Prescales, StreamConfig, TriggerGroups, HigherGroups
        setupMenu()
        triggerGroups = TriggerGroups  
        streamConfig = StreamConfig
        higherGroups = HigherGroups
        L1Prescales = Prescales.L1Prescales
        HLTPrescales = Prescales.HLTPrescales
    elif  menu_name.startswith('MC_pp_v2'):
        from TriggerMenuPython.MC_pp_v2 import setupMenu, Prescales, StreamConfig, TriggerGroups, HigherGroups
        setupMenu()
        triggerGroups = TriggerGroups  
        streamConfig = StreamConfig
        higherGroups = HigherGroups
        if 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        elif 'loose_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_loose_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_loose_mc_prescale            
            print HLTPrescales
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales

    # Higher lumi menus
    elif menu_name.startswith('enhBias'):
        from TriggerMenuPython.EnhBias import setupEnhBias, Prescales, StreamConfig, TriggerGroups
        setupEnhBias()
        L1Prescales = Prescales.L1Prescales
        HLTPrescales = Prescales.HLTPrescales
        # print 'setting streamConfig'
        streamConfig = StreamConfig
        triggerGroups = TriggerGroups
              
    # Heavy Ions menus
    elif menu_name.startswith('Physics_HI_v1'):
        from TriggerMenuPython.Physics_HI_v1 import StreamConfig, setupMenu, Prescales, TriggerGroups, HigherGroups
        setupMenu()
        L1Prescales = Prescales.L1Prescales
        HLTPrescales = Prescales.HLTPrescales
        streamConfig = StreamConfig
        triggerGroups = TriggerGroups
        higherGroups = HigherGroups
    elif menu_name.startswith('MC_HI_v1'):
        from TriggerMenuPython.MC_HI_v1 import setupMCHIV1Menu, Prescales, StreamConfig, TriggerGroups 
        setupMCHIV1Menu()
        streamConfig = StreamConfig
        triggerGroups = TriggerGroups
        if 'pPb_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_pPb_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_pPb_mc_prescale
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales
    elif menu_name.startswith('InitialBeam_HI_v1'):
        from TriggerMenuPython.InitialBeam_HI_v1 import StreamConfig, setupMenu, Prescales, TriggerGroups, HigherGroups
        setupMenu()
        L1Prescales = Prescales.L1Prescales
        HLTPrescales = Prescales.HLTPrescales
        streamConfig = StreamConfig
        triggerGroups = TriggerGroups
        higherGroups = HigherGroups
    elif menu_name.startswith('MC_InitialBeam_HI_v1'):
        from TriggerMenuPython.MC_InitialBeam_HI_v1 import setupMCInitialBeamHIV1Menu, Prescales, StreamConfig, TriggerGroups 
        setupMCInitialBeamHIV1Menu()
        L1Prescales = Prescales.L1Prescales
        HLTPrescales = Prescales.HLTPrescales
        streamConfig = StreamConfig
        triggerGroups = TriggerGroups
    elif menu_name.startswith('Physics_HI_v2'):
        from TriggerMenuPython.Physics_HI_v2 import StreamConfig, setupMenu, Prescales, TriggerGroups, HigherGroups
        setupMenu()
        L1Prescales = Prescales.L1Prescales
        HLTPrescales = Prescales.HLTPrescales
        streamConfig = StreamConfig
        triggerGroups = TriggerGroups
        higherGroups = HigherGroups
    elif menu_name.startswith('MC_HI_v2'):
        from TriggerMenuPython.MC_HI_v2 import setupMCHIV2Menu, Prescales, StreamConfig, TriggerGroups 
        setupMCHIV2Menu()
        streamConfig = StreamConfig
        triggerGroups = TriggerGroups
        L1Prescales = Prescales.L1Prescales
        HLTPrescales = Prescales.HLTPrescales
    else:
        log.fatal ('Menu with name %s is not known in this version of TriggerMenuPython! ', menu_name)
        return
    #
    return (L1Prescales, HLTPrescales, streamConfig, triggerGroups, higherGroups)

def applyPrescales(triggerPythonConfig, L1Prescales, HLTPrescales):
    no_prescale = False
    no_Bphysics = False
    mc_loose_tight_prescale = False
    if 'no_prescale' in TriggerFlags.triggerMenuSetup() or \
       TriggerFlags.triggerMenuSetup() == 'default': no_prescale = True
    #now also the loose and tight prescales in mc
    if 'loose_mc_prescale' in TriggerFlags.triggerMenuSetup() or \
       'tight_mc_prescale' in TriggerFlags.triggerMenuSetup(): mc_loose_tight_prescale = True
        
    if 'no_Bphysics' in TriggerFlags.triggerMenuSetup(): no_Bphysics = True

    if L1Prescales==None and TriggerFlags.L1PrescaleSet()!='':
        log.warning('Cannot find L1 prescale set %s, not explicitly setting them' % \
                    TriggerFlags.L1PrescaleSet())
        L1Prescales = {}
    if HLTPrescales==None and TriggerFlags.HLTPrescaleSet()!='':
        log.warning('Cannot find HLT prescale set %s, not explicitly setting them' % \
                    TriggerFlags.HLTPrescaleSet())
        HLTPrescales = {}
        
    #-----------------------------------------------------------------------
    hlt_sigs = allSignatures()
##     print 'N hlt prescales: ', len(HLTPrescales)
##     print 'N hlt prescales: ', len(L1Prescales)
    applyLVL1Prescale(triggerPythonConfig, L1Prescales)
    
    if TriggerFlags.doLVL2() or TriggerFlags.doEF():
        applyHLTPrescale(triggerPythonConfig, HLTPrescales)
    #
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
                
