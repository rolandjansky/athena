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
        LVL1MenuItem('L1_EM6'       ).setLogic( EM6        & physcond).setTriggerType( emTT )
        LVL1MenuItem('L1_EM12'      ).setLogic( EM12       & physcond).setTriggerType( emTT )
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



        # MUON ctpid=[0x20;0x2f]
        LVL1MenuItem('L1_MU4'  ).setLogic( MU4  & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU6'  ).setLogic( MU6  & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU10' ).setLogic( MU10 & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU11' ).setLogic( MU11 & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU15' ).setLogic( MU15 & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU20' ).setLogic( MU20 & physcond).setTriggerType(TT.rpcin)

        # 2xMU, 3xMU, 4xMU
        LVL1MenuItem('L1_2MU4'     ).setLogic( MU4.x(2)   & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_2MU6'     ).setLogic( MU6.x(2)   & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_2MU10'    ).setLogic( MU10.x(2)  & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU4_MU10' ).setLogic( MU4 & MU10 & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_MU6_MU10' ).setLogic( MU6 & MU10 & physcond).setTriggerType(TT.rpcin)
        LVL1MenuItem('L1_3MU6'     ).setLogic( MU6.x(3)   & physcond).setTriggerType(TT.rpcin)


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
        LVL1MenuItem('L1_J20'   ).setLogic( J20  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30'   ).setLogic( J30  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J40'   ).setLogic( J40  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J50'   ).setLogic( J50  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J75'   ).setLogic( J75  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J100'  ).setLogic( J100 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J120'  ).setLogic( J120 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J400'  ).setLogic( J400 & physcond).setTriggerType(TT.calo)

        LVL1MenuItem('L1_FJ20'  ).setLogic( FJ20  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_FJ30'  ).setLogic( FJ30  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_FJ50'  ).setLogic( FJ50  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_FJ75'  ).setLogic( FJ75  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_FJ100' ).setLogic( FJ100 & physcond).setTriggerType(TT.calo)

        # multi jet
        LVL1MenuItem('L1_2FJ25'  ).setLogic( JF25 & JB25 & physcond).setTriggerType(TT.calo)
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

        # combined jet - xe
        #LVL1MenuItem('L1_J40_XE50').setLogic( J75 & XE40 & physcond).setTriggerType(TT.calo)#CB ERROR???
        LVL1MenuItem('L1_J40_XE50').setLogic( J40 & XE50 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J75_XE40').setLogic( J75 & XE40 & physcond).setTriggerType(TT.calo)

        # XE
        LVL1MenuItem('L1_XE35').setLogic( XE35 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE45').setLogic( XE45 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE50').setLogic( XE50 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE60').setLogic( XE60 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE70').setLogic( XE70 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE80').setLogic( XE80 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE90').setLogic( XE90 & physcond).setTriggerType(TT.calo)


        # TE
        LVL1MenuItem('L1_TE20').setLogic( TE20 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE30').setLogic( TE30 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE40').setLogic( TE40 & physcond).setTriggerType(TT.calo)

        
        # MBTS
        MBTS_1   = MBTS_A | MBTS_C
        MBTS_2   = (MBTS_A.x(2) | MBTS_C.x(2) | MBTS_A) & (MBTS_A.x(2) | MBTS_C.x(2) | MBTS_C)
        MBTS_1_1 = MBTS_A & MBTS_C
        LVL1MenuItem('L1_MBTS_1'             ).setLogic( MBTS_1   & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2'             ).setLogic( MBTS_2   & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_1_1'           ).setLogic( MBTS_1_1 & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2_UNPAIRED_ISO').setLogic( MBTS_2   & unpaired_isocond).setTriggerType(TT.minb)

        # LUCID
        LVL1MenuItem('L1_LUCID'             ).setLogic( (LUCID_A | LUCID_C) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_UNPAIRED_ISO').setLogic( (LUCID_A | LUCID_C) & unpaired_isocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_EMPTY'       ).setLogic( (LUCID_A | LUCID_C) & cosmiccond).setTriggerType(TT.minb)

        # RANDOM
        LVL1MenuItem('L1_RD0_FILLED'         ).setLogic( RNDM0 & physcond           ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD0_EMPTY'          ).setLogic( RNDM0 & cosmiccond         ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD0_UNPAIRED_ISO'   ).setLogic( RNDM0 & unpaired_isocond   ).setTriggerType(TT.rand)


        # =======================================================

        # Partition 1
        LVL1MenuItem.currentPartition = 2
        
        #        LVL1MenuItem('L1_CALREQ0_P1', ctpid=480).setLogic( NIM28 & calibcond1).setTriggerType( 0x0f & TT.calreq0 )
        #        LVL1MenuItem('L1_CALREQ1_P1', ctpid=481).setLogic( NIM29 & calibcond1).setTriggerType( 0x0f & TT.calreq1 )
        #        LVL1MenuItem('L1_CALREQ2_P1', ctpid=482).setLogic( NIM30 & calibcond1).setTriggerType( 0x0f & TT.calreq2 )

        LVL1MenuItem.currentPartition = 3


        LVL1MenuItem.currentPartition = 1

        LVL1MenuItem.l1configForRegistration = None
