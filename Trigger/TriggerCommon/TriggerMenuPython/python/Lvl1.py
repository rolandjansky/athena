#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from TriggerMenuPython.TriggerPythonConfig import *
from AthenaCommon.Logging import logging
from TriggerJobOpts.TriggerFlags import TriggerFlags
import re

log = logging.getLogger('TriggerMenuPython.Lvl1.py')

#
# Physics bit has to be global variable - used by multiple functions
#
TT_phys    = 1 << 7
        
class Lvl1:
    def __init__(self):
        """Constructor"""
        pass
    
    #----------------------------------------------------------------------
    # LVL1- thresholds and items
    #----------------------------------------------------------------------
    def registerItems(tm):
        """Register LVL1 thresholds for further use"""

        # Trigger type logic - should also matche StreamingDef.py definitions
        TT_rand    = 1 << 0
        TT_bptx    = 1 << 1
        TT_calo    = 1 << 2        
        TT_tgc     = 1 << 3
        TT_rpcin   = 1 << 4
        TT_minb    = 1 << 5
        TT_rpcout  = 1 << 6
        #TT_phys    = 1 << 7 - set as global variable above
        
        TT_caloem = 0 # trigger type to stream separately 
        TT_zerobs = 0 # trigger type for zero bias triggers
        
        if ('Cosmic2009_v2' in TriggerFlags.triggerMenuSetup()\
            or 'InitialBeam_v1' in TriggerFlags.triggerMenuSetup()\
            or 'Cosmic2009_simpleL1Calib' in TriggerFlags.triggerMenuSetup()\
            or 'Cosmic2009_inclMuons' in TriggerFlags.triggerMenuSetup()\
            ):
            # Assign TGC to RPCIN to free one bit for new and CALOEM trigger type
            TT_rpcin = TT_tgc
            TT_caloem = 1 << 4

        if ('InitialBeam_v2' in TriggerFlags.triggerMenuSetup()
            or 'MC_InitialBeam_v2' in TriggerFlags.triggerMenuSetup()
            or 'Cosmic_v2' in TriggerFlags.triggerMenuSetup()\
            ):
            # Assign TGC to RPCIN to free one bit for new and CALOEM trigger type
            # Assign BPTX to new ZERO BIAS trigger type
            TT_rpcin  = TT_tgc
            TT_caloem = 1 << 4
            TT_zerobs = 1 << 1
            TT_bptx   = 0

        if ('InitialBeam_v3' in TriggerFlags.triggerMenuSetup()
            or 'MC_InitialBeam_v3' in TriggerFlags.triggerMenuSetup()
            or 'Cosmic_v3' in TriggerFlags.triggerMenuSetup()\
            or 'Physics_HI_v1' in TriggerFlags.triggerMenuSetup()\
            or 'MC_HI_v1' in TriggerFlags.triggerMenuSetup()\
            or 'Physics_HI_v2' in TriggerFlags.triggerMenuSetup()\
            or 'MC_HI_v2' in TriggerFlags.triggerMenuSetup()\
            or 'InitialBeam_HI_v1' in TriggerFlags.triggerMenuSetup()\
            or 'MC_InitialBeam_HI_v1' in TriggerFlags.triggerMenuSetup()\
            ):
            # Assign TGC to RPCIN to free one bit for new and CALOEM trigger type
            # Assign BPTX to new ZERO BIAS trigger type
            TT_rpcin  = TT_tgc
            TT_caloem = 0
            TT_zerobs = 1 << 1
            TT_bptx   = 0

        if ('Physics_pp_v1' in TriggerFlags.triggerMenuSetup() \
            or 'MC_pp_v1' in TriggerFlags.triggerMenuSetup() \
            or 'Physics_pp_v2' in TriggerFlags.triggerMenuSetup() \
            or 'Physics_pp_v3' in TriggerFlags.triggerMenuSetup() \
            or 'Physics_pp_v4' in TriggerFlags.triggerMenuSetup() \
            or 'L1_pp_v2' in TriggerFlags.triggerMenuSetup() \
            or 'L1_pp_v3' in TriggerFlags.triggerMenuSetup() \
            or 'L1_pp_v4' in TriggerFlags.triggerMenuSetup() \
            or 'L1_pp_test' in TriggerFlags.triggerMenuSetup()\
            or 'L1_alfa_v1' in TriggerFlags.triggerMenuSetup() \
            or 'L1_alfa_v2' in TriggerFlags.triggerMenuSetup() \
            or 'MC_pp_v2' in TriggerFlags.triggerMenuSetup() \
            or 'MC_pp_v3' in TriggerFlags.triggerMenuSetup() \
            or 'MC_pp_v4' in TriggerFlags.triggerMenuSetup() \
            or 'Physics_default' in TriggerFlags.triggerMenuSetup() \
            or 'MC_loose_default' in TriggerFlags.triggerMenuSetup() \
            or 'MC_tight_default' in TriggerFlags.triggerMenuSetup() \
            ):
            # Assign TGC to RPCIN to free one bit for new and CALOEM trigger type
            # Assign BPTX to new ZERO BIAS trigger type
            TT_rpcin  = TT_tgc
            TT_caloem = 0
            TT_zerobs = 1 << 1
            TT_bptx   = 0

        # Calibration requests
        #  From David B - for calibration events TriggerType
        #  will be reset by CTP - values below are placeholders
        TT_calreq0 = 0x30
        TT_calreq1 = 0x32
        TT_calreq2 = 0x34
        
        
        # MUON
        thr_mu0  = tm.registerLvl1Threshold(name='MU0', type='MUON', mapping=0,)\
                   .addMuonThresholdValue(4, -49, 49, 0, 64)
        thr_mu4  = tm.registerLvl1Threshold(name='MU4', type='MUON', mapping=0,)\
                   .addMuonThresholdValue(4, -49, 49, 0, 64)
        thr_mu6  = tm.registerLvl1Threshold(name='MU6', type='MUON', mapping=1)\
                   .addMuonThresholdValue(6, -49, 49, 0, 64)
        thr_mu10 = tm.registerLvl1Threshold(name='MU10', type='MUON', mapping=2)\
                   .addMuonThresholdValue(10, -49, 49, 0, 64)
        thr_mu11 = tm.registerLvl1Threshold(name='MU11', type='MUON', mapping=3)\
                   .addMuonThresholdValue(11, -49, 49, 0, 64)
        thr_mu15 = tm.registerLvl1Threshold(name='MU15', type='MUON', mapping=4)\
                   .addMuonThresholdValue(15, -49, 49, 0, 64)
        thr_mu20 = tm.registerLvl1Threshold(name='MU20', type='MUON', mapping=5)\
                   .addMuonThresholdValue(20, -49, 49, 0, 64)
        thr_mu40 = tm.registerLvl1Threshold(name='MU40', type='MUON', mapping=5)\
                   .addMuonThresholdValue(40, -49, 49, 0, 64)


        # - Assume a negative thresholdval would switch off
        #   Needs to be agreed upon with RPC and TGC group (20/08/2008)
        # - Eta region for the RPC and TGC (in units of 0.1 in eta)
        #    - RPC (barrel) : -10<=eta<=10  (-1.0<eta<1.0)
        #    - TGC (endcap) : 11<=|eta|<=49 (1.0<|eta|<4.9)
        # - ThresholdValue with higher priority overwrites the thresholdvalue
        #   in that region.
        thr_mu0_tgc_halo = tm.registerLvl1Threshold(name='MU0_TGC_HALO', type='MUON', mapping=4)\
                           .addMuonThresholdValue(0, -49, 49, 0, 64, priority=1)\
                           .addMuonThresholdValue(-1, -10, 10, 0, 64, priority=5)
        thr_mu0_low_rpc  = tm.registerLvl1Threshold(name='MU0_LOW_RPC',type='MUON', mapping=1)\
                           .addMuonThresholdValue(-1, -49, 49, 0, 64, priority=1)\
                           .addMuonThresholdValue(0, -10, 10, 0, 64, priority=5)
        thr_mu6_rpc      = tm.registerLvl1Threshold(name='MU6_RPC', type='MUON',mapping=2)\
                           .addMuonThresholdValue(-1, -49, 49, 0, 64, priority=1)\
                           .addMuonThresholdValue(6, -10, 10, 0, 64, priority=5)
        thr_mu0_tgc      = tm.registerLvl1Threshold(name='MU0_TGC', type='MUON',mapping=3)\
                           .addMuonThresholdValue(0, -49, 49, 0, 64, priority=1)\
                           .addMuonThresholdValue(-1, -10, 10, 0, 64, priority=5)
        thr_mu6_tgc      = tm.registerLvl1Threshold(name='MU6_TGC', type='MUON',mapping=4)\
                           .addMuonThresholdValue(6, -49, 49, 0, 64, priority=1)\
                           .addMuonThresholdValue(-1, -10, 10, 0, 64, priority=5)
        thr_mu0_high_rpc = tm.registerLvl1Threshold(name='MU0_HIGH_RPC',type='MUON', mapping=3)\
                           .addMuonThresholdValue(-1, -49, 49, 0, 64, priority=1)\
                           .addMuonThresholdValue(0, -10, 10, 0, 64, priority=5)
        thr_mu0_comm = tm.registerLvl1Threshold(name='MU0_COMM',type='MUON', mapping=3)\
                       .addMuonThresholdValue(-1, -49, 49, 0, 64, priority=1)\
                       .addMuonThresholdValue(0, -10, 10, 0, 64, priority=5)

        # EM
        thr_em2   = tm.registerLvl1Threshold(name='EM2', type='EM', mapping=0)\
                    .addEMThresholdValue(2, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)        
        thr_em3   = tm.registerLvl1Threshold(name='EM3', type='EM', mapping=1)\
                    .addEMThresholdValue(3, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em4   = tm.registerLvl1Threshold(name='EM4', type='EM', mapping=2)\
                    .addEMThresholdValue(4, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em6   = tm.registerLvl1Threshold(name='EM6', type='EM', mapping=2)\
                    .addEMThresholdValue(6, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em7   = tm.registerLvl1Threshold(name='EM7', type='EM', mapping=2)\
                    .addEMThresholdValue(7, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em8   = tm.registerLvl1Threshold(name='EM8', type='EM', mapping=2)\
                    .addEMThresholdValue(8, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em10  = tm.registerLvl1Threshold(name='EM10', type='EM', mapping=3)\
                    .addEMThresholdValue(10, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em10v  = tm.registerLvl1Threshold(name='EM10V', type='EM', mapping=3)\
                    .addEMThresholdValue(10, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=1)\
                    .addEMThresholdValue(11, -8, 8, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)\
                    .addEMThresholdValue(11, 20, 24, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)\
                    .addEMThresholdValue(11, -24, -20, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)
        thr_em10h=  tm.registerLvl1Threshold(name='EM10H', type='EM', mapping=3)\
                    .addEMThresholdValue(10, -49, 49, 0, 64, IsolationOff, IsolationOff, 1)
        thr_em11h=  tm.registerLvl1Threshold(name='EM11H', type='EM', mapping=3)\
                   .addEMThresholdValue(11, -49, 49, 0, 64, IsolationOff, IsolationOff, 1)
        thr_em10vh= tm.registerLvl1Threshold(name='EM10VH', type='EM', mapping=3)\
                    .addEMThresholdValue(11, -49, 49, 0, 64, IsolationOff, IsolationOff, 1, priority=1)\
                    .addEMThresholdValue(10, -20, -8, 0, 64, IsolationOff, IsolationOff, 1, priority=2)\
                    .addEMThresholdValue(10, 8, 20, 0, 64, IsolationOff, IsolationOff, 1, priority=2)
        thr_em12  = tm.registerLvl1Threshold(name='EM12', type='EM', mapping=3)\
                    .addEMThresholdValue(12, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em13  = tm.registerLvl1Threshold(name='EM13', type='EM', mapping=4)\
                    .addEMThresholdValue(13, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em13i = tm.registerLvl1Threshold(name='EM13I', type='EM', mapping=5)\
                    .addEMThresholdValue(13, -49, 49, 0, 64, 4, 3, 2)
        thr_em15h = tm.registerLvl1Threshold(name='EM15H', type='EM', mapping=4)\
                    .addEMThresholdValue(15, -49, 49, 0, 64, IsolationOff, IsolationOff, 1)
        thr_em14vh= tm.registerLvl1Threshold(name='EM14VH', type='EM', mapping=5)\
                   .addEMThresholdValue(15, -49, 49, 0, 64, IsolationOff, IsolationOff, 1, priority=1)\
                   .addEMThresholdValue(14, -20, -8, 0, 64, IsolationOff, IsolationOff, 1, priority=2)\
                   .addEMThresholdValue(14, 8, 20, 0, 64, IsolationOff, IsolationOff, 1, priority=2)
        thr_em16 = tm.registerLvl1Threshold(name='EM16', type='EM', mapping=4)\
                   .addEMThresholdValue(16, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em16h = tm.registerLvl1Threshold(name='EM16H', type='EM', mapping=4)\
                    .addEMThresholdValue(16, -49, 49, 0, 64, IsolationOff, IsolationOff, 1)
        thr_em16vh= tm.registerLvl1Threshold(name='EM16VH', type='EM', mapping=5)\
                    .addEMThresholdValue(18, -49, 49, 0, 64, IsolationOff, IsolationOff, 1, priority=1)\
                    .addEMThresholdValue(17, -12, -8, 0, 64, IsolationOff, IsolationOff, 1, priority=2)\
                    .addEMThresholdValue(17, 8, 12, 0, 64, IsolationOff, IsolationOff, 1, priority=2)\
                    .addEMThresholdValue(16, -16, -12, 0, 64, IsolationOff, IsolationOff, 1, priority=2)\
                    .addEMThresholdValue(16, 12, 16, 0, 64, IsolationOff, IsolationOff, 1, priority=2)\
                    .addEMThresholdValue(17, -20, -16, 0, 64, IsolationOff, IsolationOff, 1, priority=2)\
                    .addEMThresholdValue(17, 16, 20, 0, 64, IsolationOff, IsolationOff, 1, priority=2)
        thr_em16v= tm.registerLvl1Threshold(name='EM16V', type='EM', mapping=5)\
                    .addEMThresholdValue(18, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=1)\
                    .addEMThresholdValue(17, -12, -8, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)\
                    .addEMThresholdValue(17, 8, 12, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)\
                    .addEMThresholdValue(16, -16, -12, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)\
                    .addEMThresholdValue(16, 12, 16, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)\
                    .addEMThresholdValue(17, -20, -16, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)\
                    .addEMThresholdValue(17, 16, 20, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)
        thr_em18  = tm.registerLvl1Threshold(name='EM18', type='EM', mapping=6)\
                    .addEMThresholdValue(18, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em18h = tm.registerLvl1Threshold(name='EM18H', type='EM', mapping=6)\
                    .addEMThresholdValue(18, -49, 49, 0, 64, IsolationOff, IsolationOff, 1)
        thr_em18i = tm.registerLvl1Threshold(name='EM18I', type='EM', mapping=7)\
                    .addEMThresholdValue(18, -49, 49, 0, 64, 4, 3, 2)
        thr_em18vh= tm.registerLvl1Threshold(name='EM18VH', type='EM', mapping=6)\
                    .addEMThresholdValue(20, -49, 49, 0, 64, IsolationOff, IsolationOff, 1, priority=1)\
                    .addEMThresholdValue(19, -12, -8, 0, 64, IsolationOff, IsolationOff, 1, priority=2)\
                    .addEMThresholdValue(19, 8, 12, 0, 64, IsolationOff, IsolationOff, 1, priority=2)\
                    .addEMThresholdValue(18, -16, -12, 0, 64, IsolationOff, IsolationOff, 1, priority=2)\
                    .addEMThresholdValue(18, 12, 16, 0, 64, IsolationOff, IsolationOff, 1, priority=2)\
                    .addEMThresholdValue(19, -20, -16, 0, 64, IsolationOff, IsolationOff, 1, priority=2)\
                    .addEMThresholdValue(19, 16, 20, 0, 64, IsolationOff, IsolationOff, 1, priority=2)
        thr_em20h = tm.registerLvl1Threshold(name='EM20H', type='EM', mapping=4)\
                    .addEMThresholdValue(20, -49, 49, 0, 64, IsolationOff, IsolationOff, 1)
        thr_em23i = tm.registerLvl1Threshold(name='EM23I', type='EM', mapping=8)\
                    .addEMThresholdValue(23, -49, 49, 0, 64, 4, 3, 2)
        thr_em23 = tm.registerLvl1Threshold(name='EM23', type='EM', mapping=8)\
                    .addEMThresholdValue(23, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em30 = tm.registerLvl1Threshold(name='EM30', type='EM', mapping=8)\
                    .addEMThresholdValue(30, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em31 = tm.registerLvl1Threshold(name='EM31', type='EM', mapping=8)\
                   .addEMThresholdValue(31, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em35 = tm.registerLvl1Threshold(name='EM35', type='EM', mapping=8)\
                    .addEMThresholdValue(35, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em100  = tm.registerLvl1Threshold(name='EM100', type='EM', mapping=9)\
                     .addEMThresholdValue(100, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em7i   = tm.registerLvl1Threshold(name='EM7I', type='EM', mapping=7)\
                    .addEMThresholdValue(7, -49, 49, 0, 64, 4, 3, 2)
        thr_em50  = tm.registerLvl1Threshold(name='EM50', type='EM', mapping=11)\
                    .addEMThresholdValue(50, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        
        # EM
        # - Add/remap new L1 thresholds:
        #   EM7->EM5 EM13->EM10 EM13I->EM10I EM18->EM14 EM18I->EM14I EM23I->EM19I EM100->EM85
        # - For these new thresholds use the same mapping as threshold being replaced
        #   with exception of EM10 which already exists - it keeps the same mapping=3
        #
        thr_em5   = tm.registerLvl1Threshold(name='EM5', type='EM', mapping=3)\
                    .addEMThresholdValue(5, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em5i   = tm.registerLvl1Threshold(name='EM5I', type='EM', mapping=4)\
                    .addEMThresholdValue(5, -49, 49, 0, 64, 4, 3, 2)        
        thr_em10i = tm.registerLvl1Threshold(name='EM10I', type='EM', mapping=5)\
                    .addEMThresholdValue(10, -49, 49, 0, 64, 4, 3, 2)
        thr_em14  = tm.registerLvl1Threshold(name='EM14', type='EM', mapping=6)\
                    .addEMThresholdValue(14, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_em14i = tm.registerLvl1Threshold(name='EM14I', type='EM', mapping=7)\
                    .addEMThresholdValue(14, -49, 49, 0, 64, 4, 3, 2)
        thr_em19i = tm.registerLvl1Threshold(name='EM19I', type='EM', mapping=8)\
                    .addEMThresholdValue(19, -49, 49, 0, 64, 4, 3, 2)
        thr_em85  = tm.registerLvl1Threshold(name='EM85', type='EM', mapping=7)\
                    .addEMThresholdValue(85, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        
        # TAU
        thr_tau3   = tm.registerLvl1Threshold('HA3', 'TAU', mapping=0)\
                     .addTauThresholdValue(3, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_tau5   = tm.registerLvl1Threshold('HA5', 'TAU', mapping=0)\
                     .addTauThresholdValue(5, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_tau6   = tm.registerLvl1Threshold('HA6', 'TAU', mapping=1)\
                     .addTauThresholdValue(6, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_tau9  = tm.registerLvl1Threshold('HA9', 'TAU', mapping=2)\
                    .addTauThresholdValue(9, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_tau9i  = tm.registerLvl1Threshold('HA9I', 'TAU', mapping=3)\
                     .addTauThresholdValue(9, -49, 49, 0, 64, 6, IsolationOff, IsolationOff)        
        #        thr_tau11i = tm.registerLvl1Threshold('HA11I', 'TAU', mapping=4)\
        #                     .addTauThresholdValue(11, -49, 49, 0, 64, 6, IsolationOff, IsolationOff)
        thr_tau15   = tm.registerLvl1Threshold('HA15', 'TAU', mapping=3)\
                      .addTauThresholdValue(15, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_tau15i = tm.registerLvl1Threshold('HA15I', 'TAU', mapping=5)\
                     .addTauThresholdValue(15, -49, 49, 0, 64, 4, IsolationOff, IsolationOff)
        thr_tau16 = tm.registerLvl1Threshold('HA16', 'TAU', mapping=4)\
                    .addTauThresholdValue(16, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_tau16i = tm.registerLvl1Threshold('HA16I', 'TAU', mapping=5)\
                     .addTauThresholdValue(16, -49, 49, 0, 64, 6, IsolationOff, IsolationOff)
        thr_tau25  = tm.registerLvl1Threshold('HA25', 'TAU', mapping=6)\
                     .addTauThresholdValue(25, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        #        thr_tau25i = tm.registerLvl1Threshold('HA25I', 'TAU', mapping=8)\
        #                     .addTauThresholdValue(25, -49, 49, 0, 64, 6, IsolationOff, IsolationOff)
        thr_tau40  = tm.registerLvl1Threshold('HA40', 'TAU', mapping=7)\
                     .addTauThresholdValue(40, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)

        
        # TAU
        # - Add/remap new L1 thresholds:
        #   TAU5->TAU5, TAU6->TAU5, TAU9->TAU6, TAU9I->TAU6I, TAU16->TAU11, TAU16I->TAU11I, TAU25->TAU11, TAU40->TAU20
        #   
        thr_tau6i  = tm.registerLvl1Threshold('HA6I', 'TAU', mapping=2)\
                     .addTauThresholdValue(6, -49, 49, 0, 64, 5, IsolationOff, IsolationOff)
        thr_tau8 = tm.registerLvl1Threshold('HA8', 'TAU', mapping=3)\
                    .addTauThresholdValue(8, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_tau8v = tm.registerLvl1Threshold('HA8V', 'TAU', mapping=3)\
                    .addTauThresholdValue(10, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=1)\
                    .addTauThresholdValue(8, -16, -8, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)\
                    .addTauThresholdValue(8, 8, 16, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)
        thr_tau8i = tm.registerLvl1Threshold('HA8I', 'TAU', mapping=3)\
                    .addTauThresholdValue(8, -49, 49, 0, 64, 4, IsolationOff, IsolationOff)
        thr_tau8vi = tm.registerLvl1Threshold('HA8VI', 'TAU', mapping=3)\
                     .addTauThresholdValue(8, -49, 49, 0, 64, 5, IsolationOff, IsolationOff, priority=1)\
                     .addTauThresholdValue(10, -16, 16, 0, 64, 5, IsolationOff, IsolationOff, priority=2)\
                     .addTauThresholdValue(10, 20, 25, 0, 64, 5, IsolationOff, IsolationOff, priority=2)\
                     .addTauThresholdValue(10, -25, -20, 0, 64, 5, IsolationOff, IsolationOff, priority=2)
        thr_tau11 = tm.registerLvl1Threshold('HA11', 'TAU', mapping=4)\
                    .addTauThresholdValue(11, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_tau11v = tm.registerLvl1Threshold('HA11V', 'TAU', mapping=3)\
                     .addTauThresholdValue(11, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=1)\
                     .addTauThresholdValue(13, -16, 16, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)\
                     .addTauThresholdValue(13, 20, 25, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)\
                     .addTauThresholdValue(13, -25, -20, 0, 64, IsolationOff, IsolationOff, IsolationOff, priority=2)
        thr_tau11i = tm.registerLvl1Threshold('HA11I', 'TAU', mapping=5)\
                     .addTauThresholdValue(11, -49, 49, 0, 64, 4, IsolationOff, IsolationOff)
        thr_tau20  = tm.registerLvl1Threshold('HA20', 'TAU', mapping=6)\
                     .addTauThresholdValue(20, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_tau30  = tm.registerLvl1Threshold('HA30', 'TAU', mapping=7)\
                     .addTauThresholdValue(30, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        thr_tau30i = tm.registerLvl1Threshold('HA30I', 'TAU', mapping=7)\
                     .addTauThresholdValue(30, -49, 49, 0, 64, 4, IsolationOff, IsolationOff)
        thr_tau50  = tm.registerLvl1Threshold('HA50', 'TAU', mapping=7)\
                     .addTauThresholdValue(50, -49, 49, 0, 64, IsolationOff, IsolationOff, IsolationOff)
        
        # JET
        thr_j5 = tm.registerLvl1Threshold('J5', 'JET', mapping=0)\
                 .addJetThresholdValue(5, -49, 49, 0, 64, 4)
        thr_j5_win6 = tm.registerLvl1Threshold('J5_win6', 'JET', mapping=0)\
                 .addJetThresholdValue(5, -49, 49, 0, 64, 6)
        thr_j5_win8 = tm.registerLvl1Threshold('J5_win8', 'JET', mapping=0)\
                 .addJetThresholdValue(5, -49, 49, 0, 64, 8)
        thr_j10 = tm.registerLvl1Threshold('J10', 'JET', mapping=1)\
                  .addJetThresholdValue(10, -49, 49, 0, 64, 8)
        thr_j10_6 = tm.registerLvl1Threshold('J10_win6', 'JET', mapping=7)\
                    .addJetThresholdValue(10, -49, 49, 0, 64, 6)        
        thr_j18 = tm.registerLvl1Threshold('J18', 'JET', mapping=2)\
                  .addJetThresholdValue(18, -49, 49, 0, 64, 8)
        thr_j20 = tm.registerLvl1Threshold('J20', 'JET', mapping=2)\
                  .addJetThresholdValue(20, -49, 49, 0, 64, 8) # new jet threshold
        thr_j23 = tm.registerLvl1Threshold('J23', 'JET', mapping=3)\
                  .addJetThresholdValue(23, -49, 49, 0, 64, 8)
        thr_j35 = tm.registerLvl1Threshold('J35', 'JET', mapping=4)\
                  .addJetThresholdValue(35, -49, 49, 0, 64, 8)
        thr_j40 = tm.registerLvl1Threshold('J40', 'JET', mapping=3)\
                  .addJetThresholdValue(40, -49, 49, 0, 64, 8) # new jet threshold
        thr_j42 = tm.registerLvl1Threshold('J42', 'JET', mapping=5)\
                  .addJetThresholdValue(42, -49, 49, 0, 64, 8)
        thr_j70 = tm.registerLvl1Threshold('J70', 'JET', mapping=4)\
                  .addJetThresholdValue(70, -49, 49, 0, 64, 8)
        thr_j100 = tm.registerLvl1Threshold('J100', 'JET', mapping=5)\
                   .addJetThresholdValue(100, -49, 49, 0, 64, 8)
        thr_j120 = tm.registerLvl1Threshold('J120', 'JET', mapping=7)\
                   .addJetThresholdValue(120, -49, 49, 0, 64, 8)
        thr_j130 = tm.registerLvl1Threshold('J130', 'JET', mapping=6)\
                   .addJetThresholdValue(130, -49, 49, 0, 64, 8) # new jet threshold
        thr_j150 = tm.registerLvl1Threshold('J150', 'JET', mapping=7)\
                   .addJetThresholdValue(150, -49, 49, 0, 64, 8) # new jet threshold

#### from Allan
        thr_j15cf = tm.registerLvl1Threshold('J15CF', 'JET', mapping=7)\
                    .addJetThresholdValue(1023, -49, 49, 0, 64, 8)\
                    .addJetThresholdValue(15, -32, -29, 0, 64, 8, priority=1)\
                    .addJetThresholdValue(15, 27, 32, 0, 64, 8, priority=1)


        thr_j200 = tm.registerLvl1Threshold('J200', 'JET', mapping=7)\
                   .addJetThresholdValue(200, -49, 49, 0, 64, 8)
        thr_j350 = tm.registerLvl1Threshold('J350', 'JET', mapping=7)\
                   .addJetThresholdValue(350, -49, 49, 0, 64, 8)
        
        # - Add/remap new L1 thresholds:
        #   J20->J15, J40->J30, J70->J55, J100->J75, J130->J95, J150->J115
        #   J5 and J10 stay as they are
        #
        thr_j15 = tm.registerLvl1Threshold('J15', 'JET', mapping=2)\
                  .addJetThresholdValue(15, -49, 49, 0, 64, 8)
        thr_j30 = tm.registerLvl1Threshold('J30', 'JET', mapping=3)\
                   .addJetThresholdValue(30, -49, 49, 0, 64, 8)
        thr_j50 = tm.registerLvl1Threshold('J50', 'JET', mapping=4)\
                   .addJetThresholdValue(50, -49, 49, 0, 64, 8)
        thr_j55 = tm.registerLvl1Threshold('J55', 'JET', mapping=4)\
                   .addJetThresholdValue(55, -49, 49, 0, 64, 8)
        thr_j75 = tm.registerLvl1Threshold('J75', 'JET', mapping=5)\
                   .addJetThresholdValue(75, -49, 49, 0, 64, 8)
        thr_j95 = tm.registerLvl1Threshold('J95', 'JET', mapping=6)\
                   .addJetThresholdValue(95, -49, 49, 0, 64, 8)
        thr_j115 = tm.registerLvl1Threshold('J115', 'JET', mapping=7)\
                   .addJetThresholdValue(115, -49, 49, 0, 64, 8)
        thr_j175 = tm.registerLvl1Threshold('J175', 'JET', mapping=7)\
                   .addJetThresholdValue(175, -49, 49, 0, 64, 8)
        thr_j250 = tm.registerLvl1Threshold('J250', 'JET', mapping=7)\
                   .addJetThresholdValue(250, -49, 49, 0, 64, 8)

        #zero bias examples from a single and dijet seeds (mapping must be same as seed!) No threshold values.
        #thr_zb_j10  = tm.registerLvl1Threshold('ZB_J10', 'ZB',
        #mapping=1, seed='J10', seed_ttype = 'JET', seed_multi=1, bcdelay=3564)
        #thr_zb_2j10 = tm.registerLvl1Threshold('ZB_2J10', 'ZB',
        #mapping=1, seed='J10', seed_ttype = 'JET', seed_multi=2, bcdelay=3564)
        #for the v3 menu
        #thr_zb_4MBTS_A = tm.registerLvl1Threshold('ZB_4MBTS_A', 'ZB', mapping=0, seed='MBTS_A', seed_ttype = 'MBTS', seed_multi=4, bcdelay=3564)

        thr_zb_em12 = tm.registerLvl1Threshold('ZB_EM12', 'ZB', mapping=0, seed='EM12', seed_ttype = 'EM', seed_multi=1, bcdelay=3564)
        thr_zb_em14 = tm.registerLvl1Threshold('ZB_EM14', 'ZB', mapping=0, seed='EM14', seed_ttype = 'EM', seed_multi=1, bcdelay=3564)

        thr_zb_4MBTS_A = tm.registerLvl1Threshold('ZB_4MBTS_A', 'ZB', mapping=24, seed='MBTS_A', seed_ttype = 'MBTS', seed_multi=4, bcdelay=3564)
        thr_zb_j10 = tm.registerLvl1Threshold('ZB_J10', 'ZB', mapping=0, seed='J10', seed_ttype = 'JET', seed_multi=1, bcdelay=3564)
        thr_zb_j50 = tm.registerLvl1Threshold('ZB_J50', 'ZB', mapping=0, seed='J50', seed_ttype = 'JET', seed_multi=1, bcdelay=3564)
        thr_zb_j75 = tm.registerLvl1Threshold('ZB_J75', 'ZB', mapping=0, seed='J75', seed_ttype = 'JET', seed_multi=1, bcdelay=3564)
        
        # JE
        thr_je60   = tm.registerLvl1Threshold('JE60', 'JE', mapping=0)\
                     .addEnergyThresholdValue(60, -49, 49, 0, 64)
        thr_je100  = tm.registerLvl1Threshold('JE100', 'JE', mapping=1)\
                     .addEnergyThresholdValue(100, -49, 49, 0, 64)
        thr_je120  = tm.registerLvl1Threshold('JE120', 'JE', mapping=2)\
                     .addEnergyThresholdValue(120, -49, 49, 0, 64)
        thr_je140  = tm.registerLvl1Threshold('JE140', 'JE', mapping=2)\
                     .addEnergyThresholdValue(140, -49, 49, 0, 64)
        thr_je200  = tm.registerLvl1Threshold('JE200', 'JE', mapping=2)\
                     .addEnergyThresholdValue(200, -49, 49, 0, 64)
        thr_je220  = tm.registerLvl1Threshold('JE220', 'JE', mapping=3)\
                     .addEnergyThresholdValue(220, -49, 49, 0, 64)
        thr_je280 = tm.registerLvl1Threshold('JE280', 'JE', mapping=3)\
                    .addEnergyThresholdValue(280, -49, 49, 0, 64)
        thr_je300 = tm.registerLvl1Threshold('JE300', 'JE', mapping=3)\
                    .addEnergyThresholdValue(300, -49, 49, 0, 64)
        thr_je340 = tm.registerLvl1Threshold('JE340', 'JE', mapping=3)\
                    .addEnergyThresholdValue(340, -49, 49, 0, 64)
        thr_je350 = tm.registerLvl1Threshold('JE350', 'JE', mapping=3)\
                    .addEnergyThresholdValue(350, -49, 49, 0, 64)
        thr_je400 = tm.registerLvl1Threshold('JE400', 'JE', mapping=3)\
                    .addEnergyThresholdValue(400, -49, 49, 0, 64)
        thr_je500 = tm.registerLvl1Threshold('JE500', 'JE', mapping=3)\
                    .addEnergyThresholdValue(500, -49, 49, 0, 64)
        
        # FJET (JF, JB)
##         thr_jb10 = tm.registerLvl1Threshold('JB10', 'JB', mapping=0)\
##                    .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
##                    .addJetThresholdValue(10, -49, 0, 0, 64, 8, 2)
##         thr_jb18 = tm.registerLvl1Threshold('JB18', 'JB', mapping=1)\
##                    .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
##                    .addJetThresholdValue(18, -49, 0, 0, 64, 8, 2)
##         thr_jb26 = tm.registerLvl1Threshold('JB26', 'JB', mapping=2)\
##                    .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
##                    .addJetThresholdValue(26, -49, 0, 0, 64, 8, 2)
##         thr_jb65 = tm.registerLvl1Threshold('JB65', 'JB', mapping=3)\
##                    .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
##                    .addJetThresholdValue(65, -49, 0, 0, 64, 8, 2)
##         thr_jf10 = tm.registerLvl1Threshold('JF10', 'JF', mapping=0)\
##                    .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
##                    .addJetThresholdValue(10, 0, 49, 0, 64, 8, 2)
##         thr_jf18 = tm.registerLvl1Threshold('JF18', 'JF', mapping=1)\
##                    .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
##                    .addJetThresholdValue(18, 0, 49, 0, 64, 8, 2)
##         thr_jf26 = tm.registerLvl1Threshold('JF26', 'JF', mapping=2)\
##                    .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
##                    .addJetThresholdValue(26, 0, 49, 0, 64, 8, 2)
##         thr_jf65 = tm.registerLvl1Threshold('JF65', 'JF', mapping=3)\
##                    .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
##                    .addJetThresholdValue(65, 0, 49, 0, 64, 8, 2)        


        #pPb thresholds
        thr_jb0  = tm.registerLvl1Threshold('JB0', 'JB', mapping=0)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(0, -49, 0, 0, 64, 8, 2)
        thr_jb1  = tm.registerLvl1Threshold('JB1', 'JB', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(1, -49, 0, 0, 64, 8, 2)
        thr_jb2  = tm.registerLvl1Threshold('JB2', 'JB', mapping=2)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(2, -49, 0, 0, 64, 8, 2)
        thr_jb3  = tm.registerLvl1Threshold('JB3', 'JB', mapping=3)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(3, -49, 0, 0, 64, 8, 2)
                               
        thr_jb5  = tm.registerLvl1Threshold('JB5', 'JB', mapping=0)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(5, -49, 0, 0, 64, 8, 2)
        thr_jb10  = tm.registerLvl1Threshold('JB10', 'JB', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(10, -49, 0, 0, 64, 8, 2)
        thr_jb15  = tm.registerLvl1Threshold('JB15', 'JB', mapping=2)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(15, -49, 0, 0, 64, 8, 2)
        thr_jb30  = tm.registerLvl1Threshold('JB30', 'JB', mapping=3)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(30, -49, 0, 0, 64, 8, 2)
        thr_jb18 = tm.registerLvl1Threshold('JB18', 'JB', mapping=0)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(18, -49, 0, 0, 64, 8, 2)
        thr_jb35 = tm.registerLvl1Threshold('JB35', 'JB', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(35, -49, 0, 0, 64, 8, 2)
        thr_jb50 = tm.registerLvl1Threshold('JB50', 'JB', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(50, -49, 0, 0, 64, 8, 2)
        thr_jb55 = tm.registerLvl1Threshold('JB55', 'JB', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(55, -49, 0, 0, 64, 8, 2)
        thr_jb75 = tm.registerLvl1Threshold('JB75', 'JB', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(75, -49, 0, 0, 64, 8, 2)
        thr_jb100 = tm.registerLvl1Threshold('JB100', 'JB', mapping=1)\
                    .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                    .addJetThresholdValue(100, -49, 0, 0, 64, 8, 2)
        thr_jb95 = tm.registerLvl1Threshold('JB95', 'JB', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(95, -49, 0, 0, 64, 8, 2)
        thr_jb70 = tm.registerLvl1Threshold('JB70', 'JB', mapping=2)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(70, -49, 0, 0, 64, 8, 2)
        thr_jb120 = tm.registerLvl1Threshold('JB120', 'JB', mapping=3)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(120, -49, 0, 0, 64, 8, 2)
        thr_jb20  = tm.registerLvl1Threshold('JB20', 'JB', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(20, -49, 0, 0, 64, 8, 2)
        thr_jb40  = tm.registerLvl1Threshold('JB40', 'JB', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(40, -49, 0, 0, 64, 8, 2)

        #pPb thresholds
        thr_jf0  = tm.registerLvl1Threshold('JF0', 'JF', mapping=0)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(0, 0, 49, 0, 64, 8, 2)
        thr_jf1  = tm.registerLvl1Threshold('JF1', 'JF', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(1, 0, 49, 0, 64, 8, 2)
        thr_jf2  = tm.registerLvl1Threshold('JF2', 'JF', mapping=2)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(2, 0, 49, 0, 64, 8, 2)
        thr_jf3  = tm.registerLvl1Threshold('JF3', 'JF', mapping=3)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(3, 0, 49, 0, 64, 8, 2)
                        
        thr_jf5  = tm.registerLvl1Threshold('JF5', 'JF', mapping=0)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(5, 0, 49, 0, 64, 8, 2)
        thr_jf10  = tm.registerLvl1Threshold('JF10', 'JF', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(10, 0, 49, 0, 64, 8, 2)
        thr_jf15  = tm.registerLvl1Threshold('JF15', 'JF', mapping=2)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(15, 0, 49, 0, 64, 8, 2)
        thr_jf30  = tm.registerLvl1Threshold('JF30', 'JF', mapping=3)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(30, 0, 49, 0, 64, 8, 2)
        thr_jf18 = tm.registerLvl1Threshold('JF18', 'JF', mapping=0)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(18, 0, 49, 0, 64, 8, 2)
        thr_jf35 = tm.registerLvl1Threshold('JF35', 'JF', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(35, 0, 49, 0, 64, 8, 2)
        thr_jf50 = tm.registerLvl1Threshold('JF50', 'JF', mapping=2)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(50, 0, 49, 0, 64, 8, 2)
        thr_jf55 = tm.registerLvl1Threshold('JF55', 'JF', mapping=2)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(55, 0, 49, 0, 64, 8, 2)
        thr_jf75 = tm.registerLvl1Threshold('JF75', 'JF', mapping=3)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(75, 0, 49, 0, 64, 8, 2)        
        thr_jf95 = tm.registerLvl1Threshold('JF95', 'JF', mapping=3)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(95, 0, 49, 0, 64, 8, 2)        
        thr_jf100 = tm.registerLvl1Threshold('JF100', 'JF', mapping=3)\
                    .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                    .addJetThresholdValue(100, 0, 49, 0, 64, 8, 2)
        
        thr_jf70 = tm.registerLvl1Threshold('JF70', 'JF', mapping=2)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(70, 0, 49, 0, 64, 8, 2)
        thr_jf120 = tm.registerLvl1Threshold('JF120', 'JF', mapping=3)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(120, 0, 49, 0, 64, 8, 2)        
        thr_jf20  = tm.registerLvl1Threshold('JF20', 'JF', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(20, 0, 49, 0, 64, 8, 2)
        thr_jf40  = tm.registerLvl1Threshold('JF40', 'JF', mapping=1)\
                   .addJetThresholdValue(JetOff, -49, 49, 0, 64, 8, 1)\
                   .addJetThresholdValue(40, 0, 49, 0, 64, 8, 2)

        # TE, XE
        #pPb thresholds
        is_HI_v2 = 'HI_v2' in TriggerFlags.triggerMenuSetup()
        if is_HI_v2:
            thr_te0  = tm.registerLvl1Threshold('TE0', 'TE', mapping=0)\
                       .addEnergyThresholdValue(0x3fff, -49, 49, 0, 64, priority=0)\
                       .addEnergyThresholdValue(0, -49, -32, 0, 64, priority=1)\
                       .addEnergyThresholdValue(0, 32, 49, 0, 64, priority=1)
        else:
            thr_te0  = tm.registerLvl1Threshold('TE0', 'TE', mapping=0)\
                       .addEnergyThresholdValue(0, -49, 49, 0, 64)
        
        thr_te1  = tm.registerLvl1Threshold('TE1', 'TE', mapping=0)\
                   .addEnergyThresholdValue(1, -49, 49, 0, 64)
        thr_te2  = tm.registerLvl1Threshold('TE2', 'TE', mapping=0)\
                   .addEnergyThresholdValue(2, -49, 49, 0, 64)
        thr_te3  = tm.registerLvl1Threshold('TE3', 'TE', mapping=0)\
                   .addEnergyThresholdValue(3, -49, 49, 0, 64)
        thr_te4  = tm.registerLvl1Threshold('TE4', 'TE', mapping=0)\
                   .addEnergyThresholdValue(4, -49, 49, 0, 64)
        if is_HI_v2:
            thr_te5  = tm.registerLvl1Threshold('TE5', 'TE', mapping=0)\
                       .addEnergyThresholdValue(0x3fff, -49, 49, 0, 64, priority=0)\
                       .addEnergyThresholdValue(5, -49, -32, 0, 64, priority=1)\
                       .addEnergyThresholdValue(5, 32, 49, 0, 64, priority=1)            
        else:
            thr_te5  = tm.registerLvl1Threshold('TE5', 'TE', mapping=0)\
                       .addEnergyThresholdValue(5, -49, 49, 0, 64)
        
        if is_HI_v2:    
            thr_te10  = tm.registerLvl1Threshold('TE10', 'TE', mapping=0)\
                       .addEnergyThresholdValue(0x3fff, -49, 49, 0, 64, priority=0)\
                       .addEnergyThresholdValue(10, -49, -32, 0, 64, priority=1)\
                       .addEnergyThresholdValue(10, 32, 49, 0, 64, priority=1)
        else:
            thr_te10  = tm.registerLvl1Threshold('TE10', 'TE', mapping=0)\
                       .addEnergyThresholdValue(10, -49, 49, 0, 64)

        if is_HI_v2:
            thr_te20  = tm.registerLvl1Threshold('TE20', 'TE', mapping=0)\
                        .addEnergyThresholdValue(0x3fff, -49, 49, 0, 64, priority=0)\
                        .addEnergyThresholdValue(20, -49, -32, 0, 64, priority=1)\
                        .addEnergyThresholdValue(20, 32, 49, 0, 64, priority=1)            
        else:
            thr_te20  = tm.registerLvl1Threshold('TE20', 'TE', mapping=0)\
                        .addEnergyThresholdValue(20, -49, 49, 0, 64)

        if is_HI_v2:
            thr_te35  = tm.registerLvl1Threshold('TE35', 'TE', mapping=0)\
                        .addEnergyThresholdValue(0x3fff, -49, 49, 0, 64, priority=0)\
                        .addEnergyThresholdValue(35, -49, -32, 0, 64, priority=1)\
                        .addEnergyThresholdValue(35, 32, 49, 0, 64, priority=1)
        else:
            thr_te35  = tm.registerLvl1Threshold('TE35', 'TE', mapping=0)\
                        .addEnergyThresholdValue(35, -49, 49, 0, 64)

        thr_te30  = tm.registerLvl1Threshold('TE30', 'TE', mapping=0)\
                    .addEnergyThresholdValue(30, -49, 49, 0, 64)
        thr_te40  = tm.registerLvl1Threshold('TE40', 'TE', mapping=0)\
                    .addEnergyThresholdValue(40, -49, 49, 0, 64)
        if is_HI_v2:
            thr_te50  = tm.registerLvl1Threshold('TE50', 'TE', mapping=0)\
                        .addEnergyThresholdValue(0x3fff, -49, 49, 0, 64, priority=0)\
                        .addEnergyThresholdValue(50, -49, -32, 0, 64, priority=1)\
                        .addEnergyThresholdValue(50, 32, 49, 0, 64, priority=1)

            thr_te65  = tm.registerLvl1Threshold('TE65', 'TE', mapping=0)\
                        .addEnergyThresholdValue(0x3fff, -49, 49, 0, 64, priority=0)\
                        .addEnergyThresholdValue(65, -49, -32, 0, 64, priority=1)\
                        .addEnergyThresholdValue(65, 32, 49, 0, 64, priority=1)                    

            thr_te90  = tm.registerLvl1Threshold('TE90', 'TE', mapping=0)\
                        .addEnergyThresholdValue(0x3fff, -49, 49, 0, 64, priority=0)\
                        .addEnergyThresholdValue(90, -49, -32, 0, 64, priority=1)\
                        .addEnergyThresholdValue(90, 32, 49, 0, 64, priority=1)
            
            thr_te100  = tm.registerLvl1Threshold('TE100', 'TE', mapping=0)\
                         .addEnergyThresholdValue(0x3fff, -49, 49, 0, 64, priority=0)\
                         .addEnergyThresholdValue(100, -49, -32, 0, 64, priority=1)\
                         .addEnergyThresholdValue(100, 32, 49, 0, 64, priority=1)
            
            thr_te115  = tm.registerLvl1Threshold('TE115', 'TE', mapping=0)\
                         .addEnergyThresholdValue(0x3fff, -49, 49, 0, 64, priority=0)\
                         .addEnergyThresholdValue(115, -49, -32, 0, 64, priority=1)\
                         .addEnergyThresholdValue(115, 32, 49, 0, 64, priority=1)

        else:
            thr_te50  = tm.registerLvl1Threshold('TE50', 'TE', mapping=0)\
                        .addEnergyThresholdValue(50, -49, 49, 0, 64)

            thr_te65  = tm.registerLvl1Threshold('TE65', 'TE', mapping=0)\
                        .addEnergyThresholdValue(65, -49, 49, 0, 64)

            thr_te90  = tm.registerLvl1Threshold('TE90', 'TE', mapping=0)\
                        .addEnergyThresholdValue(90, -49, 49, 0, 64)
            
            thr_te100 = tm.registerLvl1Threshold('TE100', 'TE', mapping=0)\
                        .addEnergyThresholdValue(100, -49, 49, 0, 64)
            thr_te115 = tm.registerLvl1Threshold('TE115', 'TE', mapping=0)\
                        .addEnergyThresholdValue(115, -49, 49, 0, 64)


            
        thr_te150 = tm.registerLvl1Threshold('TE150', 'TE', mapping=0)\
                    .addEnergyThresholdValue(150, -49, 49, 0, 64)
        thr_te180 = tm.registerLvl1Threshold('TE180', 'TE', mapping=0)\
                    .addEnergyThresholdValue(180, -49, 49, 0, 64)        
        thr_te240 = tm.registerLvl1Threshold('TE240', 'TE', mapping=1)\
                    .addEnergyThresholdValue(240, -49, 49, 0, 64)
        thr_te250 = tm.registerLvl1Threshold('TE250', 'TE', mapping=1)\
                    .addEnergyThresholdValue(250, -49, 49, 0, 64)
        thr_te300 = tm.registerLvl1Threshold('TE300', 'TE', mapping=2)\
                    .addEnergyThresholdValue(300, -49, 49, 0, 64)
        thr_te360 = tm.registerLvl1Threshold('TE360', 'TE', mapping=2)\
                    .addEnergyThresholdValue(360, -49, 49, 0, 64)
        thr_te400 = tm.registerLvl1Threshold('TE400', 'TE', mapping=2)\
                    .addEnergyThresholdValue(400, -49, 49, 0, 64)
        thr_te550 = tm.registerLvl1Threshold('TE550', 'TE', mapping=2)\
                    .addEnergyThresholdValue(550, -49, 49, 0, 64)
        thr_te650 = tm.registerLvl1Threshold('TE650', 'TE', mapping=3)\
                    .addEnergyThresholdValue(650, -49, 49, 0, 64)
        thr_te480 = tm.registerLvl1Threshold('TE480', 'TE', mapping=3)\
                    .addEnergyThresholdValue(480, -49, 49, 0, 64)
        thr_te530 = tm.registerLvl1Threshold('TE530', 'TE', mapping=3)\
                    .addEnergyThresholdValue(530, -49, 49, 0, 64)
        thr_te750 = tm.registerLvl1Threshold('TE750', 'TE', mapping=3)\
                    .addEnergyThresholdValue(750, -49, 49, 0, 64)
        thr_te900 = tm.registerLvl1Threshold('TE900', 'TE', mapping=3)\
                    .addEnergyThresholdValue(900, -49, 49, 0, 64)
        thr_te1000 = tm.registerLvl1Threshold('TE1000', 'TE', mapping=3)\
                    .addEnergyThresholdValue(1000, -49, 49, 0, 64)
        thr_te1200 = tm.registerLvl1Threshold('TE1200', 'TE', mapping=3)\
                     .addEnergyThresholdValue(1200, -49, 49, 0, 64)
        thr_te1400 = tm.registerLvl1Threshold('TE1400', 'TE', mapping=3)\
                     .addEnergyThresholdValue(1400, -49, 49, 0, 64)

        # Other set of TE thresholds for L=10^32 and 10^33
        thr_te500 = tm.registerLvl1Threshold('TE500', 'TE', mapping=-1)\
                    .addEnergyThresholdValue(500, -49, 49, 0, 64)
        thr_te600 = tm.registerLvl1Threshold('TE600', 'TE', mapping=-1)\
                    .addEnergyThresholdValue(600, -49, 49, 0, 64)
        thr_te700 = tm.registerLvl1Threshold('TE700', 'TE', mapping=-1)\
                    .addEnergyThresholdValue(700, -49, 49, 0, 64)
        thr_te800 = tm.registerLvl1Threshold('TE800', 'TE', mapping=-1)\
                    .addEnergyThresholdValue(800, -49, 49, 0, 64)

        # TE thresholds for HI
        thr_te2000 = tm.registerLvl1Threshold('TE2000', 'TE', mapping=3)\
                    .addEnergyThresholdValue(2000, -49, 49, 0, 64)
        thr_te3000 = tm.registerLvl1Threshold('TE3000', 'TE', mapping=3)\
                     .addEnergyThresholdValue(3000, -49, 49, 0, 64)
        thr_te4000 = tm.registerLvl1Threshold('TE4000', 'TE', mapping=3)\
                    .addEnergyThresholdValue(4000, -49, 49, 0, 64)
        thr_te6000 = tm.registerLvl1Threshold('TE6000', 'TE', mapping=3)\
                     .addEnergyThresholdValue(6000, -49, 49, 0, 64)
    
    
        # missing ET thresholds
        thr_xe10 = tm.registerLvl1Threshold('XE10', 'XE', mapping=0)\
                   .addEnergyThresholdValue(10, -49, 49, 0, 64)
        thr_xe15 = tm.registerLvl1Threshold('XE15', 'XE', mapping=0)\
                   .addEnergyThresholdValue(15, -49, 49, 0, 64)
        thr_xe20 = tm.registerLvl1Threshold('XE20', 'XE', mapping=1)\
                   .addEnergyThresholdValue(20, -49, 49, 0, 64)
        thr_xe25 = tm.registerLvl1Threshold('XE25', 'XE', mapping=2)\
                   .addEnergyThresholdValue(25, -49, 49, 0, 64)
        thr_xe30 = tm.registerLvl1Threshold('XE30', 'XE', mapping=3)\
                   .addEnergyThresholdValue(30, -49, 49, 0, 64)
        thr_xe35 = tm.registerLvl1Threshold('XE35', 'XE', mapping=4)\
                   .addEnergyThresholdValue(35, -49, 49, 0, 64)
        thr_xe40 = tm.registerLvl1Threshold('XE40', 'XE', mapping=5)\
                   .addEnergyThresholdValue(40, -49, 49, 0, 64)
        thr_xe50 = tm.registerLvl1Threshold('XE50', 'XE', mapping=6)\
                   .addEnergyThresholdValue(50, -49, 49, 0, 64)
        thr_xe60 = tm.registerLvl1Threshold('XE60', 'XE', mapping=7)\
                   .addEnergyThresholdValue(60, -49, 49, 0, 64)

        # 15, 20, 25, 30, 40, 60, 70, 80
        # 35, 45, 60, 90
        thr_xe70 = tm.registerLvl1Threshold('XE70', 'XE', mapping=-1)\
                   .addEnergyThresholdValue(70, -49, 49, 0, 64)
        thr_xe80 = tm.registerLvl1Threshold('XE80', 'XE', mapping=7)\
                   .addEnergyThresholdValue(80, -49, 49, 0, 64)
        thr_xe45 = tm.registerLvl1Threshold('XE45', 'XE', mapping=-1)\
                   .addEnergyThresholdValue(45, -49, 49, 0, 64)
        thr_xe90 = tm.registerLvl1Threshold('XE90', 'XE', mapping=-1)\
                   .addEnergyThresholdValue(90, -49, 49, 0, 64)
        thr_xe100 = tm.registerLvl1Threshold('XE100', 'XE', mapping=7)\
                   .addEnergyThresholdValue(100, -49, 49, 0, 64)
        thr_xe110 = tm.registerLvl1Threshold('XE110', 'XE', mapping=7)\
                   .addEnergyThresholdValue(110, -49, 49, 0, 64)
        thr_xe120 = tm.registerLvl1Threshold('XE120', 'XE', mapping=7)\
                   .addEnergyThresholdValue(120, -49, 49, 0, 64) 
        thr_xe125 = tm.registerLvl1Threshold('XE125', 'XE', mapping=7)\
                   .addEnergyThresholdValue(125, -49, 49, 0, 64)

        thr_xs15 = tm.registerLvl1Threshold('XS15', 'XS', mapping=0)\
                   .addEnergyThresholdValue(15, -49, 49, 0, 64)
        thr_xs20 = tm.registerLvl1Threshold('XS20', 'XS', mapping=0)\
                   .addEnergyThresholdValue(20, -49, 49, 0, 64)
        thr_xs25 = tm.registerLvl1Threshold('XS25', 'XS', mapping=0)\
                   .addEnergyThresholdValue(25, -49, 49, 0, 64)
        thr_xs30 = tm.registerLvl1Threshold('XS30', 'XS', mapping=0)\
                   .addEnergyThresholdValue(30, -49, 49, 0, 64)
        thr_xs35 = tm.registerLvl1Threshold('XS35', 'XS', mapping=0)\
                   .addEnergyThresholdValue(35, -49, 49, 0, 64)
        thr_xs40 = tm.registerLvl1Threshold('XS40', 'XS', mapping=0)\
                   .addEnergyThresholdValue(40, -49, 49, 0, 64)
        thr_xs45 = tm.registerLvl1Threshold('XS45', 'XS', mapping=0)\
                   .addEnergyThresholdValue(45, -49, 49, 0, 64)
        thr_xs50 = tm.registerLvl1Threshold('XS50', 'XS', mapping=0)\
                   .addEnergyThresholdValue(50, -49, 49, 0, 64)
        thr_xs55 = tm.registerLvl1Threshold('XS55', 'XS', mapping=0)\
                   .addEnergyThresholdValue(55, -49, 49, 0, 64)
        thr_xs60 = tm.registerLvl1Threshold('XS60', 'XS', mapping=0)\
                   .addEnergyThresholdValue(60, -49, 49, 0, 64)
        thr_xs65 = tm.registerLvl1Threshold('XS65', 'XS', mapping=0)\
                   .addEnergyThresholdValue(65, -49, 49, 0, 64)

        thr_nim28 = tm.registerLvl1Threshold('NIM28', 'CALREQ', mapping=0)\
                     .addEnergyThresholdValue(40, -49, 49, 0, 64) # 
        thr_nim29 = tm.registerLvl1Threshold('NIM29', 'CALREQ', mapping=1)\
                     .addEnergyThresholdValue(40, -49, 49, 0, 64) # 
        thr_nim30 = tm.registerLvl1Threshold('NIM30', 'CALREQ', mapping=2)\
                     .addEnergyThresholdValue(40, -49, 49, 0, 64) # 
        if ('InitialBeam_v1' in TriggerFlags.triggerMenuSetup()\
            ):
            thresholdA=[26.7, 15.4, 20, 19.2, 18.3, 20.8, 17.5, 18.3,
                        19.2, 17.5, 16.7, 19.2, 18.3, 18.3, 17.5, 15.0, 30]
            thresholdC=[31.7, 32.5, 27.5, 50.0, 56.3, 25.8, 22.1, 17.1,
                        13.8, 14.2, 14.6, 20.0, 15.8, 20.8, 15.0, 17.5, 30]
        else:
            thresholdA=[32.04, 26.98, 35., 33.54, 32.08, 36.46, 30.63, 32.08,
                        33.54, 30.63, 29.17, 33.54, 32.08, 32.08, 30.63, 26.25, 30.0 ]
            thresholdC=[55.42, 31.98, 32.81, 49.48, 98.44, 32.11, 32.62, 29.9,
                        24.06, 25.81, 25.52, 35, 27.71, 36.46, 26.25, 30.63, 30]
            
        thr_mbtsA0 = tm.registerLvl1Threshold('MBTS_A0', 'MBTSSI', mapping=0)\
                     .addEnergyThresholdValue(thresholdA[0], -49, 49, 0, 64) # mV
        thr_mbtsA1 = tm.registerLvl1Threshold('MBTS_A1', 'MBTSSI', mapping=1)\
                     .addEnergyThresholdValue(thresholdA[1], -49, 49, 0, 64) # mV
        thr_mbtsA2 = tm.registerLvl1Threshold('MBTS_A2', 'MBTSSI', mapping=2)\
                     .addEnergyThresholdValue(thresholdA[2], -49, 49, 0, 64) # mV
        thr_mbtsA3 = tm.registerLvl1Threshold('MBTS_A3', 'MBTSSI', mapping=3)\
                     .addEnergyThresholdValue(thresholdA[3], -49, 49, 0, 64) # mV
        thr_mbtsA4 = tm.registerLvl1Threshold('MBTS_A4', 'MBTSSI', mapping=4)\
                     .addEnergyThresholdValue(thresholdA[4], -49, 49, 0, 64) # mV
        thr_mbtsA5 = tm.registerLvl1Threshold('MBTS_A5', 'MBTSSI', mapping=5)\
                     .addEnergyThresholdValue(thresholdA[5], -49, 49, 0, 64) # mV
        thr_mbtsA6 = tm.registerLvl1Threshold('MBTS_A6', 'MBTSSI', mapping=6)\
                     .addEnergyThresholdValue(thresholdA[6], -49, 49, 0, 64) # mV
        thr_mbtsA7 = tm.registerLvl1Threshold('MBTS_A7', 'MBTSSI', mapping=7)\
                     .addEnergyThresholdValue(thresholdA[7], -49, 49, 0, 64) # mV
        thr_mbtsA8 = tm.registerLvl1Threshold('MBTS_A8', 'MBTSSI', mapping=8)\
                     .addEnergyThresholdValue(thresholdA[8], -49, 49, 0, 64) # mV
        thr_mbtsA9 = tm.registerLvl1Threshold('MBTS_A9', 'MBTSSI', mapping=9)\
                     .addEnergyThresholdValue(thresholdA[9], -49, 49, 0, 64) # mV
        thr_mbtsA10 = tm.registerLvl1Threshold('MBTS_A10', 'MBTSSI', mapping=10)\
                      .addEnergyThresholdValue(thresholdA[10], -49, 49, 0, 64) # mV
        thr_mbtsA11 = tm.registerLvl1Threshold('MBTS_A11', 'MBTSSI', mapping=11)\
                      .addEnergyThresholdValue(thresholdA[11], -49, 49, 0, 64) # mV
        thr_mbtsA12 = tm.registerLvl1Threshold('MBTS_A12', 'MBTSSI', mapping=12)\
                      .addEnergyThresholdValue(thresholdA[12], -49, 49, 0, 64) # mV
        thr_mbtsA13 = tm.registerLvl1Threshold('MBTS_A13', 'MBTSSI', mapping=13)\
                      .addEnergyThresholdValue(thresholdA[13], -49, 49, 0, 64) # mV
        thr_mbtsA14 = tm.registerLvl1Threshold('MBTS_A14', 'MBTSSI', mapping=14)\
                      .addEnergyThresholdValue(thresholdA[14], -49, 49, 0, 64) # mV
        thr_mbtsA15 = tm.registerLvl1Threshold('MBTS_A15', 'MBTSSI', mapping=15)\
                      .addEnergyThresholdValue(thresholdA[15], -49, 49, 0, 64) # mV
        
        thr_mbtsC0 = tm.registerLvl1Threshold('MBTS_C0', 'MBTSSI', mapping=16)\
                     .addEnergyThresholdValue(thresholdC[0], -49, 49, 0, 64) # mV
        thr_mbtsC1 = tm.registerLvl1Threshold('MBTS_C1', 'MBTSSI', mapping=17)\
                     .addEnergyThresholdValue(thresholdC[1], -49, 49, 0, 64) # mV
        thr_mbtsC2 = tm.registerLvl1Threshold('MBTS_C2', 'MBTSSI', mapping=18)\
                     .addEnergyThresholdValue(thresholdC[2], -49, 49, 0, 64) # mV
        thr_mbtsC3 = tm.registerLvl1Threshold('MBTS_C3', 'MBTSSI', mapping=19)\
                     .addEnergyThresholdValue(thresholdC[3], -49, 49, 0, 64) # mV
        thr_mbtsC4 = tm.registerLvl1Threshold('MBTS_C4', 'MBTSSI', mapping=20)\
                     .addEnergyThresholdValue(thresholdC[4], -49, 49, 0, 64) # mV
        thr_mbtsC5 = tm.registerLvl1Threshold('MBTS_C5', 'MBTSSI', mapping=21)\
                     .addEnergyThresholdValue(thresholdC[5], -49, 49, 0, 64) # mV
        thr_mbtsC6 = tm.registerLvl1Threshold('MBTS_C6', 'MBTSSI', mapping=22)\
                     .addEnergyThresholdValue(thresholdC[6], -49, 49, 0, 64) # mV
        thr_mbtsC7 = tm.registerLvl1Threshold('MBTS_C7', 'MBTSSI', mapping=23)\
                     .addEnergyThresholdValue(thresholdC[7], -49, 49, 0, 64) # mV
        thr_mbtsC8 = tm.registerLvl1Threshold('MBTS_C8', 'MBTSSI', mapping=24)\
                     .addEnergyThresholdValue(thresholdC[8], -49, 49, 0, 64) # mV
        thr_mbtsC9 = tm.registerLvl1Threshold('MBTS_C9', 'MBTSSI', mapping=25)\
                     .addEnergyThresholdValue(thresholdC[9], -49, 49, 0, 64) # mV
        thr_mbtsC10 = tm.registerLvl1Threshold('MBTS_C10', 'MBTSSI', mapping=26)\
                     .addEnergyThresholdValue(thresholdC[10], -49, 49, 0, 64) # mV
        thr_mbtsC11 = tm.registerLvl1Threshold('MBTS_C11', 'MBTSSI', mapping=27)\
                     .addEnergyThresholdValue(thresholdC[11], -49, 49, 0, 64) # mV
        thr_mbtsC12 = tm.registerLvl1Threshold('MBTS_C12', 'MBTSSI', mapping=28)\
                     .addEnergyThresholdValue(thresholdC[12], -49, 49, 0, 64) # mV
        thr_mbtsC13 = tm.registerLvl1Threshold('MBTS_C13', 'MBTSSI', mapping=29)\
                     .addEnergyThresholdValue(thresholdC[13], -49, 49, 0, 64) # mV
        thr_mbtsC14 = tm.registerLvl1Threshold('MBTS_C14', 'MBTSSI', mapping=30)\
                     .addEnergyThresholdValue(thresholdC[14], -49, 49, 0, 64) # mV
        thr_mbtsC15 = tm.registerLvl1Threshold('MBTS_C15', 'MBTSSI', mapping=31)\
                     .addEnergyThresholdValue(thresholdC[15], -49, 49, 0, 64) # mV


        thr_mbtsA = tm.registerLvl1Threshold('MBTS_A', 'MBTS', mapping=0)
        thr_mbtsC = tm.registerLvl1Threshold('MBTS_C', 'MBTS', mapping=1)
        for x in xrange(16):
            thr_mbtsA.thresholdValues += [ tm.getLvl1Threshold('MBTS_A%i' % x).thresholdValues[0] ]
            thr_mbtsC.thresholdValues += [ tm.getLvl1Threshold('MBTS_C%i' % x).thresholdValues[0] ]

        thr_trt = tm.registerLvl1Threshold('TRTfast', 'TRT', mapping=0)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) # mV

        thr_zdcA = tm.registerLvl1Threshold('ZDC_A', 'ZDC', mapping=0)\
                      .addEnergyThresholdValue(250, -49, 49, 0, 64) # mV
        thr_zdcC = tm.registerLvl1Threshold('ZDC_C', 'ZDC', mapping=1)\
                      .addEnergyThresholdValue(250, -49, 49, 0, 64) # mV
        thr_zdcAND = tm.registerLvl1Threshold('ZDC_AND', 'ZDC', mapping=2)\
                      .addEnergyThresholdValue(250, -49, 49, 0, 64) # mV


        thr_bcmAtoC = tm.registerLvl1Threshold('BCM_AtoC', 'BCM', mapping=0)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) # mV
        
        thr_bcmCtoA = tm.registerLvl1Threshold('BCM_CtoA', 'BCM', mapping=1)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) # mV

        
        thr_bcmWide = tm.registerLvl1Threshold('BCM_Wide', 'BCM', mapping=2)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) # mV

        thr_bcmA1 = tm.registerLvl1Threshold('BCM_A1', 'BCM', mapping=3)\
                    .addEnergyThresholdValue(40, -49, 49, 0, 64) # mV
        
        thr_bcmA2 = tm.registerLvl1Threshold('BCM_A2', 'BCM', mapping=4)\
                    .addEnergyThresholdValue(40, -49, 49, 0, 64) # mV
        
        thr_bcmC1 = tm.registerLvl1Threshold('BCM_C1', 'BCM', mapping=5)\
                    .addEnergyThresholdValue(40, -49, 49, 0, 64) # mV
        
        thr_bcmC2 = tm.registerLvl1Threshold('BCM_C2', 'BCM', mapping=6)\
                    .addEnergyThresholdValue(40, -49, 49, 0, 64) # mV

        thr_bcmHTA = tm.registerLvl1Threshold('BCM_HTA', 'BCM', mapping=7)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) # mV
        
        thr_bcmHTC = tm.registerLvl1Threshold('BCM_HTC', 'BCM', mapping=8)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) # mV

        thr_bcmComb = tm.registerLvl1Threshold('BCM_Comb', 'BCMCMB', mapping=0)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) # mV
        


        thr_lucidA = tm.registerLvl1Threshold('LUCID_A', 'LUCID', mapping=0)\
                     .addEnergyThresholdValue(650, -49, 49, 0, 64) # mV
        thr_lucidC = tm.registerLvl1Threshold('LUCID_C', 'LUCID', mapping=1)\
                     .addEnergyThresholdValue(650, -49, 49, 0, 64) # mV


        thr_bptx0 = tm.registerLvl1Threshold('BPTX0', 'NIM', mapping=0)\
                    .addEnergyThresholdValue(40, -49, 49, 0, 64) # 
        thr_bptx1 = tm.registerLvl1Threshold('BPTX1', 'NIM', mapping=1)\
                    .addEnergyThresholdValue(40, -49, 49, 0, 64) #
        thr_nimdir2 = tm.registerLvl1Threshold('NIMDIR2', 'NIM', mapping=2)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir3 = tm.registerLvl1Threshold('NIMDIR3', 'NIM', mapping=3)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir4 = tm.registerLvl1Threshold('NIMDIR4', 'NIM', mapping=4)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir5 = tm.registerLvl1Threshold('NIMDIR5', 'NIM', mapping=5)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir6 = tm.registerLvl1Threshold('NIMDIR6', 'NIM', mapping=6)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir7 = tm.registerLvl1Threshold('NIMDIR7', 'NIM', mapping=7)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir8 = tm.registerLvl1Threshold('NIMDIR8', 'NIM', mapping=8)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir9 = tm.registerLvl1Threshold('NIMDIR9', 'NIM', mapping=9)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir10 = tm.registerLvl1Threshold('NIMDIR10', 'NIM', mapping=10)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir11 = tm.registerLvl1Threshold('NIMDIR11', 'NIM', mapping=11)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir12 = tm.registerLvl1Threshold('NIMDIR12', 'NIM', mapping=12)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir13 = tm.registerLvl1Threshold('NIMDIR13', 'NIM', mapping=13)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir14 = tm.registerLvl1Threshold('NIMDIR14', 'NIM', mapping=14)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir15 = tm.registerLvl1Threshold('NIMDIR15', 'NIM', mapping=15)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir16 = tm.registerLvl1Threshold('NIMDIR16', 'NIM', mapping=16)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir17 = tm.registerLvl1Threshold('NIMDIR17', 'NIM', mapping=17)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir18 = tm.registerLvl1Threshold('NIMDIR18', 'NIM', mapping=18)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir19 = tm.registerLvl1Threshold('NIMDIR19', 'NIM', mapping=19)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir20 = tm.registerLvl1Threshold('NIMDIR20', 'NIM', mapping=20)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir21 = tm.registerLvl1Threshold('NIMDIR21', 'NIM', mapping=21)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir22 = tm.registerLvl1Threshold('NIMDIR22', 'NIM', mapping=22)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir23 = tm.registerLvl1Threshold('NIMDIR23', 'NIM', mapping=23)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir24 = tm.registerLvl1Threshold('NIMDIR24', 'NIM', mapping=24)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir25 = tm.registerLvl1Threshold('NIMDIR25', 'NIM', mapping=25)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir26 = tm.registerLvl1Threshold('NIMDIR26', 'NIM', mapping=35)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir27 = tm.registerLvl1Threshold('NIMDIR27', 'NIM', mapping=27)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir28 = tm.registerLvl1Threshold('NIMDIR28', 'NIM', mapping=28)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir29 = tm.registerLvl1Threshold('NIMDIR29', 'NIM', mapping=29)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir30 = tm.registerLvl1Threshold('NIMDIR30', 'NIM', mapping=30)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir31 = tm.registerLvl1Threshold('NIMDIR31', 'NIM', mapping=31)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir32 = tm.registerLvl1Threshold('NIMDIR32', 'NIM', mapping=32)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir33 = tm.registerLvl1Threshold('NIMDIR33', 'NIM', mapping=33)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir34 = tm.registerLvl1Threshold('NIMDIR34', 'NIM', mapping=34)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir35 = tm.registerLvl1Threshold('NIMDIR35', 'NIM', mapping=35)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir36 = tm.registerLvl1Threshold('NIMDIR36', 'NIM', mapping=36)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64)         

##ALFA NIMS
        thr_nimdir_B7R1L = tm.registerLvl1Threshold('NIM_B7R1L', 'NIM', mapping=14)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir_B7R1U = tm.registerLvl1Threshold('NIM_B7R1U', 'NIM', mapping=15)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir_A7R1L = tm.registerLvl1Threshold('NIM_A7R1L', 'NIM', mapping=16)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir_A7R1U = tm.registerLvl1Threshold('NIM_A7R1U', 'NIM', mapping=17)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir_A7L1L = tm.registerLvl1Threshold('NIM_A7L1L', 'NIM', mapping=18)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir_A7L1U = tm.registerLvl1Threshold('NIM_A7L1U', 'NIM', mapping=19)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir_B7L1L = tm.registerLvl1Threshold('NIM_B7L1L', 'NIM', mapping=20)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir_B7L1U = tm.registerLvl1Threshold('NIM_B7L1U', 'NIM', mapping=21)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        thr_nimdir_ALFA_LOG = tm.registerLvl1Threshold('NIM_ALFA_LOG', 'NIM', mapping=22)\
                       .addEnergyThresholdValue(40, -49, 49, 0, 64) 

        thr_nimdir_B7R1L_OD = tm.registerLvl1Threshold('NIM_B7R1L_OD', 'NIM', mapping=26)\
                              .addEnergyThresholdValue(40, -49, 49, 0, 64)
        thr_nimdir_B7R1U_OD = tm.registerLvl1Threshold('NIM_B7R1U_OD', 'NIM', mapping=27)\
                              .addEnergyThresholdValue(40, -49, 49, 0, 64)
        thr_nimdir_A7R1L_OD = tm.registerLvl1Threshold('NIM_A7R1L_OD', 'NIM', mapping=28)\
                              .addEnergyThresholdValue(40, -49, 49, 0, 64)
        thr_nimdir_A7R1U_OD = tm.registerLvl1Threshold('NIM_A7R1U_OD', 'NIM', mapping=29)\
                              .addEnergyThresholdValue(40, -49, 49, 0, 64)
        thr_nimdir_A7L1L_OD = tm.registerLvl1Threshold('NIM_A7L1L_OD', 'NIM', mapping=30)\
                              .addEnergyThresholdValue(40, -49, 49, 0, 64)
        thr_nimdir_A7L1U_OD = tm.registerLvl1Threshold('NIM_A7L1U_OD', 'NIM', mapping=31)\
                              .addEnergyThresholdValue(40, -49, 49, 0, 64)
        thr_nimdir_B7L1L_OD = tm.registerLvl1Threshold('NIM_B7L1L_OD', 'NIM', mapping=32)\
                              .addEnergyThresholdValue(40, -49, 49, 0, 64)
        thr_nimdir_B7L1U_OD = tm.registerLvl1Threshold('NIM_B7L1U_OD', 'NIM', mapping=33)\
                              .addEnergyThresholdValue(40, -49, 49, 0, 64)
        thr_nimdir_ALFA_LOG_OD = tm.registerLvl1Threshold('NIM_ALFA_LOG_OD', 'NIM', mapping=34)\
                                 .addEnergyThresholdValue(40, -49, 49, 0, 64)

        thr_nim_mub = tm.registerLvl1Threshold('NIM_MUB', 'NIM', mapping=23)\
                      .addEnergyThresholdValue(40, -49, 49, 0, 64)

        thr_nim_mue = tm.registerLvl1Threshold('NIM_MUE', 'NIM', mapping=24)\
                      .addEnergyThresholdValue(40, -49 , 49, 0, 64)\
                
        
        #thr_zb_4MBTS_A = tm.registerLvl1Threshold('ZB_4MBTS_A', 'NIM', mapping=24)\
         #                .addEnergyThresholdValue(40, -49, 49, 0, 64) 
        
        # -------------------------------------------------------------------
        # Jet weights
        # ------------
        # Jet thresholds: [5, 10, 18, 23, 35, 42, 70, 100 ][10, 18, 26, 65 ]
        # Jet weights(w):  [ 7, 14, 20, 29, 38, 56, 85, 100 ][9, 6, 23, 19]
        # Jet weights(W):[ 5,  6, 6, 8, 9, 17, 29, 15 ][9, 6, 23, 19]
        # W_i=0.5*(thr_{i+1}-thr_{i-1}),
        #     thr_{0}=0 and thr_{n+1}=thr_{n}+(thr_{n}-thr_{n-1})
        # --------------------------------------------------------------------
        tm.Lvl1CaloInfo().setGlobalScale(1.0)
        tm.Lvl1CaloInfo().setJetWeights([8, 7, 15, 25, 30, 30, 25, 47, 27, 26, 42, 55])
        # tm.Lvl1CaloInfo().setXsParams( xsSigmaScale=1120, xsSigmaOffset=1280, xeMin=11, xeMax=63, teSqrtMin=4, teSqrtMax=63 )
        if ('pp_v4' in TriggerFlags.triggerMenuSetup() ):                        
            tm.Lvl1CaloInfo().setXsParams( xsSigmaScale=1150, xsSigmaOffset=1640, xeMin=11, xeMax=63, teSqrtMin=4, teSqrtMax=63 )
        else:
            tm.Lvl1CaloInfo().setXsParams( xsSigmaScale=1120, xsSigmaOffset=1280, xeMin=11, xeMax=63, teSqrtMin=4, teSqrtMax=63 )
	# ------------------------------------------------------------------
        # LVL1 items
        # ------------------------------------------------------------------

        # Setting up bunch group and BPTX conditions
        bgrpcond      = Logic(bgrp0)&Logic(bgrp1)
        cosmiccond    = Logic(bgrp0)&Logic(bgrp3)
        unpaired1cond = Logic(bgrp0)&Logic(bgrp4) # unpaired beam 1
        unpaired2cond = Logic(bgrp0)&Logic(bgrp5) # unpaired beam 2
        unpairedRcond = Logic(bgrp0)&Logic(bgrp7) # unpaired beams 1 oR beam 2
        empty1stcond  = Logic(bgrp0)&Logic(bgrp6)
        bptxcond      = Logic(bgrp0)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))
        physcond      = bgrpcond

        #bgrp2cond     = Logic(bgrp0)&(Logic(bgrp1)|Logic(bgrp2)|Logic(bgrp3)|Logic(bgrp4)|Logic(bgrp5)|Logic(bgrp6))

        NoColCond      = Logic(bgrp0)&Not(Logic(bgrp1))&Not(Logic(bgrp6))&Not(Logic(bgrp7))&Not(Logic(bgrp2))

        ## New in the Physics_pp - 30August2010
        unpaired_isocond    = Logic(bgrp0)&Logic(bgrp4) # unpaired isolated (satellite bunches)
        unpaired_nonisocond = Logic(bgrp0)&Logic(bgrp5) # unpaired non-isolated (parasitic bunches)

        ## New in Physics_pp_v2 - 24March2010
        bgrp7cond       = Logic(bgrp0)&Logic(bgrp7) # No unpaired anymore


        if ('Cosmic2009_v2' in TriggerFlags.triggerMenuSetup()\
            or 'InitialBeam_v1' in TriggerFlags.triggerMenuSetup()\
            or 'Cosmic2009_simpleL1Calib' in TriggerFlags.triggerMenuSetup()\
            or 'Cosmic2009_inclMuons' in TriggerFlags.triggerMenuSetup()\
            ):
            physcond = bptxcond
        
        # E/gamma ctpid=[0:0x1f]
        item_em2   = LVL1MenuItem('L1_EM2',   ctpid=0x0, group='1')\
                     .setLogic(thr_em2.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em3   = LVL1MenuItem('L1_EM3',   ctpid=0x1, group='1')\
                     .setLogic(thr_em3.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em6   = LVL1MenuItem('L1_EM6',   ctpid=-1, group='1')\
                     .setLogic(thr_em6.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em7   = LVL1MenuItem('L1_EM7',   ctpid=0x1, group='1')\
                     .setLogic(thr_em7.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em8   = LVL1MenuItem('L1_EM8',   ctpid=-1, group='1')\
                     .setLogic(thr_em8.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em10  = LVL1MenuItem('L1_EM10',  ctpid=0x2, group='1')\
                     .setLogic(thr_em10.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em10v  = LVL1MenuItem('L1_EM10V',  ctpid=-1, group='1')\
                      .setLogic(thr_em10v.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em10h  = LVL1MenuItem('L1_EM10H',  ctpid=-1, group='1')\
                      .setLogic(thr_em10h.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em10vh = LVL1MenuItem('L1_EM10VH',  ctpid=-1, group='1')\
                       .setLogic(thr_em10vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em11h  = LVL1MenuItem('L1_EM11H',  ctpid=-1, group='1')\
                     .setLogic(thr_em11h.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em12  = LVL1MenuItem('L1_EM12',  ctpid=0x2, group='1')\
                     .setLogic(thr_em12.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em13  = LVL1MenuItem('L1_EM13',  ctpid=0x3, group='1')\
                     .setLogic(thr_em13.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em13i = LVL1MenuItem('L1_EM13I', ctpid=0x4, group='1')\
                     .setLogic(thr_em13i.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em15h  = LVL1MenuItem('L1_EM15H',  ctpid=-1, group='1')\
                      .setLogic(thr_em15h.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em14vh  = LVL1MenuItem('L1_EM14VH',  ctpid=-1, group='1')\
                      .setLogic(thr_em14vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em18  = LVL1MenuItem('L1_EM18',  ctpid=0x5, group='1')\
                     .setLogic(thr_em18.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em18i = LVL1MenuItem('L1_EM18I', ctpid=0x6, group='1')\
                     .setLogic(thr_em18i.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em18vh = LVL1MenuItem('L1_EM18VH',  ctpid=-1, group='1')\
                      .setLogic(thr_em18vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em18h  = LVL1MenuItem('L1_EM18H',  ctpid=-1, group='1')\
                      .setLogic(thr_em18h.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em23i = LVL1MenuItem('L1_EM23I', ctpid=0x7, group='1')\
                     .setLogic(thr_em23i.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em20h  = LVL1MenuItem('L1_EM20H',  ctpid=-1, group='1')\
                      .setLogic(thr_em20h.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em31 = LVL1MenuItem('L1_EM31', ctpid=-1, group='1')\
                   .setLogic(thr_em31.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em100 = LVL1MenuItem('L1_EM100', ctpid=0x8, group='1')\
                     .setLogic(thr_em100.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em50 = LVL1MenuItem('L1_EM50', ctpid=0x9, group='1')\
                    .setLogic(thr_em50.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)

        # EM
        # - Add/remap new L1 items:
        #   EM7->EM5 EM13->EM10 EM13I->EM10I EM18->EM14 EM18I->EM14I EM23I->EM19I EM100->EM85
        # - For these new items use the same CTP id as item being replaced
        #   with exception of EM10 which already exists - it keeps the same CTP id=
        #
        item_em5   = LVL1MenuItem('L1_EM5',   ctpid=-1, group='1')\
                     .setLogic(thr_em5.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em10i = LVL1MenuItem('L1_EM10I', ctpid=0x4, group='1')\
                     .setLogic(thr_em10i.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em14  = LVL1MenuItem('L1_EM14',  ctpid=0x5, group='1')\
                     .setLogic(thr_em14.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em16  = LVL1MenuItem('L1_EM16',  ctpid=0x5, group='1')\
                     .setLogic(thr_em16.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em16vh = LVL1MenuItem('L1_EM16VH',  ctpid=-1, group='1')\
                      .setLogic(thr_em16vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em16h  = LVL1MenuItem('L1_EM16H',  ctpid=-1, group='1')\
                      .setLogic(thr_em16h.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em16v  = LVL1MenuItem('L1_EM16V',  ctpid=-1, group='1')\
                      .setLogic(thr_em16v.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em14i = LVL1MenuItem('L1_EM14I', ctpid=0x6, group='1')\
                     .setLogic(thr_em14i.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em19i = LVL1MenuItem('L1_EM19I', ctpid=0x7, group='1')\
                     .setLogic(thr_em19i.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em30  = LVL1MenuItem('L1_EM30', ctpid=0x8, group='1')\
                     .setLogic(thr_em30.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em35  = LVL1MenuItem('L1_EM35', ctpid=0x8, group='1')\
                     .setLogic(thr_em35.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em85  = LVL1MenuItem('L1_EM85', ctpid=0x8, group='1')\
                     .setLogic(thr_em85.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)

        item_em30_bgrp7  = LVL1MenuItem('L1_EM30_BGRP7', ctpid=-1, group='1')\
                                         .setLogic(thr_em30.condition(1)&bgrp7cond).setTriggerType(TT_calo|TT_caloem)
        
        item_em2_empty = LVL1MenuItem('L1_EM2_EMPTY',ctpid=-1, group='1')\
                         .setLogic(thr_em2.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_em3_empty = LVL1MenuItem('L1_EM3_EMPTY',ctpid=-1, group='1')\
                         .setLogic(thr_em3.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_em5_empty = LVL1MenuItem('L1_EM5_EMPTY',ctpid=-1, group='1')\
                         .setLogic(thr_em5.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)        
        item_em6_empty = LVL1MenuItem('L1_EM6_EMPTY',ctpid=-1, group='1')\
                         .setLogic(thr_em6.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_em4_empty = LVL1MenuItem('L1_EM4_EMPTY',ctpid=-1, group='1')\
                         .setLogic(thr_em4.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_em7_empty = LVL1MenuItem('L1_EM7_EMPTY',ctpid=-1, group='1')\
                         .setLogic(thr_em7.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_em5i_empty = LVL1MenuItem('L1_EM5I_EMPTY',ctpid=-1, group='1')\
                         .setLogic(thr_em5i.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)        
        item_em7i_empty = LVL1MenuItem('L1_EM7I_EMPTY',ctpid=-1, group='1')\
                         .setLogic(thr_em7i.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_em10_empty = LVL1MenuItem('L1_EM10_EMPTY',ctpid=-1, group='1')\
                         .setLogic(thr_em10.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_em10i_empty = LVL1MenuItem('L1_EM10I_EMPTY',ctpid=-1, group='1')\
                          .setLogic(thr_em10i.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_em13_empty = LVL1MenuItem('L1_EM13_EMPTY',ctpid=-1, group='1')\
                         .setLogic(thr_em13.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_em13i_empty = LVL1MenuItem('L1_EM13I_EMPTY',ctpid=-1, group='1')\
                          .setLogic(thr_em13i.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_em14_empty = LVL1MenuItem('L1_EM14_EMPTY',ctpid=-1, group='1')\
                         .setLogic(thr_em14.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)        
        item_em18_empty = LVL1MenuItem('L1_EM18_EMPTY',ctpid=-1, group='1')\
                         .setLogic(thr_em18.condition(1)&cosmiccond).setTriggerType(TT_calo|TT_caloem)

        item_em3_bgrp1   = LVL1MenuItem('L1_EM3_BGRP1', ctpid=-1, group='1')\
                           .setLogic(thr_em3.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_calo|TT_caloem)
        item_em3_bgrp3   = LVL1MenuItem('L1_EM3_BGRP3',   ctpid=-1, group='1')\
                           .setLogic(thr_em3.condition(1)&Logic(bgrp0)&Logic(bgrp3)).setTriggerType(TT_calo|TT_caloem)
        item_em3_bgrp1_bptx   = LVL1MenuItem('L1_EM3_BGRP1_BPTX',   ctpid=-1, group='1')\
                                .setLogic(thr_em3.condition(1)&Logic(bgrp0)&Logic(bgrp1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))).setTriggerType(TT_calo|TT_caloem)

        item_2em2   = LVL1MenuItem('L1_2EM2',   ctpid=-1).setLogic(thr_em2.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)        
        item_2em3   = LVL1MenuItem('L1_2EM3',   ctpid=0xa).setLogic(thr_em3.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em5   = LVL1MenuItem('L1_2EM5',   ctpid=0xb).setLogic(thr_em5.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)        
        item_2em6   = LVL1MenuItem('L1_2EM6',   ctpid=-1).setLogic(thr_em6.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em5i  = LVL1MenuItem('L1_2EM5I',  ctpid=-1).setLogic(thr_em5i.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)        
        item_2em7   = LVL1MenuItem('L1_2EM7',   ctpid=0xb).setLogic(thr_em7.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em8   = LVL1MenuItem('L1_2EM8',   ctpid=-1).setLogic(thr_em8.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em10  = LVL1MenuItem('L1_2EM10',  ctpid=0xc).setLogic(thr_em10.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em10i = LVL1MenuItem('L1_2EM10I', ctpid=0xe).setLogic(thr_em10i.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em10vh= LVL1MenuItem('L1_2EM10VH', ctpid=-1).setLogic(thr_em10vh.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em10h=  LVL1MenuItem('L1_2EM10H', ctpid=-1).setLogic(thr_em10h.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em11h=  LVL1MenuItem('L1_2EM11H', ctpid=-1).setLogic(thr_em11h.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em12  = LVL1MenuItem('L1_2EM12',  ctpid=0xf).setLogic(thr_em12.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em13  = LVL1MenuItem('L1_2EM13',  ctpid=0xd).setLogic(thr_em13.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em13i = LVL1MenuItem('L1_2EM13I', ctpid=0xe).setLogic(thr_em13i.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em14  = LVL1MenuItem('L1_2EM14',  ctpid=0xf).setLogic(thr_em14.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em14i = LVL1MenuItem('L1_2EM14I', ctpid=0x10).setLogic(thr_em14i.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)        
        item_2em18  = LVL1MenuItem('L1_2EM18',  ctpid=0xf).setLogic(thr_em18.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em18i = LVL1MenuItem('L1_2EM18I', ctpid=0x10).setLogic(thr_em18i.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em19i = LVL1MenuItem('L1_2EM19I', ctpid=0x11).setLogic(thr_em19i.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em23i = LVL1MenuItem('L1_2EM23I', ctpid=0x11).setLogic(thr_em23i.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)        

        item_2em3_em5 = LVL1MenuItem('L1_2EM3_EM5', ctpid=-1).setLogic(thr_em3.condition(2)&thr_em5.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em3_em6 = LVL1MenuItem('L1_2EM3_EM6', ctpid=-1).setLogic(thr_em3.condition(2)&thr_em6.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em3_em7 = LVL1MenuItem('L1_2EM3_EM7', ctpid=-1).setLogic(thr_em3.condition(2)&thr_em7.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em3_em8 = LVL1MenuItem('L1_2EM3_EM8', ctpid=-1).setLogic(thr_em3.condition(2)&thr_em8.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em3_em10 = LVL1MenuItem('L1_2EM3_EM10', ctpid=-1).setLogic(thr_em3.condition(2)&thr_em10.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em3_em12 = LVL1MenuItem('L1_2EM3_EM12', ctpid=-1).setLogic(thr_em3.condition(2)&thr_em12.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em3_em13 = LVL1MenuItem('L1_2EM3_EM13', ctpid=-1).setLogic(thr_em3.condition(2)&thr_em13.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em5_em10 = LVL1MenuItem('L1_2EM5_EM10', ctpid=-1).setLogic(thr_em5.condition(2)&thr_em10.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em5_em10vh = LVL1MenuItem('L1_2EM5_EM10VH', ctpid=-1).setLogic(thr_em5.condition(2)&thr_em10vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em5_em16h  = LVL1MenuItem('L1_2EM5_EM16H', ctpid=-1).setLogic(thr_em5.condition(2)&thr_em16h.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em6_em18h  = LVL1MenuItem('L1_2EM6_EM18H', ctpid=-1).setLogic(thr_em6.condition(2)&thr_em18h.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em5_em16vh = LVL1MenuItem('L1_2EM5_EM16VH', ctpid=-1).setLogic(thr_em5.condition(2)&thr_em16vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em6_em16vh = LVL1MenuItem('L1_2EM6_EM16VH', ctpid=-1).setLogic(thr_em6.condition(2)&thr_em16vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_3em5_em10 = LVL1MenuItem('L1_3EM5_EM10', ctpid=-1).setLogic(thr_em5.condition(3)&thr_em10.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_3em5_em10vh = LVL1MenuItem('L1_3EM5_EM10VH', ctpid=-1).setLogic(thr_em5.condition(3)&thr_em10vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_3em5_em10h = LVL1MenuItem('L1_3EM5_EM10H', ctpid=-1).setLogic(thr_em5.condition(3)&thr_em10h.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_3em6_em10vh = LVL1MenuItem('L1_3EM6_EM10VH', ctpid=-1).setLogic(thr_em6.condition(3)&thr_em10vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_3em6_em14vh = LVL1MenuItem('L1_3EM6_EM14VH', ctpid=-1).setLogic(thr_em6.condition(3)&thr_em14vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_3em6_em11h = LVL1MenuItem('L1_3EM6_EM11H', ctpid=-1).setLogic(thr_em6.condition(3)&thr_em11h.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em5_em12 = LVL1MenuItem('L1_2EM5_EM12', ctpid=-1).setLogic(thr_em5.condition(2)&thr_em12.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em7_em10 = LVL1MenuItem('L1_2EM7_EM10', ctpid=-1).setLogic(thr_em7.condition(2)&thr_em10.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em7_em10vh = LVL1MenuItem('L1_2EM7_EM10VH', ctpid=-1).setLogic(thr_em7.condition(2)&thr_em10vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em12_em16v = LVL1MenuItem('L1_2EM12_EM16V', ctpid=-1).setLogic(thr_em12.condition(2)&thr_em16v.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)

        item_3em5   = LVL1MenuItem('L1_3EM5', ctpid=0x13).setLogic(thr_em5.condition(3)&physcond).setTriggerType(TT_phys).setTriggerType(TT_calo|TT_caloem)
        item_3em6   = LVL1MenuItem('L1_3EM6', ctpid=-1).setLogic(thr_em6.condition(3)&physcond).setTriggerType(TT_phys).setTriggerType(TT_calo|TT_caloem)
        item_3em7   = LVL1MenuItem('L1_3EM7', ctpid=0x13).setLogic(thr_em7.condition(3)&physcond).setTriggerType(TT_phys).setTriggerType(TT_calo|TT_caloem)
        item_3em10   = LVL1MenuItem('L1_3EM10', ctpid=0x14).setLogic(thr_em10.condition(3)&physcond).setTriggerType(TT_phys).setTriggerType(TT_calo|TT_caloem)
        item_3em13   = LVL1MenuItem('L1_3EM13', ctpid=0x14).setLogic(thr_em13.condition(3)&physcond).setTriggerType(TT_phys).setTriggerType(TT_calo|TT_caloem)

        item_2em3_empty   = LVL1MenuItem('L1_2EM3_EMPTY',  ctpid=-1).setLogic(thr_em3.condition(2)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_2em4_empty   = LVL1MenuItem('L1_2EM4_EMPTY',  ctpid=-1).setLogic(thr_em4.condition(2)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_2em5_empty   = LVL1MenuItem('L1_2EM5_EMPTY',  ctpid=-1).setLogic(thr_em5.condition(2)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_2em5_empty   = LVL1MenuItem('L1_2EM5_EMPTY',  ctpid=-1).setLogic(thr_em5.condition(2)&cosmiccond).setTriggerType(TT_calo|TT_caloem)        
        item_2em5i_empty  = LVL1MenuItem('L1_2EM5I_EMPTY', ctpid=-1).setLogic(thr_em5i.condition(2)&cosmiccond).setTriggerType(TT_calo|TT_caloem)        
        item_2em7_empty   = LVL1MenuItem('L1_2EM7_EMPTY',  ctpid=-1).setLogic(thr_em7.condition(2)&cosmiccond).setTriggerType(TT_calo|TT_caloem)        
        item_2em7i_empty  = LVL1MenuItem('L1_2EM7I_EMPTY', ctpid=-1).setLogic(thr_em7i.condition(2)&cosmiccond).setTriggerType(TT_calo|TT_caloem)
        item_2em10_empty  = LVL1MenuItem('L1_2EM10_EMPTY', ctpid=-1).setLogic(thr_em10.condition(2)&cosmiccond).setTriggerType(TT_calo|TT_caloem)

        # Transient items that don't need constant CTPID
        item_em3_bptx   = LVL1MenuItem('L1_EM3_BPTX',   ctpid=-1, group='1')\
                          .setLogic(thr_em3.condition(1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))&physcond)\
                          .setTriggerType(TT_calo|TT_caloem)
        
        item_em4   = LVL1MenuItem('L1_EM4',   ctpid=0x54, group='1').setLogic(thr_em4.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em23  = LVL1MenuItem('L1_EM23',  ctpid=-1,   group='1').setLogic(thr_em23.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em5i  = LVL1MenuItem('L1_EM5I',  ctpid=0x55, group='1').setLogic(thr_em5i.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em7i  = LVL1MenuItem('L1_EM7I',  ctpid=0x55, group='1').setLogic(thr_em7i.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em4  = LVL1MenuItem('L1_2EM4',  ctpid=0x56).setLogic(thr_em4.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em7i = LVL1MenuItem('L1_2EM7I', ctpid=0x57).setLogic(thr_em7i.condition(2)&physcond).setTriggerType(TT_calo|TT_caloem)
        
        # MUON ctpid=[0x20;0x2f]
        item_mu0  = LVL1MenuItem('L1_MU0',  ctpid=0x20, group='1')\
                    .setLogic(thr_mu0.condition(1)&physcond)\
                    .setTriggerType(TT_rpcin)
        item_mu4  = LVL1MenuItem('L1_MU4',  ctpid=-1, group='1')\
                    .setLogic(thr_mu4.condition(1)&physcond)\
                    .setTriggerType(TT_rpcin)
        item_mu6  = LVL1MenuItem('L1_MU6',  ctpid=0x21, group='1')\
                    .setLogic(thr_mu6.condition(1)&physcond)\
                    .setTriggerType(TT_rpcin)
        item_mu10 = LVL1MenuItem('L1_MU10', ctpid=0x22, group='1')\
                    .setLogic(thr_mu10.condition(1)&physcond)\
                    .setTriggerType(TT_rpcin)
        item_mu11 = LVL1MenuItem('L1_MU11', ctpid=0x23, group='1')\
                    .setLogic(thr_mu11.condition(1)&physcond)\
                    .setTriggerType(TT_rpcin)
        item_mu20 = LVL1MenuItem('L1_MU20', ctpid=0x24, group='1')\
                    .setLogic(thr_mu20.condition(1)&physcond)\
                    .setTriggerType(TT_rpcin)
        item_mu40 = LVL1MenuItem('L1_MU40', ctpid=0x25, group='1')\
                    .setLogic(thr_mu40.condition(1)&physcond)\
                    .setTriggerType(TT_rpcin)

        item_mub  = LVL1MenuItem('L1_MUB',  ctpid=-1, group='1')\
                    .setLogic(thr_nim_mub.condition(1)&physcond)\
                    .setTriggerType(TT_rpcin)
        item_mue  = LVL1MenuItem('L1_MUE',  ctpid=-1, group='1')\
                    .setLogic(thr_nim_mue.condition(1)&physcond)\
                    .setTriggerType(TT_rpcin)


        item_2mu4_barrelonly  = LVL1MenuItem('L1_2MU4_BARRELONLY',  ctpid=-1, group='1', prescale=1)\
                                .setLogic( (thr_mu4.condition(2)&Not(thr_nim_mue.condition(1))|thr_mu4.condition(3))&physcond)\
                                .setTriggerType(TT_rpcin)

        item_2mu4_mu6_barrel  = LVL1MenuItem('L1_2MU4_MU6_BARREL',  ctpid=-1, group='1', prescale=1)\
                                .setLogic( (thr_mu4.condition(2)&thr_nim_mub.condition(1)&thr_mu6.condition(1))&physcond)\
                                .setTriggerType(TT_rpcin)

        item_2mu4_barrel  = LVL1MenuItem('L1_2MU4_BARREL',  ctpid=-1, group='1', prescale=1)\
                            .setLogic( (thr_mu4.condition(2)&thr_nim_mub.condition(1))&physcond)\
                            .setTriggerType(TT_rpcin)

        item_mu0_comm = LVL1MenuItem('L1_MU0_COMM', ctpid=-1, group='1')\
                        .setLogic(thr_mu0_comm.condition(1)&physcond)\
                        .setTriggerType(TT_rpcin)

        item_mu0_comm_empty = LVL1MenuItem('L1_MU0_COMM_EMPTY', ctpid=-1, group='1')\
                              .setLogic(thr_mu0_comm.condition(1)&cosmiccond)\
                              .setTriggerType(TT_rpcout)


        item_mu0_empty  = LVL1MenuItem('L1_MU0_EMPTY',  ctpid=-1, group='1')\
                    .setLogic(thr_mu0.condition(1)&cosmiccond)\
                    .setTriggerType(TT_rpcout)
        item_mu4_empty  = LVL1MenuItem('L1_MU4_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_mu4.condition(1)&cosmiccond)\
                          .setTriggerType(TT_rpcout)
        
        item_mu20_firstempty  = LVL1MenuItem('L1_MU20_FIRSTEMPTY',  ctpid=-1, group='1')\
                                .setLogic(thr_mu20.condition(1)&empty1stcond)\
                                .setTriggerType(TT_rpcout)

        item_2mu6_empty  = LVL1MenuItem('L1_2MU6_EMPTY',  ctpid=-1, group='1')\
                    .setLogic(thr_mu6.condition(2)&cosmiccond)\
                    .setTriggerType(TT_rpcout)

        item_mu0_j15_empty  = LVL1MenuItem('L1_MU0_J15_EMPTY',  ctpid=-1, group='1')\
                    .setLogic(thr_mu0.condition(1)&thr_j15.condition(1)&cosmiccond)\
                    .setTriggerType(TT_rpcout)

        item_mu4_j15_empty  = LVL1MenuItem('L1_MU4_J15_EMPTY',  ctpid=-1, group='1')\
                              .setLogic(thr_mu4.condition(1)&thr_j15.condition(1)&cosmiccond)\
                              .setTriggerType(TT_rpcout)

        item_mu6_empty  = LVL1MenuItem('L1_MU6_EMPTY',  ctpid=-1, group='1')\
                    .setLogic(thr_mu6.condition(1)&cosmiccond)\
                    .setTriggerType(TT_rpcout)
        item_mu15_empty  = LVL1MenuItem('L1_MU15_EMPTY',  ctpid=-1, group='1')\
                    .setLogic(thr_mu15.condition(1)&cosmiccond)\
                    .setTriggerType(TT_rpcout)
        
        item_mu10_empty  = LVL1MenuItem('L1_MU10_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_mu10.condition(1)&cosmiccond)\
                          .setTriggerType(TT_rpcout)
        item_mu20_empty  = LVL1MenuItem('L1_MU20_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_mu20.condition(1)&cosmiccond)\
                          .setTriggerType(TT_rpcout)
        
        item_mu0_tgc_halo  = LVL1MenuItem('L1_MU0_TGC_HALO',  ctpid=0x58, group='1', prescale=1)\
                             .setLogic(thr_mu0_tgc_halo.condition(1)&physcond)\
                             .setTriggerType(TT_tgc)
        item_mu0_low_rpc  = LVL1MenuItem('L1_MU0_LOW_RPC',  ctpid=0x59, group='1', prescale=1)\
                            .setLogic(thr_mu0_low_rpc.condition(1)&physcond)\
                            .setTriggerType(TT_rpcin)
        item_mu6_rpc = LVL1MenuItem('L1_MU6_RPC', ctpid=0x5a, group='1', prescale=1)\
                       .setLogic(thr_mu6_rpc.condition(1)&physcond)\
                       .setTriggerType(TT_rpcin)
        item_mu0_tgc = LVL1MenuItem('L1_MU0_TGC', ctpid=0x5b, group='1', prescale=1)\
                       .setLogic(thr_mu0_tgc.condition(1)&physcond)\
                       .setTriggerType(TT_tgc)
        item_mu6_tgc = LVL1MenuItem('L1_MU6_TGC', ctpid=0x5c, group='1', prescale=1)\
                       .setLogic(thr_mu6_tgc.condition(1)&physcond)\
                       .setTriggerType(TT_tgc)
        item_mu0_high_rpc = LVL1MenuItem('L1_MU0_HIGH_RPC', ctpid=0x5d, group='1', prescale=1)\
                            .setLogic(thr_mu0_high_rpc.condition(1)&physcond)\
                            .setTriggerType(TT_rpcin)



        item_mu0_low = LVL1MenuItem('L1_MU0_LOW', ctpid=0x73, group='1', prescale=1)\
                       .setLogic((thr_mu0_low_rpc.condition(1)|thr_mu0_tgc.condition(1))&physcond)\
                       .setTriggerType(0)



        item_mu0_tgc_halo_bptx  = LVL1MenuItem('L1_MU0_TGC_HALO_BPTX',  ctpid=0x9a, group='1', prescale=1)\
                                  .setLogic(thr_mu0_tgc_halo.condition(1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))&physcond)\
                                  .setTriggerType(TT_tgc)
        item_mu0_low_rpc_bptx  = LVL1MenuItem('L1_MU0_LOW_RPC_BPTX',  ctpid=0x9b, group='1', prescale=1)\
                                 .setLogic(thr_mu0_low_rpc.condition(1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))&physcond)\
                                 .setTriggerType(TT_rpcin)
        item_mu6_rpc_bptx = LVL1MenuItem('L1_MU6_RPC_BPTX', ctpid=0x9c, group='1', prescale=1)\
                            .setLogic(thr_mu6_rpc.condition(1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))&physcond)\
                            .setTriggerType(TT_rpcin)
        item_mu0_tgc_bptx = LVL1MenuItem('L1_MU0_TGC_BPTX', ctpid=0x9d, group='1', prescale=1)\
                            .setLogic(thr_mu0_tgc.condition(1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))&physcond)\
                            .setTriggerType(TT_tgc)
        item_mu6_tgc_bptx = LVL1MenuItem('L1_MU6_TGC_BPTX', ctpid=0x9e, group='1', prescale=1)\
                            .setLogic(thr_mu6_tgc.condition(1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))&physcond)\
                            .setTriggerType(TT_tgc)
        item_mu0_high_rpc_bptx = LVL1MenuItem('L1_MU0_HIGH_RPC_BPTX', ctpid=0x9f, group='1', prescale=1)\
                                 .setLogic(thr_mu0_high_rpc.condition(1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))&physcond)\
                                 .setTriggerType(TT_rpcin)
        

        item_mu0_tgc_halo_empty  = LVL1MenuItem('L1_MU0_TGC_HALO_EMPTY',  ctpid=-1, group='1', prescale=1)\
                                   .setLogic(thr_mu0_tgc_halo.condition(1)&cosmiccond)\
                                   .setTriggerType(TT_rpcout)
        item_mu0_low_rpc_empty  = LVL1MenuItem('L1_MU0_LOW_RPC_EMPTY',  ctpid=-1, group='1', prescale=1)\
                                  .setLogic(thr_mu0_low_rpc.condition(1)&cosmiccond)\
                                  .setTriggerType(TT_rpcout)
        item_mu0_tgc_empty = LVL1MenuItem('L1_MU0_TGC_EMPTY', ctpid=-1, group='1', prescale=1)\
                             .setLogic(thr_mu0_tgc.condition(1)&cosmiccond)\
                             .setTriggerType(TT_rpcout)
        item_mu0_high_rpc_empty = LVL1MenuItem('L1_MU0_HIGH_RPC_EMPTY', ctpid=-1, group='1', prescale=1)\
                                  .setLogic(thr_mu0_high_rpc.condition(1)&cosmiccond)\
                                  .setTriggerType(TT_rpcout)
        
        item_2mu0  = LVL1MenuItem('L1_2MU0',  ctpid=0x26).setLogic(thr_mu0.condition(2)&physcond)\
                     .setTriggerType(TT_rpcin)
        item_2mu4  = LVL1MenuItem('L1_2MU4',  ctpid=-1).setLogic(thr_mu4.condition(2)&physcond).setTriggerType(TT_rpcin)
        item_2mu6  = LVL1MenuItem('L1_2MU6',  ctpid=0x27).setLogic(thr_mu6.condition(2)&physcond).setTriggerType(TT_rpcin)
        item_2mu10 = LVL1MenuItem('L1_2MU10', ctpid=0x28).setLogic(thr_mu10.condition(2)&physcond).setTriggerType(TT_rpcin)
        item_2mu11 = LVL1MenuItem('L1_2MU11', ctpid=0x28).setLogic(thr_mu11.condition(2)&physcond).setTriggerType(TT_rpcin)
        item_2mu15 = LVL1MenuItem('L1_2MU15', ctpid=0x29).setLogic(thr_mu15.condition(2)&physcond)
        item_2mu20 = LVL1MenuItem('L1_2MU20', ctpid=0x2a).setLogic(thr_mu20.condition(2)&physcond)
        item_3mu0  = LVL1MenuItem('L1_3MU0',  ctpid=-1).setLogic(thr_mu0.condition(3)&physcond)
        item_3mu4  = LVL1MenuItem('L1_3MU4',  ctpid=-1).setLogic(thr_mu4.condition(3)&physcond)
        item_3mu6  = LVL1MenuItem('L1_3MU6',  ctpid=0x2b).setLogic(thr_mu6.condition(3)&physcond)
        item_4mu0  = LVL1MenuItem('L1_4MU0',  ctpid=-1).setLogic(thr_mu0.condition(4)&physcond)
        item_2mu0_mu6 = LVL1MenuItem('L1_2MU0_MU6', ctpid=0x2c)\
                        .setLogic(thr_mu0.condition(2)&thr_mu6.condition(1)&physcond)\
                        .setTriggerType(TT_rpcin)
        item_2mu4_mu6 = LVL1MenuItem('L1_2MU4_MU6', ctpid=-1)\
                        .setLogic(thr_mu4.condition(2)&thr_mu6.condition(1)&physcond)\
                        .setTriggerType(TT_rpcin)
        item_mu0_mu6 = LVL1MenuItem('L1_MU0_MU6', ctpid=0x2c)\
                        .setLogic(thr_mu0.condition(1)&thr_mu6.condition(1)&physcond)\
                        .setTriggerType(TT_rpcin)
        item_2mu0_empty  = LVL1MenuItem('L1_2MU0_EMPTY',  ctpid=-1).setLogic(thr_mu0.condition(2)&cosmiccond).setTriggerType(TT_rpcout)
        item_2mu4_empty  = LVL1MenuItem('L1_2MU4_EMPTY',  ctpid=-1).setLogic(thr_mu4.condition(2)&cosmiccond).setTriggerType(TT_rpcout)
        item_mu11_empty  = LVL1MenuItem('L1_MU11_EMPTY',  ctpid=-1).setLogic(thr_mu11.condition(1)&cosmiccond).setTriggerType(TT_rpcout)

        item_2mu4_xe40 = LVL1MenuItem('L1_2MU4_XE40', ctpid=-1)\
                         .setLogic(thr_mu4.condition(2)&thr_xe40.condition(1)&physcond).setTriggerType(TT_calo|TT_rpcin)
        item_2mu4_xe30 = LVL1MenuItem('L1_2MU4_XE30', ctpid=-1)\
                         .setLogic(thr_mu4.condition(2)&thr_xe30.condition(1)&physcond).setTriggerType(TT_calo|TT_rpcin)


        item_2mu6_barrelonly  = LVL1MenuItem('L1_2MU6_BARRELONLY',  ctpid=-1, group='1', prescale=1)\
                                .setLogic( (thr_mu6.condition(2)&Not(thr_nim_mue.condition(1))|thr_mu6.condition(3))&physcond)\
                                .setTriggerType(TT_rpcin)
        
        item_2mu6_barrel  = LVL1MenuItem('L1_2MU6_BARREL',  ctpid=-1, group='1', prescale=1)\
                            .setLogic( (thr_mu6.condition(2)&thr_nim_mub.condition(1))&physcond)\
                            .setTriggerType(TT_rpcin)

        item_2mu6_mu10  = LVL1MenuItem('L1_2MU6_MU10',  ctpid=-1, group='1', prescale=1)\
                                .setLogic( (thr_mu6.condition(2)&thr_mu10.condition(1))&physcond)\
                                .setTriggerType(TT_rpcin)

        item_3mu4_mu6  = LVL1MenuItem('L1_3MU4_MU6',  ctpid=-1,group='1', prescale=1)\
                         .setLogic( (thr_mu4.condition(3)&thr_mu6.condition(1))&physcond)\
                         .setTriggerType(TT_rpcin)



        # Transient items
        item_mu15 = LVL1MenuItem('L1_MU15', ctpid=-1, group='1')\
                    .setLogic(thr_mu15.condition(1)&physcond)\
                    .setTriggerType(TT_rpcin)

        # TAU ctpid=[0x40:0x4f]
        item_tau3   = LVL1MenuItem('L1_TAU3',  ctpid=0x40, group='1')\
                      .setLogic(thr_tau3.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau5   = LVL1MenuItem('L1_TAU5',  ctpid=0x40, group='1')\
                      .setLogic(thr_tau5.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau6   = LVL1MenuItem('L1_TAU6',  ctpid=0x41, group='1')\
                      .setLogic(thr_tau6.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau9  = LVL1MenuItem('L1_TAU9', ctpid=0x42, group='1')\
                      .setLogic(thr_tau9.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau9i  = LVL1MenuItem('L1_TAU9I', ctpid=0x43, group='1')\
                      .setLogic(thr_tau9i.condition(1)&physcond).setTriggerType(TT_calo)
#        item_tau11i = LVL1MenuItem('L1_TAU11I',  ctpid=0x44, group='1')\
#                      .setLogic(thr_tau11i.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_calo)
        item_tau15 = LVL1MenuItem('L1_TAU15', ctpid=-1, group='1')\
                      .setLogic(thr_tau15.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau16 = LVL1MenuItem('L1_TAU16', ctpid=0x45, group='1')\
                      .setLogic(thr_tau16.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau16i = LVL1MenuItem('L1_TAU16I', ctpid=0x46, group='1')\
                      .setLogic(thr_tau16i.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau25  = LVL1MenuItem('L1_TAU25', ctpid=0x47, group='1')\
                      .setLogic(thr_tau25.condition(1)&physcond).setTriggerType(TT_calo)
#        item_tau25i = LVL1MenuItem('L1_TAU25I', ctpid=0x48, group='1')\
#                      .setLogic(thr_tau25i.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_calo)
        item_tau40  = LVL1MenuItem('L1_TAU40', ctpid=0x49, group='1')\
                      .setLogic(thr_tau40.condition(1)&physcond).setTriggerType(TT_calo)

        # TAU
        # - Add/remap new L1 thresholds:
        #   TAU5->TAU5, TAU6->TAU5, TAU9->TAU6, TAU9I->TAU6I, TAU16->TAU11, TAU16I->TAU11I, TAU25->TAU11, TAU40->TAU20
        #   
        item_tau6i   = LVL1MenuItem('L1_TAU6I',  ctpid=0x42, group='1')\
                       .setLogic(thr_tau6i.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau8   = LVL1MenuItem('L1_TAU8',  ctpid=0x43, group='1')\
                      .setLogic(thr_tau8.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau8v   = LVL1MenuItem('L1_TAU8V',  ctpid=0x43, group='1')\
                       .setLogic(thr_tau8v.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau8i   = LVL1MenuItem('L1_TAU8I',  ctpid=0x43, group='1')\
                       .setLogic(thr_tau8i.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau8vi   = LVL1MenuItem('L1_TAU8VI',  ctpid=0x43, group='1')\
                        .setLogic(thr_tau8vi.condition(1)&physcond).setTriggerType(TT_calo)
        item_2tau8   = LVL1MenuItem('L1_2TAU8',  ctpid=-1, group='1')\
                      .setLogic(thr_tau8.condition(2)&physcond).setTriggerType(TT_calo)
        item_2tau8_tau11   = LVL1MenuItem('L1_2TAU8_TAU11',  ctpid=0x43, group='1')\
                             .setLogic(thr_tau8.condition(2)&thr_tau11.condition(1)&physcond).setTriggerType(TT_calo)
        item_2tau8i_tau11  = LVL1MenuItem('L1_2TAU8I_TAU11',  ctpid=0x43, group='1')\
                             .setLogic(thr_tau8i.condition(2)&thr_tau11.condition(1)&physcond).setTriggerType(TT_calo)
        item_2tau8i_tau15  = LVL1MenuItem('L1_2TAU8I_TAU15',  ctpid=0x43, group='1')\
                             .setLogic(thr_tau8i.condition(2)&thr_tau15.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau11   = LVL1MenuItem('L1_TAU11',  ctpid=0x44, group='1')\
                       .setLogic(thr_tau11.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau11v   = LVL1MenuItem('L1_TAU11V',  ctpid=0x44, group='1')\
                        .setLogic(thr_tau11v.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau11i   = LVL1MenuItem('L1_TAU11I',  ctpid=0x45, group='1')\
                      .setLogic(thr_tau11i.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau15i = LVL1MenuItem('L1_TAU15I', ctpid=-1, group='1')\
                     .setLogic(thr_tau15i.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau20   = LVL1MenuItem('L1_TAU20',  ctpid=0x46, group='1')\
                      .setLogic(thr_tau20.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau30   = LVL1MenuItem('L1_TAU30',  ctpid=0x47, group='1')\
                      .setLogic(thr_tau30.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau30i   = LVL1MenuItem('L1_TAU30I',  ctpid=-1, group='1')\
                       .setLogic(thr_tau30i.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau30_notem12 = LVL1MenuItem('L1_TAU30_NOTEM12', ctpid=-1, group='1', prescale=1)\
                             .setLogic(thr_tau30.condition(1)&Not(thr_em12.condition(1))&physcond)\
                             .setTriggerType(TT_calo|TT_caloem)
        item_tau50   = LVL1MenuItem('L1_TAU50',  ctpid=-1, group='1')\
                      .setLogic(thr_tau50.condition(1)&physcond).setTriggerType(TT_calo)
        item_tau5_empty = LVL1MenuItem('L1_TAU5_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_tau5.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_tau6_empty = LVL1MenuItem('L1_TAU6_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_tau6.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_tau6i_empty = LVL1MenuItem('L1_TAU6I_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_tau6i.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_tau8_empty = LVL1MenuItem('L1_TAU8_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_tau8.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_tau9_empty = LVL1MenuItem('L1_TAU9_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_tau9.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_tau9i_empty = LVL1MenuItem('L1_TAU9I_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_tau9i.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_tau11_empty = LVL1MenuItem('L1_TAU11_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_tau11.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_tau11i_empty = LVL1MenuItem('L1_TAU11I_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_tau11i.condition(1)&cosmiccond).setTriggerType(TT_calo)        
        item_tau16_empty = LVL1MenuItem('L1_TAU16_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_tau16.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_tau16i_empty = LVL1MenuItem('L1_TAU16I_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_tau16i.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_tau20_empty = LVL1MenuItem('L1_TAU20_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_tau20.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_tau25_empty = LVL1MenuItem('L1_TAU25_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_tau25.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_tau30_empty = LVL1MenuItem('L1_TAU30_EMPTY',  ctpid=-1, group='1')\
                          .setLogic(thr_tau30.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_tau5_bgrp1   = LVL1MenuItem('L1_TAU5_BGRP1', ctpid=-1, group='1')\
                           .setLogic(thr_tau5.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_calo)
        item_tau5_bgrp3   = LVL1MenuItem('L1_TAU5_BGRP3',   ctpid=-1, group='1')\
                           .setLogic(thr_tau5.condition(1)&Logic(bgrp0)&Logic(bgrp3)).setTriggerType(TT_calo)
        item_tau5_bgrp1_bptx   = LVL1MenuItem('L1_TAU5_BGRP1_BPTX',   ctpid=-1, group='1')\
                                .setLogic(thr_tau5.condition(1)&Logic(bgrp0)&Logic(bgrp1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))).setTriggerType(TT_calo)
        item_2tau9i = LVL1MenuItem('L1_2TAU9I', ctpid=0x4b, group='1')\
                      .setLogic(thr_tau9i.condition(2)&physcond).setTriggerType(TT_calo)
        item_2tau6i = LVL1MenuItem('L1_2TAU6I', ctpid=0x4b, group='1')\
                      .setLogic(thr_tau6i.condition(2)&physcond).setTriggerType(TT_calo)
        item_2tau16 = LVL1MenuItem('L1_2TAU16', ctpid=0x4c, group='1')\
                      .setLogic(thr_tau16.condition(2)&physcond).setTriggerType(TT_calo)
        item_2tau11 = LVL1MenuItem('L1_2TAU11', ctpid=0x4c, group='1')\
                      .setLogic(thr_tau11.condition(2)&physcond).setTriggerType(TT_calo)
        item_2tau16i = LVL1MenuItem('L1_2TAU16I', ctpid=0x4d, group='1')\
                       .setLogic(thr_tau16i.condition(2)&physcond).setTriggerType(TT_calo)
        item_2tau11i = LVL1MenuItem('L1_2TAU11I', ctpid=-1, group='1')\
                       .setLogic(thr_tau11i.condition(2)&physcond).setTriggerType(TT_calo)
        item_2tau15 = LVL1MenuItem('L1_2TAU15', ctpid=-1, group='1')\
                      .setLogic(thr_tau15.condition(2)&physcond).setTriggerType(TT_calo)

        item_2tau6_empty = LVL1MenuItem('L1_2TAU6_EMPTY', ctpid=-1, group='1')\
                            .setLogic(thr_tau6.condition(2)&cosmiccond).setTriggerType(TT_calo)
        item_2tau6i_empty = LVL1MenuItem('L1_2TAU6I_EMPTY', ctpid=-1, group='1')\
                            .setLogic(thr_tau6i.condition(2)&cosmiccond).setTriggerType(TT_calo)        
        item_2tau9_empty = LVL1MenuItem('L1_2TAU9_EMPTY', ctpid=-1, group='1')\
                            .setLogic(thr_tau9.condition(2)&cosmiccond).setTriggerType(TT_calo)
        item_2tau9i_empty = LVL1MenuItem('L1_2TAU9I_EMPTY', ctpid=-1, group='1')\
                            .setLogic(thr_tau9i.condition(2)&cosmiccond).setTriggerType(TT_calo)
        item_2tau11_empty = LVL1MenuItem('L1_2TAU11_EMPTY', ctpid=-1, group='1')\
                            .setLogic(thr_tau11.condition(2)&cosmiccond).setTriggerType(TT_calo)
        item_2tau20       = LVL1MenuItem('L1_2TAU20', ctpid=-1, group='1')\
                            .setLogic(thr_tau20.condition(2)&physcond).setTriggerType(TT_calo)
        item_2tau11i_empty = LVL1MenuItem('L1_2TAU11I_EMPTY', ctpid=-1, group='1')\
                            .setLogic(thr_tau11i.condition(2)&cosmiccond).setTriggerType(TT_calo)        
        item_2tau16_empty = LVL1MenuItem('L1_2TAU16_EMPTY', ctpid=-1, group='1')\
                            .setLogic(thr_tau16.condition(2)&cosmiccond).setTriggerType(TT_calo)
        item_2tau16i_empty = LVL1MenuItem('L1_2TAU16I_EMPTY', ctpid=-1, group='1')\
                            .setLogic(thr_tau16i.condition(2)&cosmiccond).setTriggerType(TT_calo)
                
        # Transient items
        item_tau5_bptx  = LVL1MenuItem('L1_TAU5_BPTX',  ctpid=-1, group='1')\
                          .setLogic(thr_tau5.condition(1)&Logic(bgrp0)&(thr_bptx0.condition(1)|thr_bptx1.condition(1)))\
                          .setTriggerType(TT_calo)
        item_2tau5 = LVL1MenuItem('L1_2TAU5', ctpid=0x6d, group='1')\
                     .setLogic(thr_tau5.condition(2)&physcond).setTriggerType(TT_calo)
        item_2tau6 = LVL1MenuItem('L1_2TAU6', ctpid=-1, group='1')\
                     .setLogic(thr_tau6.condition(2)&physcond).setTriggerType(TT_calo)
        
        item_2tau9 = LVL1MenuItem('L1_2TAU9', ctpid=0x6e, group='1', prescale=1)\
                     .setLogic(thr_tau9.condition(2)&physcond).setTriggerType(TT_calo)
        #item_2tau6_tau20 = LVL1MenuItem('L1_2TAU6_TAU25', ctpid=-1, group='1', prescale=10)\
         #                  .setLogic(thr_tau6.condition(2)&thr_tau25.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_calo)
        item_2tau25 = LVL1MenuItem('L1_2TAU25', ctpid=-1, group='1', prescale=1)\
                      .setLogic(thr_tau25.condition(2)&physcond).setTriggerType(TT_calo)
                
        # JET ctpid=[0x60:0x7f]
        item_j5   = LVL1MenuItem('L1_J5',   ctpid=0x60, group='1').setLogic(thr_j5.condition(1)&physcond).setTriggerType(TT_calo)
        item_j5_win6   = LVL1MenuItem('L1_J5_win6',   ctpid=0x60, group='1').setLogic(thr_j5_win6.condition(1)&physcond).setTriggerType(TT_calo)
        item_j5_win8   = LVL1MenuItem('L1_J5_win8',   ctpid=0x60, group='1').setLogic(thr_j5_win8.condition(1)&physcond).setTriggerType(TT_calo)
        item_j10  = LVL1MenuItem('L1_J10',  ctpid=0x61, group='1').setLogic(thr_j10.condition(1)&physcond).setTriggerType(TT_calo)
        item_j18  = LVL1MenuItem('L1_J18',  ctpid=0x62, group='1').setLogic(thr_j18.condition(1)&physcond).setTriggerType(TT_calo)
        item_j20  = LVL1MenuItem('L1_J20',  ctpid=-1, group='1').setLogic(thr_j20.condition(1)&physcond).setTriggerType(TT_calo) # njt
        item_j23  = LVL1MenuItem('L1_J23',  ctpid=0x63, group='1').setLogic(thr_j23.condition(1)&physcond).setTriggerType(TT_calo)
        item_j35  = LVL1MenuItem('L1_J35',  ctpid=0x64, group='1').setLogic(thr_j35.condition(1)&physcond).setTriggerType(TT_calo)
        item_j40  = LVL1MenuItem('L1_J40',  ctpid=-1, group='1').setLogic(thr_j40.condition(1)&physcond).setTriggerType(TT_calo) # njt
        item_j42  = LVL1MenuItem('L1_J42',  ctpid=0x65, group='1').setLogic(thr_j42.condition(1)&physcond).setTriggerType(TT_calo)
        item_j70  = LVL1MenuItem('L1_J70',  ctpid=0x66, group='1').setLogic(thr_j70.condition(1)&physcond).setTriggerType(TT_calo)
        item_j100 = LVL1MenuItem('L1_J100',  ctpid=-1, group='1').setLogic(thr_j100.condition(1)&physcond).setTriggerType(TT_calo) # njt
        item_j120 = LVL1MenuItem('L1_J120', ctpid=0x67, group='1').setLogic(thr_j120.condition(1)&physcond).setTriggerType(TT_calo)
        item_j130 = LVL1MenuItem('L1_J130',  ctpid=-1, group='1').setLogic(thr_j130.condition(1)&physcond).setTriggerType(TT_calo) # njt
        item_j150 = LVL1MenuItem('L1_J150',  ctpid=-1, group='1').setLogic(thr_j150.condition(1)&physcond).setTriggerType(TT_calo) # njt
        item_j15cf = LVL1MenuItem('L1_J15CF',  ctpid=-1, group='1').setLogic(thr_j15cf.condition(1)&physcond).setTriggerType(TT_calo) # njt

        item_j5_bgrp1   = LVL1MenuItem('L1_J5_BGRP1', ctpid=-1, group='1')\
                           .setLogic(thr_j5.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_calo)
        item_j5_bgrp3   = LVL1MenuItem('L1_J5_BGRP3',   ctpid=-1, group='1')\
                           .setLogic(thr_j5.condition(1)&Logic(bgrp0)&Logic(bgrp3)).setTriggerType(TT_calo)
        item_j5_bgrp1_bptx   = LVL1MenuItem('L1_J5_BGRP1_BPTX',   ctpid=-1, group='1')\
                                .setLogic(thr_j5.condition(1)&Logic(bgrp0)&Logic(bgrp1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))).setTriggerType(TT_calo)

	#pPb
        item_j5_te90   = LVL1MenuItem('L1_J5_TE90',   ctpid=0x60, group='1').setLogic(thr_j5.condition(1)&thr_te90.condition(1)&physcond).setTriggerType(TT_calo)
        item_2j5_te90   = LVL1MenuItem('L1_2J5_TE90',   ctpid=0x60, group='1').setLogic(thr_j5.condition(2)&thr_te90.condition(1)&physcond).setTriggerType(TT_calo)

        # - Add/remap new L1 thresholds:
        #   J20->J15, J40->J30, J70->J55, J100->J75, J130->J95, J150->J115
        item_j15   = LVL1MenuItem('L1_J15',   ctpid=-1, group='1').setLogic(thr_j15.condition(1)&physcond).setTriggerType(TT_calo)
        item_j20   = LVL1MenuItem('L1_J20',   ctpid=-1, group='1').setLogic(thr_j20.condition(1)&physcond).setTriggerType(TT_calo)
        item_j30   = LVL1MenuItem('L1_J30',   ctpid=-1, group='1').setLogic(thr_j30.condition(1)&physcond).setTriggerType(TT_calo)
        item_j50   = LVL1MenuItem('L1_J50',   ctpid=-1, group='1').setLogic(thr_j50.condition(1)&physcond).setTriggerType(TT_calo)
        item_j55   = LVL1MenuItem('L1_J55',   ctpid=-1, group='1').setLogic(thr_j55.condition(1)&physcond).setTriggerType(TT_calo)
        item_j75   = LVL1MenuItem('L1_J75',   ctpid=-1, group='1').setLogic(thr_j75.condition(1)&physcond).setTriggerType(TT_calo)
        item_j95   = LVL1MenuItem('L1_J95',   ctpid=-1, group='1').setLogic(thr_j95.condition(1)&physcond).setTriggerType(TT_calo)
        item_j115   = LVL1MenuItem('L1_J115',   ctpid=-1, group='1').setLogic(thr_j115.condition(1)&physcond).setTriggerType(TT_calo)
        item_j175   = LVL1MenuItem('L1_J175',   ctpid=-1, group='1').setLogic(thr_j175.condition(1)&physcond).setTriggerType(TT_calo)
        item_j250   = LVL1MenuItem('L1_J250',   ctpid=-1, group='1').setLogic(thr_j250.condition(1)&physcond).setTriggerType(TT_calo)
        
        item_3j15   = LVL1MenuItem('L1_3J15',   ctpid=-1, group='1').setLogic(thr_j15.condition(3)&physcond).setTriggerType(TT_calo)
        item_4j15   = LVL1MenuItem('L1_4J15',   ctpid=-1, group='1').setLogic(thr_j15.condition(4)&physcond).setTriggerType(TT_calo)
        item_5j15   = LVL1MenuItem('L1_5J15',   ctpid=-1, group='1').setLogic(thr_j15.condition(5)&physcond).setTriggerType(TT_calo)

        item_2j5 = LVL1MenuItem('L1_2J5', ctpid=0x68, group='1').setLogic(thr_j5.condition(2)&physcond).setTriggerType(TT_calo)
        item_2j10 = LVL1MenuItem('L1_2J10', ctpid=-1, group='1').setLogic(thr_j10.condition(2)&physcond).setTriggerType(TT_calo)
        item_2j10_6 = LVL1MenuItem('L1_2J10_win6', ctpid=0x6a, group='1').setLogic(thr_j10_6.condition(2)&physcond).setTriggerType(TT_calo)
        item_2j20 = LVL1MenuItem('L1_2J20', ctpid=-1, group='1').setLogic(thr_j20.condition(2)&physcond).setTriggerType(TT_calo)
        item_2j15 = LVL1MenuItem('L1_2J15', ctpid=-1, group='1').setLogic(thr_j15.condition(2)&physcond).setTriggerType(TT_calo)
        item_2j18 = LVL1MenuItem('L1_2J18', ctpid=0x6c, group='1').setLogic(thr_j18.condition(2)&physcond).setTriggerType(TT_calo)
        item_2j40 = LVL1MenuItem('L1_2J40', ctpid=-1, group='1').setLogic(thr_j40.condition(2)&physcond).setTriggerType(TT_calo)
        item_2j70 = LVL1MenuItem('L1_2J70', ctpid=-1, group='1').setLogic(thr_j70.condition(2)&physcond).setTriggerType(TT_calo)
        item_2j100 = LVL1MenuItem('L1_2J100', ctpid=-1, group='1').setLogic(thr_j100.condition(2)&physcond).setTriggerType(TT_calo)
        item_2j30 = LVL1MenuItem('L1_2J30', ctpid=-1, group='1').setLogic(thr_j30.condition(2)&physcond).setTriggerType(TT_calo)
        item_2j55 = LVL1MenuItem('L1_2J55', ctpid=-1, group='1').setLogic(thr_j55.condition(2)&physcond).setTriggerType(TT_calo)
        item_2j95 = LVL1MenuItem('L1_2J95', ctpid=-1, group='1').setLogic(thr_j95.condition(2)&physcond).setTriggerType(TT_calo)
        item_2j75 = LVL1MenuItem('L1_2J75', ctpid=-1, group='1').setLogic(thr_j75.condition(2)&physcond).setTriggerType(TT_calo)
        
        item_3j10 = LVL1MenuItem('L1_3J10', ctpid=0x7f, group='1').setLogic(thr_j10.condition(3)&physcond).setTriggerType(TT_calo)
        item_3j50 = LVL1MenuItem('L1_3J50', ctpid=0x7f, group='1').setLogic(thr_j50.condition(3)&physcond).setTriggerType(TT_calo)
        item_3j75 = LVL1MenuItem('L1_3J75', ctpid=-1, group='1').setLogic(thr_j75.condition(3)&physcond).setTriggerType(TT_calo)
        item_3j5 = LVL1MenuItem('L1_3J5', ctpid=0x7f, group='1').setLogic(thr_j5.condition(3)&physcond).setTriggerType(TT_calo)
        item_3j20 = LVL1MenuItem('L1_3J20', ctpid=-1, group='1').setLogic(thr_j20.condition(3)&physcond).setTriggerType(TT_calo) # new jet threshold
        item_3j40 = LVL1MenuItem('L1_3J40', ctpid=-1, group='1').setLogic(thr_j40.condition(3)&physcond).setTriggerType(TT_calo) # new jet threshold
        item_4j10 = LVL1MenuItem('L1_4J10', ctpid=0x6e, group='1').setLogic(thr_j10.condition(4)&physcond).setTriggerType(TT_calo)
        item_4j5 = LVL1MenuItem('L1_4J5', ctpid=0x6e, group='1').setLogic(thr_j5.condition(4)&physcond).setTriggerType(TT_calo)
        item_4j20 = LVL1MenuItem('L1_4J20', ctpid=-1, group='1').setLogic(thr_j20.condition(4)&physcond).setTriggerType(TT_calo)
        item_4j40 = LVL1MenuItem('L1_4J40', ctpid=-1, group='1').setLogic(thr_j40.condition(4)&physcond).setTriggerType(TT_calo)
        item_3j23 = LVL1MenuItem('L1_3J23', ctpid=0x70, group='1').setLogic(thr_j23.condition(3)&physcond).setTriggerType(TT_calo)
        item_4j18 = LVL1MenuItem('L1_4J18', ctpid=0x71, group='1').setLogic(thr_j18.condition(4)&physcond).setTriggerType(TT_calo)
        item_4j23 = LVL1MenuItem('L1_4J23', ctpid=0x72, group='1').setLogic(thr_j23.condition(4)&physcond).setTriggerType(TT_calo)
        item_3j35 = LVL1MenuItem('L1_3J35', ctpid=0x73, group='1').setLogic(thr_j35.condition(3)&physcond).setTriggerType(TT_calo)
        item_4j35 = LVL1MenuItem('L1_4J35', ctpid=0x74, group='1').setLogic(thr_j35.condition(4)&physcond).setTriggerType(TT_calo)
        item_3j42 = LVL1MenuItem('L1_3J42', ctpid=0x75, group='1').setLogic(thr_j42.condition(3)&physcond).setTriggerType(TT_calo)
        item_3j70 = LVL1MenuItem('L1_3J70', ctpid=0x76, group='1').setLogic(thr_j70.condition(3)&physcond).setTriggerType(TT_calo)
        item_4j10_3j20 = LVL1MenuItem('L1_4J10_3J20', ctpid=-1, group='1')\
                         .setLogic(thr_j10.condition(4)&thr_j20.condition(3)&physcond).setTriggerType(TT_calo) # new jet threshold
        item_5j10 = LVL1MenuItem('L1_5J10', ctpid=-1, group='1').setLogic(thr_j10.condition(5)&physcond).setTriggerType(TT_calo)
        item_5j20 = LVL1MenuItem('L1_5J20', ctpid=-1, group='1').setLogic(thr_j20.condition(5)&physcond).setTriggerType(TT_calo)
        item_6j10 = LVL1MenuItem('L1_6J10', ctpid=-1, group='1').setLogic(thr_j10.condition(6)&physcond).setTriggerType(TT_calo)
        
        item_j5_empty = LVL1MenuItem('L1_J5_EMPTY', ctpid=-1, group='1').setLogic(thr_j5.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_j10_empty = LVL1MenuItem('L1_J10_EMPTY', ctpid=-1, group='1').setLogic(thr_j10.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_j10_6_empty = LVL1MenuItem('L1_J10_win6_EMPTY', ctpid=-1, group='1').setLogic(thr_j10_6.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_j15_empty = LVL1MenuItem('L1_J15_EMPTY', ctpid=-1, group='1').setLogic(thr_j15.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_j20_empty = LVL1MenuItem('L1_J20_EMPTY', ctpid=-1, group='1').setLogic(thr_j20.condition(1)&cosmiccond).setTriggerType(TT_calo)        
        item_j30_empty = LVL1MenuItem('L1_J30_EMPTY', ctpid=-1, group='1').setLogic(thr_j30.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_j40_empty = LVL1MenuItem('L1_J40_EMPTY', ctpid=-1, group='1').setLogic(thr_j40.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_j55_empty = LVL1MenuItem('L1_J55_EMPTY', ctpid=-1, group='1').setLogic(thr_j55.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_j70_empty = LVL1MenuItem('L1_J70_EMPTY', ctpid=-1, group='1').setLogic(thr_j70.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_j75_empty = LVL1MenuItem('L1_J75_EMPTY', ctpid=-1, group='1').setLogic(thr_j75.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_j100_empty = LVL1MenuItem('L1_J100_EMPTY', ctpid=-1, group='1').setLogic(thr_j100.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_j130_empty = LVL1MenuItem('L1_J130_EMPTY', ctpid=-1, group='1').setLogic(thr_j130.condition(1)&cosmiccond).setTriggerType(TT_calo)


        #example zero bias seeded by single jet
        #item_zb_j10  = LVL1MenuItem('L1_ZB_J10',  ctpid=240, group='1').setLogic(thr_zb_j10.condition(1)&physcond).setTriggerType(TT_calo)
        #example zero bias seeded by di jet
        #item_zb_2j10 = LVL1MenuItem('L1_ZB_2J10', ctpid=241, group='1').setLogic(thr_zb_2j10.condition(1)&physcond).setTriggerType(TT_calo)
        #zb item for v3 menu, seeded by mbts a

        if ('InitialBeam_HI_v1' in TriggerFlags.triggerMenuSetup()):
            item_zb_4mbts_a = LVL1MenuItem('L1_ZB', ctpid=240, group='1').setLogic(thr_zb_4MBTS_A.condition(1)&physcond).setTriggerType(TT_zerobs)
        elif ('Physics_HI_v1' in TriggerFlags.triggerMenuSetup() or 'MC_HI_v1' in TriggerFlags.triggerMenuSetup()) :
            item_zb_j75 = LVL1MenuItem('L1_ZB', ctpid=240, group='1').setLogic(thr_zb_j75.condition(1)&physcond).setTriggerType(TT_zerobs)
        elif ('Physics_HI_v2' in TriggerFlags.triggerMenuSetup() or 'MC_HI_v2' in TriggerFlags.triggerMenuSetup()) :
            item_zb_j10 = LVL1MenuItem('L1_ZB', ctpid=240, group='1').setLogic(thr_zb_j10.condition(1)&physcond).setTriggerType(TT_zerobs)
        elif ('pp_v3' in TriggerFlags.triggerMenuSetup() ):
            item_zb_em12 = LVL1MenuItem('L1_ZB', ctpid=240, group='1').setLogic(thr_zb_em12.condition(1)&physcond).setTriggerType(TT_zerobs)                       
        elif ('pp_v4' in TriggerFlags.triggerMenuSetup() ):
            item_zb_em12 = LVL1MenuItem('L1_ZB', ctpid=240, group='1').setLogic(thr_zb_em12.condition(1)&physcond).setTriggerType(TT_zerobs)
        elif ('alfa_v2' in TriggerFlags.triggerMenuSetup() ):
            item_zb_em12 = LVL1MenuItem('L1_ZB', ctpid=240, group='1').setLogic(thr_zb_em12.condition(1)&physcond).setTriggerType(TT_zerobs)
        else:
            item_zb_em14 = LVL1MenuItem('L1_ZB', ctpid=240, group='1').setLogic(thr_zb_em14.condition(1)&physcond).setTriggerType(TT_zerobs)
                        
        # - Add/remap new L1 thresholds:
        #   J20->J15, J40->J30, J70->J55, J100->J75, J130->J95, J150->J115
        item_3j10_j30 = LVL1MenuItem('L1_3J10_J30',ctpid=-1, group='1')\
                        .setLogic(thr_j10.condition(3)&thr_j30.condition(1)&physcond)
        item_3j10_2j30 = LVL1MenuItem('L1_3J10_2J30',ctpid=-1, group='1')\
                        .setLogic(thr_j10.condition(3)&thr_j30.condition(2)&physcond)
        item_3j15_j30 = LVL1MenuItem('L1_3J15_J30',ctpid=-1, group='1')\
                        .setLogic(thr_j15.condition(3)&thr_j30.condition(1)&physcond)
        item_4j10_3j15 = LVL1MenuItem('L1_4J10_3J15',ctpid=-1, group='1')\
                        .setLogic(thr_j10.condition(4)&thr_j15.condition(3)&physcond)
        item_3j5_j30 = LVL1MenuItem('L1_3J5_J30', ctpid=-1, group='1').setLogic(thr_j5.condition(3)&thr_j30.condition(1)&physcond).setTriggerType(TT_calo)
        item_4j5_j15 = LVL1MenuItem('L1_4J5_J15', ctpid=-1, group='1').setLogic(thr_j5.condition(4)&thr_j15.condition(1)&physcond).setTriggerType(TT_calo)
        item_4j5_j30 = LVL1MenuItem('L1_4J5_J30', ctpid=-1, group='1').setLogic(thr_j5.condition(4)&thr_j30.condition(1)&physcond).setTriggerType(TT_calo)
        item_3j30 = LVL1MenuItem('L1_3J30',ctpid=-1, group='1')\
                    .setLogic(thr_j30.condition(3)&physcond)
        item_4j30 = LVL1MenuItem('L1_4J30',ctpid=-1, group='1')\
                    .setLogic(thr_j30.condition(4)&physcond)

        item_2j10_j30 = LVL1MenuItem('L1_2J10_J30',ctpid=-1, group='1')\
                        .setLogic(thr_j10.condition(2)&thr_j30.condition(1)&physcond)
        item_2j10_j50 = LVL1MenuItem('L1_2J10_J50',ctpid=-1, group='1')\
                        .setLogic(thr_j10.condition(2)&thr_j50.condition(1)&physcond)

        item_2j10_j30_je140 = LVL1MenuItem('L1_2J10_J30_JE140',ctpid=-1, group='1')\
                              .setLogic(thr_j10.condition(2)&thr_j30.condition(1)&thr_je140.condition(1)&physcond)

        item_2j10_j55 = LVL1MenuItem('L1_2J10_J55',ctpid=-1, group='1')\
                        .setLogic(thr_j10.condition(2)&thr_j55.condition(1)&physcond)
        item_2j10_j75 = LVL1MenuItem('L1_2J10_J75',ctpid=-1, group='1')\
                        .setLogic(thr_j10.condition(2)&thr_j75.condition(1)&physcond)
        item_2j15_j50 = LVL1MenuItem('L1_2J15_J50',ctpid=-1, group='1')\
                        .setLogic(thr_j15.condition(2)&thr_j50.condition(1)&physcond)
        item_3j15_j50 = LVL1MenuItem('L1_3J15_J50',ctpid=-1, group='1')\
                        .setLogic(thr_j15.condition(3)&thr_j50.condition(1)&physcond)
        item_2j15_j75 = LVL1MenuItem('L1_2J15_J75',ctpid=-1, group='1')\
                        .setLogic(thr_j15.condition(2)&thr_j75.condition(1)&physcond)
        item_2j15_j95 = LVL1MenuItem('L1_2J15_J95',ctpid=-1, group='1')\
                        .setLogic(thr_j15.condition(2)&thr_j95.condition(1)&physcond)
        
        item_j10_fj10 = LVL1MenuItem('L1_J10_FJ10',ctpid=-1, group='1')\
                        .setLogic(thr_j10.condition(1)&(thr_jf10.condition(1)|thr_jb10.condition(1))&physcond)
        item_j30_fj30 = LVL1MenuItem('L1_J30_FJ30',ctpid=-1, group='1')\
                        .setLogic(thr_j30.condition(1)&(thr_jf30.condition(1)|thr_jb30.condition(1))&physcond)
        item_j50_fj50 = LVL1MenuItem('L1_J50_FJ50',ctpid=-1, group='1')\
                        .setLogic(thr_j50.condition(1)&(thr_jf50.condition(1)|thr_jb50.condition(1))&physcond)
        item_j75_fj75 = LVL1MenuItem('L1_J75_FJ75',ctpid=-1, group='1')\
                        .setLogic(thr_j75.condition(1)&(thr_jf75.condition(1)|thr_jb75.condition(1))&physcond)
        

        # Transient items
        item_j5_bptx = LVL1MenuItem('L1_J5_BPTX', ctpid=-1, group='1')\
                       .setLogic(thr_j5.condition(1)&Logic(bgrp0)&(thr_bptx0.condition(1)|thr_bptx1.condition(1)))\
                       .setTriggerType(TT_calo)
        item_j10_6 = LVL1MenuItem('L1_J10_win6', ctpid=0x6f, group='1').setLogic(thr_j10_6.condition(1)&physcond).setTriggerType(TT_calo)
        item_j100 = LVL1MenuItem('L1_J100', ctpid=-1, group='1').setLogic(thr_j100.condition(1)&physcond).setTriggerType(TT_calo)
        item_j200 = LVL1MenuItem('L1_J200', ctpid=-1, group='1').setLogic(thr_j200.condition(1)&physcond).setTriggerType(TT_calo)
        item_j350 = LVL1MenuItem('L1_J350', ctpid=-1, group='1').setLogic(thr_j350.condition(1)&physcond).setTriggerType(TT_calo)
        item_4j18_3j23_2j35_j42 = LVL1MenuItem('L1_4J18_3J23_2J35_J42', ctpid=-1, group='1')\
                                  .setLogic(thr_j18.condition(4)&thr_j23.condition(3)\
                                            &thr_j35.condition(2)&thr_j42.condition(1)&physcond).setTriggerType(TT_calo)
        item_5j5_4j18_3j23_j42 = LVL1MenuItem('L1_5J5_4J18_3J23_J42', ctpid=-1, group='1')\
                                  .setLogic(thr_j5.condition(5)&thr_j18.condition(4)\
                                            &thr_j23.condition(3)&thr_j42.condition(1)&physcond).setTriggerType(TT_calo)
        item_3j10_j40 = LVL1MenuItem('L1_3J10_J40', ctpid=-1, group='1')\
                        .setLogic(thr_j10.condition(3)&thr_j40.condition(1)&physcond).setTriggerType(TT_calo) # new jet threshold
        item_3j20_j40 = LVL1MenuItem('L1_3J20_J40', ctpid=-1, group='1')\
                        .setLogic(thr_j20.condition(3)&thr_j40.condition(1)&physcond).setTriggerType(TT_calo) # new jet threshold
        item_4j23_j42 = LVL1MenuItem('L1_4J23_J42', ctpid=-1, group='1')\
                        .setLogic(thr_j23.condition(4)&thr_j42.condition(1)&physcond).setTriggerType(TT_calo)

        # FJ
        #pPb items
        item_fj0  = LVL1MenuItem('L1_FJ0', ctpid=0x7b, group='1')\
                    .setLogic((thr_jf0.condition(1) | thr_jb0.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj1  = LVL1MenuItem('L1_FJ1', ctpid=0x7b, group='1')\
                    .setLogic((thr_jf1.condition(1) | thr_jb1.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj2  = LVL1MenuItem('L1_FJ2', ctpid=0x7b, group='1')\
                    .setLogic((thr_jf2.condition(1) | thr_jb2.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj3  = LVL1MenuItem('L1_FJ3', ctpid=0x7b, group='1')\
                    .setLogic((thr_jf3.condition(1) | thr_jb3.condition(1))&physcond).setTriggerType(TT_calo)

        item_fj0_a  = LVL1MenuItem('L1_FJ0_A', ctpid=0x7b, group='1')\
                    .setLogic((thr_jf0.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj1_a  = LVL1MenuItem('L1_FJ1_A', ctpid=0x7b, group='1')\
                    .setLogic((thr_jf1.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj2_a  = LVL1MenuItem('L1_FJ2_A', ctpid=0x7b, group='1')\
                    .setLogic((thr_jf2.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj3_a  = LVL1MenuItem('L1_FJ3_A', ctpid=0x7b, group='1')\
                    .setLogic((thr_jf3.condition(1))&physcond).setTriggerType(TT_calo)

        item_fj0_c  = LVL1MenuItem('L1_FJ0_C', ctpid=0x7b, group='1')\
                    .setLogic((thr_jb0.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj1_c  = LVL1MenuItem('L1_FJ1_C', ctpid=0x7b, group='1')\
                    .setLogic((thr_jb1.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj2_c  = LVL1MenuItem('L1_FJ2_C', ctpid=0x7b, group='1')\
                    .setLogic((thr_jb2.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj3_c  = LVL1MenuItem('L1_FJ3_C', ctpid=0x7b, group='1')\
                    .setLogic((thr_jb3.condition(1))&physcond).setTriggerType(TT_calo)
        
        #pp items
        item_fj18 = LVL1MenuItem('L1_FJ18', ctpid=0x77, group='1')\
                    .setLogic((thr_jf18.condition(1) | thr_jb18.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj35 = LVL1MenuItem('L1_FJ35', ctpid=0x78, group='1')\
                    .setLogic((thr_jf35.condition(1) | thr_jb35.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj70 = LVL1MenuItem('L1_FJ70', ctpid=0x79, group='1')\
                    .setLogic((thr_jf70.condition(1) | thr_jb70.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj120 = LVL1MenuItem('L1_FJ120', ctpid=0x7a, group='1')\
                     .setLogic((thr_jf120.condition(1) | thr_jb120.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj5  = LVL1MenuItem('L1_FJ5', ctpid=0x7b, group='1')\
                    .setLogic((thr_jf5.condition(1) | thr_jb5.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj10  = LVL1MenuItem('L1_FJ10', ctpid=-1, group='1')\
                    .setLogic((thr_jf10.condition(1) | thr_jb10.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj15  = LVL1MenuItem('L1_FJ15', ctpid=-1, group='1')\
                    .setLogic((thr_jf15.condition(1) | thr_jb15.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj20  = LVL1MenuItem('L1_FJ20', ctpid=-1, group='1')\
                    .setLogic((thr_jf20.condition(1) | thr_jb20.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj30  = LVL1MenuItem('L1_FJ30', ctpid=-1, group='1')\
                    .setLogic((thr_jf30.condition(1) | thr_jb30.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj40  = LVL1MenuItem('L1_FJ40', ctpid=-1, group='1')\
                    .setLogic((thr_jf40.condition(1) | thr_jb40.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj50  = LVL1MenuItem('L1_FJ50', ctpid=-1, group='1')\
                    .setLogic((thr_jf50.condition(1) | thr_jb50.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj55  = LVL1MenuItem('L1_FJ55', ctpid=-1, group='1')\
                    .setLogic((thr_jf55.condition(1) | thr_jb55.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj75  = LVL1MenuItem('L1_FJ75', ctpid=-1, group='1')\
                    .setLogic((thr_jf75.condition(1) | thr_jb75.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj95  = LVL1MenuItem('L1_FJ95', ctpid=-1, group='1')\
                    .setLogic((thr_jf95.condition(1) | thr_jb95.condition(1))&physcond).setTriggerType(TT_calo)
        item_fj100  = LVL1MenuItem('L1_FJ100', ctpid=-1, group='1')\
                     .setLogic((thr_jf100.condition(1) | thr_jb100.condition(1))&physcond).setTriggerType(TT_calo)
        

        item_fj3_bgrp1 = LVL1MenuItem('L1_FJ3_BGRP1', ctpid=-1, group='1')\
                          .setLogic((thr_jf3.condition(1) | thr_jb3.condition(1))&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_calo)
        item_fj3_bgrp3 = LVL1MenuItem('L1_FJ3_BGRP3', ctpid=-1, group='1')\
                          .setLogic((thr_jf3.condition(1) | thr_jb3.condition(1))&Logic(bgrp0)&Logic(bgrp3)).setTriggerType(TT_calo)
        item_fj3_bgrp1_bptx = LVL1MenuItem('L1_FJ3_BGRP1_BPTX', ctpid=-1, group='1')\
                          .setLogic((thr_jf3.condition(1) | thr_jb3.condition(1))&Logic(bgrp0)&Logic(bgrp1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))).setTriggerType(TT_calo)

        item_2fj18 = LVL1MenuItem('L1_2FJ18', ctpid=0x7c, group='1')\
                     .setLogic(thr_jf18.condition(1) & thr_jb18.condition(1)&physcond).setTriggerType(TT_calo)

        item_2fj35 = LVL1MenuItem('L1_2FJ35', ctpid=0x7d, group='1')\
                     .setLogic(thr_jf35.condition(1) & thr_jb35.condition(1)&physcond).setTriggerType(TT_calo)
        item_2fj3  = LVL1MenuItem('L1_2FJ3', ctpid=0x7d, group='1')\
                     .setLogic(thr_jf3.condition(1) & thr_jb3.condition(1)&physcond).setTriggerType(TT_calo)
        
        item_2fj5  = LVL1MenuItem('L1_2FJ5', ctpid=0x7d, group='1')\
                     .setLogic(thr_jf5.condition(1) & thr_jb5.condition(1)&physcond).setTriggerType(TT_calo)
        
        item_2fj15  = LVL1MenuItem('L1_2FJ15', ctpid=-1, group='1')\
                     .setLogic(thr_jf15.condition(1) & thr_jb15.condition(1)&physcond).setTriggerType(TT_calo)
        
        item_2fj70 = LVL1MenuItem('L1_2FJ70', ctpid=0x7e, group='1')\
                     .setLogic(thr_jf70.condition(1) & thr_jb70.condition(1)&physcond).setTriggerType(TT_calo)
        item_2fj10 = LVL1MenuItem('L1_2FJ10', ctpid=0x7c, group='1')\
                     .setLogic(thr_jf10.condition(1) & thr_jb10.condition(1)&physcond).setTriggerType(TT_calo)
        item_2fj30 = LVL1MenuItem('L1_2FJ30', ctpid=0x7d, group='1')\
                     .setLogic(thr_jf30.condition(1) & thr_jb30.condition(1)&physcond).setTriggerType(TT_calo)
        item_2fj50 = LVL1MenuItem('L1_2FJ50', ctpid=0x7b, group='1')\
                     .setLogic(thr_jf50.condition(1) & thr_jb50.condition(1)&physcond).setTriggerType(TT_calo)
        item_2fj55 = LVL1MenuItem('L1_2FJ55', ctpid=0x7b, group='1')\
                     .setLogic(thr_jf55.condition(1) & thr_jb55.condition(1)&physcond).setTriggerType(TT_calo)

        item_fj0_empty = LVL1MenuItem('L1_FJ0_EMPTY', ctpid=-1, group='1')\
                    .setLogic((thr_jf0.condition(1) | thr_jb0.condition(1))&cosmiccond).setTriggerType(TT_calo)
        item_fj1_empty = LVL1MenuItem('L1_FJ1_EMPTY', ctpid=-1, group='1')\
                    .setLogic((thr_jf1.condition(1) | thr_jb1.condition(1))&cosmiccond).setTriggerType(TT_calo)
        item_fj3_empty = LVL1MenuItem('L1_FJ3_EMPTY', ctpid=-1, group='1')\
                    .setLogic((thr_jf3.condition(1) | thr_jb3.condition(1))&cosmiccond).setTriggerType(TT_calo)
        item_fj5_empty = LVL1MenuItem('L1_FJ5_EMPTY', ctpid=-1, group='1')\
                    .setLogic((thr_jf5.condition(1) | thr_jb5.condition(1))&cosmiccond).setTriggerType(TT_calo)
        item_fj10_empty = LVL1MenuItem('L1_FJ10_EMPTY', ctpid=-1, group='1')\
                    .setLogic((thr_jf10.condition(1) | thr_jb10.condition(1))&cosmiccond).setTriggerType(TT_calo)
        item_fj30_empty = LVL1MenuItem('L1_FJ30_EMPTY', ctpid=-1, group='1')\
                          .setLogic((thr_jf30.condition(1) | thr_jb30.condition(1))&cosmiccond).setTriggerType(TT_calo)
        
        item_fj18_empty = LVL1MenuItem('L1_FJ18_EMPTY', ctpid=-1, group='1')\
                    .setLogic((thr_jf18.condition(1) | thr_jb18.condition(1))&cosmiccond).setTriggerType(TT_calo)

        item_fj18_bptx = LVL1MenuItem('L1_FJ18_BPTX', ctpid=-1, group='1')\
                    .setLogic((thr_jf18.condition(1) | thr_jb18.condition(1))&Logic(bgrp0)&(thr_bptx0.condition(1)|thr_bptx1.condition(1)))\
                    .setTriggerType(TT_calo)
        item_fj3_bptx = LVL1MenuItem('L1_FJ3_BPTX', ctpid=0x84, group='1')\
                    .setLogic((thr_jf3.condition(1) | thr_jb3.condition(1))&Logic(bgrp0)&(thr_bptx0.condition(1)|thr_bptx1.condition(1)))\
                    .setTriggerType(TT_calo)

        # Diffractive

        if 'Cosmic2009_v1' in TriggerFlags.triggerMenuSetup():
            item_j18_mbtsveto = LVL1MenuItem('L1_J18_MV', ctpid=0x70, group='1', prescale=1)\
                                .setLogic(thr_j18.condition(1)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&physcond).setTriggerType(TT_calo)
        else:
            item_j18_mbtsveto = LVL1MenuItem('L1_J18_MV', ctpid=-1, group='1', prescale=1)\
                                .setLogic(thr_j18.condition(1)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&physcond).setTriggerType(TT_calo)
            item_j10_mbtsveto = LVL1MenuItem('L1_J10_MV', ctpid=-1, group='1', prescale=1)\
                                .setLogic(thr_j10.condition(1)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&physcond).setTriggerType(TT_calo)
            item_j20_mbtsveto = LVL1MenuItem('L1_J20_MV', ctpid=-1, group='1', prescale=1)\
                                .setLogic(thr_j20.condition(1)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&physcond).setTriggerType(TT_calo)

        item_j18_lucidveto = LVL1MenuItem('L1_J18_LV', ctpid=0x71, group='1', prescale=1)\
                             .setLogic(thr_j18.condition(1)&Not(thr_lucidA.condition(1)&thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)
        item_j18_nolucid = LVL1MenuItem('L1_J18_NL', ctpid=0x72, group='1', prescale=1)\
                           .setLogic(thr_j18.condition(1)&Not(thr_lucidA.condition(1))&Not(thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)


        item_j20_mbtsveto = LVL1MenuItem('L1_J20_MV', ctpid=-1, group='1', prescale=1)\
                            .setLogic(thr_j20.condition(1)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&physcond).setTriggerType(TT_calo)
        item_j20_lucidveto = LVL1MenuItem('L1_J20_LV', ctpid=-1, group='1', prescale=1)\
                             .setLogic(thr_j20.condition(1)&Not(thr_lucidA.condition(1)&thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)
        item_j20_nolucid = LVL1MenuItem('L1_J20_NL', ctpid=-1, group='1', prescale=1)\
                           .setLogic(thr_j20.condition(1)&Not(thr_lucidA.condition(1))&Not(thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)

        item_j15_mbtsveto = LVL1MenuItem('L1_J15_MV', ctpid=-1, group='1', prescale=1)\
                            .setLogic(thr_j15.condition(1)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&physcond).setTriggerType(TT_calo)
        item_j15_lucidveto = LVL1MenuItem('L1_J15_LV', ctpid=-1, group='1', prescale=1)\
                             .setLogic(thr_j15.condition(1)&Not(thr_lucidA.condition(1)&thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)
        item_j15_nolucid = LVL1MenuItem('L1_J15_NL', ctpid=-1, group='1', prescale=1)\
                           .setLogic(thr_j15.condition(1)&Not(thr_lucidA.condition(1))&Not(thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)
        item_2j15_nolucid = LVL1MenuItem('L1_2J15_NL', ctpid=-1, group='1', prescale=1)\
                           .setLogic(thr_j15.condition(2)&Not(thr_lucidA.condition(1))&Not(thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)

        item_mu0_mbtsveto = LVL1MenuItem('L1_MU0_MV', ctpid=-1, group='1', prescale=1)\
                            .setLogic(thr_mu0.condition(1)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&physcond).setTriggerType(TT_rpcin)
        item_2mu0_mbtsveto = LVL1MenuItem('L1_2MU0_MV', ctpid=-1, group='1', prescale=1)\
                            .setLogic(thr_mu0.condition(2)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&physcond).setTriggerType(TT_rpcin)
        item_mu0_veto_te50 = LVL1MenuItem('L1_MU0_VTE50', ctpid=-1).setLogic(thr_mu0.condition(1)\
                                                                             &Not(thr_te50.condition(1))&physcond).setTriggerType(TT_rpcin)
        item_mu0_veto_te20 = LVL1MenuItem('L1_MU0_VTE20', ctpid=-1).setLogic(thr_mu0.condition(1)\
                                                                             &Not(thr_te20.condition(1))&physcond).setTriggerType(TT_rpcin)

        item_mu0_te50 = LVL1MenuItem('L1_MU0_TE50', ctpid=-1).setLogic(thr_mu0.condition(1)\
                                                                                    &thr_te50.condition(1)&physcond).setTriggerType(TT_rpcin)

        item_mu0_mbtsveto_veto_te50 = LVL1MenuItem('L1_MU0_MV_VTE50', ctpid=-1, group='1', prescale=1)\
                                      .setLogic(thr_mu0.condition(1)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&Not(thr_te50.condition(1))\
                                                &physcond).setTriggerType(TT_rpcin)
        
        item_em3_veto_te50 = LVL1MenuItem('L1_EM3_VTE50', ctpid=-1).setLogic(thr_em3.condition(1)\
                                                                                    &Not(thr_te50.condition(1))&physcond).setTriggerType(TT_calo)
        item_em3_veto_te20 = LVL1MenuItem('L1_EM3_VTE20', ctpid=-1).setLogic(thr_em3.condition(1)\
                                                                             &Not(thr_te20.condition(1))&physcond).setTriggerType(TT_calo)
        item_em3_te50 = LVL1MenuItem('L1_EM3_TE50', ctpid=-1).setLogic(thr_em3.condition(1)\
                                                                       &thr_te50.condition(1)&physcond).setTriggerType(TT_calo)

        item_em3_2j5 = LVL1MenuItem('L1_EM3_2J5', ctpid=-1).setLogic(thr_em3.condition(1)\
                                                                                    &thr_j5.condition(2)&physcond).setTriggerType(TT_calo)

        item_em3_mbtsveto_veto_te50 = LVL1MenuItem('L1_EM3_MV_VTE50', ctpid=-1, group='1', prescale=1)\
                            .setLogic(thr_em3.condition(1)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&Not(thr_te50.condition(1))\
                                                               &physcond).setTriggerType(TT_calo)

        item_mu4_veto_te50 = LVL1MenuItem('L1_MU4_VTE50', ctpid=-1).setLogic(thr_mu4.condition(1)\
                                                                                    &Not(thr_te50.condition(1))&physcond).setTriggerType(TT_rpcin)
        item_mu4_te50 = LVL1MenuItem('L1_MU4_TE50', ctpid=-1).setLogic(thr_mu4.condition(1)\
                                                                                    &thr_te50.condition(1)&physcond).setTriggerType(TT_rpcin)
        
        item_mu4_mbtsveto_veto_te50 = LVL1MenuItem('L1_MU4_MV_VTE50', ctpid=-1, group='1', prescale=1)\
                            .setLogic(thr_mu4.condition(1)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&Not(thr_te50.condition(1))\
                                                               &physcond).setTriggerType(TT_rpcin)
                
        item_em2_mbtsveto = LVL1MenuItem('L1_EM2_MV', ctpid=-1, group='1', prescale=1)\
                            .setLogic(thr_em2.condition(1)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&physcond).setTriggerType(TT_calo)
        item_em3_mbtsveto = LVL1MenuItem('L1_EM3_MV', ctpid=-1, group='1', prescale=1)\
                            .setLogic(thr_em3.condition(1)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&physcond).setTriggerType(TT_calo)
        item_tau5_mbtsveto = LVL1MenuItem('L1_TAU5_MV', ctpid=-1, group='1', prescale=1)\
                            .setLogic(thr_tau5.condition(1)&Not(thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&physcond).setTriggerType(TT_calo)

        item_em10_nolucid = LVL1MenuItem('L1_EM10_NL', ctpid=0x72, group='1', prescale=1)\
                           .setLogic(thr_em10.condition(1)&Not(thr_lucidA.condition(1))&Not(thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)

        item_mu6_nolucid = LVL1MenuItem('L1_MU6_NL', ctpid=0x72, group='1', prescale=1)\
                           .setLogic(thr_mu6.condition(1)&Not(thr_lucidA.condition(1))&Not(thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)

        item_2em3_nolucid = LVL1MenuItem('L1_2EM3_NL', ctpid=0x72, group='1', prescale=1)\
                           .setLogic(thr_em3.condition(2)&Not(thr_lucidA.condition(1))&Not(thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)

        item_em3_nolucid = LVL1MenuItem('L1_EM3_NL', ctpid=0x72, group='1', prescale=1)\
                            .setLogic(thr_em3.condition(1)&Not(thr_lucidA.condition(1))&Not(thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)

        item_2em5_nolucid = LVL1MenuItem('L1_2EM5_NL', ctpid=0x72, group='1', prescale=1)\
                           .setLogic(thr_em5.condition(2)&Not(thr_lucidA.condition(1))&Not(thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)

        item_2mu0_nolucid = LVL1MenuItem('L1_2MU0_NL', ctpid=0x72, group='1', prescale=1)\
                           .setLogic(thr_mu0.condition(2)&Not(thr_lucidA.condition(1))&Not(thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)

        item_mu0_nolucid = LVL1MenuItem('L1_MU0_NL', ctpid=0x72, group='1', prescale=1)\
                            .setLogic(thr_mu0.condition(1)&Not(thr_lucidA.condition(1))&Not(thr_lucidC.condition(1))&physcond).setTriggerType(TT_rpcin)

        item_2mu6_nolucid = LVL1MenuItem('L1_2MU6_NL', ctpid=0x72, group='1', prescale=1)\
                           .setLogic(thr_mu6.condition(2)&Not(thr_lucidA.condition(1))&Not(thr_lucidC.condition(1))&physcond).setTriggerType(TT_calo)

        #Hi stuff
        item_mu0_noZdc = LVL1MenuItem('L1_MU0_NZ', ctpid=0x72, group='1', prescale=1)\
                           .setLogic(thr_mu0.condition(1)&Not(thr_zdcAND.condition(1))&physcond).setTriggerType(TT_rpcin)

        item_em3_noZdc = LVL1MenuItem('L1_EM3_NZ', ctpid=0x72, group='1', prescale=1)\
                           .setLogic(thr_em3.condition(1)&Not(thr_zdcAND.condition(1))&physcond).setTriggerType(TT_calo)

        item_j15_noZdc = LVL1MenuItem('L1_J15_NZ', ctpid=0x72, group='1', prescale=1)\
                         .setLogic(thr_j15.condition(1)&Not(thr_zdcAND.condition(1))&physcond).setTriggerType(TT_calo)

        item_te5_noZdc  = LVL1MenuItem('L1_TE5_NZ',  ctpid=-1,   group='1').setLogic(thr_te5.condition(1)&Not(thr_zdcAND.condition(1))\
                                                                                     &physcond).setTriggerType(TT_calo)

        item_2mu0_noZdc = LVL1MenuItem('L1_2MU0_NZ', ctpid=0x72, group='1', prescale=1)\
                           .setLogic(thr_mu0.condition(2)&Not(thr_zdcAND.condition(1))&physcond).setTriggerType(TT_rpcin)
        
        item_2em3_noZdc = LVL1MenuItem('L1_2EM3_NZ', ctpid=0x72, group='1', prescale=1)\
                           .setLogic(thr_em3.condition(2)&Not(thr_zdcAND.condition(1))&physcond).setTriggerType(TT_calo)
        
        item_2j15_noZdc = LVL1MenuItem('L1_2J15_NZ', ctpid=0x72, group='1', prescale=1)\
                         .setLogic(thr_j15.condition(2)&Not(thr_zdcAND.condition(1))&physcond).setTriggerType(TT_calo)
        
        #
        # Add CALO items and muon items for EMPTY, UNPAIRED1, UNPAIRED2 and UNPAIREDR
        #
        item_calo_empty = LVL1MenuItem('L1_CALO_EMPTY', ctpid=-1, group='1')\
                          .setLogic((thr_j5.condition(1)|thr_em3.condition(1)|thr_tau5.condition(1))&cosmiccond).setTriggerType(TT_calo)

        item_calo_unpaired1 = LVL1MenuItem('L1_CALO_UNPAIRED1', ctpid=-1, group='1')\
                              .setLogic((thr_j5.condition(1)|thr_em3.condition(1)|thr_tau5.condition(1))&unpaired1cond).setTriggerType(TT_calo)

        item_calo_unpaired2 = LVL1MenuItem('L1_CALO_UNPAIRED2', ctpid=-1, group='1')\
                              .setLogic((thr_j5.condition(1)|thr_em3.condition(1)|thr_tau5.condition(1))&unpaired2cond).setTriggerType(TT_calo)
        
        item_j5_unpairedR   = LVL1MenuItem('L1_J5_UNPAIRED', ctpid=-1, group='1')\
                              .setLogic(thr_j5.condition(1)&unpairedRcond).setTriggerType(TT_calo)

        item_j5_unpaired_iso   = LVL1MenuItem('L1_J5_UNPAIRED_ISO', ctpid=-1, group='1')\
                                 .setLogic(thr_j5.condition(1)&unpaired_isocond).setTriggerType(TT_calo)

        item_j5_unpaired_noniso   = LVL1MenuItem('L1_J5_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                    .setLogic(thr_j5.condition(1)&unpaired_nonisocond).setTriggerType(TT_calo)

        item_j5_empty1stR = LVL1MenuItem('L1_J5_FIRSTEMPTY', ctpid=-1, group='1')\
                            .setLogic(thr_j5.condition(1)&empty1stcond).setTriggerType(TT_calo)
        
        item_j10_unpairedR   = LVL1MenuItem('L1_J10_UNPAIRED', ctpid=-1, group='1')\
                               .setLogic(thr_j10.condition(1)&unpairedRcond).setTriggerType(TT_calo)
        
        item_j30_unpairedR   = LVL1MenuItem('L1_J30_UNPAIRED', ctpid=-1, group='1')\
                               .setLogic(thr_j30.condition(1)&unpairedRcond).setTriggerType(TT_calo)

        item_j10_unpaired_iso   = LVL1MenuItem('L1_J10_UNPAIRED_ISO', ctpid=-1, group='1')\
                                  .setLogic(thr_j10.condition(1)&unpaired_isocond).setTriggerType(TT_calo)

        item_j10_unpaired_noniso   = LVL1MenuItem('L1_J10_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                     .setLogic(thr_j10.condition(1)&unpaired_nonisocond).setTriggerType(TT_calo)

        item_j30_unpaired_iso   = LVL1MenuItem('L1_J30_UNPAIRED_ISO', ctpid=-1, group='1')\
                                  .setLogic(thr_j30.condition(1)&unpaired_isocond).setTriggerType(TT_calo)

        item_j30_unpaired_noniso   = LVL1MenuItem('L1_J30_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                  .setLogic(thr_j30.condition(1)&unpaired_nonisocond).setTriggerType(TT_calo)

        item_fj3_unpairedR  = LVL1MenuItem('L1_FJ3_UNPAIRED', ctpid=-1, group='1')\
                              .setLogic((thr_jf3.condition(1) | thr_jb3.condition(1))&unpairedRcond).setTriggerType(TT_calo)
        
        item_fj5_unpairedR  = LVL1MenuItem('L1_FJ5_UNPAIRED', ctpid=-1, group='1')\
                              .setLogic((thr_jf5.condition(1) | thr_jb5.condition(1))&unpairedRcond).setTriggerType(TT_calo)

        item_fj10_unpairedR  = LVL1MenuItem('L1_FJ10_UNPAIRED', ctpid=-1, group='1')\
                              .setLogic((thr_jf10.condition(1) | thr_jb10.condition(1))&unpairedRcond).setTriggerType(TT_calo)

        item_fj10_unpaired_iso  = LVL1MenuItem('L1_FJ10_UNPAIRED_ISO', ctpid=-1, group='1')\
                                  .setLogic((thr_jf10.condition(1) | thr_jb10.condition(1))&unpaired_isocond).setTriggerType(TT_calo)

        item_fj10_unpaired_noniso  = LVL1MenuItem('L1_FJ10_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                  .setLogic((thr_jf10.condition(1) | thr_jb10.condition(1))&unpaired_nonisocond).setTriggerType(TT_calo)

        item_fj0_unpaired_iso  = LVL1MenuItem('L1_FJ0_UNPAIRED_ISO', ctpid=-1, group='1')\
                                  .setLogic((thr_jf0.condition(1) | thr_jb0.condition(1))&unpaired_isocond).setTriggerType(TT_calo)
        item_fj1_unpaired_iso  = LVL1MenuItem('L1_FJ1_UNPAIRED_ISO', ctpid=-1, group='1')\
                                  .setLogic((thr_jf1.condition(1) | thr_jb1.condition(1))&unpaired_isocond).setTriggerType(TT_calo)

        item_fj5_unpaired_iso  = LVL1MenuItem('L1_FJ5_UNPAIRED_ISO', ctpid=-1, group='1')\
                                  .setLogic((thr_jf5.condition(1) | thr_jb5.condition(1))&unpaired_isocond).setTriggerType(TT_calo)

        item_fj30_unpaired_iso  = LVL1MenuItem('L1_FJ30_UNPAIRED_ISO', ctpid=-1, group='1')\
                                  .setLogic((thr_jf30.condition(1) | thr_jb30.condition(1))&unpaired_isocond).setTriggerType(TT_calo)
        
        item_fj5_empty1stR = LVL1MenuItem('L1_FJ5_FIRSTEMPTY', ctpid=-1, group='1')\
                             .setLogic((thr_jf5.condition(1) | thr_jb5.condition(1))&empty1stcond).setTriggerType(TT_calo )

        item_fj10_empty1stR = LVL1MenuItem('L1_FJ10_FIRSTEMPTY', ctpid=-1, group='1')\
                             .setLogic((thr_jf10.condition(1) | thr_jb10.condition(1))&empty1stcond).setTriggerType(TT_calo )

        item_fj30_empty1stR = LVL1MenuItem('L1_FJ30_FIRSTEMPTY', ctpid=-1, group='1')\
                              .setLogic((thr_jf30.condition(1) | thr_jb30.condition(1))&empty1stcond).setTriggerType(TT_calo )
        

        item_tau5_unpairedR = LVL1MenuItem('L1_TAU5_UNPAIRED', ctpid=-1, group='1')\
                              .setLogic(thr_tau5.condition(1)&unpairedRcond).setTriggerType(TT_calo)

        item_tau5_unpaired_iso = LVL1MenuItem('L1_TAU5_UNPAIRED_ISO', ctpid=-1, group='1')\
                                 .setLogic(thr_tau5.condition(1)&unpaired_isocond).setTriggerType(TT_calo)

        item_tau5_unpaired_noniso = LVL1MenuItem('L1_TAU5_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                    .setLogic(thr_tau5.condition(1)&unpaired_nonisocond).setTriggerType(TT_calo)

        item_tau5_empty1stR = LVL1MenuItem('L1_TAU5_FIRSTEMPTY', ctpid=-1, group='1')\
                                 .setLogic(thr_tau5.condition(1)&empty1stcond).setTriggerType(TT_calo)

        item_tau8_unpaired_iso = LVL1MenuItem('L1_TAU8_UNPAIRED_ISO', ctpid=-1, group='1')\
                                 .setLogic(thr_tau8.condition(1)&unpaired_isocond).setTriggerType(TT_calo)

        item_tau8_unpaired_noniso = LVL1MenuItem('L1_TAU8_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                    .setLogic(thr_tau8.condition(1)&unpaired_nonisocond).setTriggerType(TT_calo)

        item_tau8_empty1stR = LVL1MenuItem('L1_TAU8_FIRSTEMPTY', ctpid=-1, group='1')\
                                 .setLogic(thr_tau8.condition(1)&empty1stcond).setTriggerType(TT_calo)
        
        item_em2_unpairedR = LVL1MenuItem('L1_EM2_UNPAIRED', ctpid=-1, group='1')\
                             .setLogic(thr_em2.condition(1)&unpairedRcond).setTriggerType(TT_calo|TT_caloem)

        item_em2_unpaired_iso = LVL1MenuItem('L1_EM2_UNPAIRED_ISO', ctpid=-1, group='1')\
                                .setLogic(thr_em2.condition(1)&unpaired_isocond).setTriggerType(TT_calo|TT_caloem)

        item_em2_unpaired_noniso = LVL1MenuItem('L1_EM2_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                   .setLogic(thr_em2.condition(1)&unpaired_nonisocond).setTriggerType(TT_calo|TT_caloem)

        item_em3_unpaired_iso = LVL1MenuItem('L1_EM3_UNPAIRED_ISO', ctpid=-1, group='1')\
                                .setLogic(thr_em3.condition(1)&unpaired_isocond).setTriggerType(TT_calo|TT_caloem)

        item_em3_unpaired_noniso = LVL1MenuItem('L1_EM3_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                   .setLogic(thr_em3.condition(1)&unpaired_nonisocond).setTriggerType(TT_calo|TT_caloem)


        item_em3_unpairedR = LVL1MenuItem('L1_EM3_UNPAIRED', ctpid=-1, group='1')\
                             .setLogic(thr_em3.condition(1)&unpairedRcond).setTriggerType(TT_calo|TT_caloem)
        
        item_em2_empty1stR = LVL1MenuItem('L1_EM2_FIRSTEMPTY', ctpid=-1, group='1')\
                             .setLogic(thr_em2.condition(1)&empty1stcond).setTriggerType(TT_calo|TT_caloem)
        item_em3_empty1stR = LVL1MenuItem('L1_EM3_FIRSTEMPTY', ctpid=-1, group='1')\
                             .setLogic(thr_em3.condition(1)&empty1stcond).setTriggerType(TT_calo|TT_caloem)
        
        item_mu0_unpairedR = LVL1MenuItem('L1_MU0_UNPAIRED', ctpid=-1, group='1')\
                             .setLogic(thr_mu0.condition(1)&unpairedRcond).setTriggerType(TT_rpcin)

        item_2mu6_unpairedR = LVL1MenuItem('L1_2MU6_UNPAIRED', ctpid=-1, group='1')\
                              .setLogic(thr_mu6.condition(2)&unpairedRcond).setTriggerType(TT_rpcin)
        
        item_2mu6_unpaired_iso = LVL1MenuItem('L1_2MU6_UNPAIRED_ISO', ctpid=-1, group='1')\
                                 .setLogic(thr_mu6.condition(2)&unpaired_isocond).setTriggerType(TT_rpcin)

        item_2mu6_unpaired_noniso = LVL1MenuItem('L1_2MU6_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                   .setLogic(thr_mu6.condition(2)&unpaired_nonisocond).setTriggerType(TT_rpcin)

        item_mu0_j15_unpairedR = LVL1MenuItem('L1_MU0_J15_UNPAIRED', ctpid=-1, group='1')\
                                 .setLogic(thr_mu0.condition(1)&thr_j15.condition(1)&unpairedRcond).setTriggerType(TT_rpcin)
        
        item_mu0_j15_unpaired_iso = LVL1MenuItem('L1_MU0_J15_UNPAIRED_ISO', ctpid=-1, group='1')\
                                    .setLogic(thr_mu0.condition(1)&thr_j15.condition(1)&unpaired_isocond).setTriggerType(TT_rpcin)
        
        item_mu0_j15_unpaired_noniso = LVL1MenuItem('L1_MU0_J15_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                       .setLogic(thr_mu0.condition(1)&thr_j15.condition(1)&unpaired_nonisocond).setTriggerType(TT_rpcin)

        item_mu4_j15_unpaired_iso = LVL1MenuItem('L1_MU4_J15_UNPAIRED_ISO', ctpid=-1, group='1')\
                                    .setLogic(thr_mu4.condition(1)&thr_j15.condition(1)&unpaired_isocond).setTriggerType(TT_rpcin)
        
        item_mu4_j15_unpaired_noniso = LVL1MenuItem('L1_MU4_J15_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                       .setLogic(thr_mu4.condition(1)&thr_j15.condition(1)&unpaired_nonisocond).setTriggerType(TT_rpcin)
        
        item_mu0_unpaired_iso = LVL1MenuItem('L1_MU0_UNPAIRED_ISO', ctpid=-1, group='1')\
                                .setLogic(thr_mu0.condition(1)&unpaired_isocond).setTriggerType(TT_rpcin)

        item_mu4_unpaired_iso = LVL1MenuItem('L1_MU4_UNPAIRED_ISO', ctpid=-1, group='1')\
                                .setLogic(thr_mu4.condition(1)&unpaired_isocond).setTriggerType(TT_rpcin)

        item_mu10_unpaired_iso = LVL1MenuItem('L1_MU10_UNPAIRED_ISO', ctpid=-1, group='1')\
                                 .setLogic(thr_mu10.condition(1)&unpaired_isocond).setTriggerType(TT_rpcin)

        item_mu0_unpaired_noniso = LVL1MenuItem('L1_MU0_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                   .setLogic(thr_mu0.condition(1)&unpaired_nonisocond).setTriggerType(TT_rpcin)

        item_mu4_unpaired_noniso = LVL1MenuItem('L1_MU4_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                   .setLogic(thr_mu4.condition(1)&unpaired_nonisocond).setTriggerType(TT_rpcin)

        item_mu0_empty1stR = LVL1MenuItem('L1_MU0_FIRSTEMPTY', ctpid=-1, group='1')\
                             .setLogic(thr_mu0.condition(1)&empty1stcond).setTriggerType(TT_rpcin)

        item_mu4_empty1stR = LVL1MenuItem('L1_MU4_FIRSTEMPTY', ctpid=-1, group='1')\
                             .setLogic(thr_mu4.condition(1)&empty1stcond).setTriggerType(TT_rpcin)
        
        item_mu6_unpairedR = LVL1MenuItem('L1_MU6_UNPAIRED', ctpid=-1, group='1')\
                             .setLogic(thr_mu6.condition(1)&unpairedRcond).setTriggerType(TT_rpcin)
        
        item_mu6_empty1stR = LVL1MenuItem('L1_MU6_FIRSTEMPTY', ctpid=-1, group='1')\
                             .setLogic(thr_mu6.condition(1)&empty1stcond).setTriggerType(TT_rpcin)
        
        item_mu10_empty1stR = LVL1MenuItem('L1_MU10_FIRSTEMPTY', ctpid=-1, group='1')\
                              .setLogic(thr_mu10.condition(1)&empty1stcond).setTriggerType(TT_rpcin)
        
        item_2mu0_empty1stR = LVL1MenuItem('L1_2MU0_FIRSTEMPTY', ctpid=-1, group='1')\
                              .setLogic(thr_mu0.condition(2)&empty1stcond).setTriggerType(TT_rpcin)

        item_2mu4_empty1stR = LVL1MenuItem('L1_2MU4_FIRSTEMPTY', ctpid=-1, group='1')\
                              .setLogic(thr_mu4.condition(2)&empty1stcond).setTriggerType(TT_rpcin)

        item_2mu6_empty1stR = LVL1MenuItem('L1_2MU6_FIRSTEMPTY', ctpid=-1, group='1')\
                              .setLogic(thr_mu6.condition(2)&empty1stcond).setTriggerType(TT_rpcin)        

        item_mu0_j15_empty1stR = LVL1MenuItem('L1_MU0_J15_FIRSTEMPTY', ctpid=-1, group='1')\
                              .setLogic(thr_mu0.condition(1)&thr_j15.condition(1)&empty1stcond).setTriggerType(TT_rpcin)

        item_mu4_j15_empty1stR = LVL1MenuItem('L1_MU4_J15_FIRSTEMPTY', ctpid=-1, group='1')\
                                 .setLogic(thr_mu4.condition(1)&thr_j15.condition(1)&empty1stcond).setTriggerType(TT_rpcin)        

        item_mu15_unpairedR = LVL1MenuItem('L1_MU15_UNPAIRED', ctpid=-1, group='1')\
                              .setLogic(thr_mu15.condition(1)&unpairedRcond).setTriggerType(TT_rpcin)
        
        item_mu0_comm_unpaired = LVL1MenuItem('L1_MU0_COMM_UNPAIRED', ctpid=-1, group='1')\
                                 .setLogic(thr_mu0_comm.condition(1)&unpairedRcond).setTriggerType(TT_rpcin)
        
        item_mu0_comm_unpaired_iso = LVL1MenuItem('L1_MU0_COMM_UNPAIRED_ISO', ctpid=-1, group='1')\
                                     .setLogic(thr_mu0_comm.condition(1)&unpaired_isocond).setTriggerType(TT_rpcin)

        item_mu0_comm_unpaired_noniso = LVL1MenuItem('L1_MU0_COMM_UNPAIRED_NONISO', ctpid=-1, group='1')\
                                        .setLogic(thr_mu0_comm.condition(1)&unpaired_nonisocond).setTriggerType(TT_rpcin)

        item_mu0_comm_empty1stR = LVL1MenuItem('L1_MU0_COMM_FIRSTEMPTY', ctpid=-1, group='1')\
                                  .setLogic(thr_mu0_comm.condition(1)&empty1stcond).setTriggerType(TT_rpcin)
        
        item_j10_empty1stR = LVL1MenuItem('L1_J10_FIRSTEMPTY', ctpid=-1, group='1')\
                             .setLogic(thr_j10.condition(1)&empty1stcond).setTriggerType(TT_calo)
        
        item_j30_empty1stR = LVL1MenuItem('L1_J30_FIRSTEMPTY', ctpid=-1, group='1')\
                             .setLogic(thr_j30.condition(1)&empty1stcond).setTriggerType(TT_calo)
        
        # XE ctpid=[0x80:0x9f]
        item_xe10 = LVL1MenuItem('L1_XE10', ctpid=-1, group='1').setLogic(thr_xe10.condition(1)&physcond).setTriggerType(TT_calo)
        item_xe15 = LVL1MenuItem('L1_XE15', ctpid=0x80, group='1').setLogic(thr_xe15.condition(1)&physcond).setTriggerType(TT_calo)
        item_xe20 = LVL1MenuItem('L1_XE20', ctpid=0x81, group='1').setLogic(thr_xe20.condition(1)&physcond).setTriggerType(TT_calo)
        item_xe25 = LVL1MenuItem('L1_XE25', ctpid=0x82, group='1').setLogic(thr_xe25.condition(1)&physcond).setTriggerType(TT_calo)
        item_xe30 = LVL1MenuItem('L1_XE30', ctpid=0x83, group='1').setLogic(thr_xe30.condition(1)&physcond).setTriggerType(TT_calo)
        item_xe40 = LVL1MenuItem('L1_XE40', ctpid=0x84, group='1').setLogic(thr_xe40.condition(1)&physcond).setTriggerType(TT_calo)
        item_xe50 = LVL1MenuItem('L1_XE50', ctpid=0x85, group='1').setLogic(thr_xe50.condition(1)&physcond).setTriggerType(TT_calo)
        item_xe70 = LVL1MenuItem('L1_XE70', ctpid=0x86, group='1').setLogic(thr_xe70.condition(1)&physcond).setTriggerType(TT_calo)
        item_xe80 = LVL1MenuItem('L1_XE80', ctpid=0x87, group='1').setLogic(thr_xe80.condition(1)&physcond).setTriggerType(TT_calo)

        item_xe100 = LVL1MenuItem('L1_XE100', ctpid=0x88, group='1').setLogic(thr_xe100.condition(1)&physcond).setTriggerType(TT_calo)
        item_xe120 = LVL1MenuItem('L1_XE120', ctpid=0x89, group='1').setLogic(thr_xe120.condition(1)&physcond).setTriggerType(TT_calo)         
        item_xe125 = LVL1MenuItem('L1_XE125', ctpid=0x89, group='1').setLogic(thr_xe125.condition(1)&physcond).setTriggerType(TT_calo)

        item_xe10_empty = LVL1MenuItem('L1_XE10_EMPTY', ctpid=-1, group='1').setLogic(thr_xe10.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_xe15_empty = LVL1MenuItem('L1_XE15_EMPTY', ctpid=-1, group='1').setLogic(thr_xe15.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_xe20_empty = LVL1MenuItem('L1_XE20_EMPTY', ctpid=-1, group='1').setLogic(thr_xe20.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_xe25_empty = LVL1MenuItem('L1_XE25_EMPTY', ctpid=-1, group='1').setLogic(thr_xe25.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_xe30_empty = LVL1MenuItem('L1_XE30_EMPTY', ctpid=-1, group='1').setLogic(thr_xe30.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_xe35_empty = LVL1MenuItem('L1_XE35_EMPTY', ctpid=-1, group='1').setLogic(thr_xe35.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_xe50_empty = LVL1MenuItem('L1_XE50_EMPTY', ctpid=-1, group='1').setLogic(thr_xe50.condition(1)&cosmiccond).setTriggerType(TT_calo)

        item_xe35_bgrp7 = LVL1MenuItem('L1_XE35_BGRP7', ctpid=-1, group='1').setLogic(thr_xe35.condition(1)&bgrp7cond).setTriggerType(TT_calo)
        item_xe40_bgrp7 = LVL1MenuItem('L1_XE40_BGRP7', ctpid=-1, group='1').setLogic(thr_xe40.condition(1)&bgrp7cond).setTriggerType(TT_calo)
        item_xe50_bgrp7 = LVL1MenuItem('L1_XE50_BGRP7', ctpid=-1, group='1').setLogic(thr_xe50.condition(1)&bgrp7cond).setTriggerType(TT_calo)

        #pPb items

        item_te1  = LVL1MenuItem('L1_TE1',  ctpid=-1,   group='1').setLogic(thr_te1.condition(1)&physcond).setTriggerType(TT_calo)
        item_te2  = LVL1MenuItem('L1_TE2',  ctpid=-1,   group='1').setLogic(thr_te2.condition(1)&physcond).setTriggerType(TT_calo)
        item_te3  = LVL1MenuItem('L1_TE3',  ctpid=-1,   group='1').setLogic(thr_te3.condition(1)&physcond).setTriggerType(TT_calo)
        item_te4  = LVL1MenuItem('L1_TE4',  ctpid=-1,   group='1').setLogic(thr_te4.condition(1)&physcond).setTriggerType(TT_calo)
        
        item_te10  = LVL1MenuItem('L1_TE10',  ctpid=-1,   group='1').setLogic(thr_te10.condition(1)&physcond).setTriggerType(TT_calo)
        item_te20  = LVL1MenuItem('L1_TE20',  ctpid=-1,   group='1').setLogic(thr_te20.condition(1)&physcond).setTriggerType(TT_calo)
        item_te30  = LVL1MenuItem('L1_TE30',  ctpid=-1,   group='1').setLogic(thr_te30.condition(1)&physcond).setTriggerType(TT_calo)
        item_te35  = LVL1MenuItem('L1_TE35',  ctpid=-1,   group='1').setLogic(thr_te35.condition(1)&physcond).setTriggerType(TT_calo)
        item_te40  = LVL1MenuItem('L1_TE40',  ctpid=-1,   group='1').setLogic(thr_te40.condition(1)&physcond).setTriggerType(TT_calo)

        item_te150 = LVL1MenuItem('L1_TE150', ctpid=0x8a, group='1').setLogic(thr_te150.condition(1)&physcond).setTriggerType(TT_calo)
        item_te180 = LVL1MenuItem('L1_TE180', ctpid=-1,   group='1').setLogic(thr_te180.condition(1)&physcond).setTriggerType(TT_calo)        
        item_te250 = LVL1MenuItem('L1_TE250', ctpid=0x8b, group='1').setLogic(thr_te250.condition(1)&physcond).setTriggerType(TT_calo)
        item_te360 = LVL1MenuItem('L1_TE360', ctpid=0x8c, group='1').setLogic(thr_te360.condition(1)&physcond).setTriggerType(TT_calo)
        item_te650 = LVL1MenuItem('L1_TE650', ctpid=0x8d, group='1').setLogic(thr_te650.condition(1)&physcond).setTriggerType(TT_calo)
        # Other TE items for L=10^32 and 10^33
        item_te530 = LVL1MenuItem('L1_TE530', ctpid=0x8e, group='1').setLogic(thr_te530.condition(1)&physcond).setTriggerType(TT_calo)
        item_te800 = LVL1MenuItem('L1_TE800', ctpid=0x8f, group='1').setLogic(thr_te800.condition(1)&physcond).setTriggerType(TT_calo)
        item_te1000 = LVL1MenuItem('L1_TE1000', ctpid=0x90, group='1').setLogic(thr_te1000.condition(1)&physcond).setTriggerType(TT_calo)
        item_te2000 = LVL1MenuItem('L1_TE2000', ctpid=-1, group='1').setLogic(thr_te2000.condition(1)&physcond).setTriggerType(TT_calo)
        item_te4000 = LVL1MenuItem('L1_TE4000', ctpid=-1, group='1').setLogic(thr_te4000.condition(1)&physcond).setTriggerType(TT_calo)


        # items for HI p+Pb (forward TE)
        if 'HI_v2' in TriggerFlags.triggerMenuSetup():
            item_te0 = LVL1MenuItem('L1_TE0', ctpid=-1, group='1', prescale=1)\
                       .setLogic(thr_te0.condition(1) & physcond).setTriggerType(TT_calo)
            item_te5 = LVL1MenuItem('L1_TE5', ctpid=-1, group='1', prescale=1)\
                       .setLogic(thr_te5.condition(1) & physcond).setTriggerType(TT_calo)
            item_te20 = LVL1MenuItem('L1_TE20', ctpid=-1, group='1', prescale=1)\
                        .setLogic(thr_te20.condition(1) & physcond).setTriggerType(TT_calo)
            item_te50 = LVL1MenuItem('L1_TE50', ctpid=-1, group='1', prescale=1)\
                        .setLogic(thr_te50.condition(1) & physcond).setTriggerType(TT_calo)
            item_te65 = LVL1MenuItem('L1_TE65', ctpid=-1, group='1', prescale=1)\
                        .setLogic(thr_te65.condition(1) & physcond).setTriggerType(TT_calo)
            item_te90 = LVL1MenuItem('L1_TE90', ctpid=-1, group='1', prescale=1)\
                        .setLogic(thr_te90.condition(1) & physcond).setTriggerType(TT_calo)
            item_te100 = LVL1MenuItem('L1_TE100', ctpid=-1, group='1', prescale=1)\
                         .setLogic(thr_te100.condition(1) & physcond).setTriggerType(TT_calo)
            item_te115 = LVL1MenuItem('L1_TE115', ctpid=-1, group='1', prescale=1)\
                         .setLogic(thr_te115.condition(1) & physcond).setTriggerType(TT_calo)
        else:
            item_te5  = LVL1MenuItem('L1_TE5',  ctpid=-1,   group='1').setLogic(thr_te5.condition(1)&physcond).setTriggerType(TT_calo)
            item_te0  = LVL1MenuItem('L1_TE0',  ctpid=-1,   group='1').setLogic(thr_te0.condition(1)&physcond).setTriggerType(TT_calo)
            item_te50  = LVL1MenuItem('L1_TE50',  ctpid=-1,   group='1').setLogic(thr_te50.condition(1)&physcond).setTriggerType(TT_calo)
            item_te65  = LVL1MenuItem('L1_TE65',  ctpid=-1,   group='1').setLogic(thr_te65.condition(1)&physcond).setTriggerType(TT_calo)
            item_te90  = LVL1MenuItem('L1_TE90',  ctpid=-1,   group='1').setLogic(thr_te90.condition(1)&physcond).setTriggerType(TT_calo)
            item_te100 = LVL1MenuItem('L1_TE100', ctpid=-1,   group='1').setLogic(thr_te100.condition(1)&physcond).setTriggerType(TT_calo)
            item_te115  = LVL1MenuItem('L1_TE115',  ctpid=-1,   group='1').setLogic(thr_te115.condition(1)&physcond).setTriggerType(TT_calo)
            

        item_xs15 = LVL1MenuItem('L1_XS15', ctpid=-1, group='1').setLogic(thr_xs15.condition(1)&physcond).setTriggerType(TT_calo)
        item_xs20 = LVL1MenuItem('L1_XS20', ctpid=-1, group='1').setLogic(thr_xs20.condition(1)&physcond).setTriggerType(TT_calo)
        item_xs25 = LVL1MenuItem('L1_XS25', ctpid=-1, group='1').setLogic(thr_xs25.condition(1)&physcond).setTriggerType(TT_calo)
        item_xs30 = LVL1MenuItem('L1_XS30', ctpid=-1, group='1').setLogic(thr_xs30.condition(1)&physcond).setTriggerType(TT_calo)
        item_xs35 = LVL1MenuItem('L1_XS35', ctpid=-1, group='1').setLogic(thr_xs35.condition(1)&physcond).setTriggerType(TT_calo)
        item_xs40 = LVL1MenuItem('L1_XS40', ctpid=-1, group='1').setLogic(thr_xs40.condition(1)&physcond).setTriggerType(TT_calo)
        item_xs45 = LVL1MenuItem('L1_XS45', ctpid=-1, group='1').setLogic(thr_xs45.condition(1)&physcond).setTriggerType(TT_calo)
        item_xs50 = LVL1MenuItem('L1_XS50', ctpid=-1, group='1').setLogic(thr_xs50.condition(1)&physcond).setTriggerType(TT_calo)
        item_xs55 = LVL1MenuItem('L1_XS55', ctpid=-1, group='1').setLogic(thr_xs55.condition(1)&physcond).setTriggerType(TT_calo)
        item_xs60 = LVL1MenuItem('L1_XS60', ctpid=-1, group='1').setLogic(thr_xs60.condition(1)&physcond).setTriggerType(TT_calo)
        item_xs65 = LVL1MenuItem('L1_XS65', ctpid=-1, group='1').setLogic(thr_xs65.condition(1)&physcond).setTriggerType(TT_calo)
        
        item_te10_empty  = LVL1MenuItem('L1_TE10_EMPTY',  ctpid=-1, group='1').setLogic(thr_te10 .condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_te30_empty  = LVL1MenuItem('L1_TE30_EMPTY',  ctpid=-1, group='1').setLogic(thr_te30 .condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_te100_empty = LVL1MenuItem('L1_TE100_EMPTY', ctpid=-1, group='1').setLogic(thr_te100.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_te150_empty = LVL1MenuItem('L1_TE150_EMPTY', ctpid=-1, group='1').setLogic(thr_te150.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_te180_empty = LVL1MenuItem('L1_TE180_EMPTY', ctpid=-1, group='1').setLogic(thr_te180.condition(1)&cosmiccond).setTriggerType(TT_calo)        
        item_te360_empty = LVL1MenuItem('L1_TE360_EMPTY', ctpid=-1, group='1').setLogic(thr_te360.condition(1)&cosmiccond).setTriggerType(TT_calo)
        
        item_je60  = LVL1MenuItem('L1_JE60', ctpid=0x91, group='1').setLogic(thr_je60.condition(1)&physcond).setTriggerType(TT_calo)
        item_je100 = LVL1MenuItem('L1_JE100', ctpid=0x91, group='1').setLogic(thr_je100.condition(1)&physcond).setTriggerType(TT_calo)
        item_je120 = LVL1MenuItem('L1_JE120', ctpid=0x91, group='1').setLogic(thr_je120.condition(1)&physcond).setTriggerType(TT_calo)
        item_je140 = LVL1MenuItem('L1_JE140', ctpid=-1, group='1').setLogic(thr_je140.condition(1)&physcond).setTriggerType(TT_calo)
        item_je200 = LVL1MenuItem('L1_JE200', ctpid=0x92, group='1').setLogic(thr_je200.condition(1)&physcond).setTriggerType(TT_calo)
        item_je220 = LVL1MenuItem('L1_JE220', ctpid=0x92, group='1').setLogic(thr_je220.condition(1)&physcond).setTriggerType(TT_calo)
        item_je280 = LVL1MenuItem('L1_JE280', ctpid=0x93, group='1').setLogic(thr_je280.condition(1)&physcond).setTriggerType(TT_calo)
        item_je300 = LVL1MenuItem('L1_JE300', ctpid=0x93, group='1').setLogic(thr_je300.condition(1)&physcond).setTriggerType(TT_calo)
        item_je340 = LVL1MenuItem('L1_JE340', ctpid=0x94, group='1').setLogic(thr_je340.condition(1)&physcond).setTriggerType(TT_calo)
        item_je350 = LVL1MenuItem('L1_JE350', ctpid=0x94, group='1').setLogic(thr_je350.condition(1)&physcond).setTriggerType(TT_calo)
        item_je400 = LVL1MenuItem('L1_JE400', ctpid=0x95, group='1').setLogic(thr_je400.condition(1)&physcond).setTriggerType(TT_calo)
        item_je500 = LVL1MenuItem('L1_JE500', ctpid=0x96, group='1').setLogic(thr_je500.condition(1)&physcond).setTriggerType(TT_calo)

        item_je120_empty = LVL1MenuItem('L1_JE120_EMPTY', ctpid=-1, group='1').setLogic(thr_je120.condition(1)&cosmiccond).setTriggerType(TT_calo)
        item_je280_empty = LVL1MenuItem('L1_JE280_EMPTY', ctpid=-1, group='1').setLogic(thr_je280.condition(1)&cosmiccond).setTriggerType(TT_calo)

        #'L1_6J10_JE220', 'L1_4J20_JE220', 'L1_3J40_JE220',
        item_6j10_je220 = LVL1MenuItem('L1_6J10_JE220', ctpid=-1, group='1')\
                          .setLogic(thr_j10.condition(6)&thr_je220.condition(1)&physcond).setTriggerType(TT_calo)
        item_4j20_je220 = LVL1MenuItem('L1_4J20_JE220', ctpid=-1, group='1')\
                          .setLogic(thr_j20.condition(4)&thr_je220.condition(1)&physcond).setTriggerType(TT_calo)
        item_3j40_je220 = LVL1MenuItem('L1_3J40_JE220', ctpid=-1, group='1')\
                          .setLogic(thr_j40.condition(3)&thr_je220.condition(1)&physcond).setTriggerType(TT_calo)
        item_4j15_je220 = LVL1MenuItem('L1_4J15_JE220', ctpid=-1, group='1')\
                          .setLogic(thr_j15.condition(4)&thr_je220.condition(1)&physcond).setTriggerType(TT_calo)
        item_3j30_je220 = LVL1MenuItem('L1_3J30_JE220', ctpid=-1, group='1')\
                          .setLogic(thr_j30.condition(3)&thr_je220.condition(1)&physcond).setTriggerType(TT_calo)
        
        # Transient items
        item_xe35 = LVL1MenuItem('L1_XE35', ctpid=-1, group='1').setLogic(thr_xe35.condition(1)&physcond).setTriggerType(TT_calo)
        item_xe45 = LVL1MenuItem('L1_XE45', ctpid=-1, group='1').setLogic(thr_xe45.condition(1)&physcond).setTriggerType(TT_calo)
        item_xe60 = LVL1MenuItem('L1_XE60', ctpid=-1, group='1').setLogic(thr_xe60.condition(1)&physcond).setTriggerType(TT_calo)
        item_xe90 = LVL1MenuItem('L1_XE90', ctpid=-1, group='1').setLogic(thr_xe90.condition(1)&physcond).setTriggerType(TT_calo)
        item_xe110 = LVL1MenuItem('L1_XE110', ctpid=-1, group='1').setLogic(thr_xe110.condition(1)&physcond).setTriggerType(TT_calo)
#        item_xe150 = LVL1MenuItem('L1_XE150', ctpid=-1, group='1').setLogic(thr_xe150.condition(1)&physcond).setTriggerType(TT_calo)
        item_te150 = LVL1MenuItem('L1_TE150', ctpid=-1, group='1').setLogic(thr_te150.condition(1)&physcond).setTriggerType(TT_calo)
        item_te240 = LVL1MenuItem('L1_TE240', ctpid=-1, group='1').setLogic(thr_te240.condition(1)&physcond).setTriggerType(TT_calo)
        item_te300 = LVL1MenuItem('L1_TE300', ctpid=-1, group='1').setLogic(thr_te300.condition(1)&physcond).setTriggerType(TT_calo)
        item_te400 = LVL1MenuItem('L1_TE400', ctpid=-1, group='1').setLogic(thr_te400.condition(1)&physcond).setTriggerType(TT_calo)
        item_te500 = LVL1MenuItem('L1_TE500', ctpid=-1, group='1').setLogic(thr_te500.condition(1)&physcond).setTriggerType(TT_calo)
        item_te550 = LVL1MenuItem('L1_TE550', ctpid=-1, group='1').setLogic(thr_te550.condition(1)&physcond).setTriggerType(TT_calo)
        item_te480 = LVL1MenuItem('L1_TE480', ctpid=-1, group='1').setLogic(thr_te480.condition(1)&physcond).setTriggerType(TT_calo)
        item_te600 = LVL1MenuItem('L1_TE600', ctpid=-1, group='1').setLogic(thr_te600.condition(1)&physcond).setTriggerType(TT_calo)
        item_te700 = LVL1MenuItem('L1_TE700', ctpid=-1, group='1').setLogic(thr_te700.condition(1)&physcond).setTriggerType(TT_calo)
        item_te650 = LVL1MenuItem('L1_TE650', ctpid=-1, group='1').setLogic(thr_te650.condition(1)&physcond).setTriggerType(TT_calo)
        item_te750 = LVL1MenuItem('L1_TE750', ctpid=-1, group='1').setLogic(thr_te750.condition(1)&physcond).setTriggerType(TT_calo)
        item_te900 = LVL1MenuItem('L1_TE900', ctpid=-1, group='1').setLogic(thr_te900.condition(1)&physcond).setTriggerType(TT_calo)

        
        # Combined items [0xa0:0xff], [0x30:0x3f],

        # EM + (XE, MU, TAU)
        item_em10_xe15 = LVL1MenuItem('L1_EM10_XE15', ctpid=0xa0)\
                         .setLogic(thr_em10.condition(1)&thr_xe15.condition(1)&physcond)
        item_em10_xe20 = LVL1MenuItem('L1_EM10_XE20', ctpid=0xa0)\
                         .setLogic(thr_em10.condition(1)&thr_xe20.condition(1)&physcond)
        item_em10vh_xe20 = LVL1MenuItem('L1_EM10VH_XE20', ctpid=-1)\
                           .setLogic(thr_em10vh.condition(1)&thr_xe20.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em10h_xe20 = LVL1MenuItem('L1_EM10H_XE20', ctpid=-1)\
                           .setLogic(thr_em10h.condition(1)&thr_xe20.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em11h_xe20 = LVL1MenuItem('L1_EM11H_XE20', ctpid=-1)\
                          .setLogic(thr_em11h.condition(1)&thr_xe20.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em10_xe30 = LVL1MenuItem('L1_EM10_XE30', ctpid=-1)\
                         .setLogic(thr_em10.condition(1)&thr_xe30.condition(1)&physcond)
        item_em10vh_xe30 = LVL1MenuItem('L1_EM10VH_XE30', ctpid=-1)\
                           .setLogic(thr_em10vh.condition(1)&thr_xe30.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em10h_xe30 = LVL1MenuItem('L1_EM10H_XE30', ctpid=-1)\
                           .setLogic(thr_em10h.condition(1)&thr_xe30.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em11h_xe30 = LVL1MenuItem('L1_EM11H_XE30', ctpid=-1)\
                          .setLogic(thr_em11h.condition(1)&thr_xe30.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em13_xe15 = LVL1MenuItem('L1_EM13_XE15', ctpid=0xa0)\
                         .setLogic(thr_em13.condition(1)&thr_xe15.condition(1)&physcond)
        item_em10_xe10 = LVL1MenuItem('L1_EM10_XE10', ctpid=0xa0)\
                         .setLogic(thr_em10.condition(1)&thr_xe10.condition(1)&physcond)
        item_em14_xe10 = LVL1MenuItem('L1_EM14_XE10', ctpid=-1)\
                         .setLogic(thr_em14.condition(1)&thr_xe10.condition(1)&physcond)        
        item_em10vh_xe35 = LVL1MenuItem('L1_EM10VH_XE35', ctpid=-1)\
                           .setLogic(thr_em10vh.condition(1)&thr_xe35.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)        
        item_em10h_xe35 = LVL1MenuItem('L1_EM10H_XE35', ctpid=-1)\
                           .setLogic(thr_em10h.condition(1)&thr_xe35.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em11h_xe35 = LVL1MenuItem('L1_EM11H_XE35', ctpid=-1)\
                          .setLogic(thr_em11h.condition(1)&thr_xe35.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em18_xe15 = LVL1MenuItem('L1_EM18_XE15', ctpid=0xa1)\
                         .setLogic(thr_em18.condition(1)&thr_xe15.condition(1)&physcond)
        item_em5_mu0 = LVL1MenuItem('L1_EM5_MU0', ctpid=0xa2)\
                       .setLogic(thr_em5.condition(1)&thr_mu0.condition(1)&physcond)
        item_em10_mu0 = LVL1MenuItem('L1_EM10_MU0', ctpid=0xa2)\
                        .setLogic(thr_em10.condition(1)&thr_mu0.condition(1)&physcond)
        item_em10_mu6 = LVL1MenuItem('L1_EM10_MU6', ctpid=0xa2)\
                        .setLogic(thr_em10.condition(1)&thr_mu6.condition(1)&physcond)
        item_em10vh_mu6 = LVL1MenuItem('L1_EM10VH_MU6', ctpid=-1)\
                          .setLogic(thr_em10vh.condition(1)&thr_mu6.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em10h_mu6 = LVL1MenuItem('L1_EM10H_MU6', ctpid=-1)\
                          .setLogic(thr_em10h.condition(1)&thr_mu6.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em11h_mu6 = LVL1MenuItem('L1_EM11H_MU6', ctpid=-1)\
                         .setLogic(thr_em11h.condition(1)&thr_mu6.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em16vh_mu4 = LVL1MenuItem('L1_EM16VH_MU4', ctpid=-1)\
                          .setLogic(thr_em16vh.condition(1)&thr_mu4.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em16h_mu4 = LVL1MenuItem('L1_EM16H_MU4', ctpid=-1)\
                          .setLogic(thr_em16h.condition(1)&thr_mu4.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_em18h_mu4 = LVL1MenuItem('L1_EM18H_MU4', ctpid=-1)\
                         .setLogic(thr_em18h.condition(1)&thr_mu4.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2em5_mu0 = LVL1MenuItem('L1_2EM5_MU0', ctpid=0xa2)\
                        .setLogic(thr_em5.condition(2)&thr_mu0.condition(1)&physcond)
        item_2em5_mu6 = LVL1MenuItem('L1_2EM5_MU6', ctpid=0xa2)\
                        .setLogic(thr_em5.condition(2)&thr_mu6.condition(1)&physcond)
        item_2em6_mu6 = LVL1MenuItem('L1_2EM6_MU6', ctpid=-1)\
                        .setLogic(thr_em6.condition(2)&thr_mu6.condition(1)&physcond)
        item_2em7_mu6 = LVL1MenuItem('L1_2EM7_MU6', ctpid=0xa2)\
                        .setLogic(thr_em7.condition(2)&thr_mu6.condition(1)&physcond)
        item_em13i_mu10i = LVL1MenuItem('L1_EM13I_MU10I', ctpid=0xae)\
                        .setLogic(thr_em13i.condition(1)&thr_mu10.condition(1)&physcond)         
        item_2em10_mu6 = LVL1MenuItem('L1_2EM10_MU6', ctpid=0xa3)\
                         .setLogic(thr_em10.condition(2)&thr_mu6.condition(1)&physcond)        
        item_2em13_mu6 = LVL1MenuItem('L1_2EM13_MU6', ctpid=0xa3)\
                         .setLogic(thr_em13.condition(2)&thr_mu6.condition(1)&physcond)
        item_2em14_mu6 = LVL1MenuItem('L1_2EM14_MU6', ctpid=0xa4)\
                         .setLogic(thr_em14.condition(2)&thr_mu6.condition(1)&physcond)        
        item_2em18_mu6 = LVL1MenuItem('L1_2EM18_MU6', ctpid=0xa4)\
                         .setLogic(thr_em18.condition(2)&thr_mu6.condition(1)&physcond)
        item_em5_mu6   = LVL1MenuItem('L1_EM5_MU6', ctpid=0xa5)\
                         .setLogic(thr_em5.condition(1)&thr_mu6.condition(1)&physcond)
        item_em6_mu6   = LVL1MenuItem('L1_EM6_MU6', ctpid=-1)\
                         .setLogic(thr_em6.condition(1)&thr_mu6.condition(1)&physcond)
        item_em5_mu10  = LVL1MenuItem('L1_EM5_MU10', ctpid=-1)\
                         .setLogic(thr_em5.condition(1)&thr_mu10.condition(1)&physcond)          
        item_em6_mu10  = LVL1MenuItem('L1_EM6_MU10', ctpid=-1)\
                         .setLogic(thr_em6.condition(1)&thr_mu10.condition(1)&physcond)
        item_em7_mu6   = LVL1MenuItem('L1_EM7_MU6', ctpid=0xa5)\
                         .setLogic(thr_em7.condition(1)&thr_mu6.condition(1)&physcond)
        item_em10_xe15  = LVL1MenuItem('L1_EM10_XE15', ctpid=0xa6)\
                          .setLogic(thr_em10.condition(1)&thr_xe15.condition(1)&physcond)        
        item_em5_xe20  = LVL1MenuItem('L1_EM5_XE20', ctpid=0xa6)\
                         .setLogic(thr_em5.condition(1)&thr_xe20.condition(1)&physcond)        
        item_em7_xe20  = LVL1MenuItem('L1_EM7_XE20', ctpid=0xa6)\
                         .setLogic(thr_em7.condition(1)&thr_xe20.condition(1)&physcond)
        item_em5_xe15  = LVL1MenuItem('L1_EM5_XE15', ctpid=0xa6)\
                         .setLogic(thr_em5.condition(1)&thr_xe15.condition(1)&physcond)
        item_2em3_tau6  = LVL1MenuItem('L1_2EM3_TAU6', ctpid=0xa7)\
                         .setLogic(thr_em3.condition(2)&thr_tau6.condition(1)&physcond)
        item_em14_xe15 = LVL1MenuItem('L1_EM14_XE15', ctpid=-1)\
                         .setLogic(thr_em14.condition(1)&thr_xe15.condition(1)&physcond)        
        item_em14_xe20 = LVL1MenuItem('L1_EM14_XE20', ctpid=-1)\
                         .setLogic(thr_em14.condition(1)&thr_xe20.condition(1)&physcond)
        item_em12_xe20 = LVL1MenuItem('L1_EM12_XE20', ctpid=-1)\
                         .setLogic(thr_em12.condition(1)&thr_xe20.condition(1)&physcond)
        item_em12_xe30 = LVL1MenuItem('L1_EM12_XE30', ctpid=-1)\
                         .setLogic(thr_em12.condition(1)&thr_xe30.condition(1)&physcond)        
        item_em13_xe20 = LVL1MenuItem('L1_EM13_XE20', ctpid=-1)\
                         .setLogic(thr_em13.condition(1)&thr_xe20.condition(1)&physcond)
        item_em16v_xe20 = LVL1MenuItem('L1_EM16V_XE20', ctpid=-1)\
                         .setLogic(thr_em16v.condition(1)&thr_xe20.condition(1)&physcond)
        item_em13_xe30 = LVL1MenuItem('L1_EM13_XE30', ctpid=-1)\
                         .setLogic(thr_em13.condition(1)&thr_xe30.condition(1)&physcond)
        item_em18_xe20 = LVL1MenuItem('L1_EM18_XE20', ctpid=0xa8)\
                         .setLogic(thr_em18.condition(1)&thr_xe20.condition(1)&physcond)
        item_em10vh_xe20_2j10_j30 = LVL1MenuItem('L1_EM10VH_XE20_2J10_J30', ctpid=0xa8)\
                                    .setLogic(thr_em10vh.condition(1)&thr_xe20.condition(1)&thr_j10.condition(2)&thr_j30.condition(1)&physcond)
        item_em10vh_xe25_2j10_j30 = LVL1MenuItem('L1_EM10VH_XE25_2J10_J30', ctpid=0xa8)\
                                    .setLogic(thr_em10vh.condition(1)&thr_xe25.condition(1)&thr_j10.condition(2)&thr_j30.condition(1)&physcond)
        item_em10_mu15  = LVL1MenuItem('L1_EM10_MU15', ctpid=0xa9)\
                         .setLogic(thr_em10.condition(1)&thr_mu15.condition(1)&physcond)
        item_em10i_mu15 = LVL1MenuItem('L1_EM10I_MU15', ctpid=0xaa)\
                         .setLogic(thr_em10i.condition(1)&thr_mu15.condition(1)&physcond)        
        item_em13_mu15  = LVL1MenuItem('L1_EM13_MU15', ctpid=0xa9)\
                         .setLogic(thr_em13.condition(1)&thr_mu15.condition(1)&physcond)
        item_em13i_mu15 = LVL1MenuItem('L1_EM13I_MU15', ctpid=0xaa)\
                         .setLogic(thr_em13i.condition(1)&thr_mu15.condition(1)&physcond)
        item_em14i_xe15 = LVL1MenuItem('L1_EM14I_XE15', ctpid=0xab)\
                         .setLogic(thr_em14i.condition(1)&thr_xe15.condition(1)&physcond)        
        item_em18i_xe15 = LVL1MenuItem('L1_EM18I_XE15', ctpid=0xab)\
                         .setLogic(thr_em18i.condition(1)&thr_xe15.condition(1)&physcond)
        item_em5i_mu6   = LVL1MenuItem('L1_EM5I_MU6', ctpid=0xac)\
                         .setLogic(thr_em5i.condition(1)&thr_mu6.condition(1)&physcond)        
        item_em7i_mu6   = LVL1MenuItem('L1_EM7I_MU6', ctpid=0xac)\
                         .setLogic(thr_em7i.condition(1)&thr_mu6.condition(1)&physcond)
        item_em14i_xe20 = LVL1MenuItem('L1_EM14I_XE20', ctpid=0xad)\
                         .setLogic(thr_em14i.condition(1)&thr_xe20.condition(1)&physcond)        
        item_em18i_xe20 = LVL1MenuItem('L1_EM18I_XE20', ctpid=0xad)\
                         .setLogic(thr_em18i.condition(1)&thr_xe20.condition(1)&physcond)
        item_em10i_mu10 = LVL1MenuItem('L1_EM10I_MU10', ctpid=0xae)\
                          .setLogic(thr_em10i.condition(1)&thr_mu10.condition(1)&physcond)        
        item_em13i_mu10 = LVL1MenuItem('L1_EM13I_MU10', ctpid=0xae)\
                          .setLogic(thr_em13i.condition(1)&thr_mu10.condition(1)&physcond)
        item_em7_xs30 = LVL1MenuItem('L1_EM7_XS30', ctpid=-1)\
                         .setLogic(thr_em7.condition(1)&thr_xs30.condition(1)&physcond)
        item_em8_xs30 = LVL1MenuItem('L1_EM8_XS30', ctpid=-1)\
                       .setLogic(thr_em8.condition(1)&thr_xs30.condition(1)&physcond)
        item_em7_xs45 = LVL1MenuItem('L1_EM7_XS45', ctpid=-1)\
                        .setLogic(thr_em7.condition(1)&thr_xs45.condition(1)&physcond)
        item_em8_xs45 = LVL1MenuItem('L1_EM8_XS45', ctpid=-1)\
                        .setLogic(thr_em8.condition(1)&thr_xs45.condition(1)&physcond)
        item_em6_xs30 = LVL1MenuItem('L1_EM6_XS30', ctpid=-1)\
                        .setLogic(thr_em6.condition(1)&thr_xs30.condition(1)&physcond)
        item_em6_xs45 = LVL1MenuItem('L1_EM6_XS45', ctpid=-1)\
                        .setLogic(thr_em6.condition(1)&thr_xs45.condition(1)&physcond)
        item_em6_xs50 = LVL1MenuItem('L1_EM6_XS50', ctpid=-1)\
                        .setLogic(thr_em6.condition(1)&thr_xs50.condition(1)&physcond)
        item_em6_xs55 = LVL1MenuItem('L1_EM6_XS55', ctpid=-1)\
                        .setLogic(thr_em6.condition(1)&thr_xs55.condition(1)&physcond)
        item_em6_xs60 = LVL1MenuItem('L1_EM6_XS60', ctpid=-1)\
                        .setLogic(thr_em6.condition(1)&thr_xs60.condition(1)&physcond)
        item_em16v_xs45 = LVL1MenuItem('L1_EM16V_XS45', ctpid=-1)\
                          .setLogic(thr_em16v.condition(1)&thr_xs45.condition(1)&physcond)
        item_em10_xs45 = LVL1MenuItem('L1_EM10_XS45', ctpid=0xa0)\
                         .setLogic(thr_em10.condition(1)&thr_xs45.condition(1)&physcond)
        item_em12_xs45 = LVL1MenuItem('L1_EM12_XS45', ctpid=-1)\
                        .setLogic(thr_em12.condition(1)&thr_xs45.condition(1)&physcond)
        item_em12_xs30 = LVL1MenuItem('L1_EM12_XS30', ctpid=-1)\
                         .setLogic(thr_em12.condition(1)&thr_xs30.condition(1)&physcond)
        item_em13_xs45 = LVL1MenuItem('L1_EM13_XS45', ctpid=-1)\
                         .setLogic(thr_em13.condition(1)&thr_xs45.condition(1)&physcond)
        item_em13_xs30 = LVL1MenuItem('L1_EM13_XS30', ctpid=-1)\
                         .setLogic(thr_em13.condition(1)&thr_xs30.condition(1)&physcond)
        item_em10_xs50 = LVL1MenuItem('L1_EM10_XS50', ctpid=0xa0)\
                         .setLogic(thr_em10.condition(1)&thr_xs50.condition(1)&physcond)
        item_em12_xs50 = LVL1MenuItem('L1_EM12_XS50', ctpid=-1)\
                         .setLogic(thr_em12.condition(1)&thr_xs50.condition(1)&physcond)
        item_em12_3j10 = LVL1MenuItem('L1_EM12_3J10', ctpid=-1)\
                         .setLogic(thr_em12.condition(1)&thr_j10.condition(3)&physcond)
        item_em12_4j10 = LVL1MenuItem('L1_EM12_4J10', ctpid=-1)\
                         .setLogic(thr_em12.condition(1)&thr_j10.condition(4)&physcond)
        item_em13_3j10 = LVL1MenuItem('L1_EM13_3J10', ctpid=-1)\
                         .setLogic(thr_em13.condition(1)&thr_j10.condition(3)&physcond)
        item_em13_4j10 = LVL1MenuItem('L1_EM13_4J10', ctpid=-1)\
                         .setLogic(thr_em13.condition(1)&thr_j10.condition(4)&physcond)
                
        # TAU + EM
        item_2tau5_em5        = LVL1MenuItem('L1_2TAU5_EM5', ctpid=0xaf)\
                                .setLogic(thr_tau5.condition(2)&thr_em5.condition(1)&physcond)\
                                .setTriggerType(TT_calo)
        item_2tau5_em10        = LVL1MenuItem('L1_2TAU5_EM10', ctpid=0xaf)\
                                .setLogic(thr_tau5.condition(2)&thr_em10.condition(1)&physcond)\
                                .setTriggerType(TT_calo)
        item_2tau5_em7        = LVL1MenuItem('L1_2TAU5_EM7', ctpid=0xaf)\
                                .setLogic(thr_tau5.condition(2)&thr_em7.condition(1)&physcond)\
                                .setTriggerType(TT_calo)
        item_2tau6_em5        = LVL1MenuItem('L1_2TAU6_EM5', ctpid=-1)\
                                .setLogic(thr_tau6.condition(2)&thr_em5.condition(1)&physcond)\
                                .setTriggerType(TT_calo)          
        item_2tau6_em7        = LVL1MenuItem('L1_2TAU6_EM7', ctpid=0xaf)\
                                .setLogic(thr_tau6.condition(2)&thr_em7.condition(1)&physcond)\
                                .setTriggerType(TT_calo)
        item_2tau6_em10       = LVL1MenuItem('L1_2TAU6_EM10', ctpid=0x0)\
                                .setLogic(thr_tau6.condition(2)&thr_em10.condition(1)&physcond)\
                                .setTriggerType(TT_calo)
        item_2tau8_em10       = LVL1MenuItem('L1_2TAU8_EM10', ctpid=-1)\
                                .setLogic(thr_tau8.condition(2)&thr_em10.condition(1)&physcond)\
                                .setTriggerType(TT_calo)
        item_2tau8_em10vh = LVL1MenuItem('L1_2TAU8_EM10VH', ctpid=-1, group='1')\
                            .setLogic(thr_tau8.condition(2)&thr_em10vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2tau8_em10h = LVL1MenuItem('L1_2TAU8_EM10H', ctpid=-1, group='1')\
                            .setLogic(thr_tau8.condition(2)&thr_em10h.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2tau8_em11h = LVL1MenuItem('L1_2TAU8_EM11H', ctpid=-1, group='1')\
                           .setLogic(thr_tau8.condition(2)&thr_em11h.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2tau8i_em10vh = LVL1MenuItem('L1_2TAU8I_EM10VH', ctpid=-1, group='1')\
                             .setLogic(thr_tau8i.condition(2)&thr_em10vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2tau8v_em10vh = LVL1MenuItem('L1_2TAU8V_EM10VH', ctpid=-1, group='1')\
                             .setLogic(thr_tau8v.condition(2)&thr_em10vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)

        item_2tau11i_em14vh = LVL1MenuItem('L1_2TAU11I_EM14VH', ctpid=-1, group='1')\
                              .setLogic(thr_tau11i.condition(2)&thr_em14vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2tau11_tau20_em14vh = LVL1MenuItem('L1_2TAU11_TAU20_EM14VH', ctpid=-1, group='1')\
                                   .setLogic(thr_tau11.condition(2)&thr_tau20.condition(1)&thr_em14vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2tau11_tau20_em10vh = LVL1MenuItem('L1_2TAU11_TAU20_EM10VH', ctpid=-1, group='1')\
                                   .setLogic(thr_tau11.condition(2)&thr_tau20.condition(1)&thr_em10vh.condition(1)&physcond).setTriggerType(TT_calo|TT_caloem)
        item_2tau6_em10_j5       = LVL1MenuItem('L1_2TAU6_EM10_J5', ctpid=0x0)\
                                   .setLogic(thr_tau6.condition(2)&thr_em10.condition(1)&thr_j5.condition(1)&physcond)\
                                   .setTriggerType(TT_calo)          
        item_2tau6_em10_j10      = LVL1MenuItem('L1_2TAU6_EM10_J10', ctpid=0x0)\
                                   .setLogic(thr_tau6.condition(2)&thr_em10.condition(1)&thr_j10.condition(1)&physcond)\
                                   .setTriggerType(TT_calo)          
        item_2tau6_mu10_j5       = LVL1MenuItem('L1_2TAU6_MU10_J5', ctpid=0x0)\
                                   .setLogic(thr_tau6.condition(2)&thr_mu10.condition(1)&thr_j5.condition(1)&physcond)\
                                   .setTriggerType(TT_calo)          
        item_2tau6_mu10_j10      = LVL1MenuItem('L1_2TAU6_MU10_J10', ctpid=0x0)\
                                   .setLogic(thr_tau6.condition(2)&thr_mu10.condition(1)&thr_j10.condition(1)&physcond)\
                                   .setTriggerType(TT_calo)          
        item_2tau11_tau15       = LVL1MenuItem('L1_2TAU11_TAU15', ctpid=0x0)\
                                .setLogic(thr_tau11.condition(2)&thr_tau15.condition(1)&physcond)\
                                .setTriggerType(TT_calo)          
        item_2tau11_tau20       = LVL1MenuItem('L1_2TAU11_TAU20', ctpid=-1)\
                                .setLogic(thr_tau11.condition(2)&thr_tau20.condition(1)&physcond)\
                                .setTriggerType(TT_calo)          
        item_2tau11i_tau15       = LVL1MenuItem('L1_2TAU11I_TAU15', ctpid=0x0)\
                                   .setLogic(thr_tau11i.condition(2)&thr_tau15.condition(1)&physcond)\
                                   .setTriggerType(TT_calo)
        item_2tau15_tau20       = LVL1MenuItem('L1_2TAU15_TAU20', ctpid=0x0)\
                                .setLogic(thr_tau15.condition(2)&thr_tau20.condition(1)&physcond)\
                                .setTriggerType(TT_calo)          
        item_2tau6_tau9i_em5  = LVL1MenuItem('L1_2TAU6_TAU9I_EM5', ctpid=0xb0)\
                                .setLogic(thr_tau6.condition(2)&thr_tau9i.condition(1)&thr_em5.condition(1)&physcond)
        item_2tau6_tau9i_em7  = LVL1MenuItem('L1_2TAU6_TAU9I_EM7', ctpid=0xb0)\
                                .setLogic(thr_tau6.condition(2)&thr_tau9i.condition(1)&thr_em7.condition(1)&physcond)
        item_2tau5_tau6i_em5  = LVL1MenuItem('L1_2TAU5_TAU6I_EM5', ctpid=-1)\
                                .setLogic(thr_tau5.condition(2)&thr_tau6i.condition(1)&thr_em5.condition(1)&physcond)
        item_2tau5_tau6_em5  = LVL1MenuItem('L1_2TAU5_TAU6_EM5', ctpid=0xb0)\
                                .setLogic(thr_tau5.condition(2)&thr_tau6.condition(1)&thr_em5.condition(1)&physcond)        
        item_tau16_em5        = LVL1MenuItem('L1_TAU16_EM5', ctpid=0xb1)\
                                .setLogic(thr_tau16.condition(1)&thr_em5.condition(1)&physcond)        
        item_tau16_em7        = LVL1MenuItem('L1_TAU16_EM7', ctpid=0xb1)\
                                .setLogic(thr_tau16.condition(1)&thr_em7.condition(1)&physcond)
        item_tau11_em5        = LVL1MenuItem('L1_TAU11_EM5', ctpid=0xb1)\
                                .setLogic(thr_tau11.condition(1)&thr_em5.condition(1)&physcond)
        item_tau30_em10vh     = LVL1MenuItem('L1_TAU30_EM10VH', ctpid=-1)\
                                .setLogic(thr_tau30.condition(1)&thr_em10vh.condition(1)&physcond)\
                                .setTriggerType(TT_calo|TT_caloem)
        item_tau20_em10vh     = LVL1MenuItem('L1_TAU20_EM10VH', ctpid=-1)\
                                .setLogic(thr_tau20.condition(1)&thr_em10vh.condition(1)&physcond)\
                                .setTriggerType(TT_calo|TT_caloem)
        item_tau20_em10h     = LVL1MenuItem('L1_TAU20_EM10H', ctpid=-1)\
                                .setLogic(thr_tau20.condition(1)&thr_em10h.condition(1)&physcond)\
                                .setTriggerType(TT_calo|TT_caloem)
        item_tau20_em11h     = LVL1MenuItem('L1_TAU20_EM11H', ctpid=-1)\
                               .setLogic(thr_tau20.condition(1)&thr_em11h.condition(1)&physcond)\
                               .setTriggerType(TT_calo|TT_caloem)
        item_2tau11_em10        = LVL1MenuItem('L1_2TAU11_EM10', ctpid=-1)\
                                .setLogic(thr_tau11.condition(2)&thr_em10.condition(1)&physcond)
        item_2tau11_em10vh      = LVL1MenuItem('L1_2TAU11_EM10VH', ctpid=-1)\
                                  .setLogic(thr_tau11.condition(2)&thr_em10vh.condition(1)&physcond)\
                                  .setTriggerType(TT_calo|TT_caloem)
        item_2tau11_em10h      = LVL1MenuItem('L1_2TAU11_EM10H', ctpid=-1)\
                                  .setLogic(thr_tau11.condition(2)&thr_em10h.condition(1)&physcond)\
                                  .setTriggerType(TT_calo|TT_caloem)
        item_2tau11_em11h      = LVL1MenuItem('L1_2TAU11_EM11H', ctpid=-1)\
                                 .setLogic(thr_tau11.condition(2)&thr_em11h.condition(1)&physcond)\
                                 .setTriggerType(TT_calo|TT_caloem)
        item_2tau9i_em10i     = LVL1MenuItem('L1_2TAU9I_EM10I', ctpid=0xb2)\
                                .setLogic(thr_tau9i.condition(2)&thr_em10i.condition(1)&physcond)        
        item_2tau9i_em13i     = LVL1MenuItem('L1_2TAU9I_EM13I', ctpid=0xb2)\
                                .setLogic(thr_tau9i.condition(2)&thr_em13i.condition(1)&physcond)
        item_2tau16i_em13i    = LVL1MenuItem('L1_2TAU16I_EM13I', ctpid=0xb2)\
                                .setLogic(thr_tau16i.condition(2)&thr_em13i.condition(1)&physcond)         
        item_2tau6i_em10i     = LVL1MenuItem('L1_2TAU6I_EM10I', ctpid=0xb2)\
                                .setLogic(thr_tau6i.condition(2)&thr_em10i.condition(1)&physcond)        
        item_2tau6i_em13i     = LVL1MenuItem('L1_2TAU6I_EM13I', ctpid=0xb2)\
                                .setLogic(thr_tau6i.condition(2)&thr_em13i.condition(1)&physcond)
        item_tau9i_em10i      = LVL1MenuItem('L1_TAU9I_EM10I', ctpid=0xb3)\
                                .setLogic(thr_tau9i.condition(1)&thr_em10i.condition(1)&physcond)        
        item_tau9i_em13i      = LVL1MenuItem('L1_TAU9I_EM13I', ctpid=0xb3)\
                                .setLogic(thr_tau9i.condition(1)&thr_em13i.condition(1)&physcond)
        item_tau11_2j10_j20_xe20  = LVL1MenuItem('L1_TAU11_2J10_J20_XE20', ctpid=-1)\
                                    .setLogic(thr_tau11.condition(1)&thr_j10.condition(2)&thr_j20.condition(1)&thr_xe20.condition(1)&physcond)\
                                    .setTriggerType(TT_calo)          
        
        item_mu15_xe15 = LVL1MenuItem('L1_MU15_XE15', ctpid=0xb4)\
                         .setLogic(thr_mu15.condition(1)&thr_xe15.condition(1)&physcond)
        item_mu15_xe10 = LVL1MenuItem('L1_MU15_XE10', ctpid=0xb4)\
                         .setLogic(thr_mu15.condition(1)&thr_xe10.condition(1)&physcond)
        item_2mu0_xe20 = LVL1MenuItem('L1_2MU0_XE20',  ctpid=-1)\
                         .setLogic(thr_mu0.condition(2)&thr_xe20.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo)
        item_2mu0_xe25 = LVL1MenuItem('L1_2MU0_XE25',  ctpid=-1)\
                         .setLogic(thr_mu0.condition(2)&thr_xe25.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo)
        item_mu0_j5    = LVL1MenuItem('L1_MU0_J5', ctpid=0xb5)\
                         .setLogic(thr_mu0.condition(1)&thr_j5.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo)
        item_mu0_j50    = LVL1MenuItem('L1_MU0_J50', ctpid=0xb5)\
                          .setLogic(thr_mu0.condition(1)&thr_j50.condition(1)&physcond)\
                          .setTriggerType(TT_rpcin|TT_calo)
        item_mu4_j15    = LVL1MenuItem('L1_MU4_J15', ctpid=-1)\
                          .setLogic(thr_mu4.condition(1)&thr_j15.condition(1)&physcond)\
                          .setTriggerType(TT_rpcin|TT_calo)
        item_mu4_j50    = LVL1MenuItem('L1_MU4_J50', ctpid=-1)\
                          .setLogic(thr_mu4.condition(1)&thr_j50.condition(1)&physcond)\
                          .setTriggerType(TT_rpcin|TT_calo)
        item_mu0_j10   = LVL1MenuItem('L1_MU0_J10', ctpid=0xb6)\
                         .setLogic(thr_mu0.condition(1)&thr_j10.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo)
        item_mu4_j10   = LVL1MenuItem('L1_MU4_J10', ctpid=-1)\
                         .setLogic(thr_mu4.condition(1)&thr_j10.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo)
        item_mu0_j20   = LVL1MenuItem('L1_MU0_J20', ctpid=0xb7)\
                         .setLogic(thr_mu0.condition(1)&thr_j20.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo)
        item_mu0_j40   = LVL1MenuItem('L1_MU0_J40', ctpid=0xb8)\
                         .setLogic(thr_mu0.condition(1)&thr_j40.condition(1)&physcond) # new jet threshold
        item_mu0_j70   = LVL1MenuItem('L1_MU0_J70', ctpid=0xb9)\
                         .setLogic(thr_mu0.condition(1)&thr_j70.condition(1)&physcond) # new jet threshold
        item_mu0_j100   = LVL1MenuItem('L1_MU0_J100', ctpid=0xba)\
                         .setLogic(thr_mu0.condition(1)&thr_j100.condition(1)&physcond) # new jet threshold
 
        # - Add/remap new L1 thresholds:
        #   J20->J15, J40->J30, J70->J55, J100->J75, J130->J95, J150->J115
        item_mu0_j15   = LVL1MenuItem('L1_MU0_J15', ctpid=-1)\
                         .setLogic(thr_mu0.condition(1)&thr_j15.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo)
        item_mu4_j15   = LVL1MenuItem('L1_MU4_J15', ctpid=-1)\
                         .setLogic(thr_mu4.condition(1)&thr_j15.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo)
        item_mu0_j30   = LVL1MenuItem('L1_MU0_J30', ctpid=-1)\
                         .setLogic(thr_mu0.condition(1)&thr_j30.condition(1)&physcond)
        item_mu4_j30   = LVL1MenuItem('L1_MU4_J30', ctpid=-1)\
                         .setLogic(thr_mu4.condition(1)&thr_j30.condition(1)&physcond)
        item_mu0_j55   = LVL1MenuItem('L1_MU0_J55', ctpid=-1)\
                         .setLogic(thr_mu0.condition(1)&thr_j55.condition(1)&physcond)
        item_mu0_j75   = LVL1MenuItem('L1_MU0_J75', ctpid=0xba)\
                         .setLogic(thr_mu0.condition(1)&thr_j75.condition(1)&physcond) # new jet threshold
        item_mu4_j75   = LVL1MenuItem('L1_MU4_J75', ctpid=-1)\
                         .setLogic(thr_mu4.condition(1)&thr_j75.condition(1)&physcond) 

        item_mu0_j20_xe20  = LVL1MenuItem('L1_MU0_J20_XE20', ctpid=-1)\
                             .setLogic(thr_mu0.condition(1)&thr_j20.condition(1)&thr_xe20.condition(1)&physcond)
        item_mu4_j20_xe20  = LVL1MenuItem('L1_MU4_J20_XE20', ctpid=-1)\
                             .setLogic(thr_mu4.condition(1)&thr_j20.condition(1)&thr_xe20.condition(1)&physcond)
        item_mu4_j20_xe35  = LVL1MenuItem('L1_MU4_J20_XE35', ctpid=-1)\
                             .setLogic(thr_mu4.condition(1)&thr_j20.condition(1)&thr_xe35.condition(1)&physcond)

        #item_mu0_j18   = LVL1MenuItem('L1_MU0_J18', ctpid=0xb7)\
        #                 .setLogic(thr_mu0.condition(1)&thr_j18.condition(1)&physcond)
        #item_mu0_j23   = LVL1MenuItem('L1_MU0_J23', ctpid=0xb8)\
        #                 .setLogic(thr_mu0.condition(1)&thr_j23.condition(1)&physcond)
        #item_mu0_j35   = LVL1MenuItem('L1_MU0_J35', ctpid=0xb9)\
        #                 .setLogic(thr_mu0.condition(1)&thr_j35.condition(1)&physcond)
        #item_mu0_j42   = LVL1MenuItem('L1_MU0_J42', ctpid=0xba)\
        #                 .setLogic(thr_mu0.condition(1)&thr_j42.condition(1)&physcond)
        item_mu0_em3   = LVL1MenuItem('L1_MU0_EM3', ctpid=0xbb)\
                         .setLogic(thr_mu0.condition(1)&thr_em3.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo|TT_caloem)
        item_em3_mu0   = LVL1MenuItem('L1_EM3_MU0', ctpid=0xbb)\
                         .setLogic(thr_mu0.condition(1)&thr_em3.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo|TT_caloem)
        item_em3_mu4   = LVL1MenuItem('L1_EM3_MU4', ctpid=-1)\
                         .setLogic(thr_mu4.condition(1)&thr_em3.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo|TT_caloem)
        item_mu6_j5    = LVL1MenuItem('L1_MU6_J5', ctpid=0xbc)\
                         .setLogic(thr_mu6.condition(1)&thr_j5.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo)
        item_mu6_j15    = LVL1MenuItem('L1_MU6_J15', ctpid=-1)\
                         .setLogic(thr_mu6.condition(1)&thr_j15.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo)
        item_mu6_2j20    = LVL1MenuItem('L1_MU6_2J20', ctpid=-1)\
                          .setLogic(thr_mu6.condition(1)&thr_j20.condition(2)&physcond)\
                          .setTriggerType(TT_rpcin|TT_calo)
        item_mu10_j5    = LVL1MenuItem('L1_MU10_J5', ctpid=0xfc, prescale=1)\
                         .setLogic(thr_mu10.condition(1)&thr_j5.condition(1)&physcond)
        item_mu6_em3   = LVL1MenuItem('L1_MU6_EM3', ctpid=0xbd)\
                         .setLogic(thr_mu6.condition(1)&thr_em3.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo|TT_caloem)
        item_2mu4_em3   = LVL1MenuItem('L1_2MU4_EM3', ctpid=0xbd)\
                         .setLogic(thr_mu4.condition(2)&thr_em3.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo|TT_caloem)
        item_2mu4_2em3   = LVL1MenuItem('L1_2MU4_2EM3', ctpid=0xbd)\
                          .setLogic(thr_mu4.condition(2)&thr_em3.condition(2)&physcond)\
                          .setTriggerType(TT_rpcin|TT_calo|TT_caloem)

        item_em3_mu6   = LVL1MenuItem('L1_EM3_MU6', ctpid=0xbd)\
                         .setLogic(thr_mu6.condition(1)&thr_em3.condition(1)&physcond)\
                         .setTriggerType(TT_rpcin|TT_calo|TT_caloem)
        item_2mu0_em5  = LVL1MenuItem('L1_2MU0_EM5', ctpid=0xbe)\
                         .setLogic(thr_mu0.condition(2)&thr_em5.condition(1)&physcond)        
        item_2mu6_em5  = LVL1MenuItem('L1_2MU6_EM5', ctpid=-1)\
                         .setLogic(thr_mu6.condition(2)&thr_em5.condition(1)&physcond)        
        item_em5_2mu6  = LVL1MenuItem('L1_EM5_2MU6', ctpid=-1)\
                         .setLogic(thr_mu6.condition(2)&thr_em5.condition(1)&physcond)        
        item_em6_2mu6  = LVL1MenuItem('L1_EM6_2MU6', ctpid=-1)\
                         .setLogic(thr_mu6.condition(2)&thr_em6.condition(1)&physcond)
        item_2mu6_em7  = LVL1MenuItem('L1_2MU6_EM7', ctpid=0xbe)\
                         .setLogic(thr_mu6.condition(2)&thr_em7.condition(1)&physcond)
        item_mu20_xe15 = LVL1MenuItem('L1_MU20_XE15', ctpid=0xbf)\
                         .setLogic(thr_mu20.condition(1)&thr_xe15.condition(1)&physcond)
        item_mu10_xe20 = LVL1MenuItem('L1_MU10_XE20', ctpid=-1)\
                         .setLogic(thr_mu10.condition(1)&thr_xe20.condition(1)&physcond)

        item_mu10_xe25 = LVL1MenuItem('L1_MU10_XE25', ctpid=-1)\
                         .setLogic(thr_mu10.condition(1)&thr_xe25.condition(1)&physcond)
        item_mu20_xe20 = LVL1MenuItem('L1_MU20_XE20', ctpid=0xc0)\
                         .setLogic(thr_mu20.condition(1)&thr_xe20.condition(1)&physcond)
        item_mu40_xe15 = LVL1MenuItem('L1_MU40_XE15', ctpid=0xc1)\
                         .setLogic(thr_mu40.condition(1)&thr_xe15.condition(1)&physcond)
        item_mu10_j10  = LVL1MenuItem('L1_MU10_J10', ctpid=0xc2)\
                         .setLogic(thr_mu10.condition(1)&thr_j10.condition(1)&physcond)
        item_mu10_j20  = LVL1MenuItem('L1_MU10_J20', ctpid=-1)\
                         .setLogic(thr_mu10.condition(1)&thr_j20.condition(1)&physcond) # new jet threshold

        item_mu0_em3_empty   = LVL1MenuItem('L1_MU0_EM3_EMPTY', ctpid=-1)\
                               .setLogic(thr_mu0.condition(1)&thr_em3.condition(1)&cosmiccond)
        item_mu6_j5_empty    = LVL1MenuItem('L1_MU6_J5_EMPTY', ctpid=-1)\
                               .setLogic(thr_mu6.condition(1)&thr_j5.condition(1)&cosmiccond)        
        item_mu6_em3_empty   = LVL1MenuItem('L1_MU6_EM3_EMPTY', ctpid=-1)\
                               .setLogic(thr_mu6.condition(1)&thr_em3.condition(1)&cosmiccond)
        item_mu0_j5_empty    = LVL1MenuItem('L1_MU0_J5_EMPTY', ctpid=-1)\
                               .setLogic(thr_mu0.condition(1)&thr_j5.condition(1)&cosmiccond)

#        # TAU + MU
        #item_tau9i_mu6 = LVL1MenuItem('L1_TAU9I_MU6', ctpid=0xc3)\
        #                 .setLogic(thr_tau9i.condition(1)&thr_mu6.condition(1)&Logic(bgrp0)&Logic(bgrp1))
        #item_tau9i_mu10 = LVL1MenuItem('L1_TAU9I_MU10', ctpid=0xc4)\
        #                  .setLogic(thr_tau9i.condition(1)&thr_mu10.condition(1)&Logic(bgrp0)&Logic(bgrp1))
        item_tau9i_mu15 = LVL1MenuItem('L1_TAU9I_MU15', ctpid=-1)\
                          .setLogic(thr_tau9i.condition(1)&thr_mu15.condition(1)&physcond)
        item_tau6i_mu15 = LVL1MenuItem('L1_TAU6I_MU15', ctpid=-1)\
                          .setLogic(thr_tau6i.condition(1)&thr_mu15.condition(1)&physcond)
        item_tau9i_mu6 = LVL1MenuItem('L1_TAU9I_MU6', ctpid=0xc5)\
                          .setLogic(thr_tau9i.condition(1)&thr_mu6.condition(1)&physcond)
        item_tau6_mu6 = LVL1MenuItem('L1_TAU6_MU6', ctpid=0xc5)\
                        .setLogic(thr_tau6.condition(1)&thr_mu6.condition(1)&physcond)
        item_tau5_mu6 = LVL1MenuItem('L1_TAU5_MU6', ctpid=0xc5)\
                        .setLogic(thr_tau5.condition(1)&thr_mu6.condition(1)&physcond)\
                        .setTriggerType(TT_calo)
        item_tau6i_mu6 = LVL1MenuItem('L1_TAU6I_MU6', ctpid=0xc5)\
                         .setLogic(thr_tau6i.condition(1)&thr_mu6.condition(1)&physcond)
        item_tau9i_mu10 = LVL1MenuItem('L1_TAU9I_MU10', ctpid=0xc6)\
                          .setLogic(thr_tau9i.condition(1)&thr_mu10.condition(1)&physcond)
        item_tau6_mu10 = LVL1MenuItem('L1_TAU6_MU10', ctpid=-1)\
                         .setLogic(thr_tau6.condition(1)&thr_mu10.condition(1)&physcond)
        item_tau8_mu10 = LVL1MenuItem('L1_TAU8_MU10', ctpid=-1)\
                         .setLogic(thr_tau8.condition(1)&thr_mu10.condition(1)&physcond)
        item_tau8i_mu10 = LVL1MenuItem('L1_TAU8I_MU10', ctpid=-1)\
                         .setLogic(thr_tau8i.condition(1)&thr_mu10.condition(1)&physcond)
        item_tau6_mu15 = LVL1MenuItem('L1_TAU6_MU15', ctpid=-1)\
                          .setLogic(thr_tau6.condition(1)&thr_mu15.condition(1)&physcond)        
        item_tau6i_mu10 = LVL1MenuItem('L1_TAU6I_MU10', ctpid=0xc6)\
                          .setLogic(thr_tau6i.condition(1)&thr_mu10.condition(1)&physcond)  
        item_tau16_mu10 = LVL1MenuItem('L1_TAU16_MU10', ctpid=0xc7)\
                         .setLogic(thr_tau16.condition(1)&thr_mu10.condition(1)&physcond)
        item_tau11_mu10 = LVL1MenuItem('L1_TAU11_MU10', ctpid=0xc7)\
                         .setLogic(thr_tau11.condition(1)&thr_mu10.condition(1)&physcond)
        item_j70_xe20  = LVL1MenuItem('L1_J70_XE20', ctpid=0xc8)\
                         .setLogic(thr_j70.condition(1)&thr_xe20.condition(1)&physcond)
        item_j55_xe15  = LVL1MenuItem('L1_J55_XE15', ctpid=0xc8)\
                         .setLogic(thr_j55.condition(1)&thr_xe15.condition(1)&physcond)
        item_j70_xe80  = LVL1MenuItem('L1_J70_XE80', ctpid=0xc8)\
                         .setLogic(thr_j70.condition(1)&thr_xe80.condition(1)&physcond)         
        item_j40_xe20  = LVL1MenuItem('L1_J40_XE20', ctpid=-1)\
                         .setLogic(thr_j40.condition(1)&thr_xe20.condition(1)&physcond) # new jet threshold
        item_j40_xe15  = LVL1MenuItem('L1_J40_XE15', ctpid=-1)\
                         .setLogic(thr_j40.condition(1)&thr_xe15.condition(1)&physcond) # new jet threshold
        item_2j20_xe20 = LVL1MenuItem('L1_2J20_XE20', ctpid=-1)\
                         .setLogic(thr_j20.condition(2)&thr_xe20.condition(1)&physcond) # new jet threshold
        item_2j20_xe15 = LVL1MenuItem('L1_2J20_XE15', ctpid=-1)\
                         .setLogic(thr_j20.condition(2)&thr_xe15.condition(1)&physcond) # new jet threshold
        item_2j30_xe20 = LVL1MenuItem('L1_2J30_XE20', ctpid=-1)\
                         .setLogic(thr_j30.condition(2)&thr_xe20.condition(1)&physcond) # new jet threshold

        item_j20_xe15_em10i = LVL1MenuItem('L1_J20_XE15_EM10I', ctpid=-1)\
                              .setLogic(thr_j20.condition(1)&thr_xe15.condition(1)&thr_em10i.condition(1)&physcond) # new jet threshold 
        item_j20_xe20_em13i = LVL1MenuItem('L1_J20_XE20_EM13I', ctpid=-1)\
                              .setLogic(thr_j20.condition(1)&thr_xe20.condition(1)&thr_em13i.condition(1)&physcond) # new jet threshold
        item_j20_xe20_mu15  = LVL1MenuItem('L1_J20_XE20_MU15', ctpid=-1)\
                              .setLogic(thr_j20.condition(1)&thr_xe20.condition(1)&thr_mu15.condition(1)&physcond) # new jet threshold
        item_j15_xe15_mu15  = LVL1MenuItem('L1_J15_XE15_MU15', ctpid=-1)\
                              .setLogic(thr_j15.condition(1)&thr_xe15.condition(1)&thr_mu15.condition(1)&physcond) # new jet threshold
        item_j15_xe20_mu15  = LVL1MenuItem('L1_J15_XE15_MU20', ctpid=-1)\
                              .setLogic(thr_j15.condition(1)&thr_xe20.condition(1)&thr_mu15.condition(1)&physcond) # new jet threshold
        item_4j23_em13i     = LVL1MenuItem('L1_4J23_EM13I', ctpid=0xcc)\
                              .setLogic(thr_j23.condition(4)&thr_em13i.condition(1)&physcond)
        item_4j10_em10      = LVL1MenuItem('L1_4J10_EM10', ctpid=-1)\
                              .setLogic(thr_j10.condition(4)&thr_em10.condition(1)&physcond)
        item_4j10_em10i     = LVL1MenuItem('L1_4J10_EM10I', ctpid=-1)\
                              .setLogic(thr_j10.condition(4)&thr_em10i.condition(1)&physcond)        
        item_4j10_em13i     = LVL1MenuItem('L1_4J10_EM13I', ctpid=-1)\
                              .setLogic(thr_j10.condition(4)&thr_em13i.condition(1)&physcond)
        item_4j10_mu15      = LVL1MenuItem('L1_4J10_MU15', ctpid=0xcd)\
                              .setLogic(thr_j10.condition(4)&thr_mu15.condition(1)&physcond) # new jet threshold
        item_2j10_em13i     = LVL1MenuItem('L1_2J10_EM13I', ctpid=0xce)\
                              .setLogic(thr_j10.condition(2)&thr_em13i.condition(1)&physcond)

        # TAU + XE
        item_tau9_xe15   = LVL1MenuItem('L1_TAU9_XE15', ctpid=0xcf)\
                           .setLogic(thr_tau9.condition(1)&thr_xe15.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau5_xe10   = LVL1MenuItem('L1_TAU5_XE10', ctpid=-1)\
                           .setLogic(thr_tau5.condition(1)&thr_xe10.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau15_xe20   = LVL1MenuItem('L1_TAU15_XE20', ctpid=-1)\
                           .setLogic(thr_tau15.condition(1)&thr_xe20.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau15_xe35   = LVL1MenuItem('L1_TAU15_XE35', ctpid=-1)\
                            .setLogic(thr_tau15.condition(1)&thr_xe35.condition(1)&physcond)\
                            .setTriggerType(TT_calo)
        item_tau15_xe40   = LVL1MenuItem('L1_TAU15_XE40', ctpid=-1)\
                            .setLogic(thr_tau15.condition(1)&thr_xe40.condition(1)&physcond)\
                            .setTriggerType(TT_calo)
        item_tau6_xe15   = LVL1MenuItem('L1_TAU6_XE15', ctpid=0xd7)\
                           .setLogic(thr_tau6.condition(1)&thr_xe15.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau6_xe20   = LVL1MenuItem('L1_TAU6_XE20', ctpid=0xd7)\
                           .setLogic(thr_tau6.condition(1)&thr_xe20.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau6_xe10   = LVL1MenuItem('L1_TAU6_XE10', ctpid=0xd8)\
                           .setLogic(thr_tau6.condition(1)&thr_xe10.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau8_xe10   = LVL1MenuItem('L1_TAU8_XE10', ctpid=-1)\
                           .setLogic(thr_tau8.condition(1)&thr_xe10.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau11_xe10_3j10  = LVL1MenuItem('L1_TAU11_XE10_3J10', ctpid=0xd9)\
                           .setLogic(thr_tau11.condition(1)&thr_xe10.condition(1)&thr_j10.condition(3)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau11_xe10  = LVL1MenuItem('L1_TAU11_XE10', ctpid=0xd9)\
                           .setLogic(thr_tau11.condition(1)&thr_xe10.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau11_xe15  = LVL1MenuItem('L1_TAU11_XE15', ctpid=0xd9)\
                           .setLogic(thr_tau11.condition(1)&thr_xe15.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau11_xe20  = LVL1MenuItem('L1_TAU11_XE20', ctpid=0xd9)\
                           .setLogic(thr_tau11.condition(1)&thr_xe20.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau16_xe15  = LVL1MenuItem('L1_TAU16_XE15', ctpid=0xd9)\
                           .setLogic(thr_tau16.condition(1)&thr_xe15.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau9i_xe15   = LVL1MenuItem('L1_TAU9I_XE15', ctpid=0xd1)\
                           .setLogic(thr_tau9i.condition(1)&thr_xe15.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau6i_xe15   = LVL1MenuItem('L1_TAU6I_XE15', ctpid=0xd1)\
                            .setLogic(thr_tau6i.condition(1)&thr_xe15.condition(1)&physcond)\
                            .setTriggerType(TT_calo)
        item_tau9i_xe20  = LVL1MenuItem('L1_TAU9I_XE20', ctpid=0xd0)\
                           .setLogic(thr_tau9i.condition(1)&thr_xe20.condition(1)&physcond)
        item_tau6i_xe20  = LVL1MenuItem('L1_TAU6I_XE20', ctpid=0xd0)\
                           .setLogic(thr_tau6i.condition(1)&thr_xe20.condition(1)&physcond)
        item_tau6i_xe10  = LVL1MenuItem('L1_TAU6I_XE10', ctpid=0xd0)\
                           .setLogic(thr_tau6i.condition(1)&thr_xe10.condition(1)&physcond)
        
        item_tau9i_xe15_empty   = LVL1MenuItem('L1_TAU9I_XE15_EMPTY', ctpid=-1)\
                           .setLogic(thr_tau9i.condition(1)&thr_xe15.condition(1)&cosmiccond)\
                           .setTriggerType(TT_calo)
        item_tau6i_xe10_empty   = LVL1MenuItem('L1_TAU6I_XE10_EMPTY', ctpid=-1)\
                           .setLogic(thr_tau6i.condition(1)&thr_xe10.condition(1)&cosmiccond)\
                           .setTriggerType(TT_calo)
        item_tau6_xe15_empty   = LVL1MenuItem('L1_TAU6_XE15_EMPTY', ctpid=-1)\
                           .setLogic(thr_tau6.condition(1)&thr_xe15.condition(1)&cosmiccond)\
                           .setTriggerType(TT_calo)
        item_tau5_xe10_empty   = LVL1MenuItem('L1_TAU5_XE10_EMPTY', ctpid=-1)\
                           .setLogic(thr_tau5.condition(1)&thr_xe10.condition(1)&cosmiccond)\
                           .setTriggerType(TT_calo)
        item_tau6_xe10_empty   = LVL1MenuItem('L1_TAU6_XE10_EMPTY', ctpid=-1)\
                           .setLogic(thr_tau6.condition(1)&thr_xe10.condition(1)&cosmiccond)\
                           .setTriggerType(TT_calo)

        
        item_tau9i_xe40  = LVL1MenuItem('L1_TAU9I_XE40', ctpid=0xd2)\
                           .setLogic(thr_tau9i.condition(1)&thr_xe40.condition(1)&physcond)
        item_tau9i_xe30 = LVL1MenuItem('L1_TAU9I_XE30', ctpid=0xd3)\
                           .setLogic(thr_tau9i.condition(1)&thr_xe30.condition(1)&physcond)
        item_tau16i_xe20 = LVL1MenuItem('L1_TAU16I_XE20', ctpid=0xd4)\
                           .setLogic(thr_tau16i.condition(1)&thr_xe20.condition(1)&physcond)
        item_tau11i_xe15 = LVL1MenuItem('L1_TAU11I_XE15', ctpid=0xd4)\
                           .setLogic(thr_tau11i.condition(1)&thr_xe15.condition(1)&physcond)
        item_tau15i_xe35 = LVL1MenuItem('L1_TAU15I_XE35', ctpid=-1)\
                           .setLogic(thr_tau15i.condition(1)&thr_xe35.condition(1)&physcond)
        item_tau15i_xe40 = LVL1MenuItem('L1_TAU15I_XE40', ctpid=-1)\
                           .setLogic(thr_tau15i.condition(1)&thr_xe40.condition(1)&physcond)
        item_tau20_xe40 = LVL1MenuItem('L1_TAU20_XE40', ctpid=-1)\
                          .setLogic(thr_tau20.condition(1)&thr_xe40.condition(1)&physcond)
        item_tau20_xe35 = LVL1MenuItem('L1_TAU20_XE35', ctpid=-1)\
                          .setLogic(thr_tau20.condition(1)&thr_xe35.condition(1)&physcond)
        
        item_tau9i_xe50 = LVL1MenuItem('L1_TAU9I_XE50', ctpid=0xd5)\
                           .setLogic(thr_tau9i.condition(1)&thr_xe50.condition(1)&physcond)
        item_tau6i_xe30 = LVL1MenuItem('L1_TAU6I_XE30', ctpid=0xd5)\
                           .setLogic(thr_tau6i.condition(1)&thr_xe30.condition(1)&physcond)
        item_tau40_xe40 = LVL1MenuItem('L1_TAU40_XE40', ctpid=0xd6)\
                          .setLogic(thr_tau40.condition(1)&thr_xe40.condition(1)&physcond)

        item_tau15_xs35  = LVL1MenuItem('L1_TAU15_XS35', ctpid=-1)\
                           .setLogic(thr_tau15.condition(1)&thr_xs35.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau15_xs25_3j10  = LVL1MenuItem('L1_TAU15_XS25_3J10', ctpid=-1)\
                                .setLogic(thr_tau15.condition(1)&thr_xs25.condition(1)&thr_j10.condition(3)&physcond)\
                                .setTriggerType(TT_calo)
        item_tau15_xe10_3j10 = LVL1MenuItem('L1_TAU15_XE10_3J10', ctpid=-1)\
                               .setLogic(thr_tau15.condition(1)&thr_xe10.condition(1)&thr_j10.condition(3)&physcond)\
                               .setTriggerType(TT_calo)
        item_tau15_xe25_3j15 = LVL1MenuItem('L1_TAU15_XE25_3J15', ctpid=-1)\
                               .setLogic(thr_tau15.condition(1)&thr_xe25.condition(1)&thr_j15.condition(3)&physcond)\
                               .setTriggerType(TT_calo)
        item_tau15_xe25_3j10 = LVL1MenuItem('L1_TAU15_XE25_3J10', ctpid=-1)\
                               .setLogic(thr_tau15.condition(1)&thr_xe25.condition(1)&thr_j10.condition(3)&physcond)\
                               .setTriggerType(TT_calo)
        item_tau15_xe25_3j10_j30 = LVL1MenuItem('L1_TAU15_XE25_3J10_J30', ctpid=-1)\
                                   .setLogic(thr_tau15.condition(1)&thr_xe25.condition(1)&thr_j10.condition(3)&thr_j30.condition(1)&physcond)\
                                   .setTriggerType(TT_calo)
        item_tau11_xs25  = LVL1MenuItem('L1_TAU11_XS25', ctpid=-1)\
                           .setLogic(thr_tau11.condition(1)&thr_xs25.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau11_xs15_3j10  = LVL1MenuItem('L1_TAU11_XS15_3J10', ctpid=-1)\
                                .setLogic(thr_tau11.condition(1)&thr_xs15.condition(1)&thr_j10.condition(3)&physcond)\
                                .setTriggerType(TT_calo)
        item_tau11_xs30  = LVL1MenuItem('L1_TAU11_XS30', ctpid=-1)\
                           .setLogic(thr_tau11.condition(1)&thr_xs30.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau11_xs35  = LVL1MenuItem('L1_TAU11_XS35', ctpid=-1)\
                           .setLogic(thr_tau11.condition(1)&thr_xs35.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_tau11_xs40  = LVL1MenuItem('L1_TAU11_XS40', ctpid=-1)\
                           .setLogic(thr_tau11.condition(1)&thr_xs40.condition(1)&physcond)\
                           .setTriggerType(TT_calo)

        item_j50_xs15  = LVL1MenuItem('L1_J50_XS15', ctpid=-1)\
                           .setLogic(thr_j50.condition(1)&thr_xs15.condition(1)&physcond)\
                           .setTriggerType(TT_calo)
        item_j50_xs25  = LVL1MenuItem('L1_J50_XS25', ctpid=-1)\
                           .setLogic(thr_j50.condition(1)&thr_xs25.condition(1)&physcond)\
                           .setTriggerType(TT_calo)


##VBF request
        
        item_2tau8_tau15_fj15  = LVL1MenuItem('L1_2TAU8_TAU15_FJ15', ctpid=-1)\
                                 .setLogic(thr_tau8.condition(2)&thr_tau15.condition(1)&(thr_jf15.condition(1)|thr_jb15.condition(1))&physcond)\
                                 .setTriggerType(TT_calo)
        
        item_2tau8_tau15_j15cf  = LVL1MenuItem('L1_2TAU8_TAU15_J15CF', ctpid=-1)\
                                  .setLogic(thr_tau8.condition(2)&thr_tau15.condition(1)&thr_j15cf.condition(1)&physcond)\
                                  .setTriggerType(TT_calo)
                
        item_2tau8_tau11i_em10vh_fj15  = LVL1MenuItem('L1_2TAU8_TAU11I_EM10VH_FJ15', ctpid=-1)\
                                         .setLogic(thr_tau8.condition(2)&thr_tau11i.condition(1)&thr_em10vh.condition(1)&(thr_jf15.condition(1)|thr_jb15.condition(1))&physcond)\
                                         .setTriggerType(TT_calo)

        item_2tau8_tau11i_em10vh_j15cf  = LVL1MenuItem('L1_2TAU8_TAU11I_EM10VH_J15CF', ctpid=-1)\
                                          .setLogic(thr_tau8.condition(2)&thr_tau11i.condition(1)&thr_em10vh.condition(1)&thr_j15cf.condition(1)&physcond)\
                                          .setTriggerType(TT_calo)

               
       ##L1_EM10VH_FJ15,  L1_EM10VH_J15CF , L1_MU10_FJ15, L1_MU10_J15CF , L1_3J15_FJ15,  L1_2J15_2FJ15, L1_XE30_FJ15,  L1_XE30_J15CF,  L1_J15CF_FJ15      

        item_em14vh_fj15  = LVL1MenuItem('L1_EM14VH_FJ15', ctpid=-1)\
                            .setLogic(thr_em14vh.condition(1)&(thr_jf15.condition(1)|thr_jb15.condition(1))&physcond)\
                            .setTriggerType(TT_calo|TT_caloem)
        item_em14vh_j15cf  = LVL1MenuItem('L1_EM14VH_J15CF', ctpid=-1)\
                             .setLogic(thr_em14vh.condition(1)&thr_j15cf.condition(1)&physcond)\
                             .setTriggerType(TT_calo|TT_caloem)

        item_mu10_fj15  = LVL1MenuItem('L1_MU10_FJ15', ctpid=-1)\
                          .setLogic(thr_mu10.condition(1)&(thr_jf15.condition(1)|thr_jb15.condition(1))&physcond)
        item_mu10_j15cf  = LVL1MenuItem('L1_MU10_J15CF', ctpid=-1)\
                           .setLogic(thr_mu10.condition(1)&thr_j15cf.condition(1)&physcond)

        item_3j15_fj15  = LVL1MenuItem('L1_3J15_FJ15', ctpid=-1)\
                          .setLogic(thr_j15.condition(3)&(thr_jf15.condition(1)|thr_jb15.condition(1))&physcond)\
                          .setTriggerType(TT_calo)
        item_2j15_2fj15  = LVL1MenuItem('L1_2J15_2FJ15', ctpid=-1)\
                           .setLogic(thr_j15.condition(2)&(thr_jf15.condition(1)&thr_jb15.condition(1))&physcond)\
                           .setTriggerType(TT_calo)              

        # Transient items
        item_2tau9i_em18i = LVL1MenuItem('L1_2TAU9I_EM18I', ctpid=-1)\
                             .setLogic(thr_tau9i.condition(2)&thr_em18i.condition(1)&physcond)
        item_tau25_xe30  = LVL1MenuItem('L1_TAU25_XE30', ctpid=-1)\
                           .setLogic(thr_tau25.condition(1)&thr_xe30.condition(1)&physcond)
        item_tau11_xe20  = LVL1MenuItem('L1_TAU11_XE20', ctpid=-1)\
                           .setLogic(thr_tau11.condition(1)&thr_xe20.condition(1)&physcond)
        # L1_TAU6_2JT4_JT35  (or whatever soft thresholds they have. JT4 must have 
        # softer cuts than TAU06 and JT35 - please get in touch with us if it is not 
        # the case - we will need to rethink it then)
        item_tau9i_2j5_j10  = LVL1MenuItem('L1_TAU9I_2J5_J10', ctpid=0xd8)\
                              .setLogic(thr_tau9i.condition(1)&thr_j5.condition(2)&thr_j10.condition(1)&physcond)
        item_tau9i_2j5_j20 = LVL1MenuItem('L1_TAU9I_2J5_J20', ctpid=-1)\
                              .setLogic(thr_tau9i.condition(1)&thr_j5.condition(2)&thr_j20.condition(1)&physcond) # new jet threshold
        item_tau6i_2j5_j20 = LVL1MenuItem('L1_TAU6I_2J5_J20', ctpid=-1)\
                             .setLogic(thr_tau6i.condition(1)&thr_j5.condition(2)&thr_j20.condition(1)&physcond)
        item_tau9i_2j5_j40 = LVL1MenuItem('L1_TAU9I_2J5_J40', ctpid=0xda)\
                              .setLogic(thr_tau9i.condition(1)&thr_j5.condition(2)&thr_j40.condition(1)&physcond) # new jet threshold
        item_tau6_2j5_j30 = LVL1MenuItem('L1_TAU6_2J5_J30', ctpid=0xda)\
                             .setLogic(thr_tau6.condition(1)&thr_j5.condition(2)&thr_j30.condition(1)&physcond) # new jet threshold
        item_tau6i_2j5_j40 = LVL1MenuItem('L1_TAU6I_2J5_J40', ctpid=0xda)\
                             .setLogic(thr_tau6i.condition(1)&thr_j5.condition(2)&thr_j40.condition(1)&physcond) # new jet threshold
        item_tau9i_4j5_3j20 = LVL1MenuItem('L1_TAU9I_4J5_3J20', ctpid=-1)\
                              .setLogic(thr_tau9i.condition(1)&thr_j5.condition(4)&thr_j20.condition(3)&physcond) # new jet threshold
        item_tau5_4j5_3j15 = LVL1MenuItem('L1_TAU5_4J5_3J15', ctpid=-1)\
                             .setLogic(thr_tau5.condition(1)&thr_j5.condition(4)&thr_j15.condition(3)&physcond)\
                             .setTriggerType(TT_calo)
        item_tau6_4j5_3j15 = LVL1MenuItem('L1_TAU6_4J5_3J15', ctpid=-1)\
                             .setLogic(thr_tau6.condition(1)&thr_j5.condition(4)&thr_j15.condition(3)&physcond)\
                             .setTriggerType(TT_calo) # new jet threshold
        item_tau6i_4j5_3j20 = LVL1MenuItem('L1_TAU6I_4J5_3J20', ctpid=-1)\
                              .setLogic(thr_tau6i.condition(1)&thr_j5.condition(4)&thr_j20.condition(3)&physcond) # new jet threshold
        item_tau9i_5j5_4j10 = LVL1MenuItem('L1_TAU9I_5J5_4J10', ctpid=-1)\
                              .setLogic(thr_tau9i.condition(1)&thr_j5.condition(5)&thr_j10.condition(4)&physcond) # new jet threshold
        item_tau6i_5j5_4j10 = LVL1MenuItem('L1_TAU6I_5J5_4J10', ctpid=-1)\
                              .setLogic(thr_tau6i.condition(1)&thr_j5.condition(5)&thr_j10.condition(4)&physcond) # new jet threshold
        item_tau9i_5j5_4j20 = LVL1MenuItem('L1_TAU9I_5J5_4J20', ctpid=0xdb)\
                              .setLogic(thr_tau9i.condition(1)&thr_j5.condition(5)&thr_j20.condition(4)&physcond) # new jet threshold
        item_tau6_5j5_4j15 = LVL1MenuItem('L1_TAU6_5J5_4J15', ctpid=0xdb)\
                              .setLogic(thr_tau6.condition(1)&thr_j5.condition(5)&thr_j15.condition(4)&physcond) # new jet threshold
        item_tau6i_5j5_4j20 = LVL1MenuItem('L1_TAU6I_5J5_4J20', ctpid=0xdb)\
                              .setLogic(thr_tau6i.condition(1)&thr_j5.condition(5)&thr_j20.condition(4)&physcond) # new jet threshold
        item_tau9i_2j5_j70 = LVL1MenuItem('L1_TAU9I_2J5_J70', ctpid=-1)\
                             .setLogic(thr_tau9i.condition(1)&thr_j5.condition(2)&thr_j70.condition(1)&physcond) # new jet threshold
        item_tau6_2j5_j55 = LVL1MenuItem('L1_TAU6_2J5_J55', ctpid=-1)\
                             .setLogic(thr_tau6.condition(1)&thr_j5.condition(2)&thr_j55.condition(1)&physcond) # new jet threshold
        item_tau6i_2j5_j70 = LVL1MenuItem('L1_TAU6I_2J5_J70', ctpid=-1)\
                             .setLogic(thr_tau6i.condition(1)&thr_j5.condition(2)&thr_j70.condition(1)&physcond) # new jet threshold
        item_tau9i_3j5_2j40 = LVL1MenuItem('L1_TAU9I_3J5_2J40', ctpid=-1)\
                               .setLogic(thr_tau9i.condition(1)&thr_j5.condition(3)&thr_j40.condition(2)&physcond) # new jet threshold
        item_tau9i_3j5_2j10 = LVL1MenuItem('L1_TAU9I_3J5_2J10', ctpid=-1)\
                               .setLogic(thr_tau9i.condition(1)&thr_j5.condition(3)&thr_j10.condition(2)&physcond) # new jet threshold
        item_tau6i_3j5_2j10 = LVL1MenuItem('L1_TAU6I_3J5_2J10', ctpid=-1)\
                               .setLogic(thr_tau6i.condition(1)&thr_j5.condition(3)&thr_j10.condition(2)&physcond) # new jet threshold
        item_tau9i_3j5_2j20 = LVL1MenuItem('L1_TAU9I_3J5_2J20', ctpid=0xdd)\
                               .setLogic(thr_tau9i.condition(1)&thr_j5.condition(3)&thr_j20.condition(2)&physcond) # new jet threshold
        item_tau5_3j5_2j15 = LVL1MenuItem('L1_TAU5_3J5_2J15', ctpid=-1)\
                              .setLogic(thr_tau5.condition(1)&thr_j5.condition(3)&thr_j15.condition(2)&physcond) # new jet threshold        
        item_tau6_3j5_2j15 = LVL1MenuItem('L1_TAU6_3J5_2J15', ctpid=-1)\
                              .setLogic(thr_tau6.condition(1)&thr_j5.condition(3)&thr_j15.condition(2)&physcond) # new jet threshold
        item_tau6i_3j5_2j20 = LVL1MenuItem('L1_TAU6I_3J5_2J20', ctpid=0xdd)\
                              .setLogic(thr_tau6i.condition(1)&thr_j5.condition(3)&thr_j20.condition(2)&physcond) # new jet threshold
        # 'L1_TAU6_3J5_2J20_XE25', 
        item_tau6_3j5_2j20_xe25 = LVL1MenuItem('L1_TAU6_3J5_2J20_XE25', ctpid=-1)\
                                 .setLogic(thr_tau6.condition(1)&thr_j5.condition(3)&thr_j20.condition(2)&\
                                           thr_xe25.condition(1)&physcond)

        item_2tau11_j10 = LVL1MenuItem('L1_2TAU11_J10', ctpid=-1)\
                          .setLogic(thr_tau11.condition(2)&thr_j10.condition(1)&physcond)
        item_2tau6_tau11_j5 = LVL1MenuItem('L1_2TAU6_TAU11_J5', ctpid=-1)\
                              .setLogic(thr_tau6.condition(2)&thr_tau11.condition(1)&thr_j5.condition(1)&physcond)
        item_2tau6_tau11_j10 = LVL1MenuItem('L1_2TAU6_TAU11_J10', ctpid=-1)\
                              .setLogic(thr_tau6.condition(2)&thr_tau11.condition(1)&thr_j10.condition(1)&physcond)
        item_2tau8_tau11_j10 = LVL1MenuItem('L1_2TAU8_TAU11_J10', ctpid=-1)\
                              .setLogic(thr_tau8.condition(2)&thr_tau11.condition(1)&thr_j10.condition(1)&physcond)
        
        item_tau6_j50_xe20 = LVL1MenuItem('L1_TAU6_J50_XE20', ctpid=-1)\
                             .setLogic(thr_tau6.condition(1)&thr_j50.condition(1)&\
                                       thr_xe20.condition(1)&physcond)

        item_j50_xe20 = LVL1MenuItem('L1_J50_XE20', ctpid=-1)\
                             .setLogic(thr_j50.condition(1)&\
                                       thr_xe20.condition(1)&physcond)

        item_j50_xe25 = LVL1MenuItem('L1_J50_XE25', ctpid=-1)\
                             .setLogic(thr_j50.condition(1)&\
                                       thr_xe25.condition(1)&physcond)

        item_j50_xe30 = LVL1MenuItem('L1_J50_XE30', ctpid=-1)\
                             .setLogic(thr_j50.condition(1)&\
                                       thr_xe30.condition(1)&physcond)

        item_j50_xe35 = LVL1MenuItem('L1_J50_XE35', ctpid=-1)\
                             .setLogic(thr_j50.condition(1)&\
                                       thr_xe35.condition(1)&physcond)

        item_j50_xe40 = LVL1MenuItem('L1_J50_XE40', ctpid=-1)\
                             .setLogic(thr_j50.condition(1)&\
                                       thr_xe40.condition(1)&physcond)

        item_2j20_xe20 = LVL1MenuItem('L1_2J20_XE20', ctpid=-1)\
                             .setLogic(thr_j20.condition(2)&\
                                       thr_xe20.condition(1)&physcond)

        item_2j15_xe30 = LVL1MenuItem('L1_2J15_XE30', ctpid=-1)\
                             .setLogic(thr_j15.condition(2)&\
                                       thr_xe30.condition(1)&physcond)

        item_3j10_j50 = LVL1MenuItem('L1_3J10_J50', ctpid=-1)\
                             .setLogic(thr_j10.condition(3)&\
                                       thr_j50.condition(1)&physcond)

        item_3j10_2j75 = LVL1MenuItem('L1_3J10_2J75', ctpid=-1)\
                             .setLogic(thr_j10.condition(3)&\
                                       thr_j75.condition(2)&physcond)


        # - Add/remap new L1 thresholds:
        #   J20->J15, J40->J30, J70->J55, J100->J75, J130->J95, J150->J115
        item_tau6i_2j5_j30 = LVL1MenuItem('L1_TAU6I_2J5_J30', ctpid=-1, group='1')\
                             .setLogic(thr_tau6i.condition(1)&thr_j5.condition(2)&thr_j30.condition(1)&physcond)
        item_tau6i_5j5_4j15 = LVL1MenuItem('L1_TAU6I_5J5_4J15',ctpid=-1, group='1')\
                              .setLogic(thr_tau6i.condition(1)&thr_j5.condition(5)&thr_j15.condition(4)&physcond)
        item_tau6i_4j5_3j15 = LVL1MenuItem('L1_TAU6I_4J5_3J15',ctpid=-1, group='1')\
                              .setLogic(thr_tau6i.condition(1)&thr_j5.condition(4)&thr_j15.condition(3)&physcond)
        item_tau6i_3j5_2j15 = LVL1MenuItem('L1_TAU6I_3J5_2J15',ctpid=-1, group='1')\
                             .setLogic(thr_tau6i.condition(1)&thr_j5.condition(3)&thr_j15.condition(2)&physcond)
        item_tau6i_2j5_j15 = LVL1MenuItem('L1_TAU6I_2J5_J15',ctpid=-1, group='1')\
                             .setLogic(thr_tau6i.condition(1)&thr_j5.condition(2)&thr_j15.condition(1)&physcond)
        item_tau6i_2j5_j55 = LVL1MenuItem('L1_TAU6I_2J5_J55',ctpid=-1, group='1')\
                             .setLogic(thr_tau6i.condition(1)&thr_j5.condition(2)&thr_j55.condition(1)&physcond)

        item_j30_xe10 = LVL1MenuItem('L1_J30_XE10',ctpid=-1, group='1')\
                        .setLogic(thr_j30.condition(1)&thr_xe10.condition(1)&physcond)
        item_j30_xe35 = LVL1MenuItem('L1_J30_XE35',ctpid=-1, group='1')\
                        .setLogic(thr_j30.condition(1)&thr_xe35.condition(1)&physcond)
        item_j30_xe15 = LVL1MenuItem('L1_J30_XE15',ctpid=-1, group='1')\
                        .setLogic(thr_j30.condition(1)&thr_xe15.condition(1)&physcond)
        item_j50_xe15 = LVL1MenuItem('L1_J50_XE15',ctpid=-1, group='1')\
                        .setLogic(thr_j50.condition(1)&thr_xe15.condition(1)&physcond)
        item_j30_xe25 = LVL1MenuItem('L1_J30_XE25',ctpid=-1, group='1')\
                        .setLogic(thr_j30.condition(1)&thr_xe25.condition(1)&physcond)
        item_j30_xe40 = LVL1MenuItem('L1_J30_XE40',ctpid=-1, group='1')\
                        .setLogic(thr_j30.condition(1)&thr_xe40.condition(1)&physcond)
        item_j30_xe50 = LVL1MenuItem('L1_J30_XE50',ctpid=-1, group='1')\
                        .setLogic(thr_j30.condition(1)&thr_xe50.condition(1)&physcond)
        

        item_2j15_xe10 = LVL1MenuItem('L1_2J15_XE10',ctpid=-1, group='1')\
                         .setLogic(thr_j15.condition(2)&thr_xe10.condition(1)&physcond)
        item_2j15_xe15 = LVL1MenuItem('L1_2J15_XE15',ctpid=-1, group='1')\
                         .setLogic(thr_j15.condition(2)&thr_xe15.condition(1)&physcond)
        item_2j15_xe25 = LVL1MenuItem('L1_2J15_XE25',ctpid=-1, group='1')\
                         .setLogic(thr_j15.condition(2)&thr_xe25.condition(1)&physcond)
        
        item_j15_xe15_em10  = LVL1MenuItem('L1_J15_XE15_EM10',ctpid=-1, group='1')\
                              .setLogic(thr_em10.condition(1)&thr_j15.condition(1)&thr_xe15.condition(1)&physcond)
        item_j15_xe20_em10  = LVL1MenuItem('L1_J15_XE20_EM10',ctpid=-1, group='1')\
                              .setLogic(thr_em10.condition(1)&thr_j15.condition(1)&thr_xe20.condition(1)&physcond)
        item_j15_xe15_em10i = LVL1MenuItem('L1_J15_XE15_EM10I',ctpid=-1, group='1')\
                              .setLogic(thr_em10i.condition(1)&thr_j15.condition(1)&thr_xe15.condition(1)&physcond)
        item_j15_xe20_mu15 = LVL1MenuItem('L1_J15_XE20_MU15',ctpid=-1, group='1')\
                              .setLogic(thr_mu15.condition(1)&thr_j15.condition(1)&thr_xe20.condition(1)&physcond)

        #'L1_TAU6I_2J5_J15',
        #'L1_TAU6I_2J5_J55', 

        # Transient items
        item_tau9i_2j5_j70 = LVL1MenuItem('L1_TAU9I_2J5_J70', ctpid=-1)\
                              .setLogic(thr_tau9i.condition(1)&thr_j5.condition(2)&thr_j70.condition(1)&physcond)

        # MBTS


#        item_mbts1 = LVL1MenuItem('L1_MBTS_1', ctpid=0xe2)\
#                     .setLogic(Logic(bgrp0)&Logic(bgrp1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))&(thr_mbtsA.condition(1)|thr_mbtsC.condition(1)))\
#                     .setTriggerType(TT_minb)


#        item_mbts2 = LVL1MenuItem('L1_MBTS_2', ctpid=0xe3)\
#                     .setLogic(Logic(bgrp0)&Logic(bgrp1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1))\
#                               &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
#                               &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
#                               )\
#                               .setTriggerType(TT_minb)

#        item_mbts1_1 = LVL1MenuItem('L1_MBTS_1_1', ctpid=0xe4)\
#                       .setLogic(Logic(bgrp0)&Logic(bgrp1)&thr_mbtsA.condition(1)&thr_mbtsC.condition(1)&(thr_bptx0.condition(1)|thr_bptx1.condition(1)))\
#                       .setTriggerType(TT_minb)
        
        item_mbtsa = LVL1MenuItem('L1_MBTS_A', ctpid=0x1a).setLogic(thr_mbtsA.condition(1)&physcond).setTriggerType(TT_minb)
        item_mbtsc = LVL1MenuItem('L1_MBTS_C', ctpid=0x1b).setLogic(thr_mbtsC.condition(1)&physcond).setTriggerType(TT_minb)


        item_mbtsa_empty = LVL1MenuItem('L1_MBTS_A_EMPTY', ctpid=-1).setLogic(thr_mbtsA.condition(1)&cosmiccond).setTriggerType(TT_minb)
        item_mbtsc_empty = LVL1MenuItem('L1_MBTS_C_EMPTY', ctpid=-1).setLogic(thr_mbtsC.condition(1)&cosmiccond).setTriggerType(TT_minb)


        item_mbts1 = LVL1MenuItem('L1_MBTS_1', ctpid=0xe2)\
                     .setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))&physcond)\
                     .setTriggerType(TT_minb)
#                     .setLogic(Logic(bgrp0)&Logic(bgrp1)&(thr_mbtsA.condition(1)|thr_mbtsC.condition(1)))\


        item_mlz_a = LVL1MenuItem('L1_MLZ_A', ctpid=-1)\
                     .setLogic((thr_mbtsA.condition(1)|thr_zdcA.condition(1)|thr_lucidA.condition(1))&physcond)\
                     .setTriggerType(TT_minb)

        item_mlz_c = LVL1MenuItem('L1_MLZ_C', ctpid=-1)\
                     .setLogic((thr_mbtsC.condition(1)|thr_zdcC.condition(1)|thr_lucidC.condition(1))&physcond)\
                     .setTriggerType(TT_minb)

        item_mblz = LVL1MenuItem('L1_MBLZ', ctpid=-1)\
                    .setLogic(
            ((thr_mbtsA.condition(1)|thr_zdcA.condition(1)|thr_lucidA.condition(1))\
            & (thr_mbtsC.condition(1)|thr_zdcC.condition(1)|thr_lucidC.condition(1))\
            | (thr_bcmWide.condition(1))) \
            &physcond) \
            .setTriggerType(TT_minb)

        item_mbts2 = LVL1MenuItem('L1_MBTS_2', ctpid=0xe3)\
                     .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                               &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                               &physcond).setTriggerType(TT_minb)

        item_mbts2_veto_te50  = LVL1MenuItem('L1_MBTS_2_VTE50', ctpid=-1)\
                                 .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                           &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                           &Not(thr_te50.condition(1))&physcond)\
                                           .setTriggerType(TT_minb)

        item_mbts2_veto_te100  = LVL1MenuItem('L1_MBTS_2_VTE100', ctpid=-1)\
                                 .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                           &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                           &Not(thr_te100.condition(1))&physcond)\
                                           .setTriggerType(TT_minb)
        
        item_mbts2_veto_te1000  = LVL1MenuItem('L1_MBTS_2_VTE1000', ctpid=-1)\
                                  .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                            &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                            &Not(thr_te1000.condition(1))&physcond)\
                                            .setTriggerType(TT_minb)

        item_mbts2_veto_te500  = LVL1MenuItem('L1_MBTS_2_VTE500', ctpid=-1)\
                                  .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                            &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                            &Not(thr_te500.condition(1))&physcond)\
                                            .setTriggerType(TT_minb)



        item_mbts1_1_overlay = LVL1MenuItem('L1_MBTS_1_1_OVERLAY', ctpid=0xe4)\
                       .setLogic(thr_mbtsA.condition(1)&thr_mbtsC.condition(1)&physcond)\
                       .setTriggerType(TT_zerobs)

        item_mbts1_1 = LVL1MenuItem('L1_MBTS_1_1', ctpid=0xe4)\
                       .setLogic(thr_mbtsA.condition(1)&thr_mbtsC.condition(1)&physcond)\
                       .setTriggerType(TT_minb)

        item_mbts1_1_veto_te50 = LVL1MenuItem('L1_MBTS_1_1_VTE50', ctpid=0xe4)\
                       .setLogic(thr_mbtsA.condition(1)&thr_mbtsC.condition(1)&Not(thr_te50.condition(1))&physcond)\
                       .setTriggerType(TT_minb)

        item_mbts2_2 = LVL1MenuItem('L1_MBTS_2_2', ctpid=-1)\
                       .setLogic(thr_mbtsA.condition(2)&thr_mbtsC.condition(2)&physcond)\
                       .setTriggerType(TT_minb)

        item_mbts2_2_veto_te50 = LVL1MenuItem('L1_MBTS_2_2_VTE50', ctpid=-1)\
                       .setLogic(thr_mbtsA.condition(2)&thr_mbtsC.condition(2)&Not(thr_te50.condition(1))&physcond)\
                       .setTriggerType(TT_minb)

        item_mbts3_3 = LVL1MenuItem('L1_MBTS_3_3', ctpid=-1)\
                       .setLogic(thr_mbtsA.condition(3)&thr_mbtsC.condition(3)&physcond)\
                       .setTriggerType(TT_minb)

        item_mbts4_4 = LVL1MenuItem('L1_MBTS_4_4', ctpid=0xe1)\
                       .setLogic(thr_mbtsA.condition(4)&thr_mbtsC.condition(4)&physcond)\
                       .setTriggerType(TT_minb)

        item_mbts4_4_BGRP0_noCALIB = LVL1MenuItem('L1_MBTS_4_4_BGRP0_noCALIB', ctpid=0xe1)\
                             .setLogic(thr_mbtsA.condition(4)&thr_mbtsC.condition(4)&Logic(bgrp0)&Not(Logic(bgrp2)))\
                             .setTriggerType(TT_minb)

        item_mbts1_comm   = LVL1MenuItem('L1_MBTS_1_COMM', ctpid=-1)\
                            .setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))&physcond)\
                            .setTriggerType(TT_minb)

        item_mbts2_comm   = LVL1MenuItem('L1_MBTS_2_COMM', ctpid=-1)\
                            .setLogic((thr_bptx0.condition(1)|thr_bptx1.condition(1))\
                                      &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                      &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                      &physcond\
                                      )\
                                      .setTriggerType(TT_minb)

        item_mbts1_1_comm = LVL1MenuItem('L1_MBTS_1_1_COMM', ctpid=-1)\
                            .setLogic(thr_mbtsA.condition(1)&thr_mbtsC.condition(1)&physcond)\
                            .setTriggerType(TT_minb)



        item_mbts1_bgrp1   = LVL1MenuItem('L1_MBTS_1_BGRP1', ctpid=-1)\
                            .setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))&bgrpcond)\
                            .setTriggerType(TT_minb)

        item_mbts2_bgrp1   = LVL1MenuItem('L1_MBTS_2_BGRP1', ctpid=-1)\
                            .setLogic((thr_bptx0.condition(1)|thr_bptx1.condition(1))\
                                      &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                      &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                      &bgrpcond\
                                      )\
                                      .setTriggerType(TT_minb)

        item_mbts1_1_bgrp1 = LVL1MenuItem('L1_MBTS_1_1_BGRP1', ctpid=-1)\
                            .setLogic(thr_mbtsA.condition(1)&thr_mbtsC.condition(1)&bgrpcond)\
                            .setTriggerType(TT_minb)



        item_mbts1_empty   = LVL1MenuItem('L1_MBTS_1_EMPTY', ctpid=-1)\
                            .setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))&cosmiccond)\
                            .setTriggerType(TT_minb)

        item_mbts2_empty   = LVL1MenuItem('L1_MBTS_2_EMPTY', ctpid=-1)\
                            .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                      &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                      &cosmiccond).setTriggerType(TT_minb)

        item_mbts1_1_empty = LVL1MenuItem('L1_MBTS_1_1_EMPTY', ctpid=-1)\
                            .setLogic(thr_mbtsA.condition(1)&thr_mbtsC.condition(1)&cosmiccond)\
                            .setTriggerType(TT_minb)

        item_mbts2_2_empty = LVL1MenuItem('L1_MBTS_2_2_EMPTY', ctpid=-1)\
                            .setLogic(thr_mbtsA.condition(2)&thr_mbtsC.condition(2)&cosmiccond)\
                            .setTriggerType(TT_minb)

        item_mbts3_3_empty = LVL1MenuItem('L1_MBTS_3_3_EMPTY', ctpid=-1)\
                            .setLogic(thr_mbtsA.condition(3)&thr_mbtsC.condition(3)&cosmiccond)\
                            .setTriggerType(TT_minb)

        item_mbts4_4_empty = LVL1MenuItem('L1_MBTS_4_4_EMPTY', ctpid=-1)\
                            .setLogic(thr_mbtsA.condition(4)&thr_mbtsC.condition(4)&cosmiccond)\
                            .setTriggerType(TT_minb)


        item_mbts1_unpaired1  = LVL1MenuItem('L1_MBTS_1_UNPAIRED1', ctpid=-1)\
                                .setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))&unpaired1cond)\
                                .setTriggerType(TT_minb)

        item_mbts1_bgrp7  = LVL1MenuItem('L1_MBTS_1_BGRP7', ctpid=-1)\
                                .setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))&bgrp7cond)\
                                .setTriggerType(TT_minb)

#        item_mbts2_bgrp7  = LVL1MenuItem('L1_MBTS_2_BGRP7', ctpid=-1)\
#                                .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2))&bgrp7cond)\
#                                .setTriggerType(TT_minb)

        item_mbts2_brgp7 = LVL1MenuItem('L1_MBTS_2_BGRP7', ctpid=-1)\
                           .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                     &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                     &bgrp7cond).setTriggerType(TT_minb)
        

        item_mbts1_unpaired2  = LVL1MenuItem('L1_MBTS_1_UNPAIRED2', ctpid=-1)\
                                .setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))&unpaired2cond)\
                                .setTriggerType(TT_minb)

        item_mbts1_unpairedR  = LVL1MenuItem('L1_MBTS_1_UNPAIRED', ctpid=-1)\
                                .setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))&unpairedRcond)\
                                .setTriggerType(TT_minb)

        item_mbts1_unpaired_iso  = LVL1MenuItem('L1_MBTS_1_UNPAIRED_ISO', ctpid=-1)\
                                   .setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))&unpaired_isocond)\
                                   .setTriggerType(TT_minb)

        item_mbts1_unpaired_noniso  = LVL1MenuItem('L1_MBTS_1_UNPAIRED_NONISO', ctpid=-1)\
                                   .setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))&unpaired_nonisocond)\
                                   .setTriggerType(TT_minb)

        item_mbts1_empty1stR  = LVL1MenuItem('L1_MBTS_1_FIRSTEMPTY', ctpid=-1)\
                                .setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))&empty1stcond)\
                                .setTriggerType(TT_minb)
                
        item_mbts2_unpaired1  = LVL1MenuItem('L1_MBTS_2_UNPAIRED1', ctpid=-1)\
                                .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                          &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                          &unpaired1cond).setTriggerType(TT_minb)

        item_mbts2_unpaired_noniso  = LVL1MenuItem('L1_MBTS_2_UNPAIRED_NONISO', ctpid=-1)\
                                      .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                                &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                                &unpaired_nonisocond).setTriggerType(TT_minb)

        item_mbts2_unpaired2  = LVL1MenuItem('L1_MBTS_2_UNPAIRED2', ctpid=-1)\
                                .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                          &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                          &unpaired2cond).setTriggerType(TT_minb)

        item_mbts2_unpairedR  = LVL1MenuItem('L1_MBTS_2_UNPAIRED', ctpid=-1)\
                                .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                          &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                          &unpairedRcond).setTriggerType(TT_minb)

        item_mbts2_unpaired_iso  = LVL1MenuItem('L1_MBTS_2_UNPAIRED_ISO', ctpid=-1)\
                                   .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                             &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                             &unpaired_isocond).setTriggerType(TT_minb)

        item_mbts2_empty1stR  = LVL1MenuItem('L1_MBTS_2_FIRSTEMPTY', ctpid=-1)\
                                .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                          &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                          &empty1stcond).setTriggerType(TT_minb)
        
        item_mbts1_1_unpaired1 = LVL1MenuItem('L1_MBTS_1_1_UNPAIRED1', ctpid=-1)\
                                 .setLogic(thr_mbtsA.condition(1)&thr_mbtsC.condition(1)&unpaired1cond)\
                                 .setTriggerType(TT_minb)

        item_mbts1_1_unpaired2 = LVL1MenuItem('L1_MBTS_1_1_UNPAIRED2', ctpid=-1)\
                                 .setLogic(thr_mbtsA.condition(1)&thr_mbtsC.condition(1)&unpaired2cond)\
                                 .setTriggerType(TT_minb)

        item_mbts1_1_unpairedR = LVL1MenuItem('L1_MBTS_1_1_UNPAIRED', ctpid=-1)\
                                 .setLogic(thr_mbtsA.condition(1)&thr_mbtsC.condition(1)&unpairedRcond)\
                                 .setTriggerType(TT_minb)

        item_mbts1_1_unpaired_iso = LVL1MenuItem('L1_MBTS_1_1_UNPAIRED_ISO', ctpid=-1)\
                                    .setLogic(thr_mbtsA.condition(1)&thr_mbtsC.condition(1)&unpaired_isocond)\
                                    .setTriggerType(TT_minb)

        item_mbts1_1_empty1stR = LVL1MenuItem('L1_MBTS_1_1_FIRSTEMPTY', ctpid=-1)\
                                .setLogic(thr_mbtsA.condition(1)&thr_mbtsC.condition(1)&empty1stcond)\
                                .setTriggerType(TT_minb)
        
        item_mbts4_4_unpaired1 = LVL1MenuItem('L1_MBTS_4_4_UNPAIRED1', ctpid=-1)\
                                 .setLogic(thr_mbtsA.condition(4)&thr_mbtsC.condition(4)&unpaired1cond)\
                                 .setTriggerType(TT_minb)

        item_mbts4_4_unpaired2 = LVL1MenuItem('L1_MBTS_4_4_UNPAIRED2', ctpid=-1)\
                                 .setLogic(thr_mbtsA.condition(4)&thr_mbtsC.condition(4)&unpaired2cond)\
                                 .setTriggerType(TT_minb)

        item_mbts4_4_unpairedR = LVL1MenuItem('L1_MBTS_4_4_UNPAIRED', ctpid=-1)\
                                 .setLogic(thr_mbtsA.condition(4)&thr_mbtsC.condition(4)&unpairedRcond)\
                                 .setTriggerType(TT_minb)

        item_mbts2_2_unpaired_iso = LVL1MenuItem('L1_MBTS_2_2_UNPAIRED_ISO', ctpid=-1)\
                                    .setLogic(thr_mbtsA.condition(2)&thr_mbtsC.condition(2)&unpaired_isocond)\
                                    .setTriggerType(TT_minb)

        item_mbts3_3_unpaired_iso = LVL1MenuItem('L1_MBTS_3_3_UNPAIRED_ISO', ctpid=-1)\
                                    .setLogic(thr_mbtsA.condition(3)&thr_mbtsC.condition(3)&unpaired_isocond)\
                                    .setTriggerType(TT_minb)
                
        item_mbts4_4_unpaired_iso = LVL1MenuItem('L1_MBTS_4_4_UNPAIRED_ISO', ctpid=-1)\
                                    .setLogic(thr_mbtsA.condition(4)&thr_mbtsC.condition(4)&unpaired_isocond)\
                                    .setTriggerType(TT_minb)

        item_mbts4_4_unpaired_noniso = LVL1MenuItem('L1_MBTS_4_4_UNPAIRED_NONISO', ctpid=-1)\
                                       .setLogic(thr_mbtsA.condition(4)&thr_mbtsC.condition(4)&unpaired_nonisocond)\
                                       .setTriggerType(TT_minb)


        item_mbts4_4_empty1stR = LVL1MenuItem('L1_MBTS_4_4_FIRSTEMPTY', ctpid=-1)\
                                .setLogic(thr_mbtsA.condition(4)&thr_mbtsC.condition(4)&empty1stcond)\
                                .setTriggerType(TT_minb)

                
        item_mbts1_col = LVL1MenuItem('L1_MBTS_1_Col', ctpid=-1)\
                         .setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))\
                                   &Logic(bgrp0)&thr_bptx0.condition(1)&thr_bptx1.condition(1))\
                                   .setTriggerType(TT_minb)

        item_mbts2_col = LVL1MenuItem('L1_MBTS_2_Col', ctpid=-1)\
                         .setLogic((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                   &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))\
                                   &Logic(bgrp0)&thr_bptx0.condition(1)&thr_bptx1.condition(1))\
                                   .setTriggerType(TT_minb)

        item_mbts1_1_col = LVL1MenuItem('L1_MBTS_1_1_Col', ctpid=-1)\
                           .setLogic(thr_mbtsA.condition(1)&thr_mbtsC.condition(1)\
                                     &Logic(bgrp0)&thr_bptx0.condition(1)&thr_bptx1.condition(1))\
                                     .setTriggerType(TT_minb)


        # Debug items for CTP OR function
        item_mbts_A_bptx0 = LVL1MenuItem('L1_MBTS_A_BPTX0', ctpid=-1)\
                            .setLogic(Logic(bgrp0)&thr_bptx0.condition(1)&thr_mbtsA.condition(1))\
                            .setTriggerType(TT_minb)
        item_mbts_C_bptx0 = LVL1MenuItem('L1_MBTS_C_BPTX0', ctpid=-1)\
                            .setLogic(Logic(bgrp0)&thr_bptx0.condition(1)&thr_mbtsC.condition(1))\
                            .setTriggerType(TT_minb)
        item_mbts_A_bptx1 = LVL1MenuItem('L1_MBTS_A_BPTX1', ctpid=-1)\
                            .setLogic(Logic(bgrp0)&thr_bptx1.condition(1)&thr_mbtsA.condition(1))\
                            .setTriggerType(TT_minb)
        item_mbts_C_bptx1 = LVL1MenuItem('L1_MBTS_C_BPTX1', ctpid=-1)\
                            .setLogic(Logic(bgrp0)&thr_bptx1.condition(1)&thr_mbtsC.condition(1))\
                            .setTriggerType(TT_minb)

        item_mbts_a_0 = LVL1MenuItem('L1_MBTSA0', ctpid=-1)\
                        .setLogic(thr_mbtsA0.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_1 = LVL1MenuItem('L1_MBTSA1', ctpid=-1)\
                        .setLogic(thr_mbtsA1.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_2 = LVL1MenuItem('L1_MBTSA2', ctpid=-1)\
                        .setLogic(thr_mbtsA2.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_3 = LVL1MenuItem('L1_MBTSA3', ctpid=-1)\
                        .setLogic(thr_mbtsA3.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_4 = LVL1MenuItem('L1_MBTSA4', ctpid=-1)\
                        .setLogic(thr_mbtsA4.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_5 = LVL1MenuItem('L1_MBTSA5', ctpid=-1)\
                        .setLogic(thr_mbtsA5.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_6 = LVL1MenuItem('L1_MBTSA6', ctpid=-1)\
                        .setLogic(thr_mbtsA6.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_7 = LVL1MenuItem('L1_MBTSA7', ctpid=-1)\
                        .setLogic(thr_mbtsA7.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_8 = LVL1MenuItem('L1_MBTSA8', ctpid=-1)\
                        .setLogic(thr_mbtsA8.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_9 = LVL1MenuItem('L1_MBTSA9', ctpid=-1)\
                        .setLogic(thr_mbtsA9.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_10 = LVL1MenuItem('L1_MBTSA10', ctpid=-1)\
                        .setLogic(thr_mbtsA10.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_11 = LVL1MenuItem('L1_MBTSA11', ctpid=-1)\
                        .setLogic(thr_mbtsA11.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_12 = LVL1MenuItem('L1_MBTSA12', ctpid=-1)\
                        .setLogic(thr_mbtsA12.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_13 = LVL1MenuItem('L1_MBTSA13', ctpid=-1)\
                        .setLogic(thr_mbtsA13.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_14 = LVL1MenuItem('L1_MBTSA14', ctpid=-1)\
                        .setLogic(thr_mbtsA14.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_a_15 = LVL1MenuItem('L1_MBTSA15', ctpid=-1)\
                        .setLogic(thr_mbtsA15.condition(1)&physcond)\
                        .setTriggerType(TT_minb)

        item_mbts_c_0 = LVL1MenuItem('L1_MBTSC0', ctpid=-1)\
                        .setLogic(thr_mbtsC0.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_1 = LVL1MenuItem('L1_MBTSC1', ctpid=-1)\
                        .setLogic(thr_mbtsC1.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_2 = LVL1MenuItem('L1_MBTSC2', ctpid=-1)\
                        .setLogic(thr_mbtsC2.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_3 = LVL1MenuItem('L1_MBTSC3', ctpid=-1)\
                        .setLogic(thr_mbtsC3.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_4 = LVL1MenuItem('L1_MBTSC4', ctpid=-1)\
                        .setLogic(thr_mbtsC4.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_5 = LVL1MenuItem('L1_MBTSC5', ctpid=-1)\
                        .setLogic(thr_mbtsC5.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_6 = LVL1MenuItem('L1_MBTSC6', ctpid=-1)\
                        .setLogic(thr_mbtsC6.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_7 = LVL1MenuItem('L1_MBTSC7', ctpid=-1)\
                        .setLogic(thr_mbtsC7.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_8 = LVL1MenuItem('L1_MBTSC8', ctpid=-1)\
                        .setLogic(thr_mbtsC8.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_9 = LVL1MenuItem('L1_MBTSC9', ctpid=-1)\
                        .setLogic(thr_mbtsC9.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_10 = LVL1MenuItem('L1_MBTSC10', ctpid=-1)\
                        .setLogic(thr_mbtsC10.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_11 = LVL1MenuItem('L1_MBTSC11', ctpid=-1)\
                        .setLogic(thr_mbtsC11.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_12 = LVL1MenuItem('L1_MBTSC12', ctpid=-1)\
                        .setLogic(thr_mbtsC12.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_13 = LVL1MenuItem('L1_MBTSC13', ctpid=-1)\
                        .setLogic(thr_mbtsC13.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_14 = LVL1MenuItem('L1_MBTSC14', ctpid=-1)\
                        .setLogic(thr_mbtsC14.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        item_mbts_c_15 = LVL1MenuItem('L1_MBTSC15', ctpid=-1)\
                        .setLogic(thr_mbtsC15.condition(1)&physcond)\
                        .setTriggerType(TT_minb)
        
        item_trt_fast = LVL1MenuItem('L1_TRTfast', ctpid=-1).setLogic(thr_trt.condition(1)&Logic(bgrp0)).setTriggerType(TT_minb)

        item_zdcA_empty    = LVL1MenuItem('L1_ZDC_A_EMPTY',   ctpid=-1).setLogic(thr_zdcA.condition(1)&cosmiccond).setTriggerType(TT_minb)
        item_zdcC_empty    = LVL1MenuItem('L1_ZDC_C_EMPTY',   ctpid=-1).setLogic(thr_zdcC.condition(1)&cosmiccond).setTriggerType(TT_minb)
        item_zdcAND_empty  = LVL1MenuItem('L1_ZDC_AND_EMPTY', ctpid=-1).setLogic(thr_zdcAND.condition(1)&cosmiccond).setTriggerType(TT_minb)
        item_zdc_empty     = LVL1MenuItem('L1_ZDC_EMPTY',     ctpid=-1).setLogic((thr_zdcA.condition(1)|thr_zdcC.condition(1))&cosmiccond).setTriggerType(TT_minb)
        item_zdc_unpaired1    = LVL1MenuItem('L1_ZDC_UNPAIRED1', ctpid=-1).setLogic((thr_zdcA.condition(1)|thr_zdcC.condition(1))&unpaired1cond).setTriggerType(TT_minb)
        item_zdc_unpaired2    = LVL1MenuItem('L1_ZDC_UNPAIRED2', ctpid=-1).setLogic((thr_zdcA.condition(1)|thr_zdcC.condition(1))&unpaired2cond).setTriggerType(TT_minb)
        item_zdc_unpairedR    = LVL1MenuItem('L1_ZDC_UNPAIRED',  ctpid=-1).setLogic((thr_zdcA.condition(1)|thr_zdcC.condition(1))&unpairedRcond).setTriggerType(TT_minb)                
        item_zdc_unpaired_iso = LVL1MenuItem('L1_ZDC_UNPAIRED_ISO',ctpid=-1).setLogic((thr_zdcA.condition(1)|thr_zdcC.condition(1))&unpaired_isocond).setTriggerType(TT_minb)
        item_zdc_unpaired_noniso = LVL1MenuItem('L1_ZDC_UNPAIRED_NONISO',ctpid=-1).setLogic((thr_zdcA.condition(1)|thr_zdcC.condition(1))&unpaired_nonisocond).setTriggerType(TT_minb)                
        item_zdcA_C_empty     = LVL1MenuItem('L1_ZDC_A_C_EMPTY', ctpid=-1).setLogic(thr_zdcA.condition(1)&thr_zdcC.condition(1)&cosmiccond).setTriggerType(TT_minb)
        item_zdcA_C_unpaired_iso     = LVL1MenuItem('L1_ZDC_A_C_UNPAIRED_ISO', ctpid=-1).setLogic(thr_zdcA.condition(1)&thr_zdcC.condition(1)&unpaired_isocond).setTriggerType(TT_minb)
        item_zdcAnd_unpaired_iso     = LVL1MenuItem('L1_ZDC_AND_UNPAIRED_ISO', ctpid=-1).setLogic(thr_zdcAND.condition(1)&unpaired_isocond).setTriggerType(TT_minb)
        item_zdcAnd_unpaired_noniso     = LVL1MenuItem('L1_ZDC_AND_UNPAIRED_NONISO', ctpid=-1).setLogic(thr_zdcAND.condition(1)&unpaired_nonisocond).setTriggerType(TT_minb)
        item_zdcA_C_NoCol     = LVL1MenuItem('L1_ZDC_A_C_NoCol', ctpid=-1).setLogic(thr_zdcA.condition(1)&thr_zdcC.condition(1)&NoColCond).setTriggerType(TT_minb)
        item_zdcA_C_unpaired_noniso     = LVL1MenuItem('L1_ZDC_A_C_UNPAIRED_NONISO', ctpid=-1).setLogic(thr_zdcA.condition(1)&thr_zdcC.condition(1)&unpaired_nonisocond).setTriggerType(TT_minb)
        item_zdcA = LVL1MenuItem('L1_ZDC_A', ctpid=-1).setLogic(thr_zdcA.condition(1)&physcond).setTriggerType(TT_minb)
        item_zdcC = LVL1MenuItem('L1_ZDC_C', ctpid=-1).setLogic(thr_zdcC.condition(1)&physcond).setTriggerType(TT_minb)
        item_zdcAND = LVL1MenuItem('L1_ZDC_AND', ctpid=-1).setLogic(thr_zdcAND.condition(1)&physcond).setTriggerType(TT_minb)

        item_zdcAND_veto_te50 = LVL1MenuItem('L1_ZDC_AND_VTE50', ctpid=-1).setLogic(thr_zdcAND.condition(1)\
                                                                                    &Not(thr_te50.condition(1))&physcond).setTriggerType(TT_minb)
        item_zdcA_C_veto_te50 = LVL1MenuItem('L1_ZDC_A_C_VTE50', ctpid=-1).setLogic(thr_zdcA.condition(1)&thr_zdcC.condition(1)\
                                                                                    &Not(thr_te50.condition(1))&physcond).setTriggerType(TT_minb)
        item_zdcA_C_te50 = LVL1MenuItem('L1_ZDC_A_C_TE50', ctpid=-1).setLogic(thr_zdcA.condition(1)&thr_zdcC.condition(1)\
                                                                                    &thr_te50.condition(1)&physcond).setTriggerType(TT_minb)
        item_zdcA_veto_te20 = LVL1MenuItem('L1_ZDC_A_VTE20', ctpid=-1).setLogic(thr_zdcA.condition(1)\
                                                                                &Not(thr_te20.condition(1))&physcond).setTriggerType(TT_minb)
        item_zdcC_veto_te20 = LVL1MenuItem('L1_ZDC_C_VTE20', ctpid=-1).setLogic(thr_zdcC.condition(1)\
                                                                                &Not(thr_te20.condition(1))&physcond).setTriggerType(TT_minb)

        item_zdc = LVL1MenuItem('L1_ZDC', ctpid=-1).setLogic((thr_zdcA.condition(1)|thr_zdcC.condition(1))&physcond).setTriggerType(TT_minb)
        item_zdcA_C = LVL1MenuItem('L1_ZDC_A_C', ctpid=-1).setLogic(thr_zdcA.condition(1)&thr_zdcC.condition(1)&physcond).setTriggerType(TT_minb)
        item_zdcA_C_bgrp7 = LVL1MenuItem('L1_ZDC_A_C_BGRP7', ctpid=-1).setLogic(thr_zdcA.condition(1)&thr_zdcC.condition(1)&bgrp7cond).setTriggerType(TT_minb)
        item_zdcA_bgrp7 = LVL1MenuItem('L1_ZDC_A_BGRP7', ctpid=-1).setLogic(thr_zdcA.condition(1)&bgrp7cond).setTriggerType(TT_minb)
        item_zdcC_bgrp7 = LVL1MenuItem('L1_ZDC_C_BGRP7', ctpid=-1).setLogic(thr_zdcC.condition(1)&bgrp7cond).setTriggerType(TT_minb)
        item_zdcA_C_overlay = LVL1MenuItem('L1_ZDC_A_C_OVERLAY', ctpid=-1).setLogic(thr_zdcA.condition(1)&thr_zdcC.condition(1)&physcond).setTriggerType(TT_zerobs)

	
        item_zdc_mbts1 = LVL1MenuItem('L1_ZDC_MBTS_1', ctpid=-1)\
                         .setLogic((thr_zdcA.condition(1)|thr_zdcC.condition(1))&\
                                   (thr_mbtsA.condition(1)|thr_mbtsC.condition(1))&physcond)\
                                   .setTriggerType(TT_minb)
        #                     .setLogic(Logic(bgrp0)&Logic(bgrp1)&(thr_mbtsA.condition(1)|thr_mbtsC.condition(1)))\

        item_zdc_mbts2 = LVL1MenuItem('L1_ZDC_MBTS_2', ctpid=-1)\
                         .setLogic((thr_zdcA.condition(1)|thr_zdcC.condition(1))&\
                                   (thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))\
                                   &(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1))&physcond)\
                                   .setTriggerType(TT_minb)

        item_zdc_mbts1_1 = LVL1MenuItem('L1_ZDC_MBTS_1_1', ctpid=-1)\
                           .setLogic((thr_zdcA.condition(1)|thr_zdcC.condition(1))&\
                                     (thr_mbtsA.condition(1)&thr_mbtsC.condition(1))&physcond)\
                                     .setTriggerType(TT_minb)
        
        item_zdc_mbts1_2 = LVL1MenuItem('L1_ZDC_MBTS_2_2', ctpid=-1)\
                           .setLogic((thr_zdcA.condition(1)|thr_zdcC.condition(1))&\
                                     (thr_mbtsA.condition(2)&thr_mbtsC.condition(2))&physcond)\
                                     .setTriggerType(TT_minb)
                                   
        item_bcmAtoC = LVL1MenuItem('L1_BCM_AtoC', ctpid=0xe5).setLogic(thr_bcmAtoC.condition(1)&physcond).setTriggerType(TT_minb)
        item_bcmCtoA = LVL1MenuItem('L1_BCM_CtoA', ctpid=0xe6).setLogic(thr_bcmCtoA.condition(1)&physcond).setTriggerType(TT_minb)
        item_bcmAC_CA_bgrp0 = LVL1MenuItem('L1_BCM_AC_CA_BGRP0', ctpid=0xe6).setLogic((thr_bcmAtoC.condition(1)|thr_bcmCtoA.condition(1))&Logic(bgrp0)).setTriggerType(TT_minb)

        item_bcmAC_CA_unpaired_iso = LVL1MenuItem('L1_BCM_AC_CA_UNPAIRED_ISO', ctpid=-1).setLogic((thr_bcmAtoC.condition(1)|thr_bcmCtoA.condition(1))&unpaired_isocond).setTriggerType(TT_minb)
        item_bcmAC_CA_unpaired_noniso = LVL1MenuItem('L1_BCM_AC_CA_UNPAIRED_NONISO', ctpid=-1).setLogic((thr_bcmAtoC.condition(1)|thr_bcmCtoA.condition(1))&unpaired_nonisocond).setTriggerType(TT_minb)

        item_bcm1 = LVL1MenuItem('L1_BCM_MUL_1', ctpid=0xe7).setLogic(thr_bcmComb.condition(1)&physcond).setTriggerType(TT_minb)
        item_bcm2 = LVL1MenuItem('L1_BCM_MUL_2', ctpid=0xe9).setLogic(thr_bcmComb.condition(2)&physcond).setTriggerType(TT_minb)
        item_bcm3 = LVL1MenuItem('L1_BCM_MUL_3', ctpid=0xea).setLogic(thr_bcmComb.condition(3)&physcond).setTriggerType(TT_minb)
        item_bcm4 = LVL1MenuItem('L1_BCM_MUL_4', ctpid=0xeb).setLogic(thr_bcmComb.condition(4)&physcond).setTriggerType(TT_minb)
        item_bcm5 = LVL1MenuItem('L1_BCM_MUL_5', ctpid=0xf6).setLogic(thr_bcmComb.condition(5)&physcond).setTriggerType(TT_minb)
        item_bcm6 = LVL1MenuItem('L1_BCM_MUL_6', ctpid=0xf7).setLogic(thr_bcmComb.condition(6)&physcond).setTriggerType(TT_minb)
        item_bcm7 = LVL1MenuItem('L1_BCM_MUL_7', ctpid=0xf8).setLogic(thr_bcmComb.condition(7)&physcond).setTriggerType(TT_minb)
        
        item_bcmHT = LVL1MenuItem('L1_BCM_HT', ctpid=0xf8).setLogic(thr_bcmComb.condition(7)&physcond).setTriggerType(TT_minb)
        item_bcmHT_bgrp0 = LVL1MenuItem('L1_BCM_HT_BGRP0', ctpid=0xf8).setLogic(thr_bcmComb.condition(7)&Logic(bgrp0)).setTriggerType(TT_minb)

        item_bcmWide = LVL1MenuItem('L1_BCM_Wide', ctpid=-1).setLogic(thr_bcmWide.condition(1)&physcond).setTriggerType(TT_minb)
        item_bcmWide_NoCol = LVL1MenuItem('L1_BCM_Wide_NoCol', ctpid=-1).setLogic(thr_bcmWide.condition(1)&NoColCond).setTriggerType(TT_minb)
        item_bcmWide_bgrp0 = LVL1MenuItem('L1_BCM_Wide_BGRP0', ctpid=0xe8).setLogic(thr_bcmWide.condition(1)&Logic(bgrp0)).setTriggerType(TT_minb)


        item_bcmAtoC_EMPTY = LVL1MenuItem('L1_BCM_AtoC_EMPTY', ctpid=-1).setLogic(thr_bcmAtoC.condition(1)&cosmiccond).setTriggerType(TT_minb)
        item_bcmCtoA_EMPTY = LVL1MenuItem('L1_BCM_CtoA_EMPTY', ctpid=-1).setLogic(thr_bcmCtoA.condition(1)&cosmiccond).setTriggerType(TT_minb)
        
        item_bcm1_EMPTY = LVL1MenuItem('L1_BCM_MUL_1_EMPTY', ctpid=-1).setLogic(thr_bcmComb.condition(1)&cosmiccond).setTriggerType(TT_minb)
        item_bcm2_EMPTY = LVL1MenuItem('L1_BCM_MUL_2_EMPTY', ctpid=-1).setLogic(thr_bcmComb.condition(2)&cosmiccond).setTriggerType(TT_minb)
        item_bcm3_EMPTY = LVL1MenuItem('L1_BCM_MUL_3_EMPTY', ctpid=-1).setLogic(thr_bcmComb.condition(3)&cosmiccond).setTriggerType(TT_minb)
        item_bcm4_EMPTY = LVL1MenuItem('L1_BCM_MUL_4_EMPTY', ctpid=-1).setLogic(thr_bcmComb.condition(4)&cosmiccond).setTriggerType(TT_minb)
        item_bcm5_EMPTY = LVL1MenuItem('L1_BCM_MUL_5_EMPTY', ctpid=-1).setLogic(thr_bcmComb.condition(5)&cosmiccond).setTriggerType(TT_minb)
        item_bcm6_EMPTY = LVL1MenuItem('L1_BCM_MUL_6_EMPTY', ctpid=-1).setLogic(thr_bcmComb.condition(6)&cosmiccond).setTriggerType(TT_minb)
        item_bcm7_EMPTY = LVL1MenuItem('L1_BCM_MUL_7_EMPTY', ctpid=-1).setLogic(thr_bcmComb.condition(7)&cosmiccond).setTriggerType(TT_minb)
        item_bcmHT_EMPTY = LVL1MenuItem('L1_BCM_HT_EMPTY', ctpid=-1).setLogic(thr_bcmComb.condition(7)&cosmiccond).setTriggerType(TT_minb)
        item_bcmWide_EMPTY        = LVL1MenuItem('L1_BCM_Wide_EMPTY', ctpid=-1).setLogic(thr_bcmWide.condition(1)&cosmiccond).setTriggerType(TT_minb)
        item_bcmWide_unpaired_iso     = LVL1MenuItem('L1_BCM_Wide_UNPAIRED_ISO', ctpid=-1).setLogic(thr_bcmWide.condition(1)&unpaired_isocond).setTriggerType(TT_minb)
        item_bcmWide_unpaired_noniso = LVL1MenuItem('L1_BCM_Wide_UNPAIRED_NONISO', ctpid=-1).setLogic(thr_bcmWide.condition(1)&unpaired_nonisocond).setTriggerType(TT_minb)


        
        item_bcmIdeal = LVL1MenuItem('L1_BCM_Ideal', ctpid=-1)\
                        .setLogic((thr_bcmA1.condition(1)|thr_bcmA2.condition(1))&\
                                  (thr_bcmC1.condition(1)|thr_bcmC2.condition(1))&\
                                  physcond)\
                                  .setTriggerType(TT_minb)

        item_bcmAtoC_bptx = LVL1MenuItem('L1_BCM_AtoC_BPTX', ctpid=0xdc).setLogic(thr_bcmAtoC.condition(1)&Logic(bgrp0)&(thr_bptx0.condition(1)|thr_bptx1.condition(1)))\
                            .setTriggerType(TT_minb)
        item_bcmCtoA_bptx = LVL1MenuItem('L1_BCM_CtoA_BPTX', ctpid=0xdd).setLogic(thr_bcmCtoA.condition(1)&Logic(bgrp0)&(thr_bptx0.condition(1)|thr_bptx1.condition(1)))\
                            .setTriggerType(TT_minb)
        item_bcmWide_bptx = LVL1MenuItem('L1_BCM_Wide_BPTX', ctpid=0xde).setLogic(thr_bcmWide.condition(1)&Logic(bgrp0)&(thr_bptx0.condition(1)|thr_bptx1.condition(1)))\
                            .setTriggerType(TT_minb)
        item_bcm1_bptx = LVL1MenuItem('L1_BCM_MUL_1_BPTX', ctpid=0xdf).setLogic(thr_bcmComb.condition(1)&Logic(bgrp0)&(thr_bptx0.condition(1)|thr_bptx1.condition(1)))\
                         .setTriggerType(TT_minb)
        
        item_lucidA =  LVL1MenuItem('L1_LUCID_A', ctpid=0xec).setLogic(thr_lucidA.condition(1)&physcond).setTriggerType(TT_minb)
        item_lucidC =  LVL1MenuItem('L1_LUCID_C', ctpid=0xed).setLogic(thr_lucidC.condition(1)&physcond).setTriggerType(TT_minb)
        item_lucidAC =  LVL1MenuItem('L1_LUCID_A_C', ctpid=0xee).setLogic(thr_lucidA.condition(1)&thr_lucidC.condition(1)&physcond).setTriggerType(TT_minb)
        item_lucidAC_NoCol =  LVL1MenuItem('L1_LUCID_A_C_NoCol', ctpid=0xee).setLogic(thr_lucidA.condition(1)&thr_lucidC.condition(1)&NoColCond).setTriggerType(TT_minb)
        item_lucid =  LVL1MenuItem('L1_LUCID', ctpid=0xf5).setLogic((thr_lucidA.condition(1)|thr_lucidC.condition(1))&physcond).setTriggerType(TT_minb)
        item_lucid_bgpr7 =  LVL1MenuItem('L1_LUCID_BGRP7', ctpid=0xf5).setLogic((thr_lucidA.condition(1)|thr_lucidC.condition(1))&bgrp7cond).setTriggerType(TT_minb)
        item_lucid_comm =  LVL1MenuItem('L1_LUCID_COMM', ctpid=-1).setLogic(thr_nimdir5.condition(1)&physcond).setTriggerType(TT_minb)
        item_lucidA_bptx =  LVL1MenuItem('L1_LUCID_A_BPTX', ctpid=0xd0)\
                           .setLogic(thr_lucidA.condition(1)&Logic(bgrp0)&(thr_bptx0.condition(1)|thr_bptx1.condition(1)))\
                           .setTriggerType(TT_minb)
        item_lucidC_bptx =  LVL1MenuItem('L1_LUCID_C_BPTX', ctpid=0xd1)\
                           .setLogic(thr_lucidC.condition(1)&Logic(bgrp0)&(thr_bptx0.condition(1)|thr_bptx1.condition(1)))\
                           .setTriggerType(TT_minb)
        item_lucidAC_bptx =  LVL1MenuItem('L1_LUCID_A_C_BPTX', ctpid=0xd3)\
                            .setLogic(thr_lucidA.condition(1)&thr_lucidC.condition(1)&Logic(bgrp0)&(thr_bptx0.condition(1)|thr_bptx1.condition(1)))\
                            .setTriggerType(TT_minb)
        item_lucid_bptx =  LVL1MenuItem('L1_LUCID_BPTX', ctpid=0xd4)\
                          .setLogic((thr_lucidA.condition(1)|thr_lucidC.condition(1))&Logic(bgrp0)&(thr_bptx0.condition(1)|thr_bptx1.condition(1)))\
                          .setTriggerType(TT_minb)

        item_lucidA_empty     =  LVL1MenuItem('L1_LUCID_A_EMPTY',    ctpid=-1).setLogic(thr_lucidA.condition(1)&cosmiccond).setTriggerType(TT_minb)
        item_lucidC_empty     =  LVL1MenuItem('L1_LUCID_C_EMPTY',    ctpid=-1).setLogic(thr_lucidC.condition(1)&cosmiccond).setTriggerType(TT_minb)
        item_lucidAC_empty    =  LVL1MenuItem('L1_LUCID_A_C_EMPTY',  ctpid=-1).setLogic(thr_lucidA.condition(1)&thr_lucidC.condition(1)&cosmiccond).setTriggerType(TT_minb)
        item_lucid_empty      =  LVL1MenuItem('L1_LUCID_EMPTY',      ctpid=-1).setLogic((thr_lucidA.condition(1)|thr_lucidC.condition(1))&cosmiccond).setTriggerType(TT_minb)
        item_lucid_unpaired_iso      =  LVL1MenuItem('L1_LUCID_UNPAIRED_ISO',      ctpid=-1).setLogic((thr_lucidA.condition(1)|thr_lucidC.condition(1))&unpaired_isocond).setTriggerType(TT_minb)
        item_lucid_unpaired_noniso      =  LVL1MenuItem('L1_LUCID_UNPAIRED_NONISO',      ctpid=-1).setLogic((thr_lucidA.condition(1)|thr_lucidC.condition(1))&unpaired_nonisocond).setTriggerType(TT_minb)
        item_lucid_comm_empty =  LVL1MenuItem('L1_LUCID_COMM_EMPTY', ctpid=-1).setLogic(thr_nimdir5.condition(1)&cosmiccond).setTriggerType(TT_minb)
        item_lucidA_unpaired1 =  LVL1MenuItem('L1_LUCID_A_UNPAIRED1',  ctpid=-1).setLogic(thr_lucidA.condition(1)&unpaired1cond).setTriggerType(TT_minb)
        item_lucidA_unpaired2 =  LVL1MenuItem('L1_LUCID_A_UNPAIRED2',  ctpid=-1).setLogic(thr_lucidA.condition(1)&unpaired2cond).setTriggerType(TT_minb)
        item_lucidA_unpairedR =  LVL1MenuItem('L1_LUCID_A_UNPAIRED',   ctpid=-1).setLogic(thr_lucidA.condition(1)&unpairedRcond).setTriggerType(TT_minb)        
        item_lucid_comm_unpaired_iso =  LVL1MenuItem('L1_LUCID_COMM_UNPAIRED_ISO',   ctpid=-1).setLogic(thr_nimdir5.condition(1)&unpaired_isocond).setTriggerType(TT_minb)        
        item_lucidA_unpaired_iso =  LVL1MenuItem('L1_LUCID_A_UNPAIRED_ISO',   ctpid=-1).setLogic(thr_lucidA.condition(1)&unpaired_isocond).setTriggerType(TT_minb)        
        item_lucidC_unpaired1 =  LVL1MenuItem('L1_LUCID_C_UNPAIRED1',  ctpid=-1).setLogic(thr_lucidC.condition(1)&unpaired1cond).setTriggerType(TT_minb)
        item_lucidC_unpaired2 =  LVL1MenuItem('L1_LUCID_C_UNPAIRED2',  ctpid=-1).setLogic(thr_lucidC.condition(1)&unpaired2cond).setTriggerType(TT_minb)
        item_lucidC_unpairedR =  LVL1MenuItem('L1_LUCID_C_UNPAIRED',   ctpid=-1).setLogic(thr_lucidC.condition(1)&unpairedRcond).setTriggerType(TT_minb)        
        item_lucidC_unpaired_iso =  LVL1MenuItem('L1_LUCID_C_UNPAIRED_ISO',   ctpid=-1).setLogic(thr_lucidC.condition(1)&unpaired_isocond).setTriggerType(TT_minb)        
        
        item_lucidAC_unpaired1 =  LVL1MenuItem('L1_LUCID_A_C_UNPAIRED1', ctpid=-1).setLogic(thr_lucidA.condition(1)&thr_lucidC.condition(1)&unpaired1cond).setTriggerType(TT_minb)
        item_lucidAC_unpaired2 =  LVL1MenuItem('L1_LUCID_A_C_UNPAIRED2', ctpid=-1).setLogic(thr_lucidA.condition(1)&thr_lucidC.condition(1)&unpaired2cond).setTriggerType(TT_minb)
        item_lucidAC_unpairedR =  LVL1MenuItem('L1_LUCID_A_C_UNPAIRED',  ctpid=-1).setLogic(thr_lucidA.condition(1)&thr_lucidC.condition(1)&unpairedRcond).setTriggerType(TT_minb)
        item_lucidAC_unpaired_iso =  LVL1MenuItem('L1_LUCID_A_C_UNPAIRED_ISO',  ctpid=-1).setLogic(thr_lucidA.condition(1)&thr_lucidC.condition(1)&unpaired_isocond).setTriggerType(TT_minb)
        item_lucidAC_unpaired_noniso =  LVL1MenuItem('L1_LUCID_A_C_UNPAIRED_NONISO',  ctpid=-1).setLogic(thr_lucidA.condition(1)&thr_lucidC.condition(1)&unpaired_nonisocond).setTriggerType(TT_minb)

        item_lucidA_filled  =  LVL1MenuItem('L1_LUCID_A_FILLED',  ctpid=-1).setLogic(thr_lucidA.condition(1)&physcond).setTriggerType(TT_minb)
        item_lucidC_filled  =  LVL1MenuItem('L1_LUCID_C_FILLED',   ctpid=-1).setLogic(thr_lucidC.condition(1)&physcond).setTriggerType(TT_minb)
        item_lucidAC_filled =  LVL1MenuItem('L1_LUCID_A_C_FILLED', ctpid=-1).setLogic(thr_lucidA.condition(1)&thr_lucidC.condition(1)&physcond).setTriggerType(TT_minb)
        item_lucid_filled   =  LVL1MenuItem('L1_LUCID_FILLED',     ctpid=-1).setLogic((thr_lucidA.condition(1)|thr_lucidC.condition(1))&physcond).setTriggerType(TT_minb)


        item_rndm0 = LVL1MenuItem('L1_RD0', ctpid=0xef).setLogic(Logic(rndm0)&Logic(bgrp0)).setTriggerType(TT_rand)
        item_rndm1 = LVL1MenuItem('L1_RD1', ctpid=0xf0).setLogic(Logic(rndm1)&Logic(bgrp0)).setTriggerType(TT_rand)
        
        item_rndm0_filled       = LVL1MenuItem('L1_RD0_FILLED',       ctpid=-1).setLogic(Logic(rndm0)&physcond).setTriggerType(TT_rand)
        item_rndm0_empty        = LVL1MenuItem('L1_RD0_EMPTY',        ctpid=-1).setLogic(Logic(rndm0)&cosmiccond).setTriggerType(TT_rand)
        item_rndm0_unpaired1    = LVL1MenuItem('L1_RD0_UNPAIRED1',    ctpid=-1).setLogic(Logic(rndm0)&unpaired1cond).setTriggerType(TT_rand)
        item_rndm0_unpaired2    = LVL1MenuItem('L1_RD0_UNPAIRED2',    ctpid=-1).setLogic(Logic(rndm0)&unpaired2cond).setTriggerType(TT_rand)
        item_rndm0_unpairedR    = LVL1MenuItem('L1_RD0_UNPAIRED',     ctpid=-1).setLogic(Logic(rndm0)&unpairedRcond).setTriggerType(TT_rand)
        item_rndm0_1stempty     = LVL1MenuItem('L1_RD0_FIRSTEMPTY',   ctpid=-1).setLogic(Logic(rndm0)&empty1stcond).setTriggerType(TT_rand)
        item_rndm1_filled       = LVL1MenuItem('L1_RD1_FILLED',       ctpid=-1).setLogic(Logic(rndm1)&physcond).setTriggerType(TT_rand)
        item_rndm1_empty        = LVL1MenuItem('L1_RD1_EMPTY',        ctpid=-1).setLogic(Logic(rndm1)&cosmiccond).setTriggerType(TT_zerobs)
        item_rndm0_unpaired_iso = LVL1MenuItem('L1_RD0_UNPAIRED_ISO', ctpid=-1).setLogic(Logic(rndm0)&unpaired_isocond).setTriggerType(TT_rand)
        item_rndm0_unpaired_noniso = LVL1MenuItem('L1_RD0_UNPAIRED_NONISO', ctpid=-1).setLogic(Logic(rndm0)&unpaired_nonisocond).setTriggerType(TT_rand)
        
        item_rndm0_bgrp0_bgrp1 = LVL1MenuItem('L1_RD0_BGRP1', ctpid=-1).setLogic(Logic(rndm0)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_rand)        
        item_rndm1_bgrp0_bgrp3 = LVL1MenuItem('L1_RD1_NOISE', ctpid=-1).setLogic(Logic(rndm1)&Logic(bgrp0)&Logic(bgrp3)).setTriggerType(TT_rand)
        item_rndm1_bgrp0_bgrp4 = LVL1MenuItem('L1_RD1_BGRP4', ctpid=-1).setLogic(Logic(rndm1)&Logic(bgrp0)&(Logic(bgrp4))).setTriggerType(TT_rand)
        item_rndm1_bgrp0_bgrp5 = LVL1MenuItem('L1_RD1_BGRP5', ctpid=-1).setLogic(Logic(rndm1)&Logic(bgrp0)&(Logic(bgrp5))).setTriggerType(TT_rand)
        item_rndm1_bgrp0_bgrp6 = LVL1MenuItem('L1_RD1_HIST', ctpid=-1).setLogic(Logic(rndm1)&Logic(bgrp0)&Logic(bgrp6)).setTriggerType(TT_rand)
        item_rndm1_brp0        = LVL1MenuItem('L1_RD1_BGRP0', ctpid=-1).setLogic(Logic(rndm1)&Logic(bgrp0)).setTriggerType(TT_rand)

        # Bunch groups are considered transient
        item_bgrp0 = LVL1MenuItem('L1_BGRP0', ctpid=-1, group='1', prescale=1)\
                     .setLogic(Logic(bgrp0)).setTriggerType(TT_rand)
        item_bgrp1 = LVL1MenuItem('L1_BGRP1', ctpid=-1, group='1', prescale=1)\
                     .setLogic(Logic(bgrp1)).setTriggerType(TT_rand)
        item_bgrp2 = LVL1MenuItem('L1_BGRP2', ctpid=-1, group='1', prescale=1)\
                     .setLogic(Logic(bgrp2)).setTriggerType(TT_rand)
        item_bgrp3 = LVL1MenuItem('L1_BGRP3', ctpid=-1, group='1', prescale=1)\
                     .setLogic(Logic(bgrp3)).setTriggerType(TT_rand)
        item_bgrp4 = LVL1MenuItem('L1_BGRP4', ctpid=-1, group='1', prescale=1)\
                     .setLogic(Logic(bgrp4)).setTriggerType(TT_rand)
        item_bgrp5 = LVL1MenuItem('L1_BGRP5', ctpid=-1, group='1', prescale=1)\
                     .setLogic(Logic(bgrp5)).setTriggerType(TT_rand)
        item_bgrp6 = LVL1MenuItem('L1_BGRP6', ctpid=-1, group='1', prescale=1)\
                     .setLogic(Logic(bgrp6)).setTriggerType(TT_rand)
        item_bgrp7 = LVL1MenuItem('L1_BGRP7', ctpid=-1, group='1', prescale=1)\
                     .setLogic(Logic(bgrp7)).setTriggerType(TT_rand)

        item_bgrp1_alfa = LVL1MenuItem('L1_BGRP1_ALFA_BGT', ctpid=-1, group='1', prescale=1)\
                          .setLogic(Logic(bgrp1)).setTriggerType(TT_rand)
        item_bgrp4_alfa = LVL1MenuItem('L1_BGRP4_ALFA_BGT', ctpid=-1, group='1', prescale=1)\
                          .setLogic(Logic(bgrp4)).setTriggerType(TT_rand)
        item_bgrp7_alfa = LVL1MenuItem('L1_BGRP7_ALFA_BGT', ctpid=-1, group='1', prescale=1)\
                          .setLogic(Logic(bgrp7)).setTriggerType(TT_rand)

        item_bptx0_0 = LVL1MenuItem('L1_BPTX0_0', ctpid=0xf1, group='1', prescale=1)\
                       .setLogic(thr_bptx0.condition(1)&Logic(bgrp0)).setTriggerType(TT_bptx)
        item_bptx1_0 = LVL1MenuItem('L1_BPTX1_0', ctpid=0xf2, group='1', prescale=1)\
                       .setLogic(thr_bptx1.condition(1)&Logic(bgrp0)).setTriggerType(TT_bptx)
        item_bptx0_0_1 = LVL1MenuItem('L1_BPTX0_0_1', ctpid=0xf3, group='1', prescale=1)\
                         .setLogic(thr_bptx0.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_bptx)
        item_bptx1_0_1 = LVL1MenuItem('L1_BPTX1_0_1', ctpid=0xf4, group='1', prescale=1)\
                         .setLogic(thr_bptx0.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_bptx)
    
        item_bptx0_bgrp0 = LVL1MenuItem('L1_BPTX0_BGRP0', ctpid=0xf1, group='1', prescale=1)\
                           .setLogic(thr_bptx0.condition(1)&Logic(bgrp0)).setTriggerType(TT_rand)
        item_bptx1_bgrp0 = LVL1MenuItem('L1_BPTX1_BGRP0', ctpid=0xf2, group='1', prescale=1)\
                           .setLogic(thr_bptx1.condition(1)&Logic(bgrp0)).setTriggerType(TT_rand)
        item_bptx0_bptx1 = LVL1MenuItem('L1_BPTX0_BPTX1_BGRP0', ctpid=-1, group='1', prescale=1)\
                           .setLogic(thr_bptx0.condition(1)&thr_bptx1.condition(1)&Logic(bgrp0)).setTriggerType(TT_rand)
        
        item_bptx0_bgrp0_bgrp1 = LVL1MenuItem('L1_BPTX0_BGRP0_BGRP1', ctpid=0xf3, group='1', prescale=1)\
                                 .setLogic(thr_bptx0.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_bptx)
        item_bptx1_bgrp0_bgrp1 = LVL1MenuItem('L1_BPTX1_BGRP0_BGRP1', ctpid=0xf4, group='1', prescale=1)\
                                 .setLogic(thr_bptx1.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_bptx)


        

        # Direct inputs [0x50:0x5f]
        item_nimdir2 = LVL1MenuItem('L1_NIMDIR2', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir2.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir3 = LVL1MenuItem('L1_NIMDIR3', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir3.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir4 = LVL1MenuItem('L1_NIMDIR4', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir4.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir5 = LVL1MenuItem('L1_NIMDIR5', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir5.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir6 = LVL1MenuItem('L1_NIMDIR6', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir6.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir7 = LVL1MenuItem('L1_NIMDIR7', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir7.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir8 = LVL1MenuItem('L1_NIMDIR8', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir8.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir9 = LVL1MenuItem('L1_NIMDIR9', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir9.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir10 = LVL1MenuItem('L1_NIMDIR10', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir10.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir11 = LVL1MenuItem('L1_NIMDIR11', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir11.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir12 = LVL1MenuItem('L1_NIMDIR12', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir12.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir13 = LVL1MenuItem('L1_NIMDIR13', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir13.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_rpcout)
        item_nimdir14 = LVL1MenuItem('L1_NIMDIR14', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir14.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir15 = LVL1MenuItem('L1_NIMDIR15', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir15.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir16 = LVL1MenuItem('L1_NIMDIR16', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir16.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir17 = LVL1MenuItem('L1_NIMDIR17', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir17.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir18 = LVL1MenuItem('L1_NIMDIR18', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir18.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir19 = LVL1MenuItem('L1_NIMDIR19', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir19.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir20 = LVL1MenuItem('L1_NIMDIR20', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir20.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir21 = LVL1MenuItem('L1_NIMDIR21', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir21.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir22 = LVL1MenuItem('L1_NIMDIR22', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir22.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir23 = LVL1MenuItem('L1_NIMDIR23', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir23.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir24 = LVL1MenuItem('L1_NIMDIR24', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir24.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir25 = LVL1MenuItem('L1_NIMDIR25', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir25.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir26 = LVL1MenuItem('L1_NIMDIR26', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir26.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir27 = LVL1MenuItem('L1_NIMDIR27', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir27.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir28 = LVL1MenuItem('L1_NIMDIR28', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir28.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir29 = LVL1MenuItem('L1_NIMDIR29', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir29.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir30 = LVL1MenuItem('L1_NIMDIR30', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir30.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir31 = LVL1MenuItem('L1_NIMDIR31', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir31.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir32 = LVL1MenuItem('L1_NIMDIR32', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir32.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir33 = LVL1MenuItem('L1_NIMDIR33', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir33.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir34 = LVL1MenuItem('L1_NIMDIR34', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir34.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir35 = LVL1MenuItem('L1_NIMDIR35', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir35.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nimdir36 = LVL1MenuItem('L1_NIMDIR36', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir36.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)

        item_lhcf  = LVL1MenuItem('L1_LHCF',  ctpid=-1, group='1', prescale=1).setLogic( (thr_nimdir36.condition(1)&physcond)).setTriggerType(TT_minb)        

        item_alfa_empty = LVL1MenuItem('L1_ALFA_EMPTY', ctpid=0x4e, group='1', prescale=1).setLogic(thr_nimdir_ALFA_LOG.condition(1)&cosmiccond).setTriggerType(TT_rpcout)
        item_alfa_bgpr7 = LVL1MenuItem('L1_ALFA_BGRP7', ctpid=0x4e, group='1', prescale=1).setLogic(thr_nimdir_ALFA_LOG.condition(1)&bgrp7cond).setTriggerType(TT_rpcout)

        item_alfa_empty_od = LVL1MenuItem('L1_ALFA_EMPTY_OD', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir_ALFA_LOG_OD.condition(1)&cosmiccond).setTriggerType(TT_rpcout)
        item_alfa_bgpr7_od = LVL1MenuItem('L1_ALFA_BGRP7_OD', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir_ALFA_LOG_OD.condition(1)&bgrp7cond).setTriggerType(TT_rpcout)

        #BGRP items
        #item_alfa_bgt_bgrp1 = LVL1MenuItem('L1_ALFA_BGT_BGRP1', ctpid=-1, group='1', prescale=1).setLogic(bgrpcond).setTriggerType(TT_rand)
        #item_alfa_bgt_bgrp4 = LVL1MenuItem('L1_ALFA_BGT_BGRP4', ctpid=-1, group='1', prescale=1).setLogic(unpaired1cond).setTriggerType(TT_rand)
        #item_alfa_bgt_bgrp7 = LVL1MenuItem('L1_ALFA_BGT_BGRP7', ctpid=-1, group='1', prescale=1).setLogic(Logic(bgrp7)&Logic(unpairedRcond).setTriggerType(TT_rand)


        item_tile = LVL1MenuItem('L1_Tile', ctpid=0x4c, group='1', prescale=1).setLogic(thr_nimdir13.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_trt = LVL1MenuItem('L1_TRT', ctpid=0x4e, group='1', prescale=1).setLogic(thr_nimdir25.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)

        item_l1a_mon = LVL1MenuItem('L1_L1A_Mon', ctpid=0x4f, group='1', prescale=1).setLogic(thr_nimdir26.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        
        item_calreq0 = LVL1MenuItem('L1_CALREQ0', ctpid=0xfd, group='1', prescale=1)\
                       .setLogic(thr_nim28.condition(1)&Logic(bgrp0)&Logic(bgrp2))\
                       .setTriggerType(TT_calreq0)
        item_calreq1 = LVL1MenuItem('L1_CALREQ1', ctpid=0xfe, group='1', prescale=1)\
                       .setLogic(thr_nim29.condition(1)&Logic(bgrp0)&Logic(bgrp2))\
                       .setTriggerType(TT_calreq1)
        item_calreq2 = LVL1MenuItem('L1_CALREQ2', ctpid=0xff, group='1', prescale=1)\
                       .setLogic(thr_nim30.condition(1)&Logic(bgrp0)&Logic(bgrp2))\
                       .setTriggerType(TT_calreq2)

        item_nim_s9c0a19 = LVL1MenuItem('L1_NIM_S9C0A19', ctpid=0x50, group='1', prescale=1).setLogic(thr_nimdir13.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nim_s9c0a20 = LVL1MenuItem('L1_NIM_S9C0A20', ctpid=0x51, group='1', prescale=1).setLogic(thr_nimdir14.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nim_s9c1c19 = LVL1MenuItem('L1_NIM_S9C1C19', ctpid=0x52, group='1', prescale=1).setLogic(thr_nimdir25.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nim_s9c1c20 = LVL1MenuItem('L1_NIM_S9C1C20', ctpid=0x53, group='1', prescale=1).setLogic(thr_nimdir26.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nim_s8c2b19 = LVL1MenuItem('L1_NIM_S8C2B19', ctpid=0xf9, group='1', prescale=1).setLogic(thr_nimdir4.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nim_s8c2b20 = LVL1MenuItem('L1_NIM_S8C2B20', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir5.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nim_s8c2b21 = LVL1MenuItem('L1_NIM_S8C2B21', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir6.condition(1)&bgrpcond).setTriggerType(TT_rpcout)
        item_nim_s8c2b22 = LVL1MenuItem('L1_NIM_S8C2B22', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir7.condition(1)&bgrpcond).setTriggerType(TT_rpcout)
        item_nim_s8c2b23 = LVL1MenuItem('L1_NIM_S8C2B23', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir8.condition(1)&bgrpcond).setTriggerType(TT_rpcout)
        item_nim_s8c2b24 = LVL1MenuItem('L1_NIM_S8C2B24', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir9.condition(1)&bgrpcond).setTriggerType(TT_rpcout)
        item_nim_s8c2b25 = LVL1MenuItem('L1_NIM_S8C2B25', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir10.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nim_s8c2b26 = LVL1MenuItem('L1_NIM_S8C2B26', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir11.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nim_s8c2b27 = LVL1MenuItem('L1_NIM_S8C2B27', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir12.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nim_s9c0b21 = LVL1MenuItem('L1_NIM_S9C0B21', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir15.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nim_s9c0b22 = LVL1MenuItem('L1_NIM_S9C0B22', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir16.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nim_s9c0b23 = LVL1MenuItem('L1_NIM_S9C0B23', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir17.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)
        item_nim_s9c0b24 = LVL1MenuItem('L1_NIM_S9C0B24', ctpid=-1, group='1', prescale=1).setLogic(thr_nimdir18.condition(1)&Logic(bgrp0)).setTriggerType(TT_rpcout)

        item_mb_bv = LVL1MenuItem('L1_MB_BV',ctpid=-1, group='1', prescale=1).setLogic((thr_zdcA.condition(1)|thr_zdcC.condition(1)\
                                                                                        |thr_mbtsA.condition(1)|thr_mbtsC.condition(1)\
                                                                                        |thr_lucidA.condition(1)|thr_lucidC.condition(1))\
                                                                                       &Not(thr_bptx0.condition(1))&Not(thr_bptx1.condition(1))\
                                                                                       &Logic(bgrp0))\
                                                                                       .setTriggerType(TT_minb)
        
        item_mb_mv = LVL1MenuItem('L1_MB_MV',ctpid=-1, group='1', prescale=1).setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))\
                                                                                       &Not(thr_bptx0.condition(1))&Not(thr_bptx1.condition(1))\
                                                                                       &Logic(bgrp0))\
                                                                                       .setTriggerType(TT_minb)

        item_mb_col = LVL1MenuItem('L1_MB_Col',ctpid=-1, group='1', prescale=1).setLogic((thr_mbtsA.condition(1)|thr_mbtsC.condition(1))&thr_bptx0.condition(1)&thr_bptx1.condition(1))
        


        # Alfa items

        # L1_ELAST1 = Elastics1 B7L1U and A7L1U and A7R1L and B7R1L     not (B7L1L or A7L1L or A7R1U or B7R1U)
        item_alfa_Elastics1 = LVL1MenuItem('L1_ALFA_ELAST1', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1)&thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1)\
                        &Not(thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_B7R1U.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST2 = Elastics2 B7L1L and A7L1L and A7R1U and B7R1U     not (B7L1U or A7L1U or A7R1L or B7R1L)
        item_alfa_Elastics2 = LVL1MenuItem('L1_ALFA_ELAST2', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1)&thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1)\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST3 = Elastics3 B7L1U and A7L1U and A7R1L and B7R1L and B7L1L   not (A7L1L or A7R1U or B7R1U)
        item_alfa_Elastics3 = LVL1MenuItem('L1_ALFA_ELAST3', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1)&thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1)&thr_nimdir_B7L1L.condition(1)\
                        &Not(thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_B7R1U.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST4 = Elastics4 B7L1U and A7L1U and A7R1L and B7R1L and A7L1L   not (B7L1L or A7R1U or B7R1U)
        item_alfa_Elastics4 = LVL1MenuItem('L1_ALFA_ELAST4', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1)&thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1)&thr_nimdir_A7L1L.condition(1)\
                        &Not(thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_B7R1U.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST5 = Elastics5 B7L1U and A7L1U and A7R1L and B7R1L and A7R1U   not (B7L1L or A7L1L or B7R1U)
        item_alfa_Elastics5 = LVL1MenuItem('L1_ALFA_ELAST5', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1)&thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1)&thr_nimdir_A7R1U.condition(1)\
                        &Not(thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_B7R1U.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST6 = Elastics6 B7L1U and A7L1U and A7R1L and B7R1L and B7R1U   not (B7L1L or A7L1L or A7R1U)
        item_alfa_Elastics6 = LVL1MenuItem('L1_ALFA_ELAST6', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1)&thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1)&thr_nimdir_B7R1U.condition(1)\
                        &Not(thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST7 = Elastics7 B7L1L and A7L1L and A7R1U and B7R1U and B7L1U   not (A7L1U or A7R1L or B7R1L)
        item_alfa_Elastics7 = LVL1MenuItem('L1_ALFA_ELAST7', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1)&thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1)&thr_nimdir_B7L1U.condition(1)\
                        &Not(thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST8 = Elastics8 B7L1L and A7L1L and A7R1U and B7R1U and A7L1U   not (B7L1U or A7R1L or B7R1L)
        item_alfa_Elastics8 = LVL1MenuItem('L1_ALFA_ELAST8', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1)&thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1)&thr_nimdir_A7L1U.condition(1)\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST9 = Elastics9 B7L1L and A7L1L and A7R1U and B7R1U and A7R1L   not (B7L1U or A7L1U or B7R1L)
        item_alfa_Elastics9 = LVL1MenuItem('L1_ALFA_ELAST9', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1)&thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1)&thr_nimdir_A7R1L.condition(1)\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)

        
        # L1_ELAST10 = Elastics10       B7L1L and A7L1L and A7R1U and B7R1U and B7R1L   not (B7L1U or A7L1U or A7R1L)
        item_alfa_Elastics10 = LVL1MenuItem('L1_ALFA_ELAST10', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1)&thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1)&thr_nimdir_B7R1L.condition(1)\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST11 = Elastics11       B7L1U and A7L1U and A7R1L and B7R1L
        item_alfa_Elastics11 = LVL1MenuItem('L1_ALFA_ELAST11', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1)&thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST12 = Elastics12       B7L1L and A7L1L and A7R1U and B7R1U
        item_alfa_Elastics12 = LVL1MenuItem('L1_ALFA_ELAST12', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1)&thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)

        
        # L1_ELAST13 = Elastics13       (B7L1U or A7L1U) and (A7R1L and B7R1L)
        item_alfa_Elastics13 = LVL1MenuItem('L1_ALFA_ELAST13', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1U.condition(1)|thr_nimdir_A7L1U.condition(1))&(thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST14 = Elastics14       (B7L1U and A7L1U) and (A7R1L or B7R1L)
        item_alfa_Elastics14 = LVL1MenuItem('L1_ALFA_ELAST14', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1))&(thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST15 = Elastics15       (B7L1U or A7L1U) and (A7R1L or B7R1L)
        item_alfa_Elastics15 = LVL1MenuItem('L1_ALFA_ELAST15', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1U.condition(1)|thr_nimdir_A7L1U.condition(1))&(thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST16 = Elastics16       (B7L1L or A7L1L) and (A7R1U and B7R1U)
        item_alfa_Elastics16 = LVL1MenuItem('L1_ALFA_ELAST16', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1L.condition(1))&(thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST17 = Elastics17       (B7L1L and A7L1L) and (A7R1U or B7R1U)
        item_alfa_Elastics17 = LVL1MenuItem('L1_ALFA_ELAST17', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1))&(thr_nimdir_A7R1U.condition(1)|thr_nimdir_B7R1U.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ELAST18 = Elastics18       (B7L1L or A7L1L) and (A7R1U or B7R1U)
        item_alfa_Elastics18 = LVL1MenuItem('L1_ALFA_ELAST18', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1L.condition(1))&(thr_nimdir_A7R1U.condition(1)|thr_nimdir_B7R1U.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SDIFF1 = Single_Diffraction1       B7L1U and A7L1U not (B7L1L or A7L1L or A7R1U or A7R1L or B7R1U or B7R1L)
        item_alfa_Single_Diffraction1 = LVL1MenuItem('L1_ALFA_SDIFF1', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1)\
                        &Not(thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SDIFF2 = Single_Diffraction2       A7R1L and B7R1L not (B7L1U or B7L1L or A7L1U or A7L1L or A7R1U or B7R1U)
        item_alfa_Single_Diffraction2 = LVL1MenuItem('L1_ALFA_SDIFF2', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1)\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_B7R1U.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SDIFF3 = Single_Diffraction3       B7L1L and A7L1L not (B7L1U or A7L1U A7R1U or A7R1L or B7R1U or B7R1L)
        item_alfa_Single_Diffraction3 = LVL1MenuItem('L1_ALFA_SDIFF3', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1)\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SDIFF4 = Single_Diffraction4       A7R1U and B7R1U not (B7L1U or B7L1L or A7L1U or A7L1L or A7R1L or B7R1L)
        item_alfa_Single_Diffraction4 = LVL1MenuItem('L1_ALFA_SDIFF4', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1)\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SDIFF5 = Single_Diffraction5       B7L1U and A7L1U
        item_alfa_Single_Diffraction5 = LVL1MenuItem('L1_ALFA_SDIFF5', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SDIFF6 = Single_Diffraction6       A7R1L and B7R1L
        item_alfa_Single_Diffraction6 = LVL1MenuItem('L1_ALFA_SDIFF6', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SDIFF7 = Single_Diffraction7       B7L1L and A7L1L
        item_alfa_Single_Diffraction7 = LVL1MenuItem('L1_ALFA_SDIFF7', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SDIFF8 = Single_Diffraction8       A7R1U and B7R1U
        item_alfa_Single_Diffraction8 = LVL1MenuItem('L1_ALFA_SDIFF8', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SHOW1 = Big_showers1       B7L1U and B7L1L and A7L1U and A7L1L     not (A7R1U or A7R1L or B7R1U or B7R1L)
        item_alfa_Big_showers1 = LVL1MenuItem('L1_ALFA_SHOW1', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1U.condition(1)&thr_nimdir_A7L1L.condition(1)\
                        &Not(thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SHOW2 = Big_showers2       A7R1U and A7R1L and B7R1U and B7R1L     not (B7L1U or B7L1L or A7L1U or A7L1L)
        item_alfa_Big_showers2 = LVL1MenuItem('L1_ALFA_SHOW2', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_A7R1U.condition(1)&thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1U.condition(1)&thr_nimdir_B7R1L.condition(1)\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SHOW3 = Big_showers3       B7L1U and B7L1L and A7L1U and A7L1L
        item_alfa_Big_showers3 = LVL1MenuItem('L1_ALFA_SHOW3', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1U.condition(1)&thr_nimdir_A7L1L.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)

        
        # L1_SHOW4 = Big_showers4       A7R1U and A7R1L and B7R1U and B7R1L
        item_alfa_Big_showers4 = LVL1MenuItem('L1_ALFA_SHOW4', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_A7R1U.condition(1)&thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1U.condition(1)&thr_nimdir_B7R1L.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SHOW5 = Big_showers5       (A7R1U or B7R1U) and (B7R1L or A7R1L)
        item_alfa_Big_showers5 = LVL1MenuItem('L1_ALFA_SHOW5', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_A7R1U.condition(1)|thr_nimdir_B7R1U.condition(1))&(thr_nimdir_B7R1L.condition(1)|thr_nimdir_A7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SHOW6 = Big_showers6       (A7L1U or B7L1U) and (B7L1L or A7L1L)
        item_alfa_Big_showers6 = LVL1MenuItem('L1_ALFA_SHOW6', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_A7L1U.condition(1)|thr_nimdir_B7L1U.condition(1))&(thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST1 = Systematics1       B7L1U and B7L1L not (A7L1U or A7L1L or A7R1U or A7R1L or B7R1U or B7R1L)
        item_alfa_Systematics1 = LVL1MenuItem('L1_ALFA_SYST1', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_B7L1L.condition(1)\
                        &Not(thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST2 = Systematics2       A7L1U and A7L1L not (B7L1U or B7L1L or A7R1U or A7R1L or B7R1U or B7R1L)
        item_alfa_Systematics2 = LVL1MenuItem('L1_ALFA_SYST2', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_A7L1U.condition(1)&thr_nimdir_A7L1L.condition(1)\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST3 = Systematics3       A7R1U and A7R1L not (B7L1U or B7L1L or A7L1U or A7L1L or B7R1U or B7R1L)
        item_alfa_Systematics3 = LVL1MenuItem('L1_ALFA_SYST3', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_A7R1U.condition(1)&thr_nimdir_A7R1L.condition(1)\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST4 = Systematics4       B7R1U and B7R1L not (B7L1U or B7L1L or A7L1U or A7L1L or A7R1U or A7R1L)
        item_alfa_Systematics4 = LVL1MenuItem('L1_ALFA_SYST4', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7R1U.condition(1)&thr_nimdir_B7R1L.condition(1)\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SHOWSYST1 = Big_showers_Systematics1       B7L1U and B7L1L
        item_alfa_Big_showers_Systematics1 = LVL1MenuItem('L1_ALFA_SHOWSYST1', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_B7L1L.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SHOWSYST2 = Big_showers_Systematics2       A7L1U and A7L1L
        item_alfa_Big_showers_Systematics2 = LVL1MenuItem('L1_ALFA_SHOWSYST2', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_A7L1U.condition(1)&thr_nimdir_A7L1L.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SHOWSYST3 = Big_showers_Systematics3       A7R1U and A7R1L
        item_alfa_Big_showers_Systematics3 = LVL1MenuItem('L1_ALFA_SHOWSYST3', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_A7R1U.condition(1)&thr_nimdir_A7R1L.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SHOWSYST4 = Big_showers_Systematics4       B7R1U and B7R1L
        item_alfa_Big_showers_Systematics4 = LVL1MenuItem('L1_ALFA_SHOWSYST4', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7R1U.condition(1)&thr_nimdir_B7R1L.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SHOWSYST5 = Big_showers_Systematics5       (B7L1U or B7L1L or A7L1U or A7L1L) and (A7R1U or A7R1L or B7R1U or B7R1L)
        item_alfa_Big_showers_Systematics5 = LVL1MenuItem('L1_ALFA_SHOWSYST5', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1))&(thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST5 = Systematics5       B7L1U and B7L1L and B7R1U and B7R1L     not (A7L1U or A7L1L or A7R1U or A7R1L)
        item_alfa_Systematics5 = LVL1MenuItem('L1_ALFA_SYST5', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_B7L1L.condition(1)&thr_nimdir_B7R1U.condition(1)&thr_nimdir_B7R1L.condition(1)\
                        &Not(thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST6 = Systematics6       (B7L1U or B7L1L) and (B7R1U or B7R1L)   not (A7L1U or A7L1L or A7R1U or A7R1L)
        item_alfa_Systematics6 = LVL1MenuItem('L1_ALFA_SYST6', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1))&(thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))\
                        &Not(thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)

        
        # L1_SYST7 = Systematics7       A7L1U and A7L1L and A7R1U and A7R1L     not (B7L1U or B7L1L or B7R1U or B7R1L)
        item_alfa_Systematics7 = LVL1MenuItem('L1_ALFA_SYST7', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_A7L1U.condition(1)&thr_nimdir_A7L1L.condition(1)&thr_nimdir_A7R1U.condition(1)&thr_nimdir_A7R1L.condition(1)\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST8 = Systematics8       (A7L1U or A7L1L) and (A7R1U or A7R1L)   not (B7L1U or B7L1L or B7R1U or B7R1L)
        item_alfa_Systematics8 = LVL1MenuItem('L1_ALFA_SYST8', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1))&(thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1))\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST9 = Systematics9       B7L1U and A7L1U and A7R1U and B7R1U     not (B7L1L or A7L1L or A7R1L or B7R1L)
        item_alfa_Systematics9 = LVL1MenuItem('L1_ALFA_SYST9', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1)&thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1)\
                        &Not(thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST10 = Systematics10     B7L1L and A7L1L and A7R1L and B7R1L     not (B7L1U or A7L1U or A7R1U UU B7R1U)
        item_alfa_Systematics10 = LVL1MenuItem('L1_ALFA_SYST10', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1)&thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1)\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_B7R1U.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST11 = Systematics11     B7L1U and A7L1U and A7R1U and B7R1U
        item_alfa_Systematics11 = LVL1MenuItem('L1_ALFA_SYST11', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1)&thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST12 = Systematics12     B7L1L and A7L1L and A7R1L and B7R1L
        item_alfa_Systematics12 = LVL1MenuItem('L1_ALFA_SYST12', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1)&thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST13 = Systematics13     (B7L1U or A7L1U) and A7R1U and B7R1U
        item_alfa_Systematics13 = LVL1MenuItem('L1_ALFA_SYST13', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1U.condition(1)|thr_nimdir_A7L1U.condition(1))&thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST14 = Systematics14     B7L1U and A7L1U and (A7R1U or B7R1U)
        item_alfa_Systematics14 = LVL1MenuItem('L1_ALFA_SYST14', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1)&(thr_nimdir_A7R1U.condition(1)|thr_nimdir_B7R1U.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST15 = Systematics15     (B7L1L or A7L1L) and A7R1L and B7R1L
        item_alfa_Systematics15 = LVL1MenuItem('L1_ALFA_SYST15', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1L.condition(1))&thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1)&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST16 = Systematics16     B7L1L and A7L1L and (A7R1L or B7R1L)
        item_alfa_Systematics16 = LVL1MenuItem('L1_ALFA_SYST16', ctpid=-1, group='1', prescale=1).setLogic( \
                        thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1)&(thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST17 = Systematics17     (B7L1U or A7L1U) and (A7R1U or B7R1U)
        item_alfa_Systematics17 = LVL1MenuItem('L1_ALFA_SYST17', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1U.condition(1)|thr_nimdir_A7L1U.condition(1))&(thr_nimdir_A7R1U.condition(1)|thr_nimdir_B7R1U.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST18 = Systematics18     (B7L1L or A7L1L) and (A7R1L or B7R1L)
        item_alfa_Systematics18 = LVL1MenuItem('L1_ALFA_SYST18', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1L.condition(1))&(thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST19 = Systematics19     B7L1U or A7L1U or A7R1U or B7R1U        not (B7L1L or A7L1L or A7R1L or B7R1L)
        item_alfa_Systematics19 = LVL1MenuItem('L1_ALFA_SYST19', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1U.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_B7R1U.condition(1))\
                        &Not(thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_SYST20 = Systematics20     B7L1L or A7L1L or A7R1L or B7R1L        not (B7L1U or A7L1U or A7R1U or B7R1U)
        item_alfa_Systematics20 = LVL1MenuItem('L1_ALFA_SYST20', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1L.condition(1))\
                        &Not(thr_nimdir_B7L1U.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_B7R1U.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)


        # L1_ANY = Minimum_Bias (B7L1U or B7L1L or A7L1U or A7L1L or A7R1U or A7R1L or B7R1U or B7R1L)
        item_alfa_Minimum_Bias = LVL1MenuItem('L1_ALFA_ANY', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&physcond \
                        ).setTriggerType(TT_rpcout)

        # L1_ANY = Minimum_Bias (B7L1U or B7L1L or A7L1U or A7L1L or A7R1U or A7R1L or B7R1U or B7R1L)
        item_alfa_Minimum_Bias_empty = LVL1MenuItem('L1_ALFA_ANY_EMPTY', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&cosmiccond \
                        ).setTriggerType(TT_rpcout)

        # L1_ANY = Minimum_Bias (B7L1U or B7L1L or A7L1U or A7L1L or A7R1U or A7R1L or B7R1U or B7R1L)
        item_alfa_Minimum_Bias_firstempty = LVL1MenuItem('L1_ALFA_ANY_FIRSTEMPTY', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&empty1stcond \
                        ).setTriggerType(TT_rpcout)

        # L1_ANY = Minimum_Bias (B7L1U or B7L1L or A7L1U or A7L1L or A7R1U or A7R1L or B7R1U or B7R1L)
        item_alfa_Minimum_Bias_unpaired_iso = LVL1MenuItem('L1_ALFA_ANY_UNPAIRED_ISO', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&unpaired_isocond \
                        ).setTriggerType(TT_rpcout)

        # L1_ANY = Minimum_Bias (B7L1U or B7L1L or A7L1U or A7L1L or A7R1U or A7R1L or B7R1U or B7R1L)
        item_alfa_Minimum_Bias_unpaired_noniso = LVL1MenuItem('L1_ALFA_ANY_UNPAIRED_NONISO', ctpid=-1, group='1', prescale=1).setLogic( \
                        (thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&unpaired_nonisocond \
                        ).setTriggerType(TT_rpcout)

        #---------------------------------
        # new ALFA items for 2012 running:
        #---------------------------------
        #L1_A7L1U_OD = (NIM_A7L1U_OD_x1 and (BGRP0 and (BGRP1 or BGRP2 or BGRP3 or BGRP4 or BGRP5 or BGRP6)))
        item_alfa_A7L1U_OD = LVL1MenuItem('L1_ALFA_A7L1U_OD', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_A7L1U_OD.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)
        
        #L1_A7L1L_OD = (NIM_A7L1L_OD_x1 and (BGRP0 and (BGRP1 or BGRP2 or BGRP3 or BGRP4 or BGRP5 or BGRP6)))
        item_alfa_A7L1L_OD = LVL1MenuItem('L1_ALFA_A7L1L_OD', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_A7L1L_OD.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)

        # L1_B7L1U_OD = (NIM_B7L1U_OD_x1 and (BGRP0 and (BGRP1 or BGRP2 or BGRP3 or BGRP4 or BGRP5 or BGRP6)))
        item_alfa_B7L1U_OD = LVL1MenuItem('L1_ALFA_B7L1U_OD', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_B7L1U_OD.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)

        
        #L1_B7L1L_OD = (NIM_B7R1L_OD_x1 and (BGRP0 and (BGRP1 or BGRP2 or BGRP3 or BGRP4 or BGRP5 or BGRP6)))
        item_alfa_B7L1L_OD = LVL1MenuItem('L1_ALFA_B7L1L_OD', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_B7L1L_OD.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)

        
        #L1_A7R1U_OD = (NIM_A7R1U_OD_x1 and (BGRP0 and (BGRP1 or BGRP2 or BGRP3 or BGRP4 or BGRP5 or BGRP6)))
        item_alfa_A7R1U_OD = LVL1MenuItem('L1_ALFA_A7R1U_OD', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_A7R1U_OD.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)

        #L1_A7R1L_OD = (NIM_A7R1L_OD_x1 and (BGRP0 and (BGRP1 or BGRP2 or BGRP3 or BGRP4 or BGRP5 or BGRP6)))
        item_alfa_A7R1L_OD = LVL1MenuItem('L1_ALFA_A7R1L_OD', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_A7R1L_OD.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)

        # L1_B7R1U_OD = (NIM_B7R1U_OD_x1 and (BGRP0 and (BGRP1 or BGRP2 or BGRP3 or BGRP4 or BGRP5 or BGRP6)))
        item_alfa_B7R1U_OD = LVL1MenuItem('L1_ALFA_B7R1U_OD', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_B7R1U_OD.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)
        
        #L1_B7R1L_OD = (NIM_B7R1L_OD_x1 and (BGRP0 and (BGRP1 or BGRP2 or BGRP3 or BGRP4 or BGRP5 or BGRP6)))
        item_alfa_B7R1L_OD = LVL1MenuItem('L1_ALFA_B7R1L_OD', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_B7R1L_OD.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)


        #L1_B7L1_OD = ((NIM_B7L1U_OD_x1 and NIM_B7L1L_OD_x1) and (BGRP0 and (BGRP1 or BGRP2 or BGRP3 or BGRP4 or BGRP5 or BGRP6)))
        item_alfa_B7L1_OD = LVL1MenuItem('L1_ALFA_B7L1_OD', ctpid=-1, group='1', prescale=1).setLogic(\
                        ((thr_nimdir_B7L1U_OD.condition(1)&thr_nimdir_B7L1L_OD.condition(1))&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)

        #L1_A7L1_OD = ((NIM_A7L1U_OD_x1 and NIM_A7L1L_OD_x1) and (BGRP0 and (BGRP1 or BGRP2 or BGRP3 or BGRP4 or BGRP5 or BGRP6)))
        item_alfa_A7L1_OD = LVL1MenuItem('L1_ALFA_A7L1_OD', ctpid=-1, group='1', prescale=1).setLogic(\
                        ((thr_nimdir_A7L1U_OD.condition(1)&thr_nimdir_A7L1L_OD.condition(1))&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)

        #L1_A7R1_OD = ((NIM_A7R1U_OD_x1 and NIM_A7R1L_OD_x1) and (BGRP0 and (BGRP1 or BGRP2 or BGRP3 or BGRP4 or BGRP5 or BGRP6)))
        item_alfa_A7R1_OD = LVL1MenuItem('L1_ALFA_A7R1_OD', ctpid=-1, group='1', prescale=1).setLogic(\
                        ((thr_nimdir_A7R1U_OD.condition(1)&thr_nimdir_A7R1L_OD.condition(1))&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)

        #L1_B7R1_OD = ((NIM_B7R1U_OD_x1 and NIM_B7R1L_OD_x1) and (BGRP0 and (BGRP1 or BGRP2 or BGRP3 or BGRP4 or BGRP5 or BGRP6)))
        item_alfa_B7R1_OD = LVL1MenuItem('L1_ALFA_B7R1_OD', ctpid=-1, group='1', prescale=1).setLogic(\
                        ((thr_nimdir_B7R1U_OD.condition(1)&thr_nimdir_B7R1L_OD.condition(1))&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)

        #L1_B7L1U = ((NIM_B7L1U_x1) and (BGRP0))
        item_alfa_B7L1U = LVL1MenuItem('L1_ALFA_B7L1U', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_B7L1U.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)
        
        #L1_B7L1L = ((NIM_B7R1L_x1) and (BGRP0))
        item_alfa_B7L1L = LVL1MenuItem('L1_ALFA_B7L1L', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_B7L1L.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)
        
        #L1_A7L1U = ((NIM_A7L1U_x1) and (BGRP0))
        item_alfa_A7L1U = LVL1MenuItem('L1_ALFA_A7L1U', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_A7L1U.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)
        
        #L1_A7L1L = ((NIM_A7L1L_x1) and (BGRP0))
        item_alfa_A7L1L = LVL1MenuItem('L1_ALFA_A7L1L', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_A7L1L.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)
        
        #L1_A7R1U = ((NIM_A7R1U_x1) and (BGRP0))
        item_alfa_A7R1U = LVL1MenuItem('L1_ALFA_A7R1U', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_A7R1U.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)

        #L1_A7R1L = ((NIM_A7R1L_x1) and (BGRP0))
        item_alfa_A7R1L = LVL1MenuItem('L1_ALFA_A7R1L', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_A7R1L.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)

        #L1_B7R1U = ((NIM_B7R1U_x1) and (BGRP0))
        item_alfa_B7R1U = LVL1MenuItem('L1_ALFA_B7R1U', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_B7R1U.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)
        
        #L1_B7R1L = ((NIM_B7R1L_x1) and (BGRP0))
        item_alfa_B7R1L = LVL1MenuItem('L1_ALFA_B7R1L', ctpid=-1, group='1', prescale=1).setLogic(\
                        (thr_nimdir_B7R1L.condition(1)&Logic(bgrp0))\
                        ).setTriggerType(TT_rpcout)

        #L1_ALFA_B1_Empty = ((NIM_B7L1U_x1 or NIM_B7L1L_x1 or NIM_A7L1U_x1 or NIM_A7L1L_x1) and (BGRP0 and BGRP3))
        item_alfa_B1_Empty = LVL1MenuItem('L1_ALFA_B1_EMPTY', ctpid=-1, group='1', prescale=1).setLogic(\
            ((thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1))&cosmiccond)\
            ).setTriggerType(TT_rpcout)

        #L1_ALFA_B2_Empty = ((NIM_A7R1U_x1 or NIM_A7R1L_x1 or NIM_B7R1U_x1 or NIM_B7R1L_x1) and (BGRP0 and BGRP3))
        item_alfa_B2_Empty = LVL1MenuItem('L1_ALFA_B2_EMPTY', ctpid=-1, group='1', prescale=1).setLogic(\
            ((thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))&cosmiccond)\
            ).setTriggerType(TT_rpcout)


        
        #L1_MBTS_2_A_ALFA_C = ((MBTS_A_x2 and ((NIM_A7R1U_x1 and NIM_B7R1U_x1) or (NIM_A7R1L_x1 and NIM_B7R1L_x1))) and (BGRP0 and BGRP1))
        item_alfa_MBTS_2_A_ALFA_C = LVL1MenuItem('L1_MBTS_2_A_ALFA_C', ctpid=-1, group='1', prescale=1).setLogic(\
            (((thr_mbtsA.condition(2)&((thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1))|(thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1))))&bgrpcond))\
            ).setTriggerType(TT_rpcout)

        #L1_MBTS_2_C_ALFA_A = ((MBTS_C_x2 and ((NIM_A7L1U_x1 and NIM_B7L1U_x1) or (NIM_A7L1L_x1 and NIM_B7L1L_x1))) and (BGRP0 and BGRP1))
        item_alfa_MBTS_2_C_ALFA_A = LVL1MenuItem('L1_MBTS_2_C_ALFA_A', ctpid=-1, group='1', prescale=1).setLogic(\
            (((thr_mbtsC.condition(2)& ((thr_nimdir_A7L1U.condition(1)&thr_nimdir_B7L1U.condition(1))|(thr_nimdir_A7L1L.condition(1)&thr_nimdir_B7L1L.condition(1))))&bgrpcond))\
            ).setTriggerType(TT_rpcout)
                    
        #L1_MBTS_2_ALFA	= (((MBTS_A_x2 or MBTS_C_x2 or MBTS_A_x1) and (MBTS_A_x2 or MBTS_C_x2 or MBTS_C_x1)) and (NIM_A7L1U_x1 or NIM_A7L1L_x1 or NIM_B7L1U_x1 or NIM_B7L1L_x1 or NIM_A7R1U_x1 or NIM_A7R1L_x1 or NIM_B7R1U_x1 or NIM_B7R1L_x1)) and (BGRP0 and BGRP1))
        item_alfa_MBTS_2_ALFA = LVL1MenuItem('L1_MBTS_2_ALFA', ctpid=-1, group='1', prescale=1).setLogic(\
            ((((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))&(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1)))&(thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1)))&bgrpcond)\
            ).setTriggerType(TT_rpcout)


        #L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO = ((MBTS_A_x2 and ((NIM_A7R1U_x1 and NIM_B7R1U_x1) or (NIM_A7R1L_x1 and NIM_B7R1L_x1))) and unpaired_isocond)
        item_alfa_MBTS_2_A_ALFA_C_UNPAIRED_ISO = LVL1MenuItem('L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO', ctpid=-1, group='1', prescale=1).setLogic(\
            (((thr_mbtsA.condition(2)&((thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1))|(thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1))))&unpaired_isocond))\
            ).setTriggerType(TT_rpcout)

        #L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO = ((MBTS_C_x2 and ((NIM_A7L1U_x1 and NIM_B7L1U_x1) or (NIM_A7L1L_x1 and NIM_B7L1L_x1))) and unpaired_isocond)
        item_alfa_MBTS_2_C_ALFA_A_UNPAIRED_ISO = LVL1MenuItem('L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO', ctpid=-1, group='1', prescale=1).setLogic(\
            (((thr_mbtsC.condition(2)& ((thr_nimdir_A7L1U.condition(1)&thr_nimdir_B7L1U.condition(1))|(thr_nimdir_A7L1L.condition(1)&thr_nimdir_B7L1L.condition(1))))&unpaired_isocond))\
            ).setTriggerType(TT_rpcout)
            
        #L1_MBTS_2_ALFA_UNPAIRED_ISO = (((MBTS_A_x2 or MBTS_C_x2 or MBTS_A_x1) and (MBTS_A_x2 or MBTS_C_x2 or MBTS_C_x1)) and (NIM_A7L1U_x1 or NIM_A7L1L_x1 or NIM_B7L1U_x1 or NIM_B7L1L_x1 or NIM_A7R1U_x1 or NIM_A7R1L_x1 or NIM_B7R1U_x1 or NIM_B7R1L_x1)) and unpaired_isocond)
        item_alfa_MBTS_2_ALFA_UNPAIRED_ISO = LVL1MenuItem('L1_MBTS_2_ALFA_UNPAIRED_ISO', ctpid=-1, group='1', prescale=1).setLogic(\
            ((((thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsA.condition(1))&(thr_mbtsA.condition(2)|thr_mbtsC.condition(2)|thr_mbtsC.condition(1)))&(thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1)))&unpaired_isocond)\
            ).setTriggerType(TT_rpcout)


        #L1_MBTS_1_A_ALFA_C = ((MBTS_A_x1 and ((NIM_A7R1U_x1 and NIM_B7R1U_x1) or (NIM_A7R1L_x1 and NIM_B7R1L_x1))) and (BGRP0 and BGRP1))
        item_alfa_MBTS_1_A_ALFA_C = LVL1MenuItem('L1_MBTS_1_A_ALFA_C', ctpid=-1, group='1', prescale=1).setLogic(\
            (((thr_mbtsA.condition(1)&((thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1))|(thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1))))&bgrpcond))\
            ).setTriggerType(TT_rpcout)

        #L1_MBTS_1_C_ALFA_A = ((MBTS_C_x1 and ((NIM_A7L1U_x1 and NIM_B7L1U_x1) or (NIM_A7L1L_x1 and NIM_B7L1L_x1))) and (BGRP0 and BGRP1))
        item_alfa_MBTS_1_C_ALFA_A = LVL1MenuItem('L1_MBTS_1_C_ALFA_A', ctpid=-1, group='1', prescale=1).setLogic(\
            (((thr_mbtsC.condition(1)& ((thr_nimdir_A7L1U.condition(1)&thr_nimdir_B7L1U.condition(1))|(thr_nimdir_A7L1L.condition(1)&thr_nimdir_B7L1L.condition(1))))&bgrpcond))\
            ).setTriggerType(TT_rpcout)
            

        #L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO = ((MBTS_A_x1 and ((NIM_A7R1U_x1 and NIM_B7R1U_x1) or (NIM_A7R1L_x1 and NIM_B7R1L_x1))) and &unpaired_isocond)
        item_alfa_MBTS_1_A_ALFA_C_UNPAIRED_ISO = LVL1MenuItem('L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO', ctpid=-1, group='1', prescale=1).setLogic(\
            (((thr_mbtsA.condition(1)&((thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1))|(thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1))))&unpaired_isocond))\
            ).setTriggerType(TT_rpcout)

        #L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO = ((MBTS_C_x1 and ((NIM_A7L1U_x1 and NIM_B7L1U_x1) or (NIM_A7L1L_x1 and NIM_B7L1L_x1))) and &unpaired_isocond)
        item_alfa_MBTS_1_C_ALFA_A_UNPAIRED_ISO = LVL1MenuItem('L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO', ctpid=-1, group='1', prescale=1).setLogic(\
            (((thr_mbtsC.condition(1)& ((thr_nimdir_A7L1U.condition(1)&thr_nimdir_B7L1U.condition(1))|(thr_nimdir_A7L1L.condition(1)&thr_nimdir_B7L1L.condition(1))))&unpaired_isocond))\
            ).setTriggerType(TT_rpcout)
            


        #L1_LUCID_A_ALFA_C = ((LUCID_A_x1 and ((NIM_A7R1U_x1 and NIM_B7R1U_x1) or (NIM_A7R1L_x1 and NIM_B7R1L_x1))) and (BGRP0 and BGRP1))
        item_alfa_LUCID_A_ALFA_C = LVL1MenuItem('L1_LUCID_A_ALFA_C', ctpid=-1, group='1', prescale=1).setLogic(\
                                   ((thr_lucidA.condition(1)&((thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1))|(thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1))))&bgrpcond)\
                                   ).setTriggerType(TT_rpcout)
        
        #L1_LUCID_C_ALFA_A = ((LUCID_C_x1 and ((NIM_A7L1U_x1 and NIM_B7L1U_x1) or (NIM_A7L1L_x1 and NIM_B7L1L_x1))) and (BGRP0 and BGRP1))
        item_alfa_LUCID_C_ALFA_A = LVL1MenuItem('L1_LUCID_C_ALFA_A', ctpid=-1, group='1', prescale=1).setLogic(\
                                   ((thr_lucidC.condition(1)&((thr_nimdir_A7L1U.condition(1)&thr_nimdir_B7L1U.condition(1))|(thr_nimdir_A7L1L.condition(1)&thr_nimdir_B7L1L.condition(1))))&bgrpcond)\
                                   ).setTriggerType(TT_rpcout)


        #L1_LUCID_A_ALFA_C_UNPAIRED_ISO = ((LUCID_A_x1 and ((NIM_A7R1U_x1 and NIM_B7R1U_x1) or (NIM_A7R1L_x1 and NIM_B7R1L_x1))) and &unpaired_isocond)
        item_alfa_LUCID_A_ALFA_C_UNPAIRED_ISO = LVL1MenuItem('L1_LUCID_A_ALFA_C_UNPAIRED_ISO', ctpid=-1, group='1', prescale=1).setLogic(\
                                   ((thr_lucidA.condition(1)&((thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1))|(thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1))))&unpaired_isocond)\
                                   ).setTriggerType(TT_rpcout)
        
        #L1_LUCID_C_ALFA_A_UNPAIRED_ISO = ((LUCID_C_x1 and ((NIM_A7L1U_x1 and NIM_B7L1U_x1) or (NIM_A7L1L_x1 and NIM_B7L1L_x1))) and &unpaired_isocond)
        item_alfa_LUCID_C_ALFA_A_UNPAIRED_ISO = LVL1MenuItem('L1_LUCID_C_ALFA_A_UNPAIRED_ISO', ctpid=-1, group='1', prescale=1).setLogic(\
                                   ((thr_lucidC.condition(1)&((thr_nimdir_A7L1U.condition(1)&thr_nimdir_B7L1U.condition(1))|(thr_nimdir_A7L1L.condition(1)&thr_nimdir_B7L1L.condition(1))))&unpaired_isocond)\
                                   ).setTriggerType(TT_rpcout)


        
        #L1_LUCID_ALFA	= (((LUCID_A_x1 or LUCID_C_x1) and (NIM_A7L1U_x1 or NIM_A7L1L_x1 or NIM_B7L1U_x1 or NIM_B7L1L_x1 or NIM_A7R1U_x1 or NIM_A7R1L_x1 or NIM_B7R1U_x1 or NIM_B7R1L_x1)) and (BGRP0 and BGRP1))
        item_alfa_LUCID_ALFA_= LVL1MenuItem('L1_LUCID_ALFA', ctpid=-1, group='1', prescale=1).setLogic(\
                               (((thr_lucidA.condition(1)|thr_lucidC.condition(1))&(thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1)))&bgrpcond)\
                               ).setTriggerType(TT_rpcout)

        #L1_ZDC_A_ALFA_C = ((ZDC_A_x1 and ((NIM_A7R1U_x1 and NIM_B7R1U_x1) or (NIM_A7R1L_x1 and NIM_B7R1L_x1))) and (NIM_BGRP0_x1 and NIM_BGRP1_x1))
        item_alfa_ZDC_A_ALFA_C= LVL1MenuItem('L1_ZDC_A_ALFA_C', ctpid=-1, group='1', prescale=1).setLogic(\
                                ((thr_zdcA.condition(1)&((thr_nimdir_A7R1U.condition(1)&thr_nimdir_B7R1U.condition(1))|(thr_nimdir_A7R1L.condition(1)&thr_nimdir_B7R1L.condition(1))))&bgrpcond)\
                                ).setTriggerType(TT_rpcout)
        
        #L1_ZDC_C_ALFA_A = ((ZDC_C_x1 and ((NIM_A7L1U_x1 and NIM_B7L1U_x1) or (NIM_A7L1L_x1 and NIM_B7L1L_x1))) and (NIM_BGRP0_x1 and NIM_BGRP1_x1))
        item_alfa_ZDC_C_ALFA_A= LVL1MenuItem('L1_ZDC_C_ALFA_A', ctpid=-1, group='1', prescale=1).setLogic(\
                                ((thr_zdcC.condition(1)&((thr_nimdir_A7L1U.condition(1)&thr_nimdir_B7L1U.condition(1))|(thr_nimdir_A7L1L.condition(1)&thr_nimdir_B7L1L.condition(1))))&bgrpcond)\
                                ).setTriggerType(TT_rpcout)

        #L1_ZDC_ALFA = (((ZDC_A_x1 or ZDC_C_x1) and (NIM_A7L1U_x1 or NIM_A7L1L_x1 or NIM_B7L1U_x1 or NIM_B7L1L_x1 or NIM_A7R1U_x1 or NIM_A7R1L_x1 or NIM_B7R1U_x1 or NIM_B7R1L_x1)) and (BGRP0 and BGRP1))
        item_alfa_ZDC_ALFA= LVL1MenuItem('L1_ZDC_ALFA', ctpid=-1, group='1', prescale=1).setLogic(\
                            (((thr_zdcA.condition(1)|thr_zdcC.condition(1))&(thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)|thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)|thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)|thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1)))&bgrpcond)\
                               ).setTriggerType(TT_rpcout)


        #new combined ALFA items for 2012 running

        #L1_EM3_ALFA_ANY
        item_em3_alfa_any = LVL1MenuItem('L1_EM3_ALFA_ANY',   ctpid=-1, group='1', prescale=1)\
                            .setLogic(thr_em3.condition(1)\
                                      &(thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)\
                                      |thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)\
                                      |thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)\
                                      |thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))\
                                      &physcond).setTriggerType(TT_calo|TT_caloem|TT_rpcout)

        #L1_EM3_ALFA_MBTS_C
        item_em3_alfa_mbts_c = LVL1MenuItem('L1_EM3_ALFA_MBTS_C',   ctpid=-1, group='1', prescale=1)\
                               .setLogic(thr_em3.condition(1)\
                               &thr_mbtsC.condition(1)\
                               &((thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1))\
                                 |(thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1)))\
                                 &physcond).setTriggerType(TT_calo|TT_caloem|TT_minb|TT_rpcout)

        #L1_EM3_ALFA_MBTS_A
        item_em3_alfa_mbts_a = LVL1MenuItem('L1_EM3_ALFA_MBTS_A',   ctpid=-1, group='1', prescale=1)\
                               .setLogic(thr_em3.condition(1)\
                               &thr_mbtsA.condition(1)\
                               &((thr_nimdir_B7R1U.condition(1)&thr_nimdir_A7R1U.condition(1))\
                                 |(thr_nimdir_B7R1L.condition(1)&thr_nimdir_A7R1L.condition(1)))\
                                 &physcond).setTriggerType(TT_calo|TT_caloem|TT_minb|TT_rpcout)
        
        #L1_EM3_ALFA_ANY_UNPAIRED_ISO
        item_em3_alfa_any_unpiso = LVL1MenuItem('L1_EM3_ALFA_ANY_UNPAIRED_ISO',   ctpid=-1, group='1', prescale=1)\
                                   .setLogic(thr_em3.condition(1)\
                                             &(thr_nimdir_B7L1U.condition(1)|thr_nimdir_B7L1L.condition(1)\
                                               |thr_nimdir_A7L1U.condition(1)|thr_nimdir_A7L1L.condition(1)\
                                               |thr_nimdir_A7R1U.condition(1)|thr_nimdir_A7R1L.condition(1)\
                                               |thr_nimdir_B7R1U.condition(1)|thr_nimdir_B7R1L.condition(1))\
                                             &unpaired_isocond).setTriggerType(TT_calo|TT_caloem|TT_rpcout)

        #L1_EM3_ALFA_MBTS_C_UNPAIRED_ISO
        item_em3_alfa_mbts_c_unpiso = LVL1MenuItem('L1_EM3_ALFA_MBTS_C_UNPAIRED_ISO', ctpid=-1, group='1', prescale=1)\
                                      .setLogic(thr_em3.condition(1)\
                                                &thr_mbtsC.condition(1)\
                                                &((thr_nimdir_B7L1U.condition(1)&thr_nimdir_A7L1U.condition(1))\
                                                  |(thr_nimdir_B7L1L.condition(1)&thr_nimdir_A7L1L.condition(1)))\
                                                &unpaired_isocond).setTriggerType(TT_calo|TT_caloem|TT_minb|TT_rpcout)
        
        #L1_EM3_ALFA_MBTS_A_UNPAIRED_ISO
        item_em3_alfa_mbts_a_unpiso = LVL1MenuItem('L1_EM3_ALFA_MBTS_A_UNPAIRED_ISO', ctpid=-1, group='1', prescale=1)\
                                      .setLogic(thr_em3.condition(1)\
                                                &thr_mbtsA.condition(1)\
                                                &((thr_nimdir_B7R1U.condition(1)&thr_nimdir_A7R1U.condition(1))\
                                                  |(thr_nimdir_B7R1L.condition(1)&thr_nimdir_A7R1L.condition(1)))\
                                                &unpaired_isocond).setTriggerType(TT_calo|TT_caloem|TT_minb|TT_rpcout)

        

        # HI items (vetoed with the TE)
        item_j10_veto_te500  = LVL1MenuItem('L1_J10_VTE500',   ctpid=-1, group='1').setLogic(thr_j10.condition(1)&Not(thr_te500.condition(1))&physcond).setTriggerType(TT_calo)
        item_j10_veto_te1000 = LVL1MenuItem('L1_J10_VTE1000',  ctpid=-1, group='1').setLogic(thr_j10.condition(1)&Not(thr_te1000.condition(1))&physcond).setTriggerType(TT_calo)
        item_j10_veto_te2000 = LVL1MenuItem('L1_J10_VTE2000',  ctpid=-1, group='1').setLogic(thr_j10.condition(1)&Not(thr_te2000.condition(1))&physcond).setTriggerType(TT_calo)

        item_j15_veto_te500  = LVL1MenuItem('L1_J15_VTE500',   ctpid=-1, group='1').setLogic(thr_j15.condition(1)&Not(thr_te500.condition(1))&physcond).setTriggerType(TT_calo)
        item_j15_veto_te1000 = LVL1MenuItem('L1_J15_VTE1000',  ctpid=-1, group='1').setLogic(thr_j15.condition(1)&Not(thr_te1000.condition(1))&physcond).setTriggerType(TT_calo)
        item_j15_veto_te2000 = LVL1MenuItem('L1_J15_VTE2000',  ctpid=-1, group='1').setLogic(thr_j15.condition(1)&Not(thr_te2000.condition(1))&physcond).setTriggerType(TT_calo)


        item_j30_veto_te1000 = LVL1MenuItem('L1_J30_VTE1000',  ctpid=-1, group='1').setLogic(thr_j30.condition(1)&Not(thr_te1000.condition(1))&physcond).setTriggerType(TT_calo)
        item_j30_veto_te2000 = LVL1MenuItem('L1_J30_VTE2000',  ctpid=-1, group='1').setLogic(thr_j30.condition(1)&Not(thr_te2000.condition(1))&physcond).setTriggerType(TT_calo)
        item_j30_veto_te4000 = LVL1MenuItem('L1_J30_VTE4000',  ctpid=-1, group='1').setLogic(thr_j30.condition(1)&Not(thr_te4000.condition(1))&physcond).setTriggerType(TT_calo)

        item_j55_veto_te1000 = LVL1MenuItem('L1_J55_VTE1000',  ctpid=-1, group='1').setLogic(thr_j55.condition(1)&Not(thr_te1000.condition(1))&physcond).setTriggerType(TT_calo)
        item_j55_veto_te2000 = LVL1MenuItem('L1_J55_VTE2000',  ctpid=-1, group='1').setLogic(thr_j55.condition(1)&Not(thr_te2000.condition(1))&physcond).setTriggerType(TT_calo)
        item_j55_veto_te4000 = LVL1MenuItem('L1_J55_VTE4000',  ctpid=-1, group='1').setLogic(thr_j55.condition(1)&Not(thr_te4000.condition(1))&physcond).setTriggerType(TT_calo)        
        item_j55_veto_te6000 = LVL1MenuItem('L1_J55_VTE6000',  ctpid=-1, group='1').setLogic(thr_j55.condition(1)&Not(thr_te6000.condition(1))&physcond).setTriggerType(TT_calo)        

        



#        # Spare items for very initial running
#        item_em3_spare   = LVL1MenuItem('L1_EM3_SPARE',   ctpid=0xd5, group='1', prescale=1)\
#                           .setLogic(thr_em3.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_calo)
#        item_j5_spare    = LVL1MenuItem('L1_J5_SPARE',   ctpid=0xd6, group='1', prescale=1).setLogic(thr_j5.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_calo)
#        item_tau5_spare  = LVL1MenuItem('L1_TAU5_SPARE',  ctpid=0xd7, group='1', prescale=1)\
#                           .setLogic(thr_tau5.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_calo)
#        item_mu0_tgc_halo_spare = LVL1MenuItem('L1_MU0_TGC_HALO_SPARE',  ctpid=0xd8, group='1', prescale=1)\
#                                  .setLogic(thr_mu0_tgc_halo.condition(1)&Logic(bgrp0)&Logic(bgrp1))\
#                                  .setTriggerType(TT_tgc)
#        item_mu0_low_rpc_spare  = LVL1MenuItem('L1_MU0_LOW_RPC_SPARE',  ctpid=0xd9, group='1', prescale=1)\
#                                  .setLogic(thr_mu0_low_rpc.condition(1)&Logic(bgrp0)&Logic(bgrp1))\
#                                  .setTriggerType(TT_rpcin)
#        item_xe20_spare = LVL1MenuItem('L1_XE20_SPARE', ctpid=0xda, group='1', prescale=1).setLogic(thr_xe20.condition(1)&Logic(bgrp0)&Logic(bgrp1)).setTriggerType(TT_calo)

        
        for a in dir():
            if a.startswith('item_'):
                eval("tm.registerLvl1Item(%s.name, %s)" % (a, a))

        tm.CTPInfo().random.rate1 = 0
        tm.CTPInfo().random.rate2 = 0

        #set defaults for deadtime that are reasonable (from recent menus)
        tm.CTPInfo().deadtime.name = "Default"
        tm.CTPInfo().deadtime.complex1_rate = 415
        tm.CTPInfo().deadtime.complex1_level = 7
        tm.CTPInfo().deadtime.complex2_rate = 415
        tm.CTPInfo().deadtime.complex2_level = 7
        tm.CTPInfo().deadtime.simple = 5

        if ('Cosmic2009_v2' in TriggerFlags.triggerMenuSetup()\
            or 'InitialBeam_v1' in TriggerFlags.triggerMenuSetup()\
            or 'Cosmic2009_simpleL1Calib' in TriggerFlags.triggerMenuSetup()\
            or 'Cosmic2009_inclMuons' in TriggerFlags.triggerMenuSetup()\
            ):
            #tm.CTPInfo().deadtime.name = "Commissioning"
            tm.CTPInfo().deadtime.complex1_rate = 415
            tm.CTPInfo().deadtime.complex1_level = 7
            tm.CTPInfo().deadtime.complex2_rate = 415
            tm.CTPInfo().deadtime.complex2_level = 7
            tm.CTPInfo().deadtime.simple = 15

        if ('InitialBeam_v2' in TriggerFlags.triggerMenuSetup()\
            or 'MC_InitialBeam_v2' in TriggerFlags.triggerMenuSetup()\
            or 'InitialBeam_v3' in TriggerFlags.triggerMenuSetup()\
            or 'MC_InitialBeam_v3' in TriggerFlags.triggerMenuSetup()\
            or 'Cosmic_v2' in TriggerFlags.triggerMenuSetup()\
            or 'Cosmic_v3' in TriggerFlags.triggerMenuSetup()\
            or 'Physics_pp_v1' in TriggerFlags.triggerMenuSetup()\
            or 'Physics_pp_v2' in TriggerFlags.triggerMenuSetup()\
            or 'Physics_pp_v3' in TriggerFlags.triggerMenuSetup()\
            or 'Physics_pp_v4' in TriggerFlags.triggerMenuSetup()\
            or 'L1_pp_v2' in TriggerFlags.triggerMenuSetup()\
            or 'L1_pp_v3' in TriggerFlags.triggerMenuSetup()\
            or 'L1_pp_v4' in TriggerFlags.triggerMenuSetup()\
            or 'L1_pp_test' in TriggerFlags.triggerMenuSetup()\
            or 'L1_alfa_v1' in TriggerFlags.triggerMenuSetup()\
            or 'L1_alfa_v2' in TriggerFlags.triggerMenuSetup()\
            ):
            tm.CTPInfo().deadtime.name = "Physics"
            tm.CTPInfo().deadtime.complex1_rate = 415
            tm.CTPInfo().deadtime.complex1_level = 7
            tm.CTPInfo().deadtime.complex2_rate = 415
            tm.CTPInfo().deadtime.complex2_level = 7
            tm.CTPInfo().deadtime.simple = 5

            ### commented out, bunchgroup set is always loaded by default in TriggerPythonConfig (Joerg)
            # add unpaired bunch group
            #tm.CTPInfo().setBunchGroupSet("bg_set1")            
            #tm.CTPInfo().addBunchGroup( 'Unpaired', 7, [123] )

            
    registerItems = staticmethod(registerItems)

    class ThrGetter:
        def __init__(self, tm):
            self.tm = tm
        def obj(self, name):
            thr = tm.thresholdOfName(name)
            return thr
        
    def generateMenu(triggerPythonConfig, \
                     lvl1_items, lvl1_thresholds):
        # First update the mapping of thresholds
        #... empty for now
        #
        #        print "Important printout..."


        names = []
        ids = []
        ctpid0 = 0

        # Populating list of used ctpids
        for item in lvl1_items:
            item2 = triggerPythonConfig.getLvl1Item(item)
            if item2!=None:
                if(item2.ctpid!=-1):
                    ids.append(item2.ctpid)
                    names.append(item2.name)
                    ctpid0 += 1

        # Setting the physics bit
        for item in lvl1_items:
            if not item.startswith('L1_CALREQ') and not item=='L1_MU0_LOW':
                item2 = triggerPythonConfig.getLvl1Item(item)
                if item2 == None:
                    raise Exception("Function getLvl1Item failed for item: "+item)
                else:
                    log.debug( "Setting physics bit for "+item+". Changing from "+str(item2.trigger_type)+" to "+str(item2.trigger_type|TT_phys))
                    item2.setTriggerType(item2.trigger_type|TT_phys)

        # Debugging trigger types
        for item in lvl1_items:
            TT=''
            x=triggerPythonConfig.getLvl1Item(item).trigger_type
            for i in range(0,8):
                if(x%2==1):
                    TT='1'+TT
                else:
                    TT='0'+TT
                x=x/2
            log.debug("Trigger type for "+item+" is: "+TT)
                
        # Populating list of available ones
        ctpids = []
        for i in range(0, 255):
            if str(i) not in ids:
#                print "Unused CTPID: "+str(i)
                ctpids.append(i)

        i=0        
        for item in lvl1_items:
            # Add item
            item2 = triggerPythonConfig.getLvl1Item(item)
            if item2 == None:
                sss = 'Item %s was not registered' % item
                log.warning(sss)
            if item2.ctpid==str(-1):
                sss = 'Reassigning ctpid for '+item2.name+' from -1 to '+str(ctpids[i])
                log.debug(sss)
                item2.setCtpid(ctpids[i])
                i+=1
            triggerPythonConfig.addLVL1Item(item2)

        for thr in lvl1_thresholds:
            if triggerPythonConfig.thresholdOfName(thr)==None:
                thr2 = triggerPythonConfig.getLvl1Threshold(thr)
                if thr2==None:
                    log.debug('Threshold %s not defined' % thr )
                else:
                    log.debug('Setting secondary threshold %s' % thr )
                    triggerPythonConfig.addLVL1Threshold(thr2)

        if TriggerFlags.Lvl1.RemapThresholdsAsListed():
            # Set the correct mapping of thresholds according
            # to the order it was given in Lvl1Flags.thresholds list.
            thr_mapping = { # NIM and CALREQ types left out intentionally.
                'MUON': 0,
                'EM': 0, 
                'TAU': 0, 
                'JET': 0, 
                'JF': 0, 
                'JB': 0, 
                'XE': 0, 
                'TE': 0, 
                'XS': 0, 
                'JE': 0, 
                'MBTS': 0,
                'MBTSSI': 0,
                'TRT': 0,
                'ZDC': 0,
                'BCM': 0,
                'BCMCMB': 0,
                'LUCID': 0,
                'ZB': 0,
                }
            
            for thr in lvl1_thresholds:
                thr2 = triggerPythonConfig.thresholdOfName(thr)
                if thr2==None:
                    log.debug('Threshold %s not added' % thr )
                else:
                    if thr2.ttype=='NIM' or thr2.ttype=='CALREQ':
                        log.debug('Skipping remapping of '+thr2.ttype+' threshold')
                        continue
                    log.debug('Setting mapping of threshold %s as %s' %
                             (thr, int(thr_mapping[thr2.ttype])))
                    thr2.mapping = thr_mapping[thr2.ttype]
                    thr_mapping[thr2.ttype] += 1
                    # Refreshing cabling
                    # ZB threshold needs seed as mapping
                    if thr2.ttype == 'ZB':
                        (slot, connector, bitnum, range_begin, range_end) = triggerPythonConfig.cableInput(thr2.seed_ttype, thr2.mapping)
                        thr2.range_begin = 30
                        thr2.range_end = 30
                        thr2.bitnum = 1
                    else:
                        (slot, connector, bitnum, range_begin, range_end) = triggerPythonConfig.cableInput(thr2.ttype, thr2.mapping)
                        thr2.range_begin = range_begin
                        thr2.range_end = range_end
                    thr2.slot = slot
                    thr2.connector = connector
                    #thr2.cable = Lvl1Thresholds.cableName(thr2.ttype, thr2.mapping)
                    if thr2.ttype=='EM' and thr2.mapping>=8: thr2.cable = 'CP2'
    generateMenu = staticmethod(generateMenu)


    def setThresholdTypesFromXML(triggerPythonConfig,l1menufilename):
        """When called, sets the types of all trigger thresholds
        as given in the l1menu (which is generated from xml)
        
        * l1menufilename is the L1.xml filename
        """
        # check if it needs to be reloaded
        if not triggerPythonConfig.l1menuFromXML or triggerPythonConfig.l1menuFromXML.filename != l1menufilename:
            from TriggerMenuPython.XMLReader import L1MenuXMLReader
            triggerPythonConfig.l1menuFromXML = L1MenuXMLReader(l1menufilename)
        # only checking physics bit for now
        #bit=7 # physics

        for t in triggerPythonConfig.l1menuFromXML.getL1Items():
            tt = t['trigger_type']
            item = triggerPythonConfig.getLvl1Item(t['name'])
            if not item:
                log.warning("Item %s is defined in xml [%s], but not in Lvl1.py:registerItems()" % (t['name'],l1menufilename))
                continue
            # clear item trigger_type
            item.setTriggerType(0)
            # set for each bit the trigger_type
            for bit in xrange(8):
                setbit = 1<<bit
                if len(tt)>bit and tt[-bit-1]=='1':
                    item.setTriggerType(item.trigger_type|setbit)
    setThresholdTypesFromXML = staticmethod(setThresholdTypesFromXML)




    def setThresholdSetForType(triggerPythonConfig, thr_names, type):
        j = 0
        for thr_name in thr_names:
            thr = triggerPythonConfig.getLvl1Threshold(thr_name)
            if thr!=None:
                j += 1
                thr.mapping = j
        
    def updateItemPrescale(triggerPythonConfig, it_name, ps):
        item = triggerPythonConfig.getLvl1Item(it_name)
        if item != None:
            item.prescale = ps
        else:
            log.warning('Cannot find item %s to set the prescale' % \
                        (it_name) )
    updateItemPrescale = staticmethod(updateItemPrescale)
    
    def updateItemPrescales(triggerPythonConfig, item_and_ps):
        for (it_name, ps) in item_and_ps.iteritems():
            Lvl1.updateItemPrescale(triggerPythonConfig, it_name, ps)
    updateItemPrescales = staticmethod(updateItemPrescales)

    def readPrescales(fname):
        f = open(fname, 'r')
        re_ctpid_name_prescale = re.compile('\s+(\d+)\s+(L1_[\w_]+)\s+(\d+)')
        ret_map={}
        for line in f.readlines():
            if len(line)>0: line = line[0:-1]
            if line[0]=='#': continue
            mg = re_ctpid_name_prescale.match(line)
            if mg:
                (ctpid, name, ps) = mg.groups()
                if name not in ret_map.keys():
                    ret_map[name] = int(ps)
                else:
                    log.warning('Duplicate Lvl1 item "%s" found in prescale file: ' % \
                                (name, fname))
        f.close()
        return ret_map
    readPrescales = staticmethod(readPrescales)
    
            
            
#   Item name            | Prescale
#----------------------------------------------------------
L1Prescales_lumi_1E31 = {
    }

L1Prescales_lumi_1E32 = {
    }


