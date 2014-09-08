#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TriggerJobOpts.TriggerFlags import TriggerFlags
import re

from AthenaCommon.Logging import logging
log = logging.getLogger('TriggerMenu.Lvl1.py')

# The trigger types
from TriggerMenu.l1.TriggerTypeDef import TT
from TriggerMenu.l1.Limits import Limits
from TriggerMenu.l1.Logic import Logic,Not
from TriggerMenu.l1.Lvl1MenuItems import LVL1MenuItem
from TriggerMenu.l1.Lvl1Condition import ThrCondition, Lvl1InternalTrigger


class ItemDef:
    """
    Defines any items that might be needed in Run2
    """

    @staticmethod
    def registerItems(tm):
        """Register LVL1 thresholds for further use"""

        # register all thresholds ...
        from ThresholdDef import ThresholdDef
        ThresholdDef.registerThresholds(tm)

        # ... and make them accessible by their name
        for thr in tm.registeredThresholds.values():
            thrvarname = thr.name.replace('.','') # we can not have . in the variable name
            exec("%s = ThrCondition(thr)" % thrvarname)

        # InternalTriggers
        for i in range( Limits.NumBunchgroups ):
            exec("BGRP%i = Logic(Lvl1InternalTrigger('BGRP%i'))" % (i,i))

        for i in range( Limits.NumRndmTriggers ):
            exec("RNDM%i = Logic(Lvl1InternalTrigger('RNDM%i'))" % (i,i))


        LVL1MenuItem.l1configForRegistration = tm  # from now on all new LVL1MenuItems are registered to tm

        # Setting up bunch group and BPTX conditions
        physcond            = BGRP0 & BGRP1
        calibcond           = BGRP0 & BGRP2
        cosmiccond          = BGRP0 & BGRP3
        unpaired_isocond    = BGRP0 & BGRP4 # unpaired isolated (satellite bunches)
        unpaired_nonisocond = BGRP0 & BGRP5 # unpaired non-isolated (parasitic bunches)
        firstempty          = BGRP0 & BGRP6
        unpairedRcond       = BGRP0 & BGRP7 # unpaired beams 1 oR beam 2
        bgrp7cond           = BGRP0 & BGRP7 # No unpaired anymore

        # partition 1
        bgrpcond1           = BGRP0 & BGRP11
        calibcond1          = BGRP0 & BGRP12

        # partition 2
        bgrpcond2           = BGRP0 & BGRP14


        LVL1MenuItem.currentPartition = 1


        # E/gamma ctpid=[0:0x1f]
        emTT = TT.calo | TT.caloem
        LVL1MenuItem('L1_EM3'       ).setLogic( EM3        & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM4'       ).setLogic( EM4        & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM5'       ).setLogic( EM5        & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM6'       ).setLogic( EM6        & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM7'       ).setLogic( EM7        & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM10'      ).setLogic( EM10       & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM12'      ).setLogic( EM12       & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM14'      ).setLogic( EM14       & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM16'      ).setLogic( EM16       & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM30'      ).setLogic( EM30       & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM50'      ).setLogic( EM50       & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM7V'      ).setLogic( EM7V       & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM15V'     ).setLogic( EM15V      & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM20V'     ).setLogic( EM20V      & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM8VH'     ).setLogic( EM8VH      & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM15VH'    ).setLogic( EM15VH     & physcond).setTriggerType( emTT )  
        LVL1MenuItem('L1_EM18VH'    ).setLogic( EM18VH     & physcond).setTriggerType( emTT )  
        LVL1MenuItem('L1_EM20VH'    ).setLogic( EM20VH     & physcond).setTriggerType( emTT )  
        LVL1MenuItem('L1_EM24VH'    ).setLogic( EM24VH     & physcond).setTriggerType( emTT )  
        LVL1MenuItem('L1_EM26VH'    ).setLogic( EM26VH     & physcond).setTriggerType( emTT )  
        LVL1MenuItem('L1_EM15VHI'   ).setLogic( EM15VHI    & physcond).setTriggerType( emTT )  # isolation not yet defined
        LVL1MenuItem('L1_EM18VHI'   ).setLogic( EM18VHI    & physcond).setTriggerType( emTT )  # isolation not yet defined
        LVL1MenuItem('L1_EM20VHI'   ).setLogic( EM20VHI    & physcond).setTriggerType( emTT )  # isolation not yet defined
        LVL1MenuItem('L1_EM22VHI'   ).setLogic( EM22VHI    & physcond).setTriggerType( emTT )  # isolation not yet defined
        LVL1MenuItem('L1_EM24VHI'   ).setLogic( EM24VHI    & physcond).setTriggerType( emTT )  # isolation not yet defined
        LVL1MenuItem('L1_EM26VHI'   ).setLogic( EM26VHI    & physcond).setTriggerType( emTT )  # isolation not yet defined
        LVL1MenuItem('L1_EM18VHIrel').setLogic( EM18VHIrel & physcond).setTriggerType( emTT )  # isolation not yet defined

        # 2xEM, 3xEM
        LVL1MenuItem('L1_2EM3'           ).setLogic( EM3.x(2)             & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_2EM4'           ).setLogic( EM4.x(2)             & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_2EM5'           ).setLogic( EM5.x(2)             & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_2EM8VH'         ).setLogic( EM8VH.x(2)           & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_2EM15VH'        ).setLogic( EM15VH.x(2)          & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM20V_EM6'      ).setLogic( EM6 & EM20V          & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_2EM3_EM6'       ).setLogic( EM3.x(2) & EM6       & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_2EM3_EM12'      ).setLogic( EM3.x(2) & EM12      & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_2EM4_EM8VH'     ).setLogic( EM4.x(2) & EM8VH     & physcond).setTriggerType( emTT )
        # 4xEM
        LVL1MenuItem('L1_3EM7VH_EM15VH'  ).setLogic( EM7VH.x(3) & EM15VH  & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_3EM7VH_EM15VHI' ).setLogic( EM7VH.x(3) & EM15VHI & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_3EM7V_EM15VH'   ).setLogic( EM7V.x(3)  & EM15VH  & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_3EM7V_EM15VHI'  ).setLogic( EM7V.x(3)  & EM15VHI & physcond).setTriggerType( emTT )
        # 6xEM
        LVL1MenuItem('L1_3EM7V_2EM8VH_EM15VH' ).setLogic( EM7V.x(3) & EM8VH.x(2) & EM15VH  & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_3EM7V_2EM8VH_EM15VHI').setLogic( EM7V.x(3) & EM8VH.x(2) & EM15VHI & physcond).setTriggerType( emTT )

        # EM and jet
        LVL1MenuItem('L1_EM15VH_FJ15.23ETA').setLogic( EM15VH & FJ1523ETA & physcond).setTriggerType( emTT )

        # HI
        LVL1MenuItem('L1_EM3_NZ'         ).setLogic( EM3      & Not(ZDC_AND) & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_2EM3_NZ'        ).setLogic( EM3.x(2) & Not(ZDC_AND) & physcond).setTriggerType( emTT )   

        LVL1MenuItem('L1_EM3_NL'         ).setLogic( EM3      & Not(LUCID_A) & Not(LUCID_C) & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_2EM3_NL'        ).setLogic( EM3.x(2) & Not(LUCID_A) & Not(LUCID_C) & physcond).setTriggerType( emTT )

        LVL1MenuItem('L1_EM3_MV'         ).setLogic( EM3      & Not(MBTS_A & MBTS_C) & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM3_MV_VTE50'   ).setLogic( EM3      & Not(MBTS_A & MBTS_C) & Not(TE50) & physcond).setTriggerType( emTT ) 

        LVL1MenuItem('L1_EM3_TE50'       ).setLogic( EM3      & TE50 & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM3_VTE50'      ).setLogic( EM3      & Not(TE50) & physcond).setTriggerType( emTT )

        LVL1MenuItem('L1_EM3_UNPAIRED_ISO'   ).setLogic(EM3 & unpaired_isocond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM3_UNPAIRED_NONISO').setLogic(EM3 & unpaired_nonisocond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM3_EMPTY'          ).setLogic(EM3 & cosmiccond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM3_FIRSTEMPTY'     ).setLogic(EM3 & firstempty).setTriggerType( emTT )

        LVL1MenuItem('L1_EM5_EMPTY'          ).setLogic(EM5 & cosmiccond).setTriggerType( emTT )

# MUON ctpid=[0x20;0x2f]
        LVL1MenuItem('L1_MU0'  ).setLogic( MU0  & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU4'  ).setLogic( MU4  & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU6'  ).setLogic( MU6  & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU10' ).setLogic( MU10 & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU11' ).setLogic( MU11 & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU15' ).setLogic( MU15 & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU20' ).setLogic( MU20 & physcond).setTriggerType(TT.rpcin)

        # 2xMU, 3xMU, 4xMU
        LVL1MenuItem('L1_2MU0'     ).setLogic( MU0.x(2)   & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_2MU4'     ).setLogic( MU4.x(2)   & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_2MU6'     ).setLogic( MU6.x(2)   & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_2MU10'    ).setLogic( MU10.x(2)  & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_2MU11'    ).setLogic( MU11.x(2)  & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_2MU20'    ).setLogic( MU20.x(2)  & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU4_MU10' ).setLogic( MU4 & MU10 & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU6_MU10' ).setLogic( MU6 & MU10 & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_3MU6'     ).setLogic( MU6.x(3)   & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_2MU0_MU6' ).setLogic( MU0.x(2) & MU6 & physcond).setTriggerType(TT.rpcin)

        # HI
        LVL1MenuItem('L1_MU0_NZ'   ).setLogic( MU0      & Not(ZDC_AND) & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_2MU0_NZ'  ).setLogic( MU0.x(2) & Not(ZDC_AND) & physcond).setTriggerType(TT.rpcin)   

        LVL1MenuItem('L1_MU0_NL'   ).setLogic( MU0      & Not(LUCID_A) & Not(LUCID_C) & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_2MU0_NL'  ).setLogic( MU0.x(2) & Not(LUCID_A) & Not(LUCID_C) & physcond).setTriggerType(TT.rpcin)                  
                            
        LVL1MenuItem('L1_MU0_MV'   ).setLogic( MU0      & Not(MBTS_A & MBTS_C) & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_2MU0_MV'  ).setLogic( MU0.x(2) & Not(MBTS_A & MBTS_C) & physcond).setTriggerType(TT.rpcin)
        
        LVL1MenuItem('L1_MU0_TE50' ).setLogic( MU0      & TE50 & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU4_TE50' ).setLogic( MU4      & TE50 & physcond).setTriggerType(TT.rpcin)
        
        LVL1MenuItem('L1_MU0_VTE50').setLogic( MU0      & Not(TE50) & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU4_VTE50').setLogic( MU4      & Not(TE50) & physcond).setTriggerType(TT.rpcin)

        LVL1MenuItem('L1_MU0_MV_VTE50').setLogic( MU0 & Not(MBTS_A & MBTS_C) & Not(TE50) & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU4_MV_VTE50').setLogic( MU4 & Not(MBTS_A & MBTS_C) & Not(TE50) & physcond).setTriggerType(TT.rpcin)

        LVL1MenuItem('L1_MU0_UNPAIRED_ISO'   ).setLogic(MU0 & unpaired_isocond   ).setTriggerType( TT.rpcin )
        LVL1MenuItem('L1_MU0_UNPAIRED_NONISO').setLogic(MU0 & unpaired_nonisocond).setTriggerType( TT.rpcin )
        LVL1MenuItem('L1_MU0_EMPTY'          ).setLogic(MU0 & cosmiccond).setTriggerType( TT.rpcin )
        LVL1MenuItem('L1_MU0_FIRSTEMPTY'     ).setLogic(MU0 & firstempty).setTriggerType( TT.rpcin )

        LVL1MenuItem('L1_MU4_UNPAIRED_ISO'   ).setLogic(MU4 & unpaired_isocond   ).setTriggerType( TT.rpcin )
        LVL1MenuItem('L1_MU4_UNPAIRED_NONISO').setLogic(MU4 & unpaired_nonisocond).setTriggerType( TT.rpcin )
        LVL1MenuItem('L1_MU4_EMPTY'          ).setLogic(MU4 & cosmiccond).setTriggerType( TT.rpcin )
        LVL1MenuItem('L1_MU4_FIRSTEMPTY'     ).setLogic(MU4 & firstempty).setTriggerType( TT.rpcin )

        LVL1MenuItem('L1_MU6_FIRSTEMPTY'     ).setLogic(MU4  & firstempty).setTriggerType( TT.rpcin ) 
        LVL1MenuItem('L1_MU11_EMPTY'         ).setLogic(MU11 & cosmiccond).setTriggerType( TT.rpcin ) 
        
        LVL1MenuItem('L1_2MU0_EMPTY'         ).setLogic(MU0.x(2) & cosmiccond).setTriggerType( TT.rpcin )
        LVL1MenuItem('L1_2MU4_EMPTY'         ).setLogic(MU4.x(2) & cosmiccond).setTriggerType( TT.rpcin ) 
        LVL1MenuItem('L1_2MU6_UNPAIRED_ISO'  ).setLogic(MU6.x(2) & unpaired_isocond).setTriggerType( TT.rpcin )
        LVL1MenuItem('L1_2MU6_UNPAIRED_NONISO').setLogic(MU6.x(2) & unpaired_nonisocond).setTriggerType( TT.rpcin )
        LVL1MenuItem('L1_2MU6_EMPTY'         ).setLogic(MU6.x(2) & cosmiccond).setTriggerType( TT.rpcin )
        LVL1MenuItem('L1_2MU6_FIRSTEMPTY'    ).setLogic(MU6.x(2) & firstempty).setTriggerType( TT.rpcin )
        
        # EM and MU
        LVL1MenuItem('L1_EM6_MU10'   ).setLogic( EM6        & MU10     & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_EM7V_MU10'  ).setLogic( EM7V       & MU10     & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_EM8_MU10'   ).setLogic( EM8        & MU10     & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_EM15V_MU4'  ).setLogic( EM15V      & MU4      & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_EM15_MU4'   ).setLogic( EM15       & MU4      & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_EM15VH_MU10').setLogic( EM15VH     & MU10     & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_EM3_MU20'   ).setLogic( EM3        & MU20     & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_2EM8VH_MU10').setLogic( EM8VH.x(2) & MU10     & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_EM8VH_2MU6' ).setLogic( EM8VH      & MU6.x(2) & physcond).setTriggerType(TT.rpcin)


        # TAU ctpid=[0x40:0x4f]
        LVL1MenuItem('L1_TAU6'  ).setLogic( HA6   & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU8'  ).setLogic( HA8   & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12' ).setLogic( HA12  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12I').setLogic( HA12I & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20' ).setLogic( HA20  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU30' ).setLogic( HA30  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU40' ).setLogic( HA40  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU60' ).setLogic( HA60  & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_TAU8_UNPAIRED_ISO'   ).setLogic( HA8   & unpaired_isocond ).setTriggerType( TT.calo ) 

        LVL1MenuItem('L1_TAU8_UNPAIRED_NONISO').setLogic( HA8   & unpaired_nonisocond ).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU8_EMPTY'          ).setLogic( HA8   & cosmiccond ).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU8_FIRSTEMPTY'     ).setLogic( HA8   & firstempty ).setTriggerType( TT.calo )
        
        # 3xTAU
        LVL1MenuItem('L1_2TAU12_TAU20'  ).setLogic( HA12.x(2)  & HA20 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_2TAU12I_TAU20' ).setLogic( HA12I.x(2) & HA20 & physcond).setTriggerType( TT.calo )

        # mixed tau
        LVL1MenuItem('L1_EM15VH_2TAU12'                ).setLogic( EM15VH  & HA12.x(2)          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15VH_2TAU20'                ).setLogic( EM15VH  & HA20.x(2)          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15VHI_2TAU12I'              ).setLogic( EM15VHI & HA12I.x(2)         & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15VH_TAU40_2TAU15'          ).setLogic( EM15VH  & HA40 & HA15.x(2)   & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15VHI_2TAU15_TAU40'         ).setLogic( EM15VHI & HA15.x(2)  & HA40  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15VHI_2TAU12I_FJ20'         ).setLogic( EM15VHI & HA12I.x(2) & FJ20  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15VH_2TAU12_J25_2J15_3J12'  ).setLogic( EM15VH  & HA12.x(2)  & J25 & J15.x(2) & J12.x(3) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15VHI_2TAU12I_J25_2J15_3J12').setLogic( EM15VHI & HA12I.x(2) & J25 & J15.x(2) & J12.x(3) & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_TAU12_MU10'       ).setLogic( HA12  & MU10          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12I_MU10'      ).setLogic( HA12I & MU10          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU20'       ).setLogic( MU10  & HA20          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20I_MU10'      ).setLogic( HA20I & MU10          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12I_MU10_J25'  ).setLogic( HA12I & MU10 & J25    & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU12_J25_2J12' ).setLogic( MU10 & HA12 & J25 & J12.x(2) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12I_MU10_FJ20' ).setLogic( HA12I & MU10 & FJ20   & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_TAU20_2TAU12_J25_2J20_3J12'   ).setLogic( HA20 & HA12.x(2)     & J25 & J20.x(2) & J12.x(3) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_2TAU12I_TAU20_J25_2J15_3J12'  ).setLogic( HA12I.x(2)   & HA20  & J25 & J15.x(2) & J12.x(3) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20_2J20_XE45'              ).setLogic( HA20    & J20.x(2)   & XE45 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15VHI_2TAU12I_XE35'         ).setLogic( EM15VHI & HA12I.x(2) & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12I_MU10_XE35'             ).setLogic( HA12I   & MU10  & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20_2TAU12_XE35'            ).setLogic( HA20    & HA12.x(2)  & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_2TAU12I_TAU20I_XE35'          ).setLogic( HA12I.x(2)   & HA20I & XE35 & physcond).setTriggerType( TT.calo )


        # JET ctpid=[0x60:0x7f]
        LVL1MenuItem('L1_J10'   ).setLogic( J10  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J15'   ).setLogic( J15  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J20'   ).setLogic( J20  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30'   ).setLogic( J30  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J40'   ).setLogic( J40  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J50'   ).setLogic( J50  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J75'   ).setLogic( J75  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J100'  ).setLogic( J100 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J120'  ).setLogic( J120 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J175'  ).setLogic( J175 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J250'  ).setLogic( J250 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J400'  ).setLogic( J400 & physcond).setTriggerType(TT.calo)

        LVL1MenuItem('L1_FJ10'  ).setLogic( FJ10 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_FJ20'  ).setLogic( FJ20 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_FJ30'  ).setLogic( FJ30 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_FJ50'  ).setLogic( FJ50 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_FJ75'  ).setLogic( FJ75 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_FJ100' ).setLogic( FJ100 & physcond).setTriggerType(TT.calo)


        LVL1MenuItem('L1_J10_UNPAIRED_ISO'   ).setLogic( J10 & unpaired_isocond   ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J10_UNPAIRED_NONISO').setLogic( J10 & unpaired_nonisocond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J10_EMPTY').setLogic( J10 & cosmiccond ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J10_FIRSTEMPTY').setLogic( J10 & firstempty ).setTriggerType(TT.calo)

        LVL1MenuItem('L1_J30_EMPTY').setLogic( J30 & cosmiccond ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30_UNPAIRED').setLogic( J30 & unpairedRcond ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30_FIRSTEMPTY').setLogic( J30 & firstempty ).setTriggerType(TT.calo)

        LVL1MenuItem('L1_FJ10_EMPTY').setLogic( FJ10 & cosmiccond ).setTriggerType(TT.calo)
        
        # multi jet
        LVL1MenuItem('L1_2FJ25'  ).setLogic( JF25 & JB25 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_2J15'   ).setLogic( J15.x(2)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_3J15'   ).setLogic( J15.x(3)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_3J20'   ).setLogic( J20.x(3)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_3J50'   ).setLogic( J50.x(3)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_3J75'   ).setLogic( J75.x(3)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_4J20'   ).setLogic( J20.x(4)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_4J30'   ).setLogic( J30.x(4)    & physcond).setTriggerType(TT.calo)

        LVL1MenuItem('L1_4J17.ETA22' ).setLogic( J17ETA22.x(4) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_3J25.ETA22' ).setLogic( J25ETA22.x(3) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_3J15.ETA24' ).setLogic( J15ETA24.x(3) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_5J15.ETA24' ).setLogic( J15ETA24.x(5) & physcond).setTriggerType(TT.calo)

        if ('Physics_HI_v3' in TriggerFlags.triggerMenuSetup()):
            LVL1MenuItem('L1_ZB', ctpid=240).setLogic(ZB_J75 & physcond).setTriggerType(TT.zerobs)
        
        # combined jet - xe
        #LVL1MenuItem('L1_J40_XE50').setLogic( J75 & XE40 & physcond).setTriggerType(TT.calo)#CB ERROR???
        LVL1MenuItem('L1_J40_XE50').setLogic( J40 & XE50 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J75_XE40').setLogic( J75 & XE40 & physcond).setTriggerType(TT.calo)

        # HI
        LVL1MenuItem('L1_J15_NZ' ).setLogic( J15      & Not(ZDC_AND) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_2J15_NZ').setLogic( J15.x(2) & Not(ZDC_AND) & physcond).setTriggerType(TT.calo)   

        LVL1MenuItem('L1_J15_NL' ).setLogic( J15      & Not(LUCID_A) & Not(LUCID_C) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_2J15_NL').setLogic( J15.x(2) & Not(LUCID_A) & Not(LUCID_C) & physcond).setTriggerType(TT.calo)
        
        # XE
        LVL1MenuItem('L1_XE35').setLogic( XE35 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE45').setLogic( XE45 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE50').setLogic( XE50 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE60').setLogic( XE60 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE70').setLogic( XE70 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE80').setLogic( XE80 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE90').setLogic( XE90 & physcond).setTriggerType(TT.calo)


        # TE
        LVL1MenuItem('L1_TE5' ).setLogic( TE5  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE10').setLogic( TE10 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE20').setLogic( TE20 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE30').setLogic( TE30 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE40').setLogic( TE40 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE50').setLogic( TE50 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE2000').setLogic( TE2000 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE4000').setLogic( TE4000 & physcond).setTriggerType(TT.calo)

        LVL1MenuItem('L1_TE5_NZ').setLogic( TE5 & Not(ZDC_AND) & physcond).setTriggerType(TT.calo) 
        
        # MBTS
        MBTS_1   = MBTS_A | MBTS_C
        MBTS_2   = (MBTS_A.x(2) | MBTS_C.x(2) | MBTS_A) & (MBTS_A.x(2) | MBTS_C.x(2) | MBTS_C)
        MBTS_1_1 = MBTS_A & MBTS_C
        MBTS_2_2 = MBTS_A.x(2) & MBTS_C.x(2)
        MBTS_3_3 = MBTS_A.x(3) & MBTS_C.x(3)
        MBTS_4_4 = MBTS_A.x(4) & MBTS_C.x(4)
        
        LVL1MenuItem('L1_MBTS_1'             ).setLogic( MBTS_1   & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2'             ).setLogic( MBTS_2   & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_1_1'           ).setLogic( MBTS_1_1 & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2_2'           ).setLogic( MBTS_2_2 & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_3_3'           ).setLogic( MBTS_3_3 & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_4_4'           ).setLogic( MBTS_4_4 & physcond ).setTriggerType(TT.minb)

        
        LVL1MenuItem('L1_MBTS_1_UNPAIRED_ISO'  ).setLogic( MBTS_1   & unpaired_isocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2_UNPAIRED_ISO'  ).setLogic( MBTS_2   & unpaired_isocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_1_1_UNPAIRED_ISO').setLogic( MBTS_1_1 & unpaired_isocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2_2_UNPAIRED_ISO').setLogic( MBTS_2_2 & unpaired_isocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_3_3_UNPAIRED_ISO').setLogic( MBTS_3_3 & unpaired_isocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_4_4_UNPAIRED_ISO').setLogic( MBTS_3_3 & unpaired_isocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_1_UNPAIRED_NONISO').setLogic( MBTS_1 & unpaired_nonisocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2_UNPAIRED_NONISO').setLogic( MBTS_2 & unpaired_nonisocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_1_EMPTY'       ).setLogic( MBTS_1   & cosmiccond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2_EMPTY'       ).setLogic( MBTS_2   & cosmiccond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_1_1_EMPTY'     ).setLogic( MBTS_1_1 & cosmiccond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2_2_EMPTY'     ).setLogic( MBTS_2_2 & cosmiccond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_3_3_EMPTY'     ).setLogic( MBTS_3_3 & cosmiccond ).setTriggerType(TT.minb)
        
        LVL1MenuItem('L1_MBTS_1_1_VTE50'     ).setLogic( MBTS_1_1  & Not(TE50) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2_2_VTE50'     ).setLogic( MBTS_2_2  & Not(TE50) & physcond).setTriggerType(TT.minb)

        LVL1MenuItem('L1_MBTSA0' ).setLogic( MBTS_A0 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSA1' ).setLogic( MBTS_A1 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSA2' ).setLogic( MBTS_A2 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSA3' ).setLogic( MBTS_A3 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSA4' ).setLogic( MBTS_A4 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSA5' ).setLogic( MBTS_A5 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSA6' ).setLogic( MBTS_A6 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSA7' ).setLogic( MBTS_A7 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSA8' ).setLogic( MBTS_A8 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSA10').setLogic( MBTS_A10 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSA12').setLogic( MBTS_A12 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSA14').setLogic( MBTS_A14 & physcond).setTriggerType(TT.minb)
        # L1_MBTSA/C 9, 11, 13, 15 are removed in Run 2

        LVL1MenuItem('L1_MBTSC0' ).setLogic( MBTS_C0 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSC1' ).setLogic( MBTS_C1 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSC2' ).setLogic( MBTS_C2 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSC3' ).setLogic( MBTS_C3 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSC4' ).setLogic( MBTS_C4 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSC5' ).setLogic( MBTS_C5 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSC6' ).setLogic( MBTS_C6 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSC7' ).setLogic( MBTS_C7 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSC8' ).setLogic( MBTS_C8 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSC10').setLogic( MBTS_C10 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSC12').setLogic( MBTS_C12 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTSC14').setLogic( MBTS_C14 & physcond).setTriggerType(TT.minb)
        
        # ZDC
        ZDC_A_C = ZDC_A & ZDC_C
        
        LVL1MenuItem('L1_ZDC'               ).setLogic((ZDC_A | ZDC_C) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_A'             ).setLogic( ZDC_A & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_C'             ).setLogic( ZDC_C & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_AND'           ).setLogic( ZDC_AND & physcond).setTriggerType(TT.minb) 
        LVL1MenuItem('L1_ZDC_A_C'           ).setLogic( ZDC_A_C & physcond).setTriggerType(TT.minb)

        LVL1MenuItem('L1_ZDC_A_C_OVERLAY'   ).setLogic( ZDC_A_C & physcond).setTriggerType(TT.zerobs)     
        
        LVL1MenuItem('L1_ZDC_AND_VTE50'     ).setLogic( ZDC_AND & Not(TE50) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_A_C_VTE50'     ).setLogic( ZDC_A_C & Not(TE50) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_A_C_TE50'      ).setLogic( ZDC_A_C & TE50 & physcond).setTriggerType(TT.minb)

        LVL1MenuItem('L1_ZDC_MBTS_1'        ).setLogic((ZDC_A | ZDC_C) & MBTS_1 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_MBTS_2'        ).setLogic((ZDC_A | ZDC_C) & MBTS_2 & physcond).setTriggerType(TT.minb)

        LVL1MenuItem('L1_ZDC_MBTS_1_1'      ).setLogic((ZDC_A | ZDC_C) & MBTS_1_1 & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_MBTS_2_2'      ).setLogic((ZDC_A | ZDC_C) & MBTS_2_2 & physcond).setTriggerType(TT.minb)

        LVL1MenuItem('L1_ZDC_EMPTY'           ).setLogic( (ZDC_A | ZDC_C) & cosmiccond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_UNPAIRED_ISO'    ).setLogic( (ZDC_A | ZDC_C) & unpaired_isocond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_UNPAIRED_NONISO' ).setLogic( (ZDC_A | ZDC_C) & unpaired_nonisocond ).setTriggerType(TT.minb)

        LVL1MenuItem('L1_ZDC_AND_EMPTY'           ).setLogic( ZDC_AND & cosmiccond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_AND_UNPAIRED_ISO'    ).setLogic( ZDC_AND & unpaired_isocond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_AND_UNPAIRED_NONISO' ).setLogic( ZDC_AND & unpaired_nonisocond ).setTriggerType(TT.minb)

        LVL1MenuItem('L1_ZDC_A_C_EMPTY'           ).setLogic( ZDC_A_C & cosmiccond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_A_C_UNPAIRED_ISO'    ).setLogic( ZDC_A_C & unpaired_isocond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_A_C_UNPAIRED_NONISO' ).setLogic( ZDC_A_C & unpaired_nonisocond ).setTriggerType(TT.minb)
        
        # VDM
        LVL1MenuItem('L1_ZDC_A_C_BGRP7'     ).setLogic( ZDC_A_C & bgrp7cond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_BGRP7'       ).setLogic( (LUCID_A | LUCID_C) & bgrp7cond).setTriggerType(TT.minb)
        
        # LUCID
        LVL1MenuItem('L1_LUCID_A'           ).setLogic( LUCID_A             & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_C'           ).setLogic( LUCID_C             & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_A_C',        ).setLogic( LUCID_A & LUCID_C   & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID'             ).setLogic((LUCID_A | LUCID_C)  & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_COMM'        ).setLogic( NIMDIR5             & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_EMPTY'       ).setLogic((LUCID_A | LUCID_C)  & cosmiccond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_A_C_EMPTY'   ).setLogic( LUCID_A & LUCID_C   & cosmiccond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_UNPAIRED_ISO').setLogic((LUCID_A | LUCID_C)  & unpaired_isocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_COMM_UNPAIRED_ISO' ).setLogic( NIMDIR5       & unpaired_isocond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_A_C_UNPAIRED_ISO'  ).setLogic( LUCID_A & LUCID_C & unpaired_isocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_A_C_UNPAIRED_NONISO').setLogic(LUCID_A & LUCID_C & unpaired_nonisocond).setTriggerType(TT.minb)
        
        # BCM
        LVL1MenuItem('L1_BCM_Wide'                   ).setLogic( BCM_Wide & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_BCM_Wide_BGRP0'             ).setLogic( BCM_Wide & BGRP0 ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_BCM_Wide_EMPTY'             ).setLogic( BCM_Wide & cosmiccond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_BCM_Wide_UNPAIRED_ISO'      ).setLogic( BCM_Wide & unpaired_isocond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_BCM_Wide_UNPAIRED_NONISO'   ).setLogic( BCM_Wide & unpaired_nonisocond ).setTriggerType(TT.minb)

        LVL1MenuItem('L1_BCM_HT_BGRP0'               ).setLogic( BCM_Comb.x(7) & BGRP0).setTriggerType(TT.minb)
        LVL1MenuItem('L1_BCM_AC_CA_BGRP0'            ).setLogic((BCM_AtoC|BCM_CtoA) & BGRP0).setTriggerType(TT.minb) 
        LVL1MenuItem('L1_BCM_AC_CA_UNPAIRED_ISO'     ).setLogic((BCM_AtoC|BCM_CtoA)&unpaired_isocond).setTriggerType(TT.minb)
        
        # RANDOM
        LVL1MenuItem('L1_RD0_FILLED'         ).setLogic( RNDM0 & physcond           ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD0_EMPTY'          ).setLogic( RNDM0 & cosmiccond         ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD0_UNPAIRED_ISO'   ).setLogic( RNDM0 & unpaired_isocond   ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD0_FIRSTEMPTY'     ).setLogic( RNDM0 & firstempty         ).setTriggerType(TT.rand)

        LVL1MenuItem('L1_RD1_FILLED'         ).setLogic( RNDM1 & physcond           ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD1_EMPTY'          ).setLogic( RNDM1 & cosmiccond         ).setTriggerType(TT.rand)

        # BGRP
        LVL1MenuItem('L1_BGRP0').setLogic( BGRP0 ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_BGRP1').setLogic( BGRP0 & BGRP1 ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_BGRP7').setLogic( BGRP0 & BGRP7 ).setTriggerType(TT.rand)

        LVL1MenuItem('L1_BPTX0_BGRP0', ctpid=0xf1).setLogic(BPTX0 & BGRP0).setTriggerType(TT.rand)
        LVL1MenuItem('L1_BPTX1_BGRP0', ctpid=0xf2).setLogic(BPTX1 & BGRP0).setTriggerType(TT.rand)   
        
        # lumi measurements
        LVL1MenuItem('L1_MLZ_A', ctpid=-1).setLogic( (MBTS_A|ZDC_A|LUCID_A) & physcond).setTriggerType(TT.minb) 
        LVL1MenuItem('L1_MLZ_C', ctpid=-1).setLogic( (MBTS_C|ZDC_C|LUCID_C) & physcond).setTriggerType(TT.minb) 
        LVL1MenuItem('L1_MBLZ',  ctpid=-1).setLogic( ( (MBTS_A|ZDC_A|LUCID_A) & (MBTS_C|ZDC_C|LUCID_C) | BCM_Wide) & physcond ).setTriggerType(TT.minb)  

        LVL1MenuItem('L1_CALREQ0', ctpid=0xfd).setLogic( NIM28 & calibcond).setTriggerType(TT.calreq0)
        LVL1MenuItem('L1_CALREQ1', ctpid=0xfe).setLogic( NIM29 & calibcond).setTriggerType(TT.calreq1)
        LVL1MenuItem('L1_CALREQ2', ctpid=0xff).setLogic( NIM30 & calibcond).setTriggerType(TT.calreq2)   

        # Topo items (keep them all together)
        try:
            LVL1MenuItem('L1_TOPO_JetDPhi', ctpid=0xfd).setLogic( JetDeltaPhiNarrow & physcond)
        except Exception, ex:
            print "L1Topo Items creation failed",ex



        
        # =======================================================
        # Partition 1
        LVL1MenuItem.currentPartition = 2
        
        #        LVL1MenuItem('L1_CALREQ0_P1', ctpid=480).setLogic( NIM28 & calibcond1).setTriggerType( 0x0f & TT.calreq0 )
        #        LVL1MenuItem('L1_CALREQ1_P1', ctpid=481).setLogic( NIM29 & calibcond1).setTriggerType( 0x0f & TT.calreq1 )
        #        LVL1MenuItem('L1_CALREQ2_P1', ctpid=482).setLogic( NIM30 & calibcond1).setTriggerType( 0x0f & TT.calreq2 )

        LVL1MenuItem.currentPartition = 3


        LVL1MenuItem.currentPartition = 1

        LVL1MenuItem.l1configForRegistration = None
