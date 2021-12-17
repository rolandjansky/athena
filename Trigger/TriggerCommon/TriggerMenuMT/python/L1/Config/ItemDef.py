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
log = logging.getLogger(__name__)

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
        MenuItem('L1_EM10'      ).setLogic( d.EM10       & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM12'      ).setLogic( d.EM12       & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM14'      ).setLogic( d.EM14       & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15'      ).setLogic( d.EM15       & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM16'      ).setLogic( d.EM16       & physcond).setTriggerType( TT.calo )
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
        MenuItem('L1_EM15VHI'   ).setLogic( d.EM15VHI    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM22VH'    ).setLogic( d.EM22VH     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM22'    ).setLogic( d.EM22     & physcond).setTriggerType( TT.calo )

        # Phase-I
        MenuItem('L1_eEM3'      ).setLogic( d.eEM3       & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM5'      ).setLogic( d.eEM5       & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM7'      ).setLogic( d.eEM7       & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM8L'     ).setLogic( d.eEM8L      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM10L'    ).setLogic( d.eEM10L     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM12'     ).setLogic( d.eEM12      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM15'     ).setLogic( d.eEM15      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM15L'    ).setLogic( d.eEM15L     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM15M'    ).setLogic( d.eEM15M     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM18M'    ).setLogic( d.eEM18M     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM20L'    ).setLogic( d.eEM20L     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM20VM'   ).setLogic( d.eEM20VM    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM22'     ).setLogic( d.eEM22      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM22L'    ).setLogic( d.eEM22L     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM22M'    ).setLogic( d.eEM22M     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM22T'    ).setLogic( d.eEM22T     & physcond).setTriggerType( TT.calo ) 
        MenuItem('L1_eEM24M'    ).setLogic( d.eEM24M     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eEM7_EMPTY'        ).setLogic(d.eEM7 & cosmiccond      ).setTriggerType( TT.calo )
        MenuItem('L1_eEM7_UNPAIRED_ISO' ).setLogic(d.eEM7 & unpaired_isocond).setTriggerType( TT.calo )

        # 2xEM, 3xEM
        MenuItem('L1_2EM3'           ).setLogic( d.EM3.x(2)             & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM7'           ).setLogic( d.EM7.x(2)             & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM10'          ).setLogic( d.EM10.x(2)            & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM10VH'        ).setLogic( d.EM10VH.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM8I'          ).setLogic( d.EM8I.x(2)            & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM13VH'        ).setLogic( d.EM13VH.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM18VH'        ).setLogic( d.EM18VH.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM12'          ).setLogic( d.EM12.x(2)            & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM15'          ).setLogic( d.EM15.x(2)            & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM15VH'        ).setLogic( d.EM15VH.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM15VHI'       ).setLogic( d.EM15VHI.x(2)         & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM16'          ).setLogic( d.EM16.x(2)            & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM20VH'        ).setLogic( d.EM20VH.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM7_2EM3'       ).setLogic( d.EM7 & d.EM3.x(2)       & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM12_2EM3'      ).setLogic( d.EM12 & d.EM3.x(2)      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM20VH_2EM7'    ).setLogic( d.EM20VH & d.EM7.x(2)    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM20VH_3EM10VH' ).setLogic( d.EM20VH & d.EM10VH.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM20VH_2EM10VH_3EM8VH' ).setLogic( d.EM20VH & d.EM10VH.x(2) & d.EM8VH.x(3)    & physcond).setTriggerType( TT.calo )

        # PhaseI 2xEM and 3xEM
        MenuItem('L1_2eEM15L').setLogic(d.eEM15L.x(2) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2eEM15M').setLogic(d.eEM15M.x(2) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2eEM20L').setLogic(d.eEM20L.x(2) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3eEM10L').setLogic(d.eEM10L.x(3) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_eEM20L_3eEM10L').setLogic(d.eEM20L & d.eEM10L.x(3) & physcond).setTriggerType(TT.calo)

        # 4xEM
        MenuItem('L1_EM15VH_3EM7'         ).setLogic( d.EM15VH & d.EM7.x(3)      & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VH_3EM8VH'       ).setLogic( d.EM15VH & d.EM8VH.x(3)    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VH_3EM10VH'      ).setLogic( d.EM15VH & d.EM10VH.x(3)   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VH_2EM10VH_3EM7' ).setLogic( d.EM15VH & d.EM10VH.x(2)   & d.EM7.x(3)     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM18VH_3EM8VH'       ).setLogic( d.EM18VH & d.EM8VH.x(3)    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM18VH_2EM10VH_3EM7' ).setLogic( d.EM18VH & d.EM10VH.x(2)   & d.EM7.x(3)     & physcond).setTriggerType( TT.calo )

        # EM and jet
        MenuItem('L1_J15p23ETA49' ).setLogic( d.J1523ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J15p24ETA49' ).setLogic( d.J1524ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J15p31ETA49' ).setLogic( d.J1531ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J20p28ETA31' ).setLogic( d.J2028ETA31 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J20p31ETA49' ).setLogic( d.J2031ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J30p31ETA49' ).setLogic( d.J3031ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40p0ETA25'  ).setLogic( d.J400ETA25  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J50p31ETA49' ).setLogic( d.J5031ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J70p31ETA49' ).setLogic( d.J7031ETA49 & physcond).setTriggerType(TT.calo)

        MenuItem('L1_J15p0ETA25'  ).setLogic( d.J150ETA25 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J20p0ETA28'  ).setLogic( d.J200ETA28 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J25p0ETA23'  ).setLogic( d.J250ETA23 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J35p0ETA23'  ).setLogic( d.J350ETA23 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J20p0ETA49'  ).setLogic( d.J200ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J30p0ETA49'  ).setLogic( d.J300ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J25p0ETA23_2J15p31ETA49'  ).setLogic( d.J250ETA23 & d.J1531ETA49.x(2) & physcond).setTriggerType(TT.calo)


        MenuItem('L1_2EM3_VTE70').setLogic( d.EM3.x(2) & Not(d.TE70) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2EM3_VTE50').setLogic( d.EM3.x(2) & Not(d.TE50) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2EM8I_J50' ).setLogic( d.EM8I.x(2) & d.J50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2EM8I_J75' ).setLogic( d.EM8I.x(2) & d.J75 & physcond).setTriggerType(TT.calo)

        # HI
        MenuItem('L1_EM3_NZ'             ).setLogic( d.EM3      & Not(d.ZDC_AND) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM3_NZ'            ).setLogic( d.EM3.x(2) & Not(d.ZDC_AND) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_EM3_NL'             ).setLogic( d.EM3      & Not(d.LUCID_A) & Not(d.LUCID_C) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2EM3_NL'            ).setLogic( d.EM3.x(2) & Not(d.LUCID_A) & Not(d.LUCID_C) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_EM3_MV'             ).setLogic( d.EM3      & Not(d.MBTS_A & d.MBTS_C) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM3_MV_VTE50'       ).setLogic( d.EM3      & Not(d.MBTS_A & d.MBTS_C) & Not(d.TE50) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_EM3_TE50'           ).setLogic( d.EM3      & d.TE50 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM3_VTE50'          ).setLogic( d.EM3      & Not(d.TE50) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM7_VTE200'         ).setLogic( d.EM7      & Not(d.TE200) & physcond).setTriggerType( TT.calo )

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

        MenuItem('L1_EM20VH_FIRSTEMPTY'  ).setLogic( d.EM20VH  & firstempty).setTriggerType( TT.calo )
        MenuItem('L1_EM22VHI_FIRSTEMPTY' ).setLogic( d.EM22VHI & firstempty).setTriggerType( TT.calo )

        MenuItem('L1_J10_VTE100'         ).setLogic( d.J10  & Not(d.TE100) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_J10_VTE200'         ).setLogic( d.J10  & Not(d.TE200) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_J10_VTE300'         ).setLogic( d.J10  & Not(d.TE300) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_J12_VTE100'         ).setLogic( d.J12  & Not(d.TE100) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_J12_VTE200'         ).setLogic( d.J12  & Not(d.TE200) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TE50_VTE200'        ).setLogic( d.TE50 & Not(d.TE200) & physcond).setTriggerType( TT.ftk )

        MenuItem('L1_TE5_VTE200'         ).setLogic( d.TE5  & Not(d.TE200) & physcond).setTriggerType( TT.ftk )
        MenuItem('L1_TE5_VTE100'         ).setLogic( d.TE5  & Not(d.TE100) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TE5_VTE50'          ).setLogic( d.TE5  & Not(d.TE50)  & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TE4_VTE200'         ).setLogic( d.TE4  & Not(d.TE200) & physcond).setTriggerType( TT.ftk )
        MenuItem('L1_TE4_VTE100'         ).setLogic( d.TE4  & Not(d.TE100) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TE4_VTE50'          ).setLogic( d.TE4  & Not(d.TE50)  & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TE4_VTE200_EMPTY'   ).setLogic( d.TE4  & Not(d.TE200) & cosmiccond).setTriggerType( TT.calo )
        MenuItem('L1_TE5_VTE200_EMPTY'   ).setLogic( d.TE5  & Not(d.TE200) & cosmiccond).setTriggerType( TT.calo )


        # MUON ctpid=[0x20;0x2f]
        # RUn3 thresholds
        MenuItem('L1_MU3V'       ).setLogic( d.MU3V       & physcond).setTriggerType(TT.muon) # ~ Run2 MU4 eff
        MenuItem('L1_MU5VF'      ).setLogic( d.MU5VF      & physcond).setTriggerType(TT.muon) # ~ Run2 MU6
        MenuItem('L1_MU8F'       ).setLogic( d.MU8F       & physcond).setTriggerType(TT.muon) # ~ Run2 MU10
        MenuItem('L1_MU8VF'      ).setLogic( d.MU8VF      & physcond).setTriggerType(TT.muon) # ~ Run2 MU11
        MenuItem('L1_MU14FCH'    ).setLogic( d.MU14FCH    & physcond).setTriggerType(TT.muon) # ~ Run2 MU20
        MenuItem('L1_MU14FCHR'   ).setLogic( d.MU14FCHR   & physcond).setTriggerType(TT.muon) # ~ Run2 MU21

        MenuItem('L1_MU3VF'      ).setLogic( d.MU3VF      & physcond).setTriggerType(TT.muon) # ~ Run2 MU4 rate
        MenuItem('L1_MU8FC'      ).setLogic( d.MU8FC      & physcond).setTriggerType(TT.muon) # Backup MU8F
        MenuItem('L1_MU8VFC'     ).setLogic( d.MU8VFC     & physcond).setTriggerType(TT.muon) # Backup MU8VF
        MenuItem('L1_MU15VFCH'   ).setLogic( d.MU15VFCH   & physcond).setTriggerType(TT.muon) # 

        MenuItem('L1_MU10BOM'    ).setLogic( d.MU10BOM    & physcond).setTriggerType(TT.muon) # Barrel-only close-by muons
        MenuItem('L1_MU20VFC'    ).setLogic( d.MU20VFC    & physcond).setTriggerType(TT.muon) # alignment with toroid off

        MenuItem('L1_MU10BO'     ).setLogic( d.MU10BO     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU4BOM'     ).setLogic( d.MU4BOM     & physcond).setTriggerType(TT.muon) # Barrel-only close-by muons 
        MenuItem('L1_MU12BOM'    ).setLogic( d.MU12BOM    & physcond).setTriggerType(TT.muon) # Barrel-only close-by muons  

        MenuItem('L1_2MU3V'        ).setLogic( d.MU3V.x(2)             & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU5VF'       ).setLogic( d.MU5VF.x(2)            & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU8F'        ).setLogic( d.MU8F.x(2)             & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU8VF'       ).setLogic( d.MU8VF.x(2)            & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU14FCH'     ).setLogic( d.MU14FCH.x(2)          & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU5VF_2MU3V'  ).setLogic( d.MU5VF & d.MU3V.x(2)   & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU8F_2MU3V'   ).setLogic( d.MU8F & d.MU3V.x(2)    & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU8F_2MU5VF'  ).setLogic( d.MU8F & d.MU5VF.x(2)   & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU8VF_2MU5VF' ).setLogic( d.MU8VF & d.MU5VF.x(2)  & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU5VF_3MU3V'  ).setLogic( d.MU5VF & d.MU3V.x(3)   & physcond).setTriggerType(TT.muon)
        MenuItem('L1_3MU3V'        ).setLogic( d.MU3V.x(3)             & physcond).setTriggerType(TT.muon)
        MenuItem('L1_3MU5VF'       ).setLogic( d.MU5VF.x(3)            & physcond).setTriggerType(TT.muon)
        MenuItem('L1_4MU3V'        ).setLogic( d.MU3V.x(4)             & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU5VF_3MU3V' ).setLogic( d.MU5VF.x(2) & d.MU3V.x(3)   & physcond).setTriggerType(TT.muon)

        MenuItem('L1_2MU14FCH_OVERLAY').setLogic( d.MU14FCH.x(2)       & physcond).setTriggerType(TT.zerobs)

        # HI
        MenuItem('L1_MU3V_VTE10' ).setLogic( d.MU3V      & Not(d.TE10) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU3V_VTE10').setLogic( d.MU3V.x(2) & Not(d.TE10) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU3V_VTE50').setLogic( d.MU3V.x(2) & Not(d.TE50) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE10'  ).setLogic( d.MU3V      & d.TE10 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE20'  ).setLogic( d.MU3V      & d.TE20 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE30'  ).setLogic( d.MU3V      & d.TE30 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE40'  ).setLogic( d.MU3V      & d.TE40 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE50'  ).setLogic( d.MU3V      & d.TE50 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE70'  ).setLogic( d.MU3V      & d.TE70 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE90'  ).setLogic( d.MU3V      & d.TE90 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE120' ).setLogic( d.MU3V      & d.TE120 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE160' ).setLogic( d.MU3V      & d.TE160 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE10p24ETA49' ).setLogic( d.MU3V  & d.TE1024ETA49 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE15p24ETA49' ).setLogic( d.MU3V  & d.TE1524ETA49 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE20p24ETA49' ).setLogic( d.MU3V  & d.TE2024ETA49 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE25p24ETA49' ).setLogic( d.MU3V  & d.TE2524ETA49 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE30p24ETA49' ).setLogic( d.MU3V  & d.TE3024ETA49 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE35p24ETA49' ).setLogic( d.MU3V  & d.TE3524ETA49 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_TE40p24ETA49' ).setLogic( d.MU3V  & d.TE4024ETA49 & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_VTE50' ).setLogic( d.MU3V     & Not(d.TE50) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU5VF_VTE50').setLogic( d.MU5VF    & Not(d.TE50) & physcond).setTriggerType(TT.muon)

        MenuItem('L1_MU3V_UNPAIRED_ISO'   ).setLogic(d.MU3V  & unpaired_isocond   ).setTriggerType( TT.muon )
        MenuItem('L1_MU3V_UNPAIRED_NONISO').setLogic(d.MU3V  & unpaired_nonisocond).setTriggerType( TT.muon )
        MenuItem('L1_MU3V_EMPTY'          ).setLogic(d.MU3V  & cosmiccond).setTriggerType( TT.muon )
        MenuItem('L1_MU5VF_EMPTY'         ).setLogic(d.MU5VF & cosmiccond).setTriggerType( TT.muon )
        MenuItem('L1_MU3V_FIRSTEMPTY'     ).setLogic(d.MU3V  & firstempty).setTriggerType( TT.muon )
        MenuItem('L1_MU5VF_FIRSTEMPTY'    ).setLogic(d.MU5VF & firstempty).setTriggerType( TT.muon )
        MenuItem('L1_MU8F_EMPTY'          ).setLogic(d.MU8F  & cosmiccond).setTriggerType( TT.muon )
        MenuItem('L1_MU8VF_EMPTY'         ).setLogic(d.MU8VF & cosmiccond).setTriggerType( TT.muon )
        MenuItem('L1_MU14FCH_FIRSTEMPTY'  ).setLogic(d.MU14FCH  & firstempty).setTriggerType(TT.muon)
        MenuItem('L1_MU14FCH_EMPTY'       ).setLogic(d.MU14FCH  & cosmiccond).setTriggerType(TT.muon)
        MenuItem('L1_MU14FCH_UNPAIRED_ISO').setLogic(d.MU14FCH  & unpaired_isocond).setTriggerType(TT.muon)
        MenuItem('L1_MU14FCHR_FIRSTEMPTY' ).setLogic(d.MU14FCHR & firstempty).setTriggerType(TT.muon)
        MenuItem('L1_2MU3V_EMPTY'           ).setLogic(d.MU3V.x(2)  & cosmiccond).setTriggerType( TT.muon )
        MenuItem('L1_2MU5VF_UNPAIRED_ISO'   ).setLogic(d.MU5VF.x(2) & unpaired_isocond).setTriggerType( TT.muon )
        MenuItem('L1_2MU5VF_UNPAIRED_NONISO').setLogic(d.MU5VF.x(2) & unpaired_nonisocond).setTriggerType( TT.muon )
        MenuItem('L1_2MU5VF_EMPTY'          ).setLogic(d.MU5VF.x(2) & cosmiccond).setTriggerType( TT.muon )
        MenuItem('L1_2MU5VF_FIRSTEMPTY'     ).setLogic(d.MU5VF.x(2) & firstempty).setTriggerType( TT.muon )

        # EM and MU
        MenuItem('L1_EM7_MU8F'       ).setLogic( d.EM7        & d.MU8F     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM7_MU14FCH'    ).setLogic( d.EM7        & d.MU14FCH  & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM20VH_MU14FCH' ).setLogic( d.EM20VH     & d.MU14FCH  & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM7_MU8VF'      ).setLogic( d.EM7        & d.MU8VF    & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM8_MU8F'       ).setLogic( d.EM8        & d.MU8F     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM8I_MU8F'      ).setLogic( d.EM8I       & d.MU8F     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM15I_MU3V'     ).setLogic( d.EM15  & d.EM8I     & d.MU3V      & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM15VH_MU8F'    ).setLogic( d.EM15VH     & d.MU8F     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM3_MU14FCH'    ).setLogic( d.EM3        & d.MU14FCH  & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2EM8VH_MU8F'    ).setLogic( d.EM8VH.x(2) & d.MU8F     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_EM15VH_2EM8VH_MU5VF').setLogic( d.EM15VH & d.EM8VH.x(2) &  d.MU5VF & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2eEM8L_MU8F'    ).setLogic( d.eEM8L.x(2) & d.MU8F     & physcond).setTriggerType(TT.muon)
        MenuItem('L1_eEM15L_MU8F'    ).setLogic( d.eEM15L     & d.MU8F     & physcond).setTriggerType(TT.muon)

        # TAU ctpid=[0x40:0x4f]
        MenuItem('L1_TAU2'  ).setLogic( d.HA2   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU3'  ).setLogic( d.HA3   & physcond).setTriggerType( TT.calo )
        # ATR-19359
        MenuItem('L1_TAU5'  ).setLogic( d.HA5   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2TAU5' ).setLogic( d.HA5.x(2)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TAU6'  ).setLogic( d.HA6   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU8'  ).setLogic( d.HA8   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2TAU8' ).setLogic( d.HA8.x(2)  & physcond).setTriggerType(TT.calo)

        MenuItem('L1_TAU12'  ).setLogic( d.HA12  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU12IL').setLogic( d.HA12IL & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU12IM').setLogic( d.HA12IM & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU12IT').setLogic( d.HA12IT & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU12I' ).setLogic( d.HA12I & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU15'  ).setLogic( d.HA15  & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU20'  ).setLogic( d.HA20  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IL').setLogic( d.HA20IL  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM').setLogic( d.HA20IM  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IT').setLogic( d.HA20IT  & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU20A' ).setLogic( d.HA20A  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20C' ).setLogic( d.HA20C  & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU25'  ).setLogic( d.HA25  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU25IT').setLogic( d.HA25IT  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU25IM').setLogic( d.HA25IM  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU30'  ).setLogic( d.HA30  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU40'  ).setLogic( d.HA40  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU60'  ).setLogic( d.HA60  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU100' ).setLogic( d.HA100  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU90'  ).setLogic( d.HA90  & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU8_UNPAIRED_ISO'   ).setLogic( d.HA8   & unpaired_isocond ).setTriggerType( TT.calo )

        MenuItem('L1_TAU8_UNPAIRED_NONISO').setLogic( d.HA8   & unpaired_nonisocond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU8_EMPTY'          ).setLogic( d.HA8   & cosmiccond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU8_FIRSTEMPTY'     ).setLogic( d.HA8   & firstempty ).setTriggerType( TT.calo )

        MenuItem('L1_TAU12_UNPAIRED_ISO'   ).setLogic( d.HA12   & unpaired_isocond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU12_UNPAIRED_NONISO').setLogic( d.HA12   & unpaired_nonisocond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU12_EMPTY'          ).setLogic( d.HA12  & cosmiccond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU12_FIRSTEMPTY'     ).setLogic( d.HA12   & firstempty ).setTriggerType( TT.calo )
        MenuItem('L1_TAU30_EMPTY'          ).setLogic( d.HA30   & cosmiccond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU30_UNPAIRED_ISO'   ).setLogic( d.HA30  & unpaired_isocond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU40_EMPTY'          ).setLogic( d.HA40   & cosmiccond ).setTriggerType( TT.calo )
        MenuItem('L1_TAU40_UNPAIRED_ISO'   ).setLogic( d.HA40  & unpaired_isocond ).setTriggerType( TT.calo )

        #Phase-I
        MenuItem('L1_eTAU8'   ).setLogic( d.eTAU8    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU12'  ).setLogic( d.eTAU12   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU12L' ).setLogic( d.eTAU12L  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU12M' ).setLogic( d.eTAU12M  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_jTAU12'  ).setLogic( d.jTAU12   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_jTAU20'  ).setLogic( d.jTAU20   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_jTAU20M' ).setLogic( d.jTAU20M  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_cTAU12M' ).setLogic( d.cTAU12M  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU20'  ).setLogic( d.eTAU20   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_cTAU20M' ).setLogic( d.cTAU20M  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU25'  ).setLogic( d.eTAU25   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_cTAU25M' ).setLogic( d.cTAU25M  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU30HM').setLogic( d.eTAU30HM & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU40'  ).setLogic( d.eTAU40   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU60'  ).setLogic( d.eTAU60   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_eTAU100' ).setLogic( d.eTAU100  & physcond).setTriggerType( TT.calo )
        # Phase-I 2xTAU
        MenuItem('L1_eTAU60_2eTAU40').setLogic(d.eTAU60 & d.eTAU40.x(2) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_cTAU20M_2cTAU12M_4jJ12p0ETA25').setLogic(d.cTAU20M & d.cTAU12M.x(2) & d.jJ120ETA25.x(4) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_cTAU25M_2cTAU20M_2jJ25_3jJ20').setLogic(d.cTAU25M & d.cTAU20M.x(2) & d.jJ25.x(2) & d.jJ20.x(3) & physcond).setTriggerType(TT.calo)

        #UPC TAU
        MenuItem('L1_2TAU1_VTE50' ).setLogic( d.HA1.x(2)      & Not(d.TE50) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2TAU2_VTE50' ).setLogic( d.HA2.x(2)      & Not(d.TE50) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2TAU3_VTE50' ).setLogic( d.HA3.x(2)      & Not(d.TE50) & physcond).setTriggerType(TT.calo)
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

        MenuItem('L1_EM15VHI_2TAU12'               ).setLogic( d.EM15VHI & d.HA12.x(2)  	& physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IL'             ).setLogic( d.EM15VHI & d.HA12IL.x(2)	& physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IM'             ).setLogic( d.EM15VHI & d.HA12IM.x(2)	& physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_TAU40_2TAU15'         ).setLogic( d.EM15VHI & d.HA40 & d.HA15.x(2)   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12I_J25_2J15_3J12').setLogic( d.EM15VHI & d.HA12I.x(2)  & d.J25 & d.J15.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IM_J25_3J12'    ).setLogic( d.EM15VHI & d.HA12IM.x(2)  & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IL_J25_3J12'    ).setLogic( d.EM15VHI & d.HA12IL.x(2)  & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12_J25_3J12'      ).setLogic( d.EM15VHI & d.HA12.x(2)  & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IM_4J12'        ).setLogic( d.EM15VHI & d.HA12IM.x(2) & d.J12.x(4) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12'                ).setLogic( d.EM15HI  & d.HA12.x(2)          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12IL'              ).setLogic( d.EM15HI  & d.HA12IL.x(2)        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12IM'              ).setLogic( d.EM15HI  & d.HA12IM.x(2)        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_TAU40_2TAU15'          ).setLogic( d.EM15HI  & d.HA40 & d.HA15.x(2)   & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12I_J25_2J15_3J12' ).setLogic( d.EM15HI  & d.HA12I.x(2)  & d.J25 & d.J15.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12IM_J25_3J12'     ).setLogic( d.EM15HI  & d.HA12IM.x(2)  & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12IL_J25_3J12'     ).setLogic( d.EM15HI  & d.HA12IL.x(2)  & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12_J25_3J12'       ).setLogic( d.EM15HI  & d.HA12.x(2)  & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_MU8F_TAU12'       ).setLogic( d.MU8F  & d.HA12          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU8F_TAU12IM'     ).setLogic( d.MU8F  & d.HA12IM        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU8F_TAU20'       ).setLogic( d.MU8F  & d.HA20          & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU8F_TAU20IM'     ).setLogic( d.MU8F  & d.HA20IM        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU8VF_TAU20IM'    ).setLogic( d.MU8VF & d.HA20IM        & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU8F_TAU20IM_J25_2J20').setLogic( d.MU8F    & d.HA20IM   & d.J25 & d.J20.x(2) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU14FCH_TAU12IM'      ).setLogic( d.MU14FCH & d.HA12IM & physcond).setTriggerType( TT.calo)
        MenuItem('L1_MU8F_TAU12_J25_2J12'  ).setLogic( d.MU8F & d.HA12 & d.J25 & d.J12.x(2)     & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU8F_TAU12IM_J25_2J12').setLogic( d.MU8F & d.HA12IM & d.J25 & d.J12.x(2)    & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU8F_TAU12IM_3J12'    ).setLogic( d.MU8F & d.HA12IM & d.J12.x(3)    & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU20_2TAU12I_J25_2J15_3J12'   ).setLogic( d.HA20 & d.HA12I.x(2)     & d.J25 & d.J15.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20_2TAU12_J25_2J20_3J12'    ).setLogic( d.HA20 & d.HA12.x(2)     & d.J25 & d.J20.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2TAU12IM_J25_2J20_3J12').setLogic( d.HA20IM & d.HA12IM.x(2)     & d.J25 & d.J20.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IL_2TAU12IL_J25_2J20_3J12').setLogic( d.HA20IL & d.HA12IL.x(2)     & d.J25 & d.J20.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU25IT_2TAU12IT_2J25_3J12').setLogic( d.HA25IT & d.HA12IT.x(2)     & d.J25.x(2)  & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU25IM_2TAU12IM_J25_3J12' ).setLogic( d.HA25IM & d.HA12IM.x(2) & d.J25 & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU25IM_2TAU20IM_2J25_3J20').setLogic( d.HA25IM & d.HA20IM.x(2)     & d.J25.x(2)  & d.J20.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2TAU12IM_4J12'     ).setLogic( d.HA20IM & d.HA12IM.x(2)  & d.J12.x(4) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2TAU12IM_4J12p0ETA25').setLogic( d.HA20IM & d.HA12IM.x(2)  & d.J120ETA25.x(4) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2TAU12IM_4J12p0ETA28').setLogic( d.HA20IM & d.HA12IM.x(2)  & d.J120ETA28.x(4) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU60_2TAU40'                ).setLogic( d.HA60 & d.HA40.x(2)  & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2TAU12I_TAU20_J25_2J15_3J12' ).setLogic( d.HA12I.x(2)   & d.HA20  & d.J25 & d.J15.x(2) & d.J12.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2TAU20IM_3J20'  ).setLogic(  d.HA20IM.x(2) & d.J20.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_2TAU20IM_J25_3J20'  ).setLogic(  d.HA20IM.x(2) & d.J25 & d.J20.x(3) & physcond).setTriggerType( TT.calo )

        MenuItem('L1_TAU20_2J20_XE45'              ).setLogic( d.HA20    & d.J20.x(2)   & d.XE45 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2J20_XE50'            ).setLogic( d.HA20IM   & d.J20.x(2)   & d.XE50 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2J20_XE45'            ).setLogic( d.HA20IM    & d.J20.x(2)   & d.XE45 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU25_2J20_XE45'              ).setLogic( d.HA25      & d.J20.x(2)   & d.XE45 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20_2TAU12_XE35'            ).setLogic( d.HA20    & d.HA12.x(2)  & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2TAU12IM_XE35'        ).setLogic( d.HA20IM  & d.HA12IM.x(2)  & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IL_2TAU12IL_XE35'        ).setLogic( d.HA20IL  & d.HA12IL.x(2)  & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IT_2TAU12IT_XE35'        ).setLogic( d.HA20IT  & d.HA12IT.x(2)  & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20IM_2TAU12IM_XE40'        ).setLogic( d.HA20IM  & d.HA12IM.x(2)  & d.XE40 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU40_2TAU12IM_XE40'          ).setLogic( d.HA40  & d.HA12IM.x(2)  & d.XE40 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM20VHI_TAU20IM_2TAU20_J25_3J20').setLogic( d.EM20VHI  &  d.HA20IM  &  d.HA20.x(2) &  d.J25  & d.J20.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU8F_TAU12I_XE35'             ).setLogic( d.MU8F    & d.HA12I      & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU8F_TAU12_XE35'              ).setLogic( d.MU8F    & d.HA12       & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU8F_TAU12IL_XE35'            ).setLogic( d.MU8F    & d.HA12IL     & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU8F_TAU12IM_XE35'            ).setLogic( d.MU8F    & d.HA12IM     & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU8F_TAU12IT_XE35'            ).setLogic( d.MU8F    & d.HA12IT     & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_MU8F_TAU12IM_XE40'            ).setLogic( d.MU8F    & d.HA12IM     & d.XE40 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_TAU20I_2TAU12I_XE35'          ).setLogic( d.HA20I   & d.HA12I.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )

        MenuItem('L1_EM15VHI_TAU20IM_2TAU15_J25_2J20_3J15').setLogic( d.EM15VHI  &  d.HA20IM  &  d.HA15.x(2) &  d.J25  & d.J20.x(2) & d.J15.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12I_XE35'          ).setLogic( d.EM15VHI  & d.HA12I.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IM_XE35'         ).setLogic( d.EM15VHI  & d.HA12IM.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12IL_XE35'         ).setLogic( d.EM15VHI  & d.HA12IL.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15VHI_2TAU12_XE35'           ).setLogic( d.EM15VHI  & d.HA12.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )

        MenuItem('L1_EM15HI_TAU20IM_2TAU15_J25_2J20_3J15').setLogic( d.EM15HI  &  d.HA20IM  &  d.HA15.x(2) &  d.J25  & d.J20.x(2) & d.J15.x(3) & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12I_XE35'          ).setLogic( d.EM15HI  & d.HA12I.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12IM_XE35'         ).setLogic( d.EM15HI  & d.HA12IM.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12IL_XE35'         ).setLogic( d.EM15HI  & d.HA12IL.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )
        MenuItem('L1_EM15HI_2TAU12_XE35'           ).setLogic( d.EM15HI  & d.HA12.x(2) & d.XE35 & physcond).setTriggerType( TT.calo )

        # JET ctpid=[0x60:0x7f]
        MenuItem('L1_J5'   ).setLogic( d.J5   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J10'  ).setLogic( d.J10  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J12'  ).setLogic( d.J12  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J15'  ).setLogic( d.J15  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J20'  ).setLogic( d.J20  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J25'  ).setLogic( d.J25  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J30'  ).setLogic( d.J30  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J35'  ).setLogic( d.J35  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40'  ).setLogic( d.J40  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J50'  ).setLogic( d.J50  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J60'  ).setLogic( d.J60  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75'  ).setLogic( d.J75  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75A' ).setLogic( d.J75A & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75C' ).setLogic( d.J75C & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J85'  ).setLogic( d.J85  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J100' ).setLogic( d.J100 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J120' ).setLogic( d.J120 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J150' ).setLogic( d.J150 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J175' ).setLogic( d.J175 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J250' ).setLogic( d.J250 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J400' ).setLogic( d.J400 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J400_LAR' ).setLogic( d.J400 & physcond).setTriggerType(TT.lardemo) # ATR-22344

        MenuItem('L1_jJ12'          ).setLogic( d.jJ12         & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ12p0ETA25'   ).setLogic( d.jJ120ETA25   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ15'          ).setLogic( d.jJ15         & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ15p0ETA25'   ).setLogic( d.jJ150ETA25   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ15p31ETA49'  ).setLogic( d.jJ1531ETA49  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ20'          ).setLogic( d.jJ20         & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ20p31ETA49'  ).setLogic( d.jJ2031ETA49  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ25'          ).setLogic( d.jJ25         & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ25p0ETA23'   ).setLogic( d.jJ250ETA23   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ30'          ).setLogic( d.jJ30         & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ30p31ETA49'  ).setLogic( d.jJ3031ETA49  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ35p0ETA23'   ).setLogic( d.jJ350ETA23   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ40'          ).setLogic( d.jJ40         & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ40p0ETA25'   ).setLogic( d.jJ400ETA25   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ45p0ETA21'   ).setLogic( d.jJ450ETA21   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ50'          ).setLogic( d.jJ50         & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ50p31ETA49'  ).setLogic( d.jJ5031ETA49  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ75'          ).setLogic( d.jJ75         & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ75p31ETA49'  ).setLogic( d.jJ7531ETA49  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ85'          ).setLogic( d.jJ85         & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ100'         ).setLogic( d.jJ100        & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ120'         ).setLogic( d.jJ120        & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ400'         ).setLogic( d.jJ400        & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ400_LAR'     ).setLogic( d.jJ400 & physcond).setTriggerType(TT.lardemo) # ATR-22344

        MenuItem('L1_4jJ15'         ).setLogic( d.jJ15.x(4)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4jJ15p0ETA25'  ).setLogic( d.jJ150ETA25.x(4) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_5jJ15p0ETA25'  ).setLogic( d.jJ150ETA25.x(5) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4jJ20'         ).setLogic( d.jJ20.x(4)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3jJ50'         ).setLogic( d.jJ50.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3jJ35p0ETA23'  ).setLogic( d.jJ350ETA23.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ85_3jJ30'    ).setLogic( d.jJ85 & d.jJ30.x(3) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ40p0ETA25_2jJ25_jJ20p31ETA49' ).setLogic( d.jJ400ETA25 & d.jJ25.x(2) & d.jJ2031ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ25p0ETA23_2jJ15p31ETA49'      ).setLogic( d.jJ250ETA23 & d.jJ1531ETA49.x(2)            & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jJ45p0ETA21_3jJ15p0ETA25'       ).setLogic( d.jJ450ETA21 & d.jJ150ETA25.x(3)             & physcond).setTriggerType(TT.calo)

        MenuItem('L1_jJ30_EMPTY'     ).setLogic( d.jJ30 & cosmiccond ).setTriggerType(TT.calo)
        MenuItem('L1_jJ30_FIRSTEMPTY').setLogic( d.jJ30 & firstempty ).setTriggerType(TT.calo)

        MenuItem('L1_MU3V_jJ15'      ).setLogic( d.MU3V & d.jJ15    & physcond).setTriggerType(TT.calo)

        MenuItem('L1_jLJ80'         ).setLogic( d.jLJ80        & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jLJ100'        ).setLogic( d.jLJ100       & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jLJ140'        ).setLogic( d.jLJ140       & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jLJ160'        ).setLogic( d.jLJ160       & physcond).setTriggerType(TT.calo)

        MenuItem('L1_jEM15'         ).setLogic( d.jEM15        & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jEM15M'        ).setLogic( d.jEM15M       & physcond).setTriggerType(TT.calo)

        MenuItem('L1_J10p31ETA49').setLogic( d.J1031ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75p31ETA49').setLogic( d.J7531ETA49 & physcond).setTriggerType(TT.calo)


        MenuItem('L1_J10_UNPAIRED_ISO'   ).setLogic( d.J10 & unpaired_isocond   ).setTriggerType(TT.calo)
        MenuItem('L1_J10_UNPAIRED_NONISO').setLogic( d.J10 & unpaired_nonisocond).setTriggerType(TT.calo)
        MenuItem('L1_J10_EMPTY'          ).setLogic( d.J10 & cosmiccond ).setTriggerType(TT.calo)
        MenuItem('L1_J10_FIRSTEMPTY'     ).setLogic( d.J10 & firstempty ).setTriggerType(TT.calo)

        MenuItem('L1_J12_UNPAIRED_ISO'    ).setLogic( d.J12 & unpaired_isocond   ).setTriggerType(TT.calo)
        MenuItem('L1_J12_UNPAIRED_NONISO' ).setLogic( d.J12 & unpaired_nonisocond).setTriggerType(TT.calo)
        MenuItem('L1_J12_EMPTY'           ).setLogic( d.J12 & cosmiccond ).setTriggerType(TT.calo)
        MenuItem('L1_J12_FIRSTEMPTY'      ).setLogic( d.J12 & firstempty ).setTriggerType(TT.calo)
        MenuItem('L1_J12_ABORTGAPNOTCALIB').setLogic( d.J12 & abortgap ).setTriggerType(TT.calo)
        MenuItem('L1_J12_UNPAIREDB1'      ).setLogic( d.J12 & bgrp13cond  ).setTriggerType(TT.calo)
        MenuItem('L1_J12_UNPAIREDB2'      ).setLogic( d.J12 & bgrp14cond).setTriggerType(TT.calo)

        MenuItem('L1_J50_UNPAIRED_ISO'    ).setLogic( d.J50 & unpaired_isocond   ).setTriggerType(TT.calo)
        MenuItem('L1_J50_UNPAIRED_NONISO' ).setLogic( d.J50 & unpaired_nonisocond).setTriggerType(TT.calo)
        MenuItem('L1_J50_ABORTGAPNOTCALIB').setLogic( d.J50 & abortgap ).setTriggerType(TT.calo)

        MenuItem('L1_J12_BGRP12'        ).setLogic( d.J12 & bgrp12cond ).setTriggerType(TT.calo)
        MenuItem('L1_J30p31ETA49_BGRP12').setLogic( d.J3031ETA49 & bgrp12cond ).setTriggerType(TT.calo)

        MenuItem('L1_J30_EMPTY'     ).setLogic( d.J30 & cosmiccond ).setTriggerType(TT.calo)
        MenuItem('L1_J30_FIRSTEMPTY').setLogic( d.J30 & firstempty ).setTriggerType(TT.calo)

        MenuItem('L1_J10p31ETA49_EMPTY').setLogic( d.J1031ETA49 & cosmiccond ).setTriggerType(TT.calo)
        MenuItem('L1_J15p31ETA49_UNPAIRED_ISO').setLogic( d.J1531ETA49 & unpaired_isocond).setTriggerType(TT.calo)

        MenuItem('L1_J30p31ETA49_EMPTY'          ).setLogic( d.J3031ETA49 & cosmiccond ).setTriggerType(TT.calo)
        MenuItem('L1_J30p31ETA49_FIRSTEMPTY'     ).setLogic( d.J3031ETA49 & firstempty ).setTriggerType(TT.calo)
        MenuItem('L1_J30p31ETA49_UNPAIRED_ISO'   ).setLogic( d.J3031ETA49 & unpaired_isocond   ).setTriggerType(TT.calo)
        MenuItem('L1_J30p31ETA49_UNPAIRED_NONISO').setLogic( d.J3031ETA49 & unpaired_nonisocond   ).setTriggerType(TT.calo)


        MenuItem('L1_J100_FIRSTEMPTY').setLogic( d.J100 & firstempty ).setTriggerType(TT.calo)

        # multi jet
        MenuItem('L1_2J25p31ETA49'  ).setLogic( d.J2531ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2J15'   ).setLogic( d.J15.x(2)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J15_J15p31ETA49' ).setLogic( d.J15 & d.J1531ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J20_J20p31ETA49' ).setLogic( d.J20 & d.J2031ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J10'   ).setLogic( d.J10.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J15'   ).setLogic( d.J15.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J20'   ).setLogic( d.J20.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J40'   ).setLogic( d.J40.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J50'   ).setLogic( d.J50.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J75'   ).setLogic( d.J75.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4J10'   ).setLogic( d.J10.x(4)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4J15'   ).setLogic( d.J15.x(4)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4J20'   ).setLogic( d.J20.x(4)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4J20p0ETA49').setLogic( d.J200ETA49.x(4) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4J30'   ).setLogic( d.J30.x(4)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_6J15'   ).setLogic( d.J15.x(6)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75_3J20' ).setLogic( d.J75 & d.J20.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J85_3J30' ).setLogic( d.J85 & d.J30.x(3)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J30p0ETA49_2J20p0ETA49'      ).setLogic( d.J300ETA49 & d.J200ETA49.x(2)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J15p0ETA25_2J15p31ETA49'     ).setLogic( d.J150ETA25 & d.J1531ETA49.x(2)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40p0ETA25_2J15p31ETA49'     ).setLogic( d.J400ETA25 & d.J1531ETA49.x(2)    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40p0ETA25_2J25_J20p31ETA49' ).setLogic( d.J400ETA25 & d.J25.x(2) & d.J2031ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40p0ETA25_2J30_J20p31ETA49' ).setLogic( d.J400ETA25 & d.J30.x(2) & d.J2031ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J45p0ETA21_3J15p0ETA25'      ).setLogic( d.J450ETA21 & d.J150ETA25.x(3) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J50_2J40p0ETA25_3J15p0ETA25' ).setLogic( d.J50 & d.J400ETA25.x(2) & d.J150ETA25.x(3) & physcond).setTriggerType(TT.calo)

        MenuItem('L1_4J17p0ETA22' ).setLogic( d.J170ETA22.x(4) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J25p0ETA23' ).setLogic( d.J250ETA23.x(3) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J35p0ETA23' ).setLogic( d.J350ETA23.x(3) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J15p0ETA25' ).setLogic( d.J150ETA25.x(3) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_4J15p0ETA25' ).setLogic( d.J150ETA25.x(4) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_5J15p0ETA25' ).setLogic( d.J150ETA25.x(5) & physcond).setTriggerType(TT.calo)

        if ('Physics_HI_v' in menuName or 'MC_HI_v' in menuName):
            MenuItem('L1_ZB', ctpid=240).setLogic(d.ZB_J75  & physcond).setTriggerType(TT.zerobs)
        else:
            MenuItem('L1_ZB', ctpid=240).setLogic(d.ZB_EM15 & physcond).setTriggerType(TT.zerobs)


        # combined jet - xe
        MenuItem('L1_J40_XE50').setLogic( d.J40 & d.XE50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40p0ETA25_XE50').setLogic( d.J400ETA25 & d.XE50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75_XE40' ).setLogic( d.J75 & d.XE40 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J75_XE50' ).setLogic( d.J75 & d.XE50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2J15_XE55').setLogic( d.J15.x(2) & d.XE55 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2J40_XE45').setLogic( d.J40.x(2) & d.XE45 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2J50_XE40').setLogic( d.J50.x(2) & d.XE50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J40_XE60' ).setLogic( d.J40 & d.XE60 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_J30p0ETA49_XE50').setLogic( d.J300ETA49 & d.XE50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_3J15p0ETA25_XE40').setLogic( d.J150ETA25.x(3) & d.XE40 & physcond).setTriggerType(TT.calo)

        # combined em - jet
        MenuItem('L1_EM18VHI_3J20' ).setLogic( d.EM18VHI  & d.J20.x(3)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_EM20VH_3J20' ).setLogic( d.EM20VH  & d.J20.x(3)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_EM13VH_3J20' ).setLogic( d.EM13VH  & d.J20.x(3)  & physcond).setTriggerType(TT.calo)
        ### ATR-15524
        MenuItem('L1_EM18VH_3J20' ).setLogic( d.EM18VH  & d.J20.x(3)  & physcond).setTriggerType(TT.calo)


        # combined mu - jet
        MenuItem('L1_MU3V_J12'   ).setLogic( d.MU3V & d.J12    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU3V_J15'   ).setLogic( d.MU3V & d.J15    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU5VF_J20'  ).setLogic( d.MU5VF & d.J20    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU5VF_J30p0ETA49_2J20p0ETA49').setLogic( d.MU5VF & d.J300ETA49 & d.J200ETA49.x(2) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU5VF_J40'  ).setLogic( d.MU5VF & d.J40    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU5VF_J75'  ).setLogic( d.MU5VF & d.J75    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU3V_3J15'  ).setLogic( d.MU3V & d.J15.x(3)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU3V_3J20'  ).setLogic( d.MU3V & d.J20.x(3)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU3V_J30'   ).setLogic( d.MU3V & d.J30    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU3V_J50'   ).setLogic( d.MU3V & d.J50    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU8F_3J20'    ).setLogic( d.MU8F & d.J20.x(3)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU8F_2J20'    ).setLogic( d.MU8F & d.J20.x(2)  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU8F_2J15_J20').setLogic( d.MU8F & d.J15.x(2) & d.J20  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU14FCH_J50'  ).setLogic( d.MU14FCH & d.J50  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU3V_XE60'      ).setLogic( d.MU3V & d.XE60  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2MU3V_XE60'     ).setLogic( d.MU3V.x(2) & d.XE60  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2MU3V_J40_XE50' ).setLogic( d.MU3V.x(2) & d.J40 & d.XE50  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU3V_J50_XE40'  ).setLogic( d.MU3V & d.J50 & d.XE40  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_2MU3V_J40_XE20' ).setLogic( d.MU3V.x(2) & d.J40 & d.XE20  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_MU14FCH_J40'               ).setLogic( d.MU14FCH & d.J40 & physcond).setTriggerType(TT.calo) ## ATR-14377
        MenuItem('L1_MU14FCH_XE30'              ).setLogic( d.MU14FCH & d.XE30 & physcond).setTriggerType(TT.calo) ## ATR-14377

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
        # phase1
        MenuItem('L1_gXERHO30').setLogic( d.gXERHO30 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_gXERHO50').setLogic( d.gXERHO50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_gXENC30').setLogic( d.gXENC30 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_gXENC50').setLogic( d.gXENC50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_gXEJWOJ30').setLogic( d.gXEJWOJ30 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_gXEJWOJ40').setLogic( d.gXEJWOJ40 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_gXEJWOJ50').setLogic( d.gXEJWOJ50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_gMHT500').setLogic( d.gMHT500 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jXE30' ).setLogic( d.jXE30 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jXE40' ).setLogic( d.jXE40 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jXE50' ).setLogic( d.jXE50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jXE55' ).setLogic( d.jXE55 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jXE300').setLogic( d.jXE300 & physcond).setTriggerType(TT.calo)

        MenuItem('L1_jXEC50' ).setLogic( d.jXEC50 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jXEPerf50' ).setLogic( d.jXEPerf50 & physcond).setTriggerType(TT.calo)

        # phase1 TE
        MenuItem('L1_gTE200'     ).setLogic( d.gTE200 & physcond).setTriggerType(TT.calo)

        MenuItem('L1_jTE200'     ).setLogic( d.jTE200 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jTEC200'    ).setLogic( d.jTEC200 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jTEFWD100'  ).setLogic( d.jTEFWD100 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jTEFWDA100' ).setLogic( d.jTEFWDA100 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_jTEFWDC100' ).setLogic( d.jTEFWDC100 & physcond).setTriggerType(TT.calo)

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

        MenuItem('L1_TE0p24ETA49'   ).setLogic( d.TE024ETA49    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE3p24ETA49'   ).setLogic( d.TE324ETA49    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE5p24ETA49'   ).setLogic( d.TE524ETA49    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE10p24ETA49'  ).setLogic( d.TE1024ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE15p24ETA49'  ).setLogic( d.TE1524ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE20p24ETA49'  ).setLogic( d.TE2024ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE25p24ETA49'  ).setLogic( d.TE2524ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE30p24ETA49'  ).setLogic( d.TE3024ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE35p24ETA49'  ).setLogic( d.TE3524ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE40p24ETA49'  ).setLogic( d.TE4024ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE45p24ETA49'  ).setLogic( d.TE4524ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE50p24ETA49'  ).setLogic( d.TE5024ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE55p24ETA49'  ).setLogic( d.TE5524ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE60p24ETA49'  ).setLogic( d.TE6024ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE65p24ETA49'  ).setLogic( d.TE6524ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE70p24ETA49'  ).setLogic( d.TE7024ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE80p24ETA49'  ).setLogic( d.TE8024ETA49   & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE110p24ETA49' ).setLogic( d.TE11024ETA49  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE150p24ETA49' ).setLogic( d.TE15024ETA49  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE180p24ETA49' ).setLogic( d.TE18024ETA49  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE2000p24ETA49').setLogic( d.TE200024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE5000p24ETA49').setLogic( d.TE200024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE6500p24ETA49').setLogic( d.TE200024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE8000p24ETA49').setLogic( d.TE200024ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE9000p24ETA49').setLogic( d.TE200024ETA49 & physcond).setTriggerType(TT.calo)

        # HI items
        MenuItem('L1_TE3p0ETA49'   ).setLogic( d.TE30ETA49    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE7p0ETA49'   ).setLogic( d.TE70ETA49    & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE500p0ETA49' ).setLogic( d.TE5000ETA49  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE600p0ETA49' ).setLogic( d.TE6000ETA49  & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE1500p0ETA49').setLogic( d.TE15000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE3000p0ETA49').setLogic( d.TE30000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE3500p0ETA49').setLogic( d.TE35000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE5000p0ETA49').setLogic( d.TE50000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE6500p0ETA49').setLogic( d.TE65000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE8000p0ETA49').setLogic( d.TE80000ETA49 & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE9000p0ETA49').setLogic( d.TE90000ETA49 & physcond).setTriggerType(TT.calo)

        MenuItem('L1_TE500p0ETA49_OVERLAY' ).setLogic( d.TE5000ETA49  & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE600p0ETA49_OVERLAY' ).setLogic( d.TE6000ETA49  & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE1500p0ETA49_OVERLAY').setLogic( d.TE15000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE3000p0ETA49_OVERLAY').setLogic( d.TE30000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE3500p0ETA49_OVERLAY').setLogic( d.TE35000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE5000p0ETA49_OVERLAY').setLogic( d.TE50000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE6500p0ETA49_OVERLAY').setLogic( d.TE65000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE8000p0ETA49_OVERLAY').setLogic( d.TE80000ETA49 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE9000p0ETA49_OVERLAY').setLogic( d.TE90000ETA49 & physcond).setTriggerType(TT.zerobs)

        MenuItem('L1_TE50_VTE600p0ETA49'    ).setLogic( d.TE50 & Not(d.TE6000ETA49) & physcond).setTriggerType(TT.calo)
        MenuItem('L1_TE50_VTE600p0ETA49_PEB').setLogic( d.TE50 & Not(d.TE6000ETA49) & physcond).setTriggerType(TT.ftk)
        MenuItem('L1_TE600p0ETA49_PEB'      ).setLogic( d.TE6000ETA49 & physcond).setTriggerType(TT.calo)

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
        MBTS_1_A = d.MBTS_A.x(1)
        MBTS_1_C = d.MBTS_C.x(1)
        MBTS_2_A = d.MBTS_A.x(2)
        MBTS_2_C = d.MBTS_C.x(2)
        MBTS_3_3 = d.MBTS_A.x(3) & d.MBTS_C.x(3)
        MBTS_4_4 = d.MBTS_A.x(4) & d.MBTS_C.x(4)
        MBTS_4_A = d.MBTS_A.x(4)
        MBTS_4_C = d.MBTS_C.x(4)


        MenuItem('L1_MBTS_A'             ).setLogic( d.MBTS_A   & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_C'             ).setLogic( d.MBTS_C   & physcond ).setTriggerType(TT.minb)
        
        MenuItem('L1_MBTS_1'             ).setLogic( MBTS_1   & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_OVERLAY'     ).setLogic( MBTS_1   & physcond ).setTriggerType(TT.zerobs)
        MenuItem('L1_MBTS_2'             ).setLogic( MBTS_2   & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_1'           ).setLogic( MBTS_1_1 & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_1_OVERLAY'   ).setLogic( MBTS_1_1 & physcond ).setTriggerType(TT.zerobs)
        MenuItem('L1_MBTS_2_2'           ).setLogic( MBTS_2_2 & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_A'           ).setLogic( MBTS_2_A & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_C'           ).setLogic( MBTS_2_C & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_3_3'           ).setLogic( MBTS_3_3 & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_4'           ).setLogic( MBTS_4_4 & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_A'           ).setLogic( MBTS_4_A & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_C'           ).setLogic( MBTS_4_C & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_A'           ).setLogic( MBTS_1_A & physcond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_C'           ).setLogic( MBTS_1_C & physcond ).setTriggerType(TT.minb)

        MenuItem('L1_MBTS_1_UNPAIRED_ISO'   ).setLogic( MBTS_1   & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_UNPAIRED_ISO'   ).setLogic( MBTS_2   & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_1_UNPAIRED_ISO' ).setLogic( MBTS_1_1 & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_2_UNPAIRED_ISO' ).setLogic( MBTS_2_2 & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_3_3_UNPAIRED_ISO' ).setLogic( MBTS_3_3 & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_4_UNPAIRED_ISO' ).setLogic( MBTS_3_3 & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_UNPAIRED_NONISO').setLogic( MBTS_1   & unpaired_nonisocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_UNPAIRED_NONISO').setLogic( MBTS_2   & unpaired_nonisocond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_A_UNPAIRED_ISO' ).setLogic( MBTS_4_A & unpaired_isocond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_C_UNPAIRED_ISO' ).setLogic( MBTS_4_C & unpaired_isocond ).setTriggerType(TT.minb)

        MenuItem('L1_MBTS_1_A_EMPTY'    ).setLogic( MBTS_1_A   & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_C_EMPTY'    ).setLogic( MBTS_1_C   & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_EMPTY'    ).setLogic( MBTS_1   & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_EMPTY'    ).setLogic( MBTS_2   & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_1_EMPTY'  ).setLogic( MBTS_1_1 & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_2_EMPTY'  ).setLogic( MBTS_2_2 & cosmiccond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_3_3_EMPTY'  ).setLogic( MBTS_3_3 & cosmiccond ).setTriggerType(TT.minb)

        MenuItem('L1_MBTS_1_1_VTE50'  ).setLogic( MBTS_1_1  & Not(d.TE50) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_2_VTE50'  ).setLogic( MBTS_2_2  & Not(d.TE50) & physcond).setTriggerType(TT.minb)

        MenuItem('L1_MBTS_1_VTE2'     ).setLogic( MBTS_1  & Not(d.TE2) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_VTE3'     ).setLogic( MBTS_1  & Not(d.TE3) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_VTE4'     ).setLogic( MBTS_1  & Not(d.TE4) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_VTE5'     ).setLogic( MBTS_1  & Not(d.TE5) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_VTE10'    ).setLogic( MBTS_1  & Not(d.TE10) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_VTE70'    ).setLogic( MBTS_1  & Not(d.TE70) & physcond).setTriggerType(TT.minb)

        MenuItem('L1_MBTS_2_VTE2'     ).setLogic( MBTS_2  & Not(d.TE2) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_VTE3'     ).setLogic( MBTS_2  & Not(d.TE3) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_VTE4'     ).setLogic( MBTS_2  & Not(d.TE4) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_VTE5'     ).setLogic( MBTS_2  & Not(d.TE5) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_VTE10'    ).setLogic( MBTS_2  & Not(d.TE10) & physcond).setTriggerType(TT.minb)

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

        MenuItem('L1_MBTS_1_BGRP9'   ).setLogic( MBTS_1 & bgrp9cond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_1_BGRP9' ).setLogic( MBTS_1_1 & bgrp9cond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_BGRP9'   ).setLogic( MBTS_2 & bgrp9cond ).setTriggerType(TT.minb)

        MenuItem('L1_MBTS_1_BGRP11'  ).setLogic( MBTS_1 & bgrp11cond ).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_1_BGRP11').setLogic( MBTS_1_1 & bgrp11cond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_BGRP11'  ).setLogic( MBTS_2 & bgrp11cond ).setTriggerType(TT.minb)


        # ZDC
        ZDC_A_C = d.ZDC_A & d.ZDC_C
        VZDC_A_C = Not(d.ZDC_A) & Not(d.ZDC_C)
        VZDC_AORC = Not(d.ZDC_A) | Not(d.ZDC_C)
        MenuItem('L1_ZDC'             ).setLogic((d.ZDC_A | d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A'           ).setLogic( d.ZDC_A & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_C'           ).setLogic( d.ZDC_C & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_AND'         ).setLogic( d.ZDC_AND & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_C'         ).setLogic( ZDC_A_C & physcond).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_A_C_OVERLAY' ).setLogic( ZDC_A_C & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_ZDC_A_OVERLAY'   ).setLogic( d.ZDC_A & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_ZDC_C_OVERLAY'   ).setLogic( d.ZDC_C & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_ZDC_A_C_VTE50_OVERLAY').setLogic( ZDC_A_C & Not(d.TE50) & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE5_OVERLAY'     ).setLogic( d.TE5 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE10_OVERLAY'    ).setLogic( d.TE10 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE20_OVERLAY'    ).setLogic( d.TE20 & physcond).setTriggerType(TT.zerobs)
        MenuItem('L1_TE50_OVERLAY'    ).setLogic( d.TE50 & physcond).setTriggerType(TT.zerobs)

        MenuItem('L1_ZDC_VTE200'      ).setLogic((d.ZDC_A | d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_AND_VTE50'   ).setLogic( d.ZDC_AND & Not(d.TE50) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_C_VTE50'   ).setLogic( ZDC_A_C & Not(d.TE50) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_C_TE50'    ).setLogic( ZDC_A_C & d.TE50 & physcond).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_A_C_VTE50_PEB').setLogic( ZDC_A_C & Not(d.TE50) & physcond).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_A_VTE20'      ).setLogic( d.ZDC_A & Not(d.TE20) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_C_VTE20'      ).setLogic( d.ZDC_C & Not(d.TE20) & physcond).setTriggerType(TT.minb)

        MenuItem('L1_ZDC_A_C_VTE200'    ).setLogic( ZDC_A_C & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_C_TE5_VTE200').setLogic( ZDC_A_C & d.TE5 & Not(d.TE200) & physcond).setTriggerType(TT.minb)

        MenuItem('L1_VZDC_A_C_VTE50'      ).setLogic( VZDC_A_C & Not(d.TE50) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_VZDC_A_C_TE5_VTE200' ).setLogic( VZDC_A_C & d.TE5 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_VZDC_A_C_TE20_VTE200').setLogic( VZDC_A_C & d.TE20 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_VZDC_AORC_TE5_VTE200').setLogic( VZDC_AORC & d.TE5 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_VZDC_A_VZDC_C_TE5_VTE200').setLogic( Not(d.ZDC_A) & Not(d.ZDC_C) & d.TE5 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE20_ZDC_A_VZDC_C_VTE200').setLogic( d.ZDC_A & Not(d.ZDC_C) & d.TE20 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE20_ZDC_C_VZDC_A_VTE200').setLogic( d.ZDC_C & Not(d.ZDC_A) & d.TE20 & Not(d.TE200) & physcond).setTriggerType(TT.minb)

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
        MenuItem('L1_ZDC_A_VZDC_C'                  ).setLogic(d.ZDC_A & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_C_VZDC_A'                  ).setLogic(d.ZDC_C & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_A_VZDC_C_VTE200'           ).setLogic(d.ZDC_A & Not(d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_C_VZDC_A_VTE200'           ).setLogic(d.ZDC_C & Not(d.ZDC_A) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_ZDC_A_VZDC_C_VTE200'    ).setLogic(MBTS_1      & d.ZDC_A & Not(d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_1_ZDC_C_VZDC_A_VTE200'    ).setLogic(MBTS_1      & d.ZDC_C & Not(d.ZDC_A) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE3p0ETA49_ZDC_A_VZDC_C_VTE200').setLogic(d.TE30ETA49 & d.ZDC_A & Not(d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE3p0ETA49_ZDC_C_VZDC_A_VTE200').setLogic(d.TE30ETA49 & d.ZDC_C & Not(d.ZDC_A) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE4_ZDC_A_VZDC_C_VTE200'       ).setLogic(d.TE4       & d.ZDC_A & Not(d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE4_ZDC_C_VZDC_A_VTE200'       ).setLogic(d.TE4       & d.ZDC_C & Not(d.ZDC_A) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE5_ZDC_A_VZDC_C_VTE200'       ).setLogic(d.TE5       & d.ZDC_A & Not(d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE5_ZDC_C_VZDC_A_VTE200'       ).setLogic(d.TE5       & d.ZDC_C & Not(d.ZDC_A) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE7p0ETA49_ZDC_A_VZDC_C_VTE200').setLogic(d.TE70ETA49 & d.ZDC_A & Not(d.ZDC_C) & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE7p0ETA49_ZDC_C_VZDC_A_VTE200').setLogic(d.TE70ETA49 & d.ZDC_C & Not(d.ZDC_A) & Not(d.TE200) & physcond).setTriggerType(TT.minb)


        MenuItem('L1_ZDC_XOR'                  ).setLogic(ZDC_XOR & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_TE3p0ETA49_VTE200').setLogic(ZDC_XOR & d.TE30ETA49 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_TE4_VTE200'       ).setLogic(ZDC_XOR & d.TE4 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_TE5_VTE200'       ).setLogic(ZDC_XOR & d.TE5 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_TE20_VTE200'      ).setLogic(ZDC_XOR & d.TE20 & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_TRT_VTE200'       ).setLogic(ZDC_XOR & d.NIMTRT & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_VTE50'            ).setLogic(ZDC_XOR & Not(d.TE50) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_VTE200'           ).setLogic(ZDC_XOR & Not(d.TE200) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_ZDC_XOR_VTE200_MBTS_1'    ).setLogic(ZDC_XOR & Not(d.TE200) & MBTS_1 & physcond).setTriggerType(TT.minb)

        # ATR-14967
        MenuItem('L1_EM3_VZDC_A'           ).setLogic( d.EM3 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_EM3_VZDC_C'           ).setLogic( d.EM3 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_2EM3_VZDC_A'          ).setLogic( d.EM3.x(2) & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_2EM3_VZDC_C'          ).setLogic( d.EM3.x(2) & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_EM5_VZDC_A'           ).setLogic( d.EM5 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_EM5_VZDC_C'           ).setLogic( d.EM5 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE5_VZDC_A'           ).setLogic( d.TE5 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE5_VZDC_C'           ).setLogic( d.TE5 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE10_VZDC_A'          ).setLogic( d.TE10 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE10_VZDC_C'          ).setLogic( d.TE10 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE20_VZDC_A'          ).setLogic( d.TE20 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_TE20_VZDC_C'          ).setLogic( d.TE20 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_VTE10_VZDC_A'         ).setLogic( Not(d.TE10) & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_VTE10_VZDC_C'         ).setLogic( Not(d.TE10) & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J5_VZDC_A'            ).setLogic( d.J5  & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J5_VZDC_C'            ).setLogic( d.J5  & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J10_VZDC_A'           ).setLogic( d.J10 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J10_VZDC_C'           ).setLogic( d.J10 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J15_VZDC_A'           ).setLogic( d.J15 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J15_VZDC_C'           ).setLogic( d.J15 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J20_VZDC_A'           ).setLogic( d.J20 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_J20_VZDC_C'           ).setLogic( d.J20 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MU3V_VZDC_A'          ).setLogic( d.MU3V & Not(d.ZDC_A) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_VZDC_C'          ).setLogic( d.MU3V & Not(d.ZDC_C) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU3V_VZDC_A'         ).setLogic( d.MU3V.x(2) & Not(d.ZDC_A) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU3V_VZDC_C'         ).setLogic( d.MU3V.x(2) & Not(d.ZDC_C) & physcond).setTriggerType(TT.muon)

        MenuItem('L1_MU3V_VTE10_VZDC_A' ).setLogic( d.MU3V      & Not(d.TE10) & Not(d.ZDC_A) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_VTE10_VZDC_C' ).setLogic( d.MU3V      & Not(d.TE10) & Not(d.ZDC_C) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU3V_VTE10_VZDC_A').setLogic( d.MU3V.x(2) & Not(d.TE10) & Not(d.ZDC_A) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_2MU3V_VTE10_VZDC_C').setLogic( d.MU3V.x(2) & Not(d.TE10) & Not(d.ZDC_C) & physcond).setTriggerType(TT.muon)

        MenuItem('L1_MU3V_VZDC_A_C'         ).setLogic( d.MU3V  & VZDC_A_C & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU3V_VZDC_AORC_VTE200' ).setLogic( d.MU3V  & VZDC_AORC & Not(d.TE200) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU5VF_VZDC_A'          ).setLogic( d.MU5VF & Not(d.ZDC_A) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU5VF_VZDC_C'          ).setLogic( d.MU5VF & Not(d.ZDC_C) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU5VF_VTE10_VZDC_A'    ).setLogic( d.MU5VF & Not(d.TE10) & Not(d.ZDC_A) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU5VF_VTE10_VZDC_C'    ).setLogic( d.MU5VF & Not(d.TE10) & Not(d.ZDC_C) & physcond).setTriggerType(TT.muon)
        MenuItem('L1_MU5VF_VZDC_A_C'        ).setLogic( d.MU5VF & VZDC_A_C & physcond).setTriggerType(TT.muon)
        MenuItem('L1_RD0_FILLED_VZDC_A'    ).setLogic( d.RNDM0 & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_RD0_FILLED_VZDC_C'    ).setLogic( d.RNDM0 & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_C_VZDC_A'      ).setLogic( MBTS_4_C & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_4_A_VZDC_C'      ).setLogic( MBTS_4_A & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_C_VZDC_A'      ).setLogic( MBTS_2_C & Not(d.ZDC_A) & physcond).setTriggerType(TT.minb)
        MenuItem('L1_MBTS_2_A_VZDC_C'      ).setLogic( MBTS_2_A & Not(d.ZDC_C) & physcond).setTriggerType(TT.minb)




        # VDM
        MenuItem('L1_ZDC_A_C_BGRP7'     ).setLogic( ZDC_A_C & bgrp7cond).setTriggerType(TT.minb)
        MenuItem('L1_LUCID_BGRP7'       ).setLogic( (d.LUCID_A | d.LUCID_C) & bgrp7cond).setTriggerType(TT.minb)

        # LUCID
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

        MenuItem('L1_BCM_AC_UNPAIRED_ISO'        ).setLogic( d.BCM_AtoC & unpaired_isocond).setTriggerType(TT.minb)
        MenuItem('L1_BCM_CA_UNPAIRED_ISO'        ).setLogic( d.BCM_CtoA & unpaired_isocond).setTriggerType(TT.minb)

        MenuItem('L1_BCM_AC_UNPAIRED_NONISO'     ).setLogic( d.BCM_AtoC & unpaired_nonisocond).setTriggerType(TT.minb)
        MenuItem('L1_BCM_CA_UNPAIRED_NONISO'     ).setLogic( d.BCM_CtoA & unpaired_nonisocond).setTriggerType(TT.minb)

        MenuItem('L1_BCM_AC_ABORTGAPNOTCALIB'    ).setLogic( d.BCM_AtoC & abortgap).setTriggerType(TT.minb)
        MenuItem('L1_BCM_CA_ABORTGAPNOTCALIB'    ).setLogic( d.BCM_CtoA & abortgap).setTriggerType(TT.minb)
        MenuItem('L1_BCM_Wide_ABORTGAPNOTCALIB'  ).setLogic( d.BCM_Wide & abortgap).setTriggerType(TT.minb)

        MenuItem('L1_BCM_AC_CALIB'     ).setLogic( d.BCM_AtoC & calibcond).setTriggerType(TT.minb)
        MenuItem('L1_BCM_CA_CALIB'     ).setLogic( d.BCM_CtoA & calibcond).setTriggerType(TT.minb)
        MenuItem('L1_BCM_Wide_CALIB'   ).setLogic( d.BCM_Wide & calibcond).setTriggerType(TT.minb)

        MenuItem('L1_BCM_AC_UNPAIREDB1'  ).setLogic( d.BCM_AtoC & bgrp13cond).setTriggerType(TT.minb)
        MenuItem('L1_BCM_CA_UNPAIREDB2'  ).setLogic( d.BCM_CtoA & bgrp14cond).setTriggerType(TT.minb)


        # RANDOM
        MenuItem('L1_RD0_FILLED'         ).setLogic( d.RNDM0 & physcond           ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_EMPTY'          ).setLogic( d.RNDM0 & cosmiccond         ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_UNPAIRED_ISO'   ).setLogic( d.RNDM0 & unpaired_isocond   ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_UNPAIRED_NONISO').setLogic( d.RNDM0 & unpaired_nonisocond).setTriggerType(TT.rand)
        MenuItem('L1_RD0_FIRSTEMPTY'     ).setLogic( d.RNDM0 & firstempty         ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_BGRP0'          ).setLogic( d.RNDM0 & d.BGRP0            ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_BGRP7'          ).setLogic( d.RNDM0 & bgrp7cond          ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_BGRP9'          ).setLogic( d.RNDM0 & bgrp9cond          ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_BGRP10'         ).setLogic( d.RNDM0 & alfacalib          ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_BGRP11'         ).setLogic( d.RNDM0 & bgrp11cond         ).setTriggerType(TT.rand)
        MenuItem('L1_RD0_ABORTGAPNOTCALIB').setLogic( d.RNDM0 & abortgap          ).setTriggerType(TT.rand)

        MenuItem('L1_RD1_FILLED'         ).setLogic( d.RNDM1 & physcond           ).setTriggerType(TT.zerobs) # used to be TT.rand
        MenuItem('L1_RD1_EMPTY'          ).setLogic( d.RNDM1 & cosmiccond         ).setTriggerType(TT.zerobs)
        MenuItem('L1_RD1_BGRP10'         ).setLogic( d.RNDM1 & alfacalib          ).setTriggerType(TT.zerobs)

        MenuItem('L1_RD2_FILLED'         ).setLogic( d.RNDM2 & physcond           ).setTriggerType(TT.rand)
        MenuItem('L1_RD2_EMPTY'          ).setLogic( d.RNDM2 & cosmiccond         ).setTriggerType(TT.rand)
        MenuItem('L1_RD2_BGRP11'         ).setLogic( d.RNDM2 & bgrp11cond         ).setTriggerType(TT.rand)
        MenuItem('L1_RD2_BGRP12'         ).setLogic( d.RNDM2 & bgrp12cond         ).setTriggerType(TT.rand)

        MenuItem('L1_RD3_FILLED'         ).setLogic( d.RNDM3 & physcond           ).setTriggerType(TT.rand)
        MenuItem('L1_RD3_EMPTY'          ).setLogic( d.RNDM3 & cosmiccond         ).setTriggerType(TT.rand)

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
        ALFA_B1           = d.ALFA3_B7L1U | d.ALFA3_B7L1L | d.ALFA3_A7L1U | d.ALFA3_A7L1L  # 0
        ALFA_B2           = d.ALFA3_A7R1U | d.ALFA3_A7R1L | d.ALFA3_B7R1U | d.ALFA3_B7R1L # 0
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

        #AFP (ATR-23476)
        AFP_A = (d.AFP_NSA & d.AFP_FSA)
        AFP_C = (d.AFP_NSC & d.AFP_FSC)
        AFP_TOF_A = (d.AFP_FSA_TOF_T0 | d.AFP_FSA_TOF_T1 | d.AFP_FSA_TOF_T2 | d.AFP_FSA_TOF_T3)
        AFP_TOF_C = (d.AFP_FSC_TOF_T0 | d.AFP_FSC_TOF_T1 | d.AFP_FSC_TOF_T2 | d.AFP_FSC_TOF_T3)
        MenuItem('L1_AFP_NSA_BGRP0').setLogic( d.AFP_NSA & d.BGRP0)
        MenuItem('L1_AFP_NSC_BGRP0').setLogic( d.AFP_NSC & d.BGRP0)
        MenuItem('L1_AFP_FSA_BGRP0').setLogic( d.AFP_FSA & d.BGRP0)
        MenuItem('L1_AFP_FSC_BGRP0').setLogic( d.AFP_FSC & d.BGRP0)
        MenuItem('L1_AFP_FSA_TOF_T0_BGRP0').setLogic( d.AFP_FSA_TOF_T0 & d.BGRP0)
        MenuItem('L1_AFP_FSC_TOF_T0_BGRP0').setLogic( d.AFP_FSC_TOF_T0 & d.BGRP0)
        MenuItem('L1_AFP_FSA_TOF_T1_BGRP0').setLogic( d.AFP_FSA_TOF_T1 & d.BGRP0)
        MenuItem('L1_AFP_FSC_TOF_T1_BGRP0').setLogic( d.AFP_FSC_TOF_T1 & d.BGRP0)
        MenuItem('L1_AFP_FSA_TOF_T2_BGRP0').setLogic( d.AFP_FSA_TOF_T2 & d.BGRP0)
        MenuItem('L1_AFP_FSC_TOF_T2_BGRP0').setLogic( d.AFP_FSC_TOF_T2 & d.BGRP0)
        MenuItem('L1_AFP_FSA_TOF_T3_BGRP0').setLogic( d.AFP_FSA_TOF_T3 & d.BGRP0)
        MenuItem('L1_AFP_FSC_TOF_T3_BGRP0').setLogic( d.AFP_FSC_TOF_T3 & d.BGRP0)

        MenuItem('L1_AFP_A').setLogic( AFP_A & physcond)
        MenuItem('L1_AFP_C').setLogic( AFP_C & physcond)
        MenuItem('L1_AFP_A_OR_C').setLogic( (AFP_A | AFP_C) & physcond)
        MenuItem('L1_AFP_A_AND_C').setLogic( AFP_A & AFP_C & physcond)
        MenuItem('L1_AFP_A_AND_C_TOF_T0T1').setLogic( (d.AFP_FSA_TOF_T0 | d.AFP_FSA_TOF_T1) & (d.AFP_FSC_TOF_T0 | d.AFP_FSC_TOF_T1) & physcond)

        MenuItem('L1_AFP_A_OR_C_UNPAIRED_ISO').setLogic( (AFP_A | AFP_C)  & unpaired_isocond )
        MenuItem('L1_AFP_A_OR_C_UNPAIRED_NONISO').setLogic( (AFP_A | AFP_C)  & unpaired_nonisocond )
        MenuItem('L1_AFP_A_OR_C_EMPTY').setLogic( (AFP_A | AFP_C) & cosmiccond)
        MenuItem('L1_AFP_A_OR_C_FIRSTEMPTY').setLogic( (AFP_A | AFP_C) & firstempty)

        MenuItem('L1_AFP_A_AND_C_J5').setLogic( AFP_A & AFP_C & d.J5 & physcond )
        MenuItem('L1_AFP_A_OR_C_J5').setLogic( (AFP_A | AFP_C) & d.J5 & physcond )
        MenuItem('L1_AFP_A_AND_C_J12').setLogic( AFP_A & AFP_C & d.J12 & physcond )
        MenuItem('L1_AFP_A_OR_C_J12').setLogic( (AFP_A | AFP_C) & d.J12 & physcond )
        MenuItem('L1_MU5VF_AFP_A_OR_C').setLogic( (AFP_A | AFP_C) & d.MU5VF & physcond )
        MenuItem('L1_MU5VF_AFP_A_AND_C').setLogic( AFP_A & AFP_C & d.MU5VF & physcond )
        MenuItem('L1_EM7_AFP_A_OR_C').setLogic( (AFP_A | AFP_C) & d.EM7 & physcond )
        MenuItem('L1_EM7_AFP_A_AND_C').setLogic( AFP_A & AFP_C & d.EM7 & physcond )
        MenuItem('L1_eEM7_AFP_A_OR_C').setLogic( (AFP_A | AFP_C) & d.eEM7 & physcond )
        MenuItem('L1_eEM7_AFP_A_AND_C').setLogic( AFP_A & AFP_C & d.eEM7 & physcond )

        MenuItem('L1_AFP_A_AND_C_MBTS_2').setLogic( AFP_A & AFP_C & MBTS_2 & physcond )
        MenuItem('L1_AFP_A_OR_C_MBTS_2').setLogic( (AFP_A | AFP_C) & MBTS_2 & physcond )

        MenuItem('L1_AFP_A_AND_C_TOF_CEP-CjJ60').setLogic( AFP_TOF_A & AFP_TOF_C & d.TOPO_CEP_CjJ60s6 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_T0T1_CEP-CjJ60').setLogic( (d.AFP_FSA_TOF_T0 | d.AFP_FSA_TOF_T1) & (d.AFP_FSC_TOF_T0 | d.AFP_FSC_TOF_T1) & d.TOPO_CEP_CjJ60s6 & physcond )

        MenuItem('L1_AFP_A_AND_C_TOF_J20').setLogic( AFP_TOF_A & AFP_TOF_C & d.J20 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_T0T1_J20').setLogic( (d.AFP_FSA_TOF_T0 | d.AFP_FSA_TOF_T1) & (d.AFP_FSC_TOF_T0 | d.AFP_FSC_TOF_T1) & d.J20 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_J30').setLogic( AFP_TOF_A & AFP_TOF_C & d.J30 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_T0T1_J30').setLogic( (d.AFP_FSA_TOF_T0 | d.AFP_FSA_TOF_T1) & (d.AFP_FSC_TOF_T0 | d.AFP_FSC_TOF_T1) & d.J30 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_J50').setLogic( AFP_TOF_A & AFP_TOF_C & d.J50 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_T0T1_J50').setLogic( (d.AFP_FSA_TOF_T0 | d.AFP_FSA_TOF_T1) & (d.AFP_FSC_TOF_T0 | d.AFP_FSC_TOF_T1) & d.J50 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_J75').setLogic( AFP_TOF_A & AFP_TOF_C & d.J75 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_T0T1_J75').setLogic( (d.AFP_FSA_TOF_T0 | d.AFP_FSA_TOF_T1) & (d.AFP_FSC_TOF_T0 | d.AFP_FSC_TOF_T1) & d.J75 & physcond )

        MenuItem('L1_AFP_A_AND_C_TOF_jJ20').setLogic( AFP_TOF_A & AFP_TOF_C & d.jJ20 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_T0T1_jJ20').setLogic( (d.AFP_FSA_TOF_T0 | d.AFP_FSA_TOF_T1) & (d.AFP_FSC_TOF_T0 | d.AFP_FSC_TOF_T1) & d.jJ20 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_jJ30').setLogic( AFP_TOF_A & AFP_TOF_C & d.jJ30 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_T0T1_jJ30').setLogic( (d.AFP_FSA_TOF_T0 | d.AFP_FSA_TOF_T1) & (d.AFP_FSC_TOF_T0 | d.AFP_FSC_TOF_T1) & d.jJ30 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_jJ50').setLogic( AFP_TOF_A & AFP_TOF_C & d.jJ50 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_T0T1_jJ50').setLogic( (d.AFP_FSA_TOF_T0 | d.AFP_FSA_TOF_T1) & (d.AFP_FSC_TOF_T0 | d.AFP_FSC_TOF_T1) & d.jJ50 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_jJ75').setLogic( AFP_TOF_A & AFP_TOF_C & d.jJ75 & physcond )
        MenuItem('L1_AFP_A_AND_C_TOF_T0T1_jJ75').setLogic( (d.AFP_FSA_TOF_T0 | d.AFP_FSA_TOF_T1) & (d.AFP_FSC_TOF_T0 | d.AFP_FSC_TOF_T1) & d.jJ75 & physcond )

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

        MenuItem('L1_MU3V_ALFA_ANY'             ).setLogic( d.MU3V & ALFA_ANY  & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_MU3V_ALFA_ANY_UNPAIRED_ISO').setLogic( d.MU3V & ALFA_ANY  & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_MU3V_ALFA_ANY_PAIRED_UNPAIRED_ISO').setLogic( d.MU3V & ALFA_ANY  & physcond_or_unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_MU3V_ALFA_EINE'            ).setLogic( d.MU3V & ALFA_EINE & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_TE5_ALFA_ANY'             ).setLogic( d.TE5 & ALFA_ANY  & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_TE5_ALFA_ANY_UNPAIRED_ISO').setLogic( d.TE5 & ALFA_ANY  & unpaired_isocond).setTriggerType(TT.alfa)
        MenuItem('L1_TE5_ALFA_EINE'            ).setLogic( d.TE5 & ALFA_EINE & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_TRT_ALFA_ANY'             ).setLogic( d.NIMTRT & ALFA_ANY & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_TRT_ALFA_ANY_UNPAIRED_ISO').setLogic( d.NIMTRT & ALFA_ANY & unpaired_isocond).setTriggerType(TT.alfa)

        MenuItem('L1_TRT_ALFA_ANY_VETO_MBTS'           ).setLogic( d.NIMTRT & Not(MBTS_INNER) & ALFA_ANY & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_TRT_ALFA_EINE'                    ).setLogic( d.NIMTRT & ALFA_EINE & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_TRT_ALFA_EINE_VETO_MBTS'          ).setLogic( d.NIMTRT & Not(MBTS_INNER) & ALFA_EINE & physcond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_ELASTIC_VETO_MBTS'           ).setLogic( Not(MBTS_INNER) & ALFA_ELASTIC & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ELASTIC_UNPAIRED_ISO'        ).setLogic( ALFA_ELASTIC & unpaired_isocond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_ANTI_ELASTIC_VETO_MBTS'      ).setLogic( Not(MBTS_INNER) & ALFA_ANTI_ELASTIC & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ANTI_ELASTIC_UNPAIRED_ISO'   ).setLogic( ALFA_ANTI_ELASTIC & unpaired_isocond).setTriggerType(TT.alfa)

        MenuItem('L1_ALFA_ANY_VETO_MBTS'               ).setLogic( Not(MBTS_INNER) & ALFA_ANY & physcond).setTriggerType(TT.alfa)
        MenuItem('L1_ALFA_ANY_VETO_MBTS_UNPAIRED_ISO'  ).setLogic( Not(MBTS_INNER) & ALFA_ANY & unpaired_isocond).setTriggerType(TT.alfa)

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


        try:

            # Legacy primary (items passed via the merger board):
            MenuItem('L1_HT190-J15s5pETA21').setLogic( d.R2TOPO_HT190_J15s5pETA21   & physcond)
            MenuItem('L1_BPH-0M9-EM7-EM5').setLogic( d.R2TOPO_0INVM9_EM7ab_EMab & physcond)
            MenuItem('L1_BPH-0DR3-EM7J15').setLogic( d.R2TOPO_0DR03_EM7ab_CJ15ab & physcond)
            MenuItem('L1_BPH-0M9-EM7-EM5_MU5VF').setLogic( d.MU5VF & d.R2TOPO_0INVM9_EM7ab_EMab & physcond)
            MenuItem('L1_BPH-0DR3-EM7J15_MU5VF').setLogic( d.MU5VF & d.R2TOPO_0DR03_EM7ab_CJ15ab & physcond)
            MenuItem('L1_BPH-0M9-EM7-EM5_2MU3V').setLogic( d.MU3V.x(2) & d.R2TOPO_0INVM9_EM7ab_EMab & physcond)
            MenuItem('L1_BPH-0DR3-EM7J15_2MU3V').setLogic( d.MU3V.x(2) & d.R2TOPO_0DR03_EM7ab_CJ15ab & physcond)
            MenuItem("L1_JPSI-1M5-EM7" ).setLogic( d.R2TOPO_1INVM5_EM7s1_EMs6  & physcond)
            MenuItem("L1_JPSI-1M5-EM12").setLogic( d.R2TOPO_1INVM5_EM12s1_EMs6 & physcond)
            MenuItem('L1_MJJ-500-NFF').setLogic( d.R2TOPO_500INVM9999_J30s6_AJ20s6 & physcond)
            MenuItem('L1_MJJ-700').setLogic( d.R2TOPO_700INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_EM18VHI_MJJ-300').setLogic( d.EM18VHI & d.R2TOPO_300INVM9999_AJ30s6_AJ20s6 & physcond)
            MenuItem('L1_HT150-J20s5pETA31_MJJ-400-CF').setLogic( d.R2TOPO_HT150_J20s5pETA31 & d.R2TOPO_400INVM9999_AJ30s6pETA31_AJ20s6p31ETA49 & physcond)
            MenuItem('L1_30M-EM20ITAU12').setLogic( d.R2TOPO_DISAMB_30INVM_EM20his2_TAU12ab & physcond)
            MenuItem('L1_LLP-RO').setLogic( d.R2TOPO_100RATIO_0MATCH_TAU30si2_EMall & physcond)
            MenuItem('L1_LLP-NOMATCH').setLogic( d.R2TOPO_NOT_0MATCH_TAU30si1_EMall & physcond)
            MenuItem('L1_DPHI-2EM3').setLogic( d.R2TOPO_27DPHI32_EMs1_EMs6 & physcond)
            MenuItem('L1_SC111-CJ15').setLogic(  d.R2TOPO_SC111_CJ15abpETA26 & physcond)
            MenuItem('L1_J50_DETA20-J50J').setLogic( d.J50 & d.R2TOPO_0DETA20_J50s1_Js2 & physcond)
            MenuItem('L1_DR-TAU20ITAU12I-J25').setLogic( d.R2TOPO_1DISAMB_J25ab_0DR28_TAU20abi_TAU12abi & physcond)
            MenuItem('L1_TAU60_DR-TAU20ITAU12I' ).setLogic( d.HA60 & d.R2TOPO_0DR28_TAU20abi_TAU12abi & physcond)
            MenuItem('L1_DR-TAU20ITAU12I' ).setLogic( d.R2TOPO_0DR28_TAU20abi_TAU12abi & physcond)
            MenuItem('L1_LAR-ZEE').setLogic( d.R2TOPO_ZEE_EM20shi2 & physcond).setTriggerType( TT.lardemo ) # LAr demo (ATR-11897, ATR-23403)

            # Phase-1:
            MenuItem('L1_LAR-ZEE-eEM').setLogic( d.TOPO_ZEE_eEM20sm2 & physcond).setTriggerType( TT.lardemo ) # LAr demo (ATR-23403)
            MenuItem('L1_LATE-MU8F_jXE30').setLogic( d.TOPO_LATE_MU10s1 & d.jXE30 & physcond)
            MenuItem('L1_LATE-MU8F_jJ50' ).setLogic( d.TOPO_LATE_MU10s1 & d.jJ50 & physcond)
            MenuItem('L1_LFV-MU8VF').setLogic( d.TOPO_0DR15_2MU5VFab & d.MU8VF & d.MU5VF.x(2) & physcond)
            MenuItem('L1_LFV-MU5VF' ).setLogic( d.TOPO_0DR15_2MU5VFab & d.MU5VF.x(2) & physcond)
            MenuItem('L1_LFV-eEM8L-MU8VF' ).setLogic( d.TOPO_0INVM10_0DR15_eEM8abl_MU8Fab & d.MU8VF & physcond)
            MenuItem('L1_LFV-eEM12L-MU5VF' ).setLogic( d.TOPO_0INVM10_0DR15_eEM12abl_MU5VFab & physcond)

            #BLS
            #ATR-19720
            MenuItem('L1_BPH-2M9-0DR15-2MU3V'         ).setLogic( d.TOPO_2INVM9_0DR15_2MU3Vab & physcond)
            MenuItem('L1_BPH-2M9-0DR15-MU5VFMU3V'     ).setLogic( d.TOPO_2INVM9_0DR15_MU5VFab_MU3Vab & physcond)
            MenuItem('L1_BPH-2M9-2DR15-2MU5VF'        ).setLogic( d.TOPO_2INVM9_2DR15_2MU5VFab & physcond)
            MenuItem('L1_BPH-8M15-0DR22-MU5VFMU3V-BO' ).setLogic( d.TOPO_8INVM15_0DR22_CMU5VFab_CMU3Vab & physcond)
            MenuItem('L1_BPH-8M15-0DR22-2MU5VF'       ).setLogic( d.TOPO_8INVM15_0DR22_2MU5VFab & physcond)
            #ATR-19355
            MenuItem('L1_BPH-0M10-3MU3V'              ).setLogic( d.TOPO_0INVM10_3MU3Vab & physcond)
            #ATR-19638
            MenuItem('L1_BPH-0M10C-3MU3V'             ).setLogic( d.TOPO_0INVM10C_3MU3Vab & physcond)
            #ATR-19639
            MenuItem('L1_BPH-2M9-0DR15-C-MU5VFMU3V'   ).setLogic( d.TOPO_2INVM9_0DR15_C_MU5VFab_MU3Vab & physcond)        
            # ATR-21566
            MenuItem('L1_BPH-0DR12-2MU3V'      ).setLogic( d.TOPO_0DR12_2MU3Vab & physcond)
            MenuItem('L1_BPH-0DR12C-2MU3V'     ).setLogic( d.TOPO_0DR12C_2MU3Vab & physcond)             
            MenuItem('L1_BPH-7M22-0DR20-2MU3V' ).setLogic( d.TOPO_7INVM22_0DR20_2MU3Vab & physcond)
            MenuItem('L1_BPH-7M22-0DR20C-2MU3V').setLogic( d.TOPO_7INVM22_0DR20C_2MU3Vab & physcond)
            MenuItem('L1_BPH-8M22-2MU3V').setLogic( d.TOPO_8INVM22_2MU3Vab & physcond)
            MenuItem('L1_BPH-7M22-0DR12-MU5VFMU3V' ).setLogic( d.TOPO_7INVM22_0DR12_MU5VFab_MU3Vab & physcond)
            MenuItem('L1_BPH-7M22-0DR20-MU5VFMU3V' ).setLogic( d.TOPO_7INVM22_0DR20_MU5VFab_MU3Vab & physcond)
            #ATR-22782
            MenuItem('L1_BPH-0M16-20DR99-2MU3V'       ).setLogic( d.TOPO_0INVM16_20DR99_2MU3Vab & physcond)
            MenuItem('L1_BPH-0M16-15DR99-2MU3V'       ).setLogic( d.TOPO_0INVM16_15DR99_2MU3Vab & physcond)
            MenuItem('L1_BPH-8M15-20DR99-2MU3V'       ).setLogic( d.TOPO_8INVM15_20DR99_2MU3Vab & physcond)
            MenuItem('L1_BPH-8M15-15DR99-2MU3V'       ).setLogic( d.TOPO_8INVM15_15DR99_2MU3Vab & physcond)
            MenuItem('L1_BPH-8M15-20DR99-C-2MU3V'     ).setLogic( d.TOPO_8INVM15_20DR99_C_2MU3Vab & physcond)
            MenuItem('L1_BPH-7M11-25DR99-2MU3V'       ).setLogic( d.TOPO_7INVM11_25DR99_2MU3Vab & physcond)
            MenuItem('L1_BPH-7M14-0DR25-MU5VFMU3V'    ).setLogic( d.TOPO_7INVM14_0DR25_MU5VFab_MU3Vab & physcond)

            # ATR-19510, SM Low-mass DY
            MenuItem('L1_DY-BOX-2MU3V' ).setLogic( d.TOPO_5DETA99_5DPHI99_2MU3Vab & physcond)
            MenuItem('L1_DY-BOX-MU5VFMU3V').setLogic( d.TOPO_5DETA99_5DPHI99_MU5VFab_MU3Vab & physcond)
            MenuItem('L1_DY-BOX-2MU5VF').setLogic( d.TOPO_5DETA99_5DPHI99_2MU5VFab & physcond)

            #ATR-17320
            MenuItem('L1_CEP-CjJ60').setLogic( d.TOPO_CEP_CjJ60s6 & physcond )
            MenuItem('L1_CEP-CjJ50').setLogic( d.TOPO_CEP_CjJ50s6 & physcond )
            #ATR-18824
            MenuItem('L1_ZAFB-04DPHI-eEM15M' ).setLogic( d.TOPO_60INVM_04DPHI32_eEM15abm_FjJj15s623ETA49 & physcond)
            MenuItem('L1_ZAFB-25DPHI-eEM15M' ).setLogic( d.TOPO_60INVM_25DPHI32_eEM15abm_FjJj15s623ETA49 & physcond)
            #ATR-19302:
            MenuItem('L1_DPHI-M70-2eEM10M' ).setLogic( d.TOPO_0INVM70_27DPHI32_eEM10sm1_eEM10sm6 & physcond)
            MenuItem('L1_DPHI-M70-2eEM12M' ).setLogic( d.TOPO_0INVM70_27DPHI32_eEM12sm1_eEM12sm6 & physcond)
            #ATR-21637
            MenuItem('L1_DPHI-M70-2eEM7' ).setLogic( d.TOPO_0INVM70_27DPHI32_eEM7s1_eEM7s6 & physcond)
            MenuItem('L1_DPHI-M70-2eEM7L' ).setLogic( d.TOPO_0INVM70_27DPHI32_eEM7sl1_eEM7sl6 & physcond)
            #ATR-19376
            MenuItem('L1_10DR-MU14FCH-MU5VF'             ).setLogic( d.TOPO_10DR99_2MU5VFab & d.MU14FCH & physcond)
            MenuItem('L1_10DR-MU14FCH-MU5VF_EMPTY'       ).setLogic( d.TOPO_10DR99_2MU5VFab & d.MU14FCH & cosmiccond)
            MenuItem('L1_10DR-MU14FCH-MU5VF_UNPAIRED_ISO').setLogic( d.TOPO_10DR99_2MU5VFab & d.MU14FCH & unpaired_isocond)

            #Missing: KF

            # subset of legacy chains migrated phase1 boards
            MenuItem('L1_BPH-0M9-eEM7-eEM5').setLogic( d.TOPO_0INVM9_eEM7ab_eEMab & physcond)
            MenuItem('L1_BPH-0M9-eEM7-eEM5_MU5VF').setLogic( d.MU5VF & d.TOPO_0INVM9_eEM7ab_eEMab & physcond)
            MenuItem('L1_BPH-0M9-eEM7-eEM5_2MU3V').setLogic( d.MU3V.x(2) & d.TOPO_0INVM9_eEM7ab_eEMab & physcond)
            MenuItem('L1_BPH-0DR3-eEM7jJ15').setLogic( d.TOPO_0DR03_eEM7ab_CjJ15ab & physcond)
            MenuItem('L1_BPH-0DR3-eEM7jJ15_MU5VF').setLogic(  d.MU5VF & d.TOPO_0DR03_eEM7ab_CjJ15ab & physcond)
            MenuItem('L1_BPH-0DR3-eEM7jJ15_2MU3V').setLogic( d.MU3V.x(2) & d.TOPO_0DR03_eEM7ab_CjJ15ab & physcond)

            MenuItem("L1_JPSI-1M5-eEM7" ).setLogic( d.TOPO_1INVM5_eEM7s1_eEMs6  & physcond)
            MenuItem("L1_JPSI-1M5-eEM12").setLogic( d.TOPO_1INVM5_eEM12s1_eEMs6 & physcond)

            MenuItem('L1_LLP-RO-eEM').setLogic( d.TOPO_100RATIO_0MATCH_eTAU30si2_eEMall & physcond)
            MenuItem('L1_LLP-NOMATCH-eEM').setLogic( d.TOPO_NOT_0MATCH_eTAU30si1_eEMall & physcond)

            MenuItem('L1_DR25-eTAU20eTAU12').setLogic( d.TOPO_0DR25_eTAU20ab_eTAU12ab & physcond)
            MenuItem('L1_DR25-eTAU20eTAU12-jJ25').setLogic( d.TOPO_2DISAMB_jJ25ab_0DR25_eTAU20ab_eTAU12ab & physcond)
            MenuItem('L1_DR-eTAU20eTAU12').setLogic( d.TOPO_0DR28_eTAU20ab_eTAU12ab & physcond)
            MenuItem('L1_DR-eTAU20eTAU12-jJ25').setLogic( d.TOPO_2DISAMB_jJ25ab_0DR28_eTAU20ab_eTAU12ab & physcond)
            MenuItem('L1_DR-eTAU20MeTAU12M').setLogic( d.TOPO_0DR28_eTAU20abm_eTAU12abm & physcond)
            MenuItem('L1_DR-eTAU20MeTAU12M-jJ25').setLogic( d.TOPO_2DISAMB_jJ25ab_0DR28_eTAU20abm_eTAU12abm & physcond)
            MenuItem('L1_cTAU20M_2cTAU12M_DR-eTAU20MeTAU12M').setLogic( d.cTAU20M & d.cTAU12M.x(2) & d.TOPO_0DR28_eTAU20abm_eTAU12abm & physcond)
            MenuItem('L1_cTAU20M_2cTAU12M_DR-eTAU20MeTAU12M-jJ25').setLogic( d.cTAU20M & d.cTAU12M.x(2) & d.TOPO_2DISAMB_jJ25ab_0DR28_eTAU20abm_eTAU12abm & physcond)
            MenuItem('L1_eTAU60_2cTAU12M_DR-eTAU20eTAU12').setLogic( d.eTAU60 & d.cTAU12M.x(2) & d.TOPO_0DR28_eTAU20ab_eTAU12ab & physcond)

            MenuItem('L1_jMJJ-400-NFF-0DPHI20').setLogic( d.TOPO_400INVM_0DPHI20_jJ30s6_AjJ20s6 & physcond)
            MenuItem('L1_jMJJ-400-NFF-0DPHI22').setLogic( d.TOPO_400INVM_0DPHI22_jJ30s6_AjJ20s6 & physcond)
            MenuItem('L1_jMJJ-400-NFF-0DPHI24').setLogic( d.TOPO_400INVM_0DPHI24_jJ30s6_AjJ20s6 & physcond)
            MenuItem('L1_jMJJ-400-NFF-0DPHI26').setLogic( d.TOPO_400INVM_0DPHI26_jJ30s6_AjJ20s6 & physcond)
            MenuItem('L1_jMJJ-300-NFF').setLogic( d.TOPO_300INVM_jJ30s6_AjJ20s6 & physcond)
            MenuItem('L1_jMJJ-400-NFF').setLogic( d.TOPO_400INVM_jJ30s6_AjJ20s6 & physcond)
            MenuItem('L1_jMJJ-500-NFF').setLogic( d.TOPO_500INVM_jJ30s6_AjJ20s6 & physcond)
            MenuItem('L1_jMJJ-700-NFF').setLogic( d.TOPO_700INVM_jJ30s6_AjJ20s6 & physcond)
            MenuItem('L1_MU5VF_jMJJ-300-NFF').setLogic( d.MU5VF & d.TOPO_300INVM_jJ30s6_AjJ20s6 & physcond)
            MenuItem('L1_eEM18M_jMJJ-300-NFF').setLogic( d.eEM18M & d.TOPO_300INVM_jJ30s6_AjJ20s6 & physcond)
            MenuItem('L1_jHT150-jJ20s5pETA31_jMJJ-400-CF').setLogic( d.TOPO_HT150_jJ20s5pETA31   & d.TOPO_400INVM_AjJ30s6pETA31_AjJ20s6p31ETA49 & physcond)
            MenuItem('L1_jMJJ-400-CF').setLogic( d.TOPO_400INVM_AjJ30s6pETA31_AjJ20s6p31ETA49 & physcond)
            MenuItem('L1_jJ50_DETA20-jJ50J').setLogic( d.jJ50 & d.TOPO_0DETA20_jJ50s1_jJs2 & physcond)
            MenuItem('L1_HT190-jJ15s5pETA21').setLogic( d.TOPO_HT190_jJ15s5pETA21   & physcond)
            MenuItem('L1_SC111-CjJ15').setLogic(  d.TOPO_SC111_CjJ15abpETA26 & physcond)

            # Needed?
            MenuItem('L1_DPHI-2eEM3').setLogic( d.TOPO_27DPHI32_eEMs1_eEMs6 & physcond)
            MenuItem('L1_DPHI-2eEM3_VTE5p24ETA49').setLogic( d.TOPO_27DPHI32_eEMs1_eEMs6 & Not(d.TE524ETA49) & physcond).setTriggerType(TT.calo)
            MenuItem('L1_DPHI-2eEM3_VTE5p24ETA49_ALFA_EINE').setLogic( d.TOPO_27DPHI32_eEMs1_eEMs6 & Not(d.TE524ETA49) & ALFA_EINE & physcond).setTriggerType(TT.alfa)
            MenuItem('L1_DPHI-2eEM3_VTE10').setLogic( d.TOPO_27DPHI32_eEMs1_eEMs6 & Not(d.TE10) & physcond).setTriggerType(TT.calo)
            MenuItem('L1_DPHI-2eEM7_VTE50').setLogic( d.eEM7.x(2) & d.TOPO_27DPHI32_eEMs1_eEMs6 & Not(d.TE50) & physcond).setTriggerType(TT.calo)
            MenuItem('L1_BTAG-MU3VjJ15').setLogic( d.TOPO_0DR04_MU3Vab_CjJ15ab & physcond)
            MenuItem('L1_BTAG-MU5VFjJ20').setLogic( d.TOPO_0DR04_MU5VFab_CjJ20ab & physcond)
            MenuItem('L1_BPH-8M15-2MU3V-BO'    ).setLogic( d.TOPO_8INVM15_2CMU3Vab & physcond)           # 96% for Upsi

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
            log.error( "Creation of L1Topo item failed, will abort!: %s" , ex)
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
