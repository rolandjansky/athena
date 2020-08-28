#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Disable flake8 checking due to the use of 'exec':
# flake8: noqa
#

from collections import defaultdict as ddict
import re, sys
import traceback

from ..Base.L1MenuFlags import L1MenuFlags
from AthenaCommon.Logging import logging
log = logging.getLogger('Menu.L1.Config.ItemDef')

# The trigger types
from ..Base.Limits import Limits
from ..Base.Logic import Logic, Not
from ..Base.Items import MenuItem, meta_d
from ..Base.Thresholds import TopoThreshold
from ..Base.CTPCondition import ThrCondition, InternalTrigger  # noqa: F401
from .TriggerTypeDef import TT

from future.utils import with_metaclass



class ItemDef:
    """
    Defines any items that might be needed in Run2
    """

    otherTopoNames = ddict(list)


    @staticmethod
    def registerItems(tc, menuName):
        """Register L1 items for further use"""

        # dear Menu Experts, please note that it is only necessary to
        # check the menu name if a certain item needs to be defined
        # differently in different menus

        # define local flag for menu version
        isV8 = '_v8' in menuName or 'LS2_v1' in menuName or 'run3_v1' in menuName
        isHIV5 = 'HI_v5' in menuName
        isPhaseII = '_PhaseII' in menuName

        class d(with_metaclass(meta_d)): pass

        # ... and make them accessible by their name
        for thr in tc.getDefinedThresholds():
            setattr(d, thr.getVarName(), ThrCondition(thr))
            if isinstance(thr, TopoThreshold):
                ItemDef.otherTopoNames[thr.getVarName().split('_',1)[-1]].append( thr.getVarName() )

        # InternalTriggers
        for i in range( Limits.NumBunchgroups ):
            setattr (d, "BGRP%i"%i,  InternalTrigger('BGRP%i'%i))

        for i in range( Limits.NumRndmTriggers ):
            setattr (d, "RNDM%i"%i, InternalTrigger('RNDM%i'%i))

        # Setting up bunch group and BPTX conditions
        physcond            = d.BGRP0 & d.BGRP1
        calibcond           = d.BGRP0 & d.BGRP2
        cosmiccond          = d.BGRP0 & d.BGRP3
        unpaired_isocond    = d.BGRP0 & d.BGRP4 # unpaired isolated (satellite bunches)
        unpaired_nonisocond = d.BGRP0 & d.BGRP5 # unpaired non-isolated (parasitic bunches)
        firstempty          = d.BGRP0 & d.BGRP6
        unpairedRcond       = d.BGRP0 & d.BGRP7 # unpaired beams 1 or beam 2
        bgrp7cond           = d.BGRP0 & d.BGRP7 # No unpaired anymore
        bgrp9cond           = d.BGRP0 & d.BGRP9
        bgrp11cond          = d.BGRP0 & d.BGRP11
        bgrp12cond          = d.BGRP0 & d.BGRP12
        bgrp13cond          = d.BGRP0 & d.BGRP13 #UNPAIREDB1
        bgrp14cond          = d.BGRP0 & d.BGRP14 #UNPAIREDB2
        alfacalib           = d.BGRP0 & d.BGRP10
        abortgap            = d.BGRP0 & d.BGRP8
        physcond_or_unpaired_isocond    = d.BGRP0 & (d.BGRP1 | d.BGRP4)

        # partition 1
        #bgrpcond1           = d.BGRP0 & d.BGRP11
        #calibcond1          = d.BGRP0 & d.BGRP12

        # partition 2
        #bgrpcond2           = d.BGRP0 & d.BGRP14


        MenuItem.currentPartition = 1


        MenuItem('L1_EM3'       ).setLogic( d.EM3        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM7'       ).setLogic( d.EM7        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM12'      ).setLogic( d.EM12       & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15'      ).setLogic( d.EM15       & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15_BGRP7').setLogic( d.EM15       & bgrp7cond).setTriggerType( TT.calo )
        MenuItem('L1_EM15I'     ).setLogic( d.EM15 & d.EM8I & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI'    ).setLogic( d.EM15HI     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM8VH'     ).setLogic( d.EM8VH      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM10VH'    ).setLogic( d.EM10VH     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VH'    ).setLogic( d.EM15VH     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM18VH'    ).setLogic( d.EM18VH     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM20VH'    ).setLogic( d.EM20VH     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM18VHI'   ).setLogic( d.EM18VHI    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM20VHI'   ).setLogic( d.EM20VHI    & physcond).setTriggerType( TT.calo )  # isolation not yet defined
        MenuItem('L1_EM22VHI'   ).setLogic( d.EM22VHI    & physcond).setTriggerType( TT.calo )  # isolation not yet defined
        MenuItem('L1_EM24VHI'   ).setLogic( d.EM24VHI    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM26VHI'   ).setLogic( d.EM26VHI    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM30VHI'   ).setLogic( d.EM30VHI    & physcond).setTriggerType( TT.calo )  # isolation not yet defined
        MenuItem('L1_EM24VHIM'  ).setLogic( d.EM24VHIM   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM20A'     ).setLogic( d.EM20A      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM20C'     ).setLogic( d.EM20C      & physcond).setTriggerType( TT.calo )

        MenuItem('L1_eEM3'      ).setLogic( d.eEM3       & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM8'      ).setLogic( d.eEM8       & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM10'     ).setLogic( d.eEM10      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM15'     ).setLogic( d.eEM15      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI'   ).setLogic( d.EM15VHI    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM20'     ).setLogic( d.eEM20      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM22VH'    ).setLogic( d.EM22VH     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM22'     ).setLogic( d.eEM22      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM22VHI'  ).setLogic( d.eEM22VHI   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM20VH'   ).setLogic( d.eEM20VH    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM20VHI'  ).setLogic( d.eEM20VHI   & physcond).setTriggerType( TT.calo )


        # 2xEM, 3xEM
        MenuItem('L1_2EM3'           ).setLogic( d.EM3.x(2)             & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM7'           ).setLogic( d.EM7.x(2)             & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM10VH'        ).setLogic( d.EM10VH.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2eEM15VHI'      ).setLogic( d.eEM15VHI.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2eEM20VH'       ).setLogic( d.eEM20VH.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2eEM8'          ).setLogic( d.eEM8.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2eEM10'         ).setLogic( d.eEM10.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM8I'          ).setLogic( d.EM8I.x(2) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2EM13VH'        ).setLogic( d.EM13VH.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM18VH'        ).setLogic( d.EM18VH.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM12'          ).setLogic( d.EM12.x(2)            & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM15'          ).setLogic( d.EM15.x(2)            & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM15VH'        ).setLogic( d.EM15VH.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM15VHI'       ).setLogic( d.EM15VHI.x(2)      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM20VH'        ).setLogic( d.EM20VH.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM7_2EM3'       ).setLogic( d.EM7 & d.EM3.x(2)       & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM12_2EM3'      ).setLogic( d.EM12 & d.EM3.x(2)      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM20VH_2EM7'    ).setLogic( d.EM20VH & d.EM7.x(2)    & physcond).setTriggerType( TT.calo )

        MenuItem('L1_EM20VH_3EM10VH' ).setLogic( d.EM20VH & d.EM10VH.x(3)    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM20VH_2EM10VH_3EM8VH' ).setLogic( d.EM20VH & d.EM10VH.x(2) & d.EM8VH.x(3)    & physcond).setTriggerType( TT.calo )

        # 4xEM
        MenuItem('L1_EM15VH_3EM7'         ).setLogic( d.EM15VH & d.EM7.x(3)    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VH_3EM8VH'       ).setLogic( d.EM15VH & d.EM8VH.x(3)    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VH_3EM10VH'      ).setLogic( d.EM15VH & d.EM10VH.x(3)    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VH_2EM10VH_3EM7' ).setLogic( d.EM15VH & d.EM10VH.x(2) & d.EM7.x(3)     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM20VH_3eEM10VH' ).setLogic( d.eEM20VH & d.eEM10VH.x(3)     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM18VH_3EM8VH'       ).setLogic( d.EM18VH & d.EM8VH.x(3)    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM18VH_2EM10VH_3EM7' ).setLogic( d.EM18VH & d.EM10VH.x(2) & d.EM7.x(3)     & physcond).setTriggerType( TT.calo )

        # EM and MU
        MenuItem('L1_2eEM8VH_MU10'         ).setLogic( d.eEM8VH.x(2) & d.MU10    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM15VH_MU10'         ).setLogic( d.EM15VH & d.MU10    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM7_MU20'         ).setLogic( d.eEM7 & d.MU20    & physcond).setTriggerType( TT.calo )
        # EM, tau and jet
        MenuItem('L1_eEM15VHI_2eTAU12IM_4jJ12'         ).setLogic( d.eEM15VHI & d.eTAU12IM.x(2) & d.jJ12.x(4)    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM15VHI_2eTAU12IM_jJ25_3jJ12'         ).setLogic( d.eEM15VHI & d.eTAU12IM.x(2) & d.jJ25 & d.jJ12.x(3)   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM20VHI_eTAU20IM_2eTAU20_jJ25_3jJ20'         ).setLogic( d.eEM20VHI & d.eTAU20IM & d.eTAU20.x(2) & d.jJ25 & d.jJ20.x(3)   & physcond).setTriggerType( TT.calo )
        # EM, tau and XE
        MenuItem('L1_eEM15VHI_2eTAU12IM_gXERHO35'         ).setLogic( d.eEM15VHI & d.eTAU12IM.x(2) & d.gXERHO35    & physcond).setTriggerType( TT.calo )



        # EM and jet
        MenuItem('L1_eEM18VHI_3jJ20'         ).setLogic( d.eEM18VHI & d.jJ20.x(3)    & physcond).setTriggerType( TT.calo )

        MenuItem('L1_J15.23ETA49' ).setLogic( d.J1523ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J15.24ETA49' ).setLogic( d.J1524ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J15.31ETA49' ).setLogic( d.J1531ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J20.28ETA31' ).setLogic( d.J2028ETA31 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J20.31ETA49' ).setLogic( d.J2031ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J30.31ETA49' ).setLogic( d.J3031ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40.0ETA25' ).setLogic( d.J400ETA25 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J50.31ETA49' ).setLogic( d.J5031ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J70.31ETA49' ).setLogic( d.J7031ETA49 & physcond).setTriggerType(TT.calo)

        MenuItem('L1_J12.0ETA23'  ).setLogic( d.J120ETA23 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J15.0ETA25'  ).setLogic( d.J150ETA25 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J20.0ETA28'  ).setLogic( d.J200ETA28 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J25.0ETA23'  ).setLogic( d.J250ETA23 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J35.0ETA23'  ).setLogic( d.J350ETA23 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J20.0ETA49'  ).setLogic( d.J200ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J30.0ETA49'  ).setLogic( d.J300ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J25.0ETA23_2J15.31ETA49'  ).setLogic( d.J250ETA23 & d.J1531ETA49.x(2) & physcond).setTriggerType(TT.calo)


        MenuItem('L1_2EM3_VTE70').setLogic( d.EM3.x(2) & Not(d.TE70) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2EM3_VTE50').setLogic( d.EM3.x(2) & Not(d.TE50) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2EM8I_J50').setLogic( d.EM8I.x(2) & d.J50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2EM8I_J75').setLogic( d.EM8I.x(2) & d.J75 & physcond).setTriggerType(TT.calo)

        # HI
        MenuItem('L1_EM3_NZ'         ).setLogic( d.EM3      & Not(d.ZDC_AND) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM3_NZ'        ).setLogic( d.EM3.x(2) & Not(d.ZDC_AND) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_EM3_NL'         ).setLogic( d.EM3      & Not(d.LUCID_A) & Not(d.LUCID_C) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM3_NL'        ).setLogic( d.EM3.x(2) & Not(d.LUCID_A) & Not(d.LUCID_C) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_EM3_MV'         ).setLogic( d.EM3      & Not(d.MBTS_A & d.MBTS_C) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM3_MV_VTE50'   ).setLogic( d.EM3      & Not(d.MBTS_A & d.MBTS_C) & Not(d.TE50) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_EM3_TE50'       ).setLogic( d.EM3      & d.TE50 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM3_VTE50'      ).setLogic( d.EM3      & Not(d.TE50) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM7_VTE200'      ).setLogic( d.EM7      & Not(d.TE200) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_EM3_UNPAIRED_ISO'   ).setLogic(d.EM3 & unpaired_isocond).setTriggerType( TT.calo )
        MenuItem('L1_EM3_UNPAIRED_NONISO').setLogic(d.EM3 & unpaired_nonisocond).setTriggerType( TT.calo )
        MenuItem('L1_EM3_EMPTY'          ).setLogic(d.EM3 & cosmiccond).setTriggerType( TT.calo )
        MenuItem('L1_EM3_FIRSTEMPTY'     ).setLogic(d.EM3 & firstempty).setTriggerType( TT.calo )

        MenuItem('L1_EM5_EMPTY'          ).setLogic(d.EM5 & cosmiccond).setTriggerType( TT.calo )
        MenuItem('L1_EM5_UNPAIRED_ISO'   ).setLogic(d.EM5 & unpaired_isocond).setTriggerType( TT.calo )
        MenuItem('L1_EM5_UNPAIRED_NONISO').setLogic(d.EM5 & unpaired_nonisocond).setTriggerType( TT.calo )
        MenuItem('L1_EM5_FIRSTEMPTY'     ).setLogic(d.EM5 & firstempty).setTriggerType( TT.calo )

        MenuItem('L1_EM7_EMPTY'          ).setLogic(d.EM7 & cosmiccond).setTriggerType( TT.calo )
        MenuItem('L1_EM7_UNPAIRED_ISO'   ).setLogic(d.EM7 & unpaired_isocond).setTriggerType( TT.calo )
        MenuItem('L1_EM7_UNPAIRED_NONISO').setLogic(d.EM7 & unpaired_nonisocond).setTriggerType( TT.calo )
        MenuItem('L1_EM7_FIRSTEMPTY'     ).setLogic(d.EM7 & firstempty).setTriggerType( TT.calo )

        MenuItem('L1_EM20VH_FIRSTEMPTY'    ).setLogic( d.EM20VH & firstempty).setTriggerType( TT.calo )
        MenuItem('L1_EM22VHI_FIRSTEMPTY'    ).setLogic( d.EM22VHI & firstempty).setTriggerType( TT.calo )

        MenuItem('L1_J10_VTE100'      ).setLogic( d.J10      & Not(d.TE100) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_J10_VTE200'      ).setLogic( d.J10      & Not(d.TE200) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_J10_VTE300'      ).setLogic( d.J10      & Not(d.TE300) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_J12_VTE100'      ).setLogic( d.J12      & Not(d.TE100) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_J12_VTE200'      ).setLogic( d.J12      & Not(d.TE200) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TE50_VTE200'      ).setLogic( d.TE50      & Not(d.TE200) & physcond).setTriggerType( TT.ftk )

        MenuItem('L1_TE5_VTE200'      ).setLogic( d.TE5      & Not(d.TE200) & physcond).setTriggerType( TT.ftk )
        MenuItem('L1_TE5_VTE100'      ).setLogic( d.TE5      & Not(d.TE100) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TE5_VTE50'      ).setLogic( d.TE5      & Not(d.TE50) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TE4_VTE200'      ).setLogic( d.TE4      & Not(d.TE200) & physcond).setTriggerType( TT.ftk )
        MenuItem('L1_TE4_VTE100'      ).setLogic( d.TE4      & Not(d.TE100) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TE4_VTE50'      ).setLogic( d.TE4      & Not(d.TE50) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TE4_VTE200_EMPTY'      ).setLogic( d.TE4      & Not(d.TE200) & cosmiccond).setTriggerType( TT.calo )
        MenuItem('L1_TE5_VTE200_EMPTY'      ).setLogic( d.TE5      & Not(d.TE200) & cosmiccond).setTriggerType( TT.calo )


        # MUON ctpid=[0x20;0x2f]
        MenuItem('L1_MU0'  ).setLogic( d.MU0  & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4'  ).setLogic( d.MU4  & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU6'  ).setLogic( d.MU6  & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU10' ).setLogic( d.MU10 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU11' ).setLogic( d.MU11 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU15' ).setLogic( d.MU15 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU20' ).setLogic( d.MU20 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU21' ).setLogic( d.MU21 & physcond).setTriggerType(TT.muon)

        # 2xMU, 3xMU, 4xMU
        MenuItem('L1_2MU0'     ).setLogic( d.MU0.x(2)        & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU4'     ).setLogic( d.MU4.x(2)        & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU6'     ).setLogic( d.MU6.x(2)        & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU10'    ).setLogic( d.MU10.x(2)       & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU11'    ).setLogic( d.MU11.x(2)       & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU20'    ).setLogic( d.MU20.x(2)       & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_MU10' ).setLogic( d.MU4 & d.MU10      & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU6_MU4' ).setLogic( d.MU6 & d.MU4      & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU10_2MU6').setLogic( d.MU10 & d.MU6.x(2) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU11_2MU10').setLogic( d.MU11 & d.MU10.x(2) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU11_2MU6').setLogic( d.MU11 & d.MU6.x(2) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU10_2MU4').setLogic( d.MU10 & d.MU4.x(2) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_3MU4'     ).setLogic( d.MU4.x(3)        & physcond).setTriggerType(TT.muon)
        MenuItem('L1_4MU4'     ).setLogic( d.MU4.x(4)        & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU6_2MU4' ).setLogic( d.MU6 & d.MU4.x(2)  & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU6_3MU4' ).setLogic( d.MU6 & d.MU4.x(3)  & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU6_3MU4' ).setLogic( d.MU6.x(2) & d.MU4.x(3)  & physcond).setTriggerType(TT.muon)
        MenuItem('L1_3MU6'     ).setLogic( d.MU6.x(3)        & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU0_MU6' ).setLogic( d.MU0.x(2) & d.MU6  & physcond).setTriggerType(TT.muon)

        MenuItem('L1_2MU20_OVERLAY').setLogic( d.MU20.x(2)   & physcond).setTriggerType(TT.zerobs)

        # HI
        MenuItem('L1_MU0_NZ'   ).setLogic( d.MU0      & Not(d.ZDC_AND) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU0_NZ'  ).setLogic( d.MU0.x(2) & Not(d.ZDC_AND) & physcond).setTriggerType(TT.muon)

        MenuItem('L1_MU0_NL'   ).setLogic( d.MU0      & Not(d.LUCID_A) & Not(d.LUCID_C) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU0_NL'  ).setLogic( d.MU0.x(2) & Not(d.LUCID_A) & Not(d.LUCID_C) & physcond).setTriggerType(TT.muon)

        MenuItem('L1_MU0_MV'   ).setLogic( d.MU0      & Not(d.MBTS_A & d.MBTS_C) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU0_MV'  ).setLogic( d.MU0.x(2) & Not(d.MBTS_A & d.MBTS_C) & physcond).setTriggerType(TT.muon)

        MenuItem('L1_MU4_VTE10').setLogic( d.MU4      & Not(d.TE10) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU4_VTE10').setLogic( d.MU4.x(2)      & Not(d.TE10) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU4_VTE50').setLogic( d.MU4.x(2)      & Not(d.TE50) & physcond).setTriggerType(TT.muon)

        MenuItem('L1_MU0_TE50' ).setLogic( d.MU0      & d.TE50 & physcond).setTriggerType(TT.muon)


        MenuItem('L1_MU4_TE10' ).setLogic( d.MU4      & d.TE10 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE20' ).setLogic( d.MU4      & d.TE20 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE30' ).setLogic( d.MU4      & d.TE30 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE40' ).setLogic( d.MU4      & d.TE40 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE50' ).setLogic( d.MU4      & d.TE50 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE70' ).setLogic( d.MU4      & d.TE70 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE90' ).setLogic( d.MU4      & d.TE90 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE120' ).setLogic( d.MU4     & d.TE120 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE160' ).setLogic( d.MU4     & d.TE160 & physcond).setTriggerType(TT.muon)

        MenuItem('L1_MU4_TE10.24ETA49' ).setLogic( d.MU4      & d.TE1024ETA49 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE15.24ETA49' ).setLogic( d.MU4      & d.TE1524ETA49 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE20.24ETA49' ).setLogic( d.MU4      & d.TE2024ETA49 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE25.24ETA49' ).setLogic( d.MU4      & d.TE2524ETA49 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE30.24ETA49' ).setLogic( d.MU4      & d.TE3024ETA49 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE35.24ETA49' ).setLogic( d.MU4      & d.TE3524ETA49 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_TE40.24ETA49' ).setLogic( d.MU4      & d.TE4024ETA49 & physcond).setTriggerType(TT.muon)

        MenuItem('L1_MU0_VTE50').setLogic( d.MU0      & Not(d.TE50) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_VTE50').setLogic( d.MU4      & Not(d.TE50) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU6_VTE50').setLogic( d.MU6      & Not(d.TE50) & physcond).setTriggerType(TT.muon)

        MenuItem('L1_MU0_MV_VTE50').setLogic( d.MU0 & Not(d.MBTS_A & d.MBTS_C) & Not(d.TE50) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_MV_VTE50').setLogic( d.MU4 & Not(d.MBTS_A & d.MBTS_C) & Not(d.TE50) & physcond).setTriggerType(TT.muon)

        MenuItem('L1_MU0_UNPAIRED_ISO'   ).setLogic(d.MU0 & unpaired_isocond   ).setTriggerType( TT.muon )
        MenuItem('L1_MU0_UNPAIRED_NONISO').setLogic(d.MU0 & unpaired_nonisocond).setTriggerType( TT.muon )
        MenuItem('L1_MU0_EMPTY'          ).setLogic(d.MU0 & cosmiccond).setTriggerType( TT.muon )
        MenuItem('L1_MU0_FIRSTEMPTY'     ).setLogic(d.MU0 & firstempty).setTriggerType( TT.muon )

        MenuItem('L1_MU4_UNPAIRED_ISO'   ).setLogic(d.MU4 & unpaired_isocond   ).setTriggerType( TT.muon )
        MenuItem('L1_MU4_UNPAIRED_NONISO').setLogic(d.MU4 & unpaired_nonisocond).setTriggerType( TT.muon )
        MenuItem('L1_MU4_EMPTY'          ).setLogic(d.MU4 & cosmiccond).setTriggerType( TT.muon )
        MenuItem('L1_MU6_EMPTY'          ).setLogic(d.MU6 & cosmiccond).setTriggerType( TT.muon )
        MenuItem('L1_MU4_FIRSTEMPTY'     ).setLogic(d.MU4 & firstempty).setTriggerType( TT.muon )

        MenuItem('L1_MU6_FIRSTEMPTY'     ).setLogic(d.MU4  & firstempty).setTriggerType( TT.muon )
        MenuItem('L1_MU10_EMPTY'         ).setLogic(d.MU10 & cosmiccond).setTriggerType( TT.muon )
        MenuItem('L1_MU11_EMPTY'         ).setLogic(d.MU11 & cosmiccond).setTriggerType( TT.muon )

        MenuItem('L1_2MU0_EMPTY'         ).setLogic(d.MU0.x(2) & cosmiccond).setTriggerType( TT.muon )
        MenuItem('L1_2MU4_EMPTY'         ).setLogic(d.MU4.x(2) & cosmiccond).setTriggerType( TT.muon )
        MenuItem('L1_2MU6_UNPAIRED_ISO'  ).setLogic(d.MU6.x(2) & unpaired_isocond).setTriggerType( TT.muon )
        MenuItem('L1_2MU6_UNPAIRED_NONISO').setLogic(d.MU6.x(2) & unpaired_nonisocond).setTriggerType( TT.muon )
        MenuItem('L1_2MU6_EMPTY'         ).setLogic(d.MU6.x(2) & cosmiccond).setTriggerType( TT.muon )
        MenuItem('L1_2MU6_FIRSTEMPTY'    ).setLogic(d.MU6.x(2) & firstempty).setTriggerType( TT.muon )

        MenuItem('L1_MU20_FIRSTEMPTY' ).setLogic( d.MU20 & firstempty).setTriggerType(TT.muon)
        MenuItem('L1_MU21_FIRSTEMPTY' ).setLogic( d.MU21 & firstempty).setTriggerType(TT.muon)

        # EM and MU
        MenuItem('L1_EM7_MU10'   ).setLogic( d.EM7        & d.MU10     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM7_MU15'   ).setLogic( d.EM7        & d.MU15     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM8VH_MU15' ).setLogic( d.EM8VH      & d.MU15     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM7_MU20'   ).setLogic( d.EM7        & d.MU20     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM8VH_MU20' ).setLogic( d.EM8VH      & d.MU20     & physcond).setTriggerType(TT.muon)

        MenuItem('L1_EM7_MU11' ).setLogic( d.EM7        & d.MU11     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM8_MU10'   ).setLogic( d.EM8        & d.MU10     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM8I_MU10'  ).setLogic( d.EM8I       & d.MU10     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM15I_MU4'   ).setLogic( d.EM15  & d.EM8I     & d.MU4      & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM15VH_MU10').setLogic( d.EM15VH     & d.MU10     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM3_MU20'   ).setLogic( d.EM3        & d.MU20     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2EM8VH_MU10').setLogic( d.EM8VH.x(2) & d.MU10     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM15VH_2EM8VH_MU6').setLogic( d.EM15VH & d.EM8VH.x(2) &  d.MU6 & physcond).setTriggerType(TT.muon)

        # TAU ctpid=[0x40:0x4f]
        MenuItem('L1_TAU2'  ).setLogic( d.HA2   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU3'  ).setLogic( d.HA3   & physcond).setTriggerType( TT.calo )
        # ATR-19359
        MenuItem('L1_TAU5'  ).setLogic( d.HA5   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2TAU5' ).setLogic( d.HA5.x(2)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TAU6'  ).setLogic( d.HA6   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU8'  ).setLogic( d.HA8   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2TAU8' ).setLogic( d.HA8.x(2)  & physcond).setTriggerType(TT.calo)

        #MenuItem('L1_TAU10IM').setLogic( d.HA10IM  & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU12' ).setLogic( d.HA12  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU12' ).setLogic( d.eTAU12  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU100' ).setLogic( d.eTAU100  & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU12IL').setLogic( d.HA12IL & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU12IM').setLogic( d.HA12IM & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU12IT').setLogic( d.HA12IT & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU12I').setLogic( d.HA12I & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU15' ).setLogic( d.HA15  & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU20' ).setLogic( d.HA20  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IL' ).setLogic( d.HA20IL  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM' ).setLogic( d.HA20IM  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IT' ).setLogic( d.HA20IT  & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU20A' ).setLogic( d.HA20A  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20C' ).setLogic( d.HA20C  & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU25' ).setLogic( d.HA25  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU25IT' ).setLogic( d.HA25IT  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU25IM' ).setLogic( d.HA25IM  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU30' ).setLogic( d.HA30  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU40' ).setLogic( d.HA40  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU60' ).setLogic( d.HA60  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU100' ).setLogic( d.HA100  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU90' ).setLogic( d.HA90  & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU8_UNPAIRED_ISO'   ).setLogic( d.HA8   & unpaired_isocond ).setTriggerType( TT.calo )

        MenuItem('L1_TAU8_UNPAIRED_NONISO').setLogic( d.HA8   & unpaired_nonisocond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU8_EMPTY'          ).setLogic( d.HA8   & cosmiccond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU8_FIRSTEMPTY'     ).setLogic( d.HA8   & firstempty ).setTriggerType( TT.calo )

        MenuItem('L1_TAU12_UNPAIRED_ISO').setLogic( d.HA12   & unpaired_isocond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU12_UNPAIRED_NONISO').setLogic( d.HA12   & unpaired_nonisocond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU12_EMPTY'          ).setLogic( d.HA12  & cosmiccond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU12_FIRSTEMPTY'     ).setLogic( d.HA12   & firstempty ).setTriggerType( TT.calo )
        MenuItem('L1_TAU30_EMPTY'          ).setLogic( d.HA30   & cosmiccond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU30_UNPAIRED_ISO').setLogic( d.HA30  & unpaired_isocond ).setTriggerType( TT.calo )

        #UPC TAU
        MenuItem('L1_2TAU1_VTE50').setLogic( d.HA1.x(2)      & Not(d.TE50) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2TAU2_VTE50').setLogic( d.HA2.x(2)      & Not(d.TE50) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2TAU3_VTE50').setLogic( d.HA3.x(2)      & Not(d.TE50) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2TAU2_VTE100').setLogic( d.HA2.x(2)      & Not(d.TE100) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2TAU3_VTE100').setLogic( d.HA3.x(2)      & Not(d.TE100) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2TAU2_VTE200').setLogic( d.HA2.x(2)      & Not(d.TE200) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2TAU3_VTE200').setLogic( d.HA3.x(2)      & Not(d.TE200) & physcond).setTriggerType(TT.calo)

        MenuItem('L1_TAU1_TE4_VTE200').setLogic( d.HA1  & d.TE4    & Not(d.TE200) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TAU1_TE5_VTE200').setLogic( d.HA1  & d.TE5    & Not(d.TE200) & physcond).setTriggerType(TT.calo)

        # 3xTAU
        MenuItem('L1_TAU20_2TAU12'  ).setLogic( d.HA20 & d.HA12.x(2)  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20_2TAU12I' ).setLogic( d.HA20 & d.HA12I.x(2) &  physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU40_2TAU20IM' ).setLogic( d.HA40 & d.HA20IM.x(2) &  physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2TAU12IM' ).setLogic( d.HA20IM & d.HA12IM.x(2) &  physcond).setTriggerType( TT.calo )

        # mixed tau
        MenuItem('L1_EM15VH_2TAU12'                ).setLogic( d.EM15VH  & d.HA12.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VH_2TAU12IL'              ).setLogic( d.EM15VH  & d.HA12IL.x(2)        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15_2TAU12'                  ).setLogic( d.EM15    & d.HA12.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15_2TAU20'                  ).setLogic( d.EM15    & d.HA20.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VH_2TAU20'                ).setLogic( d.EM15VH  & d.HA20.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15_TAU40_2TAU15'            ).setLogic( d.EM15    & d.HA40 & d.HA15.x(2)   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15_2TAU12_J25_2J15_3J12'    ).setLogic( d.EM15  & d.HA12.x(2)  & d.J25 & d.J15.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_EM15VHI_2TAU12'                ).setLogic( d.EM15VHI  & d.HA12.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IL'              ).setLogic( d.EM15VHI  & d.HA12IL.x(2)        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IM'              ).setLogic( d.EM15VHI  & d.HA12IM.x(2)        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_TAU40_2TAU15'          ).setLogic( d.EM15VHI  & d.HA40 & d.HA15.x(2)   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12I_J25_2J15_3J12'  ).setLogic( d.EM15VHI  & d.HA12I.x(2)  & d.J25 & d.J15.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IM_J25_3J12'  ).setLogic( d.EM15VHI  & d.HA12IM.x(2)  & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IL_J25_3J12'  ).setLogic( d.EM15VHI  & d.HA12IL.x(2)  & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12_J25_3J12'  ).setLogic( d.EM15VHI  & d.HA12.x(2)  & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IM_4J12'  ).setLogic( d.EM15VHI  & d.HA12IM.x(2) & d.J12.x(4) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12'                ).setLogic( d.EM15HI  & d.HA12.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12IL'              ).setLogic( d.EM15HI  & d.HA12IL.x(2)        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12IM'              ).setLogic( d.EM15HI  & d.HA12IM.x(2)        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_TAU40_2TAU15'          ).setLogic( d.EM15HI  & d.HA40 & d.HA15.x(2)   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12I_J25_2J15_3J12'  ).setLogic( d.EM15HI  & d.HA12I.x(2)  & d.J25 & d.J15.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12IM_J25_3J12'  ).setLogic( d.EM15HI  & d.HA12IM.x(2)  & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12IL_J25_3J12'  ).setLogic( d.EM15HI  & d.HA12IL.x(2)  & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12_J25_3J12'  ).setLogic( d.EM15HI  & d.HA12.x(2)  & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_MU10_TAU12'       ).setLogic( d.MU10  & d.HA12          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU12I'      ).setLogic( d.MU10  & d.HA12I         & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU12IM'     ).setLogic( d.MU10  & d.HA12IM        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU20'       ).setLogic( d.MU10  & d.HA20          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU20IM'     ).setLogic( d.MU10  & d.HA20IM        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU11_TAU20IM'     ).setLogic( d.MU11  & d.HA20IM        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU20IM_J25_2J20'     ).setLogic( d.MU10  & d.HA20IM   & d.J25 & d.J20.x(2) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU20I'       ).setLogic( d.MU10  & d.HA20I        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU20_TAU12IM').setLogic( d.MU20 & d.HA12IM & physcond).setTriggerType( TT.calo)
        MenuItem('L1_MU10_eTAU20IM'     ).setLogic( d.MU10  & d.eTAU20IM        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_eTAU12IM_3jJ12'     ).setLogic( d.MU10  & d.eTAU12IM & d.jJ12.x(3)        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_eTAU12IM_jJ25_2jJ12'     ).setLogic( d.MU10  & d.eTAU12IM & d.jJ25 & d.jJ12.x(2)        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU12I_MU10_J25'  ).setLogic( d.HA12I & d.MU10 & d.J25    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU12I_MU10_J25_2J12'  ).setLogic( d.HA12I & d.MU10 & d.J25 & d.J12.x(2)  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU12_J25_2J12' ).setLogic( d.MU10 & d.HA12 & d.J25 & d.J12.x(2)     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU12IM_J25_2J12' ).setLogic( d.MU10 & d.HA12IM & d.J25 & d.J12.x(2)    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU12IL_J25_2J12' ).setLogic( d.MU10 & d.HA12IL & d.J25 & d.J12.x(2)    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU12I_MU10_J20.31ETA49' ).setLogic( d.HA12I & d.MU10 & d.J2031ETA49    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU12IM_3J12' ).setLogic( d.MU10 & d.HA12IM & d.J12.x(3)    & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU20_2TAU12I_J25_2J15_3J12'   ).setLogic( d.HA20 & d.HA12I.x(2)     & d.J25 & d.J15.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20_2TAU12_J25_2J20_3J12'   ).setLogic( d.HA20 & d.HA12.x(2)     & d.J25 & d.J20.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2TAU12IM_J25_2J20_3J12').setLogic( d.HA20IM & d.HA12IM.x(2)     & d.J25 & d.J20.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IL_2TAU12IL_J25_2J20_3J12').setLogic( d.HA20IL & d.HA12IL.x(2)     & d.J25 & d.J20.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU25IT_2TAU12IT_2J25_3J12').setLogic( d.HA25IT & d.HA12IT.x(2)     & d.J25.x(2)  & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU25IM_2TAU12IM_J25_3J12'  ).setLogic( d.HA25IM & d.HA12IM.x(2) & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU25IM_2TAU20IM_2J25_3J20').setLogic( d.HA25IM & d.HA20IM.x(2)     & d.J25.x(2)  & d.J20.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2TAU12IM_4J12').setLogic( d.HA20IM & d.HA12IM.x(2)  & d.J12.x(4) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2TAU12IM_4J12.0ETA23').setLogic( d.HA20IM & d.HA12IM.x(2)  & d.J120ETA23.x(4) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU60_2TAU40').setLogic( d.HA60 & d.HA40.x(2)  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2TAU12I_TAU20_J25_2J15_3J12'  ).setLogic( d.HA12I.x(2)   & d.HA20  & d.J25 & d.J15.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )
#######
        MenuItem('L1_2TAU20IM_3J20'  ).setLogic(  d.HA20IM.x(2) & d.J20.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2TAU20IM_J25_3J20'  ).setLogic(  d.HA20IM.x(2) & d.J25 & d.J20.x(3) & physcond).setTriggerType( TT.calo )

######

        MenuItem('L1_TAU20_2J20_XE45'              ).setLogic( d.HA20    & d.J20.x(2)   & d.XE45 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2J20_XE50'            ).setLogic( d.HA20IM   & d.J20.x(2)   & d.XE50 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2J20_XE45'            ).setLogic( d.HA20IM    & d.J20.x(2)   & d.XE45 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU25_2J20_XE45'              ).setLogic( d.HA25      & d.J20.x(2)   & d.XE45 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20_2TAU12_XE35'            ).setLogic( d.HA20    & d.HA12.x(2)  & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2TAU12IM_XE35'        ).setLogic( d.HA20IM  & d.HA12IM.x(2)  & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IL_2TAU12IL_XE35'        ).setLogic( d.HA20IL  & d.HA12IL.x(2)  & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IT_2TAU12IT_XE35'        ).setLogic( d.HA20IT  & d.HA12IT.x(2)  & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2TAU12IM_XE40'        ).setLogic( d.HA20IM  & d.HA12IM.x(2)  & d.XE40 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU40_2TAU12IM_XE40'        ).setLogic( d.HA40  & d.HA12IM.x(2)  & d.XE40 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM20VHI_TAU20IM_2TAU20_J25_3J20'          ).setLogic( d.EM20VHI  &  d.HA20IM  &  d.HA20.x(2) &  d.J25  & d.J20.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU12I_XE35'             ).setLogic( d.MU10    & d.HA12I      & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU12_XE35'              ).setLogic( d.MU10    & d.HA12       & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU12IL_XE35'            ).setLogic( d.MU10    & d.HA12IL     & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU12IM_XE35'            ).setLogic( d.MU10    & d.HA12IM     & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU12IT_XE35'            ).setLogic( d.MU10    & d.HA12IT     & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_TAU12IM_XE40'            ).setLogic( d.MU10    & d.HA12IM     & d.XE40 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU10_eTAU12IM_gXERHO35'            ).setLogic( d.MU10    & d.eTAU12IM     & d.gXERHO35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20I_2TAU12I_XE35'          ).setLogic( d.HA20I   & d.HA12I.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU20IM_2jJ20_gXERHO45'          ).setLogic( d.eTAU20IM   & d.jJ20.x(2) & d.gXERHO45 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU20IM_2eTAU12IM_4jJ12.0ETA23'          ).setLogic( d.eTAU20IM   & d.eTAU12IM.x(2) & d.jJ120ETA23.x(4) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU25IM_2eTAU20IM_2jJ25_3jJ20'          ).setLogic( d.eTAU25IM   & d.eTAU20IM.x(2) & d.jJ25.x(2) & d.jJ20.x(3)  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU40_2eTAU12IM_gXERHO40'          ).setLogic( d.eTAU40   & d.eTAU12IM.x(2) & d.gXERHO40 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU60_2eTAU40'          ).setLogic( d.eTAU60 & d.eTAU40.x(2) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_EM15VHI_TAU20IM_2TAU15_J25_2J20_3J15'          ).setLogic( d.EM15VHI  &  d.HA20IM  &  d.HA15.x(2) &  d.J25  & d.J20.x(2) & d.J15.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12I_XE35'          ).setLogic( d.EM15VHI  & d.HA12I.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IM_XE35'         ).setLogic( d.EM15VHI  & d.HA12IM.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IL_XE35'         ).setLogic( d.EM15VHI  & d.HA12IL.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12_XE35'           ).setLogic( d.EM15VHI  & d.HA12.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )

        MenuItem('L1_EM15HI_TAU20IM_2TAU15_J25_2J20_3J15').setLogic( d.EM15HI  &  d.HA20IM  &  d.HA15.x(2) &  d.J25  & d.J20.x(2) & d.J15.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12I_XE35'          ).setLogic( d.EM15HI  & d.HA12I.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12IM_XE35'         ).setLogic( d.EM15HI  & d.HA12IM.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12IL_XE35'         ).setLogic( d.EM15HI  & d.HA12IL.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12_XE35'         ).setLogic( d.EM15HI  & d.HA12.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )

        # JET ctpid=[0x60:0x7f]
        MenuItem('L1_J5'   ).setLogic( d.J5   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J10'   ).setLogic( d.J10  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J12'   ).setLogic( d.J12  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J15'   ).setLogic( d.J15  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J20'   ).setLogic( d.J20  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J25'   ).setLogic( d.J25  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J30'   ).setLogic( d.J30  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J35'   ).setLogic( d.J35  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40'   ).setLogic( d.J40  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J50'   ).setLogic( d.J50  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J60'   ).setLogic( d.J60  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75'   ).setLogic( d.J75  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75A'  ).setLogic( d.J75A & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75C'  ).setLogic( d.J75C & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J85'   ).setLogic( d.J85  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J100'  ).setLogic( d.J100 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J120'  ).setLogic( d.J120 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J150'  ).setLogic( d.J150 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J175'  ).setLogic( d.J175 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J250'  ).setLogic( d.J250 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J400'  ).setLogic( d.J400 & physcond).setTriggerType(TT.calo)

        MenuItem('L1_jJ100'  ).setLogic( d.jJ100 & physcond).setTriggerType(TT.calo)

        MenuItem('L1_J10.31ETA49').setLogic( d.J1031ETA49 & physcond).setTriggerType(TT.calo)
        #MenuItem('L1_J20.31ETA49').setLogic( d.J2031ETA49 & physcond).setTriggerType(TT.calo)
        #MenuItem('L1_J30.31ETA49').setLogic( d.J3031ETA49 & physcond).setTriggerType(TT.calo)
        #MenuItem('L1_J50.31ETA49').setLogic( d.J5031ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75.31ETA49').setLogic( d.J7531ETA49 & physcond).setTriggerType(TT.calo)
        #MenuItem('L1_J100.31ETA49').setLogic( d.J10031ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ75.31ETA49').setLogic( d.jJ7531ETA49 & physcond).setTriggerType(TT.calo)


        MenuItem('L1_J10_UNPAIRED_ISO'   ).setLogic( d.J10 & unpaired_isocond   ).setTriggerType(TT.calo)
        MenuItem('L1_J10_UNPAIRED_NONISO').setLogic( d.J10 & unpaired_nonisocond).setTriggerType(TT.calo)
        MenuItem('L1_J10_EMPTY').setLogic( d.J10 & cosmiccond ).setTriggerType(TT.calo)
        MenuItem('L1_J10_FIRSTEMPTY').setLogic( d.J10 & firstempty ).setTriggerType(TT.calo)

        MenuItem('L1_J12_UNPAIRED_ISO'   ).setLogic( d.J12 & unpaired_isocond   ).setTriggerType(TT.calo)
        MenuItem('L1_J12_UNPAIRED_NONISO').setLogic( d.J12 & unpaired_nonisocond).setTriggerType(TT.calo)
        MenuItem('L1_J12_EMPTY').setLogic( d.J12 & cosmiccond ).setTriggerType(TT.calo)
        MenuItem('L1_J12_FIRSTEMPTY').setLogic( d.J12 & firstempty ).setTriggerType(TT.calo)
        MenuItem('L1_J12_ABORTGAPNOTCALIB').setLogic( d.J12 & abortgap ).setTriggerType(TT.calo)
        MenuItem('L1_J12_UNPAIREDB1').setLogic( d.J12 & bgrp13cond  ).setTriggerType(TT.calo)
        MenuItem('L1_J12_UNPAIREDB2').setLogic( d.J12 & bgrp14cond).setTriggerType(TT.calo)

        MenuItem('L1_J50_UNPAIRED_ISO'   ).setLogic( d.J50 & unpaired_isocond   ).setTriggerType(TT.calo)
        MenuItem('L1_J50_UNPAIRED_NONISO').setLogic( d.J50 & unpaired_nonisocond).setTriggerType(TT.calo)
        MenuItem('L1_J50_ABORTGAPNOTCALIB').setLogic( d.J50 & abortgap ).setTriggerType(TT.calo)

        MenuItem('L1_J12_BGRP12').setLogic( d.J12 & bgrp12cond ).setTriggerType(TT.calo)
        MenuItem('L1_J30.31ETA49_BGRP12').setLogic( d.J3031ETA49 & bgrp12cond ).setTriggerType(TT.calo)

        MenuItem('L1_J30_EMPTY').setLogic( d.J30 & cosmiccond ).setTriggerType(TT.calo)
        MenuItem('L1_J30_UNPAIRED').setLogic( d.J30 & unpairedRcond ).setTriggerType(TT.calo)
        MenuItem('L1_J30_FIRSTEMPTY').setLogic( d.J30 & firstempty ).setTriggerType(TT.calo)

        MenuItem('L1_J10.31ETA49_EMPTY').setLogic( d.J1031ETA49 & cosmiccond ).setTriggerType(TT.calo)

        MenuItem('L1_J15.31ETA49_UNPAIRED_ISO' ).setLogic( d.J1531ETA49 & unpaired_isocond).setTriggerType(TT.calo)

        MenuItem('L1_J30.31ETA49_EMPTY').setLogic( d.J3031ETA49 & cosmiccond ).setTriggerType(TT.calo)
        MenuItem('L1_J30.31ETA49_FIRSTEMPTY').setLogic( d.J3031ETA49 & firstempty ).setTriggerType(TT.calo)
        MenuItem('L1_J30.31ETA49_UNPAIRED_ISO'   ).setLogic( d.J3031ETA49 & unpaired_isocond   ).setTriggerType(TT.calo)
        MenuItem('L1_J30.31ETA49_UNPAIRED_NONISO'   ).setLogic( d.J3031ETA49 & unpaired_nonisocond   ).setTriggerType(TT.calo)


        MenuItem('L1_J100_FIRSTEMPTY').setLogic( d.J100 & firstempty ).setTriggerType(TT.calo)
        #MenuItem('L1_J100.31ETA49_FIRSTEMPTY' ).setLogic( d.J10031ETA49 & firstempty ).setTriggerType(TT.calo)

        # multi jet
        MenuItem('L1_2J25.31ETA49'  ).setLogic( d.J2531ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2J15'   ).setLogic( d.J15.x(2)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J15_J15.31ETA49' ).setLogic( d.J15 & d.J1531ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J20_J20.31ETA49' ).setLogic( d.J20 & d.J2031ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J10'   ).setLogic( d.J10.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J15'   ).setLogic( d.J15.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J20'   ).setLogic( d.J20.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J40'   ).setLogic( d.J40.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J50'   ).setLogic( d.J50.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J75'   ).setLogic( d.J75.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4J10'   ).setLogic( d.J10.x(4)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4J15'   ).setLogic( d.J15.x(4)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4J20'   ).setLogic( d.J20.x(4)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4J20.0ETA49').setLogic( d.J200ETA49.x(4) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4J30'   ).setLogic( d.J30.x(4)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_6J15'   ).setLogic( d.J15.x(6)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75_3J20' ).setLogic( d.J75 & d.J20.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J85_3J30' ).setLogic( d.J85 & d.J30.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J30.0ETA49_2J20.0ETA49' ).setLogic( d.J300ETA49 & d.J200ETA49.x(2)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J15.0ETA25_2J15.31ETA49' ).setLogic( d.J150ETA25 & d.J1531ETA49.x(2)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40.0ETA25_2J15.31ETA49' ).setLogic( d.J400ETA25 & d.J1531ETA49.x(2)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40.0ETA25_2J25_J20.31ETA49' ).setLogic( d.J400ETA25 & d.J25.x(2) & d.J2031ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40.0ETA25_2J30_J20.31ETA49' ).setLogic( d.J400ETA25 & d.J30.x(2) & d.J2031ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J45.0ETA20_3J15.0ETA25'  ).setLogic( d.J450ETA20 & d.J150ETA25.x(3) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J50_2J40.0ETA25_3J15.0ETA25'  ).setLogic( d.J50 & d.J400ETA25.x(2) & d.J150ETA25.x(3) & physcond).setTriggerType(TT.calo)



        MenuItem('L1_4J17.0ETA22' ).setLogic( d.J170ETA22.x(4) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J25.0ETA23' ).setLogic( d.J250ETA23.x(3) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J35.0ETA23' ).setLogic( d.J350ETA23.x(3) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J15.0ETA25' ).setLogic( d.J150ETA25.x(3) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4J15.0ETA25' ).setLogic( d.J150ETA25.x(4) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_5J15.0ETA25' ).setLogic( d.J150ETA25.x(5) & physcond).setTriggerType(TT.calo)

        MenuItem('L1_3jJ35.0ETA23' ).setLogic( d.jJ350ETA23.x(3) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3jJ50' ).setLogic( d.jJ50.x(3) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4jJ15' ).setLogic( d.jJ15.x(4) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4jJ15.0ETA25' ).setLogic( d.jJ150ETA25.x(3) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ25.0ETA23_2jJ15.31ETA49' ).setLogic( d.jJ250ETA23 & d.jJ1531ETA49.x(2) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ40.0ETA25_2jJ25_jJ20.31ETA49' ).setLogic( d.jJ400ETA25 & d.jJ25.x(2) & d.jJ2031ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ85_3jJ30' ).setLogic( d.jJ85 & d.jJ30.x(3) & physcond).setTriggerType(TT.calo)

        # jet and XE
        MenuItem('L1_2jJ50_gXERHO40' ).setLogic( d.jJ50.x(2) & d.gXERHO40 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3jJ15.0ETA25_gXERHO40' ).setLogic( d.jJ150ETA25.x(3) & d.gXERHO40 & physcond).setTriggerType(TT.calo)

#        MenuItem('L1_ZB_J20').setLogic(d.ZB_EM15 & d.J20 & physcond).setTriggerType(TT.zerobs)
        if ('Physics_HI_v' in menuName or 'MC_HI_v' in menuName):
            MenuItem('L1_ZB', ctpid=240).setLogic(d.ZB_J75 & physcond).setTriggerType(TT.zerobs)
        else:
            MenuItem('L1_ZB', ctpid=240).setLogic(d.ZB_EM15 & physcond).setTriggerType(TT.zerobs)


        # combined jet - xe
        #MenuItem('L1_J40_XE50').setLogic( d.J75 & d.XE40 & physcond).setTriggerType(TT.calo)#CB ERROR???
        MenuItem('L1_J40_XE50').setLogic( d.J40 & d.XE50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40.0ETA25_XE50').setLogic( d.J400ETA25 & d.XE50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75_XE40').setLogic( d.J75 & d.XE40 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75_XE50').setLogic( d.J75 & d.XE50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2J15_XE55').setLogic( d.J15.x(2) & d.XE55 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2J40_XE45').setLogic( d.J40.x(2) & d.XE45 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2J50_XE40').setLogic( d.J50.x(2) & d.XE50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40_XE60').setLogic( d.J40 & d.XE60 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J30.0ETA49_XE50').setLogic( d.J300ETA49 & d.XE50 & physcond).setTriggerType(TT.calo)

        MenuItem('L1_3J15.0ETA25_XE40').setLogic( d.J150ETA25.x(3) & d.XE40 & physcond).setTriggerType(TT.calo)

        # combined em - jet
        MenuItem('L1_EM18VHI_3J20' ).setLogic( d.EM18VHI  & d.J20.x(3)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_EM20VH_3J20' ).setLogic( d.EM20VH  & d.J20.x(3)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_EM13VH_3J20' ).setLogic( d.EM13VH  & d.J20.x(3)  & physcond).setTriggerType(TT.calo)
        ### ATR-15524
        MenuItem('L1_EM18VH_3J20' ).setLogic( d.EM18VH  & d.J20.x(3)  & physcond).setTriggerType(TT.calo)

        # combined mu - jet
        MenuItem('L1_MU4_J12'   ).setLogic( d.MU4 & d.J12    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU4_J15'   ).setLogic( d.MU4 & d.J15    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU6_J20'   ).setLogic( d.MU6 & d.J20    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU6_J30.0ETA49_2J20.0ETA49').setLogic( d.MU6 & d.J300ETA49 & d.J200ETA49.x(2) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU6_J40'   ).setLogic( d.MU6 & d.J40    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU6_J75'   ).setLogic( d.MU6 & d.J75    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU4_3J15'  ).setLogic( d.MU4 & d.J15.x(3)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU4_3J20'  ).setLogic( d.MU4 & d.J20.x(3)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU4_J30'   ).setLogic( d.MU4 & d.J30    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU4_J50'   ).setLogic( d.MU4 & d.J50    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU10_3J20'  ).setLogic( d.MU10 & d.J20.x(3)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU10_2J20'  ).setLogic( d.MU10 & d.J20.x(2)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU10_2J15_J20'  ).setLogic( d.MU10 & d.J15.x(2) & d.J20  & physcond).setTriggerType(TT.calo)

        MenuItem('L1_MU10_2jJ15_jJ20'  ).setLogic( d.MU10 & d.jJ15.x(2) & d.jJ20  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU10_2jJ20'  ).setLogic( d.MU10 & d.jJ20.x(2)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU10_2jJ40'  ).setLogic( d.MU10 & d.jJ40.x(2)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU20_2jJ40'  ).setLogic( d.MU20 & d.jJ40.x(2)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU20_gXERHO30'  ).setLogic( d.MU20 & d.gXERHO30  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU20_J50'  ).setLogic( d.MU20 & d.J50  & physcond).setTriggerType(TT.calo)

        MenuItem('L1_MU4_J50_XE40'   ).setLogic( d.MU4 & d.J50 & d.XE40  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2MU4_J40_XE20'  ).setLogic( d.MU4.x(2) & d.J40 & d.XE20  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU20_J40'  ).setLogic( d.MU20 & d.J40 & physcond).setTriggerType(TT.calo) ## ATR-14377
        MenuItem('L1_MU20_jJ40'  ).setLogic( d.MU20 & d.jJ40 & physcond).setTriggerType(TT.calo) ## ATR-14377
        MenuItem('L1_MU20_XE30' ).setLogic( d.MU20 & d.XE30 & physcond) ## ATR-14377
        # HI
        MenuItem('L1_J15_NZ' ).setLogic( d.J15      & Not(d.ZDC_AND) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2J15_NZ').setLogic( d.J15.x(2) & Not(d.ZDC_AND) & physcond).setTriggerType(TT.calo)

        MenuItem('L1_J15_NL' ).setLogic( d.J15      & Not(d.LUCID_A) & Not(d.LUCID_C) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2J15_NL').setLogic( d.J15.x(2) & Not(d.LUCID_A) & Not(d.LUCID_C) & physcond).setTriggerType(TT.calo)

        # XE
        MenuItem('L1_XE35').setLogic( d.XE35 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE40').setLogic( d.XE40 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE45').setLogic( d.XE45 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE50').setLogic( d.XE50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE55').setLogic( d.XE55 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE60').setLogic( d.XE60 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE70').setLogic( d.XE70 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE80').setLogic( d.XE80 & physcond).setTriggerType(TT.calo)

        MenuItem('L1_gXERHO20').setLogic( d.gXERHO20 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_gXERHO50').setLogic( d.gXERHO50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_gXEPUFIT20').setLogic( d.gXEPUFIT20 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_gXEPUFIT50').setLogic( d.gXEPUFIT50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_gXE50').setLogic( d.gXE50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jXE50').setLogic( d.jXE50 & physcond).setTriggerType(TT.calo)

        MenuItem('L1_XE10').setLogic( d.XE10 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE20').setLogic( d.XE20 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE25').setLogic( d.XE25 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE30').setLogic( d.XE30 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE65').setLogic( d.XE65 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE75').setLogic( d.XE75 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE150').setLogic( d.XE150 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE300').setLogic( d.XE300 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XE35_BGRP7').setLogic( d.XE35 & bgrp7cond).setTriggerType(TT.calo)
        MenuItem('L1_XE40_BGRP7').setLogic( d.XE40 & bgrp7cond).setTriggerType(TT.calo)
        MenuItem('L1_XE45_BGRP7').setLogic( d.XE45 & bgrp7cond).setTriggerType(TT.calo)
        MenuItem('L1_XE50_BGRP7').setLogic( d.XE50 & bgrp7cond).setTriggerType(TT.calo)
        MenuItem('L1_XE55_BGRP7').setLogic( d.XE55 & bgrp7cond).setTriggerType(TT.calo)
        MenuItem('L1_XE60_BGRP7').setLogic( d.XE60 & bgrp7cond).setTriggerType(TT.calo)
        MenuItem('L1_XE70_BGRP7').setLogic( d.XE70 & bgrp7cond).setTriggerType(TT.calo)
        MenuItem('L1_XE80_BGRP7').setLogic( d.XE80 & bgrp7cond).setTriggerType(TT.calo)
        # XS
        MenuItem('L1_XS20').setLogic( d.XS20.x(1) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XS30').setLogic( d.XS30.x(1) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XS40').setLogic( d.XS40.x(1) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XS45').setLogic( d.XS45.x(1) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XS50').setLogic( d.XS50.x(1) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XS55').setLogic( d.XS55.x(1) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XS60').setLogic( d.XS60.x(1) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_XS65').setLogic( d.XS65.x(1) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_EM10_XS20').setLogic( d.EM10 & d.XS20.x(1) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_EM12_XS20').setLogic( d.EM12 & d.XS20.x(1) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_EM15_XS30').setLogic( d.EM15 & d.XS30.x(1) & physcond).setTriggerType(TT.calo)

        # TE
        MenuItem('L1_TE0' ).setLogic( d.TE0  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE2' ).setLogic( d.TE2  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE3' ).setLogic( d.TE3  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE4' ).setLogic( d.TE4  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE5' ).setLogic( d.TE5  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE10').setLogic( d.TE10 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE15').setLogic( d.TE15 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE20').setLogic( d.TE20 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE25').setLogic( d.TE25 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE30').setLogic( d.TE30 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE40').setLogic( d.TE40 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE45').setLogic( d.TE45 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE50').setLogic( d.TE50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE55').setLogic( d.TE55 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE60').setLogic( d.TE60 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE65').setLogic( d.TE65 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE70').setLogic( d.TE70 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE90').setLogic( d.TE90 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE100').setLogic( d.TE100 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE120').setLogic( d.TE120 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE140').setLogic( d.TE140 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE160').setLogic( d.TE160 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE200').setLogic( d.TE200 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE280').setLogic( d.TE280 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE300').setLogic( d.TE300 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE360').setLogic( d.TE360 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE2000').setLogic( d.TE2000 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE4000').setLogic( d.TE4000 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE10000').setLogic( d.TE10000 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE12000').setLogic( d.TE12000 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE14000').setLogic( d.TE14000 & physcond).setTriggerType(TT.calo)

        MenuItem('L1_TE0.24ETA49').setLogic( d.TE024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE3.24ETA49').setLogic( d.TE324ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE5.24ETA49').setLogic( d.TE524ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE10.24ETA49').setLogic( d.TE1024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE15.24ETA49').setLogic( d.TE1524ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE20.24ETA49').setLogic( d.TE2024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE25.24ETA49').setLogic( d.TE2524ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE30.24ETA49').setLogic( d.TE3024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE35.24ETA49').setLogic( d.TE3524ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE40.24ETA49').setLogic( d.TE4024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE45.24ETA49').setLogic( d.TE4524ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE50.24ETA49').setLogic( d.TE5024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE55.24ETA49').setLogic( d.TE5524ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE60.24ETA49').setLogic( d.TE6024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE65.24ETA49').setLogic( d.TE6524ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE70.24ETA49').setLogic( d.TE7024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE80.24ETA49').setLogic( d.TE8024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE110.24ETA49').setLogic( d.TE11024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE150.24ETA49').setLogic( d.TE15024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE180.24ETA49').setLogic( d.TE18024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE2000.24ETA49').setLogic( d.TE200024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE5000.24ETA49').setLogic( d.TE200024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE6500.24ETA49').setLogic( d.TE200024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE8000.24ETA49').setLogic( d.TE200024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE9000.24ETA49').setLogic( d.TE200024ETA49 & physcond).setTriggerType(TT.calo)

        # HI items
        MenuItem('L1_TE3.0ETA49').setLogic( d.TE30ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE7.0ETA49').setLogic( d.TE70ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE500.0ETA49').setLogic( d.TE5000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE600.0ETA49').setLogic( d.TE6000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE1500.0ETA49').setLogic( d.TE15000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE3000.0ETA49').setLogic( d.TE30000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE3500.0ETA49').setLogic( d.TE35000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE5000.0ETA49').setLogic( d.TE50000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE6500.0ETA49').setLogic( d.TE65000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE8000.0ETA49').setLogic( d.TE80000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE9000.0ETA49').setLogic( d.TE90000ETA49 & physcond).setTriggerType(TT.calo)

        MenuItem('L1_TE500.0ETA49_OVERLAY').setLogic( d.TE5000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE600.0ETA49_OVERLAY').setLogic( d.TE6000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE1500.0ETA49_OVERLAY').setLogic( d.TE15000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE3000.0ETA49_OVERLAY').setLogic( d.TE30000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE3500.0ETA49_OVERLAY').setLogic( d.TE35000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE5000.0ETA49_OVERLAY').setLogic( d.TE50000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE6500.0ETA49_OVERLAY').setLogic( d.TE65000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE8000.0ETA49_OVERLAY').setLogic( d.TE80000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE9000.0ETA49_OVERLAY').setLogic( d.TE90000ETA49 & physcond).setTriggerType(TT.zerobs)

        MenuItem('L1_TE50_VTE600.0ETA49').setLogic( d.TE50 & Not(d.TE6000ETA49) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE50_VTE600.0ETA49_PEB').setLogic( d.TE50 & Not(d.TE6000ETA49) & physcond).setTriggerType(TT.ftk)
        MenuItem('L1_TE600.0ETA49_PEB').setLogic( d.TE6000ETA49 & physcond).setTriggerType(TT.calo)

        MenuItem('L1_VTE2' ).setLogic( Not(d.TE2)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_VTE3' ).setLogic( Not(d.TE3)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_VTE4' ).setLogic( Not(d.TE4)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_VTE5' ).setLogic( Not(d.TE5)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_VTE10').setLogic( Not(d.TE10) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_VTE20').setLogic( Not(d.TE20) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_VTE50').setLogic( Not(d.TE50) & physcond).setTriggerType(TT.calo)

        MenuItem('L1_TE5_NZ').setLogic( d.TE5 & Not(d.ZDC_AND) & physcond).setTriggerType(TT.calo)

        # MBTS
        MBTS_1   = d.MBTS_A | d.MBTS_C
        MBTS_2   = (d.MBTS_A.x(2) | d.MBTS_C.x(2) | d.MBTS_A) & (d.MBTS_A.x(2) | d.MBTS_C.x(2) | d.MBTS_C)
        MBTS_1_1 = d.MBTS_A & d.MBTS_C
        MBTS_2_2 = d.MBTS_A.x(2) & d.MBTS_C.x(2)
        MBTS_2_A = d.MBTS_A.x(2)
        MBTS_2_C = d.MBTS_C.x(2)
        MBTS_3_3 = d.MBTS_A.x(3) & d.MBTS_C.x(3)
        MBTS_4_4 = d.MBTS_A.x(4) & d.MBTS_C.x(4)
        MBTS_4_A = d.MBTS_A.x(4)
        MBTS_4_C = d.MBTS_C.x(4)


        MenuItem('L1_MBTS_1'             ).setLogic( MBTS_1   & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_OVERLAY'     ).setLogic( MBTS_1   & physcond ).setTriggerType(TT.zerobs)
        MenuItem('L1_MBTS_2'             ).setLogic( MBTS_2   & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_1'           ).setLogic( MBTS_1_1 & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_1_OVERLAY'     ).setLogic( MBTS_1_1   & physcond ).setTriggerType(TT.zerobs)
        MenuItem('L1_MBTS_2_2'           ).setLogic( MBTS_2_2 & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_A'           ).setLogic( MBTS_2_A & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_C'           ).setLogic( MBTS_2_C & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_3_3'           ).setLogic( MBTS_3_3 & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_4'           ).setLogic( MBTS_4_4 & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_A'           ).setLogic( MBTS_4_A & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_C'           ).setLogic( MBTS_4_C & physcond ).setTriggerType(TT.minb)


        MenuItem('L1_MBTS_1_UNPAIRED_ISO'  ).setLogic( MBTS_1   & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_UNPAIRED_ISO'  ).setLogic( MBTS_2   & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_1_UNPAIRED_ISO').setLogic( MBTS_1_1 & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_2_UNPAIRED_ISO').setLogic( MBTS_2_2 & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_3_3_UNPAIRED_ISO').setLogic( MBTS_3_3 & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_4_UNPAIRED_ISO').setLogic( MBTS_3_3 & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_UNPAIRED_NONISO').setLogic( MBTS_1 & unpaired_nonisocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_UNPAIRED_NONISO').setLogic( MBTS_2 & unpaired_nonisocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_A_UNPAIRED_ISO' ).setLogic( MBTS_4_A & unpaired_isocond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_C_UNPAIRED_ISO' ).setLogic( MBTS_4_C & unpaired_isocond ).setTriggerType(TT.minb)

        MenuItem('L1_MBTS_1_EMPTY'       ).setLogic( MBTS_1   & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_EMPTY'       ).setLogic( MBTS_2   & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_1_EMPTY'     ).setLogic( MBTS_1_1 & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_2_EMPTY'     ).setLogic( MBTS_2_2 & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_3_3_EMPTY'     ).setLogic( MBTS_3_3 & cosmiccond ).setTriggerType(TT.minb)

        MenuItem('L1_MBTS_1_1_VTE50'     ).setLogic( MBTS_1_1  & Not(d.TE50) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_2_VTE50'     ).setLogic( MBTS_2_2  & Not(d.TE50) & physcond).setTriggerType(TT.minb)

        MenuItem('L1_MBTS_1_VTE2'     ).setLogic( MBTS_1  & Not(d.TE2) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_VTE3'     ).setLogic( MBTS_1  & Not(d.TE3) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_VTE4'     ).setLogic( MBTS_1  & Not(d.TE4) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_VTE5'     ).setLogic( MBTS_1  & Not(d.TE5) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_VTE10'     ).setLogic( MBTS_1  & Not(d.TE10) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_VTE70'     ).setLogic( MBTS_1  & Not(d.TE70) & physcond).setTriggerType(TT.minb)

        MenuItem('L1_MBTS_2_VTE2'     ).setLogic( MBTS_2  & Not(d.TE2) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_VTE3'     ).setLogic( MBTS_2  & Not(d.TE3) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_VTE4'     ).setLogic( MBTS_2  & Not(d.TE4) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_VTE5'     ).setLogic( MBTS_2  & Not(d.TE5) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_VTE10'     ).setLogic( MBTS_2  & Not(d.TE10) & physcond).setTriggerType(TT.minb)

        MenuItem('L1_MBTSA0' ).setLogic( d.MBTS_A0 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA1' ).setLogic( d.MBTS_A1 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA2' ).setLogic( d.MBTS_A2 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA3' ).setLogic( d.MBTS_A3 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA4' ).setLogic( d.MBTS_A4 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA5' ).setLogic( d.MBTS_A5 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA6' ).setLogic( d.MBTS_A6 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA7' ).setLogic( d.MBTS_A7 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA8' ).setLogic( d.MBTS_A8 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA10').setLogic( d.MBTS_A10 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA12').setLogic( d.MBTS_A12 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA14').setLogic( d.MBTS_A14 & physcond).setTriggerType(TT.minb)

        MenuItem('L1_MBTSA9' ).setLogic( d.MBTS_A9 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA11').setLogic( d.MBTS_A11 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA13').setLogic( d.MBTS_A13 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSA15').setLogic( d.MBTS_A15 & physcond).setTriggerType(TT.minb)

        MenuItem('L1_MBTSC0' ).setLogic( d.MBTS_C0 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC1' ).setLogic( d.MBTS_C1 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC2' ).setLogic( d.MBTS_C2 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC3' ).setLogic( d.MBTS_C3 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC4' ).setLogic( d.MBTS_C4 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC5' ).setLogic( d.MBTS_C5 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC6' ).setLogic( d.MBTS_C6 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC7' ).setLogic( d.MBTS_C7 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC8' ).setLogic( d.MBTS_C8 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC10').setLogic( d.MBTS_C10 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC12').setLogic( d.MBTS_C12 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC14').setLogic( d.MBTS_C14 & physcond).setTriggerType(TT.minb)

        MenuItem('L1_MBTSC9' ).setLogic( d.MBTS_C9 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC11').setLogic( d.MBTS_C11 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC13').setLogic( d.MBTS_C13 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTSC15').setLogic( d.MBTS_C15 & physcond).setTriggerType(TT.minb)

        MenuItem('L1_MBTS_1_BGRP9'    ).setLogic( MBTS_1 & bgrp9cond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_1_BGRP9'  ).setLogic( MBTS_1_1 & bgrp9cond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_BGRP9'    ).setLogic( MBTS_2 & bgrp9cond ).setTriggerType(TT.minb)

        MenuItem('L1_MBTS_1_BGRP11'   ).setLogic( MBTS_1 & bgrp11cond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_1_BGRP11' ).setLogic( MBTS_1_1 & bgrp11cond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_BGRP11'   ).setLogic( MBTS_2 & bgrp11cond ).setTriggerType(TT.minb)


        # ZDC
        ZDC_A_C = d.ZDC_A & d.ZDC_C
        VZDC_A_C = Not(d.ZDC_A) & Not(d.ZDC_C)
        VZDC_AORC = Not(d.ZDC_A) | Not(d.ZDC_C)
        MenuItem('L1_ZDC'               ).setLogic((d.ZDC_A | d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A'             ).setLogic( d.ZDC_A & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_C'             ).setLogic( d.ZDC_C & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_AND'           ).setLogic( d.ZDC_AND & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_C'           ).setLogic( ZDC_A_C & physcond).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_A_C_OVERLAY'   ).setLogic( ZDC_A_C & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_ZDC_A_OVERLAY'   ).setLogic( d.ZDC_A & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_ZDC_C_OVERLAY'   ).setLogic( d.ZDC_C & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_ZDC_A_C_VTE50_OVERLAY'     ).setLogic( ZDC_A_C & Not(d.TE50) & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE5_OVERLAY'   ).setLogic( d.TE5 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE10_OVERLAY'   ).setLogic( d.TE10 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE20_OVERLAY'   ).setLogic( d.TE20 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE50_OVERLAY'   ).setLogic( d.TE50 & physcond).setTriggerType(TT.zerobs)

        MenuItem('L1_ZDC_VTE200'        ).setLogic((d.ZDC_A | d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_AND_VTE50'     ).setLogic( d.ZDC_AND & Not(d.TE50) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_C_VTE50'     ).setLogic( ZDC_A_C & Not(d.TE50) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_C_TE50'      ).setLogic( ZDC_A_C & d.TE50 & physcond).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_A_C_VTE50_PEB'     ).setLogic( ZDC_A_C & Not(d.TE50) & physcond).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_A_VTE20'      ).setLogic( d.ZDC_A & Not(d.TE20) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_C_VTE20'      ).setLogic( d.ZDC_C & Not(d.TE20) & physcond).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_A_C_VTE200'           ).setLogic( ZDC_A_C & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_C_TE5_VTE200'       ).setLogic( ZDC_A_C & d.TE5 & Not(d.TE200) & physcond).setTriggerType(TT.minb)

        MenuItem('L1_VZDC_A_C_VTE50'       ).setLogic( VZDC_A_C & Not(d.TE50) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_VZDC_A_C_TE5_VTE200'       ).setLogic( VZDC_A_C & d.TE5 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_VZDC_A_C_TE20_VTE200'       ).setLogic( VZDC_A_C & d.TE20 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_VZDC_AORC_TE5_VTE200'      ).setLogic( VZDC_AORC & d.TE5 & Not(d.TE200) & physcond).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_MBTS_1'        ).setLogic((d.ZDC_A | d.ZDC_C) & MBTS_1 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_MBTS_2'        ).setLogic((d.ZDC_A | d.ZDC_C) & MBTS_2 & physcond).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_MBTS_1_1'      ).setLogic((d.ZDC_A | d.ZDC_C) & MBTS_1_1 & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_MBTS_2_2'      ).setLogic((d.ZDC_A | d.ZDC_C) & MBTS_2_2 & physcond).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_EMPTY'           ).setLogic( (d.ZDC_A | d.ZDC_C) & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_EMPTY'           ).setLogic( d.ZDC_A & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_C_EMPTY'           ).setLogic( d.ZDC_C & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_UNPAIRED_ISO'    ).setLogic( (d.ZDC_A | d.ZDC_C) & unpaired_isocond ).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_UNPAIRED_NONISO' ).setLogic( (d.ZDC_A | d.ZDC_C) & unpaired_nonisocond ).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_AND_EMPTY'           ).setLogic( d.ZDC_AND & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_AND_UNPAIRED_ISO'    ).setLogic( d.ZDC_AND & unpaired_isocond ).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_AND_UNPAIRED_NONISO' ).setLogic( d.ZDC_AND & unpaired_nonisocond ).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_UNPAIRED_NONISO' ).setLogic( d.ZDC_A & unpaired_nonisocond ).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_C_UNPAIRED_NONISO' ).setLogic( d.ZDC_C & unpaired_nonisocond ).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_A_C_EMPTY'           ).setLogic( ZDC_A_C & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_C_UNPAIRED_ISO'    ).setLogic( ZDC_A_C & unpaired_isocond ).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_C_UNPAIRED_NONISO' ).setLogic( ZDC_A_C & unpaired_nonisocond ).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_UNPAIRED_ISO'    ).setLogic( d.ZDC_A & unpaired_isocond ).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_C_UNPAIRED_ISO'    ).setLogic( d.ZDC_C & unpaired_isocond ).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_A_C_BGRP9'      ).setLogic( ZDC_A_C & bgrp9cond & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_BGRP9'      ).setLogic( d.ZDC_A & bgrp9cond & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_C_BGRP9'      ).setLogic( d.ZDC_C & bgrp9cond & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_C_BGRP11'     ).setLogic( ZDC_A_C & bgrp11cond & physcond).setTriggerType(TT.minb)

  # ATR-12470
        ZDC_XOR = (d.ZDC_A & Not(d.ZDC_C)) | (d.ZDC_C & Not(d.ZDC_A))
        MenuItem('L1_ZDC_A_VZDC_C'      ).setLogic(d.ZDC_A & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_C_VZDC_A'      ).setLogic(d.ZDC_C & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_VZDC_C_VTE200'      ).setLogic(d.ZDC_A & Not(d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_C_VZDC_A_VTE200'      ).setLogic(d.ZDC_C & Not(d.ZDC_A) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_ZDC_A_VZDC_C_VTE200'      ).setLogic(MBTS_1 & d.ZDC_A & Not(d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_ZDC_C_VZDC_A_VTE200'      ).setLogic(MBTS_1 & d.ZDC_C & Not(d.ZDC_A) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE3.0ETA49_ZDC_A_VZDC_C_VTE200'      ).setLogic(d.TE30ETA49 & d.ZDC_A & Not(d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE3.0ETA49_ZDC_C_VZDC_A_VTE200'      ).setLogic(d.TE30ETA49 & d.ZDC_C & Not(d.ZDC_A) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE4_ZDC_A_VZDC_C_VTE200'      ).setLogic(d.TE4 & d.ZDC_A & Not(d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE4_ZDC_C_VZDC_A_VTE200'      ).setLogic(d.TE4 & d.ZDC_C & Not(d.ZDC_A) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE5_ZDC_A_VZDC_C_VTE200'      ).setLogic(d.TE5 & d.ZDC_A & Not(d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE5_ZDC_C_VZDC_A_VTE200'      ).setLogic(d.TE5 & d.ZDC_C & Not(d.ZDC_A) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE7.0ETA49_ZDC_A_VZDC_C_VTE200'      ).setLogic(d.TE70ETA49 & d.ZDC_A & Not(d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE7.0ETA49_ZDC_C_VZDC_A_VTE200'      ).setLogic(d.TE70ETA49 & d.ZDC_C & Not(d.ZDC_A) & Not(d.TE200) & physcond).setTriggerType(TT.minb)



        MenuItem('L1_ZDC_XOR'           ).setLogic(ZDC_XOR  & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_TE3.0ETA49_VTE200'           ).setLogic(ZDC_XOR  & d.TE30ETA49 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_TE4_VTE200'           ).setLogic(ZDC_XOR  & d.TE4 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_TE5_VTE200'           ).setLogic(ZDC_XOR  & d.TE5 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_TE20_VTE200'           ).setLogic(ZDC_XOR  & d.TE20 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_TRT_VTE200'           ).setLogic(ZDC_XOR  & d.NIMTRT & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_VTE50'               ).setLogic(ZDC_XOR  & Not(d.TE50) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_VTE200'               ).setLogic(ZDC_XOR  & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_VTE200_MBTS_1'               ).setLogic(ZDC_XOR  & Not(d.TE200) & MBTS_1 & physcond).setTriggerType(TT.minb)

        # ATR-14967
        MenuItem('L1_EM3_VZDC_A'               ).setLogic( d.EM3 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_EM3_VZDC_C'               ).setLogic( d.EM3 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_2EM3_VZDC_A'               ).setLogic( d.EM3.x(2) & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_2EM3_VZDC_C'               ).setLogic( d.EM3.x(2) & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_EM5_VZDC_A'               ).setLogic( d.EM5 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_EM5_VZDC_C'               ).setLogic( d.EM5 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE5_VZDC_A'               ).setLogic( d.TE5 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE5_VZDC_C'               ).setLogic( d.TE5 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE10_VZDC_A'               ).setLogic( d.TE10 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE10_VZDC_C'               ).setLogic( d.TE10 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE20_VZDC_A'               ).setLogic( d.TE20 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE20_VZDC_C'               ).setLogic( d.TE20 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_VTE10_VZDC_A'               ).setLogic( Not(d.TE10) & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_VTE10_VZDC_C'               ).setLogic( Not(d.TE10) & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J5_VZDC_A'               ).setLogic( d.J5 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J5_VZDC_C'               ).setLogic( d.J5 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J10_VZDC_A'               ).setLogic( d.J10 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J10_VZDC_C'               ).setLogic( d.J10 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J15_VZDC_A'               ).setLogic( d.J15 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J15_VZDC_C'               ).setLogic( d.J15 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J20_VZDC_A'               ).setLogic( d.J20 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J20_VZDC_C'               ).setLogic( d.J20 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MU4_VZDC_A'               ).setLogic( d.MU4 & Not(d.ZDC_A) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_VZDC_C'               ).setLogic( d.MU4 & Not(d.ZDC_C) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU4_VZDC_A'               ).setLogic( d.MU4.x(2) & Not(d.ZDC_A) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU4_VZDC_C'               ).setLogic( d.MU4.x(2) & Not(d.ZDC_C) & physcond).setTriggerType(TT.muon)

        MenuItem('L1_MU4_VTE10_VZDC_A').setLogic( d.MU4      & Not(d.TE10) & Not(d.ZDC_A) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_VTE10_VZDC_C').setLogic( d.MU4      & Not(d.TE10) & Not(d.ZDC_C) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU4_VTE10_VZDC_A').setLogic( d.MU4.x(2)      & Not(d.TE10) & Not(d.ZDC_A) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU4_VTE10_VZDC_C').setLogic( d.MU4.x(2)      & Not(d.TE10) & Not(d.ZDC_C) & physcond).setTriggerType(TT.muon)

        MenuItem('L1_MU4_VZDC_A_C'               ).setLogic( d.MU4 & VZDC_A_C & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4_VZDC_AORC_VTE200'               ).setLogic( d.MU4 & VZDC_AORC & Not(d.TE200) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU6_VZDC_A'               ).setLogic( d.MU6 & Not(d.ZDC_A) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU6_VZDC_C'               ).setLogic( d.MU6 & Not(d.ZDC_C) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU6_VTE10_VZDC_A').setLogic( d.MU6      & Not(d.TE10) & Not(d.ZDC_A) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU6_VTE10_VZDC_C').setLogic( d.MU6      & Not(d.TE10) & Not(d.ZDC_C) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU6_VZDC_A_C'               ).setLogic( d.MU6 & VZDC_A_C & physcond).setTriggerType(TT.muon)
        MenuItem('L1_RD0_FILLED_VZDC_A'               ).setLogic( d.RNDM0 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_RD0_FILLED_VZDC_C'               ).setLogic( d.RNDM0 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_C_VZDC_A'               ).setLogic( MBTS_4_C & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_A_VZDC_C'               ).setLogic( MBTS_4_A & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_C_VZDC_A'               ).setLogic( MBTS_2_C & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_A_VZDC_C'               ).setLogic( MBTS_2_A & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)




        # VDM
        MenuItem('L1_ZDC_A_C_BGRP7'     ).setLogic( ZDC_A_C & bgrp7cond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_BGRP7'       ).setLogic( (d.LUCID_A | d.LUCID_C) & bgrp7cond).setTriggerType(TT.minb)

        # LUCID
        #MenuItem('L1_LUCID_COMM'      ).setLogic( d.LUCID_COMM        & physcond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_A'           ).setLogic( d.LUCID_A             & physcond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_C'           ).setLogic( d.LUCID_C             & physcond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_A_C',        ).setLogic( d.LUCID_A & d.LUCID_C   & physcond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID'             ).setLogic((d.LUCID_A | d.LUCID_C)  & physcond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_COMM'        ).setLogic( d.LUCID_COMM          & physcond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_COMM_EMPTY'  ).setLogic( d.LUCID_COMM          & cosmiccond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_EMPTY'       ).setLogic((d.LUCID_A | d.LUCID_C)  & cosmiccond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_A_C_EMPTY'   ).setLogic( d.LUCID_A & d.LUCID_C   & cosmiccond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_UNPAIRED_ISO').setLogic((d.LUCID_A | d.LUCID_C)  & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_COMM_UNPAIRED_ISO' ).setLogic( d.LUCID_COMM        & unpaired_isocond ).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_A_C_UNPAIRED_ISO'  ).setLogic( d.LUCID_A & d.LUCID_C & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_A_C_UNPAIRED_NONISO').setLogic(d.LUCID_A & d.LUCID_C & unpaired_nonisocond).setTriggerType(TT.minb)

        MenuItem('L1_LUCID_BGRP9').setLogic( (d.LUCID_A | d.LUCID_C) & bgrp9cond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_BGRP11').setLogic( (d.LUCID_A | d.LUCID_C) & bgrp11cond).setTriggerType(TT.minb)

        # BCM
        MenuItem('L1_BCM_Wide'                   ).setLogic( d.BCM_Wide & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_BCM_Wide_BGRP0'             ).setLogic( d.BCM_Wide & d.BGRP0 ).setTriggerType(TT.minb)
        MenuItem('L1_BCM_Wide_EMPTY'             ).setLogic( d.BCM_Wide & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_BCM_Wide_UNPAIRED_ISO'      ).setLogic( d.BCM_Wide & unpaired_isocond ).setTriggerType(TT.minb)
        MenuItem('L1_BCM_Wide_UNPAIRED_NONISO'   ).setLogic( d.BCM_Wide & unpaired_nonisocond ).setTriggerType(TT.minb)

        MenuItem('L1_BCM_HT_BGRP0'               ).setLogic( d.BCM_Comb.x(7) & d.BGRP0).setTriggerType(TT.minb)
        MenuItem('L1_BCM_AC_CA_BGRP0'            ).setLogic((d.BCM_AtoC | d.BCM_CtoA) & d.BGRP0).setTriggerType(TT.minb)
        MenuItem('L1_BCM_AC_CA_UNPAIRED_ISO'     ).setLogic((d.BCM_AtoC | d.BCM_CtoA) & unpaired_isocond).setTriggerType(TT.minb)

        MenuItem('L1_BCM_AC_UNPAIRED_ISO'     ).setLogic( d.BCM_AtoC & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_BCM_CA_UNPAIRED_ISO'     ).setLogic( d.BCM_CtoA & unpaired_isocond).setTriggerType(TT.minb)

        MenuItem('L1_BCM_AC_UNPAIRED_NONISO'     ).setLogic( d.BCM_AtoC & unpaired_nonisocond).setTriggerType(TT.minb)
        MenuItem('L1_BCM_CA_UNPAIRED_NONISO'     ).setLogic( d.BCM_CtoA & unpaired_nonisocond).setTriggerType(TT.minb)

        MenuItem('L1_BCM_AC_ABORTGAPNOTCALIB'     ).setLogic( d.BCM_AtoC & abortgap).setTriggerType(TT.minb)
        MenuItem('L1_BCM_CA_ABORTGAPNOTCALIB'     ).setLogic( d.BCM_CtoA & abortgap).setTriggerType(TT.minb)
        MenuItem('L1_BCM_Wide_ABORTGAPNOTCALIB'   ).setLogic( d.BCM_Wide & abortgap).setTriggerType(TT.minb)

        MenuItem('L1_BCM_AC_CALIB'     ).setLogic( d.BCM_AtoC & calibcond).setTriggerType(TT.minb)
        MenuItem('L1_BCM_CA_CALIB'     ).setLogic( d.BCM_CtoA & calibcond).setTriggerType(TT.minb)
        MenuItem('L1_BCM_Wide_CALIB'   ).setLogic( d.BCM_Wide & calibcond).setTriggerType(TT.minb)

        MenuItem('L1_BCM_AC_UNPAIREDB1'  ).setLogic( d.BCM_AtoC & bgrp13cond).setTriggerType(TT.minb)
        MenuItem('L1_BCM_CA_UNPAIREDB2'  ).setLogic( d.BCM_CtoA & bgrp14cond).setTriggerType(TT.minb)



        # RANDOM
        MenuItem('L1_RD0_FILLED'         ).setLogic( d.RNDM0 & physcond           ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_FILLED_OVERLAY' ).setLogic( d.RNDM0 & physcond           ).setTriggerType(TT.zerobs)
        MenuItem('L1_RD0_EMPTY'          ).setLogic( d.RNDM0 & cosmiccond         ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_UNPAIRED_ISO'   ).setLogic( d.RNDM0 & unpaired_isocond   ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_UNPAIRED_NONISO'   ).setLogic( d.RNDM0 & unpaired_nonisocond   ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_FIRSTEMPTY'     ).setLogic( d.RNDM0 & firstempty         ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_BGRP9'          ).setLogic( d.RNDM0 & bgrp9cond          ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_BGRP10'         ).setLogic( d.RNDM0 & alfacalib          ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_BGRP11'         ).setLogic( d.RNDM0 & bgrp11cond         ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_ABORTGAPNOTCALIB').setLogic( d.RNDM0 & abortgap          ).setTriggerType(TT.rand)



        MenuItem('L1_RD1_FILLED'         ).setLogic( d.RNDM1 & physcond           ).setTriggerType(TT.rand)
        MenuItem('L1_RD1_EMPTY'          ).setLogic( d.RNDM1 & cosmiccond         ).setTriggerType(TT.zerobs)
        MenuItem('L1_RD1_BGRP10'         ).setLogic( d.RNDM1 & alfacalib          ).setTriggerType(TT.zerobs)

        MenuItem('L1_RD2_FILLED'         ).setLogic( d.RNDM2 & physcond           ).setTriggerType(TT.rand)
        MenuItem('L1_RD2_EMPTY'          ).setLogic( d.RNDM2 & cosmiccond         ).setTriggerType(TT.rand)
        MenuItem('L1_RD2_BGRP11'         ).setLogic( d.RNDM2 & bgrp11cond         ).setTriggerType(TT.rand)
        MenuItem('L1_RD2_BGRP12'         ).setLogic( d.RNDM2 & bgrp12cond         ).setTriggerType(TT.rand)

        MenuItem('L1_RD3_FILLED'         ).setLogic( d.RNDM3 & physcond           ).setTriggerType(TT.rand)
        MenuItem('L1_RD3_EMPTY'          ).setLogic( d.RNDM3 & cosmiccond         ).setTriggerType(TT.rand)

        # BGRP
        MenuItem('L1_BGRP0').setLogic( d.RNDM0 & d.BGRP0 ).setTriggerType(TT.rand)
        MenuItem('L1_BGRP1').setLogic( d.RNDM0 & d.BGRP0 & d.BGRP1 ).setTriggerType(TT.rand)
        MenuItem('L1_BGRP7').setLogic( d.RNDM0 & d.BGRP0 & d.BGRP7 ).setTriggerType(TT.rand)
        MenuItem('L1_BGRP9').setLogic( d.RNDM0 & d.BGRP0 & d.BGRP9 ).setTriggerType(TT.rand)  #fixed in ATR-14201

        MenuItem('L1_BPTX0_BGRP0', ctpid=0xf1).setLogic(d.BPTX0 & d.BGRP0).setTriggerType(TT.rand)
        MenuItem('L1_BPTX1_BGRP0', ctpid=0xf2).setLogic(d.BPTX1 & d.BGRP0).setTriggerType(TT.rand)

        # lumi measurements
        MenuItem('L1_MLZ_A').setLogic( (d.MBTS_A|d.ZDC_A|d.LUCID_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MLZ_C').setLogic( (d.MBTS_C|d.ZDC_C|d.LUCID_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBLZ' ).setLogic( ( (d.MBTS_A|d.ZDC_A|d.LUCID_A) & (d.MBTS_C|d.ZDC_C|d.LUCID_C) | d.BCM_Wide) & physcond ).setTriggerType(TT.minb)

        MenuItem('L1_CALREQ0', ctpid=0x1fd).setLogic( d.CAL0 & calibcond).setTriggerType(TT.calreq0)
        MenuItem('L1_CALREQ1', ctpid=0x1fe).setLogic( d.CAL1 & calibcond).setTriggerType(TT.calreq1)
        MenuItem('L1_CALREQ2', ctpid=0x1ff).setLogic( d.CAL2 & calibcond).setTriggerType(TT.calreq2)

        # TRT
        MenuItem('L1_TRT'       , ctpid=0x4e).setLogic(d.NIMTRT & d.BGRP0).setTriggerType(TT.nim)
        MenuItem('L1_TRT_FILLED').setLogic(d.NIMTRT & physcond).setTriggerType(TT.nim)
        MenuItem('L1_TRT_EMPTY' ).setLogic(d.NIMTRT & cosmiccond).setTriggerType(TT.nim)

        # TGC
        MenuItem('L1_TGC_BURST').setLogic(d.NIMTGC & d.BGRP0).setTriggerType(TT.nim)
        # MenuItem('L1_TGC_BURST_EMPTY').setLogic(d.NIMTGC & cosmiccond).setTriggerType(TT.nim)


        # LHCF
        MenuItem('L1_LHCF').setLogic( d.NIMLHCF & physcond).setTriggerType(TT.nim)
        MenuItem('L1_LHCF_UNPAIRED_ISO').setLogic( d.NIMLHCF & unpaired_isocond).setTriggerType(TT.nim)
        MenuItem('L1_LHCF_EMPTY').setLogic( d.NIMLHCF & cosmiccond).setTriggerType(TT.nim)

        # ALFA

        # LUT 22 (12 Outputs)
        # ALFA LUT output #1-8 are the single thresholds
        ALFA_ANY_A    = d.ALFA_B7L1U | d.ALFA_B7L1L | d.ALFA_A7L1U | d.ALFA_A7L1L  #  9
        ALFA_ANY_C    = d.ALFA_A7R1U | d.ALFA_A7R1L | d.ALFA_B7R1U | d.ALFA_B7R1L  # 10
        ALFA_ANY_U    = d.ALFA_B7L1U | d.ALFA_A7L1U | d.ALFA_A7R1U | d.ALFA_B7R1U  # 11
        ALFA_ANY_L    = d.ALFA_A7R1L | d.ALFA_B7R1L | d.ALFA_B7L1L | d.ALFA_A7L1L  # 12

        # LUT 23 (12 Outputs)
        ALFA_LU       = d.ALFA2_A7L1U | d.ALFA2_B7L1U #  0
        ALFA_RL       = d.ALFA2_A7R1L | d.ALFA2_B7R1L #  1
        ALFA_LL       = d.ALFA2_B7L1L | d.ALFA2_A7L1L #  2
        ALFA_RU       = d.ALFA2_A7R1U | d.ALFA2_B7R1U #  3

        ALFA_A_UL_AA  = d.ALFA2_A7L1L | d.ALFA2_A7L1U #  4
        ALFA_A_UL_AB  = d.ALFA2_A7L1U | d.ALFA2_B7L1L #  5
        ALFA_A_UL_BA  = d.ALFA2_A7L1L | d.ALFA2_B7L1U #  6
        ALFA_A_UL_BB  = d.ALFA2_B7L1L | d.ALFA2_B7L1U #  7

        ALFA_C_UL_AA  = d.ALFA2_A7R1L | d.ALFA2_A7R1U #  8
        ALFA_C_UL_AB  = d.ALFA2_A7R1U | d.ALFA2_B7R1L #  9
        ALFA_C_UL_BA  = d.ALFA2_A7R1L | d.ALFA2_B7R1U # 10
        ALFA_C_UL_BB  = d.ALFA2_B7R1L | d.ALFA2_B7R1U # 11

        # LUT 24 (9 Outputs)
        ALFA_ANY          = d.ALFA3_B7L1U | d.ALFA3_B7L1L | d.ALFA3_A7L1U | d.ALFA3_A7L1L | d.ALFA3_A7R1U | d.ALFA3_A7R1L | d.ALFA3_B7R1U | d.ALFA3_B7R1L # 0
        ALFA_B1          = d.ALFA3_B7L1U | d.ALFA3_B7L1L | d.ALFA3_A7L1U | d.ALFA3_A7L1L  # 0
        ALFA_B2          = d.ALFA3_A7R1U | d.ALFA3_A7R1L | d.ALFA3_B7R1U | d.ALFA3_B7R1L # 0
        #NOT_ALFA_ANY_A    = Not(ALFA3_B7L1U | d.ALFA3_B7L1L | d.ALFA3_A7L1U | d.ALFA3_A7L1L)  #  1-4
        #NOT_ALFA_ANY_C    = Not(ALFA3_A7R1U | d.ALFA3_A7R1L | d.ALFA3_B7R1U | d.ALFA3_B7R1L)  #  5-8

        # LUT 25 (4 outputs)
        ALFA_AE1          = d.ALFA4_B7L1U | d.ALFA4_A7L1U | d.ALFA4_B7L1L | d.ALFA4_A7L1L
        ALFA_AE2          = d.ALFA4_B7L1U | d.ALFA4_A7L1U | d.ALFA4_A7R1L | d.ALFA4_B7R1L
        ALFA_AE3          = d.ALFA4_A7R1U | d.ALFA4_B7R1U | d.ALFA4_B7L1L | d.ALFA4_A7L1L
        ALFA_AE4          = d.ALFA4_A7R1U | d.ALFA4_B7R1U | d.ALFA4_A7R1L | d.ALFA4_B7R1L


        # further simplification (in CAM)
        ALFA_A            = ALFA_A_UL_AA & ALFA_A_UL_AB & ALFA_A_UL_BA & ALFA_A_UL_BB
        ALFA_C            = ALFA_C_UL_AA & ALFA_C_UL_AB & ALFA_C_UL_BA & ALFA_C_UL_BB
        ALFA_ELASTIC      = ALFA_ANY_A & ALFA_ANY_C & ALFA_ANY_U & ALFA_ANY_L
        ALFA_EINE         = ALFA_ANY_A & ALFA_ANY_C
        ALFA_ANTI_ELASTIC = ALFA_AE1 & ALFA_AE2 & ALFA_AE3 & ALFA_AE4


        MBTS_INNER = (d.MBTS_A0 | d.MBTS_A1 |  d.MBTS_A2 | d.MBTS_A3 | d.MBTS_A4 | d.MBTS_A5 | d.MBTS_A6 | d.MBTS_A7 | d.MBTS_C0 | d.MBTS_C1 | d.MBTS_C2 | d.MBTS_C3 | d.MBTS_C4 | d.MBTS_C5 | d.MBTS_C6 | d.MBTS_C7)

        #AFP
        AFP_A = (d.AFP_NSA & d.AFP_FSA_SIT)
        AFP_C = (d.AFP_NSC & d.AFP_FSC_SIT)
        AFP_A_AND_C_SPECTOF = (d.AFP_FSA_TOF & d.AFP_FSC_TOF)

        MenuItem('L1_AFP_A_BGRP0').setLogic( AFP_A & d.BGRP0)
        MenuItem('L1_AFP_A_SPECTOF').setLogic( d.AFP_FSA_TOF & physcond)
        MenuItem('L1_AFP_A_SPECTOF_BGRP0').setLogic( d.AFP_FSA_TOF & d.BGRP0)
        MenuItem('L1_AFP_C_BGRP0').setLogic( AFP_C & d.BGRP0)
        MenuItem('L1_AFP_C_SPECTOF').setLogic( d.AFP_FSC_TOF & physcond)
        MenuItem('L1_AFP_C_SPECTOF_BGRP0').setLogic( d.AFP_FSC_TOF & d.BGRP0)
        MenuItem('L1_AFP_NSA_BGRP0').setLogic( d.AFP_NSA & d.BGRP0)
        MenuItem('L1_AFP_NSC_BGRP0').setLogic( d.AFP_NSC & d.BGRP0)
        MenuItem('L1_AFP_FSA_SIT_BGRP0').setLogic( d.AFP_FSA_SIT & d.BGRP0)
        MenuItem('L1_AFP_FSC_SIT_BGRP0').setLogic( d.AFP_FSC_SIT & d.BGRP0)
        
        MenuItem('L1_AFP_A_OR_C_UNPAIRED_ISO').setLogic( (AFP_A | AFP_C)  & unpaired_isocond )
        MenuItem('L1_AFP_A_OR_C_UNPAIRED_NONISO').setLogic( (AFP_A | AFP_C)  & unpaired_nonisocond )
        
        MenuItem('L1_AFP_A_OR_C_EMPTY').setLogic( (AFP_A | AFP_C) & cosmiccond)
        MenuItem('L1_AFP_A_OR_C_FIRSTEMPTY').setLogic( (AFP_A | AFP_C) & firstempty)
        
        MenuItem('L1_AFP_A_AND_C_SPECTOF').setLogic( AFP_A_AND_C_SPECTOF & physcond )
        
        MenuItem('L1_AFP_A_AND_C').setLogic( AFP_A & AFP_C & physcond )
        MenuItem('L1_AFP_A_OR_C').setLogic( (AFP_A | AFP_C) & physcond )
        MenuItem('L1_AFP_A').setLogic( AFP_A & physcond )
        MenuItem('L1_AFP_C').setLogic( AFP_C & physcond )
        
        MenuItem('L1_AFP_A_AND_C_MBTS_2').setLogic( AFP_A & AFP_C & MBTS_2 & physcond )
        MenuItem('L1_AFP_A_OR_C_MBTS_2').setLogic( (AFP_A | AFP_C) & MBTS_2 & physcond )
        
        MenuItem('L1_AFP_A_AND_C_J12').setLogic( AFP_A & AFP_C & d.J12 & physcond )
        MenuItem('L1_AFP_A_OR_C_J12').setLogic( (AFP_A | AFP_C) & d.J12 & physcond )
        MenuItem('L1_EM3_AFP_A_AND_C').setLogic( AFP_A & AFP_C & d.EM3 & physcond )
        MenuItem('L1_EM3_AFP_A_OR_C').setLogic( (AFP_A | AFP_C) & d.EM3 & physcond )
        MenuItem('L1_MU4_AFP_A_AND_C').setLogic( AFP_A & AFP_C & d.MU4 & physcond )
        MenuItem('L1_MU4_AFP_A_OR_C').setLogic( (AFP_A | AFP_C) & d.MU4 & physcond )
        MenuItem('L1_AFP_A_AND_C_TE5').setLogic( AFP_A & AFP_C & d.TE5 & physcond )
        MenuItem('L1_AFP_A_OR_C_TE5').setLogic( (AFP_A | AFP_C) & d.TE5 & physcond )
        MenuItem('L1_AFP_A_AND_C_J50').setLogic( AFP_A & AFP_C & d.J50 & physcond )
        MenuItem('L1_AFP_A_AND_C_SPECTOF_J50').setLogic( AFP_A_AND_C_SPECTOF & d.J50 & physcond )
        MenuItem('L1_AFP_A_AND_C_J75').setLogic( AFP_A & AFP_C & d.J75 & physcond )
        MenuItem('L1_AFP_A_AND_C_SPECTOF_J75').setLogic( AFP_A_AND_C_SPECTOF & d.J75 & physcond )
        MenuItem('L1_AFP_A_AND_C_J100').setLogic( AFP_A & AFP_C & d.J100 & physcond )
        MenuItem('L1_AFP_A_AND_C_SPECTOF_J100').setLogic( AFP_A_AND_C_SPECTOF & d.J100 & physcond )
        
        MenuItem('L1_AFP_NSA').setLogic( d.AFP_NSA & physcond )
        MenuItem('L1_AFP_NSC').setLogic( d.AFP_NSC & physcond )
        MenuItem('L1_AFP_FSA_SIT').setLogic( d.AFP_FSA_SIT & physcond )
        MenuItem('L1_AFP_FSC_SIT').setLogic( d.AFP_FSC_SIT & physcond )
        MenuItem('L1_EM7_AFP_A_OR_C').setLogic( (AFP_A | AFP_C) & d.EM7 & physcond )
        MenuItem('L1_EM7_AFP_A_AND_C').setLogic( AFP_A & AFP_C & d.EM7 & physcond )
        MenuItem('L1_EM12_AFP_A_OR_C').setLogic( (AFP_A | AFP_C) & d.EM12 & physcond )
        MenuItem('L1_EM12_AFP_A_AND_C').setLogic( AFP_A & AFP_C & d.EM12 & physcond )

        ## ALFA Single items
        MenuItem('L1_ALFA_B7L1U').setLogic(d.ALFA_B7L1U & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_B7L1L').setLogic(d.ALFA_B7L1L & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_A7L1U').setLogic(d.ALFA_A7L1U & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_A7L1L').setLogic(d.ALFA_A7L1L & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_A7R1U').setLogic(d.ALFA_A7R1U & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_A7R1L').setLogic(d.ALFA_A7R1L & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_B7R1U').setLogic(d.ALFA_B7R1U & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_B7R1L').setLogic(d.ALFA_B7R1L & d.BGRP0).setTriggerType(TT.alfa)


        MenuItem('L1_ALFA_ELAST1').setLogic( d.ALFA_B7L1U & d.ALFA_A7L1U & d.ALFA_A7R1L & d.ALFA_B7R1L &
                                                 Not(d.ALFA3_B7L1L | d.ALFA3_A7L1L | d.ALFA3_A7R1U | d.ALFA3_B7R1U)
                                                 & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_ELAST2').setLogic( d.ALFA_B7L1L & d.ALFA_A7L1L & d.ALFA_A7R1U & d.ALFA_B7R1U &
                                                 Not(d.ALFA3_B7L1U | d.ALFA3_A7L1U | d.ALFA3_A7R1L | d.ALFA3_B7R1L)
                                                 & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_ELAST11').setLogic( d.ALFA_B7L1U & d.ALFA_A7L1U & d.ALFA_A7R1L & d.ALFA_B7R1L    & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ELAST12').setLogic( d.ALFA_B7L1L & d.ALFA_A7L1L & d.ALFA_A7R1U & d.ALFA_B7R1U    & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ELAST13').setLogic( ALFA_LU & (d.ALFA_A7R1L & d.ALFA_B7R1L)                  & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ELAST14').setLogic( (d.ALFA_B7L1U & d.ALFA_A7L1U) & ALFA_RL & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ELAST15').setLogic( ALFA_LU & ALFA_RL & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ELAST15_Calib').setLogic( ALFA_LU & ALFA_RL &  alfacalib).setTriggerType(TT.alfa) ## CHECK
        MenuItem('L1_ALFA_ELAST16').setLogic( ALFA_LL & (d.ALFA_A7R1U & d.ALFA_B7R1U) & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ELAST17').setLogic((d.ALFA_B7L1L & d.ALFA_A7L1L) & ALFA_RU & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ELAST18').setLogic( ALFA_LL & ALFA_RU & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ELAST18_Calib').setLogic( ALFA_LL & ALFA_RU & alfacalib).setTriggerType(TT.alfa)


        MenuItem('L1_ALFA_SDIFF1').setLogic( d.ALFA_B7L1U & d.ALFA_A7L1U &
                                                 Not(d.ALFA3_B7L1L | d.ALFA3_A7L1L | d.ALFA3_A7R1U | d.ALFA3_A7R1L | d.ALFA3_B7R1U | d.ALFA3_B7R1L)
                                                 & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_SDIFF2').setLogic( d.ALFA_A7R1L & d.ALFA_B7R1L &
                                                 Not(d.ALFA3_B7L1U | d.ALFA3_B7L1L | d.ALFA3_A7L1U | d.ALFA3_A7L1L | d.ALFA3_A7R1U | d.ALFA3_B7R1U)
                                                 & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_SDIFF3').setLogic( d.ALFA_B7L1L & d.ALFA_A7L1L &
                                                 Not(d.ALFA3_B7L1U | d.ALFA3_A7L1U | d.ALFA3_A7R1U | d.ALFA3_A7R1L | d.ALFA3_B7R1U | d.ALFA3_B7R1L)
                                                 & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_SDIFF4').setLogic( d.ALFA_A7R1U & d.ALFA_B7R1U &
                                                 Not(d.ALFA3_B7L1U | d.ALFA3_B7L1L | d.ALFA3_A7L1U | d.ALFA3_A7L1L | d.ALFA3_A7R1L | d.ALFA3_B7R1L)
                                                 & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_SDIFF5').setLogic( d.ALFA_B7L1U & d.ALFA_A7L1U &  physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_SDIFF6').setLogic( d.ALFA_A7R1L & d.ALFA_B7R1L &  physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_SDIFF7').setLogic( d.ALFA_B7L1L & d.ALFA_A7L1L &  physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_SDIFF8').setLogic( d.ALFA_A7R1U & d.ALFA_B7R1U &  physcond).setTriggerType(TT.alfa)

        MenuItem('L1_MBTS_1_A_ALFA_C').setLogic( d.MBTS_A & ALFA_C  & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_MBTS_1_C_ALFA_A').setLogic( d.MBTS_C & ALFA_A & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO').setLogic( d.MBTS_A & ALFA_C & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO').setLogic( d.MBTS_C & ALFA_A & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_MBTS_1_ALFA_ANY').setLogic( MBTS_1 & ALFA_ANY  & physcond).setTriggerType(TT.alfa) ##should be called L1_MBTS_1_ALFA_ANY

        ## check definition of MBTS_2
        MenuItem('L1_MBTS_2_A_ALFA_C').setLogic( d.MBTS_A.x(2) & ALFA_C & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_MBTS_2_C_ALFA_A').setLogic( d.MBTS_C.x(2) & ALFA_A & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO').setLogic( d.MBTS_A.x(2) & ALFA_C & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO').setLogic( d.MBTS_C.x(2) & ALFA_A & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_MBTS_2_ALFA').setLogic( MBTS_2 & ALFA_ANY & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_LUCID_A_ALFA_C').setLogic( d.LUCID_A & ALFA_C & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_LUCID_C_ALFA_A').setLogic( d.LUCID_C & ALFA_A & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_LUCID_A_ALFA_C_UNPAIRED_ISO').setLogic( d.LUCID_A & ALFA_C & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_LUCID_C_ALFA_A_UNPAIRED_ISO').setLogic( d.LUCID_C & ALFA_A & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_LUCID_ALFA').setLogic( (d.LUCID_A | d.LUCID_C) & ALFA_ANY & physcond).setTriggerType(TT.alfa)

        #MenuItem('L1_ZDC_A_ALFA_C').setLogic( d.ZDC_A  & ALFA_C & physcond).setTriggerType(TT.alfa)
        #MenuItem('L1_ZDC_C_ALFA_A').setLogic( d.ZDC_C  & ALFA_A & physcond).setTriggerType(TT.alfa)
        #MenuItem('L1_ZDC_ALFA').setLogic( (d.ZDC_A | ZDC_C) & ALFA_ANY & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_EM3_ALFA_ANY'             ).setLogic( d.EM3 & ALFA_ANY & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_EM3_ALFA_ANY_UNPAIRED_ISO').setLogic( d.EM3 & ALFA_ANY & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_EM3_ALFA_ANY_VETO_MBTS' ).setLogic( d.EM3 & ALFA_ANY  & Not(MBTS_INNER) & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_EM3_ALFA_EINE'          ).setLogic( d.EM3 & ALFA_EINE & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_EM3_ALFA_EINE_VETO_MBTS').setLogic( d.EM3 & ALFA_EINE & Not(MBTS_INNER) & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_EM3_ALFA_MBTS_A'             ).setLogic( d.EM3 & d.MBTS_A & ALFA_ANY_A & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_EM3_ALFA_MBTS_C'             ).setLogic( d.EM3 & d.MBTS_C & ALFA_ANY_C & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_EM3_ALFA_MBTS_A_UNPAIRED_ISO').setLogic( d.EM3 & d.MBTS_A & ALFA_ANY_A & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_EM3_ALFA_MBTS_C_UNPAIRED_ISO').setLogic( d.EM3 & d.MBTS_A & ALFA_ANY_C & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_2EM3_ALFA_EINE'          ).setLogic( d.EM3.x(2) & ALFA_EINE & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_J12_ALFA_ANY'             ).setLogic( d.J12 & ALFA_ANY & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_J12_ALFA_ANY_UNPAIRED_ISO').setLogic( d.J12 & ALFA_ANY & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_J12_ALFA_EINE'            ).setLogic( d.J12 & ALFA_EINE & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_MU4_ALFA_ANY'             ).setLogic( d.MU4 & ALFA_ANY  & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_MU4_ALFA_ANY_UNPAIRED_ISO').setLogic( d.MU4 & ALFA_ANY  & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_MU4_ALFA_ANY_PAIRED_UNPAIRED_ISO').setLogic( d.MU4 & ALFA_ANY  & physcond_or_unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_MU4_ALFA_EINE'            ).setLogic( d.MU4 & ALFA_EINE & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_TE5_ALFA_ANY'             ).setLogic( d.TE5 & ALFA_ANY  & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_TE5_ALFA_ANY_UNPAIRED_ISO').setLogic( d.TE5 & ALFA_ANY  & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_TE5_ALFA_EINE'            ).setLogic( d.TE5 & ALFA_EINE & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_TRT_ALFA_ANY'             ).setLogic( d.NIMTRT & ALFA_ANY & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_TRT_ALFA_ANY_UNPAIRED_ISO').setLogic( d.NIMTRT & ALFA_ANY & unpaired_isocond).setTriggerType(TT.alfa)

        MenuItem('L1_TRT_ALFA_ANY_VETO_MBTS'           ).setLogic( d.NIMTRT & Not(MBTS_INNER) & ALFA_ANY & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_TRT_ALFA_EINE'                    ).setLogic( d.NIMTRT & ALFA_EINE & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_TRT_ALFA_EINE_VETO_MBTS'          ).setLogic( d.NIMTRT & Not(MBTS_INNER) & ALFA_EINE & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_ELASTIC_VETO_MBTS'                  ).setLogic( Not(MBTS_INNER) & ALFA_ELASTIC & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ELASTIC_UNPAIRED_ISO'               ).setLogic( ALFA_ELASTIC & unpaired_isocond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_ANTI_ELASTIC_VETO_MBTS'             ).setLogic( Not(MBTS_INNER) & ALFA_ANTI_ELASTIC & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ANTI_ELASTIC_UNPAIRED_ISO').setLogic( ALFA_ANTI_ELASTIC & unpaired_isocond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_ANY_VETO_MBTS'                      ).setLogic( Not(MBTS_INNER) & ALFA_ANY & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ANY_VETO_MBTS_UNPAIRED_ISO'         ).setLogic( Not(MBTS_INNER) & ALFA_ANY & unpaired_isocond).setTriggerType(TT.alfa)

        MenuItem('L1_LHCF_ALFA_ANY_A'             ).setLogic( d.NIMLHCF & ALFA_ANY_A & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_LHCF_ALFA_ANY_C'             ).setLogic( d.NIMLHCF & ALFA_ANY_C & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_LHCF_ALFA_ANY_A_UNPAIRED_ISO').setLogic( d.NIMLHCF & ALFA_ANY_A & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_LHCF_ALFA_ANY_C_UNPAIRED_ISO').setLogic( d.NIMLHCF & ALFA_ANY_C & unpaired_isocond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_BGT' ).setLogic(d.RNDM3 & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_BGT_UNPAIRED_ISO' ).setLogic(d.RNDM3 & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_BGT_BGRP1').setLogic(d.RNDM3 & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_BGT_BGRP4').setLogic(d.RNDM3 & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_BGT_BGRP10').setLogic(d.RNDM3 & alfacalib).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_SHOWSYST5').setLogic( (ALFA_ANY_A & ALFA_ANY_C) & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_SYST9' ).setLogic( d.ALFA_B7L1U & d.ALFA_A7L1U & d.ALFA_A7R1U & d.ALFA_B7R1U & Not(d.ALFA3_B7L1L | d.ALFA3_A7L1L | d.ALFA3_A7R1L | d.ALFA3_B7R1L) & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_SYST10').setLogic( d.ALFA_B7L1L & d.ALFA_A7L1L & d.ALFA_A7R1L & d.ALFA_B7R1L & Not(d.ALFA3_B7L1U | d.ALFA3_A7L1U | d.ALFA3_A7R1U | d.ALFA3_B7R1U) & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_SYST11').setLogic( d.ALFA_B7L1U & d.ALFA_A7L1U & d.ALFA_A7R1U & d.ALFA_B7R1U & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_SYST12').setLogic( d.ALFA_B7L1L & d.ALFA_A7L1L & d.ALFA_A7R1L & d.ALFA_B7R1L & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_SYST17').setLogic( ALFA_LU & ALFA_RU & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_SYST18').setLogic( ALFA_LL & ALFA_RL & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_ANY').setLogic(ALFA_ANY & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_B1_EMPTY').setLogic(ALFA_B1 & cosmiccond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_B2_EMPTY').setLogic(ALFA_B2 & cosmiccond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ANY_EMPTY').setLogic(ALFA_ANY & cosmiccond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ANY_FIRSTEMPTY').setLogic(ALFA_ANY & firstempty).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ANY_UNPAIRED_ISO').setLogic(ALFA_ANY & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ANY_UNPAIRED_NONISO').setLogic(ALFA_ANY & unpaired_nonisocond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ANY_BGRP10').setLogic(ALFA_ANY & alfacalib).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ANY_ABORTGAPNOTCALIB').setLogic( ALFA_ANY & abortgap).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ANY_CALIB').setLogic( ALFA_ANY & calibcond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_ANY_A_EMPTY').setLogic(ALFA_ANY_A & cosmiccond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ANY_C_EMPTY').setLogic(ALFA_ANY_C & cosmiccond).setTriggerType(TT.alfa)

        ## ALFA _OD items (LUT 26, 12 Outputs)
        MenuItem('L1_ALFA_B7L1U_OD').setLogic(d.ALFA_B7L1U_OD & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_B7L1L_OD').setLogic(d.ALFA_B7L1L_OD & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_A7L1U_OD').setLogic(d.ALFA_A7L1U_OD & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_A7L1L_OD').setLogic(d.ALFA_A7L1L_OD & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_A7R1U_OD').setLogic(d.ALFA_A7R1U_OD & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_A7R1L_OD').setLogic(d.ALFA_A7R1L_OD & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_B7R1U_OD').setLogic(d.ALFA_B7R1U_OD & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_B7R1L_OD').setLogic(d.ALFA_B7R1L_OD & d.BGRP0).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_B7L1_OD').setLogic( (d.ALFA_B7L1U_OD & d.ALFA_B7L1L_OD) & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_A7L1_OD').setLogic( (d.ALFA_A7L1U_OD & d.ALFA_A7L1L_OD) & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_B7R1_OD').setLogic( (d.ALFA_B7R1U_OD & d.ALFA_B7R1L_OD) & d.BGRP0).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_A7R1_OD').setLogic( (d.ALFA_A7R1U_OD & d.ALFA_A7R1L_OD) & d.BGRP0).setTriggerType(TT.alfa)


        #ATR-19355
        MenuItem('L1_BPH-0M10-3MU4' ).setLogic( d.TOPO_0INVM10_3MU4ab & physcond)

        #ATR-18815
        MenuItem('L1_LFV-EM8I-MU11' ).setLogic( d.TOPO_0INVM10_0DR15_EM8abi_MU10ab & d.MU11 & physcond)
        MenuItem('L1_LFV-EM12I-MU6' ).setLogic( d.TOPO_0INVM10_0DR15_EM12abi_MU6ab & physcond)

        #ATR-18824
        MenuItem('L1_ZAFB-04DPHI-EM15I' ).setLogic( d.TOPO_60INVM9999_04DPHI32_EM15abhi_FJj15s623ETA49 & physcond)
        MenuItem('L1_ZAFB-25DPHI-EM15I' ).setLogic( d.TOPO_60INVM9999_25DPHI32_EM15abhi_FJj15s623ETA49 & physcond)
        MenuItem('L1_ZAFB-25DPHI-EM18I' ).setLogic( d.TOPO_60INVM9999_25DPHI32_EM15abhi_FJj15s623ETA49 & d.EM18VHI & physcond)

        #ATR-19302:
        MenuItem('L1_DPHI-M70-2EM10I' ).setLogic( d.TOPO_0INVM70_27DPHI32_EM10his1_EM10his6 & physcond)
        MenuItem('L1_DPHI-M70-2EM12I' ).setLogic( d.TOPO_0INVM70_27DPHI32_EM12his1_EM12his6 & physcond)

        #ATR-19510
        MenuItem('L1_DY-BOX-2MU4' ).setLogic( d.TOPO_5DETA99_5DPHI99_2MU4ab & d.MU4.x(2) & physcond)

        try:
            ### NOT WORKING due to accepted number of bits in TOPO1
            MenuItem("L1_2MU4-BO"     ).setLogic( d.R2TOPO_MULT_CMU4ab.x(2) & physcond)
            MenuItem("L1_2MU6-BO"     ).setLogic( d.R2TOPO_MULT_CMU6ab.x(2) & d.MU6.x(2) & physcond)
            MenuItem("L1_MU6MU4-BO").setLogic( d.R2TOPO_MULT_CMU4ab.x(2) & d.R2TOPO_MULT_CMU6ab & d.MU6 & physcond)


            # B-jet
            MenuItem('L1_BTAG-MU4J15').setLogic( d.R2TOPO_0DR04_MU4ab_CJ15ab & physcond)
            MenuItem('L1_BTAG-MU4J30').setLogic( d.R2TOPO_0DR04_MU4ab_CJ30ab & physcond)
            MenuItem('L1_BTAG-MU6J20').setLogic( d.R2TOPO_0DR04_MU6ab_CJ20ab & d.MU6 & physcond)
            MenuItem('L1_BTAG-MU6J25').setLogic( d.R2TOPO_0DR04_MU6ab_CJ25ab & d.MU6 & physcond)
            MenuItem('L1_BTAG-MU4J20').setLogic( d.R2TOPO_0DR04_MU4ab_CJ20ab & physcond)
            MenuItem('L1_3J15_BTAG-MU4J15').setLogic( d.J15.x(3) & d.R2TOPO_0DR04_MU4ab_CJ15ab & physcond)
            MenuItem('L1_3J15_BTAG-MU4J30').setLogic( d.J15.x(3) & d.R2TOPO_0DR04_MU4ab_CJ30ab & physcond)
            MenuItem('L1_3J15_BTAG-MU6J25').setLogic( d.J15.x(3) & d.R2TOPO_0DR04_MU6ab_CJ25ab & d.MU6 & physcond)
            MenuItem('L1_3J20_BTAG-MU4J20').setLogic( d.J20.x(3) & d.R2TOPO_0DR04_MU4ab_CJ20ab & physcond)

            # ZH
            MenuItem('L1_DPHI-AJ20s2XE50'  ).setLogic( d.R2TOPO_10MINDPHI_AJ20s2_XE50    & physcond)

            MenuItem('L1_J40_DPHI-J20s2XE50').setLogic( d.J40 & d.TOPO_10MINDPHI_J20s2_XE50  & physcond)
            MenuItem('L1_J40_DPHI-J20s2XE30').setLogic( d.J40 & d.TOPO_10MINDPHI_J20s2_XE30  & physcond)
            MenuItem('L1_MU4_J20_XE30_DPHI-J20s2XE30').setLogic( d.MU4 & d.J20 & d.XE30 & d.TOPO_10MINDPHI_J20s2_XE30  & physcond)
            MenuItem('L1_2MU4_J20_XE30_DPHI-J20s2XE30').setLogic( d.MU4.x(2) & d.J20 & d.XE30 & d.TOPO_10MINDPHI_J20s2_XE30  & physcond)
            MenuItem('L1_MU4_2EM3_J20_XE30_DPHI-J20s2XE30').setLogic( d.MU4 & d.EM3.x(2) & d.J20 & d.XE30 & d.TOPO_10MINDPHI_J20s2_XE30  & physcond)
            MenuItem('L1_3EM3_J40_XE50_DPHI-J20s2XE50').setLogic( d.EM3.x(3) & d.J40 & d.XE50 & d.TOPO_10MINDPHI_J20s2_XE50  & physcond)


            MenuItem('L1_J40_XE50_DPHI-J20s2XE50').setLogic( d.J40 & d.XE50 & d.TOPO_10MINDPHI_J20s2_XE50  & physcond)
            MenuItem('L1_J40_DPHI-J20XE50'  ).setLogic( d.J40 & d.R2TOPO_10MINDPHI_J20ab_XE50  & physcond)
            MenuItem('L1_J40_DPHI-CJ20XE50' ).setLogic( d.J40 & d.R2TOPO_10MINDPHI_CJ20ab_XE50 & physcond)

            #ATR-15243 Higgsino
            MenuItem('L1_2MU4_J40_XE50'  ).setLogic( d.MU4.x(2) & d.J40 & d.XE50  & physcond).setTriggerType(TT.calo)
            MenuItem('L1_2MU4_J20_XE40_DPHI-J20s2XE30').setLogic( d.MU4.x(2) & d.J20 & d.XE40 & d.TOPO_10MINDPHI_J20s2_XE30  & physcond)
            MenuItem('L1_2MU4_J20_XE50_DPHI-J20s2XE30').setLogic( d.MU4.x(2) & d.J20 & d.XE50 & d.TOPO_10MINDPHI_J20s2_XE30  & physcond)

            MenuItem('L1_MU4_J30_XE40_DPHI-J20s2XE30').setLogic( d.MU4 & d.J30 & d.XE40 & d.TOPO_10MINDPHI_J20s2_XE30  & physcond)
            MenuItem('L1_MU4_J50_XE50_DPHI-J20s2XE30').setLogic( d.MU4 & d.J50 & d.XE50 & d.TOPO_10MINDPHI_J20s2_XE30  & physcond)
            MenuItem('L1_MU4_XE60'  ).setLogic( d.MU4 & d.XE60  & physcond).setTriggerType(TT.calo)
            MenuItem('L1_2MU4_XE60'  ).setLogic( d.MU4.x(2) & d.XE60  & physcond).setTriggerType(TT.calo)

            # HT
            #MenuItem('L1_HT0-J0.ETA49'     ).setLogic( d.TOPO_HT0_AJ0allETA49    & physcond)
            MenuItem('L1_HT190-J15.ETA21'  ).setLogic( d.R2TOPO_HT190_AJ15allETA21 & physcond)
            MenuItem('L1_HT190-J15s5.ETA21').setLogic( d.TOPO_HT190_J15s5ETA21   & physcond)
            MenuItem('L1_HT150-J20.ETA31'  ).setLogic( d.R2TOPO_HT150_AJ20allETA31 & physcond)
            MenuItem('L1_HT150-J20s5.ETA31').setLogic( d.TOPO_HT150_J20s5ETA31   & physcond)


            MenuItem('L1_HT150-J20s5.ETA31_MJJ-400').setLogic( d.R2TOPO_HT150_J20s5ETA31 & d.R2TOPO_400INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_HT150-J20s5.ETA31_MJJ-400-CF').setLogic( d.TOPO_HT150_J20s5ETA31 & d.TOPO_400INVM9999_AJ30s6ETA31_AJ20s631ETA49 & physcond)
            # Jpsi T&P
            MenuItem("L1_JPSI-1M5"     ).setLogic( d.R2TOPO_1INVM5_EMs1_EMs6   & physcond)
            MenuItem("L1_JPSI-1M5-EM7" ).setLogic( d.TOPO_1INVM5_EM7s1_EMs6  & physcond)
            MenuItem("L1_JPSI-1M5-EM12").setLogic( d.TOPO_1INVM5_EM12s1_EMs6 & physcond)

            #MenuItem("L1_W-05DPHI-JXE-0"   ).setLogic( d.TOPO_05MINDPHI_AJj10s6_XE0 & physcond)
            #MenuItem("L1_W-10DPHI-JXE-0"   ).setLogic( d.TOPO_10MINDPHI_AJj10s6_XE0 & physcond)
            #MenuItem("L1_W-15DPHI-JXE-0"   ).setLogic( d.TOPO_15MINDPHI_AJj10s6_XE0 & physcond)
            #MenuItem("L1_W-05DPHI-EMXE-1"  ).setLogic( d.TOPO_05MINDPHI_EM12s6_XE0  & physcond)
            #MenuItem("L1_W-05DPHI-EM15XE-1").setLogic( d.TOPO_05MINDPHI_EM15s6_XE0  & physcond)
            #MenuItem("L1_W-15DPHI-EM15XE-1").setLogic( d.TOPO_15MINDPHI_EM15s6_XE0  & physcond)
            #MenuItem("L1_W-15DPHI-EMXE-1"  ).setLogic( d.TOPO_15MINDPHI_EM12s6_XE0  & physcond)

            MenuItem("L1_W-05RO-XEHT-0"    ).setLogic( d.R2TOPO_05RATIO_XE0_HT0_AJj15allETA49   & physcond)
            MenuItem("L1_W-90RO2-XEHT-0"   ).setLogic( d.R2TOPO_90RATIO2_XE0_HT0_AJj15allETA49  & physcond)
            MenuItem("L1_W-250RO2-XEHT-0"  ).setLogic( d.R2TOPO_250RATIO2_XE0_HT0_AJj15allETA49 & physcond)
            MenuItem("L1_W-HT20-JJ15.ETA49").setLogic( d.R2TOPO_HT20_AJj15allETA49 & physcond)

            #MenuItem("L1_W-NOMATCH").setLogic( d.TOPO_NOT_02MATCH_EM10s1_AJj15allETA49 & physcond)

            #MenuItem("L1_EM12_W-MT25").setLogic( d.TOPO_25MT_EM12s6_XE0 & d.EM12 & physcond)
            #MenuItem("L1_EM15_W-MT35").setLogic( d.TOPO_35MT_EM15s6_XE0 & d.EM15 & physcond)
            #MenuItem("L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE" ).setLogic( d.TOPO_35MT_EM15s6_XE0 & d.EM15 & d.TOPO_250RATIO2_XE0_HT0_AJj15allETA49 & d.TOPO_05MINDPHI_AJj10s6_XE0 & d.TOPO_05MINDPHI_EM15s6_XE0 & physcond)
            #MenuItem("L1_EM15_W-MT35_XS60_W-05DPHI-JXE-0_W-05DPHI-EM15XE"            ).setLogic( d.TOPO_35MT_EM15s6_XE0 & d.EM15 & d.XS60 & d.TOPO_05MINDPHI_AJj10s6_XE0 & d.TOPO_05MINDPHI_EM15s6_XE0 & physcond)
            #MenuItem("L1_EM15_W-MT35_XS40_W-05DPHI-JXE-0_W-05DPHI-EM15XE"            ).setLogic( d.TOPO_35MT_EM15s6_XE0 & d.EM15 & d.XS40 & d.TOPO_05MINDPHI_AJj10s6_XE0 & d.TOPO_05MINDPHI_EM15s6_XE0 & physcond)
            #MenuItem("L1_EM15_W-MT35_XS60").setLogic( d.TOPO_35MT_EM15s6_XE0 & d.EM15 & d.XS60 & physcond)
            #MenuItem("L1_EM15VH_W-MT35_XS60").setLogic( d.TOPO_35MT_EM15s6_XE0 & d.EM15VH & d.XS60 & physcond)
            #MenuItem("L1_EM20VH_W-MT35_XS60").setLogic( d.TOPO_35MT_EM15s6_XE0 & d.EM20VH & d.XS60 & physcond)
            #MenuItem("L1_EM22VHI_W-MT35_XS40").setLogic( d.TOPO_35MT_EM15s6_XE0 & d.EM22VHI & d.XS40 & physcond)
            #MenuItem("L1_EM12_W-MT35_XS40_W-05DPHI-JXE-0_W-05DPHI-EMXE"            ).setLogic( d.TOPO_35MT_EM12s6_XE0 & d.EM12 & d.XS40 & d.TOPO_05MINDPHI_AJj10s6_XE0 & d.TOPO_05MINDPHI_EM12s6_XE0 & physcond)
            #MenuItem("L1_EM12_W-MT35").setLogic( d.TOPO_35MT_EM12s6_XE0 & d.EM12 & physcond)
            #MenuItem("L1_EM12_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EMXE" ).setLogic( d.TOPO_35MT_EM12s6_XE0 & d.EM12 & d.TOPO_250RATIO2_XE0_HT0_AJj15allETA49 & d.TOPO_05MINDPHI_AJj10s6_XE0 & d.TOPO_05MINDPHI_EM12s6_XE0 & physcond)
            #MenuItem("L1_EM12_W-MT35_XS50").setLogic( d.TOPO_35MT_EM12s6_XE0 & d.EM12 & d.XS50 & physcond)
            #MenuItem("L1_EM10VH_W-MT35_XS50").setLogic( d.TOPO_35MT_EM12s6_XE0 & d.EM10VH & d.XS50 & physcond)
            #MenuItem("L1_W-15DPHI-EM15XE-0"  ).setLogic( d.TOPO_15MINDPHI_EM15s6_XE0  & physcond)
            #MenuItem("L1_EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE" ).setLogic( d.TOPO_35MT_EM15s6_XE0 & d.EM15 & d.TOPO_250RATIO2_XE0_HT0_AJj15allETA49 & d.TOPO_15MINDPHI_AJj10s6_XE0 & d.TOPO_15MINDPHI_EM15s6_XE0 & physcond)
            #MenuItem("L1_EM15_W-MT35_XS60_W-15DPHI-JXE-0_W-15DPHI-EM15XE"            ).setLogic( d.TOPO_35MT_EM15s6_XE0 & d.EM15 & d.XS60 & d.TOPO_15MINDPHI_AJj10s6_XE0 & d.TOPO_15MINDPHI_EM15s6_XE0 & physcond)
            #MenuItem("L1_EM15_W-MT35_XS40_W-15DPHI-JXE-0_W-15DPHI-EM15XE"            ).setLogic( d.TOPO_35MT_EM15s6_XE0 & d.EM15 & d.XS40 & d.TOPO_15MINDPHI_AJj10s6_XE0 & d.TOPO_15MINDPHI_EM15s6_XE0 & physcond)
            #MenuItem("L1_W-15DPHI-EMXE-0"  ).setLogic( d.TOPO_15MINDPHI_EM12s6_XE0  & physcond)
            #MenuItem("L1_EM12_W-MT35_XS30_W-15DPHI-JXE-0_W-15DPHI-EMXE"            ).setLogic( d.TOPO_35MT_EM12s6_XE0 & d.EM12 & d.XS30 & d.TOPO_15MINDPHI_AJj10s6_XE0 & d.TOPO_15MINDPHI_EM12s6_XE0 & physcond)
            #MenuItem("L1_EM12_W-MT35_W-90RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EMXE"  ).setLogic( d.TOPO_35MT_EM12s6_XE0 & d.EM12 & d.TOPO_90RATIO2_XE0_HT0_AJj15allETA49 & d.TOPO_15MINDPHI_AJj10s6_XE0 & d.TOPO_15MINDPHI_EM12s6_XE0 & physcond)
            ## Old
            #MenuItem("L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0"   ).setLogic( d.TOPO_25MT_EM12s6_XE0 & d.EM12 & d.TOPO_15MINDPHI_AJj10s6_XE0 & d.TOPO_15MINDPHI_EM12s6_XE0 & d.TOPO_90RATIO2_XE0_HT0_AJj15allETA49 & physcond)
            #MenuItem("L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS30"           ).setLogic( d.TOPO_25MT_EM12s6_XE0 & d.EM12 & d.TOPO_15MINDPHI_AJj10s6_XE0 & d.TOPO_15MINDPHI_EM12s6_XE0 & d.XS30 & physcond)
            #MenuItem("L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20"           ).setLogic( d.TOPO_25MT_EM12s6_XE0 & d.EM12 & d.TOPO_15MINDPHI_AJj10s6_XE0 & d.TOPO_15MINDPHI_EM12s6_XE0 & d.XS20 & physcond)
            #MenuItem("L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE"                ).setLogic( d.TOPO_25MT_EM12s6_XE0 & d.EM12 & d.TOPO_15MINDPHI_AJj10s6_XE0 & d.TOPO_15MINDPHI_EM12s6_XE0 & physcond)
            #MenuItem("L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30"         ).setLogic( d.TOPO_35MT_EM15s6_XE0 & d.EM15 & d.TOPO_05MINDPHI_AJj10s6_XE0 & d.TOPO_05MINDPHI_EM15s6_XE0 & d.XS30 & physcond)
            #MenuItem("L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE"              ).setLogic( d.TOPO_35MT_EM15s6_XE0 & d.EM15 & d.TOPO_05MINDPHI_AJj10s6_XE0 & d.TOPO_05MINDPHI_EM15s6_XE0 & physcond)

            ##Combined L1Top
            MenuItem('L1_DR-MU10TAU12I_TAU12I-J25').setLogic( d.TOPO_0DR28_MU10ab_TAU12abi &  d.TOPO_1DISAMB_TAU12abi_J25ab & d.MU10 & physcond)
            MenuItem('L1_MU10_TAU12I-J25').setLogic( d.MU10 &  d.TOPO_1DISAMB_TAU12abi_J25ab & physcond)
            MenuItem('L1_J25_2J20_3J12_DR-TAU20ITAU12I').setLogic( d.J25 & d.J20.x(2) & d.J12.x(3) &  d.TOPO_0DR28_TAU20abi_TAU12abi & physcond)
            MenuItem('L1_J25_2J12_DR-MU10TAU12I').setLogic( d.J25 & d.J12.x(2) &  d.TOPO_0DR28_MU10ab_TAU12abi & d.MU10 & physcond)
            MenuItem('L1_J25_2J20_3J12_BOX-TAU20ITAU12I').setLogic( d.J25 & d.J20.x(2) & d.J12.x(3) & d.R2TOPO_0DETA20_0DPHI20_TAU20abi_TAU12abi & physcond)

            MenuItem('L1_MU6_MJJ-200').setLogic( d.MU6 & d.R2TOPO_200INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_MU6_MJJ-300').setLogic( d.MU6 & d.TOPO_300INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_MU6_MJJ-400').setLogic( d.MU6 & d.R2TOPO_400INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_MU6_MJJ-500').setLogic( d.MU6 & d.R2TOPO_500INVM9999_AJ30s6_AJ20s6 & physcond)

            MenuItem('L1_J30_2J20_4J20.0ETA49_MJJ-400').setLogic( d.J30 & d.J20.x(2) & d.J200ETA49.x(4) & d.R2TOPO_400INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_J30_2J20_4J20.0ETA49_MJJ-700').setLogic( d.J30 & d.J20.x(2) & d.J200ETA49.x(4) & d.TOPO_700INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_J30_2J20_4J20.0ETA49_MJJ-800').setLogic( d.J30 & d.J20.x(2) & d.J200ETA49.x(4) & d.R2TOPO_800INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_J30_2J20_4J20.0ETA49_MJJ-900').setLogic( d.J30 & d.J20.x(2) & d.J200ETA49.x(4) & d.R2TOPO_900INVM9999_AJ30s6_AJ20s6 & physcond)

            MenuItem('L1_3J20_4J20.0ETA49_MJJ-400').setLogic( d.J20.x(3) & d.J200ETA49.x(4) & d.R2TOPO_400INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_3J20_4J20.0ETA49_MJJ-700').setLogic( d.J20.x(3) & d.J200ETA49.x(4) & d.R2TOPO_700INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_3J20_4J20.0ETA49_MJJ-800').setLogic( d.J20.x(3) & d.J200ETA49.x(4) & d.R2TOPO_800INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_3J20_4J20.0ETA49_MJJ-900').setLogic( d.J20.x(3) & d.J200ETA49.x(4) & d.R2TOPO_900INVM9999_AJ30s6_AJ20s6 & physcond)

            MenuItem('L1_XE35_MJJ-200').setLogic( d.XE35 & d.R2TOPO_200INVM9999_AJ30s6_AJ20s6 & physcond)

            ### ATR-15062
            MenuItem('L1_EM18VHI_MJJ-300').setLogic( d.EM18VHI & d.TOPO_300INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_EM18VH_MJJ-300').setLogic( d.EM18VH & d.TOPO_300INVM9999_AJ30s6_AJ20s6 & physcond)

            # CTP items for L1Topo streamers
            MenuItem('L1_DY-DR-2MU4').setLogic( d.R2TOPO_2DR99_2MU4ab & physcond)
            MenuItem('L1_DY-BOX-MU6MU4').setLogic( d.TOPO_5DETA99_5DPHI99_MU6ab_MU4ab & d.MU6 & physcond)
            MenuItem('L1_DY-BOX-2MU6').setLogic( d.TOPO_5DETA99_5DPHI99_2MU6ab & d.MU6.x(2) & physcond)

            MenuItem('L1_LFV-MU11').setLogic( d.TOPO_0DR15_2MU6ab & d.MU11 & d.MU6.x(2) & physcond)
            MenuItem('L1_LFV-MU6').setLogic( d.TOPO_0DR15_2MU6ab & d.MU6.x(2) & physcond)
            MenuItem('L1_LFV-MU').setLogic( d.R2TOPO_0DR10_MU10ab_MU6ab & d.MU10 & d.MU6.x(2) & physcond)

            if isV8 or isHIV5 or isPhaseII:
                MenuItem('L1_LFV-EM8I').setLogic( d.TOPO_0DETA04_EM8abi_MU10ab & d.TOPO_0DPHI03_EM8abi_MU10ab & d.MU10 & physcond) #ATR-14282
                MenuItem('L1_LFV-EM15I').setLogic( d.TOPO_0DETA04_EM15abi_MUab & d.TOPO_0DPHI03_EM15abi_MUab & physcond) #ATR-14282
            else:
                MenuItem('L1_LFV-EM8I').setLogic( d.TOPO_0DETA04_0DPHI03_EM8abi_MU10ab & physcond)
                MenuItem('L1_LFV-EM15I').setLogic( d.TOPO_0DETA04_0DPHI03_EM15abi_MUab & physcond)
                
            MenuItem('L1_DPHI-J20s2XE50').setLogic( d.TOPO_10MINDPHI_J20s2_XE50 & physcond)
            MenuItem('L1_DPHI-J20XE50').setLogic( d.R2TOPO_10MINDPHI_J20ab_XE50 & physcond)
            MenuItem('L1_DPHI-CJ20XE50').setLogic( d.R2TOPO_10MINDPHI_CJ20ab_XE50 & physcond)

            MenuItem('L1_MJJ-900').setLogic( d.R2TOPO_900INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_MJJ-800').setLogic( d.R2TOPO_800INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_MJJ-700').setLogic( d.TOPO_700INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_MJJ-400').setLogic( d.R2TOPO_400INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_MJJ-300').setLogic( d.TOPO_300INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_MJJ-200').setLogic( d.R2TOPO_200INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_MJJ-100').setLogic( d.R2TOPO_100INVM9999_AJ30s6_AJ20s6 & physcond)

            MenuItem('L1_MJJ-600-NFF').setLogic( d.R2TOPO_600INVM9999_J30s6_AJ20s6 & physcond)
            MenuItem('L1_MJJ-500-NFF').setLogic( d.TOPO_500INVM9999_J30s6_AJ20s6 & physcond)
            MenuItem('L1_MJJ-400-NFF').setLogic( d.R2TOPO_400INVM9999_J30s6_AJ20s6 & physcond)
            MenuItem('L1_MJJ-200-NFF').setLogic( d.R2TOPO_200INVM9999_J30s6_AJ20s6 & physcond)
            MenuItem('L1_63DETA-J30J15').setLogic( d.R2TOPO_63DETA127_FJ20s1_FJ20s2 & physcond)
            MenuItem('L1_J50_DETA20-J50J').setLogic( d.J50 & d.TOPO_0DETA20_J50s1_Js2 & physcond)
            MenuItem('L1_DPHI-2EM3').setLogic( d.TOPO_27DPHI32_EMs1_EMs6 & physcond)
            MenuItem('L1_DPHI-2EM3_VTE5.24ETA49').setLogic( d.TOPO_27DPHI32_EMs1_EMs6 & Not(d.TE524ETA49) & physcond).setTriggerType(TT.calo)
            MenuItem('L1_DPHI-2EM3_VTE5.24ETA49_ALFA_EINE').setLogic( d.TOPO_27DPHI32_EMs1_EMs6 & Not(d.TE524ETA49) & ALFA_EINE & physcond).setTriggerType(TT.alfa)
            MenuItem('L1_DPHI-2EM3_VTE10').setLogic( d.TOPO_27DPHI32_EMs1_EMs6 & Not(d.TE10) & physcond).setTriggerType(TT.calo)
            MenuItem('L1_DPHI-2EM7_VTE50').setLogic( d.EM7.x(2) & d.TOPO_27DPHI32_EMs1_EMs6 & Not(d.TE50) & physcond).setTriggerType(TT.calo)
            
            MenuItem('L1_HT150-JJ15.ETA49').setLogic( d.R2TOPO_HT150_AJj15allETA49 & physcond)
            MenuItem('L1_HT150-JJ15.ETA49_MJJ-400').setLogic( d.R2TOPO_HT150_AJj15allETA49 & d.R2TOPO_400INVM9999_AJ30s6_AJ20s6 & physcond)


            MenuItem('L1_J4-MATCH').setLogic( d.R2TOPO_0MATCH_4AJ20ETA31_4AJj15ETA31 & physcond)

            MenuItem('L1_LLP-RO').setLogic( d.TOPO_100RATIO_0MATCH_TAU30si2_EMall & physcond)

            MenuItem('L1_LLP-NOMATCH').setLogic( d.TOPO_NOT_0MATCH_TAU30si1_EMall & physcond)
            MenuItem('L1_DR-MU10TAU12I').setLogic( d.TOPO_0DR28_MU10ab_TAU12abi & physcond)
            MenuItem('L1_TAU12I-J25').setLogic( d.TOPO_1DISAMB_TAU12abi_J25ab & physcond)
            MenuItem('L1_EM15TAU12I-J25').setLogic( d.R2TOPO_1DISAMB_EM15his2_TAU12abi_J25ab & physcond)

            MenuItem('L1_DR-EM15TAU12I').setLogic( d.R2TOPO_DISAMB_0DR28_EM15his2_TAU12abi & physcond)
            MenuItem('L1_DR-EM15TAU12I-J25').setLogic( d.TOPO_1DISAMB_J25ab_0DR28_EM15his2_TAU12abi & physcond)
            MenuItem('L1_TAU20ITAU12I-J25').setLogic( d.R2TOPO_1DISAMB_TAU20abi_TAU12abi_J25ab & physcond)
            MenuItem('L1_DR25-TAU20ITAU12I').setLogic( d.TOPO_0DR25_TAU20abi_TAU12abi & physcond)

            MenuItem('L1_30M-EM20ITAU12').setLogic( d.TOPO_DISAMB_30INVM_EM20his2_TAU12ab & physcond)
            MenuItem('L1_MJJ-400-CF').setLogic( d.TOPO_400INVM9999_AJ30s6ETA31_AJ20s631ETA49 & physcond)
            MenuItem('L1_DR-TAU20ITAU12I').setLogic( d.TOPO_0DR28_TAU20abi_TAU12abi & physcond)
            MenuItem('L1_BOX-TAU20ITAU12I').setLogic( d.R2TOPO_0DETA20_0DPHI20_TAU20abi_TAU12abi & physcond)

            if isV8:
                MenuItem('L1_DR-TAU20ITAU12I-J25').setLogic( d.TOPO_2DISAMB_J25ab_0DR28_TAU20abi_TAU12abi & physcond)
            else:
                MenuItem('L1_DR-TAU20ITAU12I-J25').setLogic( d.TOPO_1DISAMB_J25ab_0DR28_TAU20abi_TAU12abi & physcond)

            MenuItem('L1_DR25-TAU20ITAU12I-J25').setLogic( d.R2TOPO_1DISAMB_J25ab_0DR25_TAU20abi_TAU12abi & physcond)
            MenuItem('L1_LAR-EM').setLogic( d.R2TOPO_LAR_EM20shi1 & physcond).setTriggerType( TT.lardemo ) # LAr demo (ATR-11897)
            MenuItem('L1_LAR-J').setLogic( d.R2TOPO_LAR_J100s1 & physcond).setTriggerType( TT.lardemo )  # LAr demo (ATR-11897)

            MenuItem('L1_LAR-ZEE').setLogic( d.R2TOPO_ZEE_EM20shi2 & physcond).setTriggerType( TT.calo ) # LAr demo (ATR-11897)
            MenuItem('L1_LAR-ZEE_LAR-EM').setLogic( d.R2TOPO_ZEE_EM20shi2 & d.R2TOPO_LAR_EM20shi1 &physcond).setTriggerType( TT.lardemo )

            MenuItem('L1_MJJ-400-NFF-0DPHI20').setLogic( d.TOPO_400INVM9999_0DPHI20_J30s6_AJ20s6 & physcond)
            MenuItem('L1_MJJ-400-NFF-0DPHI22').setLogic( d.TOPO_400INVM9999_0DPHI22_J30s6_AJ20s6 & physcond)
            MenuItem('L1_MJJ-400-NFF-0DPHI24').setLogic( d.TOPO_400INVM9999_0DPHI24_J30s6_AJ20s6 & physcond)
            MenuItem('L1_MJJ-400-NFF-0DPHI26').setLogic( d.TOPO_400INVM9999_0DPHI26_J30s6_AJ20s6 & physcond)

            MenuItem('L1_BPH-2M8-2MU4_BPH-0DR15-2MU4'     ).setLogic( d.R2TOPO_2INVM8_2MU4ab & d.R2TOPO_0DR15_2MU4ab & physcond)            # 78% for Bs,J/psi
            MenuItem('L1_BPH-2M9-2MU4_BPH-0DR15-2MU4'     ).setLogic( d.R2TOPO_2INVM9_2MU4ab & d.R2TOPO_0DR15_2MU4ab & physcond)            # 78% for Bs,J/psi
            MenuItem('L1_BPH-7M15-2MU4_BPH-0DR24-2MU4'    ).setLogic( d.R2TOPO_7INVM15_2MU4ab & d.R2TOPO_0DR24_2MU4ab & physcond)           # 78% for Upsi
            MenuItem('L1_BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4' ).setLogic( d.R2TOPO_2INVM8_MU6ab_MU4ab & d.R2TOPO_0DR15_MU6ab_MU4ab & d.MU6 & physcond)  # 86% for Bs,J/psi
            MenuItem('L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4' ).setLogic( d.R2TOPO_2INVM9_MU6ab_MU4ab & d.R2TOPO_0DR15_MU6ab_MU4ab & d.MU6 & physcond)

            #ATR-19720
            MenuItem('L1_BPH-8M15-0DR22-2MU6'     ).setLogic( d.TOPO_8INVM15_0DR22_2MU6ab & d.MU6.x(2) & physcond)
            MenuItem('L1_BPH-2M9-2DR15-2MU6'      ).setLogic( d.TOPO_2INVM9_2DR15_2MU6ab & d.MU6.x(2) & physcond)
            MenuItem('L1_BPH-2M9-0DR15-MU6MU4'    ).setLogic( d.TOPO_2INVM9_0DR15_MU6ab_MU4ab & d.MU6 & physcond)
            MenuItem('L1_BPH-8M15-0DR22-MU6MU4-BO').setLogic( d.TOPO_8INVM15_0DR22_MU6ab_MU4ab & d.R2TOPO_MULT_CMU4ab.x(2) & d.R2TOPO_MULT_CMU6ab & d.MU6 & physcond)
            MenuItem('L1_BPH-2M9-0DR15-2MU4'      ).setLogic( d.TOPO_2INVM9_0DR15_2MU4ab & physcond)

            MenuItem('L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4').setLogic( d.R2TOPO_8INVM15_MU6ab_MU4ab & d.R2TOPO_0DR22_MU6ab_MU4ab & d.MU6 & physcond) # 86% for Upsi
            MenuItem('L1_BPH-2M9-2MU6_BPH-2DR15-2MU6'     ).setLogic( d.R2TOPO_2INVM9_2MU6ab & d.R2TOPO_2DR15_2MU6ab & d.MU6.x(2) & physcond)            # 96% for Bs,J/psi
            MenuItem('L1_BPH-2M9-2MU6'     ).setLogic( d.R2TOPO_2INVM9_2MU6ab & d.MU6.x(2) & physcond)

            MenuItem('L1_BPH-8M15-2MU6_BPH-0DR22-2MU6'    ).setLogic( d.R2TOPO_8INVM15_2MU6ab & d.R2TOPO_0DR22_2MU6ab & d.MU6.x(2) & physcond)           # 96% for Upsi

            # Barrel and BarrelOnly items w/o topo selection
            MenuItem('L1_2MU4-B'    ).setLogic( d.MU4.x(2) & d.R2TOPO_MULT_CMU4ab & physcond)              # 2MU4_Barrel
            MenuItem('L1_2MU6-B'    ).setLogic( d.MU6.x(2) & d.R2TOPO_MULT_CMU6ab & physcond)              # 2MU6_Barrel
            MenuItem('L1_MU6_2MU4-B').setLogic( d.MU6 & d.MU4.x(2) & d.R2TOPO_MULT_CMU4ab & physcond)        # MU6_2MU4_Barrel,

            # Barrel and BarrelOnly variants of L1Topo items
            MenuItem('L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4'     ).setLogic( d.R2TOPO_2INVM8_CMU4ab_MU4ab & d.R2TOPO_0DR15_2MU4ab & physcond)                # 78% for Bs,J/psi + OneBarrel
            MenuItem('L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-B'    ).setLogic( d.R2TOPO_7INVM15_2MU4ab & d.R2TOPO_0DR24_2MU4ab & d.R2TOPO_MULT_CMU4ab & physcond) # 78% for Upsi + OneBarrel
            MenuItem('L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-BO'   ).setLogic( d.R2TOPO_7INVM15_2MU4ab & d.R2TOPO_0DR24_2CMU4ab & physcond)                    # 78% for Upsi + BarrelOnly
            MenuItem('L1_BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4' ).setLogic( d.R2TOPO_2INVM8_ONEBARREL_MU6ab_MU4ab & d.R2TOPO_0DR15_MU6ab_MU4ab & d.MU6 & physcond)  # 86% for Bs,J/psi + OneBarrel
            MenuItem('L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-B').setLogic( d.R2TOPO_8INVM15_MU6ab_MU4ab & d.R2TOPO_0DR22_MU6ab_MU4ab & d.R2TOPO_MULT_CMU4ab & d.MU6 & physcond)
            MenuItem('L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-BO').setLogic( d.R2TOPO_8INVM15_MU6ab_MU4ab & d.R2TOPO_0DR22_MU6ab_MU4ab & d.R2TOPO_MULT_CMU4ab.x(2) & d.R2TOPO_MULT_CMU6ab & d.MU6 & physcond)

            MenuItem('L1_BPH-2M8-2MU4').setLogic( d.R2TOPO_2INVM8_2MU4ab & physcond)                  # backup 71% for Bs,J/psi
            MenuItem('L1_BPH-8M15-MU6MU4').setLogic( d.R2TOPO_8INVM15_MU6ab_MU4ab & d.MU6 & physcond) # backup 71% for Upsi
            MenuItem('L1_BPH-8M15-2MU6').setLogic( d.R2TOPO_8INVM15_2MU6ab & d.MU6.x(2) & physcond)   # backup 95% for Upsi

            MenuItem('L1_BPH-0M9-EM7-EM5').setLogic( d.TOPO_0INVM9_EM7ab_EMab & physcond)
            MenuItem('L1_BPH-0DR3-EM7J15').setLogic( d.TOPO_0DR03_EM7ab_CJ15ab & physcond)
            MenuItem('L1_BPH-0M9-EM7-EM5_MU6').setLogic( d.MU6 & d.TOPO_0INVM9_EM7ab_EMab & physcond)
            MenuItem('L1_BPH-0DR3-EM7J15_MU6').setLogic(  d.MU6 &d.TOPO_0DR03_EM7ab_CJ15ab & physcond)
            MenuItem('L1_BPH-0M9-EM7-EM5_2MU4').setLogic( d.MU4.x(2) & d.TOPO_0INVM9_EM7ab_EMab & physcond)
            MenuItem('L1_BPH-0DR3-EM7J15_2MU4').setLogic( d.MU4.x(2) & d.TOPO_0DR03_EM7ab_CJ15ab & physcond)
            MenuItem('L1_BPH-0M9-EM7-EM5_MU4').setLogic( d.MU4 & d.TOPO_0INVM9_EM7ab_EMab & physcond)
            MenuItem('L1_BPH-0DR3-EM7J15_MU4').setLogic(  d.MU4 &d.TOPO_0DR03_EM7ab_CJ15ab & physcond)

            #SX end ATR-12789
            MenuItem('L1_LATE-MU10_XE50').setLogic( d.TOPO_LATE_MU10s1 & d.XE50 & physcond)
            MenuItem('L1_LATE-MU10_XE40').setLogic( d.TOPO_LATE_MU10s1 & d.XE40 & physcond)
            MenuItem('L1_LATE-MU10_J50').setLogic(  d.TOPO_LATE_MU10s1 & d.J50 & physcond)

            MenuItem('L1_SC111-CJ15').setLogic(  d.TOPO_SC111_CJ15abETA26 & physcond)
            MenuItem('L1_SC85-CJ15').setLogic(  d.R2TOPO_SC85_CJ15abETA26 & physcond)

            MenuItem('L1_TAU60_DR-TAU20ITAU12I' ).setLogic( d.HA60 & d.TOPO_0DR28_TAU20abi_TAU12abi & physcond)

            MenuItem('L1_CEP-CJ60').setLogic( d.TOPO_CEP_CJ60s6 & physcond )
            MenuItem('L1_CEP-CJ50').setLogic( d.TOPO_CEP_CJ50s6 & physcond )
            MenuItem('L1_CEP-CJ50.ETA21').setLogic( d.TOPO_CEP_CJ50s6ETA21 & physcond )

        except NameError as ex:
            exc_type, exc_value, exc_traceback = sys.exc_info()
            fn,ln,_,_ = traceback.extract_tb(exc_traceback)[0]
            fn = fn.rsplit("/",1)[-1]

            msg = ex.args[0]
            log.error("Creation of L1Topo item failed, since variable %s" % msg)
            m = re.match("name '(?P<varname>.*)' is not defined", msg)
            if m:
                newmsg = "In line %i in file %s, %s" % (ln, fn, msg)
                missingVar =  m.groupdict()["varname"]
                key = missingVar.split('_',1)[-1]
                if key in ItemDef.otherTopoNames:
                    alternative = ', '.join(ItemDef.otherTopoNames[key])
                    log.info("However, there is an alternative defined: %s" % alternative)
                    newmsg += ", however there is an alternative: %s" % alternative
                ex.args=(newmsg,)
            raise


        except Exception as ex:
            log.error( "Creation of L1Topo item failed, will abort! Exception is a %s: '%s'" , type(ex), ex)
            raise


        # =======================================================
        #
        # Here we define the items for the secondary partitions
        #
        # =======================================================

        # Partition 2
        MenuItem.currentPartition = 2

        #Removed after ATR-17056 since we need L1_BCM_CA_UNPAIREDB2 in partition 1
        #MenuItem('L1_RD2_BGRP14'         ).setLogic( d.RNDM2 & d.BGRP0 & d.BGRP14             ).setTriggerType(TT.rand)


        # Partition 3
        MenuItem.currentPartition = 3

        MenuItem('L1_RD3_BGRP15'         ).setLogic( d.RNDM3 & d.BGRP0 & d.BGRP15             ).setTriggerType(TT.rand)


        # reset to partition 1
        MenuItem.currentPartition = 1





        MenuItem.l1configForRegistration = None
