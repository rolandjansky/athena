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
            exec("%s = ThrCondition(thr)" % thr.getVarName())
            
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
        bgrp9cond           = BGRP0 & BGRP9 
        bgrp11cond          = BGRP0 & BGRP11 
        bgrp12cond          = BGRP0 & BGRP12
        
        alfacalib           = BGRP0 & BGRP10
        abortgap            = BGRP0 & BGRP8
        
        # partition 1
        bgrpcond1           = BGRP0 & BGRP11
        calibcond1          = BGRP0 & BGRP12

        # partition 2
        bgrpcond2           = BGRP0 & BGRP14


        LVL1MenuItem.currentPartition = 1


        # E/gamma ctpid=[0:0x1f]
        LVL1MenuItem('L1_EM3'       ).setLogic( EM3        & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM3HI'     ).setLogic( EM3HI      & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM4'       ).setLogic( EM4        & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM5'       ).setLogic( EM5        & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM7'       ).setLogic( EM7        & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM10'      ).setLogic( EM10       & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM24VHI'      ).setLogic( EM24VHI & physcond).setTriggerType( TT.calo)
        LVL1MenuItem('L1_EM8'       ).setLogic( EM8        & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM8I'      ).setLogic( EM8I       & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM12'      ).setLogic( EM12       & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM14'      ).setLogic( EM14       & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15'      ).setLogic( EM15       & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15_BGRP7').setLogic( EM15       & bgrp7cond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15I'     ).setLogic( EM15I      & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15HI'    ).setLogic( EM15HI     & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM16'      ).setLogic( EM16       & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM8VH'     ).setLogic( EM8VH      & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM10VH'    ).setLogic( EM10VH     & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM20A'     ).setLogic( EM20A      & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM20C'     ).setLogic( EM20C      & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM13VH'    ).setLogic( EM13VH     & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15VH'    ).setLogic( EM15VH     & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM18'      ).setLogic( EM18       & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM18VH'    ).setLogic( EM18VH     & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM20'      ).setLogic( EM20       & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM20VH'    ).setLogic( EM20VH     & physcond).setTriggerType( TT.calo )  
        LVL1MenuItem('L1_EM20VHI'   ).setLogic( EM20VHI    & physcond).setTriggerType( TT.calo )  # isolation not yet defined
        LVL1MenuItem('L1_EM22'      ).setLogic( EM22       & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM22VHI'   ).setLogic( EM22VHI    & physcond).setTriggerType( TT.calo )  # isolation not yet defined
        LVL1MenuItem('L1_EM50V'     ).setLogic( EM50V      & physcond).setTriggerType( TT.calo )



        # 2xEM, 3xEM
        LVL1MenuItem('L1_2EM3'           ).setLogic( EM3.x(2)             & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_2EM5'           ).setLogic( EM5.x(2)             & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_2EM7'           ).setLogic( EM7.x(2)             & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_2EM10'          ).setLogic( EM10.x(2)            & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_2EM10VH'        ).setLogic( EM10VH.x(2)          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_2EM13VH'        ).setLogic( EM13VH.x(2)          & physcond).setTriggerType( TT.calo )                
        LVL1MenuItem('L1_2EM15'          ).setLogic( EM15.x(2)            & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_2EM15VH'        ).setLogic( EM15VH.x(2)          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM7_2EM3'       ).setLogic( EM7 & EM3.x(2)       & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM12_2EM3'      ).setLogic( EM12 & EM3.x(2)      & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM20VH_2EM7'    ).setLogic( EM20VH & EM7.x(2)    & physcond).setTriggerType( TT.calo )        

        # 4xEM
        LVL1MenuItem('L1_EM15VH_3EM7'   ).setLogic( EM15VH & EM7.x(3)    & physcond).setTriggerType( TT.calo )

        # EM and jet
        LVL1MenuItem('L1_JJ15.23ETA49' ).setLogic( JJ1523ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J15.23ETA49' ).setLogic( J1523ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J15.24ETA49' ).setLogic( J1524ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J15.31ETA49' ).setLogic( J1531ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J20.28ETA31' ).setLogic( J2028ETA31 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J20.31ETA49' ).setLogic( J2031ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30.31ETA49' ).setLogic( J3031ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J40.0ETA25' ).setLogic( J400ETA25 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J50.31ETA49' ).setLogic( J5031ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J70.31ETA49' ).setLogic( J7031ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J100.31ETA49' ).setLogic( J10031ETA49 & physcond).setTriggerType(TT.calo)

        LVL1MenuItem('L1_J15.0ETA25'  ).setLogic( J150ETA25 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J20.0ETA28'  ).setLogic( J200ETA28 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J25.0ETA23'  ).setLogic( J250ETA23 & physcond).setTriggerType(TT.calo)

        LVL1MenuItem('L1_J20.0ETA49'  ).setLogic( J200ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30.0ETA49'  ).setLogic( J300ETA49 & physcond).setTriggerType(TT.calo)
        









        # HI
        LVL1MenuItem('L1_EM3_NZ'         ).setLogic( EM3      & Not(ZDC_AND) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_2EM3_NZ'        ).setLogic( EM3.x(2) & Not(ZDC_AND) & physcond).setTriggerType( TT.calo )   

        LVL1MenuItem('L1_EM3_NL'         ).setLogic( EM3      & Not(LUCID_A) & Not(LUCID_C) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_2EM3_NL'        ).setLogic( EM3.x(2) & Not(LUCID_A) & Not(LUCID_C) & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_EM3_MV'         ).setLogic( EM3      & Not(MBTS_A & MBTS_C) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM3_MV_VTE50'   ).setLogic( EM3      & Not(MBTS_A & MBTS_C) & Not(TE50) & physcond).setTriggerType( TT.calo ) 

        LVL1MenuItem('L1_EM3_TE50'       ).setLogic( EM3      & TE50 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM3_VTE50'      ).setLogic( EM3      & Not(TE50) & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_EM3_UNPAIRED_ISO'   ).setLogic(EM3 & unpaired_isocond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM3_UNPAIRED_NONISO').setLogic(EM3 & unpaired_nonisocond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM3_EMPTY'          ).setLogic(EM3 & cosmiccond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM3_FIRSTEMPTY'     ).setLogic(EM3 & firstempty).setTriggerType( TT.calo )

        LVL1MenuItem('L1_EM5_EMPTY'          ).setLogic(EM5 & cosmiccond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM5_UNPAIRED_ISO'   ).setLogic(EM5 & unpaired_isocond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM5_UNPAIRED_NONISO').setLogic(EM5 & unpaired_nonisocond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM5_FIRSTEMPTY'     ).setLogic(EM5 & firstempty).setTriggerType( TT.calo )

        LVL1MenuItem('L1_EM7_EMPTY'          ).setLogic(EM7 & cosmiccond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM7_UNPAIRED_ISO'   ).setLogic(EM7 & unpaired_isocond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM7_UNPAIRED_NONISO').setLogic(EM7 & unpaired_nonisocond).setTriggerType( TT.calo ) 
        LVL1MenuItem('L1_EM7_FIRSTEMPTY'     ).setLogic(EM7 & firstempty).setTriggerType( TT.calo )

        LVL1MenuItem('L1_EM20VH_FIRSTEMPTY'    ).setLogic( EM20VH & firstempty).setTriggerType( TT.calo )  
        LVL1MenuItem('L1_EM22VHI_FIRSTEMPTY'    ).setLogic( EM22VHI & firstempty).setTriggerType( TT.calo )

        LVL1MenuItem('L1_J10_VTE100'      ).setLogic( J10      & Not(TE100) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_J10_VTE200'      ).setLogic( J10      & Not(TE200) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_J10_VTE300'      ).setLogic( J10      & Not(TE300) & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_TE5_VTE200'      ).setLogic( TE5      & Not(TE200) & physcond).setTriggerType( TT.calo )


        # MUON ctpid=[0x20;0x2f]
        LVL1MenuItem('L1_MU0'  ).setLogic( MU0  & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU4'  ).setLogic( MU4  & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU6'  ).setLogic( MU6  & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU10' ).setLogic( MU10 & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU11' ).setLogic( MU11 & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU15' ).setLogic( MU15 & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU20' ).setLogic( MU20 & physcond).setTriggerType(TT.muon)

        # 2xMU, 3xMU, 4xMU
        LVL1MenuItem('L1_2MU0'     ).setLogic( MU0.x(2)        & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_2MU4'     ).setLogic( MU4.x(2)        & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_2MU6'     ).setLogic( MU6.x(2)        & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_2MU10'    ).setLogic( MU10.x(2)       & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_2MU11'    ).setLogic( MU11.x(2)       & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_2MU20'    ).setLogic( MU20.x(2)       & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU4_MU10' ).setLogic( MU4 & MU10      & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU10_2MU6').setLogic( MU10 & MU6.x(2) & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU11_2MU6').setLogic( MU11 & MU6.x(2) & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU10_2MU4').setLogic( MU10 & MU4.x(2) & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_3MU4'     ).setLogic( MU4.x(3)        & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU6_2MU4' ).setLogic( MU6 & MU4.x(2)  & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU6_3MU4' ).setLogic( MU6 & MU4.x(3)  & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_2MU6_3MU4' ).setLogic( MU6.x(2) & MU4.x(3)  & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_3MU6'     ).setLogic( MU6.x(3)        & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_2MU0_MU6' ).setLogic( MU0.x(2) & MU6  & physcond).setTriggerType(TT.muon)

        LVL1MenuItem('L1_2MU20_OVERLAY').setLogic( MU20.x(2)   & physcond).setTriggerType(TT.zerobs)

        # HI
        LVL1MenuItem('L1_MU0_NZ'   ).setLogic( MU0      & Not(ZDC_AND) & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_2MU0_NZ'  ).setLogic( MU0.x(2) & Not(ZDC_AND) & physcond).setTriggerType(TT.muon)   

        LVL1MenuItem('L1_MU0_NL'   ).setLogic( MU0      & Not(LUCID_A) & Not(LUCID_C) & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_2MU0_NL'  ).setLogic( MU0.x(2) & Not(LUCID_A) & Not(LUCID_C) & physcond).setTriggerType(TT.muon)                  
                            
        LVL1MenuItem('L1_MU0_MV'   ).setLogic( MU0      & Not(MBTS_A & MBTS_C) & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_2MU0_MV'  ).setLogic( MU0.x(2) & Not(MBTS_A & MBTS_C) & physcond).setTriggerType(TT.muon)
        
        LVL1MenuItem('L1_MU0_TE50' ).setLogic( MU0      & TE50 & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU4_TE50' ).setLogic( MU4      & TE50 & physcond).setTriggerType(TT.muon)
        
        LVL1MenuItem('L1_MU0_VTE50').setLogic( MU0      & Not(TE50) & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU4_VTE50').setLogic( MU4      & Not(TE50) & physcond).setTriggerType(TT.muon)

        LVL1MenuItem('L1_MU0_MV_VTE50').setLogic( MU0 & Not(MBTS_A & MBTS_C) & Not(TE50) & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_MU4_MV_VTE50').setLogic( MU4 & Not(MBTS_A & MBTS_C) & Not(TE50) & physcond).setTriggerType(TT.muon)

        LVL1MenuItem('L1_MU0_UNPAIRED_ISO'   ).setLogic(MU0 & unpaired_isocond   ).setTriggerType( TT.muon )
        LVL1MenuItem('L1_MU0_UNPAIRED_NONISO').setLogic(MU0 & unpaired_nonisocond).setTriggerType( TT.muon )
        LVL1MenuItem('L1_MU0_EMPTY'          ).setLogic(MU0 & cosmiccond).setTriggerType( TT.muon )
        LVL1MenuItem('L1_MU0_FIRSTEMPTY'     ).setLogic(MU0 & firstempty).setTriggerType( TT.muon )

        LVL1MenuItem('L1_MU4_UNPAIRED_ISO'   ).setLogic(MU4 & unpaired_isocond   ).setTriggerType( TT.muon )
        LVL1MenuItem('L1_MU4_UNPAIRED_NONISO').setLogic(MU4 & unpaired_nonisocond).setTriggerType( TT.muon )
        LVL1MenuItem('L1_MU4_EMPTY'          ).setLogic(MU4 & cosmiccond).setTriggerType( TT.muon )
        LVL1MenuItem('L1_MU4_FIRSTEMPTY'     ).setLogic(MU4 & firstempty).setTriggerType( TT.muon )

        LVL1MenuItem('L1_MU6_FIRSTEMPTY'     ).setLogic(MU4  & firstempty).setTriggerType( TT.muon ) 
        LVL1MenuItem('L1_MU11_EMPTY'         ).setLogic(MU11 & cosmiccond).setTriggerType( TT.muon ) 
        
        LVL1MenuItem('L1_2MU0_EMPTY'         ).setLogic(MU0.x(2) & cosmiccond).setTriggerType( TT.muon )
        LVL1MenuItem('L1_2MU4_EMPTY'         ).setLogic(MU4.x(2) & cosmiccond).setTriggerType( TT.muon ) 
        LVL1MenuItem('L1_2MU6_UNPAIRED_ISO'  ).setLogic(MU6.x(2) & unpaired_isocond).setTriggerType( TT.muon )
        LVL1MenuItem('L1_2MU6_UNPAIRED_NONISO').setLogic(MU6.x(2) & unpaired_nonisocond).setTriggerType( TT.muon )
        LVL1MenuItem('L1_2MU6_EMPTY'         ).setLogic(MU6.x(2) & cosmiccond).setTriggerType( TT.muon )
        LVL1MenuItem('L1_2MU6_FIRSTEMPTY'    ).setLogic(MU6.x(2) & firstempty).setTriggerType( TT.muon )

        LVL1MenuItem('L1_MU20_FIRSTEMPTY' ).setLogic( MU20 & firstempty).setTriggerType(TT.muon)

        
        # EM and MU
        LVL1MenuItem('L1_EM6_MU10'   ).setLogic( EM6        & MU10     & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_EM7_MU10'   ).setLogic( EM7        & MU10     & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_EM8_MU10'   ).setLogic( EM8        & MU10     & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_EM8I_MU10'  ).setLogic( EM8I       & MU10     & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_EM15I_MU4'   ).setLogic( EM15I       & MU4      & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_EM15VH_MU10').setLogic( EM15VH     & MU10     & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_EM3_MU20'   ).setLogic( EM3        & MU20     & physcond).setTriggerType(TT.muon)
        LVL1MenuItem('L1_2EM8VH_MU10').setLogic( EM8VH.x(2) & MU10     & physcond).setTriggerType(TT.muon)

        # TAU ctpid=[0x40:0x4f]
        LVL1MenuItem('L1_TAU6'  ).setLogic( HA6   & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU8'  ).setLogic( HA8   & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_TAU10IM').setLogic( HA10IM  & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_TAU12' ).setLogic( HA12  & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_TAU12IL').setLogic( HA12IL & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12IM').setLogic( HA12IM & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12IT').setLogic( HA12IT & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12I').setLogic( HA12I & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_TAU15' ).setLogic( HA15  & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_TAU20' ).setLogic( HA20  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20IL' ).setLogic( HA20IL  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20IM' ).setLogic( HA20IM  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20IT' ).setLogic( HA20IT  & physcond).setTriggerType( TT.calo )
        
        LVL1MenuItem('L1_TAU20A' ).setLogic( HA20A  & physcond).setTriggerType( TT.calo ) 
        LVL1MenuItem('L1_TAU20C' ).setLogic( HA20C  & physcond).setTriggerType( TT.calo ) 

        LVL1MenuItem('L1_TAU25' ).setLogic( HA25  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU25IT' ).setLogic( HA25IT  & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_TAU30' ).setLogic( HA30  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU40' ).setLogic( HA40  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU60' ).setLogic( HA60  & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_TAU8_UNPAIRED_ISO'   ).setLogic( HA8   & unpaired_isocond ).setTriggerType( TT.calo ) 

        LVL1MenuItem('L1_TAU8_UNPAIRED_NONISO').setLogic( HA8   & unpaired_nonisocond ).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU8_EMPTY'          ).setLogic( HA8   & cosmiccond ).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU8_FIRSTEMPTY'     ).setLogic( HA8   & firstempty ).setTriggerType( TT.calo )

        LVL1MenuItem('L1_TAU12_UNPAIRED_ISO').setLogic( HA12   & unpaired_isocond ).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12_UNPAIRED_NONISO').setLogic( HA12   & unpaired_nonisocond ).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12_EMPTY'          ).setLogic( HA12  & cosmiccond ).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12_FIRSTEMPTY'     ).setLogic( HA12   & firstempty ).setTriggerType( TT.calo )

        # 3xTAU
        LVL1MenuItem('L1_TAU20_2TAU12'  ).setLogic( HA20 & HA12.x(2)  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20_2TAU12I' ).setLogic( HA20 & HA12I.x(2) &  physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU40_2TAU20IM' ).setLogic( HA40 & HA20IM.x(2) &  physcond).setTriggerType( TT.calo )	
        LVL1MenuItem('L1_TAU20IM_2TAU12IM' ).setLogic( HA20IM & HA12IM.x(2) &  physcond).setTriggerType( TT.calo )

        # mixed tau
        LVL1MenuItem('L1_EM15VH_2TAU12'                ).setLogic( EM15VH  & HA12.x(2)          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15VH_2TAU12IL'              ).setLogic( EM15VH  & HA12IL.x(2)        & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15HI_2TAU12'                ).setLogic( EM15HI  & HA12.x(2)          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15HI_2TAU12IL'              ).setLogic( EM15HI  & HA12IL.x(2)        & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15HI_2TAU12IM'              ).setLogic( EM15HI  & HA12IM.x(2)        & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15_2TAU12'                  ).setLogic( EM15    & HA12.x(2)          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15_2TAU20'                  ).setLogic( EM15    & HA20.x(2)          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15VH_2TAU20'                ).setLogic( EM15VH  & HA20.x(2)          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15_TAU40_2TAU15'            ).setLogic( EM15    & HA40 & HA15.x(2)   & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15HI_TAU40_2TAU15'          ).setLogic( EM15HI  & HA40 & HA15.x(2)   & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_EM15_2TAU12_J25_2J15_3J12'    ).setLogic( EM15  & HA12.x(2)  & J25 & J15.x(2) & J12.x(3) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15HI_2TAU12I_J25_2J15_3J12'  ).setLogic( EM15HI  & HA12I.x(2)  & J25 & J15.x(2) & J12.x(3) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15HI_2TAU12IM_J25_3J12'  ).setLogic( EM15HI  & HA12IM.x(2)  & J25 & J12.x(3) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15HI_2TAU12IL_J25_3J12'  ).setLogic( EM15HI  & HA12IL.x(2)  & J25 & J12.x(3) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15HI_2TAU12_J25_3J12'  ).setLogic( EM15HI  & HA12.x(2)  & J25 & J12.x(3) & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_MU10_TAU12'       ).setLogic( MU10  & HA12          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU12I'      ).setLogic( MU10  & HA12I         & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU12IM'     ).setLogic( MU10  & HA12IM        & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU20'       ).setLogic( MU10  & HA20          & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU20IM'     ).setLogic( MU10  & HA20IM        & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU20I'       ).setLogic( MU10  & HA20I        & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12I_MU10_J25'  ).setLogic( HA12I & MU10 & J25    & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12I_MU10_J25_2J12'  ).setLogic( HA12I & MU10 & J25 & J12.x(2)  & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU12_J25_2J12' ).setLogic( MU10 & HA12 & J25 & J12.x(2)     & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU12IM_J25_2J12' ).setLogic( MU10 & HA12IM & J25 & J12.x(2)    & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU12IL_J25_2J12' ).setLogic( MU10 & HA12IL & J25 & J12.x(2)    & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU12I_MU10_J20.31ETA49' ).setLogic( HA12I & MU10 & J2031ETA49    & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20_2TAU12I_J25_2J15_3J12'   ).setLogic( HA20 & HA12I.x(2)     & J25 & J15.x(2) & J12.x(3) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20_2TAU12_J25_2J20_3J12'   ).setLogic( HA20 & HA12.x(2)     & J25 & J20.x(2) & J12.x(3) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20IM_2TAU12IM_J25_2J20_3J12').setLogic( HA20IM & HA12IM.x(2)     & J25 & J20.x(2) & J12.x(3) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20IL_2TAU12IL_J25_2J20_3J12').setLogic( HA20IL & HA12IL.x(2)     & J25 & J20.x(2) & J12.x(3) & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_TAU25IT_2TAU12IT_2J25_3J12').setLogic( HA25IT & HA12IT.x(2)     & J25.x(2)  & J12.x(3) & physcond).setTriggerType( TT.calo )

        LVL1MenuItem('L1_2TAU12I_TAU20_J25_2J15_3J12'  ).setLogic( HA12I.x(2)   & HA20  & J25 & J15.x(2) & J12.x(3) & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20_2J20_XE45'              ).setLogic( HA20    & J20.x(2)   & XE45 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20IM_2J20_XE50'            ).setLogic( HA20IM   & J20.x(2)   & XE50 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20IM_2J20_XE45'            ).setLogic( HA20IM    & J20.x(2)   & XE45 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU25_2J20_XE45'              ).setLogic( HA25      & J20.x(2)   & XE45 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20_2TAU12_XE35'            ).setLogic( HA20    & HA12.x(2)  & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20IM_2TAU12IM_XE35'        ).setLogic( HA20IM  & HA12IM.x(2)  & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20IL_2TAU12IL_XE35'        ).setLogic( HA20IL  & HA12IL.x(2)  & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20IT_2TAU12IT_XE35'        ).setLogic( HA20IT  & HA12IT.x(2)  & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20IM_2TAU12IM_XE40'        ).setLogic( HA20IM  & HA12IM.x(2)  & XE40 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15HI_2TAU12I_XE35'          ).setLogic( EM15HI  & HA12I.x(2) & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15HI_2TAU12IM_XE35'         ).setLogic( EM15HI  & HA12IM.x(2) & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15HI_2TAU12IL_XE35'         ).setLogic( EM15HI  & HA12IL.x(2) & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_EM15HI_2TAU12_XE35'         ).setLogic( EM15HI  & HA12.x(2) & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU12I_XE35'             ).setLogic( MU10    & HA12I      & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU12_XE35'              ).setLogic( MU10    & HA12       & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU12IL_XE35'            ).setLogic( MU10    & HA12IL     & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU12IM_XE35'            ).setLogic( MU10    & HA12IM     & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU12IT_XE35'            ).setLogic( MU10    & HA12IT     & XE35 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_MU10_TAU12IM_XE40'            ).setLogic( MU10    & HA12IM     & XE40 & physcond).setTriggerType( TT.calo )
        LVL1MenuItem('L1_TAU20I_2TAU12I_XE35'          ).setLogic( HA20I   & HA12I.x(2) & XE35 & physcond).setTriggerType( TT.calo )




        # JET ctpid=[0x60:0x7f]
        LVL1MenuItem('L1_J5'   ).setLogic( J5   & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J10'   ).setLogic( J10  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J12'   ).setLogic( J12  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J15'   ).setLogic( J15  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J20'   ).setLogic( J20  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J25'   ).setLogic( J25  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30'   ).setLogic( J30  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J35'   ).setLogic( J35  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J40'   ).setLogic( J40  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J50'   ).setLogic( J50  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J60'   ).setLogic( J60  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J75'   ).setLogic( J75  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J75A'  ).setLogic( J75A & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J75C'  ).setLogic( J75C & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J85'   ).setLogic( J85  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J100'  ).setLogic( J100 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J120'  ).setLogic( J120 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J150'  ).setLogic( J150 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J175'  ).setLogic( J175 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J250'  ).setLogic( J250 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J400'  ).setLogic( J400 & physcond).setTriggerType(TT.calo)

        LVL1MenuItem('L1_J10.31ETA49').setLogic( J1031ETA49 & physcond).setTriggerType(TT.calo)
        #LVL1MenuItem('L1_J20.31ETA49').setLogic( J2031ETA49 & physcond).setTriggerType(TT.calo)
        #LVL1MenuItem('L1_J30.31ETA49').setLogic( J3031ETA49 & physcond).setTriggerType(TT.calo)
        #LVL1MenuItem('L1_J50.31ETA49').setLogic( J5031ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J75.31ETA49').setLogic( J7531ETA49 & physcond).setTriggerType(TT.calo)
        #LVL1MenuItem('L1_J100.31ETA49').setLogic( J10031ETA49 & physcond).setTriggerType(TT.calo)


        LVL1MenuItem('L1_J10_UNPAIRED_ISO'   ).setLogic( J10 & unpaired_isocond   ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J10_UNPAIRED_NONISO').setLogic( J10 & unpaired_nonisocond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J10_EMPTY').setLogic( J10 & cosmiccond ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J10_FIRSTEMPTY').setLogic( J10 & firstempty ).setTriggerType(TT.calo)

        LVL1MenuItem('L1_J12_UNPAIRED_ISO'   ).setLogic( J12 & unpaired_isocond   ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J12_UNPAIRED_NONISO').setLogic( J12 & unpaired_nonisocond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J12_EMPTY').setLogic( J12 & cosmiccond ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J12_FIRSTEMPTY').setLogic( J12 & firstempty ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J12_ABORTGAPNOTCALIB').setLogic( J12 & abortgap ).setTriggerType(TT.calo)

        LVL1MenuItem('L1_J50_UNPAIRED_ISO'   ).setLogic( J50 & unpaired_isocond   ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J50_UNPAIRED_NONISO').setLogic( J50 & unpaired_nonisocond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J50_ABORTGAPNOTCALIB').setLogic( J50 & abortgap ).setTriggerType(TT.calo)

        LVL1MenuItem('L1_J12_BGRP12').setLogic( J12 & bgrp12cond ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30.31ETA49_BGRP12').setLogic( J3031ETA49 & bgrp12cond ).setTriggerType(TT.calo)

        LVL1MenuItem('L1_J30_EMPTY').setLogic( J30 & cosmiccond ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30_UNPAIRED').setLogic( J30 & unpairedRcond ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30_FIRSTEMPTY').setLogic( J30 & firstempty ).setTriggerType(TT.calo)

        LVL1MenuItem('L1_J10.31ETA49_EMPTY').setLogic( J1031ETA49 & cosmiccond ).setTriggerType(TT.calo)



        LVL1MenuItem('L1_J30.31ETA49_EMPTY').setLogic( J3031ETA49 & cosmiccond ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30.31ETA49_FIRSTEMPTY').setLogic( J3031ETA49 & firstempty ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30.31ETA49_UNPAIRED_ISO'   ).setLogic( J3031ETA49 & unpaired_isocond   ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30.31ETA49_UNPAIRED_NONISO'   ).setLogic( J3031ETA49 & unpaired_nonisocond   ).setTriggerType(TT.calo)

        
        LVL1MenuItem('L1_J100_FIRSTEMPTY').setLogic( J100 & firstempty ).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J100.31ETA49_FIRSTEMPTY' ).setLogic( J10031ETA49 & firstempty ).setTriggerType(TT.calo)

        # multi jet
        LVL1MenuItem('L1_2J25.31ETA49'  ).setLogic( J2531ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_2J15'   ).setLogic( J15.x(2)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J15_J15.31ETA49' ).setLogic( J15 & J1531ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J20_J20.31ETA49' ).setLogic( J20 & J2031ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_3J15'   ).setLogic( J15.x(3)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_3J20'   ).setLogic( J20.x(3)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_3J40'   ).setLogic( J40.x(3)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_3J50'   ).setLogic( J50.x(3)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_3J75'   ).setLogic( J75.x(3)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_4J15'   ).setLogic( J15.x(4)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_4J20'   ).setLogic( J20.x(4)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_4J20.0ETA49').setLogic( J200ETA49.x(4) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_4J30'   ).setLogic( J30.x(4)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_6J15'   ).setLogic( J15.x(6)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J75_3J20' ).setLogic( J75 & J20.x(3)    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30.0ETA49_2J20.0ETA49' ).setLogic( J300ETA49 & J200ETA49.x(2)    & physcond).setTriggerType(TT.calo)
        
        LVL1MenuItem('L1_4J17.0ETA22' ).setLogic( J170ETA22.x(4) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_4J15.0ETA25' ).setLogic( J150ETA25.x(4) & physcond).setTriggerType(TT.calo) 
        LVL1MenuItem('L1_3J25.0ETA23' ).setLogic( J250ETA23.x(3) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_3J15.0ETA25' ).setLogic( J150ETA25.x(3) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_5J15.0ETA25' ).setLogic( J150ETA25.x(5) & physcond).setTriggerType(TT.calo)

#        LVL1MenuItem('L1_ZB_J20').setLogic(ZB_EM15 & J20 & physcond).setTriggerType(TT.zerobs)
        if (('Physics_HI_v3' in TriggerFlags.triggerMenuSetup()) or ('MC_HI_v3' in TriggerFlags.triggerMenuSetup())):
            LVL1MenuItem('L1_ZB', ctpid=240).setLogic(ZB_J75 & physcond).setTriggerType(TT.zerobs)
        else:
            LVL1MenuItem('L1_ZB', ctpid=240).setLogic(ZB_EM15 & physcond).setTriggerType(TT.zerobs)


        # combined jet - xe
        #LVL1MenuItem('L1_J40_XE50').setLogic( J75 & XE40 & physcond).setTriggerType(TT.calo)#CB ERROR???
        LVL1MenuItem('L1_J40_XE50').setLogic( J40 & XE50 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J40.0ETA25_XE50').setLogic( J400ETA25 & XE50 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J75_XE40').setLogic( J75 & XE40 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_2J15_XE55').setLogic( J15.x(2) & XE55 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_2J40_XE45').setLogic( J40.x(2) & XE45 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_2J50_XE40').setLogic( J50.x(2) & XE50 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J40_XE60').setLogic( J40 & XE60 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_J30.0ETA49_XE50').setLogic( J300ETA49 & XE50 & physcond).setTriggerType(TT.calo)

        # combined em - jet
        LVL1MenuItem('L1_EM15VH_JJ15.23ETA49' ).setLogic( EM15VH  & JJ1523ETA49  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_EM13VH_3J20' ).setLogic( EM13VH  & J20.x(3)  & physcond).setTriggerType(TT.calo)

        # combined mu - jet
        LVL1MenuItem('L1_MU4_J12'   ).setLogic( MU4 & J12    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_MU6_J20'   ).setLogic( MU6 & J20    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_MU6_J30.0ETA49_2J20.0ETA49').setLogic( MU6 & J300ETA49 & J200ETA49.x(2) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_MU6_J40'   ).setLogic( MU6 & J40    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_MU6_J75'   ).setLogic( MU6 & J75    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_MU4_3J15'  ).setLogic( MU4 & J15.x(3)  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_MU4_3J20'  ).setLogic( MU4 & J20.x(3)  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_MU4_J30'   ).setLogic( MU4 & J30    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_MU4_J50'   ).setLogic( MU4 & J50    & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_MU10_3J20'  ).setLogic( MU10 & J20.x(3)  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_MU10_2J20'  ).setLogic( MU10 & J20.x(2)  & physcond).setTriggerType(TT.calo)
        # HI
        LVL1MenuItem('L1_J15_NZ' ).setLogic( J15      & Not(ZDC_AND) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_2J15_NZ').setLogic( J15.x(2) & Not(ZDC_AND) & physcond).setTriggerType(TT.calo)   

        LVL1MenuItem('L1_J15_NL' ).setLogic( J15      & Not(LUCID_A) & Not(LUCID_C) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_2J15_NL').setLogic( J15.x(2) & Not(LUCID_A) & Not(LUCID_C) & physcond).setTriggerType(TT.calo)
        
        # XE
        LVL1MenuItem('L1_XE35').setLogic( XE35 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE40').setLogic( XE40 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE45').setLogic( XE45 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE50').setLogic( XE50 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE55').setLogic( XE55 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE60').setLogic( XE60 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE70').setLogic( XE70 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE80').setLogic( XE80 & physcond).setTriggerType(TT.calo)


        LVL1MenuItem('L1_XE10').setLogic( XE10 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE20').setLogic( XE20 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE25').setLogic( XE25 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE30').setLogic( XE30 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE65').setLogic( XE65 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE75').setLogic( XE75 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE150').setLogic( XE150 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE300').setLogic( XE300 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE35_BGRP7').setLogic( XE35 & bgrp7cond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE40_BGRP7').setLogic( XE40 & bgrp7cond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE45_BGRP7').setLogic( XE45 & bgrp7cond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE50_BGRP7').setLogic( XE50 & bgrp7cond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE55_BGRP7').setLogic( XE55 & bgrp7cond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE60_BGRP7').setLogic( XE60 & bgrp7cond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE70_BGRP7').setLogic( XE70 & bgrp7cond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XE80_BGRP7').setLogic( XE80 & bgrp7cond).setTriggerType(TT.calo)	
        # XS
        LVL1MenuItem('L1_XS20').setLogic( XS20.x(1) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XS30').setLogic( XS30.x(1) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XS40').setLogic( XS40.x(1) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XS45').setLogic( XS45.x(1) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XS50').setLogic( XS50.x(1) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XS55').setLogic( XS55.x(1) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XS60').setLogic( XS60.x(1) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_XS65').setLogic( XS65.x(1) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_EM10_XS20').setLogic( EM10 & XS20.x(1) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_EM12_XS20').setLogic( EM12 & XS20.x(1) & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_EM15_XS30').setLogic( EM15 & XS30.x(1) & physcond).setTriggerType(TT.calo)
        
        # TE
        LVL1MenuItem('L1_TE0' ).setLogic( TE0  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE3' ).setLogic( TE3  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE5' ).setLogic( TE5  & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE10').setLogic( TE10 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE15').setLogic( TE15 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE20').setLogic( TE20 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE25').setLogic( TE25 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE30').setLogic( TE30 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE40').setLogic( TE40 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE45').setLogic( TE45 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE50').setLogic( TE50 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE55').setLogic( TE55 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE60').setLogic( TE60 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE65').setLogic( TE65 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE70').setLogic( TE70 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE100').setLogic( TE100 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE200').setLogic( TE200 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE300').setLogic( TE300 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE2000').setLogic( TE2000 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE4000').setLogic( TE4000 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE10000').setLogic( TE10000 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE12000').setLogic( TE12000 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE14000').setLogic( TE14000 & physcond).setTriggerType(TT.calo)

        LVL1MenuItem('L1_TE0.0ETA24').setLogic( TE00ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE3.0ETA24').setLogic( TE30ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE5.0ETA24').setLogic( TE50ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE10.0ETA24').setLogic( TE100ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE15.0ETA24').setLogic( TE150ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE20.0ETA24').setLogic( TE200ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE25.0ETA24').setLogic( TE250ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE30.0ETA24').setLogic( TE300ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE40.0ETA24').setLogic( TE400ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE45.0ETA24').setLogic( TE450ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE50.0ETA24').setLogic( TE500ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE55.0ETA24').setLogic( TE550ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE60.0ETA24').setLogic( TE600ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE65.0ETA24').setLogic( TE650ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE70.0ETA24').setLogic( TE700ETA24 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE2000.0ETA24').setLogic( TE20000ETA24 & physcond).setTriggerType(TT.calo)

	# HI items
        LVL1MenuItem('L1_TE500.0ETA49').setLogic( TE5000ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE1500.0ETA49').setLogic( TE15000ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE3000.0ETA49').setLogic( TE30000ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE3500.0ETA49').setLogic( TE35000ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE5000.0ETA49').setLogic( TE50000ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE6500.0ETA49').setLogic( TE65000ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE8000.0ETA49').setLogic( TE80000ETA49 & physcond).setTriggerType(TT.calo)
        LVL1MenuItem('L1_TE9000.0ETA49').setLogic( TE90000ETA49 & physcond).setTriggerType(TT.calo)

        LVL1MenuItem('L1_TE500.0ETA49_OVERLAY').setLogic( TE5000ETA49 & physcond).setTriggerType(TT.zerobs)
        LVL1MenuItem('L1_TE1500.0ETA49_OVERLAY').setLogic( TE15000ETA49 & physcond).setTriggerType(TT.zerobs)
        LVL1MenuItem('L1_TE3000.0ETA49_OVERLAY').setLogic( TE30000ETA49 & physcond).setTriggerType(TT.zerobs)
        LVL1MenuItem('L1_TE3500.0ETA49_OVERLAY').setLogic( TE35000ETA49 & physcond).setTriggerType(TT.zerobs)
        LVL1MenuItem('L1_TE5000.0ETA49_OVERLAY').setLogic( TE50000ETA49 & physcond).setTriggerType(TT.zerobs)
        LVL1MenuItem('L1_TE6500.0ETA49_OVERLAY').setLogic( TE65000ETA49 & physcond).setTriggerType(TT.zerobs)
        LVL1MenuItem('L1_TE8000.0ETA49_OVERLAY').setLogic( TE80000ETA49 & physcond).setTriggerType(TT.zerobs)
        LVL1MenuItem('L1_TE9000.0ETA49_OVERLAY').setLogic( TE90000ETA49 & physcond).setTriggerType(TT.zerobs)

        LVL1MenuItem('L1_TE5_NZ').setLogic( TE5 & Not(ZDC_AND) & physcond).setTriggerType(TT.calo) 
        
        # MBTS
        MBTS_1   = MBTS_A | MBTS_C
        MBTS_2   = (MBTS_A.x(2) | MBTS_C.x(2) | MBTS_A) & (MBTS_A.x(2) | MBTS_C.x(2) | MBTS_C)
        MBTS_1_1 = MBTS_A & MBTS_C
        MBTS_2_2 = MBTS_A.x(2) & MBTS_C.x(2)
        MBTS_3_3 = MBTS_A.x(3) & MBTS_C.x(3)
        MBTS_4_4 = MBTS_A.x(4) & MBTS_C.x(4)
        MBTS_4_A = MBTS_A.x(4)
        MBTS_4_C = MBTS_C.x(4)

        LVL1MenuItem('L1_MBTS_1'             ).setLogic( MBTS_1   & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2'             ).setLogic( MBTS_2   & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_1_1'           ).setLogic( MBTS_1_1 & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2_2'           ).setLogic( MBTS_2_2 & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_3_3'           ).setLogic( MBTS_3_3 & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_4_4'           ).setLogic( MBTS_4_4 & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_4_A'           ).setLogic( MBTS_4_A & physcond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_4_C'           ).setLogic( MBTS_4_C & physcond ).setTriggerType(TT.minb)

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
        
        LVL1MenuItem('L1_MBTS_1_BGRP9'    ).setLogic( MBTS_1 & bgrp9cond ).setTriggerType(TT.minb) 
        LVL1MenuItem('L1_MBTS_1_1_BGRP9'  ).setLogic( MBTS_1_1 & bgrp9cond ).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2_BGRP9'    ).setLogic( MBTS_2 & bgrp9cond ).setTriggerType(TT.minb)
        
        LVL1MenuItem('L1_MBTS_1_BGRP11'   ).setLogic( MBTS_1 & bgrp11cond ).setTriggerType(TT.minb) 
        LVL1MenuItem('L1_MBTS_1_1_BGRP11' ).setLogic( MBTS_1_1 & bgrp11cond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_MBTS_2_BGRP11'   ).setLogic( MBTS_2 & bgrp11cond ).setTriggerType(TT.minb) 


        # ZDC
        ZDC_A_C = ZDC_A & ZDC_C
        
        LVL1MenuItem('L1_ZDC'               ).setLogic((ZDC_A | ZDC_C) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_A'             ).setLogic( ZDC_A & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_C'             ).setLogic( ZDC_C & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_AND'           ).setLogic( ZDC_AND & physcond).setTriggerType(TT.minb) 
        LVL1MenuItem('L1_ZDC_A_C'           ).setLogic( ZDC_A_C & physcond).setTriggerType(TT.minb)

        LVL1MenuItem('L1_ZDC_A_C_OVERLAY'   ).setLogic( ZDC_A_C & physcond).setTriggerType(TT.zerobs)
        LVL1MenuItem('L1_ZDC_A_C_VTE50_OVERLAY'     ).setLogic( ZDC_A_C & Not(TE50) & physcond).setTriggerType(TT.zerobs)     
        LVL1MenuItem('L1_TE50_OVERLAY'   ).setLogic( TE50 & physcond).setTriggerType(TT.zerobs)

        LVL1MenuItem('L1_ZDC_VTE200'        ).setLogic((ZDC_A | ZDC_C) & Not(TE200) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_AND_VTE50'     ).setLogic( ZDC_AND & Not(TE50) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_A_C_VTE50'     ).setLogic( ZDC_A_C & Not(TE50) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_A_C_TE50'      ).setLogic( ZDC_A_C & TE50 & physcond).setTriggerType(TT.minb)

        LVL1MenuItem('L1_ZDC_A_C_VTE200'           ).setLogic( ZDC_A_C & Not(TE200) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_A_C_TE5_VTE200'       ).setLogic( ZDC_A_C & TE5 & Not(TE200) & physcond).setTriggerType(TT.minb)
	
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

        LVL1MenuItem('L1_ZDC_A_C_BGRP9'      ).setLogic( ZDC_A_C & bgrp9cond & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_A_C_BGRP11'     ).setLogic( ZDC_A_C & bgrp11cond & physcond).setTriggerType(TT.minb)

	# ATR-12470
        ZDC_XOR = (ZDC_A & Not(ZDC_C)) | (ZDC_C & Not(ZDC_A))
        LVL1MenuItem('L1_ZDC_A_VZDC_C'      ).setLogic(ZDC_A & Not(ZDC_C) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_C_VZDC_A'      ).setLogic(ZDC_C & Not(ZDC_A) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_A_VZDC_C_VTE200'      ).setLogic(ZDC_A & Not(ZDC_C) & Not(TE200) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_C_VZDC_A_VTE200'      ).setLogic(ZDC_C & Not(ZDC_A) & Not(TE200) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_XOR'           ).setLogic(ZDC_XOR  & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_XOR_TE5_VTE200'           ).setLogic(ZDC_XOR  & TE5 & Not(TE200) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_XOR_TRT_VTE200'           ).setLogic(ZDC_XOR  & NIMTRT & Not(TE200) & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_ZDC_XOR_VTE200'               ).setLogic(ZDC_XOR  & Not(TE200) & physcond).setTriggerType(TT.minb)	

        # VDM
        LVL1MenuItem('L1_ZDC_A_C_BGRP7'     ).setLogic( ZDC_A_C & bgrp7cond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_BGRP7'       ).setLogic( (LUCID_A | LUCID_C) & bgrp7cond).setTriggerType(TT.minb)
        
        # LUCID
        #LVL1MenuItem('L1_LUCID_COMM'      ).setLogic( LUCID_COMM        & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_A'           ).setLogic( LUCID_A             & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_C'           ).setLogic( LUCID_C             & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_A_C',        ).setLogic( LUCID_A & LUCID_C   & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID'             ).setLogic((LUCID_A | LUCID_C)  & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_COMM'        ).setLogic( LUCID_COMM          & physcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_COMM_EMPTY'  ).setLogic( LUCID_COMM          & cosmiccond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_EMPTY'       ).setLogic((LUCID_A | LUCID_C)  & cosmiccond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_A_C_EMPTY'   ).setLogic( LUCID_A & LUCID_C   & cosmiccond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_UNPAIRED_ISO').setLogic((LUCID_A | LUCID_C)  & unpaired_isocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_LUCID_COMM_UNPAIRED_ISO' ).setLogic( LUCID_COMM        & unpaired_isocond ).setTriggerType(TT.minb)
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

        LVL1MenuItem('L1_BCM_AC_UNPAIRED_ISO'     ).setLogic( BCM_AtoC & unpaired_isocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_BCM_CA_UNPAIRED_ISO'     ).setLogic( BCM_CtoA & unpaired_isocond).setTriggerType(TT.minb)
        
        LVL1MenuItem('L1_BCM_AC_UNPAIRED_NONISO'     ).setLogic( BCM_AtoC & unpaired_nonisocond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_BCM_CA_UNPAIRED_NONISO'     ).setLogic( BCM_CtoA & unpaired_nonisocond).setTriggerType(TT.minb)
        
        LVL1MenuItem('L1_BCM_AC_ABORTGAPNOTCALIB'     ).setLogic( BCM_AtoC & abortgap).setTriggerType(TT.minb)
        LVL1MenuItem('L1_BCM_CA_ABORTGAPNOTCALIB'     ).setLogic( BCM_CtoA & abortgap).setTriggerType(TT.minb)
        LVL1MenuItem('L1_BCM_Wide_ABORTGAPNOTCALIB'   ).setLogic( BCM_Wide & abortgap).setTriggerType(TT.minb)        

        LVL1MenuItem('L1_BCM_AC_CALIB'     ).setLogic( BCM_AtoC & calibcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_BCM_CA_CALIB'     ).setLogic( BCM_CtoA & calibcond).setTriggerType(TT.minb)
        LVL1MenuItem('L1_BCM_Wide_CALIB'   ).setLogic( BCM_Wide & calibcond).setTriggerType(TT.minb)        


        # RANDOM
        LVL1MenuItem('L1_RD0_FILLED'         ).setLogic( RNDM0 & physcond           ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD0_EMPTY'          ).setLogic( RNDM0 & cosmiccond         ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD0_UNPAIRED_ISO'   ).setLogic( RNDM0 & unpaired_isocond   ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD0_FIRSTEMPTY'     ).setLogic( RNDM0 & firstempty         ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD0_BGRP9'          ).setLogic( RNDM0 & bgrp9cond          ).setTriggerType(TT.rand) 
        LVL1MenuItem('L1_RD0_BGRP11'         ).setLogic( RNDM0 & bgrp11cond         ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD0_ABORTGAPNOTCALIB').setLogic( RNDM0 & abortgap          ).setTriggerType(TT.rand)

        LVL1MenuItem('L1_RD1_FILLED'         ).setLogic( RNDM1 & physcond           ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD1_EMPTY'          ).setLogic( RNDM1 & cosmiccond         ).setTriggerType(TT.zerobs)

        LVL1MenuItem('L1_RD2_FILLED'         ).setLogic( RNDM2 & physcond           ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD2_EMPTY'          ).setLogic( RNDM2 & cosmiccond         ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD2_BGRP11'         ).setLogic( RNDM2 & bgrp11cond         ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD2_BGRP12'         ).setLogic( RNDM2 & bgrp12cond         ).setTriggerType(TT.rand)

        LVL1MenuItem('L1_RD3_FILLED'         ).setLogic( RNDM3 & physcond           ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_RD3_EMPTY'          ).setLogic( RNDM3 & cosmiccond         ).setTriggerType(TT.rand)

        # BGRP
        LVL1MenuItem('L1_BGRP0').setLogic( BGRP0 ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_BGRP1').setLogic( BGRP0 & BGRP1 ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_BGRP7').setLogic( BGRP0 & BGRP7 ).setTriggerType(TT.rand)
        LVL1MenuItem('L1_BGRP9').setLogic( BGRP0 & BGRP9 ).setTriggerType(TT.rand)

        LVL1MenuItem('L1_BPTX0_BGRP0', ctpid=0xf1).setLogic(BPTX0 & BGRP0).setTriggerType(TT.rand)
        LVL1MenuItem('L1_BPTX1_BGRP0', ctpid=0xf2).setLogic(BPTX1 & BGRP0).setTriggerType(TT.rand)   
        
        # lumi measurements
        LVL1MenuItem('L1_MLZ_A').setLogic( (MBTS_A|ZDC_A|LUCID_A) & physcond).setTriggerType(TT.minb) 
        LVL1MenuItem('L1_MLZ_C').setLogic( (MBTS_C|ZDC_C|LUCID_C) & physcond).setTriggerType(TT.minb) 
        LVL1MenuItem('L1_MBLZ' ).setLogic( ( (MBTS_A|ZDC_A|LUCID_A) & (MBTS_C|ZDC_C|LUCID_C) | BCM_Wide) & physcond ).setTriggerType(TT.minb)  

        LVL1MenuItem('L1_CALREQ0', ctpid=0x1fd).setLogic( CAL0 & calibcond).setTriggerType(TT.calreq0) 
        LVL1MenuItem('L1_CALREQ1', ctpid=0x1fe).setLogic( CAL1 & calibcond).setTriggerType(TT.calreq1)
        LVL1MenuItem('L1_CALREQ2', ctpid=0x1ff).setLogic( CAL2 & calibcond).setTriggerType(TT.calreq2)   

        # TRT
        LVL1MenuItem('L1_TRT'       , ctpid=0x4e).setLogic(NIMTRT & BGRP0).setTriggerType(TT.nim)
        LVL1MenuItem('L1_TRT_FILLED').setLogic(NIMTRT & physcond).setTriggerType(TT.nim)
        LVL1MenuItem('L1_TRT_EMPTY' ).setLogic(NIMTRT & cosmiccond).setTriggerType(TT.nim)

        # TGC
        LVL1MenuItem('L1_TGC_BURST').setLogic(NIMTGC & physcond).setTriggerType(TT.nim)

        # LHCF
        LVL1MenuItem('L1_LHCF').setLogic( NIMLHCF & physcond).setTriggerType(TT.nim)
        LVL1MenuItem('L1_LHCF_UNPAIRED_ISO').setLogic( NIMLHCF & unpaired_isocond).setTriggerType(TT.nim)
        LVL1MenuItem('L1_LHCF_EMPTY').setLogic( NIMLHCF & cosmiccond).setTriggerType(TT.nim)
        
        # ALFA 

        # LUT 22 (12 Outputs)
        # ALFA LUT output #1-8 are the single thresholds
        ALFA_ANY_A    = ALFA_B7L1U | ALFA_B7L1L | ALFA_A7L1U | ALFA_A7L1L  #  9
        ALFA_ANY_C    = ALFA_A7R1U | ALFA_A7R1L | ALFA_B7R1U | ALFA_B7R1L  # 10
        ALFA_ANY_U    = ALFA_B7L1U | ALFA_A7L1U | ALFA_A7R1U | ALFA_B7R1U  # 11
        ALFA_ANY_L    = ALFA_A7R1L | ALFA_B7R1L | ALFA_B7L1L | ALFA_A7L1L  # 12

        # LUT 23 (12 Outputs)
        ALFA_LU       = ALFA2_A7L1U | ALFA2_B7L1U #  0
        ALFA_RL       = ALFA2_A7R1L | ALFA2_B7R1L #  1
        ALFA_LL       = ALFA2_B7L1L | ALFA2_A7L1L #  2
        ALFA_RU       = ALFA2_A7R1U | ALFA2_B7R1U #  3

        ALFA_A_UL_AA  = ALFA2_A7L1L | ALFA2_A7L1U #  4
        ALFA_A_UL_AB  = ALFA2_A7L1U | ALFA2_B7L1L #  5
        ALFA_A_UL_BA  = ALFA2_A7L1L | ALFA2_B7L1U #  6
        ALFA_A_UL_BB  = ALFA2_B7L1L | ALFA2_B7L1U #  7

        ALFA_C_UL_AA  = ALFA2_A7R1L | ALFA2_A7R1U #  8
        ALFA_C_UL_AB  = ALFA2_A7R1U | ALFA2_B7R1L #  9
        ALFA_C_UL_BA  = ALFA2_A7R1L | ALFA2_B7R1U # 10
        ALFA_C_UL_BB  = ALFA2_B7R1L | ALFA2_B7R1U # 11

        # LUT 24 (9 Outputs)
        ALFA_ANY          = ALFA3_B7L1U | ALFA3_B7L1L | ALFA3_A7L1U | ALFA3_A7L1L | ALFA3_A7R1U | ALFA3_A7R1L | ALFA3_B7R1U | ALFA3_B7R1L # 0
        NOT_ALFA_ANY_A    = Not(ALFA3_B7L1U | ALFA3_B7L1L | ALFA3_A7L1U | ALFA3_A7L1L)  #  1-4
        NOT_ALFA_ANY_C    = Not(ALFA3_A7R1U | ALFA3_A7R1L | ALFA3_B7R1U | ALFA3_B7R1L)  #  5-8

        # LUT 25 (4 outputs)
        ALFA_AE1          = ALFA4_B7L1U | ALFA4_A7L1U | ALFA4_B7L1L | ALFA4_A7L1L
        ALFA_AE2          = ALFA4_B7L1U | ALFA4_A7L1U | ALFA4_A7R1L | ALFA4_B7R1L
        ALFA_AE3          = ALFA4_A7R1U | ALFA4_B7R1U | ALFA4_B7L1L | ALFA4_A7L1L
        ALFA_AE4          = ALFA4_A7R1U | ALFA4_B7R1U | ALFA4_A7R1L | ALFA4_B7R1L


        # further simplification (in CAM)
        ALFA_A            = ALFA_A_UL_AA & ALFA_A_UL_AB & ALFA_A_UL_BA & ALFA_A_UL_BB
        ALFA_C            = ALFA_C_UL_AA & ALFA_C_UL_AB & ALFA_C_UL_BA & ALFA_C_UL_BB
        ALFA_ELASTIC      = ALFA_ANY_A & ALFA_ANY_C & ALFA_ANY_U & ALFA_ANY_L
        ALFA_EINE         = ALFA_ANY_A & ALFA_ANY_C
        ALFA_ANTI_ELASTIC = ALFA_AE1 & ALFA_AE2 & ALFA_AE3 & ALFA_AE4


        MBTS_INNER = (MBTS_A0 | MBTS_A1 |  MBTS_A2 | MBTS_A3 | MBTS_A4 | MBTS_A5 | MBTS_A6 | MBTS_A7 | MBTS_C0 | MBTS_C1 | MBTS_C2 | MBTS_C3 | MBTS_C4 | MBTS_C5 | MBTS_C6 | MBTS_C7)         



        ## Single items
        LVL1MenuItem('L1_ALFA_B7L1U').setLogic(ALFA_B7L1U & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_B7L1L').setLogic(ALFA_B7L1L & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_A7L1U').setLogic(ALFA_A7L1U & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_A7L1L').setLogic(ALFA_A7L1L & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_A7R1U').setLogic(ALFA_A7R1U & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_A7R1L').setLogic(ALFA_A7R1L & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_B7R1U').setLogic(ALFA_B7R1U & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_B7R1L').setLogic(ALFA_B7R1L & BGRP0).setTriggerType(TT.alfa)


        LVL1MenuItem('L1_ALFA_ELAST1').setLogic( ALFA_B7L1U & ALFA_A7L1U & ALFA_A7R1L & ALFA_B7R1L &
                                                 Not(ALFA3_B7L1L | ALFA3_A7L1L | ALFA3_A7R1U | ALFA3_B7R1U)
                                                 & physcond).setTriggerType(TT.alfa)
        
        LVL1MenuItem('L1_ALFA_ELAST2').setLogic( ALFA_B7L1L & ALFA_A7L1L & ALFA_A7R1U & ALFA_B7R1U &
                                                 Not(ALFA3_B7L1U | ALFA3_A7L1U | ALFA3_A7R1L | ALFA3_B7R1L)
                                                 & physcond).setTriggerType(TT.alfa)
        
        LVL1MenuItem('L1_ALFA_ELAST11').setLogic( ALFA_B7L1U & ALFA_A7L1U & ALFA_A7R1L & ALFA_B7R1L    & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ELAST12').setLogic( ALFA_B7L1L & ALFA_A7L1L & ALFA_A7R1U & ALFA_B7R1U    & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ELAST13').setLogic( ALFA_LU & (ALFA_A7R1L & ALFA_B7R1L)                  & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ELAST14').setLogic( (ALFA_B7L1U & ALFA_A7L1U) & ALFA_RL & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ELAST15').setLogic( ALFA_LU & ALFA_RL & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ELAST15_Calib').setLogic( ALFA_LU & ALFA_RL &  alfacalib).setTriggerType(TT.alfa) ## CHECK
        LVL1MenuItem('L1_ALFA_ELAST16').setLogic( ALFA_LL & (ALFA_A7R1U & ALFA_B7R1U) & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ELAST17').setLogic((ALFA_B7L1L & ALFA_A7L1L) & ALFA_RU & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ELAST18').setLogic( ALFA_LL & ALFA_RU & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ELAST18_Calib').setLogic( ALFA_LL & ALFA_RU & alfacalib).setTriggerType(TT.alfa)


        LVL1MenuItem('L1_ALFA_SDIFF1').setLogic( ALFA_B7L1U & ALFA_A7L1U &
                                                 Not(ALFA3_B7L1L | ALFA3_A7L1L | ALFA3_A7R1U | ALFA3_A7R1L | ALFA3_B7R1U | ALFA3_B7R1L)
                                                 & physcond).setTriggerType(TT.alfa)
        
        LVL1MenuItem('L1_ALFA_SDIFF2').setLogic( ALFA_A7R1L & ALFA_B7R1L &
                                                 Not(ALFA3_B7L1U | ALFA3_B7L1L | ALFA3_A7L1U | ALFA3_A7L1L | ALFA3_A7R1U | ALFA3_B7R1U)
                                                 & physcond).setTriggerType(TT.alfa)
         
        LVL1MenuItem('L1_ALFA_SDIFF3').setLogic( ALFA_B7L1L & ALFA_A7L1L &
                                                 Not(ALFA3_B7L1U | ALFA3_A7L1U | ALFA3_A7R1U | ALFA3_A7R1L | ALFA3_B7R1U | ALFA3_B7R1L)
                                                 & physcond).setTriggerType(TT.alfa)
         
        LVL1MenuItem('L1_ALFA_SDIFF4').setLogic( ALFA_A7R1U & ALFA_B7R1U &
                                                 Not(ALFA3_B7L1U | ALFA3_B7L1L | ALFA3_A7L1U | ALFA3_A7L1L | ALFA3_A7R1L | ALFA3_B7R1L)
                                                 & physcond).setTriggerType(TT.alfa)

        LVL1MenuItem('L1_ALFA_SDIFF5').setLogic( ALFA_B7L1U & ALFA_A7L1U &  physcond).setTriggerType(TT.alfa) 
        LVL1MenuItem('L1_ALFA_SDIFF6').setLogic( ALFA_A7R1L & ALFA_B7R1L &  physcond).setTriggerType(TT.alfa) 
        LVL1MenuItem('L1_ALFA_SDIFF7').setLogic( ALFA_B7L1L & ALFA_A7L1L &  physcond).setTriggerType(TT.alfa) 
        LVL1MenuItem('L1_ALFA_SDIFF8').setLogic( ALFA_A7R1U & ALFA_B7R1U &  physcond).setTriggerType(TT.alfa) 
                                                           
        LVL1MenuItem('L1_MBTS_1_A_ALFA_C').setLogic( MBTS_A & ALFA_C  & physcond).setTriggerType(TT.alfa) 
        LVL1MenuItem('L1_MBTS_1_C_ALFA_A').setLogic( MBTS_C & ALFA_A & physcond).setTriggerType(TT.alfa)        
        LVL1MenuItem('L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO').setLogic( MBTS_A & ALFA_C & unpaired_isocond).setTriggerType(TT.alfa) 
        LVL1MenuItem('L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO').setLogic( MBTS_C & ALFA_A & unpaired_isocond).setTriggerType(TT.alfa)        
        LVL1MenuItem('L1_MBTS_1_ALFA_ANY').setLogic( MBTS_1 & ALFA_ANY  & physcond).setTriggerType(TT.alfa) ##should be called L1_MBTS_1_ALFA_ANY                          

        ## check definition of MBTS_2
        LVL1MenuItem('L1_MBTS_2_A_ALFA_C').setLogic( MBTS_A.x(2) & ALFA_C & physcond).setTriggerType(TT.alfa) 
        LVL1MenuItem('L1_MBTS_2_C_ALFA_A').setLogic( MBTS_C.x(2) & ALFA_A & physcond).setTriggerType(TT.alfa)        
        LVL1MenuItem('L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO').setLogic( MBTS_A.x(2) & ALFA_C & unpaired_isocond).setTriggerType(TT.alfa) 
        LVL1MenuItem('L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO').setLogic( MBTS_C.x(2) & ALFA_A & unpaired_isocond).setTriggerType(TT.alfa)        
        LVL1MenuItem('L1_MBTS_2_ALFA').setLogic( MBTS_2 & ALFA_ANY & physcond).setTriggerType(TT.alfa)              
        
        LVL1MenuItem('L1_LUCID_A_ALFA_C').setLogic( LUCID_A & ALFA_C & physcond).setTriggerType(TT.alfa) 
        LVL1MenuItem('L1_LUCID_C_ALFA_A').setLogic( LUCID_C & ALFA_A & physcond).setTriggerType(TT.alfa)     
        LVL1MenuItem('L1_LUCID_A_ALFA_C_UNPAIRED_ISO').setLogic( LUCID_A & ALFA_C & unpaired_isocond).setTriggerType(TT.alfa) 
        LVL1MenuItem('L1_LUCID_C_ALFA_A_UNPAIRED_ISO').setLogic( LUCID_C & ALFA_A & unpaired_isocond).setTriggerType(TT.alfa) 
        LVL1MenuItem('L1_LUCID_ALFA').setLogic( (LUCID_A | LUCID_C) & ALFA_ANY & physcond).setTriggerType(TT.alfa) 
        
        #LVL1MenuItem('L1_ZDC_A_ALFA_C').setLogic( ZDC_A  & ALFA_C & physcond).setTriggerType(TT.alfa) 
        #LVL1MenuItem('L1_ZDC_C_ALFA_A').setLogic( ZDC_C  & ALFA_A & physcond).setTriggerType(TT.alfa)     
        #LVL1MenuItem('L1_ZDC_ALFA').setLogic( (ZDC_A | ZDC_C) & ALFA_ANY & physcond).setTriggerType(TT.alfa) 
        
        LVL1MenuItem('L1_EM3_ALFA_ANY'             ).setLogic( EM3 & ALFA_ANY & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_EM3_ALFA_ANY_UNPAIRED_ISO').setLogic( EM3 & ALFA_ANY & unpaired_isocond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_EM3_ALFA_ANY_VETO_MBTS' ).setLogic( EM3 & ALFA_ANY  & Not(MBTS_INNER) & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_EM3_ALFA_EINE'          ).setLogic( EM3 & ALFA_EINE & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_EM3_ALFA_EINE_VETO_MBTS').setLogic( EM3 & ALFA_EINE & Not(MBTS_INNER) & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_EM3_ALFA_MBTS_A'             ).setLogic( EM3 & MBTS_A & ALFA_ANY_A & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_EM3_ALFA_MBTS_C'             ).setLogic( EM3 & MBTS_C & ALFA_ANY_C & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_EM3_ALFA_MBTS_A_UNPAIRED_ISO').setLogic( EM3 & MBTS_A & ALFA_ANY_A & unpaired_isocond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_EM3_ALFA_MBTS_C_UNPAIRED_ISO').setLogic( EM3 & MBTS_A & ALFA_ANY_C & unpaired_isocond).setTriggerType(TT.alfa)
        
        LVL1MenuItem('L1_J12_ALFA_ANY'             ).setLogic( J12 & ALFA_ANY & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_J12_ALFA_ANY_UNPAIRED_ISO').setLogic( J12 & ALFA_ANY & unpaired_isocond).setTriggerType(TT.alfa)

        LVL1MenuItem('L1_TE5_ALFA_ANY'             ).setLogic( TE5 & ALFA_ANY  & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_TE5_ALFA_ANY_UNPAIRED_ISO').setLogic( TE5 & ALFA_ANY  & unpaired_isocond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_TE5_ALFA_EINE'            ).setLogic( TE5 & ALFA_EINE & physcond).setTriggerType(TT.alfa)
        
        LVL1MenuItem('L1_TRT_ALFA_ANY'             ).setLogic( NIMTRT & ALFA_ANY & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_TRT_ALFA_ANY_UNPAIRED_ISO').setLogic( NIMTRT & ALFA_ANY & unpaired_isocond).setTriggerType(TT.alfa)
                   
        LVL1MenuItem('L1_TRT_ALFA_ANY_VETO_MBTS'           ).setLogic( NIMTRT & Not(MBTS_INNER) & ALFA_ANY & physcond).setTriggerType(TT.alfa)

        LVL1MenuItem('L1_TRT_ALFA_EINE'                    ).setLogic( NIMTRT & ALFA_EINE & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_TRT_ALFA_EINE_VETO_MBTS'          ).setLogic( NIMTRT & Not(MBTS_INNER) & ALFA_EINE & physcond).setTriggerType(TT.alfa)
        
        LVL1MenuItem('L1_ALFA_ELASTIC_VETO_MBTS'                  ).setLogic( Not(MBTS_INNER) & ALFA_ELASTIC & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ELASTIC_UNPAIRED_ISO'               ).setLogic( ALFA_ELASTIC & unpaired_isocond).setTriggerType(TT.alfa)

        LVL1MenuItem('L1_ALFA_ANTI_ELASTIC_VETO_MBTS'             ).setLogic( Not(MBTS_INNER) & ALFA_ANTI_ELASTIC & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ANTI_ELASTIC_UNPAIRED_ISO').setLogic( ALFA_ANTI_ELASTIC & unpaired_isocond).setTriggerType(TT.alfa)

        LVL1MenuItem('L1_ALFA_ANY_VETO_MBTS'                      ).setLogic( Not(MBTS_INNER) & ALFA_ANY & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ANY_VETO_MBTS_UNPAIRED_ISO'         ).setLogic( Not(MBTS_INNER) & ALFA_ANY & unpaired_isocond).setTriggerType(TT.alfa)
                
        LVL1MenuItem('L1_LHCF_ALFA_ANY_A'             ).setLogic( NIMLHCF & ALFA_ANY_A & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_LHCF_ALFA_ANY_C'             ).setLogic( NIMLHCF & ALFA_ANY_C & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_LHCF_ALFA_ANY_A_UNPAIRED_ISO').setLogic( NIMLHCF & ALFA_ANY_A & unpaired_isocond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_LHCF_ALFA_ANY_C_UNPAIRED_ISO').setLogic( NIMLHCF & ALFA_ANY_C & unpaired_isocond).setTriggerType(TT.alfa)
        
        LVL1MenuItem('L1_ALFA_BGT' ).setLogic(RNDM3 & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_BGT_UNPAIRED_ISO' ).setLogic(RNDM3 & unpaired_isocond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_BGT_BGRP10').setLogic(RNDM3 & alfacalib).setTriggerType(TT.alfa)

        LVL1MenuItem('L1_ALFA_SHOWSYST5').setLogic( (ALFA_ANY_A & ALFA_ANY_C) & physcond).setTriggerType(TT.alfa)

        LVL1MenuItem('L1_ALFA_SYST9' ).setLogic( ALFA_B7L1U & ALFA_A7L1U & ALFA_A7R1U & ALFA_B7R1U & Not(ALFA3_B7L1L | ALFA3_A7L1L | ALFA3_A7R1L | ALFA3_B7R1L) & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_SYST10').setLogic( ALFA_B7L1L & ALFA_A7L1L & ALFA_A7R1L & ALFA_B7R1L & Not(ALFA3_B7L1U | ALFA3_A7L1U | ALFA3_A7R1U | ALFA3_B7R1U) & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_SYST11').setLogic( ALFA_B7L1U & ALFA_A7L1U & ALFA_A7R1U & ALFA_B7R1U & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_SYST12').setLogic( ALFA_B7L1L & ALFA_A7L1L & ALFA_A7R1L & ALFA_B7R1L & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_SYST17').setLogic( ALFA_LU & ALFA_RU & physcond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_SYST18').setLogic( ALFA_LL & ALFA_RL & physcond).setTriggerType(TT.alfa)
       
        LVL1MenuItem('L1_ALFA_ANY').setLogic(ALFA_ANY & physcond).setTriggerType(TT.alfa)
        
        LVL1MenuItem('L1_ALFA_ANY_EMPTY').setLogic(ALFA_ANY & cosmiccond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ANY_FIRSTEMPTY').setLogic(ALFA_ANY & firstempty).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ANY_UNPAIRED_ISO').setLogic(ALFA_ANY & unpaired_isocond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ANY_UNPAIRED_NONISO').setLogic(ALFA_ANY & unpaired_nonisocond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ANY_BGRP10').setLogic(ALFA_ANY & alfacalib).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ANY_ABORTGAPNOTCALIB').setLogic( ALFA_ANY & abortgap).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ANY_CALIB').setLogic( ALFA_ANY & calibcond).setTriggerType(TT.alfa)

        LVL1MenuItem('L1_ALFA_ANY_A_EMPTY').setLogic(ALFA_ANY_A & cosmiccond).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_ANY_C_EMPTY').setLogic(ALFA_ANY_C & cosmiccond).setTriggerType(TT.alfa)

        ## ALFA _OD items (LUT 26, 12 Outputs)
        LVL1MenuItem('L1_ALFA_B7L1U_OD').setLogic(ALFA_B7L1U_OD & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_B7L1L_OD').setLogic(ALFA_B7L1L_OD & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_A7L1U_OD').setLogic(ALFA_A7L1U_OD & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_A7L1L_OD').setLogic(ALFA_A7L1L_OD & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_A7R1U_OD').setLogic(ALFA_A7R1U_OD & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_A7R1L_OD').setLogic(ALFA_A7R1L_OD & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_B7R1U_OD').setLogic(ALFA_B7R1U_OD & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_B7R1L_OD').setLogic(ALFA_B7R1L_OD & BGRP0).setTriggerType(TT.alfa)

        LVL1MenuItem('L1_ALFA_B7L1_OD').setLogic( (ALFA_B7L1U_OD & ALFA_B7L1L_OD) & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_A7L1_OD').setLogic( (ALFA_A7L1U_OD & ALFA_A7L1L_OD) & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_B7R1_OD').setLogic( (ALFA_B7R1U_OD & ALFA_B7R1L_OD) & BGRP0).setTriggerType(TT.alfa)
        LVL1MenuItem('L1_ALFA_A7R1_OD').setLogic( (ALFA_A7R1U_OD & ALFA_A7R1L_OD) & BGRP0).setTriggerType(TT.alfa)

        #if (('Physics_HI_v3' not in TriggerFlags.triggerMenuSetup()) and ('MC_HI_v3' not in TriggerFlags.triggerMenuSetup())):
            # Topo items (keep them all together)
        try:
                
                # M8 menu
                #LVL1MenuItem('L1_MJJ-4').setLogic( TOPO_4INVM9999_AJ0s6_AJ0s6 & physcond)
                #LVL1MenuItem('L1_MJJ-3').setLogic( TOPO_3INVM9999_AJ0s6_AJ0s6 & physcond)
                #LVL1MenuItem('L1_MJJ-2').setLogic( TOPO_2INVM9999_AJ0s6_AJ0s6 & physcond)
                #LVL1MenuItem('L1_MJJ-1').setLogic( TOPO_1INVM9999_AJ0s6_AJ0s6 & physcond)
                
                #LVL1MenuItem('L1_HT200-J20s5.ETA49').setLogic( TOPO_HT200_AJ20s5ETA49 & physcond)
                #LVL1MenuItem('L1_HT190-J20s5.ETA49').setLogic( TOPO_HT190_AJ20s5ETA49 & physcond)
                #LVL1MenuItem('L1_HT1-J0.ETA49').setLogic( TOPO_HT1_AJ0allETA49 & physcond)
                
                # LVL1MenuItem('L1_JPSI-1M5-EM6').setLogic( TOPO_1INVM5_EM6s2_EMall & physcond)
                #LVL1MenuItem('L1_JPSI-1M5-EMs').setLogic( TOPO_1INVM5_EMs2_EMs & physcond)
                #LVL1MenuItem('L1_JPSI-1M5-EM6s').setLogic( TOPO_1INVM5_EM6s2_EMs & physcond)
                #LVL1MenuItem('L1_JPSI-1M5-EM12s').setLogic( TOPO_1INVM5_EM12s2_EMs & physcond)

                ### NOT WORKING due to acceptaed number of bits in TOPO1
                LVL1MenuItem("L1_2CMU4"     ).setLogic( TOPO_MULT_CMU4ab.x(2) & physcond)
                LVL1MenuItem("L1_2CMU6"     ).setLogic( TOPO_MULT_CMU6ab.x(2) & physcond)
                LVL1MenuItem("L1_CMU6_2CMU4").setLogic( TOPO_MULT_CMU4ab.x(2) & TOPO_MULT_CMU6ab & physcond)

                # B-jet
                LVL1MenuItem('L1_BTAG-MU4J15').setLogic( TOPO_0DR04_MU4ab_CJ15ab & physcond)
                LVL1MenuItem('L1_BTAG-MU4J30').setLogic( TOPO_0DR04_MU4ab_CJ30ab & physcond)
                LVL1MenuItem('L1_BTAG-MU6J20').setLogic( TOPO_0DR04_MU6ab_CJ20ab & physcond)
                LVL1MenuItem('L1_BTAG-MU6J25').setLogic( TOPO_0DR04_MU6ab_CJ25ab & physcond)
                LVL1MenuItem('L1_BTAG-MU4J20').setLogic( TOPO_0DR04_MU4ab_CJ20ab & physcond)
                LVL1MenuItem('L1_3J15_BTAG-MU4J15').setLogic( J15.x(3) & TOPO_0DR04_MU4ab_CJ15ab & physcond)
                LVL1MenuItem('L1_3J15_BTAG-MU4J30').setLogic( J15.x(3) & TOPO_0DR04_MU4ab_CJ30ab & physcond)
                LVL1MenuItem('L1_3J15_BTAG-MU6J25').setLogic( J15.x(3) & TOPO_0DR04_MU6ab_CJ25ab & physcond)
                LVL1MenuItem('L1_3J20_BTAG-MU4J20').setLogic( J20.x(3) & TOPO_0DR04_MU4ab_CJ20ab & physcond)
                #LVL1MenuItem('L1_3J20_BTAG-MU4J30').setLogic( J20.x(3) & TOPO_0DR04_MU4ab_CJ30ab & physcond)
                #LVL1MenuItem('L1_3J20_BTAG-MU6J25').setLogic( J20.x(3) & TOPO_0DR04_MU6ab_CJ25ab & physcond)

                # ZH
                if ('MC_pp_v6' in TriggerFlags.triggerMenuSetup()): 
                    LVL1MenuItem('L1_DPHI-AJ20s2XE50'  ).setLogic( TOPO_10MINDPHI_AJ20s2_XE50    & physcond)
                    
                LVL1MenuItem('L1_J40_DPHI-Js2XE50'  ).setLogic( J40 & TOPO_10MINDPHI_Js2_XE50    & physcond) 
                LVL1MenuItem('L1_J40_DPHI-J20s2XE50').setLogic( J40 & TOPO_10MINDPHI_J20s2_XE50  & physcond) 
                LVL1MenuItem('L1_J40_DPHI-J20XE50'  ).setLogic( J40 & TOPO_10MINDPHI_J20ab_XE50  & physcond) 
                LVL1MenuItem('L1_J40_DPHI-CJ20XE50' ).setLogic( J40 & TOPO_10MINDPHI_CJ20ab_XE50 & physcond) 
                
                # HT
                #LVL1MenuItem('L1_HT0-J0.ETA49'     ).setLogic( TOPO_HT0_AJ0allETA49    & physcond)
                LVL1MenuItem('L1_HT190-J15.ETA21'  ).setLogic( TOPO_HT190_AJ15allETA21 & physcond)
                LVL1MenuItem('L1_HT190-J15s5.ETA21').setLogic( TOPO_HT190_J15s5ETA21   & physcond)
                LVL1MenuItem('L1_HT150-J20.ETA31'  ).setLogic( TOPO_HT150_AJ20allETA31 & physcond)
                LVL1MenuItem('L1_HT150-J20s5.ETA31').setLogic( TOPO_HT150_J20s5ETA31   & physcond)
                
                # Jpsi T&P
                LVL1MenuItem("L1_JPSI-1M5"     ).setLogic( TOPO_1INVM5_EMs1_EMall   & physcond)
                LVL1MenuItem("L1_JPSI-1M5-EM7" ).setLogic( TOPO_1INVM5_EM7s1_EMall  & physcond)
                LVL1MenuItem("L1_JPSI-1M5-EM12").setLogic( TOPO_1INVM5_EM12s1_EMall & physcond)
                
                # W T&P
                if not '_v6' in TriggerFlags.triggerMenuSetup():
                    LVL1MenuItem("L1_W-05DPHI-JXE-0"   ).setLogic( TOPO_05MINDPHI_AJj20s6_XE0 & physcond)
                    LVL1MenuItem("L1_W-10DPHI-JXE-0"   ).setLogic( TOPO_10MINDPHI_AJj20s6_XE0 & physcond)
                    LVL1MenuItem("L1_W-15DPHI-JXE-0"   ).setLogic( TOPO_15MINDPHI_AJj20s6_XE0 & physcond)
                    LVL1MenuItem("L1_W-10DPHI-EMXE-0"  ).setLogic( TOPO_10MINDPHI_EM10s6_XE0  & physcond)
                    LVL1MenuItem("L1_W-15DPHI-EMXE-0"  ).setLogic( TOPO_15MINDPHI_EM10s6_XE0  & physcond)
                    LVL1MenuItem("L1_W-05DPHI-EMXE-1"  ).setLogic( TOPO_05MINDPHI_EM15s6_XE0  & physcond)
                    LVL1MenuItem("L1_W-05RO-XEHT-0"    ).setLogic( TOPO_05RATIO_XE0_HT0_AJj15allETA49   & physcond)
                    LVL1MenuItem("L1_W-08RO-XEHT-0"    ).setLogic( TOPO_08RATIO_XE0_HT0_AJj15allETA49   & physcond)
                    LVL1MenuItem("L1_W-90RO2-XEHT-0"   ).setLogic( TOPO_90RATIO2_XE0_HT0_AJj15allETA49  & physcond)
                    LVL1MenuItem("L1_W-250RO2-XEHT-0"  ).setLogic( TOPO_250RATIO2_XE0_HT0_AJj15allETA49 & physcond)
                    LVL1MenuItem("L1_W-HT20-JJ15.ETA49").setLogic( TOPO_HT20_AJj15allETA49 & physcond)
                    LVL1MenuItem("L1_W-NOMATCH"        ).setLogic( TOPO_NOT_02MATCH_EM10s1_AJj15allETA49 & physcond)
                    LVL1MenuItem("L1_W-NOMATCH_W-05RO-XEEMHT").setLogic( TOPO_NOT_02MATCH_EM10s1_AJj15allETA49 & TOPO_05RATIO_XE0_SUM0_EM10s1_HT0_AJj15allETA49 & physcond)
                    LVL1MenuItem("L1_EM10_W-MT25").setLogic( TOPO_25MT_EM10s6_XE0 & EM10 & physcond)
                    LVL1MenuItem("L1_EM10_W-MT30").setLogic( TOPO_30MT_EM10s6_XE0 & EM10 & physcond)
                    LVL1MenuItem("L1_EM15_W-MT35").setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & physcond)
                    LVL1MenuItem("L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0" ).setLogic( TOPO_25MT_EM10s6_XE0 & EM10 & TOPO_15MINDPHI_AJj20s6_XE0 & TOPO_15MINDPHI_EM10s6_XE0 & TOPO_90RATIO2_XE0_HT0_AJj15allETA49 & physcond)
                    LVL1MenuItem("L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS30"           ).setLogic( TOPO_25MT_EM10s6_XE0 & EM10 & TOPO_15MINDPHI_AJj20s6_XE0 & TOPO_15MINDPHI_EM10s6_XE0 & XS30 & physcond)
                    LVL1MenuItem("L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20"           ).setLogic( TOPO_25MT_EM10s6_XE0 & EM10 & TOPO_15MINDPHI_AJj20s6_XE0 & TOPO_15MINDPHI_EM10s6_XE0 & XS20 & physcond)
                    LVL1MenuItem("L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE"                ).setLogic( TOPO_25MT_EM10s6_XE0 & EM10 & TOPO_15MINDPHI_AJj20s6_XE0 & TOPO_15MINDPHI_EM10s6_XE0 & physcond)
                    LVL1MenuItem("L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0").setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & TOPO_05MINDPHI_AJj20s6_XE0 & TOPO_05MINDPHI_EM15s6_XE0 & TOPO_250RATIO2_XE0_HT0_AJj15allETA49 & physcond)
                    LVL1MenuItem("L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS60"           ).setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & TOPO_05MINDPHI_AJj20s6_XE0 & TOPO_05MINDPHI_EM15s6_XE0 & XS60 & physcond)
                    LVL1MenuItem("L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30"           ).setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & TOPO_05MINDPHI_AJj20s6_XE0 & TOPO_05MINDPHI_EM15s6_XE0 & XS30 & physcond)
                    LVL1MenuItem("L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE"                ).setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & TOPO_05MINDPHI_AJj20s6_XE0 & TOPO_05MINDPHI_EM15s6_XE0 & physcond)
                else:
                    LVL1MenuItem("L1_W-05DPHI-JXE-0"   ).setLogic( TOPO_05MINDPHI_AJj10s6_XE0 & physcond)
                    LVL1MenuItem("L1_W-10DPHI-JXE-0"   ).setLogic( TOPO_10MINDPHI_AJj10s6_XE0 & physcond)
                    LVL1MenuItem("L1_W-15DPHI-JXE-0"   ).setLogic( TOPO_15MINDPHI_AJj10s6_XE0 & physcond)
                    LVL1MenuItem("L1_W-05DPHI-EMXE-1"  ).setLogic( TOPO_05MINDPHI_EM12s6_XE0  & physcond)
                    LVL1MenuItem("L1_W-10DPHI-EMXE-0"  ).setLogic( TOPO_10MINDPHI_EM12s6_XE0  & physcond)

                    LVL1MenuItem("L1_W-05DPHI-EM15XE-1"  ).setLogic( TOPO_05MINDPHI_EM15s6_XE0  & physcond)
                    LVL1MenuItem("L1_W-05RO-XEHT-0"    ).setLogic( TOPO_05RATIO_XE0_HT0_AJj15allETA49   & physcond)
                    LVL1MenuItem("L1_W-08RO-XEHT-0"    ).setLogic( TOPO_08RATIO_XE0_HT0_AJj15allETA49   & physcond)
                    LVL1MenuItem("L1_W-90RO2-XEHT-0"   ).setLogic( TOPO_90RATIO2_XE0_HT0_AJj15allETA49  & physcond)
                    LVL1MenuItem("L1_W-250RO2-XEHT-0"  ).setLogic( TOPO_250RATIO2_XE0_HT0_AJj15allETA49 & physcond)
                    LVL1MenuItem("L1_W-HT20-JJ15.ETA49").setLogic( TOPO_HT20_AJj15allETA49 & physcond)
                    LVL1MenuItem("L1_W-NOMATCH"        ).setLogic( TOPO_NOT_02MATCH_EM10s1_AJj15allETA49 & physcond)
                    LVL1MenuItem("L1_W-NOMATCH_W-05RO-XEEMHT").setLogic( TOPO_NOT_02MATCH_EM10s1_AJj15allETA49 & TOPO_05RATIO_XE0_SUM0_EM10s1_HT0_AJj15allETA49 & physcond)
                    LVL1MenuItem("L1_EM12_W-MT25").setLogic( TOPO_25MT_EM12s6_XE0 & EM12 & physcond)
                    LVL1MenuItem("L1_EM12_W-MT30").setLogic( TOPO_30MT_EM12s6_XE0 & EM12 & physcond)
                    LVL1MenuItem("L1_EM15_W-MT35").setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & physcond)
                    
                    LVL1MenuItem("L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE" ).setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & TOPO_250RATIO2_XE0_HT0_AJj15allETA49 & TOPO_05MINDPHI_AJj10s6_XE0 & TOPO_05MINDPHI_EM15s6_XE0 & physcond)
                    LVL1MenuItem("L1_EM15_W-MT35_XS60_W-05DPHI-JXE-0_W-05DPHI-EM15XE"            ).setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & XS60 & TOPO_05MINDPHI_AJj10s6_XE0 & TOPO_05MINDPHI_EM15s6_XE0 & physcond)
                    LVL1MenuItem("L1_EM15_W-MT35_XS40_W-05DPHI-JXE-0_W-05DPHI-EM15XE"            ).setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & XS40 & TOPO_05MINDPHI_AJj10s6_XE0 & TOPO_05MINDPHI_EM15s6_XE0 & physcond)
                    LVL1MenuItem("L1_EM15_W-MT35_XS60").setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & XS60 & physcond)
                    
                    LVL1MenuItem("L1_EM15VH_W-MT35_XS60").setLogic( TOPO_35MT_EM15s6_XE0 & EM15VH & XS60 & physcond)
                    LVL1MenuItem("L1_EM20VH_W-MT35_XS60").setLogic( TOPO_35MT_EM15s6_XE0 & EM20VH & XS60 & physcond)
                    LVL1MenuItem("L1_EM22VHI_W-MT35_XS40").setLogic( TOPO_35MT_EM15s6_XE0 & EM22VHI & XS40 & physcond)


                    if ('MC_pp_v6' in TriggerFlags.triggerMenuSetup()):
                        LVL1MenuItem("L1_EM12_W-MT35_XS40_W-05DPHI-JXE-0_W-05DPHI-EMXE"            ).setLogic( TOPO_35MT_EM12s6_XE0 & EM12 & XS40 & TOPO_05MINDPHI_AJj10s6_XE0 & TOPO_05MINDPHI_EM12s6_XE0 & physcond)
                        LVL1MenuItem("L1_EM12_W-MT35").setLogic( TOPO_35MT_EM12s6_XE0 & EM12 & physcond)                    
                        LVL1MenuItem("L1_EM12_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EMXE" ).setLogic( TOPO_35MT_EM12s6_XE0 & EM12 & TOPO_250RATIO2_XE0_HT0_AJj15allETA49 & TOPO_05MINDPHI_AJj10s6_XE0 & TOPO_05MINDPHI_EM12s6_XE0 & physcond)
                        LVL1MenuItem("L1_EM12_W-MT35_XS50").setLogic( TOPO_35MT_EM12s6_XE0 & EM12 & XS50 & physcond)
                        LVL1MenuItem("L1_EM10VH_W-MT35_XS50").setLogic( TOPO_35MT_EM12s6_XE0 & EM10VH & XS50 & physcond)
                        LVL1MenuItem("L1_W-15DPHI-EM15XE-0"  ).setLogic( TOPO_15MINDPHI_EM15s6_XE0  & physcond)
                        LVL1MenuItem("L1_EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE" ).setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & TOPO_250RATIO2_XE0_HT0_AJj15allETA49 & TOPO_15MINDPHI_AJj10s6_XE0 & TOPO_15MINDPHI_EM15s6_XE0 & physcond)
                        LVL1MenuItem("L1_EM15_W-MT35_XS60_W-15DPHI-JXE-0_W-15DPHI-EM15XE"            ).setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & XS60 & TOPO_15MINDPHI_AJj10s6_XE0 & TOPO_15MINDPHI_EM15s6_XE0 & physcond)
                        LVL1MenuItem("L1_EM15_W-MT35_XS40_W-15DPHI-JXE-0_W-15DPHI-EM15XE"            ).setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & XS40 & TOPO_15MINDPHI_AJj10s6_XE0 & TOPO_15MINDPHI_EM15s6_XE0 & physcond)
                        LVL1MenuItem("L1_W-15DPHI-EMXE-0"  ).setLogic( TOPO_15MINDPHI_EM12s6_XE0  & physcond)
                        LVL1MenuItem("L1_EM12_W-MT35_XS30_W-15DPHI-JXE-0_W-15DPHI-EMXE"            ).setLogic( TOPO_35MT_EM12s6_XE0 & EM12 & XS30 & TOPO_15MINDPHI_AJj10s6_XE0 & TOPO_15MINDPHI_EM12s6_XE0 & physcond)
                        LVL1MenuItem("L1_EM12_W-MT35_W-90RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EMXE"  ).setLogic( TOPO_35MT_EM12s6_XE0 & EM12 & TOPO_90RATIO2_XE0_HT0_AJj15allETA49 & TOPO_15MINDPHI_AJj10s6_XE0 & TOPO_15MINDPHI_EM12s6_XE0 & physcond)
                    # Old
                        LVL1MenuItem("L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0" 	).setLogic( TOPO_25MT_EM12s6_XE0 & EM12 & TOPO_15MINDPHI_AJj10s6_XE0 & TOPO_15MINDPHI_EM12s6_XE0 & TOPO_90RATIO2_XE0_HT0_AJj15allETA49 & physcond)
                        LVL1MenuItem("L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS30"           ).setLogic( TOPO_25MT_EM12s6_XE0 & EM12 & TOPO_15MINDPHI_AJj10s6_XE0 & TOPO_15MINDPHI_EM12s6_XE0 & XS30 & physcond)
                        LVL1MenuItem("L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20"           ).setLogic( TOPO_25MT_EM12s6_XE0 & EM12 & TOPO_15MINDPHI_AJj10s6_XE0 & TOPO_15MINDPHI_EM12s6_XE0 & XS20 & physcond)
                        LVL1MenuItem("L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE"                ).setLogic( TOPO_25MT_EM12s6_XE0 & EM12 & TOPO_15MINDPHI_AJj10s6_XE0 & TOPO_15MINDPHI_EM12s6_XE0 & physcond)
                        
                        
                    LVL1MenuItem("L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30"         ).setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & TOPO_05MINDPHI_AJj10s6_XE0 & TOPO_05MINDPHI_EM15s6_XE0 & XS30 & physcond)
                    LVL1MenuItem("L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE"              ).setLogic( TOPO_35MT_EM15s6_XE0 & EM15 & TOPO_05MINDPHI_AJj10s6_XE0 & TOPO_05MINDPHI_EM15s6_XE0 & physcond)

                #LVL1MenuItem("L1_W-10DPHI-JXE-0").setLogic( TOPO_10MINDPHI_AJj15s2_XE0 & physcond)
                #LVL1MenuItem("L1_W-20DPHI-JXE-1").setLogic( TOPO_20MINDPHI_AJjs6_XE0 & physcond)
                #LVL1MenuItem("L1_W-20DPHI-JXE-0").setLogic( TOPO_20MINDPHI_AJj15s2_XE0 & physcond)
                #LVL1MenuItem("L1_W-10DPHI-EMXE-0").setLogic( TOPO_10MINDPHI_EM6s1_XE0 & physcond)
                #LVL1MenuItem("L1_W-20DPHI-EMXE-1").setLogic( TOPO_20MINDPHI_EM9s6_XE0 & physcond)
                #LVL1MenuItem("L1_W-20DPHI-EMXE-0").setLogic( TOPO_20MINDPHI_EM6s1_XE0 & physcond)
                #LVL1MenuItem("L1_W-05RO-XEHT-0").setLogic( TOPO_05RATIO_XE0_HT0_AJj15allETA49 & physcond)
                #LVL1MenuItem("L1_W-08RO-XEHT-1").setLogic( TOPO_08RATIO_XE0_HT0_AJj0allETA49 & physcond)
                #LVL1MenuItem("L1_W-40RO2-XEHT-0").setLogic( TOPO_40RATIO2_XE0_HT0_AJj15allETA49 & physcond)
                #LVL1MenuItem("L1_W-90RO2-XEHT-1").setLogic( TOPO_90RATIO2_XE0_HT0_AJj0allETA49 & physcond)
                #LVL1MenuItem("L1_W-HT20-JJ0.ETA49").setLogic( TOPO_HT20_AJj0allETA49 & physcond)
                #LVL1MenuItem("L1_W-NOMATCH").setLogic( TOPO_NOT_02MATCH_EM9s1_AJj15allETA49 & physcond)
                #LVL1MenuItem("L1_W-NOMATCH_W-05RO-XEEMHT").setLogic( TOPO_NOT_02MATCH_EM9s1_AJj15allETA49 & TOPO_05RATIO_XE0_SUM0_EM9s1_HT0_AJj15allETA49 & physcond)
                #LVL1MenuItem("L1_EM3_W-MT20").setLogic( TOPO_20MT_EM6s6_XE0 & EM3 & physcond)
                #LVL1MenuItem("L1_EM3_W-MT30").setLogic( TOPO_30MT_EM6s6_XE0 & EM3 & physcond)
                #LVL1MenuItem("L1_EM3_W-MT40").setLogic( TOPO_40MT_EM6s6_XE0 & EM3 & physcond)
                
                # Bphysics triggers
                if not '_v6' in TriggerFlags.triggerMenuSetup():
#SX
                    LVL1MenuItem("L1_BPH-DR-2MU4").setLogic( TOPO_2DR15_2MU4ab & physcond)            
                    LVL1MenuItem("L1_BPH-2M-2MU4").setLogic( TOPO_2INVM999_2MU4ab & physcond)        
                    LVL1MenuItem("L1_BPH-4M8-2MU4").setLogic( TOPO_4INVM8_2MU4ab & physcond)            
                    LVL1MenuItem("L1_BPH-DR-2MU4_BPH-2M-2MU4").setLogic( TOPO_2DR15_2MU4ab & TOPO_2INVM999_2MU4ab & physcond)            
                    LVL1MenuItem("L1_BPH-DR-2MU4_BPH-4M8-2MU4").setLogic( TOPO_2DR15_2MU4ab & TOPO_4INVM8_2MU4ab & physcond)            
                    LVL1MenuItem("L1_BPH-DR-2MU6").setLogic( TOPO_2DR15_2MU6ab & physcond)            
                    LVL1MenuItem("L1_BPH-2M-2MU6").setLogic( TOPO_2INVM999_2MU6ab & physcond)            
                    LVL1MenuItem("L1_BPH-4M8-2MU6").setLogic( TOPO_4INVM8_2MU6ab & physcond)            
                    LVL1MenuItem("L1_BPH-DR-2MU6_BPH-2M-2MU6").setLogic( TOPO_2DR15_2MU6ab & TOPO_2INVM999_2MU6ab & physcond)            
                    LVL1MenuItem("L1_BPH-DR-2MU6_BPH-4M8-2MU6").setLogic( TOPO_2DR15_2MU6ab & TOPO_4INVM8_2MU6ab & physcond)            
                
                ##Combined L1Top 
                LVL1MenuItem('L1_DR-MU10TAU12I_TAU12I-J25').setLogic( TOPO_0DR28_MU10ab_TAU12abi &  TOPO_1DISAMB_TAU12abi_J25ab & physcond)
                LVL1MenuItem('L1_MU10_TAU12I-J25').setLogic( MU10 &  TOPO_1DISAMB_TAU12abi_J25ab & physcond)
                LVL1MenuItem('L1_XE35_EM15-TAU12I').setLogic( XE35 &  TOPO_DISAMB_EM15abhi_TAU12abi & physcond)  
                LVL1MenuItem('L1_XE40_EM15-TAU12I').setLogic( XE40 &  TOPO_DISAMB_EM15abhi_TAU12abi & physcond)  
                LVL1MenuItem('L1_XE45_TAU20-J20').setLogic( XE45 & TOPO_1DISAMB_TAU20ab_J20ab & physcond)
                LVL1MenuItem('L1_J25_2J20_3J12_DR-TAU20ITAU12I').setLogic( J25 & J20.x(2) & J12.x(3) &  TOPO_0DR28_TAU20abi_TAU12abi & physcond)
                LVL1MenuItem('L1_J25_3J12_EM15-TAU12I').setLogic( J25 & J12.x(3) &  TOPO_DISAMB_EM15abhi_TAU12abi & physcond)  
                LVL1MenuItem('L1_J25_2J12_DR-MU10TAU12I').setLogic( J25 & J12.x(2) &  TOPO_0DR28_MU10ab_TAU12abi & physcond)
                LVL1MenuItem('L1_J25_2J20_3J12_BOX-TAU20ITAU12I').setLogic( J25 & J20.x(2) & J12.x(3) & TOPO_0DETA20_0DPHI20_TAU20abi_TAU12abi & physcond)
                
                LVL1MenuItem('L1_MU6_MJJ-200').setLogic( MU6 & TOPO_200INVM9999_AJ30s6_AJ20s6 & physcond)
                LVL1MenuItem('L1_MU6_MJJ-300').setLogic( MU6 & TOPO_300INVM9999_AJ30s6_AJ20s6 & physcond)
                LVL1MenuItem('L1_MU6_MJJ-400').setLogic( MU6 & TOPO_400INVM9999_AJ30s6_AJ20s6 & physcond)
                LVL1MenuItem('L1_MU6_MJJ-500').setLogic( MU6 & TOPO_500INVM9999_AJ30s6_AJ20s6 & physcond) 
                
                LVL1MenuItem('L1_J30_2J20_4J20.0ETA49_MJJ-400').setLogic( J30 & J20.x(2) & J200ETA49.x(4) & TOPO_400INVM9999_AJ30s6_AJ20s6 & physcond)
                LVL1MenuItem('L1_J30_2J20_4J20.0ETA49_MJJ-700').setLogic( J30 & J20.x(2) & J200ETA49.x(4) & TOPO_700INVM9999_AJ30s6_AJ20s6 & physcond) 
                LVL1MenuItem('L1_J30_2J20_4J20.0ETA49_MJJ-800').setLogic( J30 & J20.x(2) & J200ETA49.x(4) & TOPO_800INVM9999_AJ30s6_AJ20s6 & physcond)
                LVL1MenuItem('L1_J30_2J20_4J20.0ETA49_MJJ-900').setLogic( J30 & J20.x(2) & J200ETA49.x(4) & TOPO_900INVM9999_AJ30s6_AJ20s6 & physcond)
                
                LVL1MenuItem('L1_3J20_4J20.0ETA49_MJJ-400').setLogic( J20.x(3) & J200ETA49.x(4) & TOPO_400INVM9999_AJ30s6_AJ20s6 & physcond)
                LVL1MenuItem('L1_3J20_4J20.0ETA49_MJJ-700').setLogic( J20.x(3) & J200ETA49.x(4) & TOPO_700INVM9999_AJ30s6_AJ20s6 & physcond) 
                LVL1MenuItem('L1_3J20_4J20.0ETA49_MJJ-800').setLogic( J20.x(3) & J200ETA49.x(4) & TOPO_800INVM9999_AJ30s6_AJ20s6 & physcond)
                LVL1MenuItem('L1_3J20_4J20.0ETA49_MJJ-900').setLogic( J20.x(3) & J200ETA49.x(4) & TOPO_900INVM9999_AJ30s6_AJ20s6 & physcond)
                
                LVL1MenuItem('L1_XE35_MJJ-200').setLogic( XE35 & TOPO_200INVM9999_AJ30s6_AJ20s6 & physcond)
                
                
                # CTP items for L1Topo streamers
                #LVL1MenuItem('L1_DPhi_AJsAJs').setLogic( TOPO_DPhi_AJsAJs & physcond)
                #LVL1MenuItem('L1_DPhi_EMsTAUs').setLogic( TOPO_DPhi_EMsTAUs & physcond)
                #LVL1MenuItem('L1_DEta_AJabAJab').setLogic( TOPO_DEta_AJabAJab & physcond)
                #LVL1MenuItem('L1_DEta_EMabTAUab').setLogic( TOPO_DEta_EMabTAUab & physcond)
                if not '_v6' in TriggerFlags.triggerMenuSetup():
#SX
                    LVL1MenuItem('L1_BPH-2M-2MU4-B').setLogic( TOPO_2INVM999_CMU4ab_MU4ab & physcond)
                    LVL1MenuItem('L1_BPH-2M-2MU4-BO').setLogic( TOPO_2INVM999_2CMU4ab & physcond)
                    LVL1MenuItem('L1_BPH-2M-MU6MU4').setLogic( TOPO_2INVM999_MU6ab_MU4ab & physcond)
                    LVL1MenuItem('L1_BPH-2M-MU6MU4-B').setLogic( TOPO_2INVM999_ONEBARREL_MU6ab_MU4ab & physcond)
                    LVL1MenuItem('L1_BPH-2M-MU6MU4-BO').setLogic( TOPO_2INVM999_CMU6ab_CMU4ab & physcond)
                    LVL1MenuItem('L1_BPH-4M8-2MU4-B').setLogic( TOPO_4INVM8_CMU4ab_MU4ab & physcond)
                    LVL1MenuItem('L1_BPH-4M8-2MU4-BO').setLogic( TOPO_4INVM8_2CMU4ab & physcond)
                    LVL1MenuItem('L1_BPH-4M8-MU6MU4').setLogic( TOPO_4INVM8_MU6ab_MU4ab & physcond)
                    LVL1MenuItem('L1_BPH-4M8-MU6MU4-B').setLogic( TOPO_4INVM8_ONEBARREL_MU6ab_MU4ab & physcond)
                    LVL1MenuItem('L1_BPH-4M8-MU6MU4-BO').setLogic( TOPO_4INVM8_CMU6ab_CMU4ab & physcond)

                LVL1MenuItem('L1_DY-DR-2MU4').setLogic( TOPO_2DR99_2MU4ab & physcond)
                LVL1MenuItem('L1_DY-BOX-2MU4').setLogic( TOPO_5DETA99_5DPHI99_2MU4ab & physcond)
                LVL1MenuItem('L1_DY-BOX-MU6MU4').setLogic( TOPO_5DETA99_5DPHI99_MU6ab_MU4ab & physcond)
                LVL1MenuItem('L1_DY-BOX-2MU6').setLogic( TOPO_5DETA99_5DPHI99_2MU6ab & physcond)
                LVL1MenuItem('L1_LFV-MU').setLogic( TOPO_0DR10_MU10ab_MU6ab & physcond)
                LVL1MenuItem('L1_LFV-EM8I').setLogic( TOPO_0DETA04_0DPHI03_EM8abi_MU10ab & physcond)
                LVL1MenuItem('L1_LFV-EM15I').setLogic( TOPO_0DETA04_0DPHI03_EM15abi_MUab & physcond)
                LVL1MenuItem('L1_DPHI-Js2XE50').setLogic( TOPO_10MINDPHI_Js2_XE50 & physcond)
                LVL1MenuItem('L1_DPHI-J20s2XE50').setLogic( TOPO_10MINDPHI_J20s2_XE50 & physcond)
                LVL1MenuItem('L1_DPHI-J20XE50').setLogic( TOPO_10MINDPHI_J20ab_XE50 & physcond)
                LVL1MenuItem('L1_DPHI-CJ20XE50').setLogic( TOPO_10MINDPHI_CJ20ab_XE50 & physcond)
                
                LVL1MenuItem('L1_MJJ-900').setLogic( TOPO_900INVM9999_AJ30s6_AJ20s6 & physcond)
                LVL1MenuItem('L1_MJJ-800').setLogic( TOPO_800INVM9999_AJ30s6_AJ20s6 & physcond)
                LVL1MenuItem('L1_MJJ-700').setLogic( TOPO_700INVM9999_AJ30s6_AJ20s6 & physcond)
                LVL1MenuItem('L1_MJJ-400').setLogic( TOPO_400INVM9999_AJ30s6_AJ20s6 & physcond)
                #LVL1MenuItem('L1_MJJ-350').setLogic( TOPO_350INVM9999_AJ30s6_AJ20s6 & physcond)
                LVL1MenuItem('L1_MJJ-300').setLogic( TOPO_300INVM9999_AJ30s6_AJ20s6 & physcond)
                LVL1MenuItem('L1_MJJ-200').setLogic( TOPO_200INVM9999_AJ30s6_AJ20s6 & physcond)
                LVL1MenuItem('L1_MJJ-100').setLogic( TOPO_100INVM9999_AJ30s6_AJ20s6 & physcond)
                
                #LVL1MenuItem('L1_MJJ-350-0').setLogic( TOPO_350INVM9999_J30s6_J20s6 & physcond)
                #LVL1MenuItem('L1_MJJ-300-0').setLogic( TOPO_300INVM9999_J30s6_J20s6 & physcond)
                #LVL1MenuItem('L1_MJJ-250-0').setLogic( TOPO_250INVM9999_J30s6_J20s6 & physcond)
                #LVL1MenuItem('L1_MJJ-200-0').setLogic( TOPO_200INVM9999_J30s6_J20s6 & physcond)
                LVL1MenuItem('L1_HT150-JJ15.ETA49').setLogic( TOPO_HT150_AJj15allETA49 & physcond)
                LVL1MenuItem('L1_DETA-JJ').setLogic( TOPO_0DETA10_Js1_Js2 & physcond)
                LVL1MenuItem('L1_J4-MATCH').setLogic( TOPO_0MATCH_4AJ20ETA31_4AJj15ETA31 & physcond)
                LVL1MenuItem('L1_W-05RO-XEEMHT').setLogic( TOPO_05RATIO_XE0_SUM0_EM10s1_HT0_AJj15allETA49 & physcond)
                #LVL1MenuItem('L1_W-05RO-XEEMHT').setLogic( TOPO_05RATIO_XE0_SUM0_EM9s1_HT0_AJj15allETA49 & physcond)
                LVL1MenuItem('L1_LLP-RO').setLogic( TOPO_210RATIO_0MATCH_TAU30si2_EMall & physcond)
                LVL1MenuItem('L1_LLP-NOMATCH').setLogic( TOPO_NOT_0MATCH_TAU30si2_EMall & physcond)
                LVL1MenuItem('L1_DR-MU10TAU12I').setLogic( TOPO_0DR28_MU10ab_TAU12abi & physcond)
                LVL1MenuItem('L1_TAU12I-J25').setLogic( TOPO_1DISAMB_TAU12abi_J25ab & physcond)
                LVL1MenuItem('L1_EM15-TAU40').setLogic( TOPO_DISAMB_EM15abhi_TAU40ab & physcond)
                LVL1MenuItem('L1_TAU20-J20').setLogic( TOPO_1DISAMB_TAU20ab_J20ab & physcond)
                LVL1MenuItem('L1_EM15-TAU12I').setLogic( TOPO_DISAMB_EM15abhi_TAU12abi & physcond)
                LVL1MenuItem('L1_EM15TAU12I-J25').setLogic( TOPO_1DISAMB_EM15his2_TAU12abi_J25ab & physcond)
                if 'MC_pp_v6' in TriggerFlags.triggerMenuSetup():
                    LVL1MenuItem('L1_DR-EM15TAU12I').setLogic( TOPO_DISAMB_0DR28_EM15his2_TAU12abi & physcond)
                LVL1MenuItem('L1_DR-EM15TAU12I-J25').setLogic( TOPO_1DISAMB_J25ab_0DR28_EM15his2_TAU12abi & physcond)
                LVL1MenuItem('L1_TAU20ITAU12I-J25').setLogic( TOPO_1DISAMB_TAU20abi_TAU12abi_J25ab & physcond)
                LVL1MenuItem('L1_DR-TAU20ITAU12I').setLogic( TOPO_0DR28_TAU20abi_TAU12abi & physcond)
                LVL1MenuItem('L1_BOX-TAU20ITAU12I').setLogic( TOPO_0DETA20_0DPHI20_TAU20abi_TAU12abi & physcond)
                LVL1MenuItem('L1_DR-TAU20ITAU12I-J25').setLogic( TOPO_1DISAMB_J25ab_0DR28_TAU20abi_TAU12abi & physcond)
                LVL1MenuItem('L1_LAR-EM').setLogic( TOPO_LAR_EM50s1 & physcond).setTriggerType( TT.lardemo ) # LAr demo (ATR-11897)
                LVL1MenuItem('L1_LAR-J').setLogic( TOPO_LAR_J100s1 & physcond).setTriggerType( TT.lardemo )  # LAr demo (ATR-11897)
                if not '_v6' in TriggerFlags.triggerMenuSetup():
                    LVL1MenuItem('L1_BPH-DR-2MU4-B').setLogic( TOPO_2DR15_CMU4ab_MU4ab & physcond)
                    LVL1MenuItem('L1_BPH-DR-2MU4-BO').setLogic( TOPO_2DR15_2CMU4ab & physcond)
                    LVL1MenuItem('L1_BPH-DR-MU6MU4').setLogic( TOPO_2DR15_MU6ab_MU4ab & physcond)
                    LVL1MenuItem('L1_BPH-DR-MU6MU4-B').setLogic( TOPO_2DR15_ONEBARREL_MU6ab_MU4ab & physcond)
                    LVL1MenuItem('L1_BPH-DR-MU6MU4-BO').setLogic( TOPO_2DR15_CMU6ab_CMU4ab & physcond)
                    LVL1MenuItem("L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B").setLogic( TOPO_2DR15_CMU4ab_MU4ab & TOPO_2INVM999_CMU4ab_MU4ab & physcond)            
                    LVL1MenuItem("L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B").setLogic( TOPO_2DR15_CMU4ab_MU4ab & TOPO_4INVM8_CMU4ab_MU4ab & physcond)            
                    LVL1MenuItem("L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO").setLogic( TOPO_2DR15_2CMU4ab & TOPO_2INVM999_2CMU4ab & physcond)            
                    LVL1MenuItem("L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO").setLogic( TOPO_2DR15_2CMU4ab & TOPO_4INVM8_2CMU4ab & physcond)            
                
                LVL1MenuItem('L1_KF-XE35').setLogic( TOPO_KF_XE35_AJall & physcond)
                LVL1MenuItem('L1_KF-XE45').setLogic( TOPO_KF_XE45_AJall & physcond)
                LVL1MenuItem('L1_KF-XE55').setLogic( TOPO_KF_XE55_AJall & physcond)
                LVL1MenuItem('L1_KF-XE60').setLogic( TOPO_KF_XE60_AJall & physcond)
                LVL1MenuItem('L1_KF-XE65').setLogic( TOPO_KF_XE65_AJall & physcond)
                LVL1MenuItem('L1_KF-XE75').setLogic( TOPO_KF_XE75_AJall & physcond)
#SX ATR-12789
                
                if  '_v6' in TriggerFlags.triggerMenuSetup():
# nominal L1-topo items
                    LVL1MenuItem('L1_BPH-1M19-2MU4_BPH-0DR34-2MU4'    ).setLogic( TOPO_1INVM19_2MU4ab & TOPO_0DR34_2MU4ab & physcond)           # 50% for Bs,J/psi,Upsi
                    if  'Physics' not in TriggerFlags.triggerMenuSetup():
                        LVL1MenuItem('L1_BPH-2M8-2MU4_BPH-0DR15-2MU4'     ).setLogic( TOPO_2INVM8_2MU4ab & TOPO_0DR15_2MU4ab & physcond)            # 78% for Bs,J/psi
                        LVL1MenuItem('L1_BPH-7M15-2MU4_BPH-0DR24-2MU4'    ).setLogic( TOPO_7INVM15_2MU4ab & TOPO_0DR24_2MU4ab & physcond)           # 78% for Upsi
                    LVL1MenuItem('L1_BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4' ).setLogic( TOPO_2INVM8_MU6ab_MU4ab & TOPO_0DR15_MU6ab_MU4ab & physcond)  # 86% for Bs,J/psi
                    if  'Physics' not in TriggerFlags.triggerMenuSetup():
                        LVL1MenuItem('L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4').setLogic( TOPO_8INVM15_MU6ab_MU4ab & TOPO_0DR22_MU6ab_MU4ab & physcond) # 86% for Upsi
                    LVL1MenuItem('L1_BPH-2M9-2MU6_BPH-2DR15-2MU6'     ).setLogic( TOPO_2INVM9_2MU6ab & TOPO_2DR15_2MU6ab & physcond)            # 96% for Bs,J/psi
                    if  'Physics' not in TriggerFlags.triggerMenuSetup():
                        LVL1MenuItem('L1_BPH-8M15-2MU6_BPH-0DR22-2MU6'    ).setLogic( TOPO_8INVM15_2MU6ab & TOPO_0DR22_2MU6ab & physcond)           # 96% for Upsi

# Barrel and BarrelOnly items w/o topo selection
#                    LVL1MenuItem('L1_CMU6_2CMU4').setLogic( TOPO_MULT_CMU4ab.x(2) & TOPO_MULT_CMU6ab & physcond) # MU6_2MU4_BarrelOnly
                    LVL1MenuItem('L1_2MU4-B'    ).setLogic( MU4.x(2) & TOPO_MULT_CMU4ab & physcond)              # 2MU4_Barrel
                    LVL1MenuItem('L1_2MU6-B'    ).setLogic( MU6.x(2) & TOPO_MULT_CMU6ab & physcond)              # 2MU6_Barrel
                    LVL1MenuItem('L1_MU6_2MU4-B').setLogic( MU6 & MU4.x(2) & TOPO_MULT_CMU4ab & physcond)        # MU6_2MU4_Barrel,     already in the menu

# Barrel and BarrelOnly variants of L1Topo items
                    LVL1MenuItem('L1_BPH-1M19-2MU4-B_BPH-0DR34-2MU4'    ).setLogic( TOPO_1INVM19_CMU4ab_MU4ab & TOPO_0DR34_2MU4ab & physcond)               # 50% for Bs,J/psi,Upsi + OneBarrel
                    if  'Physics' not in TriggerFlags.triggerMenuSetup():
                        LVL1MenuItem('L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4'     ).setLogic( TOPO_2INVM8_CMU4ab_MU4ab & TOPO_0DR15_2MU4ab & physcond)                # 78% for Bs,J/psi + OneBarrel

                        LVL1MenuItem('L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-B'    ).setLogic( TOPO_7INVM15_2MU4ab & TOPO_0DR24_CMU4ab_MU4ab & physcond)               # 78% for Upsi + OneBarrel
                    LVL1MenuItem('L1_BPH-1M19-2MU4-BO_BPH-0DR34-2MU4'   ).setLogic( TOPO_1INVM19_2CMU4ab & TOPO_0DR34_2MU4ab & physcond)                    # 50% for Bs,J/psi,Upsi + BarrelOnly
                    if  'Physics' not in TriggerFlags.triggerMenuSetup():
                        LVL1MenuItem('L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4'    ).setLogic( TOPO_2INVM8_2CMU4ab & TOPO_0DR15_2MU4ab & physcond)                     # 78% for Bs,J/psi + BarrelOnly
                        LVL1MenuItem('L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-BO'   ).setLogic( TOPO_7INVM15_2MU4ab & TOPO_0DR24_2CMU4ab & physcond)                    # 78% for Upsi + BarrelOnly
                    LVL1MenuItem('L1_BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4' ).setLogic( TOPO_2INVM8_ONEBARREL_MU6ab_MU4ab & TOPO_0DR15_MU6ab_MU4ab & physcond)  # 86% for Bs,J/psi + OneBarrel

                    LVL1MenuItem('L1_BPH-2M8-2MU4'       ).setLogic( TOPO_2INVM8_2MU4ab & physcond)                 # backup 71% for Bs,J/psi
                    LVL1MenuItem('L1_BPH-8M15-MU6MU4'    ).setLogic( TOPO_8INVM15_MU6ab_MU4ab & physcond)           # backup 71% for Upsi
                    LVL1MenuItem('L1_BPH-8M15-2MU6'      ).setLogic( TOPO_8INVM15_2MU6ab & physcond)                # backup 95% for Upsi
#SX end ATR-12789

                if 'MC_pp_v6' in TriggerFlags.triggerMenuSetup():                    
                    LVL1MenuItem('L1_LATEMU10_XE50').setLogic( TOPO_LATEMU10s1 & XE50 & physcond) 
                    LVL1MenuItem('L1_LATEMU10_J50').setLogic(  TOPO_LATEMU10s1 & J50 & physcond)  
                    LVL1MenuItem('L1_SC111').setLogic(  TOPO_SC111_CJ15abETA26 & physcond)
                    LVL1MenuItem('L1_SC85').setLogic(  TOPO_SC85_CJ15abETA26 & physcond)
                                        
        except Exception, ex:
                print "Creation of L1Topo item failed, will abort! Exception is : " , ex
                raise RuntimeError("Creation of L1Topo item failed, will not continue")
        
        
        # =======================================================
        # Partition 1
        LVL1MenuItem.currentPartition = 2
        
        #        LVL1MenuItem('L1_CALREQ0_P1', ctpid=480).setLogic( NIM28 & calibcond1).setTriggerType( 0x0f & TT.calreq0 )
        #        LVL1MenuItem('L1_CALREQ1_P1', ctpid=481).setLogic( NIM29 & calibcond1).setTriggerType( 0x0f & TT.calreq1 )
        #        LVL1MenuItem('L1_CALREQ2_P1', ctpid=482).setLogic( NIM30 & calibcond1).setTriggerType( 0x0f & TT.calreq2 )

        LVL1MenuItem.currentPartition = 3

        LVL1MenuItem.currentPartition = 1





        LVL1MenuItem.l1configForRegistration = None
