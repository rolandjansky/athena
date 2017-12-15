#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TriggerJobOpts.TriggerFlags import TriggerFlags

from AthenaCommon.Logging import logging
log = logging.getLogger('TriggerMenu.Lvl1.py')


# The trigger types
from TriggerMenu.l1.TriggerTypeDefRun1 import TT
from TriggerMenu.l1.Limits import Limits
from TriggerMenu.l1.Logic import Not
from TriggerMenu.l1.Lvl1MenuItems import LVL1MenuItem

class ItemDef:
    def __init__(self):
        """Constructor"""
        pass
    
    #----------------------------------------------------------------------
    # LVL1- thresholds and items
    #----------------------------------------------------------------------
    @staticmethod
    def registerItems(tm):
        """Register LVL1 thresholds for further use"""

        # register all thresholds ...
        from ThresholdDefRun1 import ThresholdDef
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

        for i in range(2):
            exec("PCLK%i = Logic(Lvl1InternalTrigger('PCLK%i'))" % (i,i))


        LVL1MenuItem.l1configForRegistration = tm  # from now on all new LVL1MenuItems are registered to tm

        # Setting up bunch group and BPTX conditions
        bgrpcond            = BGRP0 & BGRP1                                                 # noqa: F821
        physcond            = BGRP0 & BGRP1                                                 # noqa: F821
        calibcond           = BGRP0 & BGRP2                                                 # noqa: F821
        cosmiccond          = BGRP0 & BGRP3                                                 # noqa: F821
        unpaired_isocond    = BGRP0 & BGRP4 # unpaired isolated (satellite bunches)         # noqa: F821
        unpaired_nonisocond = BGRP0 & BGRP5 # unpaired non-isolated (parasitic bunches)     # noqa: F821
        firstempty          = BGRP0 & BGRP6                                                 # noqa: F821
        unpairedRcond       = BGRP0 & BGRP7 # unpaired beams 1 oR beam 2                    # noqa: F821
        bgrp7cond           = BGRP0 & BGRP7 # No unpaired anymore                           # noqa: F821


        LVL1MenuItem.currentPartition = 1

        # /gamma ctpid=[0:0x1f]

        emTT = TT.calo | TT.caloem
        
        LVL1MenuItem('L1_EM3',    ctpid=0x1 ).setLogic( EM3.x(1)    & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM4',    ctpid=0x54).setLogic( EM4.x(1)    & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM5',    ctpid=-1  ).setLogic( EM5.x(1)    & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM6',    ctpid=-1  ).setLogic( EM6.x(1)    & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM7',    ctpid=0x1 ).setLogic( EM7.x(1)    & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM7V',   ctpid=-1  ).setLogic( EM7V.x(1)   & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM8VH',  ctpid=-1  ).setLogic( EM8VH.x(1)  & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM10',   ctpid=0x2 ).setLogic( EM10.x(1)   & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM10VH', ctpid=-1  ).setLogic( EM10VH.x(1) & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM12',   ctpid=0x2 ).setLogic( EM12.x(1)   & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM14',   ctpid=0x5 ).setLogic( EM14.x(1)   & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM14VH', ctpid=-1  ).setLogic( EM14VH.x(1) & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM15V',  ctpid=-1  ).setLogic( EM15V.x(1)  & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM15VH', ctpid=-1  ).setLogic( EM15VH.x(1) & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM16',   ctpid=0x5 ).setLogic( EM16.x(1)   & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM16VH', ctpid=-1  ).setLogic( EM16VH.x(1) & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM16V',  ctpid=-1  ).setLogic( EM16V.x(1)  & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM18VH', ctpid=-1  ).setLogic( EM18VH.x(1) & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM20V',  ctpid=-1  ).setLogic( EM20V.x(1)  & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM24VHI', ctpid=-1  ).setLogic( EM24VHI.x(1) & physcond).setTriggerType( emTT )  # noqa: F821
        LVL1MenuItem('L1_EM30',   ctpid=0x8 ).setLogic( EM30.x(1)   & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM50',   ctpid=0x8 ).setLogic( EM50.x(1)   & physcond).setTriggerType( emTT )    # noqa: F821


        # special bgrp
        LVL1MenuItem('L1_EM30_BGRP7',  ctpid=-1).setLogic( EM30.x(1) & bgrp7cond ).setTriggerType( emTT ) # noqa: F821
        
        # 2xEM
        LVL1MenuItem('L1_2EM3',    ctpid=0xa ).setLogic( EM3.x(2)    & physcond).setTriggerType( emTT )   # noqa: F821
        LVL1MenuItem('L1_2EM5',    ctpid=0xb ).setLogic( EM5.x(2)    & physcond).setTriggerType( emTT )   # noqa: F821
        LVL1MenuItem('L1_2EM6',    ctpid=-1  ).setLogic( EM6.x(2)    & physcond).setTriggerType( emTT )   # noqa: F821
        LVL1MenuItem('L1_2EM8VH',  ctpid=-1  ).setLogic( EM8VH.x(2)  & physcond).setTriggerType( emTT )   # noqa: F821
        LVL1MenuItem('L1_2EM10VH', ctpid=-1  ).setLogic( EM10VH.x(2) & physcond).setTriggerType( emTT )   # noqa: F821
        LVL1MenuItem('L1_2EM12',   ctpid=0xf ).setLogic( EM12.x(2)   & physcond).setTriggerType( emTT )   # noqa: F821
        LVL1MenuItem('L1_2EM15V',  ctpid=-1 ).setLogic( EM15V.x(2)  & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_2EM15VH', ctpid=0xf ).setLogic( EM15VH.x(2) & physcond).setTriggerType( emTT )   # noqa: F821



        # 3xEM
        LVL1MenuItem('L1_2EM3_EM6'   ).setLogic( EM3.x(2)& EM6.x(1) & physcond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM7V_2EM3'   ).setLogic(EM7V.x(1)& EM3.x(2) & physcond).setTriggerType( emTT )   # noqa: F821
        LVL1MenuItem('L1_2EM3_EM12'  ).setLogic( EM3.x(2)& EM12.x(1) & physcond).setTriggerType( emTT )   # noqa: F821
        LVL1MenuItem('L1_2EM6_EM16VH').setLogic( EM6.x(2)& EM16VH.x(1) & physcond).setTriggerType( emTT ) # noqa: F821
        LVL1MenuItem('L1_3EM6',  ctpid=-1  ).setLogic( EM6.x(3)  & physcond).setTriggerType(TT.phys).setTriggerType( emTT )   # noqa: F821
        LVL1MenuItem('L1_EM15VH_3EM7V'  ).setLogic( EM15VH.x(1) & EM7V.x(3) & physcond).setTriggerType( emTT )   # noqa: F821

        # 4xEM
        LVL1MenuItem('L1_3EM6_EM14VH').setLogic( EM6.x(3) & EM14VH.x(1)& physcond).setTriggerType( emTT )   # noqa: F821
        LVL1MenuItem('L1_2EM12_EM16V').setLogic( EM12.x(2)& EM16V.x(1) & physcond).setTriggerType( emTT )   # noqa: F821

        
        # MUON ctpid=[0x20;0x2f]

        LVL1MenuItem('L1_MU0',  ctpid=0x20).setLogic( MU0.x(1)  & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU4',  ctpid=-1  ).setLogic( MU4.x(1)  & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU6',  ctpid=0x21).setLogic( MU6.x(1)  & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU10', ctpid=0x22).setLogic( MU10.x(1) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU11', ctpid=0x23).setLogic( MU11.x(1) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU15', ctpid=-1  ).setLogic( MU15.x(1) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU20', ctpid=0x24).setLogic( MU20.x(1) & physcond).setTriggerType(TT.rpcin)    # noqa: F821

        # 2xMU, 3xMU, 4xMU
        LVL1MenuItem('L1_2MU0',  ctpid=0x26).setLogic( MU0.x(2)  & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU4',  ctpid=-1  ).setLogic( MU4.x(2)  & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU6',  ctpid=0x27).setLogic( MU6.x(2)  & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU10', ctpid=0x28).setLogic( MU10.x(2) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU11', ctpid=0x28).setLogic( MU11.x(2) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU20', ctpid=0x2a).setLogic( MU20.x(2) & physcond)    # noqa: F821
        LVL1MenuItem('L1_3MU4',  ctpid=-1  ).setLogic( MU4.x(3)  & physcond)    # noqa: F821
        LVL1MenuItem('L1_3MU6',  ctpid=0x2b).setLogic( MU6.x(3)  & physcond)    # noqa: F821
        LVL1MenuItem('L1_2MU4_MU6'         ).setLogic( MU4.x(2) & MU6.x(1)  & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU6_MU10'        ).setLogic( MU6.x(2) & MU10.x(1) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_3MU4_MU6'         ).setLogic( MU4.x(3) & MU6.x(1)  & physcond).setTriggerType(TT.rpcin)    # noqa: F821

        LVL1MenuItem('L1_2MU4_XE40', ctpid=-1  ).setLogic( MU4.x(2) &  XE40.x(1)  & physcond).setTriggerType(TT.calo|TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU4_XE30', ctpid=-1  ).setLogic( MU4.x(2) &  XE30.x(1) & physcond).setTriggerType(TT.calo|TT.rpcin)    # noqa: F821

        # barrel 
        LVL1MenuItem('L1_MUB'            ).setLogic( NIM_MUB.x(1) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MUE'            ).setLogic( NIM_MUE.x(1) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU4_BARRELONLY').setLogic( ( MU4.x(2) & Not(NIM_MUE.x(1)) |  MU4.x(3)) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU4_MU6_BARREL').setLogic( MU4.x(2) & NIM_MUB.x(1) & MU6.x(1) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU4_BARREL'    ).setLogic( MU4.x(2) & NIM_MUB.x(1) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU6_BARRELONLY').setLogic( (MU6.x(2)&Not(NIM_MUE.x(1))| MU6.x(3)) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU6_BARREL'    ).setLogic( MU6.x(2) & NIM_MUB.x(1) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        


        # TAU ctpid=[0x40:0x4f]

        # 1xTAU
        LVL1MenuItem('L1_TAU3',   ctpid=0x40).setLogic( HA3.x(1)   & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_TAU6',   ctpid=-1  ).setLogic( HA6.x(1)   & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_TAU8',   ctpid=0x43).setLogic( HA8.x(1)   & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_TAU11',  ctpid=0x44).setLogic( HA11.x(1)  & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_TAU11I', ctpid=0x44).setLogic( HA11I.x(1) & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_TAU12',  ctpid=-1  ).setLogic( HA12.x(1)  & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_TAU15',  ctpid=-1  ).setLogic( HA15.x(1)  & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_TAU15I'            ).setLogic( HA15I.x(1) & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_TAU20',  ctpid=0x46).setLogic( HA20.x(1)  & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_TAU40',  ctpid=0x49).setLogic( HA40.x(1)  & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_TAU60',  ctpid=-1  ).setLogic( HA60.x(1)  & physcond).setTriggerType( TT.calo )    # noqa: F821

        # 2xTAU
        LVL1MenuItem('L1_2TAU11', ctpid=0x4c ).setLogic( HA11.x(2)  & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_2TAU11I'            ).setLogic( HA11I.x(2) & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_2TAU15'             ).setLogic( HA15.x(2)  & physcond).setTriggerType( TT.calo )    # noqa: F821

        # mixed tau
        LVL1MenuItem('L1_EM15VH_TAU40_2TAU15'          ).setLogic( EM15VH  & HA40 & HA15.x(2)   & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_EM15VH_2TAU12_J25_2J15_3J12'  ).setLogic( EM15VH  & HA12.x(2)  & J25 & J15.x(2) & J12.x(3) & physcond).setTriggerType( TT.calo )    # noqa: F821

        LVL1MenuItem('L1_MU10_TAU20'       ).setLogic( MU10  & HA20          & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_MU10_TAU12_J25_2J12' ).setLogic( MU10 & HA12 & J25 & J12.x(2) & physcond).setTriggerType( TT.calo )    # noqa: F821

        LVL1MenuItem('L1_TAU20_2TAU12_J25_2J20_3J12'   ).setLogic( HA20 & HA12.x(2)     & J25 & J20.x(2) & J12.x(3) & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_TAU20_2J20_XE45'              ).setLogic( HA20    & J20.x(2)   & XE45 & physcond).setTriggerType( TT.calo )    # noqa: F821
        LVL1MenuItem('L1_TAU20_2TAU12_XE35'            ).setLogic( HA20    & HA12.x(2)  & XE35 & physcond).setTriggerType( TT.calo )    # noqa: F821

        # EMPTY
        LVL1MenuItem('L1_2TAU20'    ).setLogic( HA20.x(2) & physcond).setTriggerType(TT.calo)    # noqa: F821


        # JET ctpid=[0x60:0x7f]

        LVL1MenuItem('L1_J5',      ctpid=0x60).setLogic( J5.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J5_win6', ctpid=0x60).setLogic( J5_win6.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J10',     ctpid=0x61).setLogic( J10.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J15'                ).setLogic( J15.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J18',     ctpid=0x62).setLogic( J18.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J20',     ctpid=-1  ).setLogic( J20.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J30'                ).setLogic( J30.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J40'                ).setLogic( J40.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J50'                ).setLogic( J50.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J75'                ).setLogic( J75.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J100',    ctpid=-1  ).setLogic( J100.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J350',    ctpid=-1  ).setLogic( J350.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821

        LVL1MenuItem('L1_J15.ETA24', ctpid=-1).setLogic(J15ETA24.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821

        LVL1MenuItem('L1_J15.23ETA49' ).setLogic( (JF1523ETA49  | JB1523ETA49  ) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J20.32ETA49' ).setLogic( (JF2032ETA49  | JB2032ETA49  ) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J25.32ETA49' ).setLogic( (JF2532ETA49  | JB2532ETA49  ) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J100.32ETA49').setLogic( (JF10032ETA49 | JB10032ETA49 ) & physcond).setTriggerType(TT.calo)    # noqa: F821

        # pPb
        LVL1MenuItem('L1_J5_TE90',  ctpid=0x60).setLogic( J5.x(1)& TE90.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_2J5_TE90', ctpid=0x60).setLogic( J5.x(2)& TE90.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821

        # Multi-jet
        LVL1MenuItem('L1_2J5', ctpid=0x68 ).setLogic( J5.x(2)  & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_2J10'            ).setLogic( J10.x(2) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_3J10', ctpid=0x7f).setLogic( J10.x(3) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_3J15'            ).setLogic( J15.x(3) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_3J50', ctpid=0x7f).setLogic( J50.x(3) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_3J75', ctpid=-1  ).setLogic( J75.x(3) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_3J20', ctpid=-1  ).setLogic( J20.x(3) & physcond).setTriggerType(TT.calo) # new jet threshold  # noqa: F821
        LVL1MenuItem('L1_4J10', ctpid=0x6e).setLogic( J10.x(4) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_4J15'            ).setLogic( J15.x(4) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_4J20', ctpid=-1  ).setLogic( J20.x(4) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_5J10', ctpid=-1  ).setLogic( J10.x(5) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_5J15'            ).setLogic( J15.x(5) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_5J20', ctpid=-1  ).setLogic( J20.x(5) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_6J10', ctpid=-1  ).setLogic( J10.x(6) & physcond).setTriggerType(TT.calo)    # noqa: F821
        
        LVL1MenuItem('L1_3J15.ETA24' ).setLogic( J15ETA24.x(3) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_5J15.ETA24' ).setLogic( J15ETA24.x(5) & physcond).setTriggerType(TT.calo)    # noqa: F821


        #example zero bias seeded by single jet
        #item_zb_j10  = LVL1MenuItem('L1_ZB_J10',  ctpid=240).setLogic(thr_zb_j10.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        #example zero bias seeded by di jet
        #item_zb_2j10 = LVL1MenuItem('L1_ZB_2J10', ctpid=241).setLogic(thr_zb_2j10.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        #zb item for v3 menu, seeded by mbts a

        if ('pp_v4' in TriggerFlags.triggerMenuSetup() or 'LS1_v1' in TriggerFlags.triggerMenuSetup()) :
            LVL1MenuItem('L1_ZB', ctpid=240).setLogic(ZB_EM12.x(1) & physcond).setTriggerType(TT.zerobs)    # noqa: F821
        elif  'DC14' in TriggerFlags.triggerMenuSetup():
            LVL1MenuItem('L1_ZB', ctpid=240).setLogic(ZB_EM15V.x(1) & physcond).setTriggerType(TT.zerobs)    # noqa: F821
        else:
            LVL1MenuItem('L1_ZB', ctpid=240).setLogic(ZB_EM14.x(1) & physcond).setTriggerType(TT.zerobs)    # noqa: F821


            
        # - Add/remap new L1 thresholds:
        #   J20->J15, J40->J30, J70->J55, J100->J75, J130->J95, J150->J115
        LVL1MenuItem('L1_4J30'    ).setLogic( J30.x(4) & physcond)    # noqa: F821
        LVL1MenuItem('L1_3J15_J50').setLogic( J15.x(3) & J50.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_J30_FJ30').setLogic( J30.x(1) & ( JF30.x(1) | JB30.x(1) ) & physcond)    # noqa: F821
        LVL1MenuItem('L1_J50_FJ50').setLogic( J50.x(1) & ( JF50.x(1) | JB50.x(1) ) & physcond)    # noqa: F821



        # FJ
        #pPb items
        
        LVL1MenuItem('L1_FJ0',   ctpid=0x7b).setLogic(( JF0.x(1) | JB0.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_FJ0_A', ctpid=0x7b).setLogic(( JF0.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_FJ0_C', ctpid=0x7b).setLogic(( JB0.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821
        
        #pp items
        LVL1MenuItem('L1_FJ5', ctpid=0x7b).setLogic(( JF5.x(1) |  JB5.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_FJ10').setLogic(( JF10.x(1) |  JB10.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_FJ15').setLogic(( JF15.x(1) |  JB15.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_FJ30').setLogic(( JF30.x(1) |  JB30.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_FJ50').setLogic(( JF50.x(1) |  JB50.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_FJ75').setLogic(( JF75.x(1) |  JB75.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821

        # 2xFJ
        LVL1MenuItem('L1_2FJ15').setLogic( JF15.x(1) &  JB15.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_2FJ30', ctpid=0x7d).setLogic( JF30.x(1) &  JB30.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_2FJ50', ctpid=0x7b).setLogic( JF50.x(1) &  JB50.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821

        
        # Diffractive

        LVL1MenuItem('L1_MU0_TE50'    ).setLogic( MU0.x(1) & TE50.x(1) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU4_TE50'    ).setLogic( MU4.x(1) & TE50.x(1) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_EM3_2J5'     ).setLogic( EM3.x(1) & J5.x(2)   & physcond).setTriggerType(TT.calo)    # noqa: F821

        LVL1MenuItem('L1_MU0_VTE50'   ).setLogic( MU0.x(1) & Not(TE50.x(1)) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU0_VTE20'   ).setLogic( MU0.x(1) & Not(TE20.x(1)) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU4_VTE50'   ).setLogic( MU4.x(1) & Not(TE50.x(1)) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_EM3_VTE50'   ).setLogic( EM3.x(1) & Not(TE50.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_EM3_VTE20'   ).setLogic( EM3.x(1) & Not(TE20.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821

        LVL1MenuItem('L1_MU0_MV_VTE50').setLogic( MU0.x(1) & Not(MBTS_A.x(1)& MBTS_C.x(1)) & Not(TE50.x(1)) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU4_MV_VTE50').setLogic( MU4.x(1) & Not(MBTS_A.x(1)& MBTS_C.x(1)) & Not(TE50.x(1)) & physcond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_EM3_MV_VTE50').setLogic( EM3.x(1) & Not(MBTS_A.x(1)& MBTS_C.x(1)) & Not(TE50.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821


        # Items for EMPTY
        # EM
        LVL1MenuItem('L1_EM3_EMPTY',   ctpid=-1).setLogic( EM3.x(1)   & cosmiccond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM5_EMPTY',   ctpid=-1).setLogic( EM5.x(1)   & cosmiccond).setTriggerType( emTT )    # noqa: F821
        LVL1MenuItem('L1_EM6_EMPTY',   ctpid=-1).setLogic( EM6.x(1)   & cosmiccond).setTriggerType( emTT )    # noqa: F821
        # Muon
        LVL1MenuItem('L1_MU0_EMPTY'      ).setLogic( MU0.x(1)  & cosmiccond).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_MU4_EMPTY'      ).setLogic( MU4.x(1)  & cosmiccond).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_MU10_EMPTY'     ).setLogic( MU10.x(1) & cosmiccond).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_2MU6_EMPTY'     ).setLogic( MU6.x(2)  & cosmiccond).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_2MU0_EMPTY'     ).setLogic( MU0.x(2)  & cosmiccond).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_2MU4_EMPTY'     ).setLogic( MU4.x(2)  & cosmiccond).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_MU11_EMPTY'     ).setLogic( MU11.x(1) & cosmiccond).setTriggerType(TT.rpcout)    # noqa: F821
        # Tau
        LVL1MenuItem('L1_TAU8_EMPTY').setLogic( HA8.x(1) & cosmiccond).setTriggerType(TT.calo)    # noqa: F821
        # Jet
        LVL1MenuItem('L1_J5_EMPTY' ).setLogic( J5.x(1)  & cosmiccond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J10_EMPTY').setLogic( J10.x(1) & cosmiccond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J30_EMPTY').setLogic( J30.x(1) & cosmiccond).setTriggerType(TT.calo)    # noqa: F821
        # Forward Jet
        LVL1MenuItem('L1_FJ5_EMPTY' ).setLogic( ( JF5.x(1) |  JB5.x(1)) & cosmiccond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_FJ0_EMPTY' ).setLogic( ( JF0.x(1) |  JB0.x(1)) & cosmiccond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_FJ30_EMPTY').setLogic( (JF30.x(1) | JB30.x(1)) & cosmiccond).setTriggerType(TT.calo)    # noqa: F821
        # combined
        LVL1MenuItem('L1_MU4_J15_EMPTY'  ).setLogic( MU4.x(1)  &  J15.x(1) & cosmiccond).setTriggerType(TT.rpcout)    # noqa: F821


                
        
        # Items for EMPTY, UNPAIRED1, UNPAIRED2 and UNPAIRED

        # Jet
        LVL1MenuItem('L1_J5_UNPAIRED_ISO'     ).setLogic( J5.x(1)  & unpaired_isocond   ).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J5_UNPAIRED_NONISO'  ).setLogic( J5.x(1)  & unpaired_nonisocond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J5_FIRSTEMPTY'       ).setLogic( J5.x(1)  & firstempty         ).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J10_UNPAIRED_ISO'    ).setLogic( J10.x(1) & unpaired_isocond   ).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J10_UNPAIRED_NONISO' ).setLogic( J10.x(1) & unpaired_nonisocond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J10_FIRSTEMPTY'      ).setLogic( J10.x(1) & firstempty         ).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J30_UNPAIRED'        ).setLogic( J30.x(1) & unpairedRcond      ).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J30_UNPAIRED_ISO'    ).setLogic( J30.x(1) & unpaired_isocond   ).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J30_UNPAIRED_NONISO' ).setLogic( J30.x(1) & unpaired_nonisocond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_J30_FIRSTEMPTY'      ).setLogic( J30.x(1) & firstempty         ).setTriggerType(TT.calo)    # noqa: F821
        # Forward jet
        LVL1MenuItem('L1_FJ10_UNPAIRED_NONISO').setLogic( ( JF10.x(1) |  JB10.x(1)) & unpaired_nonisocond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_FJ0_UNPAIRED_ISO'    ).setLogic( ( JF0.x(1)  |  JB0.x(1) ) & unpaired_isocond   ).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_FJ5_UNPAIRED_ISO'    ).setLogic( ( JF5.x(1)  |  JB5.x(1) ) & unpaired_isocond   ).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_FJ30_UNPAIRED_ISO'   ).setLogic( ( JF30.x(1) |  JB30.x(1)) & unpaired_isocond   ).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_FJ30_FIRSTEMPTY'     ).setLogic( ( JF30.x(1) |  JB30.x(1)) & firstempty         ).setTriggerType(TT.calo)    # noqa: F821
        # Tau
        LVL1MenuItem('L1_TAU8_UNPAIRED_ISO'   ).setLogic( HA8.x(1) & unpaired_isocond   ).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_TAU8_UNPAIRED_NONISO').setLogic( HA8.x(1) & unpaired_nonisocond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_TAU8_FIRSTEMPTY'     ).setLogic( HA8.x(1) & firstempty         ).setTriggerType(TT.calo)    # noqa: F821
        # EM
        LVL1MenuItem('L1_EM3_UNPAIRED_ISO'    ).setLogic( EM3.x(1) & unpaired_isocond   ).setTriggerType(TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_EM3_UNPAIRED_NONISO' ).setLogic( EM3.x(1) & unpaired_nonisocond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_EM3_FIRSTEMPTY'      ).setLogic( EM3.x(1) & firstempty         ).setTriggerType(TT.calo|TT.caloem)    # noqa: F821
        # Muon
        LVL1MenuItem('L1_MU0_UNPAIRED_ISO'    ).setLogic( MU0.x(1)  & unpaired_isocond   ).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU4_UNPAIRED_ISO'    ).setLogic( MU4.x(1)  & unpaired_isocond   ).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU10_UNPAIRED_ISO'   ).setLogic( MU10.x(1) & unpaired_isocond   ).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU0_UNPAIRED_NONISO' ).setLogic( MU0.x(1)  & unpaired_nonisocond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU4_UNPAIRED_NONISO' ).setLogic( MU4.x(1)  & unpaired_nonisocond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU0_FIRSTEMPTY'      ).setLogic( MU0.x(1)  & firstempty         ).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU4_FIRSTEMPTY'      ).setLogic( MU4.x(1)  & firstempty         ).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU6_FIRSTEMPTY'      ).setLogic( MU6.x(1)  & firstempty         ).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU10_FIRSTEMPTY'     ).setLogic( MU10.x(1) & firstempty         ).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_MU20_FIRSTEMPTY'     ).setLogic( MU20.x(1) & firstempty         ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_2MU6_UNPAIRED_ISO'   ).setLogic( MU6.x(2)  & unpaired_isocond   ).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU6_UNPAIRED_NONISO').setLogic( MU6.x(2)  & unpaired_nonisocond).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU4_FIRSTEMPTY'     ).setLogic( MU4.x(2)  & firstempty         ).setTriggerType(TT.rpcin)    # noqa: F821
        LVL1MenuItem('L1_2MU6_FIRSTEMPTY'     ).setLogic( MU6.x(2)  & firstempty         ).setTriggerType(TT.rpcin)    # noqa: F821
        # combined
        LVL1MenuItem('L1_MU4_J15_UNPAIRED_ISO').setLogic( MU4.x(1) & J15.x(1) & unpaired_isocond).setTriggerType(TT.rpcin)    # noqa: F821

        #Combined JET+MET
        LVL1MenuItem('L1_J40_XE50').setLogic( (J40.x(1) & XE50.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821
        
        # XE ctpid=[0x80:0x9f]
        LVL1MenuItem('L1_XE20', ctpid=0x81).setLogic( XE20.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_XE25', ctpid=0x82).setLogic( XE25.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_XE30', ctpid=0x83).setLogic( XE30.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_XE40', ctpid=0x84).setLogic( XE40.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_XE50', ctpid=0x85).setLogic( XE50.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_XE70', ctpid=0x86).setLogic( XE70.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821

        LVL1MenuItem('L1_XE35_BGRP7').setLogic( XE35.x(1)&bgrp7cond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_XE40_BGRP7').setLogic( XE40.x(1)&bgrp7cond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_XE50_BGRP7').setLogic( XE50.x(1)&bgrp7cond).setTriggerType(TT.calo)    # noqa: F821

        #pPb items        
        LVL1MenuItem('L1_TE10').setLogic( TE10.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_TE20').setLogic( TE20.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_TE30').setLogic( TE30.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_TE40').setLogic( TE40.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_TE35').setLogic( TE35.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821

        # Other TE items for L=10^32 and 10^33
        LVL1MenuItem('L1_TE800', ctpid=0x8f).setLogic( TE800.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821


        # items for HI p+Pb (forward TE)    # noqa: F821
        if 'HI_v2' in TriggerFlags.triggerMenuSetup():
            LVL1MenuItem('L1_TE0'  ).setLogic( TE0.x(1)   & physcond).setTriggerType(TT.calo)    # noqa: F821
            LVL1MenuItem('L1_TE20' ).setLogic( TE20.x(1)  & physcond).setTriggerType(TT.calo)    # noqa: F821
            LVL1MenuItem('L1_TE50' ).setLogic( TE50.x(1)  & physcond).setTriggerType(TT.calo)    # noqa: F821
            LVL1MenuItem('L1_TE65' ).setLogic( TE65.x(1)  & physcond).setTriggerType(TT.calo)    # noqa: F821
            LVL1MenuItem('L1_TE90' ).setLogic( TE90.x(1)  & physcond).setTriggerType(TT.calo)    # noqa: F821
            LVL1MenuItem('L1_TE100').setLogic( TE100.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        else:
            LVL1MenuItem('L1_TE0'  ).setLogic( TE0.x(1)   & physcond).setTriggerType(TT.calo)    # noqa: F821
            LVL1MenuItem('L1_TE50' ).setLogic( TE50.x(1)  & physcond).setTriggerType(TT.calo)    # noqa: F821
            LVL1MenuItem('L1_TE65' ).setLogic( TE65.x(1)  & physcond).setTriggerType(TT.calo)    # noqa: F821
            LVL1MenuItem('L1_TE90' ).setLogic( TE90.x(1)  & physcond).setTriggerType(TT.calo)    # noqa: F821
            LVL1MenuItem('L1_TE100').setLogic( TE100.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
            

        LVL1MenuItem('L1_XS30').setLogic( XS30.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_XS45').setLogic( XS45.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_XS50').setLogic( XS50.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_XS55').setLogic( XS55.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_XS60').setLogic( XS60.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_XS65').setLogic( XS65.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        
        LVL1MenuItem('L1_JE140'            ).setLogic( JE140.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_JE200', ctpid=0x92).setLogic( JE200.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_JE350', ctpid=0x94).setLogic( JE350.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_JE500', ctpid=0x96).setLogic( JE500.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821

        # Transient items
        LVL1MenuItem('L1_XE35').setLogic( XE35.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_XE60').setLogic( XE60.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821

        LVL1MenuItem('L1_TE300').setLogic( TE300.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_TE500').setLogic( TE500.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_TE700').setLogic( TE700.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        
        # Combined items [0xa0:0xff], [0x30:0x3f],

        # EM + (XE, MU, TAU)
        LVL1MenuItem('L1_EM10VH_XE20').setLogic( EM10VH.x(1)& XE20.x(1) & physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_EM10VH_XE30').setLogic( EM10VH.x(1)& XE30.x(1) & physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_EM10VH_XE35').setLogic( EM10VH.x(1)& XE35.x(1) & physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821


        LVL1MenuItem('L1_EM3_MU6', ctpid=0xbd).setLogic( MU6.x(1)& EM3.x(1) & physcond).setTriggerType(TT.rpcin|TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_EM6_2MU6'   ).setLogic( MU6.x(2)& EM6.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM10VH_MU6' ).setLogic( EM10VH.x(1)& MU6.x(1) & physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_EM15VH_MU10').setLogic( EM15VH.x(1)& MU10.x(1)& physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_EM16VH_MU4' ).setLogic( EM16VH.x(1)& MU4.x(1) & physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_2EM8VH_MU10' ).setLogic( EM8VH.x(2)& MU10.x(1) & physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_EM8VH_2MU6' ).setLogic( EM8VH.x(1)& MU6.x(2) & physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821


        LVL1MenuItem('L1_2EM6_MU6'   ).setLogic( EM6.x(2)& MU6.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM6_MU6'    ).setLogic( EM6.x(1)& MU6.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM6_MU10'   ).setLogic( EM6.x(1)& MU10.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM12_XE20'  ).setLogic( EM12.x(1)& XE20.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM16V_XE20' ).setLogic( EM16V.x(1)& XE20.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM6_XS45'   ).setLogic( EM6.x(1)& XS45.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM6_XS50'   ).setLogic( EM6.x(1)& XS50.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM6_XS55'   ).setLogic( EM6.x(1)& XS55.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM6_XS60'   ).setLogic( EM6.x(1)& XS60.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM16V_XS45' ).setLogic( EM16V.x(1)& XS45.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM12_XS45'  ).setLogic( EM12.x(1)& XS45.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM12_XS30'  ).setLogic( EM12.x(1)& XS30.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM12_4J10'  ).setLogic( EM12.x(1)& J10.x(4) & physcond)    # noqa: F821
        LVL1MenuItem('L1_EM15VH_J15.23ETA49' ).setLogic( EM15VH&  (JF1523ETA49.x(1)  | JB1523ETA49.x(1)  ) & physcond).setTriggerType(TT.calo)    # noqa: F821


        # TAU + EM
        LVL1MenuItem('L1_2TAU11I_EM14VH').setLogic( HA11I.x(2)& EM14VH.x(1) & physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_2TAU11_TAU20_EM14VH').setLogic( HA11.x(2)& HA20.x(1)& EM14VH.x(1) & physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_2TAU11_TAU20_EM10VH').setLogic( HA11.x(2)& HA20.x(1)& EM10VH.x(1) & physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_2TAU11_TAU15', ctpid=0x0).setLogic( HA11.x(2)& HA15.x(1) & physcond).setTriggerType(TT.calo)      # noqa: F821
        LVL1MenuItem('L1_2TAU11I_TAU15', ctpid=0x0).setLogic( HA11I.x(2)& HA15.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_2TAU11_EM10VH').setLogic( HA11.x(2)& EM10VH.x(1) & physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821

        
        LVL1MenuItem('L1_MU4_J50').setLogic( MU4.x(1)& J50.x(1) & physcond).setTriggerType(TT.rpcin|TT.calo)    # noqa: F821
        LVL1MenuItem('L1_MU4_J10').setLogic( MU4.x(1)& J10.x(1) & physcond).setTriggerType(TT.rpcin|TT.calo)    # noqa: F821
 
        # - Add/remap new L1 thresholds:
        #   J20->J15, J40->J30, J70->J55, J100->J75, J130->J95, J150->J115
        LVL1MenuItem('L1_MU4_J15').setLogic( MU4.x(1)& J15.x(1) & physcond).setTriggerType(TT.rpcin|TT.calo)    # noqa: F821
        LVL1MenuItem('L1_MU4_J30').setLogic( MU4.x(1)& J30.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_MU4_J75').setLogic( MU4.x(1)& J75.x(1) & physcond)    # noqa: F821

        LVL1MenuItem('L1_MU4_J20_XE20').setLogic( MU4.x(1)& J20.x(1)& XE20.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_MU4_J20_XE35').setLogic( MU4.x(1)& J20.x(1)& XE35.x(1) & physcond)    # noqa: F821

        LVL1MenuItem('L1_MU6_J15').setLogic( MU6.x(1)& J15.x(1) & physcond).setTriggerType(TT.rpcin|TT.calo)    # noqa: F821
        LVL1MenuItem('L1_MU6_2J20').setLogic( MU6.x(1)& J20.x(2) & physcond).setTriggerType(TT.rpcin|TT.calo)    # noqa: F821
        LVL1MenuItem('L1_2MU4_EM3', ctpid=0xbd).setLogic( MU4.x(2)& EM3.x(1) & physcond).setTriggerType(TT.rpcin|TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_2MU4_2EM3', ctpid=0xbd).setLogic( MU4.x(2)& EM3.x(2) & physcond).setTriggerType(TT.rpcin|TT.calo|TT.caloem)    # noqa: F821

        LVL1MenuItem('L1_MU10_XE20').setLogic( MU10.x(1)& XE20.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_MU10_XE25').setLogic( MU10.x(1)& XE25.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_MU10_J20').setLogic( MU10.x(1) & J20.x(1) & physcond) # new jet threshold   # noqa: F821

        # TAU + MU
        LVL1MenuItem('L1_TAU8_MU10'             ).setLogic( HA8.x(1)  & MU10.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_TAU11_MU10', ctpid=0xc7).setLogic( HA11.x(1) & MU10.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_2J30_XE20'             ).setLogic( J30.x(2)  & XE20.x(1) & physcond) # new jet threshold    # noqa: F821

        # TAU + XE
        LVL1MenuItem('L1_TAU15_XE35'     ).setLogic( HA15.x(1)  & XE35.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_TAU15_XE40'     ).setLogic( HA15.x(1)  & XE40.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_TAU15I_XE35'    ).setLogic( HA15I.x(1) & XE35.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_TAU15I_XE40'    ).setLogic( HA15I.x(1) & XE40.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_TAU20_XE40'     ).setLogic( HA20.x(1)  & XE40.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_TAU20_XE35'     ).setLogic( HA20.x(1)  & XE35.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_TAU15_XS35'     ).setLogic( HA15.x(1)  & XS35.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_TAU15_XE25_3J15').setLogic( HA15.x(1)  & XE25.x(1) & J15.x(3) & physcond).setTriggerType(TT.calo)    # noqa: F821

        # VBF request
        
        LVL1MenuItem('L1_2TAU8_TAU15_FJ15'         ).setLogic( HA8.x(2) & HA15.x(1)  & ( JF15.x(1) | JB15.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_2TAU8_TAU15_J15CF'        ).setLogic( HA8.x(2) & HA15.x(1)  & J15CF.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_2TAU8_TAU11I_EM10VH_FJ15' ).setLogic( HA8.x(2) & HA11I.x(1) & EM10VH.x(1)&( JF15.x(1)| JB15.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_2TAU8_TAU11I_EM10VH_J15CF').setLogic( HA8.x(2) & HA11I.x(1) & EM10VH.x(1)& J15CF.x(1) & physcond).setTriggerType(TT.calo)    # noqa: F821

               
        LVL1MenuItem('L1_EM14VH_FJ15' ).setLogic( EM14VH.x(1)&( JF15.x(1)| JB15.x(1)) & physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821
        LVL1MenuItem('L1_EM14VH_J15CF').setLogic( EM14VH.x(1)& J15CF.x(1) & physcond).setTriggerType(TT.calo|TT.caloem)    # noqa: F821

        LVL1MenuItem('L1_MU10_FJ15').setLogic(  MU10.x(1) & ( JF15.x(1) | JB15.x(1)) & physcond)    # noqa: F821
        LVL1MenuItem('L1_MU10_J15CF').setLogic( MU10.x(1)& J15CF.x(1) & physcond)    # noqa: F821

        LVL1MenuItem('L1_3J15_FJ15').setLogic( J15.x(3)&( JF15.x(1)| JB15.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821
        LVL1MenuItem('L1_2J15_2FJ15').setLogic( J15.x(2)&( JF15.x(1)& JB15.x(1)) & physcond).setTriggerType(TT.calo)    # noqa: F821

        # Transient items
        # L1_TAU6_2JT4_JT35  (or whatever soft thresholds they have. JT4 must have 
        # softer cuts than TAU06 and JT35 - please get in touch with us if it is not 
        # the case - we will need to rethink it then)    # noqa: F821

        LVL1MenuItem('L1_J50_XE30').setLogic( J50.x(1)&  XE30.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_J50_XE35').setLogic( J50.x(1)&  XE35.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_J50_XE40').setLogic( J50.x(1)&  XE40.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_J30_XE35').setLogic( J30.x(1)& XE35.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_J30_XE40').setLogic( J30.x(1)& XE40.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_J30_XE50').setLogic( J30.x(1)& XE50.x(1) & physcond)    # noqa: F821
        LVL1MenuItem('L1_2J20_XE20').setLogic( J20.x(2)&  XE20.x(1) & physcond)    # noqa: F821
        

        # MBTS
        MBTS_1   = MBTS_A.x(1) | MBTS_C.x(1)    # noqa: F821
        MBTS_2   = (MBTS_A.x(2) | MBTS_C.x(2) | MBTS_A.x(1)) & (MBTS_A.x(2) | MBTS_C.x(2) | MBTS_C.x(1))    # noqa: F821
        MBTS_1_1 = MBTS_A.x(1) & MBTS_C.x(1)    # noqa: F821
        MBTS_2_2 = MBTS_A.x(2) & MBTS_C.x(2)    # noqa: F821
        MBTS_3_3 = MBTS_A.x(3) & MBTS_C.x(3)    # noqa: F821
        MBTS_4_4 = MBTS_A.x(4) & MBTS_C.x(4)    # noqa: F821

        LVL1MenuItem('L1_MBTS_1',           ctpid=0xe2).setLogic( MBTS_1   & physcond ).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_2',           ctpid=0xe3).setLogic( MBTS_2   & physcond ).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_1_1_OVERLAY', ctpid=0xe4).setLogic( MBTS_1_1 & physcond ).setTriggerType(TT.zerobs)    # noqa: F821
        LVL1MenuItem('L1_MBTS_1_1',         ctpid=0xe4).setLogic( MBTS_1_1 & physcond ).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_2_2'                    ).setLogic( MBTS_2_2 & physcond ).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_3_3'                    ).setLogic( MBTS_3_3 & physcond ).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_4_4',         ctpid=0xe1).setLogic( MBTS_4_4 & physcond ).setTriggerType(TT.minb)    # noqa: F821

        LVL1MenuItem('L1_MBTS_1_EMPTY'  ).setLogic( MBTS_1   & cosmiccond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_2_EMPTY'  ).setLogic( MBTS_2   & cosmiccond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_1_1_EMPTY').setLogic( MBTS_1_1 & cosmiccond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_2_2_EMPTY').setLogic( MBTS_2_2 & cosmiccond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_3_3_EMPTY').setLogic( MBTS_3_3 & cosmiccond).setTriggerType(TT.minb)    # noqa: F821

        LVL1MenuItem('L1_MBTS_1_UNPAIRED_ISO'   ).setLogic( MBTS_1 & unpaired_isocond    ).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_1_UNPAIRED_NONISO').setLogic( MBTS_1 & unpaired_nonisocond ).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_2_UNPAIRED_ISO'   ).setLogic( MBTS_2 & unpaired_isocond ).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_2_UNPAIRED_NONISO').setLogic( MBTS_2 & unpaired_nonisocond ).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_2_BGRP7'          ).setLogic( MBTS_2 & bgrp7cond           ).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_1_1_UNPAIRED_ISO' ).setLogic( MBTS_1_1 & unpaired_isocond ).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_2_2_UNPAIRED_ISO' ).setLogic( MBTS_2_2 & unpaired_isocond ).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_3_3_UNPAIRED_ISO' ).setLogic( MBTS_3_3 & unpaired_isocond ).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTS_4_4_UNPAIRED_ISO' ).setLogic( MBTS_4_4 & unpaired_isocond ).setTriggerType(TT.minb)    # noqa: F821

        LVL1MenuItem('L1_MBTSA0' ).setLogic( MBTS_A0.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA1' ).setLogic( MBTS_A1.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA2' ).setLogic( MBTS_A2.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA3' ).setLogic( MBTS_A3.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA4' ).setLogic( MBTS_A4.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA5' ).setLogic( MBTS_A5.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA6' ).setLogic( MBTS_A6.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA7' ).setLogic( MBTS_A7.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA8' ).setLogic( MBTS_A8.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA9' ).setLogic( MBTS_A9.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA10').setLogic( MBTS_A10.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA11').setLogic( MBTS_A11.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA12').setLogic( MBTS_A12.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA13').setLogic( MBTS_A13.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA14').setLogic( MBTS_A14.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSA15').setLogic( MBTS_A15.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821

        LVL1MenuItem('L1_MBTSC0' ).setLogic( MBTS_C0.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC1' ).setLogic( MBTS_C1.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC2' ).setLogic( MBTS_C2.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC3' ).setLogic( MBTS_C3.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC4' ).setLogic( MBTS_C4.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC5' ).setLogic( MBTS_C5.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC6' ).setLogic( MBTS_C6.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC7' ).setLogic( MBTS_C7.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC8' ).setLogic( MBTS_C8.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC9' ).setLogic( MBTS_C9.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC10').setLogic( MBTS_C10.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC11').setLogic( MBTS_C11.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC12').setLogic( MBTS_C12.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC13').setLogic( MBTS_C13.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC14').setLogic( MBTS_C14.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_MBTSC15').setLogic( MBTS_C15.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        

        # ZDC
        
        LVL1MenuItem('L1_ZDC'                     ).setLogic((ZDC_A.x(1)|ZDC_C.x(1)) & physcond).setTriggerType(TT.minb)                                # noqa: F821
        LVL1MenuItem('L1_ZDC_EMPTY'               ).setLogic((ZDC_A.x(1)|ZDC_C.x(1))&cosmiccond).setTriggerType(TT.minb)                                # noqa: F821
        LVL1MenuItem('L1_ZDC_UNPAIRED_ISO'        ).setLogic((ZDC_A.x(1)|ZDC_C.x(1))&unpaired_isocond).setTriggerType(TT.minb)                          # noqa: F821
        LVL1MenuItem('L1_ZDC_UNPAIRED_NONISO'     ).setLogic((ZDC_A.x(1)|ZDC_C.x(1))&unpaired_nonisocond).setTriggerType(TT.minb)                       # noqa: F821
        LVL1MenuItem('L1_ZDC_A_C'                 ).setLogic(ZDC_A.x(1)&ZDC_C.x(1) & physcond).setTriggerType(TT.minb)                                  # noqa: F821
        LVL1MenuItem('L1_ZDC_A_C_EMPTY'           ).setLogic(ZDC_A.x(1)&ZDC_C.x(1)&cosmiccond).setTriggerType(TT.minb)                                  # noqa: F821
        LVL1MenuItem('L1_ZDC_A_C_UNPAIRED_ISO'    ).setLogic(ZDC_A.x(1)&ZDC_C.x(1)&unpaired_isocond).setTriggerType(TT.minb)                            # noqa: F821
        LVL1MenuItem('L1_ZDC_A_C_UNPAIRED_NONISO' ).setLogic(ZDC_A.x(1)&ZDC_C.x(1)&unpaired_nonisocond).setTriggerType(TT.minb)                         # noqa: F821
        LVL1MenuItem('L1_ZDC_A'                   ).setLogic(ZDC_A.x(1) & physcond).setTriggerType(TT.minb)                                             # noqa: F821
        LVL1MenuItem('L1_ZDC_C'                   ).setLogic(ZDC_C.x(1) & physcond).setTriggerType(TT.minb)                                             # noqa: F821
        LVL1MenuItem('L1_ZDC_AND'                 ).setLogic(ZDC_AND.x(1) & physcond).setTriggerType(TT.minb)                                           # noqa: F821
        LVL1MenuItem('L1_ZDC_A_VTE20'             ).setLogic(ZDC_A.x(1)&Not( TE20.x(1)) & physcond).setTriggerType(TT.minb)                             # noqa: F821
        LVL1MenuItem('L1_ZDC_C_VTE20'             ).setLogic(ZDC_C.x(1)&Not( TE20.x(1)) & physcond).setTriggerType(TT.minb)                             # noqa: F821
        LVL1MenuItem('L1_ZDC_A_BGRP7'             ).setLogic(ZDC_A.x(1)&bgrp7cond).setTriggerType(TT.minb)                                              # noqa: F821
        LVL1MenuItem('L1_ZDC_C_BGRP7'             ).setLogic(ZDC_C.x(1)&bgrp7cond).setTriggerType(TT.minb)                                              # noqa: F821
        LVL1MenuItem('L1_ZDC_MBTS_1'              ).setLogic((ZDC_A.x(1)|ZDC_C.x(1))&( MBTS_A.x(1)| MBTS_C.x(1)) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_ZDC_MBTS_2'              ).setLogic((ZDC_A.x(1)|ZDC_C.x(1))&\                                                                  # noqa: F821
                                                             ( MBTS_A.x(2)| MBTS_C.x(2)| MBTS_A.x(1))\                                                  # noqa: F821
                                                             &( MBTS_A.x(2)| MBTS_C.x(2)| MBTS_C.x(1)) & physcond).setTriggerType(TT.minb)              # noqa: F821
        LVL1MenuItem('L1_ZDC_MBTS_1_1'            ).setLogic((ZDC_A.x(1)|ZDC_C.x(1))&( MBTS_A.x(1)& MBTS_C.x(1)) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_ZDC_MBTS_2_2'            ).setLogic((ZDC_A.x(1)|ZDC_C.x(1))&( MBTS_A.x(2)& MBTS_C.x(2)) & physcond).setTriggerType(TT.minb)    # noqa: F821


        # BCM

        LVL1MenuItem('L1_BCM_AC_CA_BGRP0',ctpid=0xe6 ).setLogic((BCM_AtoC.x(1)|BCM_CtoA.x(1))&BGRP0).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_BCM_AC_CA_UNPAIRED_ISO'     ).setLogic((BCM_AtoC.x(1)|BCM_CtoA.x(1))&unpaired_isocond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_BCM_AC_CA_UNPAIRED_NONISO'  ).setLogic((BCM_AtoC.x(1)|BCM_CtoA.x(1))&unpaired_nonisocond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_BCM_HT_BGRP0',   ctpid=0xf8 ).setLogic( BCM_Comb.x(7)&BGRP0).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_BCM_Wide_BGRP0', ctpid=0xe8 ).setLogic( BCM_Wide.x(1)&BGRP0).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_BCM_Wide_EMPTY'             ).setLogic( BCM_Wide.x(1)&cosmiccond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_BCM_Wide_UNPAIRED_ISO'      ).setLogic( BCM_Wide.x(1)&unpaired_isocond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_BCM_Wide_UNPAIRED_NONISO'   ).setLogic( BCM_Wide.x(1)&unpaired_nonisocond).setTriggerType(TT.minb)    # noqa: F821


        # LUCID
        
        LVL1MenuItem('L1_LUCID_C',     ctpid=0xed  ).setLogic( LUCID_C.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_LUCID_A',     ctpid=0xec  ).setLogic( LUCID_A.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_LUCID_A_C',   ctpid=0xee  ).setLogic( LUCID_A.x(1)&LUCID_C.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_LUCID',       ctpid=0xf5  ).setLogic( (LUCID_A.x(1)|LUCID_C.x(1)) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_LUCID_BGRP7'              ).setLogic( (LUCID_A.x(1)|LUCID_C.x(1))&bgrp7cond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_LUCID_COMM'               ).setLogic( NIMDIR5.x(1) & physcond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_LUCID_A_C_EMPTY'          ).setLogic( LUCID_A.x(1)&LUCID_C.x(1)&cosmiccond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_LUCID_EMPTY'              ).setLogic( (LUCID_A.x(1)|LUCID_C.x(1))&cosmiccond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_LUCID_UNPAIRED_ISO'       ).setLogic( (LUCID_A.x(1)|LUCID_C.x(1))&unpaired_isocond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_LUCID_UNPAIRED_NONISO'    ).setLogic( (LUCID_A.x(1)|LUCID_C.x(1))&unpaired_nonisocond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_LUCID_COMM_EMPTY'         ).setLogic( NIMDIR5.x(1)&cosmiccond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_LUCID_COMM_UNPAIRED_ISO'  ).setLogic( NIMDIR5.x(1)&unpaired_isocond).setTriggerType(TT.minb)        
        LVL1MenuItem('L1_LUCID_A_C_UNPAIRED_ISO'   ).setLogic( LUCID_A.x(1)&LUCID_C.x(1)&unpaired_isocond).setTriggerType(TT.minb)    # noqa: F821
        LVL1MenuItem('L1_LUCID_A_C_UNPAIRED_NONISO').setLogic( LUCID_A.x(1)&LUCID_C.x(1)&unpaired_nonisocond).setTriggerType(TT.minb)    # noqa: F821


        # RANDOM

        LVL1MenuItem('L1_RD0_FILLED'         ).setLogic( RNDM0 & physcond           ).setTriggerType(TT.rand)    # noqa: F821
        LVL1MenuItem('L1_RD0_EMPTY'          ).setLogic( RNDM0 & cosmiccond         ).setTriggerType(TT.rand)    # noqa: F821
        LVL1MenuItem('L1_RD0_FIRSTEMPTY'     ).setLogic( RNDM0 & firstempty         ).setTriggerType(TT.rand)    # noqa: F821
        LVL1MenuItem('L1_RD0_UNPAIRED_ISO'   ).setLogic( RNDM0 & unpaired_isocond   ).setTriggerType(TT.rand)    # noqa: F821
        LVL1MenuItem('L1_RD0_UNPAIRED_NONISO').setLogic( RNDM0 & unpaired_nonisocond).setTriggerType(TT.rand)    # noqa: F821
        LVL1MenuItem('L1_RD1_FILLED'         ).setLogic( RNDM1 & physcond           ).setTriggerType(TT.rand)    # noqa: F821
        LVL1MenuItem('L1_RD1_EMPTY'          ).setLogic( RNDM1 & cosmiccond         ).setTriggerType(TT.zerobs)    # noqa: F821

        
        # Bunch groups

        LVL1MenuItem('L1_BGRP1').setLogic(BGRP0 & BGRP1).setTriggerType(TT.rand)    # noqa: F821
        LVL1MenuItem('L1_BGRP7').setLogic(BGRP0 & BGRP7).setTriggerType(TT.rand)    # noqa: F821
        LVL1MenuItem('L1_BGRP1_ALFA_BGT').setLogic(BGRP0 & BGRP1).setTriggerType(TT.rand)    # noqa: F821
        LVL1MenuItem('L1_BGRP4_ALFA_BGT').setLogic(BGRP0 & BGRP4).setTriggerType(TT.rand)    # noqa: F821
        LVL1MenuItem('L1_BGRP7_ALFA_BGT').setLogic(BGRP0 & BGRP7).setTriggerType(TT.rand)    # noqa: F821
        LVL1MenuItem('L1_BPTX0_BGRP0', ctpid=0xf1).setLogic(BPTX0.x(1)&BGRP0).setTriggerType(TT.rand)    # noqa: F821
        LVL1MenuItem('L1_BPTX1_BGRP0', ctpid=0xf2).setLogic(BPTX1.x(1)&BGRP0).setTriggerType(TT.rand)    # noqa: F821
        

        # Direct inputs [0x50:0x5f]
        #LVL1MenuItem('L1_LHCF',  ctpid=-1).setLogic( (NIMDIR36.x(1) & physcond)).setTriggerType(TT.minb)        
        LVL1MenuItem('L1_LHCF',  ctpid=-1).setLogic( (NIMDIR36.x(1) & physcond))    # noqa: F821



        LVL1MenuItem('L1_ALFA_EMPTY').setLogic( NIM_ALFA_LOG.x(1) & cosmiccond).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_BGRP7').setLogic( NIM_ALFA_LOG.x(1) & bgrp7cond).setTriggerType(TT.rpcout)    # noqa: F821

        LVL1MenuItem('L1_ALFA_EMPTY_OD', ctpid=-1).setLogic( NIM_ALFA_LOG_OD.x(1)&cosmiccond).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_BGRP7_OD', ctpid=-1).setLogic( NIM_ALFA_LOG_OD.x(1)&bgrp7cond).setTriggerType(TT.rpcout)    # noqa: F821


        LVL1MenuItem('L1_TRT', ctpid=0x4e).setLogic(NIMDIR25.x(1)&BGRP0).setTriggerType(TT.rpcout)    # noqa: F821

        LVL1MenuItem('L1_L1A_Mon', ctpid=0x4f).setLogic(NIMDIR26.x(1)&BGRP0).setTriggerType(TT.rpcout)    # noqa: F821
        
        LVL1MenuItem('L1_CALREQ0', ctpid=0xfd).setLogic( NIM28.x(1) & calibcond).setTriggerType(TT.calreq0)    # noqa: F821
        LVL1MenuItem('L1_CALREQ1', ctpid=0xfe).setLogic( NIM29.x(1) & calibcond).setTriggerType(TT.calreq1)    # noqa: F821
        LVL1MenuItem('L1_CALREQ2', ctpid=0xff).setLogic( NIM30.x(1) & calibcond).setTriggerType(TT.calreq2)    # noqa: F821


        LVL1MenuItem('L1_NIM_S8C2B21').setLogic(NIMDIR6.x(1) & bgrpcond).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_NIM_S8C2B22').setLogic(NIMDIR7.x(1) & bgrpcond).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_NIM_S8C2B23').setLogic(NIMDIR8.x(1) & bgrpcond).setTriggerType(TT.rpcout)    # noqa: F821

        


        # Alfa items
        ARL = NIM_A7R1L.x(1)    # noqa: F821
        ARU = NIM_A7R1U.x(1)    # noqa: F821
        ALL = NIM_A7L1L.x(1)    # noqa: F821
        ALU = NIM_A7L1U.x(1)    # noqa: F821
        BRL = NIM_B7R1L.x(1)    # noqa: F821
        BRU = NIM_B7R1U.x(1)    # noqa: F821
        BLL = NIM_B7L1L.x(1)    # noqa: F821
        BLU = NIM_B7L1U.x(1)    # noqa: F821

        # Elastics1 and 2
        LVL1MenuItem('L1_ALFA_ELAST1').setLogic( BLU & ALU & ARL & BRL & Not( BLL | ALL | ARU | BRU ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_ELAST2').setLogic( BLL & ALL & ARU & BRU & Not( BLU | ALU | ARL | BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821

        # Elastics 11 - Elastics 18
        LVL1MenuItem('L1_ALFA_ELAST11').setLogic( BLU & ALU & ARL & BRL & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_ELAST12').setLogic( BLL & ALL & ARU & BRU & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_ELAST13').setLogic( ( BLU | ALU ) & ( ARL & BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_ELAST14').setLogic( ( BLU & ALU ) & ( ARL | BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_ELAST15').setLogic( ( BLU | ALU ) & ( ARL | BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_ELAST16').setLogic( ( BLL | ALL ) & ( ARU & BRU ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_ELAST17').setLogic( ( BLL & ALL ) & ( ARU | BRU ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_ELAST18').setLogic( ( BLL | ALL ) & ( ARU | BRU ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821

        # Single_Diffraction 1 - 8
        LVL1MenuItem('L1_ALFA_SDIFF1').setLogic( BLU & ALU & Not( BLL | ALL | ARU | ARL | BRU | BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SDIFF2').setLogic( ARL & BRL & Not( BLU | BLL | ALU | ALL | ARU | BRU ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SDIFF3').setLogic( BLL & ALL & Not( BLU | ALU | ARU | ARL | BRU | BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SDIFF4').setLogic( ARU & BRU & Not( BLU | BLL | ALU | ALL | ARL | BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821

        LVL1MenuItem('L1_ALFA_SDIFF5').setLogic( BLU & ALU & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SDIFF6').setLogic( ARL & BRL & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SDIFF7').setLogic( BLL & ALL & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SDIFF8').setLogic( ARU & BRU & physcond ).setTriggerType(TT.rpcout)    # noqa: F821

        # Big_showers 1 - 4
        LVL1MenuItem('L1_ALFA_SHOW1').setLogic( BLU & BLL & ALU & ALL & Not( ARU | ARL | BRU | BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SHOW2').setLogic( ARU & ARL & BRU & BRL & Not( BLU | BLL | ALU | ALL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SHOW3').setLogic( BLU & BLL & ALU & ALL & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SHOW4').setLogic( ARU & ARL & BRU & BRL & physcond ).setTriggerType(TT.rpcout)    # noqa: F821

        # Systematics 1 - 4
        LVL1MenuItem('L1_ALFA_SYST1').setLogic( BLU & BLL & Not( ALU | ALL | ARU | ARL | BRU | BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SYST2').setLogic( ALU & ALL & Not( BLU | BLL | ARU | ARL | BRU | BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SYST3').setLogic( ARU & ARL & Not( BLU | BLL | ALU | ALL | BRU | BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SYST4').setLogic( BRU & BRL & Not( BLU | BLL | ALU | ALL | ARU | ARL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821

        # Big_showers_Systematics 1 - 5
        LVL1MenuItem('L1_ALFA_SHOWSYST1').setLogic( BLU & BLL & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SHOWSYST2').setLogic( ALU & ALL & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SHOWSYST3').setLogic( ARU & ARL & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SHOWSYST4').setLogic( BRU & BRL & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SHOWSYST5').setLogic( ( BLU | BLL | ALU | ALL ) & ( ARU | ARL | BRU | BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821

        # Systematics 9 - 12, 17, 18
        LVL1MenuItem('L1_ALFA_SYST9' ).setLogic( BLU & ALU & ARU & BRU & Not( BLL | ALL | ARL | BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SYST10').setLogic( BLL & ALL & ARL & BRL & Not( BLU | ALU | ARU | BRU ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SYST11').setLogic( BLU & ALU & ARU & BRU & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SYST12').setLogic( BLL & ALL & ARL & BRL & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SYST17').setLogic( ( BLU | ALU ) & ( ARU | BRU ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_SYST18').setLogic( ( BLL | ALL ) & ( ARL | BRL ) & physcond ).setTriggerType(TT.rpcout)    # noqa: F821

        # ANY_ALFA = Minimum_Bias : B7L1U or B7L1L or A7L1U or A7L1L or A7R1U or A7R1L or B7R1U or B7R1L
        ANY_ALFA = BLU | BLL | ALU | ALL | ARU | ARL | BRU | BRL
        LVL1MenuItem('L1_ALFA_ANY'                ).setLogic( ANY_ALFA & physcond            ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_ANY_EMPTY'          ).setLogic( ANY_ALFA & cosmiccond          ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_ANY_FIRSTEMPTY'     ).setLogic( ANY_ALFA & firstempty          ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_ANY_UNPAIRED_ISO'   ).setLogic( ANY_ALFA & unpaired_isocond    ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_ANY_UNPAIRED_NONISO').setLogic( ANY_ALFA & unpaired_nonisocond ).setTriggerType(TT.rpcout)    # noqa: F821

        #---------------------------------
        # new ALFA items for 2012 running:
        #---------------------------------
        ARL_OD = NIM_A7R1L_OD.x(1)    # noqa: F821
        ARU_OD = NIM_A7R1U_OD.x(1)    # noqa: F821
        ALL_OD = NIM_A7L1L_OD.x(1)    # noqa: F821
        ALU_OD = NIM_A7L1U_OD.x(1)    # noqa: F821
        BRL_OD = NIM_B7R1L_OD.x(1)    # noqa: F821
        BRU_OD = NIM_B7R1U_OD.x(1)    # noqa: F821
        BLL_OD = NIM_B7L1L_OD.x(1)    # noqa: F821
        BLU_OD = NIM_B7L1U_OD.x(1)    # noqa: F821

        LVL1MenuItem('L1_ALFA_A7L1U').setLogic( ALU & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_A7L1L').setLogic( ALL & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_A7R1U').setLogic( ARU & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_A7R1L').setLogic( ARL & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_B7L1U').setLogic( BLU & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_B7L1L').setLogic( BLL & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_B7R1U').setLogic( BRU & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_B7R1L').setLogic( BRL & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821

        LVL1MenuItem('L1_ALFA_A7L1U_OD').setLogic( ALU_OD & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_A7L1L_OD').setLogic( ALL_OD & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_B7L1U_OD').setLogic( BLU_OD & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_B7L1L_OD').setLogic( BLL_OD & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_A7R1U_OD').setLogic( ARU_OD & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_A7R1L_OD').setLogic( ARL_OD & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_B7R1U_OD').setLogic( BRU_OD & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_B7R1L_OD').setLogic( BRL_OD & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821

        LVL1MenuItem('L1_ALFA_B7L1_OD').setLogic( ( BLU_OD & BLL_OD ) & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_A7L1_OD').setLogic( ( ALU_OD & ALL_OD ) & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_A7R1_OD').setLogic( ( ARU_OD & ARL_OD ) & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_B7R1_OD').setLogic( ( BRU_OD & BRL_OD ) & BGRP0 ).setTriggerType(TT.rpcout)    # noqa: F821
        
        LVL1MenuItem('L1_ALFA_B1_EMPTY').setLogic( ( BLU | BLL | ALU | ALL ) & cosmiccond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ALFA_B2_EMPTY').setLogic( ( ARU | ARL | BRU | BRL ) & cosmiccond ).setTriggerType(TT.rpcout)    # noqa: F821

        # ALFA and MBTS 
        LVL1MenuItem('L1_MBTS_2_A_ALFA_C').setLogic( MBTS_A.x(2) & (( ARU & BRU ) | ( ARL & BRL )) & bgrpcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_MBTS_2_C_ALFA_A').setLogic( MBTS_C.x(2) & (( ALU & BLU ) | ( ALL & BLL )) & bgrpcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_MBTS_2_ALFA').setLogic( MBTS_2 & ANY_ALFA & bgrpcond ).setTriggerType(TT.rpcout)    # noqa: F821

        LVL1MenuItem('L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO').setLogic( MBTS_A.x(2) & ((ARU & BRU) | (ARL & BRL)) & unpaired_isocond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO').setLogic( MBTS_C.x(2) & ((ALU & BLU) | (ALL & BLL)) & unpaired_isocond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_MBTS_2_ALFA_UNPAIRED_ISO'    ).setLogic( MBTS_2 & ANY_ALFA & unpaired_isocond ).setTriggerType(TT.rpcout)    # noqa: F821

        LVL1MenuItem('L1_MBTS_1_A_ALFA_C').setLogic( MBTS_A.x(1) & ((ARU & BRU) | (ARL & BRL)) & bgrpcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_MBTS_1_C_ALFA_A').setLogic( MBTS_C.x(1) & ((ALU & BLU) | (ALL & BLL)) & bgrpcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO').setLogic( MBTS_A.x(1) & ((ARU & BRU) | (ARL & BRL)) & unpaired_isocond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO').setLogic( MBTS_C.x(1) & ((ALU & BLU) | (ALL & BLL)) & unpaired_isocond ).setTriggerType(TT.rpcout)    # noqa: F821
            
        # ALFA and LUCID 
        LVL1MenuItem('L1_LUCID_A_ALFA_C').setLogic( LUCID_A.x(1) & ((ARU & BRU) | (ARL & BRL)) & bgrpcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_LUCID_C_ALFA_A').setLogic( LUCID_C.x(1) & ((ALU & BLU) | (ALL & BLL)) & bgrpcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_LUCID_A_ALFA_C_UNPAIRED_ISO').setLogic( LUCID_A.x(1) & ((ARU & BRU) | (ARL & BRL)) & unpaired_isocond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_LUCID_C_ALFA_A_UNPAIRED_ISO').setLogic( LUCID_C.x(1) & ((ALU & BLU) | (ALL & BLL)) & unpaired_isocond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_LUCID_ALFA').setLogic( (LUCID_A.x(1) | LUCID_C.x(1)) & ANY_ALFA & bgrpcond ).setTriggerType(TT.rpcout)    # noqa: F821

        # ALFA and ZDC
        LVL1MenuItem('L1_ZDC_A_ALFA_C').setLogic( ZDC_A.x(1) & ((ARU & BRU) | (ARL & BRL)) & bgrpcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ZDC_C_ALFA_A').setLogic( ZDC_C.x(1) & ((ALU & BLU) | (ALL & BLL)) & bgrpcond ).setTriggerType(TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_ZDC_ALFA').setLogic( (ZDC_A.x(1) | ZDC_C.x(1)) & ANY_ALFA & bgrpcond ).setTriggerType(TT.rpcout)    # noqa: F821

        # new combined ALFA items for 2012 running
        LVL1MenuItem('L1_EM3_ALFA_ANY'   ).setLogic( EM3.x(1) & ANY_ALFA & physcond ).setTriggerType( TT.calo | TT.caloem | TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_EM3_ALFA_MBTS_A').setLogic( EM3.x(1) & MBTS_A.x(1) & ((BRU & ARU) | (BRL & ARL)) & physcond).setTriggerType( TT.calo | TT.caloem | TT.minb | TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_EM3_ALFA_MBTS_C').setLogic( EM3.x(1) & MBTS_C.x(1) & ((BLU & ALU) | (BLL & ALL)) & physcond).setTriggerType( TT.calo | TT.caloem | TT.minb | TT.rpcout)    # noqa: F821
        
        LVL1MenuItem('L1_EM3_ALFA_ANY_UNPAIRED_ISO'   ).setLogic( EM3.x(1) & ANY_ALFA & unpaired_isocond).setTriggerType( TT.calo | TT.caloem | TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_EM3_ALFA_MBTS_C_UNPAIRED_ISO').setLogic( EM3.x(1) & MBTS_C.x(1) & ((BLU & ALU) | (BLL & ALL)) & unpaired_isocond).setTriggerType( TT.calo | TT.caloem | TT.minb | TT.rpcout)    # noqa: F821
        LVL1MenuItem('L1_EM3_ALFA_MBTS_A_UNPAIRED_ISO').setLogic( EM3.x(1) & MBTS_A.x(1) & ((BRU & ARU) | (BRL & ARL)) & unpaired_isocond).setTriggerType( TT.calo | TT.caloem | TT.minb | TT.rpcout)    # noqa: F821


        LVL1MenuItem('L1_TOPO_DPHI', ctpid=300).setLogic( TOPO_DPHI.x(1) & physcond).setTriggerType( TT.calo )    # noqa: F821


        # =======================================================

        LVL1MenuItem.currentPartition = 1
        LVL1MenuItem.l1configForRegistration = None
